// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <os>
#include <timers>
#include <net/inet4>

// prevent default serial out
void default_stdout_handlers() {}
#include <hw/serial.hpp>

using Connection_ptr = net::tcp::Connection_ptr;
#include <kernel/irq_manager.hpp>
#include <kernel/context.hpp>
extern "C" void* get_cpu_esp();

// a pre-allocated pool of stacks we can use for blocking calls
#define CONTEXT_STACK_SIZE   8192
#include "context_pool.hpp"
ContextPool<CONTEXT_STACK_SIZE> pool(10);

void blocking_write(Connection_ptr conn, const char* data, size_t len)
{
  bool written = false;
  conn->write(data, len,
  [&written] (bool) { written = true; });
  // sometimes we can just write and forget
  if (written) return;
  // do the blocking wait in another context
  auto cpv = pool.get();
  Context::jump(cpv.start_address(),
  [conn, &written] {
    while (!written) {
      OS::halt();
      IRQ_manager::get().process_interrupts();
    }
  });
}

Connection_ptr blocking_connect(net::Inet4& inet, net::IP4::addr addr, uint16_t port)
{
  auto outgoing = inet.tcp().connect({addr, port});
  // do the blocking wait in another context
  auto cpv = pool.get();
  Context::jump(cpv.start_address(),
  [outgoing, port] {
    printf("[%p] connecting to 10.0.0.1:%u...\n", get_cpu_esp(), port);
    // wait for connection state to change
    while (not (outgoing->is_connected() || outgoing->is_closing() || outgoing->is_closed())) {
      OS::halt();
      IRQ_manager::get().process_interrupts();
    }
  });
  printf("Stack estimated used %d bytes\n", cpv.estimate_used());
  // return connection whether good or bad
  if (outgoing->is_connected())
      return outgoing;
  else
      return nullptr;
}

void blocking_close(Connection_ptr socket)
{
  // close connection
  socket->close();
  // do the blocking wait in another context
  auto cpv = pool.get();
  Context::jump(cpv.start_address(),
  [socket] {
    // wait for connection to close
    while (!socket->is_closed()) {
      OS::halt();
      IRQ_manager::get().process_interrupts();
    }
  });
}

void recursive_connect(net::Inet4& inet, uint16_t port)
{
  int retries = 3;
  while (retries--)
  {
    auto socket = blocking_connect(inet, {10,0,0,1}, port);
    if (socket) {
        const std::string Long(200, '+');
        blocking_write(socket, Long.data(), Long.size());
        blocking_close(socket);

        // connect to next port
        recursive_connect(inet, port+1);
        break;
    }
  }
  printf("Too many retries\n");
}

void recursive_context(int n)
{
  auto cpv = pool.get();
  Context::jump(cpv.start_address(),
  [&n, &cpv] {
    // make sure we good
    assert(cpv.valid_stack());
    //printf("[%p] In context %d...\n", get_cpu_esp(), n);
    //printf("[%d] Pool size: %u  capacity: %u\n", n, pool.size(), pool.capacity());
    if (n < 1000)
      recursive_context(n + 1);
    // make sure we still good
    assert(cpv.valid_stack());
    //printf("[%d] Pool size: %u  capacity: %u\n", n, pool.size(), pool.capacity());
    //printf("[%p] Leaving context %d\n", get_cpu_esp(), n);
  });
}

#include <sys/mman.h>

void Service::start(const std::string&)
{
  // add own serial out after service start
  auto& com1 = hw::Serial::port<1>();
  OS::add_stdout(com1.get_print_handler());
  // show that we are starting :)
  printf("*** POSIX Service starting up...\n");

  printf("bool: %u\n", sizeof(bool));

  recursive_context(1);
  printf("Pool size: %u  capacity: %u\n", pool.size(), pool.capacity());

  // default configuration (with DHCP)
  auto& inet = net::Inet4::ifconfig<>(10);
  inet.network_config(
      {  10, 0,  0, 42 },  // IP
      { 255,255,255, 0 },  // Netmask
      {  10, 0,  0,  1 },  // Gateway
      {  10, 0,  0,  1 }); // DNS

  recursive_connect(inet, 1234);

  // create our own working memory ...
  Context::create(4096,
  [] {
    // and run same event loop as OS
    while (OS::is_running()) {
      OS::halt();
      IRQ_manager::get().process_interrupts();
    }
  });
}

void print_heap_info()
{
  static intptr_t last = 0;
  // show information on heap status, to discover leaks etc.
  extern uintptr_t heap_begin;
  extern uintptr_t heap_end;
  intptr_t heap_size = heap_end - heap_begin;
  last = heap_size - last;
  printf("Heap begin  %#x  size %u Kb\n",     heap_begin, heap_size / 1024);
  printf("Heap end    %#x  diff %u (%d Kb)\n", heap_end,  last, last / 1024);
  last = heap_size;
}

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
Connection_ptr blocking_connect(net::Inet4& inet, net::IP4::addr addr, uint16_t port)
{
  auto outgoing = inet.tcp().connect({addr, port});
  // wait for connection state to change
  while (!outgoing->is_connected() && !outgoing->is_closing()) {
    OS::halt();
    IRQ_manager::get().process_interrupts();
  }
  // return connection whether good or bad
  if (outgoing->is_connected())
      return outgoing;
  else
      return nullptr;
}

extern "C" void* get_cpu_esp();
void recursive_connect(net::Inet4& inet, uint16_t port)
{
  printf("[%p] connecting on port %u...\n", get_cpu_esp(), port);

  auto socket = blocking_connect(inet, {10,0,0,1}, port);
  if (socket) {
      printf("connected\n");
      assert(socket);
      socket->write("test\n");
      bool not_yet_closed = true;
      // close connection
      socket->close();
      socket->on_disconnect(
      [&not_yet_closed] (auto, auto) {
        not_yet_closed = false;
      });
      // wait for connection to close
      while (not_yet_closed) {
        OS::halt();
        IRQ_manager::get().process_interrupts();
      }
      // connect again
      recursive_connect(inet, port+1);
  }
  else {
      printf("we are closed and socket is null\n");
      assert(!socket);
  }
  OS::shutdown();
}

void Service::start(const std::string&)
{
  // add own serial out after service start
  auto& com1 = hw::Serial::port<1>();
  OS::add_stdout(com1.get_print_handler());
  // show that we are starting :)
  printf("*** IRC Service starting up...\n");

  // default configuration (with DHCP)
  auto& inet = net::Inet4::ifconfig<>(10);
  inet.network_config(
      {  10, 0,  0, 42 },  // IP
      { 255,255,255, 0 },  // Netmask
      {  10, 0,  0,  1 },  // Gateway
      {  10, 0,  0,  1 }); // DNS

  recursive_connect(inet, 1234);
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
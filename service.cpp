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

// a pre-allocated pool of stacks we can use for blocking calls
#define CONTEXT_STACK_SIZE   8192
#include "context_pool.hpp"
ContextPool<CONTEXT_STACK_SIZE> pool(10);
#include <kernel/context.hpp>

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

extern "C" void socket_test();
extern "C" void sqlite_test();

void Service::start(const std::string&)
{
  // add own serial out after service start
  auto& com1 = hw::Serial::port<1>();
  OS::add_stdout(com1.get_print_handler());
  // show that we are starting :)
  printf("*** POSIX Service starting up...\n");

  // default configuration (with DHCP)
  auto& inet = net::Inet4::ifconfig<>(10);
  inet.network_config(
      {  10, 0,  0, 42 },  // IP
      { 255,255,255, 0 },  // Netmask
      {  10, 0,  0,  1 },  // Gateway
      {  10, 0,  0,  1 }); // DNS

  sqlite_test();
}

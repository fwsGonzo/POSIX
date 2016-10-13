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

#include <sys/socket.h>
#include <net/inet4>
using namespace net;
using Connection_ptr = tcp::Connection_ptr;

#include <kernel/irq_manager.hpp>
#include <kernel/context.hpp>

// return the "currently selected" networking stack
static net::Inet4& net_stack() {
  return Inet4::stack<> ();
}

void __blocking_write(Connection_ptr conn, const char* data, size_t len)
{
  bool written = false;
  conn->write(data, len,
  [&written] (bool) { written = true; });
  // sometimes we can just write and forget
  if (written) return;
  while (!written) {
    OS::halt();
    IRQ_manager::get().process_interrupts();
  }
}

struct TCP_FD;
void __blocking_read(TCP_FD&, char* data, size_t len)
{
  /// retrieve X bytes from existing TCP_FD
  /// return how many bytes we read
  /// if we didnt read len bytes, then block until we can
  /// if the connection is closing or closed, return 0
}

Connection_ptr __blocking_connect(net::IP4::addr addr, uint16_t port)
{
  auto outgoing = net_stack().tcp().connect({addr, port});
  printf("[*] connecting to 10.0.0.1:%u...\n", port);
  // wait for connection state to change
  while (not (outgoing->is_connected() || outgoing->is_closing() || outgoing->is_closed())) {
    OS::halt();
    IRQ_manager::get().process_interrupts();
  }
  // return connection whether good or bad
  if (outgoing->is_connected())
      return outgoing;
  else
      return nullptr;
}

void __blocking_close(Connection_ptr socket)
{
  // close connection
  socket->close();
  // wait for connection to close
  while (!socket->is_closed()) {
    OS::halt();
    IRQ_manager::get().process_interrupts();
  }
}

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

extern "C"
int dlopen(...) {
  return -1;
}
extern "C"
int dlerror(...) {
  return -1;
}
extern "C"
int dlsym(...) {
  return -1;
}
extern "C"
int dlclose(...) {
  return -1;
}
extern "C"
long sysconf(int) {
  return -1;
}
extern "C"
uid_t geteuid() {
  return 0;
}
extern "C"
ssize_t readlink(const char *path, char *buf, size_t bufsiz)
{
  printf("readlink %s (buf=%u)\n", bufsiz);
  return 0;
}
int utime(const char *filename, const struct utimbuf *times)
{
  return -1;
}
int utimes(const char *filename, const struct timeval times[2])
{
  return -1;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

#include "sqlite/sqlite3.h"
void Service::start(const std::string&)
{
  // add own serial out after service start
  auto& com1 = hw::Serial::port<1>();
  OS::add_stdout(com1.get_print_handler());
  // show that we are starting :)
  printf("*** POSIX Service starting up...\n");

  //
  sqlite3* db;
  int rc = sqlite3_open(":memory:", &db);
  printf("sqlite opened: %d\n", rc);
  assert(rc == 0);

  /* Create SQL statement */
  const char*
  sql = "CREATE TABLE COMPANY("  \
        "ID INT PRIMARY KEY     NOT NULL," \
        "NAME           TEXT    NOT NULL," \
        "AGE            INT     NOT NULL," \
        "ADDRESS        CHAR(50)," \
        "SALARY         REAL );";

  /* Execute SQL statement */
  char* zErrMsg = 0;
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "Table created successfully\n");
  }
  rc = sqlite3_close(db);
  printf("sqlite closed: %d\n", rc);
  return;

  // default configuration (with DHCP)
  auto& inet = net::Inet4::ifconfig<>(10);
  inet.network_config(
      {  10, 0,  0, 42 },  // IP
      { 255,255,255, 0 },  // Netmask
      {  10, 0,  0,  1 },  // Gateway
      {  10, 0,  0,  1 }); // DNS

  socket_test();
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

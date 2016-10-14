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

#include "sqlite/sqlite3.h"
#include <stdio.h>
#include <assert.h>

int dlopen(const char* name, ...) {
  (void) name;
  return -1;
}
int dlerror(int d) {
  (void) d;
  return -1;
}
int dlsym(int d) {
  (void) d;
  return -1;
}
int dlclose(int d) {
  (void) d;
  return -1;
}
long sysconf(int d) {
  (void) d;
  return -1;
}
uid_t geteuid() {
  return 0;
}
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

void sqlite_test()
{
  //
  sqlite3* db;
  int rc = sqlite3_open(":memory:", &db);
  printf("sqlite opened: %d\n", rc);
  assert(rc == 0);

  /* Create SQL statement */
  const char*
  sql = "CREATE TABLE COMPANY("
        "ID INT PRIMARY KEY     NOT NULL,"
        "NAME           TEXT    NOT NULL,"
        "AGE            INT     NOT NULL,"
        "ADDRESS        CHAR(50),"
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
}

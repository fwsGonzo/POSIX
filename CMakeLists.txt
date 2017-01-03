cmake_minimum_required(VERSION 2.8.9)
project(service)

# default IncludeOS location #FIXME#
if (NOT DEFINED ENV{INCLUDEOS_PREFIX})
  set(ENV{INCLUDEOS_PREFIX} /usr/local)
endif()

set(SERVICE_NAME "POSIX test")
set(BINARY       "ptest")
set(MAX_MEM 32)
set(SOURCES
    service.cpp socket_test.c sqlite_test.c
    sqlite/sqlite3.c
  )
set(LOCAL_INCLUDES ".")
add_definitions(-DSQLITE_THREADSAFE=0)
add_definitions(-DSQLITE_OMIT_LOAD_EXTENSION)

# include service build script
include($ENV{INCLUDEOS_PREFIX}/includeos/service.cmake)

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

#pragma once
#ifndef POSIX_SYS_TYPES_H
#define POSIX_SYS_TYPES_H

#include "common.h"
#include <stdint.h>

POSIX_BEGIN_DECLS

typedef int blkcnt_t;
typedef int blksize_t;

typedef int64_t clock_t;
typedef uint8_t clockid_t;

typedef uint32_t dev_t;

typedef int fsblkcnt_t;
typedef int fsfilcnt_t;

typedef uint32_t gid_t;

typedef uint32_t id_t;

typedef uint64_t ino_t;

typedef int mode_t;

typedef int nlink_t;

typedef int off_t;

typedef uint32_t pid_t;

typedef int   size_t;
typedef short ssize_t;

typedef int64_t suseconds_t;

typedef int64_t  time_t;
typedef uint32_t timer_t;

typedef uint32_t uid_t;


POSIX_END_DECLS

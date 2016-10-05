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
#include <vector>
#include <delegate>

template <int>
struct CPV;

template <int StackSize>
class ContextPool
{
public:
  inline CPV<StackSize> get();
  inline void free(void*);

  size_t size() const noexcept {
    return stacks.size();
  }
  size_t capacity() const noexcept {
    return stacks.capacity();
  }

  ContextPool(const int N) {
    stacks.reserve(N);
    for (int i = 0; i < N; i++)
        stacks.emplace_back(new char[StackSize]);
  }
  ~ContextPool() {
    for (void* stack : stacks)
        delete[] (char*) stack;
  }

private:
  std::vector<void*> stacks;
};

extern "C" void panic(const char*) __attribute__((noreturn));
extern "C" void* get_cpu_esp();

/// ContextPool value
template <int StackSize>
struct CPV
{
  CPV(void* v, ContextPool<StackSize>& p) noexcept
    : value(v), pool(p) {}
  ~CPV() {
    pool.free(value);
  }

  void* start_address() const noexcept {
    // return end-aligned to 16 bytes
    return (void*) (((uintptr_t)value + StackSize) & ~0xF);
  }

  inline bool
  valid_stack() const noexcept {
    auto esp   = (uintptr_t) get_cpu_esp();
    auto start = (uintptr_t) start_address();
    auto end   = (uintptr_t) value;
    return esp < start && esp > end;
  }

private:
  void* value;
  ContextPool<StackSize>& pool;
};

///
template <int StackSize>
inline CPV<StackSize> ContextPool<StackSize>::get()
{
  void* stack = nullptr;
  if (stacks.empty()) {
    stack = new char[StackSize];
    assert(stack);
  } else {
    stack = stacks.back();
    stacks.pop_back();
  }
  return {stack, *this};
}

template <int StackSize>
void ContextPool<StackSize>::free(void* ptr)
{
  stacks.push_back(ptr);
}

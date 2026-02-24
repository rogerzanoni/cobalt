// Copyright 2015 The Cobalt Authors. All Rights Reserved.
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

// TODO: Figure out how to gracefully use symbolize given where it has
// been placed in the source tree. What follows is an ungraceful use.

#include "starboard/system.h"

#include <dlfcn.h>
#include <elf.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>

#include "starboard/elf_loader/evergreen_info.h"
#include "third_party/symbolize/symbolize.h"

bool SbSystemSymbolize(const void* address, char* out_buffer, int buffer_size) {
  if (address == nullptr || *reinterpret_cast<const uintptr_t*>(address) == 0)
    return false;
  // I believe this casting-away const in the implementation is better than the
  // alternative of removing const-ness from the address parameter.
  if (google::Symbolize(const_cast<void*>(address), out_buffer, buffer_size))
    return true;

  // If symbolization failed and the address is in the Evergreen library,
  // try to resolve it from the in-memory ELF symbol table.
  EvergreenInfo info;
  if (GetEvergreenInfo(&info)) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(address);
    if (addr >= info.base_address &&
        (addr - info.base_address) < info.load_size) {
      // Fall back to offset-only output for offline symbolization.
      uintptr_t offset = addr - info.base_address;
      snprintf(out_buffer, buffer_size, "evergreen+0x%x (base=0x%x)",
               static_cast<unsigned int>(offset),
               static_cast<unsigned int>(info.base_address));
      return true;
    }
  }

  // Fall back to dladdr() for offline symbolization of native library frames.
  Dl_info dl_info;
  if (dladdr(address, &dl_info) && dl_info.dli_fbase) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(address);
    uintptr_t base = reinterpret_cast<uintptr_t>(dl_info.dli_fbase);
    uintptr_t offset = addr - base;
    const char* fname = dl_info.dli_fname ? dl_info.dli_fname : "<unknown>";
    // Use only the basename to keep the output concise.
    const char* slash = strrchr(fname, '/');
    if (slash) fname = slash + 1;
    snprintf(out_buffer, buffer_size, "%s+0x%x (base=0x%x)", fname,
             static_cast<unsigned int>(offset),
             static_cast<unsigned int>(base));
    return true;
  }

  return false;
}

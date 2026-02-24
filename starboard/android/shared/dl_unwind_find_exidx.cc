// Copyright 2026 The Cobalt Authors. All Rights Reserved.
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

// Override dl_unwind_find_exidx to make the ARM unwinder aware of the
// Evergreen-loaded library in anonymous memory. Without this, _Unwind_Backtrace
// cannot unwind through the ELF-loaded code because the system unwinder cannot
// find the .ARM.exidx section for anonymous mappings.

#include <dlfcn.h>
#include <elf.h>
#include <link.h>
#include <unwind.h>

#include "starboard/elf_loader/evergreen_info.h"

extern "C" _Unwind_Ptr dl_unwind_find_exidx(_Unwind_Ptr pc, int* pcount) {
  EvergreenInfo info;
  if (GetEvergreenInfo(&info)) {
    uintptr_t addr = static_cast<uintptr_t>(pc);
    if (addr >= info.base_address &&
        (addr - info.base_address) < info.load_size) {
      // PC is in the Evergreen-loaded library. Scan its program headers
      // for PT_ARM_EXIDX.
      const Elf32_Phdr* phdr =
          reinterpret_cast<const Elf32_Phdr*>(info.phdr_table);
      for (size_t i = 0; i < info.phdr_table_num; ++i) {
        if (phdr[i].p_type == PT_ARM_EXIDX) {
          *pcount = static_cast<int>(phdr[i].p_memsz / sizeof(_Unwind_Ptr) / 2);
          return static_cast<_Unwind_Ptr>(info.base_address + phdr[i].p_vaddr);
        }
      }
    }
  }

  // Fall back to the original implementation for system libraries.
  typedef _Unwind_Ptr (*orig_fn)(_Unwind_Ptr, int*);
  static orig_fn original =
      reinterpret_cast<orig_fn>(dlsym(RTLD_NEXT, "dl_unwind_find_exidx"));
  if (original) {
    return original(pc, pcount);
  }

  *pcount = 0;
  return 0;
}

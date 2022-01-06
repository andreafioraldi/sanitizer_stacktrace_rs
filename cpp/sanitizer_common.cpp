//===-- sanitizer_common.cpp ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is shared between AddressSanitizer and ThreadSanitizer
// run-time libraries.
//===----------------------------------------------------------------------===//

// !!! This file contains only the relevant parts of the original sanitizer_common.cpp
#include "sanitizer_common.h"
#include <cstdlib>
#include <cstdio>

namespace __sanitizer {

uptr PageSizeCached;

const char *StripModuleName(const char *module) {
  if (!module)
    return nullptr;
  if (SANITIZER_WINDOWS) {
    // On Windows, both slash and backslash are possible.
    // Pick the one that goes last.
    if (const char *bslash_pos = internal_strrchr(module, '\\'))
      return StripModuleName(bslash_pos + 1);
  }
  if (const char *slash_pos = internal_strrchr(module, '/')) {
    return slash_pos + 1;
  }
  return module;
}

void CheckFailed(const char *file, int line, const char *cond,
                          u64 v1, u64 v2) {
  fprintf(stderr, "CHECK failed: %s:%d \"%s\" (0x%zx, 0x%zx)\n",
         StripModuleName(file), line, cond, (uptr)v1,
         (uptr)v2);
  abort();
}

} // namespace __sanitizer

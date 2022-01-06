//===-- sanitizer_mac.cpp -------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is shared between various sanitizers' runtime libraries and
// implements OSX-specific functions.
//===----------------------------------------------------------------------===//

// !!! This file contains only the relevant parts of the original sanitizer_mac.cpp
#include "sanitizer_platform.h"
#if SANITIZER_MAC

#include "sanitizer_common.h"

#include <unistd.h>

namespace __sanitizer {

uptr GetPageSize() {
  return sysconf(_SC_PAGESIZE);
}

}  // namespace __sanitizer

#endif  // SANITIZER_MAC

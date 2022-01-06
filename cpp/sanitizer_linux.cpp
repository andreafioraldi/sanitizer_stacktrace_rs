//===-- sanitizer_linux.cpp -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is shared between AddressSanitizer and ThreadSanitizer
// run-time libraries and implements linux-specific functions from
// sanitizer_libc.h.
//===----------------------------------------------------------------------===//

#include "sanitizer_platform.h"

#if SANITIZER_FREEBSD || SANITIZER_LINUX || SANITIZER_NETBSD || \
    SANITIZER_SOLARIS

#include "sanitizer_common.h"
#include <unistd.h>

#if SANITIZER_FREEBSD || SANITIZER_NETBSD
#include <sys/sysctl.h>
#define internal_sysctl sysctl
#endif

namespace __sanitizer {

#if !SANITIZER_ANDROID
uptr GetPageSize() {
#if SANITIZER_LINUX && (defined(__x86_64__) || defined(__i386__)) && \
    defined(EXEC_PAGESIZE)
  return EXEC_PAGESIZE;
#elif SANITIZER_FREEBSD || SANITIZER_NETBSD
// Use sysctl as sysconf can trigger interceptors internally.
  int pz = 0;
  uptr pzl = sizeof(pz);
  int mib[2] = {CTL_HW, HW_PAGESIZE};
  int rv = internal_sysctl(mib, 2, &pz, &pzl, nullptr, 0);
  CHECK_EQ(rv, 0);
  return (uptr)pz;
#elif SANITIZER_USE_GETAUXVAL
  return getauxval(AT_PAGESZ);
#else
  return sysconf(_SC_PAGESIZE);  // EXEC_PAGESIZE may not be trustworthy.
#endif
}
#endif // !SANITIZER_ANDROID

} // namespace __sanitizer

#endif

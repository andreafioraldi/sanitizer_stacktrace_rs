//===-- sanitizer_stacktrace_libcdep.cpp ----------------------------------===//
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

#include "sanitizer_common.h"
#include "sanitizer_stacktrace.h"

namespace __sanitizer {

void BufferedStackTrace::Unwind(u32 max_depth, uptr pc, uptr bp, void *context,
                                uptr stack_top, uptr stack_bottom,
                                bool request_fast_unwind) {
  // Ensures all call sites get what they requested.
  CHECK_EQ(request_fast_unwind, WillUseFastUnwind(request_fast_unwind));
  top_frame_bp = (max_depth > 0) ? bp : 0;
  // Avoid doing any work for small max_depth.
  if (max_depth == 0) {
    size = 0;
    return;
  }
  if (max_depth == 1) {
    size = 1;
    trace_buffer[0] = pc;
    return;
  }
  if (!WillUseFastUnwind(request_fast_unwind)) {
#if SANITIZER_CAN_SLOW_UNWIND
    if (context)
      UnwindSlow(pc, context, max_depth);
    else
      UnwindSlow(pc, max_depth);
    // If there are too few frames, the program may be built with
    // -fno-asynchronous-unwind-tables. Fall back to fast unwinder below.
    if (size > 2 || size >= max_depth)
      return;
#else
    UNREACHABLE("slow unwind requested but not available");
#endif
  }
  UnwindFast(pc, bp, stack_top, stack_bottom, max_depth);
}

}  // namespace __sanitizer

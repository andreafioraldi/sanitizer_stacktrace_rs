//===-- sanitizer_common.h --------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is shared between run-time libraries of sanitizers.
//
// It declares common functions and classes that are used in both runtimes.
// Implementation of some functions are provided in sanitizer_common, while
// others must be defined by run-time library itself.
//===----------------------------------------------------------------------===//

// !!! This file contains only the relevant parts of the original sanitizer_common.h
#ifndef SANITIZER_COMMON_H
#define SANITIZER_COMMON_H

#include "sanitizer_internal_defs.h"
#include "sanitizer_platform.h"

#include <cstring> // mem*
#define internal_memcpy memcpy
#define internal_memset memset
#define internal_strrchr strrchr

#include <vector>
#define InternalMmapVector std::vector

namespace __sanitizer {

void RawWrite(const char *buffer);

#if SANITIZER_ANDROID
inline uptr GetPageSize() {
// Android post-M sysconf(_SC_PAGESIZE) crashes if called from .preinit_array.
  return 4096;
}
inline uptr GetPageSizeCached() {
  return 4096;
}
#else
uptr GetPageSize();
extern uptr PageSizeCached;
inline uptr GetPageSizeCached() {
  if (!PageSizeCached)
    PageSizeCached = GetPageSize();
  return PageSizeCached;
}
#endif

// Don't use std::min, std::max or std::swap, to minimize dependency
// on libstdc++.
template <class T>
constexpr T Min(T a, T b) {
  return a < b ? a : b;
}
template <class T>
constexpr T Max(T a, T b) {
  return a > b ? a : b;
}
template <class T>
constexpr T Abs(T a) {
  return a < 0 ? -a : a;
}
template<class T> void Swap(T& a, T& b) {
  T tmp = a;
  a = b;
  b = tmp;
}

inline uptr MostSignificantSetBitIndex(uptr x) {
  CHECK_NE(x, 0U);
  unsigned long up;
#if !SANITIZER_WINDOWS || defined(__clang__) || defined(__GNUC__)
# ifdef _WIN64
  up = SANITIZER_WORDSIZE - 1 - __builtin_clzll(x);
# else
  up = SANITIZER_WORDSIZE - 1 - __builtin_clzl(x);
# endif
#elif defined(_WIN64)
  _BitScanReverse64(&up, x);
#else
  _BitScanReverse(&up, x);
#endif
  return up;
}

inline uptr LeastSignificantSetBitIndex(uptr x) {
  CHECK_NE(x, 0U);
  unsigned long up;
#if !SANITIZER_WINDOWS || defined(__clang__) || defined(__GNUC__)
# ifdef _WIN64
  up = __builtin_ctzll(x);
# else
  up = __builtin_ctzl(x);
# endif
#elif defined(_WIN64)
  _BitScanForward64(&up, x);
#else
  _BitScanForward(&up, x);
#endif
  return up;
}

inline constexpr bool IsPowerOfTwo(uptr x) { return (x & (x - 1)) == 0; }

inline uptr RoundUpToPowerOfTwo(uptr size) {
  CHECK(size);
  if (IsPowerOfTwo(size)) return size;

  uptr up = MostSignificantSetBitIndex(size);
  CHECK_LT(size, (1ULL << (up + 1)));
  CHECK_GT(size, (1ULL << up));
  return 1ULL << (up + 1);
}

inline constexpr uptr RoundUpTo(uptr size, uptr boundary) {
  RAW_CHECK(IsPowerOfTwo(boundary));
  return (size + boundary - 1) & ~(boundary - 1);
}

inline constexpr uptr RoundDownTo(uptr x, uptr boundary) {
  return x & ~(boundary - 1);
}

inline constexpr bool IsAligned(uptr a, uptr alignment) {
  return (a & (alignment - 1)) == 0;
}

class InternalScopedString {
 public:
  InternalScopedString() : buffer_(1) { buffer_[0] = '\0'; }

  uptr length() const { return buffer_.size() - 1; }
  void clear() {
    buffer_.resize(1);
    buffer_[0] = '\0';
  }
  void append(const char *format, ...) FORMAT(2, 3);
  const char *data() const { return buffer_.data(); }
  char *data() { return buffer_.data(); }

 private:
  InternalMmapVector<char> buffer_;
};

}  // namespace __sanitizer

#endif  // SANITIZER_COMMON_H

// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MEMORY_RAW_SPAN_H_
#define BASE_MEMORY_RAW_SPAN_H_

#include "base/containers/span.h"
#include "base/memory/raw_ptr.h"

namespace base {

template <typename T, RawPtrTraits Traits = RawPtrTraits::kEmpty>
using raw_span = span<T, dynamic_extent>;

template <typename T>
auto ExtractAsDanglingSpan(raw_span<T>& arg) {
  raw_span<T> result = std::exchange(arg, raw_span<T>());
  return result;
}

}  // namespace base

#endif  // BASE_MEMORY_RAW_SPAN_H_

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

#ifndef STARBOARD_SHARED_STARBOARD_FEATURE_DEFAULTS_H_
#define STARBOARD_SHARED_STARBOARD_FEATURE_DEFAULTS_H_

namespace starboard::features {

// Seeds the Starboard FeatureList singleton with the default state of every
// feature/param declared in starboard/extension/feature_config.h. Must be
// called once before any code (e.g. the Evergreen inner library's static
// initializers) queries a Starboard feature.
void InitializeStarboardFeatureListWithDefaults();

}  // namespace starboard::features

#endif  // STARBOARD_SHARED_STARBOARD_FEATURE_DEFAULTS_H_

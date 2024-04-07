/*
 * Copyright 2022 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CPP_SCHEME_SCHEME_H_
#define CPP_SCHEME_SCHEME_H_

#include "../palettes/core.h"
#include "../utils/utils.h"

// This file is automatically generated. Do not modify it.

namespace material_color_utilities {

struct Scheme {
  Argb primary = 0;
  Argb on_primary = 0;
  Argb primary_container = 0;
  Argb on_primary_container = 0;
  Argb secondary = 0;
  Argb on_secondary = 0;
  Argb secondary_container = 0;
  Argb on_secondary_container = 0;
  Argb tertiary = 0;
  Argb on_tertiary = 0;
  Argb tertiary_container = 0;
  Argb on_tertiary_container = 0;
  Argb error = 0;
  Argb on_error = 0;
  Argb error_container = 0;
  Argb on_error_container = 0;
  Argb background = 0;
  Argb on_background = 0;
  Argb surface = 0;
  Argb on_surface = 0;
  Argb surface_variant = 0;
  Argb on_surface_variant = 0;
  Argb outline = 0;
  Argb outline_variant = 0;
  Argb shadow = 0;
  Argb scrim = 0;
  Argb inverse_surface = 0;
  Argb inverse_on_surface = 0;
  Argb inverse_primary = 0;
};

/**
 * Returns the light material color scheme based on the given core palette.
 */
Scheme MaterialLightColorSchemeFromPalette(CorePalette palette);

/**
 * Returns the dark material color scheme based on the given core palette.
 */
Scheme MaterialDarkColorSchemeFromPalette(CorePalette palette);

/**
 * Returns the light material color scheme based on the given color,
 * in ARGB format.
 */
Scheme MaterialLightColorScheme(Argb color);

/**
 * Returns the dark material color scheme based on the given color,
 * in ARGB format.
 */
Scheme MaterialDarkColorScheme(Argb color);

/**
 * Returns the light material content color scheme based on the given color,
 * in ARGB format.
 */
Scheme MaterialLightContentColorScheme(Argb color);

/**
 * Returns the dark material content color scheme based on the given color,
 * in ARGB format.
 */
Scheme MaterialDarkContentColorScheme(Argb color);

}  // namespace material_color_utilities

#endif  // CPP_SCHEME_SCHEME_H_

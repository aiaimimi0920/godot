/**************************************************************************/
/*  color_scheme.h                                                 */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef COLOR_ROLE_H
#define COLOR_ROLE_H
#include "color_scheme.h"
#include "core/math/color.h"

// enum class ColorRole {
// 	STATIC_COLOR,
// 	PRIMARY_PALETTE_KEY,
// 	SECONDARY_PALETTE_KEY,
// 	TERTIARY_PALETTE_KEY,
// 	NEUTRAL_PALETTE_KEY,
// 	NEUTRAL_VARIANT_PALETTE_KEY,
// 	STATIC_TRANSPARENT,
// 	STATIC_ONE,
// 	STATIC_ONE_40,
// 	BACKGROUND,
// 	BACKGROUND_08,
// 	BACKGROUND_10,
// 	BACKGROUND_12,
// 	BACKGROUND_16,
// 	BACKGROUND_38,
// 	ON_BACKGROUND,
// 	ON_BACKGROUND_08,
// 	ON_BACKGROUND_10,
// 	ON_BACKGROUND_12,
// 	ON_BACKGROUND_16,
// 	ON_BACKGROUND_38,
// 	SURFACE,
// 	SURFACE_08,
// 	SURFACE_10,
// 	SURFACE_12,
// 	SURFACE_16,
// 	SURFACE_38,
// 	SURFACE_DIM,
// 	SURFACE_DIM_08,
// 	SURFACE_DIM_10,
// 	SURFACE_DIM_12,
// 	SURFACE_DIM_16,
// 	SURFACE_DIM_38,
// 	SURFACE_BRIGHT,
// 	SURFACE_BRIGHT_08,
// 	SURFACE_BRIGHT_10,
// 	SURFACE_BRIGHT_12,
// 	SURFACE_BRIGHT_16,
// 	SURFACE_BRIGHT_38,
// 	SURFACE_CONTAINER_LOWEST,
// 	SURFACE_CONTAINER_LOWEST_08,
// 	SURFACE_CONTAINER_LOWEST_10,
// 	SURFACE_CONTAINER_LOWEST_12,
// 	SURFACE_CONTAINER_LOWEST_16,
// 	SURFACE_CONTAINER_LOWEST_38,
// 	SURFACE_CONTAINER_LOW,
// 	SURFACE_CONTAINER_LOW_08,
// 	SURFACE_CONTAINER_LOW_10,
// 	SURFACE_CONTAINER_LOW_12,
// 	SURFACE_CONTAINER_LOW_16,
// 	SURFACE_CONTAINER_LOW_38,
// 	SURFACE_CONTAINER,
// 	SURFACE_CONTAINER_08,
// 	SURFACE_CONTAINER_10,
// 	SURFACE_CONTAINER_12,
// 	SURFACE_CONTAINER_16,
// 	SURFACE_CONTAINER_38,
// 	SURFACE_CONTAINER_HIGH,
// 	SURFACE_CONTAINER_HIGH_08,
// 	SURFACE_CONTAINER_HIGH_10,
// 	SURFACE_CONTAINER_HIGH_12,
// 	SURFACE_CONTAINER_HIGH_16,
// 	SURFACE_CONTAINER_HIGH_38,
// 	SURFACE_CONTAINER_HIGHEST,
// 	SURFACE_CONTAINER_HIGHEST_08,
// 	SURFACE_CONTAINER_HIGHEST_10,
// 	SURFACE_CONTAINER_HIGHEST_12,
// 	SURFACE_CONTAINER_HIGHEST_16,
// 	SURFACE_CONTAINER_HIGHEST_38,
// 	ON_SURFACE,
// 	ON_SURFACE_08,
// 	ON_SURFACE_10,
// 	ON_SURFACE_12,
// 	ON_SURFACE_16,
// 	ON_SURFACE_38,
// 	ON_SURFACE_60,
// 	ON_SURFACE_65,
// 	SURFACE_VARIANT,
// 	SURFACE_VARIANT_08,
// 	SURFACE_VARIANT_10,
// 	SURFACE_VARIANT_12,
// 	SURFACE_VARIANT_16,
// 	SURFACE_VARIANT_38,
// 	ON_SURFACE_VARIANT,
// 	ON_SURFACE_VARIANT_08,
// 	ON_SURFACE_VARIANT_10,
// 	ON_SURFACE_VARIANT_12,
// 	ON_SURFACE_VARIANT_16,
// 	ON_SURFACE_VARIANT_38,
// 	INVERSE_SURFACE,
// 	INVERSE_SURFACE_08,
// 	INVERSE_SURFACE_10,
// 	INVERSE_SURFACE_12,
// 	INVERSE_SURFACE_16,
// 	INVERSE_SURFACE_38,
// 	INVERSE_ON_SURFACE,
// 	INVERSE_ON_SURFACE_08,
// 	INVERSE_ON_SURFACE_10,
// 	INVERSE_ON_SURFACE_12,
// 	INVERSE_ON_SURFACE_16,
// 	INVERSE_ON_SURFACE_38,
// 	OUTLINE,
// 	OUTLINE_08,
// 	OUTLINE_10,
// 	OUTLINE_12,
// 	OUTLINE_16,
// 	OUTLINE_38,
// 	OUTLINE_VARIANT,
// 	OUTLINE_VARIANT_08,
// 	OUTLINE_VARIANT_10,
// 	OUTLINE_VARIANT_12,
// 	OUTLINE_VARIANT_16,
// 	OUTLINE_VARIANT_38,
// 	SHADOW,
// 	SHADOW_08,
// 	SHADOW_10,
// 	SHADOW_12,
// 	SHADOW_16,
// 	SHADOW_38,
// 	SCRIM,
// 	SCRIM_08,
// 	SCRIM_10,
// 	SCRIM_12,
// 	SCRIM_16,
// 	SCRIM_38,
// 	SURFACE_TINT,
// 	SURFACE_TINT_08,
// 	SURFACE_TINT_10,
// 	SURFACE_TINT_12,
// 	SURFACE_TINT_16,
// 	SURFACE_TINT_38,
// 	PRIMARY,
// 	PRIMARY_08,
// 	PRIMARY_10,
// 	PRIMARY_12,
// 	PRIMARY_16,
// 	PRIMARY_38,
// 	ON_PRIMARY,
// 	ON_PRIMARY_08,
// 	ON_PRIMARY_10,
// 	ON_PRIMARY_12,
// 	ON_PRIMARY_16,
// 	ON_PRIMARY_38,
// 	PRIMARY_CONTAINER,
// 	PRIMARY_CONTAINER_08,
// 	PRIMARY_CONTAINER_10,
// 	PRIMARY_CONTAINER_12,
// 	PRIMARY_CONTAINER_16,
// 	PRIMARY_CONTAINER_38,
// 	ON_PRIMARY_CONTAINER,
// 	ON_PRIMARY_CONTAINER_08,
// 	ON_PRIMARY_CONTAINER_10,
// 	ON_PRIMARY_CONTAINER_12,
// 	ON_PRIMARY_CONTAINER_16,
// 	ON_PRIMARY_CONTAINER_38,
// 	INVERSE_PRIMARY,
// 	INVERSE_PRIMARY_08,
// 	INVERSE_PRIMARY_10,
// 	INVERSE_PRIMARY_12,
// 	INVERSE_PRIMARY_16,
// 	INVERSE_PRIMARY_38,
// 	INVERSE_PRIMARY_60,
// 	SECONDARY,
// 	SECONDARY_08,
// 	SECONDARY_10,
// 	SECONDARY_12,
// 	SECONDARY_16,
// 	SECONDARY_38,
// 	ON_SECONDARY,
// 	ON_SECONDARY_08,
// 	ON_SECONDARY_10,
// 	ON_SECONDARY_12,
// 	ON_SECONDARY_16,
// 	ON_SECONDARY_38,
// 	SECONDARY_CONTAINER,
// 	SECONDARY_CONTAINER_08,
// 	SECONDARY_CONTAINER_10,
// 	SECONDARY_CONTAINER_12,
// 	SECONDARY_CONTAINER_16,
// 	SECONDARY_CONTAINER_38,
// 	ON_SECONDARY_CONTAINER,
// 	ON_SECONDARY_CONTAINER_08,
// 	ON_SECONDARY_CONTAINER_10,
// 	ON_SECONDARY_CONTAINER_12,
// 	ON_SECONDARY_CONTAINER_16,
// 	ON_SECONDARY_CONTAINER_38,
// 	TERTIARY,
// 	TERTIARY_08,
// 	TERTIARY_10,
// 	TERTIARY_12,
// 	TERTIARY_16,
// 	TERTIARY_38,
// 	ON_TERTIARY,
// 	ON_TERTIARY_08,
// 	ON_TERTIARY_10,
// 	ON_TERTIARY_12,
// 	ON_TERTIARY_16,
// 	ON_TERTIARY_38,
// 	TERTIARY_CONTAINER,
// 	TERTIARY_CONTAINER_08,
// 	TERTIARY_CONTAINER_10,
// 	TERTIARY_CONTAINER_12,
// 	TERTIARY_CONTAINER_16,
// 	TERTIARY_CONTAINER_38,
// 	ON_TERTIARY_CONTAINER,
// 	ON_TERTIARY_CONTAINER_08,
// 	ON_TERTIARY_CONTAINER_10,
// 	ON_TERTIARY_CONTAINER_12,
// 	ON_TERTIARY_CONTAINER_16,
// 	ON_TERTIARY_CONTAINER_38,
// 	ERROR,
// 	ERROR_08,
// 	ERROR_10,
// 	ERROR_12,
// 	ERROR_16,
// 	ERROR_38,
// 	ON_ERROR,
// 	ON_ERROR_08,
// 	ON_ERROR_10,
// 	ON_ERROR_12,
// 	ON_ERROR_16,
// 	ON_ERROR_38,
// 	ERROR_CONTAINER,
// 	ERROR_CONTAINER_08,
// 	ERROR_CONTAINER_10,
// 	ERROR_CONTAINER_12,
// 	ERROR_CONTAINER_16,
// 	ERROR_CONTAINER_38,
// 	ON_ERROR_CONTAINER,
// 	ON_ERROR_CONTAINER_08,
// 	ON_ERROR_CONTAINER_10,
// 	ON_ERROR_CONTAINER_12,
// 	ON_ERROR_CONTAINER_16,
// 	ON_ERROR_CONTAINER_38,
// 	PRIMARY_FIXED,
// 	PRIMARY_FIXED_08,
// 	PRIMARY_FIXED_10,
// 	PRIMARY_FIXED_12,
// 	PRIMARY_FIXED_16,
// 	PRIMARY_FIXED_38,
// 	PRIMARY_FIXED_DIM,
// 	PRIMARY_FIXED_DIM_08,
// 	PRIMARY_FIXED_DIM_10,
// 	PRIMARY_FIXED_DIM_12,
// 	PRIMARY_FIXED_DIM_16,
// 	PRIMARY_FIXED_DIM_38,
// 	ON_PRIMARY_FIXED,
// 	ON_PRIMARY_FIXED_08,
// 	ON_PRIMARY_FIXED_10,
// 	ON_PRIMARY_FIXED_12,
// 	ON_PRIMARY_FIXED_16,
// 	ON_PRIMARY_FIXED_38,
// 	ON_PRIMARY_FIXED_VARIANT,
// 	ON_PRIMARY_FIXED_VARIANT_08,
// 	ON_PRIMARY_FIXED_VARIANT_10,
// 	ON_PRIMARY_FIXED_VARIANT_12,
// 	ON_PRIMARY_FIXED_VARIANT_16,
// 	ON_PRIMARY_FIXED_VARIANT_38,
// 	SECONDARY_FIXED,
// 	SECONDARY_FIXED_08,
// 	SECONDARY_FIXED_10,
// 	SECONDARY_FIXED_12,
// 	SECONDARY_FIXED_16,
// 	SECONDARY_FIXED_38,
// 	SECONDARY_FIXED_DIM,
// 	SECONDARY_FIXED_DIM_08,
// 	SECONDARY_FIXED_DIM_10,
// 	SECONDARY_FIXED_DIM_12,
// 	SECONDARY_FIXED_DIM_16,
// 	SECONDARY_FIXED_DIM_38,
// 	ON_SECONDARY_FIXED,
// 	ON_SECONDARY_FIXED_08,
// 	ON_SECONDARY_FIXED_10,
// 	ON_SECONDARY_FIXED_12,
// 	ON_SECONDARY_FIXED_16,
// 	ON_SECONDARY_FIXED_38,
// 	ON_SECONDARY_FIXED_VARIANT,
// 	ON_SECONDARY_FIXED_VARIANT_08,
// 	ON_SECONDARY_FIXED_VARIANT_10,
// 	ON_SECONDARY_FIXED_VARIANT_12,
// 	ON_SECONDARY_FIXED_VARIANT_16,
// 	ON_SECONDARY_FIXED_VARIANT_38,
// 	TERTIARY_FIXED,
// 	TERTIARY_FIXED_08,
// 	TERTIARY_FIXED_10,
// 	TERTIARY_FIXED_12,
// 	TERTIARY_FIXED_16,
// 	TERTIARY_FIXED_38,
// 	TERTIARY_FIXED_DIM,
// 	TERTIARY_FIXED_DIM_08,
// 	TERTIARY_FIXED_DIM_10,
// 	TERTIARY_FIXED_DIM_12,
// 	TERTIARY_FIXED_DIM_16,
// 	TERTIARY_FIXED_DIM_38,
// 	ON_TERTIARY_FIXED,
// 	ON_TERTIARY_FIXED_08,
// 	ON_TERTIARY_FIXED_10,
// 	ON_TERTIARY_FIXED_12,
// 	ON_TERTIARY_FIXED_16,
// 	ON_TERTIARY_FIXED_38,
// 	ON_TERTIARY_FIXED_VARIANT,
// 	ON_TERTIARY_FIXED_VARIANT_08,
// 	ON_TERTIARY_FIXED_VARIANT_10,
// 	ON_TERTIARY_FIXED_VARIANT_12,
// 	ON_TERTIARY_FIXED_VARIANT_16,
// 	ON_TERTIARY_FIXED_VARIANT_38,
// };

// static const String color_role_hint = "Static Color, Primary Palette Key, Secondary Palette Key, Tertiary Palette Key, Neutral Palette Key, Neutral Variant Palette Key, Static Transparent, Static One, Static One 40, Background, Background 08, Background 10, Background 12, Background 16, Background 38, On Background, On Background 08, On Background 10, On Background 12, On Background 16, On Background 38, Surface, Surface 08, Surface 10, Surface 12, Surface 16, Surface 38, Surface Dim, Surface Dim 08, Surface Dim 10, Surface Dim 12, Surface Dim 16, Surface Dim 38, Surface Bright, Surface Bright 08, Surface Bright 10, Surface Bright 12, Surface Bright 16, Surface Bright 38, Surface Container Lowest, Surface Container Lowest 08, Surface Container Lowest 10, Surface Container Lowest 12, Surface Container Lowest 16, Surface Container Lowest 38, Surface Container Low, Surface Container Low 08, Surface Container Low 10, Surface Container Low 12, Surface Container Low 16, Surface Container Low 38, Surface Container, Surface Container 08, Surface Container 10, Surface Container 12, Surface Container 16, Surface Container 38, Surface Container High, Surface Container High 08, Surface Container High 10, Surface Container High 12, Surface Container High 16, Surface Container High 38, Surface Container Highest, Surface Container Highest 08, Surface Container Highest 10, Surface Container Highest 12, Surface Container Highest 16, Surface Container Highest 38, On Surface, On Surface 08, On Surface 10, On Surface 12, On Surface 16, On Surface 38, On Surface 60, On Surface 65, Surface Variant, Surface Variant 08, Surface Variant 10, Surface Variant 12, Surface Variant 16, Surface Variant 38, On Surface Variant, On Surface Variant 08, On Surface Variant 10, On Surface Variant 12, On Surface Variant 16, On Surface Variant 38, Inverse Surface, Inverse Surface 08, Inverse Surface 10, Inverse Surface 12, Inverse Surface 16, Inverse Surface 38, Inverse On Surface, Inverse On Surface 08, Inverse On Surface 10, Inverse On Surface 12, Inverse On Surface 16, Inverse On Surface 38, Outline, Outline 08, Outline 10, Outline 12, Outline 16, Outline 38, Outline Variant, Outline Variant 08, Outline Variant 10, Outline Variant 12, Outline Variant 16, Outline Variant 38, Shadow, Shadow 08, Shadow 10, Shadow 12, Shadow 16, Shadow 38, Scrim, Scrim 08, Scrim 10, Scrim 12, Scrim 16, Scrim 38, Surface Tint, Surface Tint 08, Surface Tint 10, Surface Tint 12, Surface Tint 16, Surface Tint 38, Primary, Primary 08, Primary 10, Primary 12, Primary 16, Primary 38, On Primary, On Primary 08, On Primary 10, On Primary 12, On Primary 16, On Primary 38, Primary Container, Primary Container 08, Primary Container 10, Primary Container 12, Primary Container 16, Primary Container 38, On Primary Container, On Primary Container 08, On Primary Container 10, On Primary Container 12, On Primary Container 16, On Primary Container 38, Inverse Primary, Inverse Primary 08, Inverse Primary 10, Inverse Primary 12, Inverse Primary 16, Inverse Primary 38, Inverse Primary 60, Secondary, Secondary 08, Secondary 10, Secondary 12, Secondary 16, Secondary 38, On Secondary, On Secondary 08, On Secondary 10, On Secondary 12, On Secondary 16, On Secondary 38, Secondary Container, Secondary Container 08, Secondary Container 10, Secondary Container 12, Secondary Container 16, Secondary Container 38, On Secondary Container, On Secondary Container 08, On Secondary Container 10, On Secondary Container 12, On Secondary Container 16, On Secondary Container 38, Tertiary, Tertiary 08, Tertiary 10, Tertiary 12, Tertiary 16, Tertiary 38, On Tertiary, On Tertiary 08, On Tertiary 10, On Tertiary 12, On Tertiary 16, On Tertiary 38, Tertiary Container, Tertiary Container 08, Tertiary Container 10, Tertiary Container 12, Tertiary Container 16, Tertiary Container 38, On Tertiary Container, On Tertiary Container 08, On Tertiary Container 10, On Tertiary Container 12, On Tertiary Container 16, On Tertiary Container 38, Error, Error 08, Error 10, Error 12, Error 16, Error 38, On Error, On Error 08, On Error 10, On Error 12, On Error 16, On Error 38, Error Container,Error Container 08,Error Container 10,Error Container 12,Error Container 16,Error Container 38,On Error Container,On Error Container 08,On Error Container 10,On Error Container 12,On Error Container 16,On Error Container 38,Primary Fixed,Primary Fixed 08,Primary Fixed 10,Primary Fixed 12,Primary Fixed 16,Primary Fixed 38,Primary Fixed Dim,Primary Fixed Dim 08,Primary Fixed Dim 10,Primary Fixed Dim 12,Primary Fixed Dim 16,Primary Fixed Dim 38,On Primary Fixed,On Primary Fixed 08,On Primary Fixed 10,On Primary Fixed 12,On Primary Fixed 16,On Primary Fixed 38,On Primary Fixed Variant,On Primary Fixed Variant 08,On Primary Fixed Variant 10,On Primary Fixed Variant 12,On Primary Fixed Variant 16,On Primary Fixed Variant 38,Secondary Fixed,Secondary Fixed 08,Secondary Fixed 10,Secondary Fixed 12,Secondary Fixed 16,Secondary Fixed 38,Secondary Fixed Dim,Secondary Fixed Dim 08,Secondary Fixed Dim 10,Secondary Fixed Dim 12,Secondary Fixed Dim 16,Secondary Fixed Dim 38,On Secondary Fixed,On Secondary Fixed 08,On Secondary Fixed 10,On Secondary Fixed 12,On Secondary Fixed 16,On Secondary Fixed 38,On Secondary Fixed Variant,On Secondary Fixed Variant 08,On Secondary Fixed Variant 10,On Secondary Fixed Variant 12,On Secondary Fixed Variant 16,On Secondary Fixed Variant 38,Tertiary Fixed,Tertiary Fixed 08,Tertiary Fixed 10,Tertiary Fixed 12,Tertiary Fixed 16,Tertiary Fixed 38,Tertiary Fixed Dim,Tertiary Fixed Dim 08,Tertiary Fixed Dim 10,Tertiary Fixed Dim 12,Tertiary Fixed Dim 16,Tertiary Fixed Dim 38,On Tertiary Fixed,On Tertiary Fixed 08,On Tertiary Fixed 10,On Tertiary Fixed 12,On Tertiary Fixed 16,On Tertiary Fixed 38,On Tertiary Fixed Variant,On Tertiary Fixed Variant 08,On Tertiary Fixed Variant 10,On Tertiary Fixed Variant 12,On Tertiary Fixed Variant 16,On Tertiary Fixed Variant 38";

enum class ColorRoleEnum{
	STATIC_COLOR,
	NO_LERP,
	PRIMARY_PALETTE_KEY,
	SECONDARY_PALETTE_KEY,
	TERTIARY_PALETTE_KEY,
	NEUTRAL_PALETTE_KEY,
	NEUTRAL_VARIANT_PALETTE_KEY,
	STATIC_TRANSPARENT,
	STATIC_ONE,
	BACKGROUND,
	ON_BACKGROUND,
	SURFACE,
	SURFACE_DIM,
	SURFACE_BRIGHT,
	SURFACE_CONTAINER_LOWEST,
	SURFACE_CONTAINER_LOW,
	SURFACE_CONTAINER,
	SURFACE_CONTAINER_HIGH,
	SURFACE_CONTAINER_HIGHEST,
	ON_SURFACE,
	SURFACE_VARIANT,
	ON_SURFACE_VARIANT,
	INVERSE_SURFACE,
	INVERSE_ON_SURFACE,
	OUTLINE,
	OUTLINE_VARIANT,
	SHADOW,
	SCRIM,
	SURFACE_TINT,
	PRIMARY,
	ON_PRIMARY,
	PRIMARY_CONTAINER,
	ON_PRIMARY_CONTAINER,
	INVERSE_PRIMARY,
	SECONDARY,
	ON_SECONDARY,
	SECONDARY_CONTAINER,
	ON_SECONDARY_CONTAINER,
	TERTIARY,
	ON_TERTIARY,
	TERTIARY_CONTAINER,
	ON_TERTIARY_CONTAINER,
	ERROR,
	ON_ERROR,
	ERROR_CONTAINER,
	ON_ERROR_CONTAINER,
	PRIMARY_FIXED,
	PRIMARY_FIXED_DIM,
	ON_PRIMARY_FIXED,
	ON_PRIMARY_FIXED_VARIANT,
	SECONDARY_FIXED,
	SECONDARY_FIXED_DIM,
	ON_SECONDARY_FIXED,
	ON_SECONDARY_FIXED_VARIANT,
	TERTIARY_FIXED,
	TERTIARY_FIXED_DIM,
	ON_TERTIARY_FIXED,
	ON_TERTIARY_FIXED_VARIANT,
};

static const String color_role_enum_hint = "Static Color, No Lerp, Primary Palette Key, Secondary Palette Key, Tertiary Palette Key, Neutral Palette Key, Neutral Variant Palette Key, Static Transparent, Static One, Background, On Background, Surface, Surface Dim, Surface Bright, Surface Container Lowest, Surface Container Low, Surface Container, Surface Container High, Surface Container Highest, On Surface, Surface Variant, On Surface Variant, Inverse Surface, Inverse On Surface, Outline, Outline Variant, Shadow, Scrim, Surface Tint, Primary, On Primary, Primary Container, On Primary Container, Inverse Primary, Secondary, On Secondary, Secondary Container, On Secondary Container, Tertiary, On Tertiary, Tertiary Container, On Tertiary Container, Error, On Error, Error Container, On Error Container, Primary Fixed, Primary Fixed Dim, On Primary Fixed, On Primary Fixed Variant, Secondary Fixed, Secondary Fixed Dim, On Secondary Fixed, On Secondary Fixed Variant, Tertiary Fixed, Tertiary Fixed Dim, On Tertiary Fixed, On Tertiary Fixed Variant";

struct _NO_DISCARD_ ColorRole {
	struct {
		ColorRoleEnum color_role_enum = ColorRoleEnum::STATIC_ONE;
		Color color_scale = Color(1, 1, 1, 1);

		bool inverted = false;

		float darkened_amount = 0.0;
		float lightened_amount = 0.0;

		float lerp_weight = 0.0;
		ColorRole *lerp_color_role;
		Color lerp_color = Color(1, 1, 1, 1);
	};

	ColorRole() {}

	ColorRole(ColorRoleEnum p_color_role_enum, Color &p_color_scale = Color(1, 1, 1, 1), bool p_inverted = false, float p_darkened_amount = 0.0, float p_lightened_amount = 0.0, float p_lerp_weight = 0.0, ColorRole *p_lerp_color_role = nullptr, Color &p_lerp_color = Color(1, 1, 1, 1)) {
		color_role_enum = p_color_role_enum;
		color_scale = p_color_scale;
		inverted = p_inverted;
		darkened_amount = p_darkened_amount;
		lightened_amount = p_lightened_amount;
		lerp_weight = p_lerp_weight;
		lerp_color_role = p_lerp_color_role;
		lerp_color = p_lerp_color;
	}

	Color get_color(const Ref<ColorScheme> &p_color_scheme) {
		Color base_color = p_color_scheme->get_color(color_role_enum);
		base_color *= color_scale;

		if (inverted) {
			base_color = base_color.inverted();
		}

		if (darkened_amount != 0.0) {
			base_color = base_color.darkened(darkened_amount);
		}
		if (lightened_amount != 0.0) {
			base_color = base_color.lightened(lightened_amount);
		}
		if (lerp_weight != 0.0) {
			if (lerp_color_role != nullptr) {
				lerp_color = lerp_color_role->get_color(p_color_scheme);
			}
			base_color = base_color.lerp(lerp_color, lerp_weight);
		}
		return base_color;
	}
};


#endif // COLOR_ROLE_H

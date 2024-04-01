/**************************************************************************/
/*  color_role.h                                                 		  */
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

#include "core/io/resource.h"
#include "core/math/color.h"

class ColorScheme;

static const String color_role_enum_hint = "Static Color, No Lerp, Primary Palette Key, Secondary Palette Key, Tertiary Palette Key, Neutral Palette Key, Neutral Variant Palette Key, Static Transparent, Static One, Background, On Background, Surface, Surface Dim, Surface Bright, Surface Container Lowest, Surface Container Low, Surface Container, Surface Container High, Surface Container Highest, On Surface, Surface Variant, On Surface Variant, Inverse Surface, Inverse On Surface, Outline, Outline Variant, Shadow, Scrim, Surface Tint, Primary, On Primary, Primary Container, On Primary Container, Inverse Primary, Secondary, On Secondary, Secondary Container, On Secondary Container, Tertiary, On Tertiary, Tertiary Container, On Tertiary Container, Error, On Error, Error Container, On Error Container, Primary Fixed, Primary Fixed Dim, On Primary Fixed, On Primary Fixed Variant, Secondary Fixed, Secondary Fixed Dim, On Secondary Fixed, On Secondary Fixed Variant, Tertiary Fixed, Tertiary Fixed Dim, On Tertiary Fixed, On Tertiary Fixed Variant";


enum ColorRoleEnum {
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

class ColorRole : public Resource {
	GDCLASS(ColorRole, Resource);

public:
	ColorRoleEnum color_role_enum = ColorRoleEnum::STATIC_ONE;
	Color color_scale = Color(1, 1, 1, 1);

	bool inverted = false;

	float darkened_amount = 0.0;
	float lightened_amount = 0.0;

	float lerp_weight = 0.0;
	Ref<ColorRole> lerp_color_role;
	Color lerp_color = Color(1, 1, 1, 1);

	void _update_color();
	bool dirty;

protected:
	static void _bind_methods();

public:

	ColorRole();

	ColorRole(ColorRoleEnum p_color_role_enum, Color &p_color_scale = Color(1, 1, 1, 1), bool p_inverted = false, float p_darkened_amount = 0.0, float p_lightened_amount = 0.0, float p_lerp_weight = 0.0, Ref<ColorRole> p_lerp_color_role = nullptr, Color &p_lerp_color = Color(1, 1, 1, 1));

	Color get_color(const Ref<ColorScheme> &p_color_scheme);
	~ColorRole();

	void set_color_role_enum(ColorRoleEnum p_color_role_enum);
	ColorRoleEnum get_color_role_enum() const;

	void set_color_scale(const Color &p_color_scale);
	Color get_color_scale() const;

	void set_inverted(bool p_inverted);
	bool get_inverted() const;

	void set_darkened_amount(float p_darkened_amount);
	float get_darkened_amount() const;

	void set_lightened_amount(float p_lightened_amount);
	float get_lightened_amount() const;

	void set_lerp_weight(float p_lerp_weight);
	float get_lerp_weight() const;

	void set_lerp_color_role(const Ref<ColorRole> &p_lerp_color_role);
	Ref<ColorRole> get_lerp_color_role() const;

	void set_lerp_color(const Color &p_lerp_color);
	Color get_lerp_color() const;
};

VARIANT_ENUM_CAST(ColorRoleEnum)

#endif // COLOR_ROLE_H

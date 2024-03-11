/**************************************************************************/
/*  default_theme.cpp                                                     */
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

#include "default_theme.h"

#include "core/os/os.h"
#include "default_font.gen.h"
#include "default_theme_icons.gen.h"
#include "scene/resources/font.h"
#include "scene/resources/gradient_texture.h"
#include "scene/resources/image_texture.h"
#include "scene/resources/style_box_flat.h"
#include "scene/resources/style_box_line.h"
#include "scene/resources/theme.h"
#include "scene/theme/theme_db.h"
#include "servers/text_server.h"

#include "modules/modules_enabled.gen.h" // For svg.
#ifdef MODULE_SVG_ENABLED
#include "modules/svg/image_loader_svg.h"
#endif

static const int default_font_size = 16;

static float scale = 1.0;

static const int default_margin = 4;
static const int default_corner_radius = 3;

static Ref<StyleBoxFlat> make_flat_stylebox(Color p_color, float p_margin_left = default_margin, float p_margin_top = default_margin, float p_margin_right = default_margin, float p_margin_bottom = default_margin, int p_corner_radius = default_corner_radius, bool p_draw_center = true, int p_border_width = 0) {
	Ref<StyleBoxFlat> style(memnew(StyleBoxFlat));
	style->set_bg_color(p_color);
	style->set_content_margin_individual(Math::round(p_margin_left * scale), Math::round(p_margin_top * scale), Math::round(p_margin_right * scale), Math::round(p_margin_bottom * scale));

	style->set_corner_radius_all(Math::round(p_corner_radius * scale));
	style->set_anti_aliased(true);
	// Adjust level of detail based on the corners' effective sizes.
	style->set_corner_detail(MIN(Math::ceil(1.5 * p_corner_radius), 6) * scale);

	style->set_draw_center(p_draw_center);
	style->set_border_width_all(Math::round(p_border_width * scale));

	return style;
}

static Ref<StyleBoxFlat> make_color_role_flat_stylebox(ColorRole p_color_role, Color p_color_scale, StyleBoxFlat::ElevationLevel p_level = StyleBoxFlat::ElevationLevel::Elevation_Level_0, float p_margin_left = default_margin, float p_margin_top = default_margin, float p_margin_right = default_margin, float p_margin_bottom = default_margin, int p_corner_radius = default_corner_radius, bool p_draw_center = true, int p_border_width = 0) {
	Ref<StyleBoxFlat> style(memnew(StyleBoxFlat));
	style->set_bg_color_role(p_color_role);
	style->set_bg_color_scale(p_color_scale);
	style->set_dynamic_shadow(true);
	style->set_elevation_level(p_level);

	style->set_shadow_color_role(ColorRole::SHADOW);
	style->set_shadow_color_scale(Color(1, 1, 1, 1));

	style->set_border_color_role(ColorRole::SECONDARY);
	style->set_border_color_scale(Color(1, 1, 1, 1));

	style->set_content_margin_individual(Math::round(p_margin_left * scale), Math::round(p_margin_top * scale), Math::round(p_margin_right * scale), Math::round(p_margin_bottom * scale));

	style->set_corner_radius_all(Math::round(p_corner_radius * scale));
	style->set_anti_aliased(true);
	// Adjust level of detail based on the corners' effective sizes.
	style->set_corner_detail(MIN(Math::ceil(1.5 * p_corner_radius), 6) * scale);

	style->set_draw_center(p_draw_center);
	style->set_border_width_all(Math::round(p_border_width * scale));

	return style;
}

static Ref<StyleBoxFlat> sb_expand(Ref<StyleBoxFlat> p_sbox, float p_left, float p_top, float p_right, float p_bottom) {
	p_sbox->set_expand_margin(SIDE_LEFT, Math::round(p_left * scale));
	p_sbox->set_expand_margin(SIDE_TOP, Math::round(p_top * scale));
	p_sbox->set_expand_margin(SIDE_RIGHT, Math::round(p_right * scale));
	p_sbox->set_expand_margin(SIDE_BOTTOM, Math::round(p_bottom * scale));
	return p_sbox;
}

// See also `editor_generate_icon()` in `editor/themes/editor_icons.cpp`.
static Ref<ImageTexture> generate_icon(int p_index) {
	Ref<Image> img = memnew(Image);

#ifdef MODULE_SVG_ENABLED
	// Upsample icon generation only if the scale isn't an integer multiplier.
	// Generating upsampled icons is slower, and the benefit is hardly visible
	// with integer scales.
	const bool upsample = !Math::is_equal_approx(Math::round(scale), scale);

	Error err = ImageLoaderSVG::create_image_from_string(img, default_theme_icons_sources[p_index], scale, upsample, HashMap<Color, Color>());
	ERR_FAIL_COND_V_MSG(err != OK, Ref<ImageTexture>(), "Failed generating icon, unsupported or invalid SVG data in default theme.");
#else
	// If the SVG module is disabled, we can't really display the UI well, but at least we won't crash.
	// 16 pixels is used as it's the most common base size for Godot icons.
	img = Image::create_empty(Math::round(16 * scale), Math::round(16 * scale), false, Image::FORMAT_RGBA8);
#endif

	return ImageTexture::create_from_image(img);
}

static Ref<StyleBox> make_empty_stylebox(float p_margin_left = -1, float p_margin_top = -1, float p_margin_right = -1, float p_margin_bottom = -1) {
	Ref<StyleBox> style(memnew(StyleBoxEmpty));
	style->set_content_margin_individual(Math::round(p_margin_left * scale), Math::round(p_margin_top * scale), Math::round(p_margin_right * scale), Math::round(p_margin_bottom * scale));
	return style;
}

void fill_default_theme(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon,const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme) {
	scale = p_scale;

	// Default theme properties.
	theme->set_default_font(default_font);
	theme->set_default_font_size(Math::round(default_font_size * scale));
	theme->set_default_base_scale(scale);
	theme->set_default_color_scheme(default_color_scheme);

	const float hover_state_layer_opacity = 0.08;
	const float focus_state_layer_opacity = 0.10;
	const float pressed_state_layer_opacity = 0.10;
	const float dragged_state_layer_opacity = 0.16;

	const Color hover_state_layer_color_scale = Color(1, 1, 1, hover_state_layer_opacity);
	const Color focus_state_layer_color_scale = Color(1, 1, 1, focus_state_layer_opacity);
	const Color pressed_state_layer_color_scale = Color(1, 1, 1, pressed_state_layer_opacity);
	const Color dragged_state_layer_color_scale = Color(1, 1, 1, dragged_state_layer_opacity);

	const Color one_color_scale = Color(1, 1, 1, 1);

	// Font colors
	const Color control_font_color = Color(0.875, 0.875, 0.875);
	const Color control_font_low_color = Color(0.7, 0.7, 0.7);
	const Color control_font_lower_color = Color(0.65, 0.65, 0.65);
	const Color control_font_hover_color = Color(0.95, 0.95, 0.95);
	const Color control_font_focus_color = Color(0.95, 0.95, 0.95);
	const Color control_font_disabled_color = control_font_color * Color(1, 1, 1, 0.5);
	const Color control_font_placeholder_color = Color(control_font_color.r, control_font_color.g, control_font_color.b, 0.6f);
	const Color control_font_pressed_color = Color(1, 1, 1);
	const Color control_selection_color = Color(0.5, 0.5, 0.5);

	const Color control_font_color_scale = Color(1, 1, 1, 1);
	const Color control_font_low_color_scale = Color(0.8, 0.8, 0.8);
	const Color control_font_lower_color_scale = Color(0.7, 0.7, 0.7);
	const Color control_font_hover_color_scale = Color(1.2, 1.2, 1.2, 1);
	const Color control_font_focus_color_scale = Color(1.2, 1.2, 1.2, 1);
	const Color control_font_disabled_color_scale = Color(1, 1, 1, 0.5);
	const Color control_font_pressed_color_scale = Color(1.3, 1.3, 1.3, 1);
	const Color control_font_hover_pressed_color_scale = Color(1.3, 1.3, 1.3, 1);
	const Color control_font_outline_color_scale = Color(1, 1, 1);
	const Color control_font_placeholder_color_scale = Color(1, 1, 1, 0.6f);
	const Color control_selection_color_scale = Color(0.5, 0.5, 0.5);

	// StyleBox colors
	const Color style_normal_color = Color(0.1, 0.1, 0.1, 0.6);
	const Color style_hover_color = Color(0.225, 0.225, 0.225, 0.6);
	const Color style_pressed_color = Color(0, 0, 0, 0.6);
	const Color style_disabled_color = Color(0.1, 0.1, 0.1, 0.3);
	const Color style_focus_color = Color(1, 1, 1, 0.75);
	const Color style_popup_color = Color(0.25, 0.25, 0.25, 1);
	const Color style_popup_border_color = Color(0.175, 0.175, 0.175, 1);
	const Color style_popup_hover_color = Color(0.4, 0.4, 0.4, 1);
	const Color style_selected_color = Color(1, 1, 1, 0.3);
	// Don't use a color too bright to keep the percentage readable.
	const Color style_progress_color = Color(1, 1, 1, 0.4);
	const Color style_separator_color = Color(0.5, 0.5, 0.5);

	const Color style_one_color_scale = Color(1, 1, 1, 1);
	const Color style_zero_color_scale = Color(1, 1, 1, 0);
	

	const Color style_normal_color_scale = Color(1, 1, 1, 0.6);
	const Color style_hover_color_scale = Color(1.5, 1.5, 1.5, 0.6);
	const Color style_pressed_color_scale = Color(0.5, 0.5, 0.5, 0.6);
	const Color style_disabled_color_scale = Color(1, 1, 1, 0.3);
	const Color style_focus_color_scale = Color(1, 1, 1, 0.75);
	const Color style_popup_color_scale = Color(1, 1, 1, 1);
	const Color style_popup_border_color_scale = Color(0.7, 0.7, 0.7);
	const Color style_popup_hover_color_scale = Color(1.6, 1.6, 1.6);
	const Color style_selected_color_scale = Color(1, 1, 1, 0.3);
	const Color style_progress_color_scale = Color(1, 1, 1, 0.4);
	const Color style_separator_color_scale = Color(1, 1, 1, 1);

	// Convert the generated icon sources to a dictionary for easier access.
	// Unlike the editor icons, there is no central repository of icons in the Theme resource itself to keep it tidy.
	Dictionary icons;
	for (int i = 0; i < default_theme_icons_count; i++) {
		icons[default_theme_icons_names[i]] = generate_icon(i);
	}

	// Panel

	theme->set_stylebox("panel", "Panel", make_color_role_flat_stylebox(ColorRole::PRIMARY_CONTAINER, Color(1, 1, 1, 0.6),StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0));

	// ColorRect
	theme->set_color_scheme("default_color_scheme", "ColorRect", default_color_scheme);

	// Button
	const Ref<StyleBoxFlat> button_normal = make_color_role_flat_stylebox(ColorRole::SURFACE_CONTAINER_LOW, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_1);
	const Ref<StyleBoxFlat> button_hover = make_color_role_flat_stylebox(ColorRole::SURFACE_CONTAINER_LOW, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_2);
	const Ref<StyleBoxFlat> button_pressed = make_color_role_flat_stylebox(ColorRole::SURFACE_CONTAINER_LOW, style_one_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_1);
	const Ref<StyleBoxFlat> button_disabled = make_color_role_flat_stylebox(ColorRole::ON_SURFACE, Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	Ref<StyleBoxFlat> focus = make_color_role_flat_stylebox(ColorRole::SECONDARY, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// Make the focus outline appear to be flush with the buttons it's focusing.
	focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<StyleBoxFlat> button_hover_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	const Ref<StyleBoxFlat> button_pressed_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, pressed_state_layer_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	Ref<StyleBoxFlat> focus_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	theme->set_stylebox("normal", "Button", button_normal);
	theme->set_stylebox("hover", "Button", button_hover);
	theme->set_stylebox("pressed", "Button", button_pressed);
	theme->set_stylebox("disabled", "Button", button_disabled);
	theme->set_stylebox("focus", "Button", focus);

	theme->set_stylebox("hover_state_layer", "Button", button_hover_state_layer);
	theme->set_stylebox("pressed_state_layer", "Button", button_pressed_state_layer);
	theme->set_stylebox("focus_state_layer", "Button", focus_state_layer);

	theme->set_font("font", "Button", default_font);
	theme->set_font("text_icon_font", "Button", default_icon_font);
	theme->set_font_size("font_size", "Button", -1);
	theme->set_font_size("text_icon_font_size", "Button", -1);
	theme->set_constant("outline_size", "Button", 0);

	theme->set_color_scheme("default_color_scheme", "Button", default_color_scheme);

	theme->set_color("font_color_scale", "Button", one_color_scale);
	theme->set_color("font_focus_color_scale", "Button", one_color_scale);
	theme->set_color("font_pressed_color_scale", "Button", one_color_scale);
	theme->set_color("font_hover_color_scale", "Button", one_color_scale);
	theme->set_color("font_hover_pressed_color_scale", "Button", one_color_scale);
	theme->set_color("font_disabled_color_scale", "Button", Color(one_color_scale, 0.38));
	theme->set_color("font_outline_color_scale", "Button", one_color_scale);

	theme->set_color("icon_normal_color_scale", "Button", one_color_scale);
	theme->set_color("icon_focus_color_scale", "Button", one_color_scale);
	theme->set_color("icon_pressed_color_scale", "Button", one_color_scale);
	theme->set_color("icon_hover_color_scale", "Button", one_color_scale);
	theme->set_color("icon_hover_pressed_color_scale", "Button", one_color_scale);
	theme->set_color("icon_disabled_color_scale", "Button", Color(one_color_scale, 0.38));

	theme->set_color("text_icon_normal_color_scale", "Button", one_color_scale);
	theme->set_color("text_icon_focus_color_scale", "Button", one_color_scale);
	theme->set_color("text_icon_pressed_color_scale", "Button", one_color_scale);
	theme->set_color("text_icon_hover_color_scale", "Button", one_color_scale);
	theme->set_color("text_icon_hover_pressed_color_scale", "Button", one_color_scale);
	theme->set_color("text_icon_disabled_color_scale", "Button", Color(one_color_scale, 0.38));

	theme->set_color_scheme("font_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("font_pressed_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("font_focus_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_pressed_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "Button", Ref<ColorScheme>());

	theme->set_color_scheme("icon_normal_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("icon_pressed_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("icon_hover_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("icon_focus_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("icon_hover_pressed_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("icon_disabled_color_scheme", "Button", Ref<ColorScheme>());

	theme->set_color_scheme("text_icon_normal_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("text_icon_pressed_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("text_icon_hover_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("text_icon_focus_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "Button", Ref<ColorScheme>());
	theme->set_color_scheme("text_icon_disabled_color_scheme", "Button", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "Button", ColorRole::PRIMARY);
	theme->set_color_role("font_pressed_color_role", "Button", ColorRole::PRIMARY);
	theme->set_color_role("font_hover_color_role", "Button", ColorRole::PRIMARY);
	theme->set_color_role("font_focus_color_role", "Button", ColorRole::PRIMARY);
	theme->set_color_role("font_hover_pressed_color_role", "Button", ColorRole::PRIMARY);
	theme->set_color_role("font_disabled_color_role", "Button", ColorRole::ON_SURFACE);
	theme->set_color_role("font_outline_color_role", "Button", ColorRole::OUTLINE);

	theme->set_color_role("icon_normal_color_role", "Button", ColorRole::STATIC_COLOR);
	theme->set_color_role("icon_pressed_color_role", "Button", ColorRole::STATIC_COLOR);
	theme->set_color_role("icon_hover_color_role", "Button", ColorRole::STATIC_COLOR);
	theme->set_color_role("icon_hover_pressed_color_role", "Button", ColorRole::STATIC_COLOR);
	theme->set_color_role("icon_focus_color_role", "Button", ColorRole::STATIC_COLOR);
	theme->set_color_role("icon_disabled_color_role", "Button", ColorRole::STATIC_COLOR);

	theme->set_color_role("text_icon_normal_color_role", "Button", ColorRole::PRIMARY);
	theme->set_color_role("text_icon_pressed_color_role", "Button", ColorRole::PRIMARY);
	theme->set_color_role("text_icon_hover_color_role", "Button", ColorRole::PRIMARY);
	theme->set_color_role("text_icon_hover_pressed_color_role", "Button", ColorRole::PRIMARY);
	theme->set_color_role("text_icon_focus_color_role", "Button", ColorRole::PRIMARY);
	theme->set_color_role("text_icon_disabled_color_role", "Button", ColorRole::ON_SURFACE);

	theme->set_color("font_color", "Button", control_font_color);
	theme->set_color("font_pressed_color", "Button", control_font_pressed_color);
	theme->set_color("font_hover_color", "Button", control_font_hover_color);
	theme->set_color("font_focus_color", "Button", control_font_focus_color);
	theme->set_color("font_hover_pressed_color", "Button", control_font_pressed_color);
	theme->set_color("font_disabled_color", "Button", control_font_disabled_color);
	theme->set_color("font_outline_color", "Button", Color(1, 1, 1));

	theme->set_color("icon_normal_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("icon_pressed_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("icon_hover_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("icon_hover_pressed_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("icon_focus_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("icon_disabled_color", "Button", Color(1, 1, 1, 0.4));

	theme->set_color("text_icon_normal_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("text_icon_pressed_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("text_icon_hover_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("text_icon_hover_pressed_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("text_icon_focus_color", "Button", Color(1, 1, 1, 1));
	theme->set_color("text_icon_disabled_color", "Button", Color(1, 1, 1, 0.4));

	theme->set_constant("h_separation", "Button", Math::round(4 * scale));
	theme->set_constant("icon_max_width", "Button", 0);

	// ElevatedButton
	{
		theme->set_type_variation("ElevatedButton", "Button");
		const Ref<StyleBoxFlat> elevated_button_normal = make_color_role_flat_stylebox(ColorRole::SURFACE_CONTAINER_LOW, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_1);
		const Ref<StyleBoxFlat> elevated_button_hover = make_color_role_flat_stylebox(ColorRole::SURFACE_CONTAINER_LOW, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_2);
		const Ref<StyleBoxFlat> elevated_button_pressed = make_color_role_flat_stylebox(ColorRole::SURFACE_CONTAINER_LOW, style_one_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_1);
		const Ref<StyleBoxFlat> elevated_button_disabled = make_color_role_flat_stylebox(ColorRole::ON_SURFACE, Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		Ref<StyleBoxFlat> elevated_focus = make_color_role_flat_stylebox(ColorRole::SECONDARY, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		// Make the focus outline appear to be flush with the buttons it's focusing.
		elevated_focus->set_expand_margin_all(Math::round(2 * scale));

		const Ref<StyleBoxFlat> elevated_button_hover_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		const Ref<StyleBoxFlat> elevated_button_pressed_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, pressed_state_layer_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		Ref<StyleBoxFlat> elevated_focus_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		theme->set_stylebox("normal", "ElevatedButton", elevated_button_normal);
		theme->set_stylebox("hover", "ElevatedButton", elevated_button_hover);
		theme->set_stylebox("pressed", "ElevatedButton", elevated_button_pressed);
		theme->set_stylebox("disabled", "ElevatedButton", elevated_button_disabled);
		theme->set_stylebox("focus", "ElevatedButton", elevated_focus);

		theme->set_stylebox("hover_state_layer", "ElevatedButton", elevated_button_hover_state_layer);
		theme->set_stylebox("pressed_state_layer", "ElevatedButton", elevated_button_pressed_state_layer);
		theme->set_stylebox("focus_state_layer", "ElevatedButton", elevated_focus_state_layer);

		theme->set_font("font", "ElevatedButton", default_font);
		theme->set_font("text_icon_font", "ElevatedButton", default_icon_font);
		theme->set_font_size("font_size", "ElevatedButton", -1);
		theme->set_font_size("text_icon_font_size", "ElevatedButton", 18);
		theme->set_constant("outline_size", "ElevatedButton", 0);

		theme->set_color_scheme("default_color_scheme", "ElevatedButton", default_color_scheme);

		theme->set_color("font_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("font_focus_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("font_pressed_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("font_hover_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("font_hover_pressed_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("font_disabled_color_scale", "ElevatedButton", Color(one_color_scale, 0.38));
		theme->set_color("font_outline_color_scale", "ElevatedButton", one_color_scale);

		theme->set_color("icon_normal_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("icon_focus_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("icon_pressed_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("icon_hover_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("icon_hover_pressed_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("icon_disabled_color_scale", "ElevatedButton", Color(one_color_scale, 0.38));

		theme->set_color("text_icon_normal_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("text_icon_focus_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("text_icon_pressed_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("text_icon_hover_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("text_icon_hover_pressed_color_scale", "ElevatedButton", one_color_scale);
		theme->set_color("text_icon_disabled_color_scale", "ElevatedButton", Color(one_color_scale, 0.38));

		theme->set_color_scheme("font_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_focus_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_disabled_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_outline_color_scheme", "ElevatedButton", Ref<ColorScheme>());

		theme->set_color_scheme("icon_normal_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_focus_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_disabled_color_scheme", "ElevatedButton", Ref<ColorScheme>());

		theme->set_color_scheme("text_icon_normal_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_focus_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_disabled_color_scheme", "ElevatedButton", Ref<ColorScheme>());

		theme->set_color_role("font_color_role", "ElevatedButton", ColorRole::PRIMARY);
		theme->set_color_role("font_pressed_color_role", "ElevatedButton", ColorRole::PRIMARY);
		theme->set_color_role("font_hover_color_role", "ElevatedButton", ColorRole::PRIMARY);
		theme->set_color_role("font_focus_color_role", "ElevatedButton", ColorRole::PRIMARY);
		theme->set_color_role("font_hover_pressed_color_role", "ElevatedButton", ColorRole::PRIMARY);
		theme->set_color_role("font_disabled_color_role", "ElevatedButton", ColorRole::ON_SURFACE);
		theme->set_color_role("font_outline_color_role", "ElevatedButton", ColorRole::OUTLINE);

		theme->set_color_role("icon_normal_color_role", "ElevatedButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_pressed_color_role", "ElevatedButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_color_role", "ElevatedButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_pressed_color_role", "ElevatedButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_focus_color_role", "ElevatedButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_disabled_color_role", "ElevatedButton", ColorRole::STATIC_COLOR);

		theme->set_color_role("text_icon_normal_color_role", "ElevatedButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_pressed_color_role", "ElevatedButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_hover_color_role", "ElevatedButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_hover_pressed_color_role", "ElevatedButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_focus_color_role", "ElevatedButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_disabled_color_role", "ElevatedButton", ColorRole::ON_SURFACE);

		theme->set_color("font_color", "ElevatedButton", control_font_color);
		theme->set_color("font_pressed_color", "ElevatedButton", control_font_pressed_color);
		theme->set_color("font_hover_color", "ElevatedButton", control_font_hover_color);
		theme->set_color("font_focus_color", "ElevatedButton", control_font_focus_color);
		theme->set_color("font_hover_pressed_color", "ElevatedButton", control_font_pressed_color);
		theme->set_color("font_disabled_color", "ElevatedButton", control_font_disabled_color);
		theme->set_color("font_outline_color", "ElevatedButton", Color(1, 1, 1));

		theme->set_color("icon_normal_color", "ElevatedButton", Color(1, 1, 1, 1));
		theme->set_color("icon_pressed_color", "ElevatedButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_color", "ElevatedButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_pressed_color", "ElevatedButton", Color(1, 1, 1, 1));
		theme->set_color("icon_focus_color", "ElevatedButton", Color(1, 1, 1, 1));
		theme->set_color("icon_disabled_color", "ElevatedButton", Color(1, 1, 1, 0.4));

		theme->set_color("text_icon_normal_color", "ElevatedButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_pressed_color", "ElevatedButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_color", "ElevatedButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_pressed_color", "ElevatedButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_focus_color", "ElevatedButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_disabled_color", "ElevatedButton", Color(1, 1, 1, 0.4));

		theme->set_constant("h_separation", "ElevatedButton", Math::round(4 * scale));
		theme->set_constant("icon_max_width", "ElevatedButton", 0);
	}


	// FilledButton
	{
		theme->set_type_variation("FilledButton", "Button");
		const Ref<StyleBoxFlat> filled_button_normal = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		const Ref<StyleBoxFlat> filled_button_hover = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_1);

		const Ref<StyleBoxFlat> filled_button_pressed = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_one_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		const Ref<StyleBoxFlat> filled_button_disabled = make_color_role_flat_stylebox(ColorRole::ON_SURFACE, Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		Ref<StyleBoxFlat> filled_focus = make_color_role_flat_stylebox(ColorRole::SECONDARY, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		// Make the focus outline appear to be flush with the buttons it's focusing.
		filled_focus->set_expand_margin_all(Math::round(2 * scale));

		const Ref<StyleBoxFlat> filled_button_hover_state_layer = make_color_role_flat_stylebox(ColorRole::ON_PRIMARY, hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		const Ref<StyleBoxFlat> filled_button_pressed_state_layer = make_color_role_flat_stylebox(ColorRole::ON_PRIMARY, pressed_state_layer_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		Ref<StyleBoxFlat> filled_focus_state_layer = make_color_role_flat_stylebox(ColorRole::ON_PRIMARY, focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		theme->set_stylebox("normal", "FilledButton", filled_button_normal);
		theme->set_stylebox("hover", "FilledButton", filled_button_hover);
		theme->set_stylebox("pressed", "FilledButton", filled_button_pressed);
		theme->set_stylebox("disabled", "FilledButton", filled_button_disabled);
		theme->set_stylebox("focus", "FilledButton", filled_focus);

		theme->set_stylebox("hover_state_layer", "FilledButton", filled_button_hover_state_layer);
		theme->set_stylebox("pressed_state_layer", "FilledButton", filled_button_pressed_state_layer);
		theme->set_stylebox("focus_state_layer", "FilledButton", filled_focus_state_layer);

		theme->set_font("font", "FilledButton", default_font);
		theme->set_font("text_icon_font", "FilledButton", default_icon_font);
		theme->set_font_size("font_size", "FilledButton", -1);
		theme->set_font_size("text_icon_font_size", "FilledButton", 18);
		theme->set_constant("outline_size", "FilledButton", 0);

		theme->set_color_scheme("default_color_scheme", "FilledButton", default_color_scheme);

		theme->set_color("font_color_scale", "FilledButton", one_color_scale);
		theme->set_color("font_focus_color_scale", "FilledButton", one_color_scale);
		theme->set_color("font_pressed_color_scale", "FilledButton", one_color_scale);
		theme->set_color("font_hover_color_scale", "FilledButton", one_color_scale);
		theme->set_color("font_hover_pressed_color_scale", "FilledButton", one_color_scale);
		theme->set_color("font_disabled_color_scale", "FilledButton", Color(one_color_scale, 0.38));
		theme->set_color("font_outline_color_scale", "FilledButton", one_color_scale);

		theme->set_color("icon_normal_color_scale", "FilledButton", one_color_scale);
		theme->set_color("icon_focus_color_scale", "FilledButton", one_color_scale);
		theme->set_color("icon_pressed_color_scale", "FilledButton", one_color_scale);
		theme->set_color("icon_hover_color_scale", "FilledButton", one_color_scale);
		theme->set_color("icon_hover_pressed_color_scale", "FilledButton", one_color_scale);
		theme->set_color("icon_disabled_color_scale", "FilledButton", Color(one_color_scale, 0.38));

		theme->set_color("text_icon_normal_color_scale", "FilledButton", one_color_scale);
		theme->set_color("text_icon_focus_color_scale", "FilledButton", one_color_scale);
		theme->set_color("text_icon_pressed_color_scale", "FilledButton", one_color_scale);
		theme->set_color("text_icon_hover_color_scale", "FilledButton", one_color_scale);
		theme->set_color("text_icon_hover_pressed_color_scale", "FilledButton", one_color_scale);
		theme->set_color("text_icon_disabled_color_scale", "FilledButton", Color(one_color_scale, 0.38));

		theme->set_color_scheme("font_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_focus_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_disabled_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_outline_color_scheme", "FilledButton", Ref<ColorScheme>());

		theme->set_color_scheme("icon_normal_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_focus_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_disabled_color_scheme", "FilledButton", Ref<ColorScheme>());

		theme->set_color_scheme("text_icon_normal_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_focus_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_disabled_color_scheme", "FilledButton", Ref<ColorScheme>());

		theme->set_color_role("font_color_role", "FilledButton", ColorRole::ON_PRIMARY);
		theme->set_color_role("font_pressed_color_role", "FilledButton", ColorRole::ON_PRIMARY);
		theme->set_color_role("font_hover_color_role", "FilledButton", ColorRole::ON_PRIMARY);
		theme->set_color_role("font_focus_color_role", "FilledButton", ColorRole::ON_PRIMARY);
		theme->set_color_role("font_hover_pressed_color_role", "FilledButton", ColorRole::ON_PRIMARY);
		theme->set_color_role("font_disabled_color_role", "FilledButton", ColorRole::ON_SURFACE);
		theme->set_color_role("font_outline_color_role", "FilledButton", ColorRole::OUTLINE);

		theme->set_color_role("icon_normal_color_role", "FilledButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_pressed_color_role", "FilledButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_color_role", "FilledButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_pressed_color_role", "FilledButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_focus_color_role", "FilledButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_disabled_color_role", "FilledButton", ColorRole::STATIC_COLOR);

		theme->set_color_role("text_icon_normal_color_role", "FilledButton", ColorRole::ON_PRIMARY);
		theme->set_color_role("text_icon_pressed_color_role", "FilledButton", ColorRole::ON_PRIMARY);
		theme->set_color_role("text_icon_hover_color_role", "FilledButton", ColorRole::ON_PRIMARY);
		theme->set_color_role("text_icon_hover_pressed_color_role", "FilledButton", ColorRole::ON_PRIMARY);
		theme->set_color_role("text_icon_focus_color_role", "FilledButton", ColorRole::ON_PRIMARY);
		theme->set_color_role("text_icon_disabled_color_role", "FilledButton", ColorRole::ON_SURFACE);

		theme->set_color("font_color", "FilledButton", control_font_color);
		theme->set_color("font_pressed_color", "FilledButton", control_font_pressed_color);
		theme->set_color("font_hover_color", "FilledButton", control_font_hover_color);
		theme->set_color("font_focus_color", "FilledButton", control_font_focus_color);
		theme->set_color("font_hover_pressed_color", "FilledButton", control_font_pressed_color);
		theme->set_color("font_disabled_color", "FilledButton", control_font_disabled_color);
		theme->set_color("font_outline_color", "FilledButton", Color(1, 1, 1));

		theme->set_color("icon_normal_color", "FilledButton", Color(1, 1, 1, 1));
		theme->set_color("icon_pressed_color", "FilledButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_color", "FilledButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_pressed_color", "FilledButton", Color(1, 1, 1, 1));
		theme->set_color("icon_focus_color", "FilledButton", Color(1, 1, 1, 1));
		theme->set_color("icon_disabled_color", "FilledButton", Color(1, 1, 1, 0.4));

		theme->set_color("text_icon_normal_color", "FilledButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_pressed_color", "FilledButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_color", "FilledButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_pressed_color", "FilledButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_focus_color", "FilledButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_disabled_color", "FilledButton", Color(1, 1, 1, 0.4));

		theme->set_constant("h_separation", "FilledButton", Math::round(4 * scale));
		theme->set_constant("icon_max_width", "FilledButton", 0);
	}

	// FilledTonalButton
	{
		theme->set_type_variation("FilledTonalButton", "Button");
		const Ref<StyleBoxFlat> filled_tonal_button_normal = make_color_role_flat_stylebox(ColorRole::SECONDARY_CONTAINER, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		const Ref<StyleBoxFlat> filled_tonal_button_hover = make_color_role_flat_stylebox(ColorRole::SECONDARY_CONTAINER, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_1);

		const Ref<StyleBoxFlat> filled_tonal_button_pressed = make_color_role_flat_stylebox(ColorRole::SECONDARY_CONTAINER, style_one_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		const Ref<StyleBoxFlat> filled_tonal_button_disabled = make_color_role_flat_stylebox(ColorRole::ON_SURFACE, Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		Ref<StyleBoxFlat> filled_tonal_focus = make_color_role_flat_stylebox(ColorRole::SECONDARY, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		// Make the focus outline appear to be flush with the buttons it's focusing.
		filled_tonal_focus->set_expand_margin_all(Math::round(2 * scale));

		const Ref<StyleBoxFlat> filled_tonal_button_hover_state_layer = make_color_role_flat_stylebox(ColorRole::ON_SECONDARY_CONTAINER, hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		const Ref<StyleBoxFlat> filled_tonal_button_pressed_state_layer = make_color_role_flat_stylebox(ColorRole::ON_SECONDARY_CONTAINER, pressed_state_layer_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		Ref<StyleBoxFlat> filled_tonal_focus_state_layer = make_color_role_flat_stylebox(ColorRole::ON_SECONDARY_CONTAINER, focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		theme->set_stylebox("normal", "FilledTonalButton", filled_tonal_button_normal);
		theme->set_stylebox("hover", "FilledTonalButton", filled_tonal_button_hover);
		theme->set_stylebox("pressed", "FilledTonalButton", filled_tonal_button_pressed);
		theme->set_stylebox("disabled", "FilledTonalButton", filled_tonal_button_disabled);
		theme->set_stylebox("focus", "FilledTonalButton", filled_tonal_focus);

		theme->set_stylebox("hover_state_layer", "FilledTonalButton",filled_tonal_button_hover_state_layer);
		theme->set_stylebox("pressed_state_layer", "FilledTonalButton", filled_tonal_button_pressed_state_layer);
		theme->set_stylebox("focus_state_layer", "FilledTonalButton", filled_tonal_focus_state_layer);

		theme->set_font("font", "FilledTonalButton", default_font);
		theme->set_font("text_icon_font", "FilledTonalButton", default_icon_font);
		theme->set_font_size("font_size", "FilledTonalButton", -1);
		theme->set_font_size("text_icon_font_size", "FilledTonalButton", 18);
		theme->set_constant("outline_size", "FilledTonalButton", 0);

		theme->set_color_scheme("default_color_scheme", "FilledTonalButton", default_color_scheme);

		theme->set_color("font_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("font_focus_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("font_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("font_hover_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("font_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("font_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));
		theme->set_color("font_outline_color_scale", "FilledTonalButton", one_color_scale);

		theme->set_color("icon_normal_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("icon_focus_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("icon_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("icon_hover_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("icon_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("icon_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));

		theme->set_color("text_icon_normal_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("text_icon_focus_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("text_icon_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("text_icon_hover_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("text_icon_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("text_icon_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));

		theme->set_color_scheme("font_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_outline_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

		theme->set_color_scheme("icon_normal_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

		theme->set_color_scheme("text_icon_normal_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

		theme->set_color_role("font_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("font_pressed_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("font_hover_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("font_focus_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("font_hover_pressed_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("font_disabled_color_role", "FilledTonalButton", ColorRole::ON_SURFACE);
		theme->set_color_role("font_outline_color_role", "FilledTonalButton", ColorRole::OUTLINE);

		theme->set_color_role("icon_normal_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_pressed_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_pressed_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_focus_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_disabled_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);

		theme->set_color_role("text_icon_normal_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("text_icon_pressed_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("text_icon_hover_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("text_icon_hover_pressed_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("text_icon_focus_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("text_icon_disabled_color_role", "FilledTonalButton", ColorRole::ON_SURFACE);

		theme->set_color("font_color", "FilledTonalButton", control_font_color);
		theme->set_color("font_pressed_color", "FilledTonalButton", control_font_pressed_color);
		theme->set_color("font_hover_color", "FilledTonalButton", control_font_hover_color);
		theme->set_color("font_focus_color", "FilledTonalButton", control_font_focus_color);
		theme->set_color("font_hover_pressed_color", "FilledTonalButton", control_font_pressed_color);
		theme->set_color("font_disabled_color", "FilledTonalButton", control_font_disabled_color);
		theme->set_color("font_outline_color", "FilledTonalButton", Color(1, 1, 1));

		theme->set_color("icon_normal_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("icon_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("icon_focus_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("icon_disabled_color", "FilledTonalButton", Color(1, 1, 1, 0.4));

		theme->set_color("text_icon_normal_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_focus_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_disabled_color", "FilledTonalButton", Color(1, 1, 1, 0.4));

		theme->set_constant("h_separation", "FilledTonalButton", Math::round(4 * scale));
		theme->set_constant("icon_max_width", "FilledTonalButton", 0);
	}

	// OutlinedButton
	{
		theme->set_type_variation("OutlinedButton", "Button");
		const Ref<StyleBoxFlat> filled_tonal_button_normal = make_color_role_flat_stylebox(ColorRole::SECONDARY_CONTAINER, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		const Ref<StyleBoxFlat> filled_tonal_button_hover = make_color_role_flat_stylebox(ColorRole::SECONDARY_CONTAINER, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_1);

		const Ref<StyleBoxFlat> filled_tonal_button_pressed = make_color_role_flat_stylebox(ColorRole::SECONDARY_CONTAINER, style_one_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		const Ref<StyleBoxFlat> filled_tonal_button_disabled = make_color_role_flat_stylebox(ColorRole::ON_SURFACE, Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		Ref<StyleBoxFlat> filled_tonal_focus = make_color_role_flat_stylebox(ColorRole::SECONDARY, style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		// Make the focus outline appear to be flush with the buttons it's focusing.
		filled_tonal_focus->set_expand_margin_all(Math::round(2 * scale));

		const Ref<StyleBoxFlat> filled_tonal_button_hover_state_layer = make_color_role_flat_stylebox(ColorRole::ON_SECONDARY_CONTAINER, hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		const Ref<StyleBoxFlat> filled_tonal_button_pressed_state_layer = make_color_role_flat_stylebox(ColorRole::ON_SECONDARY_CONTAINER, pressed_state_layer_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		Ref<StyleBoxFlat> filled_tonal_focus_state_layer = make_color_role_flat_stylebox(ColorRole::ON_SECONDARY_CONTAINER, focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		theme->set_stylebox("normal", "FilledTonalButton", filled_tonal_button_normal);
		theme->set_stylebox("hover", "FilledTonalButton", filled_tonal_button_hover);
		theme->set_stylebox("pressed", "FilledTonalButton", filled_tonal_button_pressed);
		theme->set_stylebox("disabled", "FilledTonalButton", filled_tonal_button_disabled);
		theme->set_stylebox("focus", "FilledTonalButton", filled_tonal_focus);

		theme->set_stylebox("hover_state_layer", "FilledTonalButton",filled_tonal_button_hover_state_layer);
		theme->set_stylebox("pressed_state_layer", "FilledTonalButton", filled_tonal_button_pressed_state_layer);
		theme->set_stylebox("focus_state_layer", "FilledTonalButton", filled_tonal_focus_state_layer);

		theme->set_font("font", "FilledTonalButton", default_font);
		theme->set_font("text_icon_font", "FilledTonalButton", default_icon_font);
		theme->set_font_size("font_size", "FilledTonalButton", -1);
		theme->set_font_size("text_icon_font_size", "FilledTonalButton", 18);
		theme->set_constant("outline_size", "FilledTonalButton", 0);

		theme->set_color_scheme("default_color_scheme", "FilledTonalButton", default_color_scheme);

		theme->set_color("font_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("font_focus_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("font_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("font_hover_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("font_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("font_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));
		theme->set_color("font_outline_color_scale", "FilledTonalButton", one_color_scale);

		theme->set_color("icon_normal_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("icon_focus_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("icon_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("icon_hover_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("icon_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("icon_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));

		theme->set_color("text_icon_normal_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("text_icon_focus_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("text_icon_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("text_icon_hover_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("text_icon_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
		theme->set_color("text_icon_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));

		theme->set_color_scheme("font_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_outline_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

		theme->set_color_scheme("icon_normal_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

		theme->set_color_scheme("text_icon_normal_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

		theme->set_color_role("font_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("font_pressed_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("font_hover_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("font_focus_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("font_hover_pressed_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("font_disabled_color_role", "FilledTonalButton", ColorRole::ON_SURFACE);
		theme->set_color_role("font_outline_color_role", "FilledTonalButton", ColorRole::OUTLINE);

		theme->set_color_role("icon_normal_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_pressed_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_pressed_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_focus_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_disabled_color_role", "FilledTonalButton", ColorRole::STATIC_COLOR);

		theme->set_color_role("text_icon_normal_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("text_icon_pressed_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("text_icon_hover_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("text_icon_hover_pressed_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("text_icon_focus_color_role", "FilledTonalButton", ColorRole::ON_SECONDARY_CONTAINER);
		theme->set_color_role("text_icon_disabled_color_role", "FilledTonalButton", ColorRole::ON_SURFACE);

		theme->set_color("font_color", "FilledTonalButton", control_font_color);
		theme->set_color("font_pressed_color", "FilledTonalButton", control_font_pressed_color);
		theme->set_color("font_hover_color", "FilledTonalButton", control_font_hover_color);
		theme->set_color("font_focus_color", "FilledTonalButton", control_font_focus_color);
		theme->set_color("font_hover_pressed_color", "FilledTonalButton", control_font_pressed_color);
		theme->set_color("font_disabled_color", "FilledTonalButton", control_font_disabled_color);
		theme->set_color("font_outline_color", "FilledTonalButton", Color(1, 1, 1));

		theme->set_color("icon_normal_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("icon_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("icon_focus_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("icon_disabled_color", "FilledTonalButton", Color(1, 1, 1, 0.4));

		theme->set_color("text_icon_normal_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_focus_color", "FilledTonalButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_disabled_color", "FilledTonalButton", Color(1, 1, 1, 0.4));

		theme->set_constant("h_separation", "FilledTonalButton", Math::round(4 * scale));
		theme->set_constant("icon_max_width", "FilledTonalButton", 0);
	}


	// OutlinedButton
	{
		theme->set_type_variation("OutlinedButton", "Button");

		Ref<StyleBoxFlat> outlined_button_normal = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		outlined_button_normal->set_expand_margin_all(Math::round(2 * scale));
		outlined_button_normal->set_border_color_role(ColorRole::OUTLINE);

		Ref<StyleBoxFlat> outlined_button_hover = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		outlined_button_hover->set_expand_margin_all(Math::round(2 * scale));
		outlined_button_hover->set_border_color_role(ColorRole::OUTLINE);

		Ref<StyleBoxFlat> outlined_button_pressed = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		outlined_button_pressed->set_expand_margin_all(Math::round(2 * scale));
		outlined_button_pressed->set_border_color_role(ColorRole::OUTLINE);

		Ref<StyleBoxFlat> outlined_button_disabled = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		outlined_button_disabled->set_expand_margin_all(Math::round(2 * scale));
		outlined_button_disabled->set_border_color_role(ColorRole::ON_SURFACE);
		outlined_button_disabled->set_border_color_scale(Color(1, 1, 1, 0.12));

		Ref<StyleBoxFlat> outlined_focus = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		// Make the focus outline appear to be flush with the buttons it's focusing.
		outlined_focus->set_expand_margin_all(Math::round(2 * scale));
		outlined_button_disabled->set_border_color_role(ColorRole::OUTLINE);
		outlined_button_disabled->set_border_color_scale(Color(1, 1, 1, 1));

		const Ref<StyleBoxFlat> outlined_button_hover_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		const Ref<StyleBoxFlat> outlined_button_pressed_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, pressed_state_layer_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		Ref<StyleBoxFlat> outlined_focus_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		theme->set_stylebox("normal", "OutlinedButton", outlined_button_normal);
		theme->set_stylebox("hover", "OutlinedButton", outlined_button_hover);
		theme->set_stylebox("pressed", "OutlinedButton", outlined_button_pressed);
		theme->set_stylebox("disabled", "OutlinedButton", outlined_button_disabled);
		theme->set_stylebox("focus", "OutlinedButton", outlined_focus);

		theme->set_stylebox("hover_state_layer", "OutlinedButton",outlined_button_hover_state_layer);
		theme->set_stylebox("pressed_state_layer", "OutlinedButton", outlined_button_pressed_state_layer);
		theme->set_stylebox("focus_state_layer", "OutlinedButton", outlined_focus_state_layer);

		theme->set_font("font", "OutlinedButton", default_font);
		theme->set_font("text_icon_font", "OutlinedButton", default_icon_font);
		theme->set_font_size("font_size", "OutlinedButton", -1);
		theme->set_font_size("text_icon_font_size", "OutlinedButton", 18);
		theme->set_constant("outline_size", "OutlinedButton", 0);

		theme->set_color_scheme("default_color_scheme", "OutlinedButton", default_color_scheme);

		theme->set_color("font_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("font_focus_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("font_pressed_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("font_hover_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("font_hover_pressed_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("font_disabled_color_scale", "OutlinedButton", Color(one_color_scale, 0.38));
		theme->set_color("font_outline_color_scale", "OutlinedButton", one_color_scale);

		theme->set_color("icon_normal_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("icon_focus_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("icon_pressed_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("icon_hover_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("icon_hover_pressed_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("icon_disabled_color_scale", "OutlinedButton", Color(one_color_scale, 0.38));

		theme->set_color("text_icon_normal_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("text_icon_focus_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("text_icon_pressed_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("text_icon_hover_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("text_icon_hover_pressed_color_scale", "OutlinedButton", one_color_scale);
		theme->set_color("text_icon_disabled_color_scale", "OutlinedButton", Color(one_color_scale, 0.38));

		theme->set_color_scheme("font_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_focus_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_disabled_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_outline_color_scheme", "OutlinedButton", Ref<ColorScheme>());

		theme->set_color_scheme("icon_normal_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_focus_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_disabled_color_scheme", "OutlinedButton", Ref<ColorScheme>());

		theme->set_color_scheme("text_icon_normal_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_focus_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_disabled_color_scheme", "OutlinedButton", Ref<ColorScheme>());

		theme->set_color_role("font_color_role", "OutlinedButton", ColorRole::PRIMARY);
		theme->set_color_role("font_pressed_color_role", "OutlinedButton", ColorRole::PRIMARY);
		theme->set_color_role("font_hover_color_role", "OutlinedButton", ColorRole::PRIMARY);
		theme->set_color_role("font_focus_color_role", "OutlinedButton", ColorRole::PRIMARY);
		theme->set_color_role("font_hover_pressed_color_role", "OutlinedButton", ColorRole::PRIMARY);
		theme->set_color_role("font_disabled_color_role", "OutlinedButton", ColorRole::ON_SURFACE);
		theme->set_color_role("font_outline_color_role", "OutlinedButton", ColorRole::OUTLINE);

		theme->set_color_role("icon_normal_color_role", "OutlinedButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_pressed_color_role", "OutlinedButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_color_role", "OutlinedButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_pressed_color_role", "OutlinedButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_focus_color_role", "OutlinedButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_disabled_color_role", "OutlinedButton", ColorRole::STATIC_COLOR);

		theme->set_color_role("text_icon_normal_color_role", "OutlinedButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_pressed_color_role", "OutlinedButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_hover_color_role", "OutlinedButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_hover_pressed_color_role", "OutlinedButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_focus_color_role", "OutlinedButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_disabled_color_role", "OutlinedButton", ColorRole::ON_SURFACE);

		theme->set_color("font_color", "OutlinedButton", control_font_color);
		theme->set_color("font_pressed_color", "OutlinedButton", control_font_pressed_color);
		theme->set_color("font_hover_color", "OutlinedButton", control_font_hover_color);
		theme->set_color("font_focus_color", "OutlinedButton", control_font_focus_color);
		theme->set_color("font_hover_pressed_color", "OutlinedButton", control_font_pressed_color);
		theme->set_color("font_disabled_color", "OutlinedButton", control_font_disabled_color);
		theme->set_color("font_outline_color", "OutlinedButton", Color(1, 1, 1));

		theme->set_color("icon_normal_color", "OutlinedButton", Color(1, 1, 1, 1));
		theme->set_color("icon_pressed_color", "OutlinedButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_color", "OutlinedButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_pressed_color", "OutlinedButton", Color(1, 1, 1, 1));
		theme->set_color("icon_focus_color", "OutlinedButton", Color(1, 1, 1, 1));
		theme->set_color("icon_disabled_color", "OutlinedButton", Color(1, 1, 1, 0.4));

		theme->set_color("text_icon_normal_color", "OutlinedButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_pressed_color", "OutlinedButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_color", "OutlinedButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_pressed_color", "OutlinedButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_focus_color", "OutlinedButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_disabled_color", "OutlinedButton", Color(1, 1, 1, 0.4));

		theme->set_constant("h_separation", "OutlinedButton", Math::round(4 * scale));
		theme->set_constant("icon_max_width", "OutlinedButton", 0);
	}

	// TextButton
	{
		theme->set_type_variation("TextButton", "Button");

		Ref<StyleBoxFlat> text_button_normal = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		Ref<StyleBoxFlat> text_button_hover = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		Ref<StyleBoxFlat> text_button_pressed = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		Ref<StyleBoxFlat> text_button_disabled = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		Ref<StyleBoxFlat> text_focus = make_color_role_flat_stylebox(ColorRole::SECONDARY, style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		// Make the focus outline appear to be flush with the buttons it's focusing.
		text_focus->set_expand_margin_all(Math::round(2 * scale));

		const Ref<StyleBoxFlat> text_button_hover_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		const Ref<StyleBoxFlat> text_button_pressed_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, pressed_state_layer_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0);
		Ref<StyleBoxFlat> text_focus_state_layer = make_color_role_flat_stylebox(ColorRole::PRIMARY, focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

		theme->set_stylebox("normal", "TextButton", text_button_normal);
		theme->set_stylebox("hover", "TextButton", text_button_hover);
		theme->set_stylebox("pressed", "TextButton", text_button_pressed);
		theme->set_stylebox("disabled", "TextButton", text_button_disabled);
		theme->set_stylebox("focus", "TextButton", text_focus);

		theme->set_stylebox("hover_state_layer", "TextButton",text_button_hover_state_layer);
		theme->set_stylebox("pressed_state_layer", "TextButton", text_button_pressed_state_layer);
		theme->set_stylebox("focus_state_layer", "TextButton", text_focus_state_layer);

		theme->set_font("font", "TextButton", default_font);
		theme->set_font("text_icon_font", "TextButton", default_icon_font);
		theme->set_font_size("font_size", "TextButton", -1);
		theme->set_font_size("text_icon_font_size", "TextButton", 18);
		theme->set_constant("outline_size", "TextButton", 0);

		theme->set_color_scheme("default_color_scheme", "TextButton", default_color_scheme);

		theme->set_color("font_color_scale", "TextButton", one_color_scale);
		theme->set_color("font_focus_color_scale", "TextButton", one_color_scale);
		theme->set_color("font_pressed_color_scale", "TextButton", one_color_scale);
		theme->set_color("font_hover_color_scale", "TextButton", one_color_scale);
		theme->set_color("font_hover_pressed_color_scale", "TextButton", one_color_scale);
		theme->set_color("font_disabled_color_scale", "TextButton", Color(one_color_scale, 0.38));
		theme->set_color("font_outline_color_scale", "TextButton", one_color_scale);

		theme->set_color("icon_normal_color_scale", "TextButton", one_color_scale);
		theme->set_color("icon_focus_color_scale", "TextButton", one_color_scale);
		theme->set_color("icon_pressed_color_scale", "TextButton", one_color_scale);
		theme->set_color("icon_hover_color_scale", "TextButton", one_color_scale);
		theme->set_color("icon_hover_pressed_color_scale", "TextButton", one_color_scale);
		theme->set_color("icon_disabled_color_scale", "TextButton", Color(one_color_scale, 0.38));

		theme->set_color("text_icon_normal_color_scale", "TextButton", one_color_scale);
		theme->set_color("text_icon_focus_color_scale", "TextButton", one_color_scale);
		theme->set_color("text_icon_pressed_color_scale", "TextButton", one_color_scale);
		theme->set_color("text_icon_hover_color_scale", "TextButton", one_color_scale);
		theme->set_color("text_icon_hover_pressed_color_scale", "TextButton", one_color_scale);
		theme->set_color("text_icon_disabled_color_scale", "TextButton", Color(one_color_scale, 0.38));

		theme->set_color_scheme("font_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_focus_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_hover_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_disabled_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("font_outline_color_scheme", "TextButton", Ref<ColorScheme>());

		theme->set_color_scheme("icon_normal_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_focus_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_hover_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("icon_disabled_color_scheme", "TextButton", Ref<ColorScheme>());

		theme->set_color_scheme("text_icon_normal_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_focus_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
		theme->set_color_scheme("text_icon_disabled_color_scheme", "TextButton", Ref<ColorScheme>());

		theme->set_color_role("font_color_role", "TextButton", ColorRole::PRIMARY);
		theme->set_color_role("font_pressed_color_role", "TextButton", ColorRole::PRIMARY);
		theme->set_color_role("font_hover_color_role", "TextButton", ColorRole::PRIMARY);
		theme->set_color_role("font_focus_color_role", "TextButton", ColorRole::PRIMARY);
		theme->set_color_role("font_hover_pressed_color_role", "TextButton", ColorRole::PRIMARY);
		theme->set_color_role("font_disabled_color_role", "TextButton", ColorRole::ON_SURFACE);
		theme->set_color_role("font_outline_color_role", "TextButton", ColorRole::OUTLINE);

		theme->set_color_role("icon_normal_color_role", "TextButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_pressed_color_role", "TextButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_color_role", "TextButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_hover_pressed_color_role", "TextButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_focus_color_role", "TextButton", ColorRole::STATIC_COLOR);
		theme->set_color_role("icon_disabled_color_role", "TextButton", ColorRole::STATIC_COLOR);

		theme->set_color_role("text_icon_normal_color_role", "TextButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_pressed_color_role", "TextButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_hover_color_role", "TextButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_hover_pressed_color_role", "TextButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_focus_color_role", "TextButton", ColorRole::PRIMARY);
		theme->set_color_role("text_icon_disabled_color_role", "TextButton", ColorRole::ON_SURFACE);

		theme->set_color("font_color", "TextButton", control_font_color);
		theme->set_color("font_pressed_color", "TextButton", control_font_pressed_color);
		theme->set_color("font_hover_color", "TextButton", control_font_hover_color);
		theme->set_color("font_focus_color", "TextButton", control_font_focus_color);
		theme->set_color("font_hover_pressed_color", "TextButton", control_font_pressed_color);
		theme->set_color("font_disabled_color", "TextButton", control_font_disabled_color);
		theme->set_color("font_outline_color", "TextButton", Color(1, 1, 1));

		theme->set_color("icon_normal_color", "TextButton", Color(1, 1, 1, 1));
		theme->set_color("icon_pressed_color", "TextButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_color", "TextButton", Color(1, 1, 1, 1));
		theme->set_color("icon_hover_pressed_color", "TextButton", Color(1, 1, 1, 1));
		theme->set_color("icon_focus_color", "TextButton", Color(1, 1, 1, 1));
		theme->set_color("icon_disabled_color", "TextButton", Color(1, 1, 1, 0.4));

		theme->set_color("text_icon_normal_color", "TextButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_pressed_color", "TextButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_color", "TextButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_hover_pressed_color", "TextButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_focus_color", "TextButton", Color(1, 1, 1, 1));
		theme->set_color("text_icon_disabled_color", "TextButton", Color(1, 1, 1, 0.4));

		theme->set_constant("h_separation", "TextButton", Math::round(4 * scale));
		theme->set_constant("icon_max_width", "TextButton", 0);
	}


	// MenuBar
	theme->set_stylebox("normal", "MenuBar", button_normal);
	theme->set_stylebox("hover", "MenuBar", button_hover);
	theme->set_stylebox("pressed", "MenuBar", button_pressed);
	theme->set_stylebox("disabled", "MenuBar", button_disabled);

	theme->set_font("font", "MenuBar", Ref<Font>());
	theme->set_font_size("font_size", "MenuBar", -1);
	theme->set_constant("outline_size", "MenuBar", 0);

	theme->set_color_scheme("default_color_scheme", "MenuBar", default_color_scheme);

	theme->set_color("font_color_scale", "MenuBar", control_font_color_scale);
	theme->set_color("font_focus_color_scale", "MenuBar", control_font_focus_color_scale);
	theme->set_color("font_pressed_color_scale", "MenuBar", control_font_pressed_color_scale);
	theme->set_color("font_hover_color_scale", "MenuBar", control_font_hover_color_scale);
	theme->set_color("font_hover_pressed_color_scale", "MenuBar", control_font_hover_pressed_color_scale);
	theme->set_color("font_disabled_color_scale", "MenuBar", control_font_disabled_color_scale);
	theme->set_color("font_outline_color_scale", "MenuBar", control_font_outline_color_scale);

	theme->set_color_scheme("font_color_scheme", "MenuBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_pressed_color_scheme", "MenuBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_color_scheme", "MenuBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_focus_color_scheme", "MenuBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_pressed_color_scheme", "MenuBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "MenuBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "MenuBar", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "MenuBar", ColorRole::ON_SURFACE);
	theme->set_color_role("font_pressed_color_role", "MenuBar", ColorRole::ON_SURFACE);
	theme->set_color_role("font_hover_color_role", "MenuBar", ColorRole::ON_SURFACE);
	theme->set_color_role("font_focus_color_role", "MenuBar", ColorRole::ON_SURFACE);
	theme->set_color_role("font_hover_pressed_color_role", "MenuBar", ColorRole::ON_SURFACE);
	theme->set_color_role("font_disabled_color_role", "MenuBar", ColorRole::ON_SURFACE);
	theme->set_color_role("font_outline_color_role", "MenuBar", ColorRole::OUTLINE);

	theme->set_color("font_color", "MenuBar", control_font_color);
	theme->set_color("font_pressed_color", "MenuBar", control_font_pressed_color);
	theme->set_color("font_hover_color", "MenuBar", control_font_hover_color);
	theme->set_color("font_focus_color", "MenuBar", control_font_focus_color);
	theme->set_color("font_hover_pressed_color", "MenuBar", control_font_pressed_color);
	theme->set_color("font_disabled_color", "MenuBar", control_font_disabled_color);
	theme->set_color("font_outline_color", "MenuBar", Color(1, 1, 1));

	theme->set_constant("h_separation", "MenuBar", Math::round(4 * scale));

	// LinkButton

	theme->set_stylebox("focus", "LinkButton", focus);

	theme->set_font("font", "LinkButton", Ref<Font>());
	theme->set_font_size("font_size", "LinkButton", -1);

	theme->set_color_scheme("default_color_scheme", "LinkButton", default_color_scheme);

	theme->set_color_scheme("font_color_scheme", "LinkButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_pressed_color_scheme", "LinkButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_color_scheme", "LinkButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_focus_color_scheme", "LinkButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_pressed_color_scheme", "LinkButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "LinkButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "LinkButton", Ref<ColorScheme>());

	theme->set_color("font_color_scale", "LinkButton", control_font_color_scale);
	theme->set_color("font_focus_color_scale", "LinkButton", control_font_focus_color_scale);
	theme->set_color("font_pressed_color_scale", "LinkButton", control_font_pressed_color_scale);
	theme->set_color("font_hover_color_scale", "LinkButton", control_font_hover_color_scale);
	theme->set_color("font_hover_pressed_color_scale", "LinkButton", control_font_hover_pressed_color_scale);
	theme->set_color("font_disabled_color_scale", "LinkButton", control_font_disabled_color_scale);
	theme->set_color("font_outline_color_scale", "LinkButton", control_font_outline_color_scale);

	theme->set_color_role("font_color_role", "LinkButton", ColorRole::PRIMARY);
	theme->set_color_role("font_pressed_color_role", "LinkButton", ColorRole::PRIMARY);
	theme->set_color_role("font_hover_color_role", "LinkButton", ColorRole::PRIMARY);
	theme->set_color_role("font_focus_color_role", "LinkButton", ColorRole::PRIMARY);
	theme->set_color_role("font_hover_pressed_color_role", "LinkButton", ColorRole::PRIMARY);
	theme->set_color_role("font_disabled_color_role", "LinkButton", ColorRole::PRIMARY);
	theme->set_color_role("font_outline_color_role", "LinkButton", ColorRole::OUTLINE);

	theme->set_color("font_color", "LinkButton", control_font_color);
	theme->set_color("font_pressed_color", "LinkButton", control_font_pressed_color);
	theme->set_color("font_hover_color", "LinkButton", control_font_hover_color);
	theme->set_color("font_focus_color", "LinkButton", control_font_focus_color);
	theme->set_color("font_hover_pressed_color", "LinkButton", control_font_pressed_color);
	theme->set_color("font_disabled_color", "LinkButton", control_font_disabled_color);
	theme->set_color("font_outline_color", "LinkButton", Color(1, 1, 1));

	theme->set_constant("outline_size", "LinkButton", 0);
	theme->set_constant("underline_spacing", "LinkButton", Math::round(2 * scale));

	// OptionButton
	theme->set_stylebox("focus", "OptionButton", focus);

	Ref<StyleBox> sb_optbutton_normal = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 2 * default_margin, default_margin, 21, default_margin);
	Ref<StyleBox> sb_optbutton_hover = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_hover_color,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 2 * default_margin, default_margin, 21, default_margin);
	Ref<StyleBox> sb_optbutton_pressed = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_pressed_color,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 2 * default_margin, default_margin, 21, default_margin);
	Ref<StyleBox> sb_optbutton_disabled = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_disabled_color, StyleBoxFlat::ElevationLevel::Elevation_Level_0,2 * default_margin, default_margin, 21, default_margin);

	theme->set_stylebox("normal", "OptionButton", sb_optbutton_normal);
	theme->set_stylebox("hover", "OptionButton", sb_optbutton_hover);
	theme->set_stylebox("pressed", "OptionButton", sb_optbutton_pressed);
	theme->set_stylebox("disabled", "OptionButton", sb_optbutton_disabled);

	Ref<StyleBox> sb_optbutton_normal_mirrored = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 21, default_margin, 2 * default_margin, default_margin);
	Ref<StyleBox> sb_optbutton_hover_mirrored = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_hover_color,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 21, default_margin, 2 * default_margin, default_margin);
	Ref<StyleBox> sb_optbutton_pressed_mirrored = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_pressed_color,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 21, default_margin, 2 * default_margin, default_margin);
	Ref<StyleBox> sb_optbutton_disabled_mirrored = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_disabled_color,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 21, default_margin, 2 * default_margin, default_margin);

	theme->set_stylebox("normal_mirrored", "OptionButton", sb_optbutton_normal_mirrored);
	theme->set_stylebox("hover_mirrored", "OptionButton", sb_optbutton_hover_mirrored);
	theme->set_stylebox("pressed_mirrored", "OptionButton", sb_optbutton_pressed_mirrored);
	theme->set_stylebox("disabled_mirrored", "OptionButton", sb_optbutton_disabled_mirrored);

	theme->set_icon("arrow", "OptionButton", icons["option_button_arrow"]);

	theme->set_font("font", "OptionButton", Ref<Font>());
	theme->set_font_size("font_size", "OptionButton", -1);

	theme->set_color_scheme("default_color_scheme", "OptionButton", default_color_scheme);

	theme->set_color("font_color_scale", "OptionButton", control_font_color_scale);
	theme->set_color("font_pressed_color_scale", "OptionButton", control_font_pressed_color_scale);
	theme->set_color("font_hover_color_scale", "OptionButton", control_font_hover_color_scale);
	theme->set_color("font_focus_color_scale", "OptionButton", control_font_focus_color_scale);
	theme->set_color("font_hover_pressed_color_scale", "OptionButton", control_font_pressed_color_scale);
	theme->set_color("font_disabled_color_scale", "OptionButton", control_font_disabled_color_scale);
	theme->set_color("font_outline_color_scale", "OptionButton", Color(1, 1, 1));

	theme->set_color("font_color", "OptionButton", control_font_color);
	theme->set_color("font_pressed_color", "OptionButton", control_font_pressed_color);
	theme->set_color("font_hover_color", "OptionButton", control_font_hover_color);
	theme->set_color("font_hover_pressed_color", "OptionButton", control_font_pressed_color);
	theme->set_color("font_focus_color", "OptionButton", control_font_focus_color);
	theme->set_color("font_disabled_color", "OptionButton", control_font_disabled_color);
	theme->set_color("font_outline_color", "OptionButton", Color(1, 1, 1));

	theme->set_color_scheme("font_color_scheme", "OptionButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_pressed_color_scheme", "OptionButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_color_scheme", "OptionButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_focus_color_scheme", "OptionButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_pressed_color_scheme", "OptionButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "OptionButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "OptionButton", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "OptionButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_pressed_color_role", "OptionButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hover_color_role", "OptionButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_focus_color_role", "OptionButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hover_pressed_color_role", "OptionButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_disabled_color_role", "OptionButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_outline_color_role", "OptionButton", ColorRole::OUTLINE);

	theme->set_constant("h_separation", "OptionButton", Math::round(4 * scale));
	theme->set_constant("arrow_margin", "OptionButton", Math::round(4 * scale));
	theme->set_constant("outline_size", "OptionButton", 0);
	theme->set_constant("modulate_arrow", "OptionButton", false);

	// MenuButton

	theme->set_stylebox("normal", "MenuButton", button_normal);
	theme->set_stylebox("pressed", "MenuButton", button_pressed);
	theme->set_stylebox("hover", "MenuButton", button_hover);
	theme->set_stylebox("disabled", "MenuButton", button_disabled);
	theme->set_stylebox("focus", "MenuButton", focus);

	theme->set_font("font", "MenuButton", Ref<Font>());
	theme->set_font_size("font_size", "MenuButton", -1);

	theme->set_color_scheme("default_color_scheme", "MenuButton", default_color_scheme);

	theme->set_color("font_color_scale", "MenuButton", control_font_color_scale);
	theme->set_color("font_pressed_color_scale", "MenuButton", control_font_pressed_color_scale);
	theme->set_color("font_hover_color_scale", "MenuButton", control_font_hover_color_scale);
	theme->set_color("font_focus_color_scale", "MenuButton", control_font_focus_color_scale);
	theme->set_color("font_hover_pressed_color_scale", "MenuButton", control_font_pressed_color_scale);
	theme->set_color("font_disabled_color_scale", "MenuButton", control_font_disabled_color_scale);
	theme->set_color("font_outline_color_scale", "MenuButton", Color(1, 1, 1));

	theme->set_color("font_color", "MenuButton", control_font_color);
	theme->set_color("font_pressed_color", "MenuButton", control_font_pressed_color);
	theme->set_color("font_hover_color", "MenuButton", control_font_hover_color);
	theme->set_color("font_focus_color", "MenuButton", control_font_focus_color);
	theme->set_color("font_hover_pressed_color", "MenuButton", control_font_pressed_color);
	theme->set_color("font_disabled_color", "MenuButton", Color(1, 1, 1, 0.3));
	theme->set_color("font_outline_color", "MenuButton", Color(1, 1, 1));

	theme->set_color_scheme("font_color_scheme", "MenuButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_pressed_color_scheme", "MenuButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_color_scheme", "MenuButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_focus_color_scheme", "MenuButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_pressed_color_scheme", "MenuButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "MenuButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "MenuButton", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "MenuButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_pressed_color_role", "MenuButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hover_color_role", "MenuButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_focus_color_role", "MenuButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hover_pressed_color_role", "MenuButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_disabled_color_role", "MenuButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_outline_color_role", "MenuButton", ColorRole::OUTLINE);

	theme->set_constant("h_separation", "MenuButton", Math::round(4 * scale));
	theme->set_constant("outline_size", "MenuButton", 0);

	// CheckBox

	Ref<StyleBox> cbx_empty = memnew(StyleBoxEmpty);
	cbx_empty->set_content_margin_all(Math::round(4 * scale));
	Ref<StyleBox> cbx_focus = focus;
	cbx_focus->set_content_margin_all(Math::round(4 * scale));

	theme->set_stylebox("normal", "CheckBox", cbx_empty);
	theme->set_stylebox("pressed", "CheckBox", cbx_empty);
	theme->set_stylebox("disabled", "CheckBox", cbx_empty);
	theme->set_stylebox("hover", "CheckBox", cbx_empty);
	theme->set_stylebox("hover_pressed", "CheckBox", cbx_empty);
	theme->set_stylebox("focus", "CheckBox", cbx_focus);

	theme->set_icon("checked", "CheckBox", icons["checked"]);
	theme->set_icon("checked_disabled", "CheckBox", icons["checked"]);
	theme->set_icon("unchecked", "CheckBox", icons["unchecked"]);
	theme->set_icon("unchecked_disabled", "CheckBox", icons["unchecked"]);
	theme->set_icon("radio_checked", "CheckBox", icons["radio_checked"]);
	theme->set_icon("radio_checked_disabled", "CheckBox", icons["radio_checked"]);
	theme->set_icon("radio_unchecked", "CheckBox", icons["radio_unchecked"]);
	theme->set_icon("radio_unchecked_disabled", "CheckBox", icons["radio_unchecked"]);

	theme->set_font("font", "CheckBox", Ref<Font>());
	theme->set_font_size("font_size", "CheckBox", -1);

	theme->set_color_scheme("default_color_scheme", "CheckBox", default_color_scheme);

	theme->set_color("font_color_scale", "CheckBox", control_font_color_scale);
	theme->set_color("font_pressed_color_scale", "CheckBox", control_font_pressed_color_scale);
	theme->set_color("font_hover_color_scale", "CheckBox", control_font_hover_color_scale);
	theme->set_color("font_focus_color_scale", "CheckBox", control_font_focus_color_scale);
	theme->set_color("font_hover_pressed_color_scale", "CheckBox", control_font_pressed_color_scale);
	theme->set_color("font_disabled_color_scale", "CheckBox", control_font_disabled_color_scale);
	theme->set_color("font_outline_color_scale", "CheckBox", Color(1, 1, 1));

	theme->set_color("font_color", "CheckBox", control_font_color);
	theme->set_color("font_pressed_color", "CheckBox", control_font_pressed_color);
	theme->set_color("font_hover_color", "CheckBox", control_font_hover_color);
	theme->set_color("font_hover_pressed_color", "CheckBox", control_font_pressed_color);
	theme->set_color("font_focus_color", "CheckBox", control_font_focus_color);
	theme->set_color("font_disabled_color", "CheckBox", control_font_disabled_color);
	theme->set_color("font_outline_color", "CheckBox", Color(1, 1, 1));

	theme->set_color_scheme("font_color_scheme", "CheckBox", Ref<ColorScheme>());
	theme->set_color_scheme("font_pressed_color_scheme", "CheckBox", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_color_scheme", "CheckBox", Ref<ColorScheme>());
	theme->set_color_scheme("font_focus_color_scheme", "CheckBox", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_pressed_color_scheme", "CheckBox", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "CheckBox", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "CheckBox", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "CheckBox", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_pressed_color_role", "CheckBox", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hover_color_role", "CheckBox", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_focus_color_role", "CheckBox", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hover_pressed_color_role", "CheckBox", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_disabled_color_role", "CheckBox", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_outline_color_role", "CheckBox", ColorRole::OUTLINE);

	theme->set_constant("h_separation", "CheckBox", Math::round(4 * scale));
	theme->set_constant("check_v_offset", "CheckBox", 0);
	theme->set_constant("outline_size", "CheckBox", 0);

	// CheckButton

	Ref<StyleBox> cb_empty = memnew(StyleBoxEmpty);
	cb_empty->set_content_margin_individual(Math::round(6 * scale), Math::round(4 * scale), Math::round(6 * scale), Math::round(4 * scale));

	theme->set_stylebox("normal", "CheckButton", cb_empty);
	theme->set_stylebox("pressed", "CheckButton", cb_empty);
	theme->set_stylebox("disabled", "CheckButton", cb_empty);
	theme->set_stylebox("hover", "CheckButton", cb_empty);
	theme->set_stylebox("hover_pressed", "CheckButton", cb_empty);
	theme->set_stylebox("focus", "CheckButton", focus);

	theme->set_icon("checked", "CheckButton", icons["toggle_on"]);
	theme->set_icon("checked_disabled", "CheckButton", icons["toggle_on_disabled"]);
	theme->set_icon("unchecked", "CheckButton", icons["toggle_off"]);
	theme->set_icon("unchecked_disabled", "CheckButton", icons["toggle_off_disabled"]);

	theme->set_icon("checked_mirrored", "CheckButton", icons["toggle_on_mirrored"]);
	theme->set_icon("checked_disabled_mirrored", "CheckButton", icons["toggle_on_disabled_mirrored"]);
	theme->set_icon("unchecked_mirrored", "CheckButton", icons["toggle_off_mirrored"]);
	theme->set_icon("unchecked_disabled_mirrored", "CheckButton", icons["toggle_off_disabled_mirrored"]);

	theme->set_font("font", "CheckButton", Ref<Font>());
	theme->set_font_size("font_size", "CheckButton", -1);

	theme->set_color_scheme("default_color_scheme", "CheckButton", default_color_scheme);

	theme->set_color("font_color_scale", "CheckButton", control_font_color_scale);
	theme->set_color("font_pressed_color_scale", "CheckButton", control_font_pressed_color_scale);
	theme->set_color("font_hover_color_scale", "CheckButton", control_font_hover_color_scale);
	theme->set_color("font_focus_color_scale", "CheckButton", control_font_focus_color_scale);
	theme->set_color("font_hover_pressed_color_scale", "CheckButton", control_font_pressed_color_scale);
	theme->set_color("font_disabled_color_scale", "CheckButton", control_font_disabled_color_scale);
	theme->set_color("font_outline_color_scale", "CheckButton", Color(1, 1, 1));

	theme->set_color("font_color", "CheckButton", control_font_color);
	theme->set_color("font_pressed_color", "CheckButton", control_font_pressed_color);
	theme->set_color("font_hover_color", "CheckButton", control_font_hover_color);
	theme->set_color("font_hover_pressed_color", "CheckButton", control_font_pressed_color);
	theme->set_color("font_focus_color", "CheckButton", control_font_focus_color);
	theme->set_color("font_disabled_color", "CheckButton", control_font_disabled_color);
	theme->set_color("font_outline_color", "CheckButton", Color(1, 1, 1));

	theme->set_color_scheme("font_color_scheme", "CheckButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_pressed_color_scheme", "CheckButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_color_scheme", "CheckButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_focus_color_scheme", "CheckButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_pressed_color_scheme", "CheckButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "CheckButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "CheckButton", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "CheckButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_pressed_color_role", "CheckButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hover_color_role", "CheckButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_focus_color_role", "CheckButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hover_pressed_color_role", "CheckButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_disabled_color_role", "CheckButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_outline_color_role", "CheckButton", ColorRole::OUTLINE);

	theme->set_constant("h_separation", "CheckButton", Math::round(4 * scale));
	theme->set_constant("check_v_offset", "CheckButton", 0);
	theme->set_constant("outline_size", "CheckButton", 0);

	// Button variations

	theme->set_type_variation("FlatButton", "Button");
	theme->set_type_variation("FlatMenuButton", "MenuButton");

	Ref<StyleBoxEmpty> flat_button_normal = make_empty_stylebox();
	for (int i = 0; i < 4; i++) {
		flat_button_normal->set_content_margin((Side)i, button_normal->get_margin((Side)i) + button_normal->get_border_width((Side)i));
	}
	Ref<StyleBoxFlat> flat_button_pressed = button_pressed->duplicate();
	flat_button_pressed->set_bg_color_scale(Color(1, 1, 1, 0.6) * Color(1, 1, 1, 0.85));

	theme->set_stylebox("normal", "FlatButton", flat_button_normal);
	theme->set_stylebox("hover", "FlatButton", flat_button_normal);
	theme->set_stylebox("pressed", "FlatButton", flat_button_pressed);
	theme->set_stylebox("disabled", "FlatButton", flat_button_normal);

	theme->set_stylebox("normal", "FlatMenuButton", flat_button_normal);
	theme->set_stylebox("hover", "FlatMenuButton", flat_button_normal);
	theme->set_stylebox("pressed", "FlatMenuButton", flat_button_pressed);
	theme->set_stylebox("disabled", "FlatMenuButton", flat_button_normal);

	// Label

	theme->set_stylebox("normal", "Label", memnew(StyleBoxEmpty));
	theme->set_font("font", "Label", Ref<Font>());
	theme->set_font_size("font_size", "Label", -1);

	theme->set_color_scheme("default_color_scheme", "Label", default_color_scheme);

	theme->set_color("font_color_scale", "Label", control_font_color_scale);
	theme->set_color("font_shadow_color_scale", "Label", Color(1, 1, 1, 1));
	theme->set_color("font_outline_color_scale", "Label", Color(1, 1, 1));

	theme->set_color_scheme("font_color_scheme", "Label", Ref<ColorScheme>());
	theme->set_color_scheme("font_shadow_color_scheme", "Label", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "Label", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "Label", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_shadow_color_role", "Label", ColorRole::SHADOW);
	theme->set_color_role("font_outline_color_role", "Label", ColorRole::OUTLINE);

	theme->set_color("font_color", "Label", Color(1, 1, 1));
	theme->set_color("font_shadow_color", "Label", Color(0, 0, 0, 0));
	theme->set_color("font_outline_color", "Label", Color(1, 1, 1));

	theme->set_constant("shadow_offset_x", "Label", Math::round(1 * scale));
	theme->set_constant("shadow_offset_y", "Label", Math::round(1 * scale));
	theme->set_constant("outline_size", "Label", 0);
	theme->set_constant("shadow_outline_size", "Label", Math::round(1 * scale));
	theme->set_constant("line_spacing", "Label", Math::round(3 * scale));

	theme->set_type_variation("HeaderSmall", "Label");
	theme->set_font_size("font_size", "HeaderSmall", default_font_size + 4);

	theme->set_type_variation("HeaderMedium", "Label");
	theme->set_font_size("font_size", "HeaderMedium", default_font_size + 8);

	theme->set_type_variation("HeaderLarge", "Label");
	theme->set_font_size("font_size", "HeaderLarge", default_font_size + 12);

	// LineEdit

	Ref<StyleBoxFlat> style_line_edit = make_color_role_flat_stylebox(ColorRole::PRIMARY_CONTAINER, style_normal_color_scale);
	// Add a line at the bottom to make LineEdits distinguishable from Buttons.
	style_line_edit->set_border_width(SIDE_BOTTOM, 2);
	style_line_edit->set_border_color(style_pressed_color);
	style_line_edit->set_border_color_scale(style_pressed_color_scale);
	theme->set_stylebox("normal", "LineEdit", style_line_edit);

	theme->set_stylebox("focus", "LineEdit", focus);

	Ref<StyleBoxFlat> style_line_edit_read_only = make_color_role_flat_stylebox(ColorRole::PRIMARY_CONTAINER, style_disabled_color_scale);
	// Add a line at the bottom to make LineEdits distinguishable from Buttons.
	style_line_edit_read_only->set_border_width(SIDE_BOTTOM, 2);
	style_line_edit_read_only->set_border_color(style_pressed_color * Color(1, 1, 1, 0.5));
	style_line_edit_read_only->set_border_color_scale(style_pressed_color_scale * Color(1, 1, 1, 0.5));
	theme->set_stylebox("read_only", "LineEdit", style_line_edit_read_only);

	theme->set_font("font", "LineEdit", Ref<Font>());
	theme->set_font_size("font_size", "LineEdit", -1);

	theme->set_color_scheme("default_color_scheme", "LineEdit", default_color_scheme);

	theme->set_color("font_color_scale", "LineEdit", control_font_color_scale);
	theme->set_color("font_selected_color_scale", "LineEdit", control_font_pressed_color_scale);
	theme->set_color("font_uneditable_color_scale", "LineEdit", control_font_disabled_color_scale);
	theme->set_color("font_placeholder_color_scale", "LineEdit", control_font_placeholder_color_scale);
	theme->set_color("font_outline_color_scale", "LineEdit", Color(1, 1, 1));
	theme->set_color("caret_color_scale", "LineEdit", control_font_hover_color_scale);
	theme->set_color("selection_color_scale", "LineEdit", control_selection_color_scale);
	theme->set_color("clear_button_color_scale", "LineEdit", control_font_color_scale);
	theme->set_color("clear_button_color_pressed_scale", "LineEdit", control_font_pressed_color_scale);

	theme->set_color_scheme("font_color_scheme", "LineEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_selected_color_scheme", "LineEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_uneditable_color_scheme", "LineEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_placeholder_color_scheme", "LineEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "LineEdit", Ref<ColorScheme>());
	theme->set_color_scheme("caret_color_scheme", "LineEdit", Ref<ColorScheme>());
	theme->set_color_scheme("selection_color_scheme", "LineEdit", Ref<ColorScheme>());
	theme->set_color_scheme("clear_button_color_scheme", "LineEdit", Ref<ColorScheme>());
	theme->set_color_scheme("clear_button_color_pressed_scheme", "LineEdit", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "LineEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_selected_color_role", "LineEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_uneditable_color_role", "LineEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_placeholder_color_role", "LineEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_outline_color_role", "LineEdit", ColorRole::OUTLINE);
	theme->set_color_role("caret_color_role", "LineEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("selection_color_role", "LineEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("clear_button_color_role", "LineEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("clear_button_color_pressed_role", "LineEdit", ColorRole::ON_PRIMARY_CONTAINER);

	theme->set_color("font_color", "LineEdit", control_font_color);
	theme->set_color("font_selected_color", "LineEdit", control_font_pressed_color);
	theme->set_color("font_uneditable_color", "LineEdit", control_font_disabled_color);
	theme->set_color("font_placeholder_color", "LineEdit", control_font_placeholder_color);
	theme->set_color("font_outline_color", "LineEdit", Color(1, 1, 1));
	theme->set_color("caret_color", "LineEdit", control_font_hover_color);
	theme->set_color("selection_color", "LineEdit", control_selection_color);
	theme->set_color("clear_button_color", "LineEdit", control_font_color);
	theme->set_color("clear_button_color_pressed", "LineEdit", control_font_pressed_color);

	theme->set_constant("minimum_character_width", "LineEdit", 4);
	theme->set_constant("outline_size", "LineEdit", 0);
	theme->set_constant("caret_width", "LineEdit", 1);

	theme->set_icon("clear", "LineEdit", icons["line_edit_clear"]);

	// ProgressBar

	theme->set_stylebox("background", "ProgressBar", make_color_role_flat_stylebox(ColorRole::INVERSE_PRIMARY, Color(1, 1, 1, 1),StyleBoxFlat::ElevationLevel::Elevation_Level_0, 2, 2, 2, 2, 6));
	theme->set_stylebox("fill", "ProgressBar", make_color_role_flat_stylebox(ColorRole::PRIMARY, Color(1, 1, 1, 1),StyleBoxFlat::ElevationLevel::Elevation_Level_0, 2, 2, 2, 2, 6));

	theme->set_font("font", "ProgressBar", Ref<Font>());
	theme->set_font_size("font_size", "ProgressBar", -1);

	theme->set_color_scheme("default_color_scheme", "ProgressBar", default_color_scheme);

	theme->set_color("font_color_scale", "ProgressBar", control_font_hover_color_scale);
	theme->set_color("font_outline_color_scale", "ProgressBar", Color(1, 1, 1));

	theme->set_color_scheme("font_color_scheme", "ProgressBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "ProgressBar", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "ProgressBar", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_outline_color_role", "ProgressBar", ColorRole::OUTLINE);

	theme->set_color("font_color", "ProgressBar", control_font_hover_color);
	theme->set_color("font_outline_color", "ProgressBar", Color(1, 1, 1));

	theme->set_constant("outline_size", "ProgressBar", 0);

	// TextEdit

	theme->set_stylebox("normal", "TextEdit", style_line_edit);
	theme->set_stylebox("focus", "TextEdit", focus);
	theme->set_stylebox("read_only", "TextEdit", style_line_edit_read_only);

	theme->set_color_scheme("default_color_scheme", "TextEdit", default_color_scheme);

	theme->set_color("background_color_scale", "TextEdit", Color(1, 1, 1, 1));
	theme->set_color("font_color_scale", "TextEdit", control_font_color_scale);
	theme->set_color("font_selected_color_scale", "TextEdit", control_font_pressed_color_scale);
	theme->set_color("font_readonly_color_scale", "TextEdit", control_font_disabled_color_scale);
	theme->set_color("font_placeholder_color_scale", "TextEdit", control_font_placeholder_color_scale);
	theme->set_color("font_outline_color_scale", "TextEdit", Color(1, 1, 1));
	theme->set_color("selection_color_scale", "TextEdit", control_selection_color_scale);
	theme->set_color("current_line_color_scale", "TextEdit", Color(0.5, 0.5, 0.5, 0.8));
	theme->set_color("caret_color_scale", "TextEdit", control_font_hover_color_scale);
	theme->set_color("caret_background_color_scale", "TextEdit", Color(1, 1, 1));
	theme->set_color("word_highlighted_color_scale", "TextEdit", control_font_focus_color_scale);
	theme->set_color("search_result_color_scale", "TextEdit", Color(0.4, 0.4, 0.4));
	theme->set_color("search_result_border_color_scale", "TextEdit", Color(0.4, 0.4, 0.4, 0.5));

	theme->set_icon("tab", "TextEdit", icons["text_edit_tab"]);
	theme->set_icon("space", "TextEdit", icons["text_edit_space"]);

	theme->set_font("font", "TextEdit", Ref<Font>());
	theme->set_font_size("font_size", "TextEdit", -1);

	theme->set_color_scheme("search_result_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("search_result_border_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("caret_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("caret_background_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_selected_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("selection_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_readonly_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_placeholder_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("background_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("current_line_color_scheme", "TextEdit", Ref<ColorScheme>());
	theme->set_color_scheme("word_highlighted_color_scheme", "TextEdit", Ref<ColorScheme>());

	theme->set_color_role("background_color_role", "TextEdit", ColorRole::PRIMARY_CONTAINER);
	theme->set_color_role("font_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_selected_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_readonly_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_placeholder_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_outline_color_role", "TextEdit", ColorRole::OUTLINE);
	theme->set_color_role("selection_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("current_line_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("caret_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("caret_background_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("word_highlighted_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("search_result_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("search_result_border_color_role", "TextEdit", ColorRole::ON_PRIMARY_CONTAINER);

	theme->set_color("background_color", "TextEdit", Color(0, 0, 0, 0));
	theme->set_color("font_color", "TextEdit", control_font_color);
	theme->set_color("font_selected_color", "TextEdit", Color(0, 0, 0, 0));
	theme->set_color("font_readonly_color", "TextEdit", control_font_disabled_color);
	theme->set_color("font_placeholder_color", "TextEdit", control_font_placeholder_color);
	theme->set_color("font_outline_color", "TextEdit", Color(1, 1, 1));
	theme->set_color("selection_color", "TextEdit", control_selection_color);
	theme->set_color("current_line_color", "TextEdit", Color(0.25, 0.25, 0.26, 0.8));
	theme->set_color("caret_color", "TextEdit", control_font_color);
	theme->set_color("caret_background_color", "TextEdit", Color(0, 0, 0));
	theme->set_color("word_highlighted_color", "TextEdit", Color(0.5, 0.5, 0.5, 0.25));
	theme->set_color("search_result_color", "TextEdit", Color(0.3, 0.3, 0.3));
	theme->set_color("search_result_border_color", "TextEdit", Color(0.3, 0.3, 0.3, 0.4));

	theme->set_constant("line_spacing", "TextEdit", Math::round(4 * scale));
	theme->set_constant("outline_size", "TextEdit", 0);
	theme->set_constant("caret_width", "TextEdit", 1);

	// CodeEdit

	theme->set_stylebox("normal", "CodeEdit", style_line_edit);
	theme->set_stylebox("focus", "CodeEdit", focus);
	theme->set_stylebox("read_only", "CodeEdit", style_line_edit_read_only);
	theme->set_stylebox("completion", "CodeEdit", make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0));

	theme->set_color_scheme("default_color_scheme", "CodeEdit", default_color_scheme);

	theme->set_icon("tab", "CodeEdit", icons["text_edit_tab"]);
	theme->set_icon("space", "CodeEdit", icons["text_edit_space"]);
	theme->set_icon("breakpoint", "CodeEdit", icons["breakpoint"]);
	theme->set_icon("bookmark", "CodeEdit", icons["bookmark"]);
	theme->set_icon("executing_line", "CodeEdit", icons["arrow_right"]);
	theme->set_icon("can_fold", "CodeEdit", icons["arrow_down"]);
	theme->set_icon("folded", "CodeEdit", icons["arrow_right"]);
	theme->set_icon("can_fold_code_region", "CodeEdit", icons["region_unfolded"]);
	theme->set_icon("folded_code_region", "CodeEdit", icons["region_folded"]);
	theme->set_icon("folded_eol_icon", "CodeEdit", icons["text_edit_ellipsis"]);

	theme->set_font("font", "CodeEdit", Ref<Font>());
	theme->set_font_size("font_size", "CodeEdit", -1);

	theme->set_color_scheme("background_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("completion_background_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("completion_selected_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("completion_existing_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("completion_scroll_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("completion_scroll_hovered_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_selected_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_readonly_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_placeholder_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("selection_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("bookmark_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("breakpoint_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("executing_line_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("current_line_color_scheme", "CodeEdit", Ref<ColorScheme>());

	theme->set_color_scheme("code_folding_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("folded_code_region_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("caret_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("caret_background_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("brace_mismatch_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("line_number_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("word_highlighted_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("line_length_guideline_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("search_result_color_scheme", "CodeEdit", Ref<ColorScheme>());
	theme->set_color_scheme("search_result_border_color_scheme", "CodeEdit", Ref<ColorScheme>());

	theme->set_color_role("background_color_role", "CodeEdit", ColorRole::PRIMARY_CONTAINER);
	theme->set_color_role("font_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_selected_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_readonly_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_placeholder_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_outline_color_role", "CodeEdit", ColorRole::OUTLINE);
	theme->set_color_role("selection_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("current_line_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("caret_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("caret_background_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("word_highlighted_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("search_result_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("search_result_border_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);

	theme->set_color_role("completion_background_color_role", "CodeEdit", ColorRole::PRIMARY_CONTAINER);
	theme->set_color_role("completion_selected_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("completion_existing_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("completion_scroll_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("completion_scroll_hovered_color_role", "CodeEdit", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("bookmark_color_role", "CodeEdit", ColorRole::SECONDARY);
	theme->set_color_role("breakpoint_color_role", "CodeEdit", ColorRole::ERROR);
	theme->set_color_role("executing_line_color_role", "CodeEdit", ColorRole::SECONDARY);
	theme->set_color_role("code_folding_color_role", "CodeEdit", ColorRole::SECONDARY);
	theme->set_color_role("folded_code_region_color_role", "CodeEdit", ColorRole::SECONDARY);
	theme->set_color_role("brace_mismatch_color_role", "CodeEdit", ColorRole::SECONDARY);
	theme->set_color_role("line_number_color_role", "CodeEdit", ColorRole::ON_SECONDARY);
	theme->set_color_role("line_length_guideline_color_role", "CodeEdit", ColorRole::ON_SECONDARY);

	theme->set_color("background_color_scale", "CodeEdit", Color(1, 1, 1, 1));
	theme->set_color("font_color_scale", "CodeEdit", control_font_color_scale);
	theme->set_color("font_selected_color_scale", "CodeEdit", control_font_pressed_color_scale);
	theme->set_color("font_readonly_color_scale", "CodeEdit", control_font_disabled_color_scale);
	theme->set_color("font_placeholder_color_scale", "CodeEdit", control_font_placeholder_color_scale);
	theme->set_color("font_outline_color_scale", "CodeEdit", Color(1, 1, 1));
	theme->set_color("selection_color_scale", "CodeEdit", control_selection_color_scale);
	theme->set_color("current_line_color_scale", "CodeEdit", Color(0.5, 0.5, 0.5, 0.8));
	theme->set_color("caret_color_scale", "CodeEdit", control_font_hover_color_scale);
	theme->set_color("caret_background_color_scale", "CodeEdit", Color(1, 1, 1));
	theme->set_color("word_highlighted_color_scale", "CodeEdit", control_font_focus_color_scale);
	theme->set_color("search_result_color_scale", "CodeEdit", Color(0.4, 0.4, 0.4));
	theme->set_color("search_result_border_color_scale", "CodeEdit", Color(0.4, 0.4, 0.4, 0.5));

	theme->set_color("completion_background_color_scale", "CodeEdit", Color(0.5, 0.5, 0.5));
	theme->set_color("completion_selected_color_scale", "CodeEdit", Color(0.5, 0.5, 0.5));
	theme->set_color("completion_existing_color_scale", "CodeEdit", Color(1.2, 1.2, 1.2, 0.13));
	theme->set_color("completion_scroll_color_scale", "CodeEdit", Color(1, 1, 1, 0.29));
	theme->set_color("completion_scroll_hovered_color_scale", "CodeEdit", Color(1, 1, 1, 0.4));
	theme->set_color("bookmark_color_scale", "CodeEdit", Color(1, 1, 1, 0.8));
	theme->set_color("breakpoint_color_scale", "CodeEdit", Color(1, 1, 1));
	theme->set_color("executing_line_color_scale", "CodeEdit", Color(1, 1, 1));
	theme->set_color("code_folding_color_scale", "CodeEdit", Color(1, 1, 1, 0.8));
	theme->set_color("folded_code_region_color_scale", "CodeEdit", Color(1, 1, 1, 0.2));
	theme->set_color("brace_mismatch_color_scale", "CodeEdit", Color(1, 1, 1));
	theme->set_color("line_number_color_scale", "CodeEdit", Color(1, 1, 1, 0.4));
	theme->set_color("line_length_guideline_color_scale", "CodeEdit", Color(1, 1, 1, 0.1));

	theme->set_color("background_color", "CodeEdit", Color(0, 0, 0, 0));
	theme->set_color("completion_background_color", "CodeEdit", Color(0.17, 0.16, 0.2));
	theme->set_color("completion_selected_color", "CodeEdit", Color(0.26, 0.26, 0.27));
	theme->set_color("completion_existing_color", "CodeEdit", Color(0.87, 0.87, 0.87, 0.13));
	theme->set_color("completion_scroll_color", "CodeEdit", control_font_pressed_color * Color(1, 1, 1, 0.29));
	theme->set_color("completion_scroll_hovered_color", "CodeEdit", control_font_pressed_color * Color(1, 1, 1, 0.4));
	theme->set_color("font_color", "CodeEdit", control_font_color);
	theme->set_color("font_selected_color", "CodeEdit", Color(0, 0, 0, 0));
	theme->set_color("font_readonly_color", "CodeEdit", Color(control_font_color.r, control_font_color.g, control_font_color.b, 0.5f));
	theme->set_color("font_placeholder_color", "CodeEdit", control_font_placeholder_color);
	theme->set_color("font_outline_color", "CodeEdit", Color(1, 1, 1));
	theme->set_color("selection_color", "CodeEdit", control_selection_color);
	theme->set_color("bookmark_color", "CodeEdit", Color(0.5, 0.64, 1, 0.8));
	theme->set_color("breakpoint_color", "CodeEdit", Color(0.9, 0.29, 0.3));
	theme->set_color("executing_line_color", "CodeEdit", Color(0.98, 0.89, 0.27));
	theme->set_color("current_line_color", "CodeEdit", Color(0.25, 0.25, 0.26, 0.8));
	theme->set_color("code_folding_color", "CodeEdit", Color(0.8, 0.8, 0.8, 0.8));
	theme->set_color("folded_code_region_color", "CodeEdit", Color(0.68, 0.46, 0.77, 0.2));
	theme->set_color("caret_color", "CodeEdit", control_font_color);
	theme->set_color("caret_background_color", "CodeEdit", Color(0, 0, 0));
	theme->set_color("brace_mismatch_color", "CodeEdit", Color(1, 0.2, 0.2));
	theme->set_color("line_number_color", "CodeEdit", Color(0.67, 0.67, 0.67, 0.4));
	theme->set_color("word_highlighted_color", "CodeEdit", Color(0.8, 0.9, 0.9, 0.15));
	theme->set_color("line_length_guideline_color", "CodeEdit", Color(0.3, 0.5, 0.8, 0.1));
	theme->set_color("search_result_color", "CodeEdit", Color(0.3, 0.3, 0.3));
	theme->set_color("search_result_border_color", "CodeEdit", Color(0.3, 0.3, 0.3, 0.4));

	theme->set_constant("completion_lines", "CodeEdit", 7);
	theme->set_constant("completion_max_width", "CodeEdit", 50);
	theme->set_constant("completion_scroll_width", "CodeEdit", 6);
	theme->set_constant("line_spacing", "CodeEdit", Math::round(4 * scale));
	theme->set_constant("outline_size", "CodeEdit", 0);

	Ref<Texture2D> empty_icon = memnew(ImageTexture);

	const Ref<StyleBoxFlat> style_h_scrollbar = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 4, 0, 4, 10);
	const Ref<StyleBoxFlat> style_v_scrollbar = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 0, 4, 0, 10);
	Ref<StyleBoxFlat> style_scrollbar_grabber = make_color_role_flat_stylebox(ColorRole::INVERSE_PRIMARY, style_progress_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 10);
	Ref<StyleBoxFlat> style_scrollbar_grabber_highlight = make_color_role_flat_stylebox(ColorRole::INVERSE_PRIMARY, style_focus_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 10);
	Ref<StyleBoxFlat> style_scrollbar_grabber_pressed = make_color_role_flat_stylebox(ColorRole::INVERSE_PRIMARY, style_focus_color_scale * Color(0.75, 0.75, 0.75, 1), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 10);

	// HScrollBar

	theme->set_stylebox("scroll", "HScrollBar", style_h_scrollbar);
	theme->set_stylebox("scroll_focus", "HScrollBar", focus);
	theme->set_stylebox("grabber", "HScrollBar", style_scrollbar_grabber);
	theme->set_stylebox("grabber_highlight", "HScrollBar", style_scrollbar_grabber_highlight);
	theme->set_stylebox("grabber_pressed", "HScrollBar", style_scrollbar_grabber_pressed);

	theme->set_icon("increment", "HScrollBar", empty_icon);
	theme->set_icon("increment_highlight", "HScrollBar", empty_icon);
	theme->set_icon("increment_pressed", "HScrollBar", empty_icon);
	theme->set_icon("decrement", "HScrollBar", empty_icon);
	theme->set_icon("decrement_highlight", "HScrollBar", empty_icon);
	theme->set_icon("decrement_pressed", "HScrollBar", empty_icon);

	// VScrollBar

	theme->set_stylebox("scroll", "VScrollBar", style_v_scrollbar);
	theme->set_stylebox("scroll_focus", "VScrollBar", focus);
	theme->set_stylebox("grabber", "VScrollBar", style_scrollbar_grabber);
	theme->set_stylebox("grabber_highlight", "VScrollBar", style_scrollbar_grabber_highlight);
	theme->set_stylebox("grabber_pressed", "VScrollBar", style_scrollbar_grabber_pressed);

	theme->set_icon("increment", "VScrollBar", empty_icon);
	theme->set_icon("increment_highlight", "VScrollBar", empty_icon);
	theme->set_icon("increment_pressed", "VScrollBar", empty_icon);
	theme->set_icon("decrement", "VScrollBar", empty_icon);
	theme->set_icon("decrement_highlight", "VScrollBar", empty_icon);
	theme->set_icon("decrement_pressed", "VScrollBar", empty_icon);

	const Ref<StyleBoxFlat> style_slider = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 4);
	const Ref<StyleBoxFlat> style_slider_grabber = make_color_role_flat_stylebox(ColorRole::INVERSE_PRIMARY, style_progress_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 4);
	const Ref<StyleBoxFlat> style_slider_grabber_highlight = make_color_role_flat_stylebox(ColorRole::INVERSE_PRIMARY, style_focus_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0,4, 4, 4, 4, 4);

	// HSlider

	theme->set_stylebox("slider", "HSlider", style_slider);
	theme->set_stylebox("grabber_area", "HSlider", style_slider_grabber);
	theme->set_stylebox("grabber_area_highlight", "HSlider", style_slider_grabber_highlight);

	theme->set_icon("grabber", "HSlider", icons["slider_grabber"]);
	theme->set_icon("grabber_highlight", "HSlider", icons["slider_grabber_hl"]);
	theme->set_icon("grabber_disabled", "HSlider", icons["slider_grabber_disabled"]);
	theme->set_icon("tick", "HSlider", icons["hslider_tick"]);

	theme->set_constant("center_grabber", "HSlider", 0);
	theme->set_constant("grabber_offset", "HSlider", 0);

	// VSlider

	theme->set_stylebox("slider", "VSlider", style_slider);
	theme->set_stylebox("grabber_area", "VSlider", style_slider_grabber);
	theme->set_stylebox("grabber_area_highlight", "VSlider", style_slider_grabber_highlight);

	theme->set_icon("grabber", "VSlider", icons["slider_grabber"]);
	theme->set_icon("grabber_highlight", "VSlider", icons["slider_grabber_hl"]);
	theme->set_icon("grabber_disabled", "VSlider", icons["slider_grabber_disabled"]);
	theme->set_icon("tick", "VSlider", icons["vslider_tick"]);

	theme->set_constant("center_grabber", "VSlider", 0);
	theme->set_constant("grabber_offset", "VSlider", 0);

	// SpinBox

	theme->set_icon("updown", "SpinBox", icons["updown"]);

	// ScrollContainer

	Ref<StyleBoxEmpty> empty;
	empty.instantiate();
	theme->set_stylebox("panel", "ScrollContainer", empty);

	// Window

	theme->set_stylebox("embedded_border", "Window", sb_expand(make_color_role_flat_stylebox(ColorRole::PRIMARY, style_popup_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 10, 28, 10, 8), 8, 32, 8, 6));
	theme->set_stylebox("embedded_unfocused_border", "Window", sb_expand(make_color_role_flat_stylebox(ColorRole::PRIMARY, style_popup_hover_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 10, 28, 10, 8), 8, 32, 8, 6));

	theme->set_color_scheme("default_color_scheme", "Window", default_color_scheme);

	theme->set_color("title_color_scale", "Window", control_font_color_scale);
	theme->set_color("title_outline_modulate_scale", "Window", Color(1, 1, 1));

	theme->set_color_scheme("title_color_scheme", "Window", Ref<ColorScheme>());
	theme->set_color_scheme("title_outline_modulate_scheme", "Window", Ref<ColorScheme>());

	theme->set_color_role("title_color_role", "Window", ColorRole::ON_PRIMARY);
	theme->set_color_role("title_outline_modulate_role", "Window", ColorRole::OUTLINE);

	theme->set_font("title_font", "Window", Ref<Font>());
	theme->set_font_size("title_font_size", "Window", -1);
	theme->set_color("title_color", "Window", control_font_color);
	theme->set_color("title_outline_modulate", "Window", Color(1, 1, 1));
	theme->set_constant("title_outline_size", "Window", 0);
	theme->set_constant("title_height", "Window", 36 * scale);
	theme->set_constant("resize_margin", "Window", Math::round(4 * scale));

	theme->set_icon("close", "Window", icons["close"]);
	theme->set_icon("close_pressed", "Window", icons["close_hl"]);
	theme->set_constant("close_h_offset", "Window", 18 * scale);
	theme->set_constant("close_v_offset", "Window", 24 * scale);

	// Dialogs

	// AcceptDialog is currently the base dialog, so this defines styles for all extending nodes.
	theme->set_stylebox("panel", "AcceptDialog", make_color_role_flat_stylebox(ColorRole::PRIMARY_CONTAINER, style_popup_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, Math::round(8 * scale), Math::round(8 * scale), Math::round(8 * scale), Math::round(8 * scale)));
	theme->set_constant("buttons_separation", "AcceptDialog", Math::round(10 * scale));

	// File Dialog

	theme->set_icon("parent_folder", "FileDialog", icons["folder_up"]);
	theme->set_icon("back_folder", "FileDialog", icons["arrow_left"]);
	theme->set_icon("forward_folder", "FileDialog", icons["arrow_right"]);
	theme->set_icon("reload", "FileDialog", icons["reload"]);
	theme->set_icon("toggle_hidden", "FileDialog", icons["visibility_visible"]);
	theme->set_icon("folder", "FileDialog", icons["folder"]);
	theme->set_icon("file", "FileDialog", icons["file"]);

	theme->set_color_scheme("default_color_scheme", "FileDialog", default_color_scheme);

	theme->set_color_scheme("folder_icon_color_scheme", "FileDialog", Ref<ColorScheme>());
	theme->set_color_scheme("file_icon_color_scheme", "FileDialog", Ref<ColorScheme>());
	theme->set_color_scheme("file_disabled_color_scheme", "FileDialog", Ref<ColorScheme>());
	theme->set_color_scheme("icon_normal_color_scheme", "FileDialog", Ref<ColorScheme>());
	theme->set_color_scheme("icon_hover_color_scheme", "FileDialog", Ref<ColorScheme>());
	theme->set_color_scheme("icon_focus_color_scheme", "FileDialog", Ref<ColorScheme>());
	theme->set_color_scheme("icon_pressed_color_scheme", "FileDialog", Ref<ColorScheme>());

	theme->set_color_role("folder_icon_color_role", "FileDialog", ColorRole::ON_PRIMARY);
	theme->set_color_role("file_icon_color_role", "FileDialog", ColorRole::ON_PRIMARY);
	theme->set_color_role("file_disabled_color_role", "FileDialog", ColorRole::ON_PRIMARY);
	theme->set_color_role("icon_normal_color_role", "FileDialog", ColorRole::ON_PRIMARY);
	theme->set_color_role("icon_hover_color_role", "FileDialog", ColorRole::ON_PRIMARY);
	theme->set_color_role("icon_focus_color_role", "FileDialog", ColorRole::ON_PRIMARY);
	theme->set_color_role("icon_pressed_color_role", "FileDialog", ColorRole::ON_PRIMARY);

	theme->set_color("folder_icon_color", "FileDialog", Color(1, 1, 1));
	theme->set_color("file_icon_color", "FileDialog", Color(1, 1, 1));
	theme->set_color("file_disabled_color", "FileDialog", Color(1, 1, 1, 0.25));
	theme->set_color("icon_normal_color", "FileDialog", Color(1, 1, 1, 0.25));
	theme->set_color("icon_hover_color", "FileDialog", Color(1, 1, 1, 0.25));
	theme->set_color("icon_focus_color", "FileDialog", Color(1, 1, 1, 0.25));
	theme->set_color("icon_pressed_color", "FileDialog", Color(1, 1, 1, 0.25));

	theme->set_color("folder_icon_color_scale", "FileDialog", Color(1, 1, 1));
	theme->set_color("file_icon_color_scale", "FileDialog", Color(1, 1, 1));
	theme->set_color("file_disabled_color_scale", "FileDialog", Color(1, 1, 1, 0.25));
	theme->set_color("icon_normal_color_scale", "FileDialog", Color(1, 1, 1, 0.25));
	theme->set_color("icon_hover_color_scale", "FileDialog", Color(1, 1, 1, 0.25));
	theme->set_color("icon_focus_color_scale", "FileDialog", Color(1, 1, 1, 0.25));
	theme->set_color("icon_pressed_color_scale", "FileDialog", Color(1, 1, 1, 0.25));

	// Popup

	theme->set_stylebox("panel", "PopupPanel", make_color_role_flat_stylebox(ColorRole::PRIMARY_CONTAINER, style_normal_color_scale));

	// PopupDialog

	theme->set_stylebox("panel", "PopupDialog", make_color_role_flat_stylebox(ColorRole::PRIMARY_CONTAINER, style_normal_color_scale));

	// PopupMenu

	Ref<StyleBoxLine> separator_horizontal = memnew(StyleBoxLine);
	separator_horizontal->set_thickness(Math::round(scale));
	separator_horizontal->set_color(style_separator_color);
	separator_horizontal->set_color_scale(style_separator_color_scale);
	separator_horizontal->set_content_margin_individual(default_margin, 0, default_margin, 0);
	separator_horizontal->set_color_scheme(default_color_scheme);
	separator_horizontal->set_color_role(ColorRole::SURFACE_CONTAINER_HIGHEST);

	Ref<StyleBoxLine> separator_vertical = separator_horizontal->duplicate();
	separator_vertical->set_vertical(true);
	separator_vertical->set_content_margin_individual(0, default_margin, 0, default_margin);

	// Always display a border for PopupMenus so they can be distinguished from their background.
	Ref<StyleBoxFlat> style_popup_panel = make_color_role_flat_stylebox(ColorRole::SURFACE_CONTAINER, style_popup_color_scale);
	style_popup_panel->set_border_width_all(2);
	style_popup_panel->set_border_color_scale(Color(1.0, 1.0, 1.0));

	theme->set_stylebox("panel", "PopupMenu", style_popup_panel);
	theme->set_stylebox("hover", "PopupMenu", make_color_role_flat_stylebox(ColorRole::SURFACE_CONTAINER, style_popup_hover_color_scale));
	theme->set_stylebox("separator", "PopupMenu", separator_horizontal);
	theme->set_stylebox("labeled_separator_left", "PopupMenu", separator_horizontal);
	theme->set_stylebox("labeled_separator_right", "PopupMenu", separator_horizontal);

	theme->set_icon("checked", "PopupMenu", icons["checked"]);
	theme->set_icon("checked_disabled", "PopupMenu", icons["checked"]);
	theme->set_icon("unchecked", "PopupMenu", icons["unchecked"]);
	theme->set_icon("unchecked_disabled", "PopupMenu", icons["unchecked"]);
	theme->set_icon("radio_checked", "PopupMenu", icons["radio_checked"]);
	theme->set_icon("radio_checked_disabled", "PopupMenu", icons["radio_checked"]);
	theme->set_icon("radio_unchecked", "PopupMenu", icons["radio_unchecked"]);
	theme->set_icon("radio_unchecked_disabled", "PopupMenu", icons["radio_unchecked"]);
	theme->set_icon("submenu", "PopupMenu", icons["popup_menu_arrow_right"]);
	theme->set_icon("submenu_mirrored", "PopupMenu", icons["popup_menu_arrow_left"]);

	theme->set_font("font", "PopupMenu", Ref<Font>());
	theme->set_font("font_separator", "PopupMenu", Ref<Font>());
	theme->set_font_size("font_size", "PopupMenu", -1);
	theme->set_font_size("font_separator_size", "PopupMenu", -1);

	theme->set_color_scheme("default_color_scheme", "PopupMenu", default_color_scheme);

	theme->set_color("font_color_scale", "PopupMenu", control_font_color_scale);
	theme->set_color("font_accelerator_color_scale", "PopupMenu", Color(0.9, 0.9, 0.9, 0.8));
	theme->set_color("font_disabled_color_scale", "PopupMenu", Color(0.45, 0.45, 0.45, 0.8));
	theme->set_color("font_hover_color_scale", "PopupMenu", Color(1, 1, 1, 1));
	theme->set_color("font_separator_color_scale", "PopupMenu", Color(1, 1, 1, 1));
	theme->set_color("font_outline_color_scale", "PopupMenu", Color(1, 1, 1));
	theme->set_color("font_separator_outline_color_scale", "PopupMenu", Color(1, 1, 1));

	theme->set_color_scheme("font_color_scheme", "PopupMenu", Ref<ColorScheme>());
	theme->set_color_scheme("font_accelerator_color_scheme", "PopupMenu", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "PopupMenu", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_color_scheme", "PopupMenu", Ref<ColorScheme>());
	theme->set_color_scheme("font_separator_color_scheme", "PopupMenu", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "PopupMenu", Ref<ColorScheme>());
	theme->set_color_scheme("font_separator_outline_color_scheme", "PopupMenu", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "PopupMenu", ColorRole::ON_SURFACE);
	theme->set_color_role("font_accelerator_color_role", "PopupMenu", ColorRole::ON_SURFACE);
	theme->set_color_role("font_disabled_color_role", "PopupMenu", ColorRole::ON_SURFACE);
	theme->set_color_role("font_hover_color_role", "PopupMenu", ColorRole::ON_SURFACE);
	theme->set_color_role("font_separator_color_role", "PopupMenu", ColorRole::ON_SURFACE);
	theme->set_color_role("font_outline_color_role", "PopupMenu", ColorRole::OUTLINE);
	theme->set_color_role("font_separator_outline_color_role", "PopupMenu", ColorRole::OUTLINE_VARIANT);

	theme->set_color("font_color", "PopupMenu", control_font_color);
	theme->set_color("font_accelerator_color", "PopupMenu", Color(0.7, 0.7, 0.7, 0.8));
	theme->set_color("font_disabled_color", "PopupMenu", Color(0.4, 0.4, 0.4, 0.8));
	theme->set_color("font_hover_color", "PopupMenu", control_font_color);
	theme->set_color("font_separator_color", "PopupMenu", control_font_color);
	theme->set_color("font_outline_color", "PopupMenu", Color(1, 1, 1));
	theme->set_color("font_separator_outline_color", "PopupMenu", Color(1, 1, 1));

	theme->set_constant("indent", "PopupMenu", Math::round(10 * scale));
	theme->set_constant("h_separation", "PopupMenu", Math::round(4 * scale));
	theme->set_constant("v_separation", "PopupMenu", Math::round(4 * scale));
	theme->set_constant("outline_size", "PopupMenu", 0);
	theme->set_constant("separator_outline_size", "PopupMenu", 0);
	theme->set_constant("item_start_padding", "PopupMenu", Math::round(2 * scale));
	theme->set_constant("item_end_padding", "PopupMenu", Math::round(2 * scale));
	theme->set_constant("icon_max_width", "PopupMenu", 0);

	// GraphNode

	Ref<StyleBoxFlat> graphnode_normal = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0,18, 12, 18, 12);
	graphnode_normal->set_border_color(Color(0.325, 0.325, 0.325, 0.6));
	graphnode_normal->set_border_color_scale(Color(0.325, 0.325, 0.325, 0.6));
	Ref<StyleBoxFlat> graphnode_selected = graphnode_normal->duplicate();
	graphnode_selected->set_border_color(Color(0.625, 0.625, 0.625, 0.6));
	graphnode_selected->set_border_color_scale(Color(0.625, 0.625, 0.625, 0.6));

	Ref<StyleBoxFlat> graphn_sb_titlebar = make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color_scale.lightened(0.3),StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4);
	Ref<StyleBoxFlat> graphn_sb_titlebar_selected = graphnode_normal->duplicate();
	graphn_sb_titlebar_selected->set_bg_color(Color(1.0, 0.625, 0.625, 0.6));
	graphn_sb_titlebar_selected->set_bg_color_scale(Color(1.0, 0.625, 0.625, 0.6));
	Ref<StyleBoxEmpty> graphnode_slot = make_empty_stylebox(0, 0, 0, 0);

	theme->set_stylebox("panel", "GraphNode", graphnode_normal);
	theme->set_stylebox("panel_selected", "GraphNode", graphnode_selected);
	theme->set_stylebox("titlebar", "GraphNode", graphn_sb_titlebar);
	theme->set_stylebox("titlebar_selected", "GraphNode", graphn_sb_titlebar_selected);
	theme->set_stylebox("slot", "GraphNode", graphnode_slot);
	theme->set_icon("port", "GraphNode", icons["graph_port"]);
	theme->set_icon("resizer", "GraphNode", icons["resizer_se"]);

	theme->set_color_scheme("default_color_scheme", "GraphNode", default_color_scheme);
	theme->set_color_scheme("resizer_color_scheme", "GraphNode", Ref<ColorScheme>());
	theme->set_color_role("resizer_color_role", "GraphNode", ColorRole::PRIMARY);
	theme->set_color("resizer_color", "GraphNode", control_font_color);
	theme->set_color("resizer_color_scale", "GraphNode", control_font_color_scale);
	theme->set_constant("separation", "GraphNode", Math::round(2 * scale));
	theme->set_constant("port_h_offset", "GraphNode", 0);

	// GraphNodes's title Label.

	theme->set_type_variation("GraphNodeTitleLabel", "Label");

	theme->set_stylebox("normal", "GraphNodeTitleLabel", make_empty_stylebox(0, 0, 0, 0));
	theme->set_font("font", "GraphNodeTitleLabel", Ref<Font>());
	theme->set_font_size("font_size", "GraphNodeTitleLabel", -1);

	theme->set_color_scheme("default_color_scheme", "GraphNodeTitleLabel", default_color_scheme);

	theme->set_color_scheme("font_color_scheme", "GraphNodeTitleLabel", Ref<ColorScheme>());
	theme->set_color_scheme("font_shadow_color_scheme", "GraphNodeTitleLabel", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "GraphNodeTitleLabel", Ref<ColorScheme>());

	theme->set_color("font_color_scale", "GraphNodeTitleLabel", control_font_color_scale);
	theme->set_color("font_shadow_color_scale", "GraphNodeTitleLabel", Color(1, 1, 1, 1));
	theme->set_color("font_outline_color_scale", "GraphNodeTitleLabel", control_font_color_scale);

	theme->set_color_role("font_color_role", "GraphNodeTitleLabel", ColorRole::PRIMARY);
	theme->set_color_role("font_shadow_color_role", "GraphNodeTitleLabel", ColorRole::SHADOW);
	theme->set_color_role("font_outline_color_role", "GraphNodeTitleLabel", ColorRole::PRIMARY);

	theme->set_color("font_color", "GraphNodeTitleLabel", control_font_color);
	theme->set_color("font_shadow_color", "GraphNodeTitleLabel", Color(0, 0, 0, 0));
	theme->set_color("font_outline_color", "GraphNodeTitleLabel", control_font_color);

	theme->set_constant("shadow_offset_x", "GraphNodeTitleLabel", Math::round(1 * scale));
	theme->set_constant("shadow_offset_y", "GraphNodeTitleLabel", Math::round(1 * scale));
	theme->set_constant("outline_size", "GraphNodeTitleLabel", 0);
	theme->set_constant("shadow_outline_size", "GraphNodeTitleLabel", Math::round(1 * scale));
	theme->set_constant("line_spacing", "GraphNodeTitleLabel", Math::round(3 * scale));

	// Tree

	theme->set_color_scheme("default_color_scheme", "Tree", default_color_scheme);

	theme->set_stylebox("panel", "Tree", make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 5));
	theme->set_stylebox("focus", "Tree", focus);
	theme->set_stylebox("selected", "Tree", make_color_role_flat_stylebox(ColorRole::PRIMARY, style_selected_color_scale));
	theme->set_stylebox("selected_focus", "Tree", make_color_role_flat_stylebox(ColorRole::PRIMARY, style_selected_color_scale));
	theme->set_stylebox("cursor", "Tree", focus);
	theme->set_stylebox("cursor_unfocused", "Tree", focus);
	theme->set_stylebox("button_pressed", "Tree", button_pressed);
	theme->set_stylebox("title_button_normal", "Tree", make_color_role_flat_stylebox(ColorRole::PRIMARY, style_pressed_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0,4, 4, 4, 4));
	theme->set_stylebox("title_button_pressed", "Tree", make_color_role_flat_stylebox(ColorRole::PRIMARY, style_hover_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4));
	theme->set_stylebox("title_button_hover", "Tree", make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0,4, 4, 4, 4));
	theme->set_stylebox("custom_button", "Tree", button_normal);
	theme->set_stylebox("custom_button_pressed", "Tree", button_pressed);
	theme->set_stylebox("custom_button_hover", "Tree", button_hover);

	theme->set_icon("checked", "Tree", icons["checked"]);
	theme->set_icon("unchecked", "Tree", icons["unchecked"]);
	theme->set_icon("indeterminate", "Tree", icons["indeterminate"]);
	theme->set_icon("updown", "Tree", icons["updown"]);
	theme->set_icon("select_arrow", "Tree", icons["option_button_arrow"]);
	theme->set_icon("arrow", "Tree", icons["arrow_down"]);
	theme->set_icon("arrow_collapsed", "Tree", icons["arrow_right"]);
	theme->set_icon("arrow_collapsed_mirrored", "Tree", icons["arrow_left"]);

	theme->set_font("title_button_font", "Tree", Ref<Font>());
	theme->set_font("font", "Tree", Ref<Font>());
	theme->set_font_size("font_size", "Tree", -1);
	theme->set_font_size("title_button_font_size", "Tree", -1);

	theme->set_color_scheme("title_button_color_scheme", "Tree", Ref<ColorScheme>());
	theme->set_color_scheme("font_color_scheme", "Tree", Ref<ColorScheme>());
	theme->set_color_scheme("font_selected_color_scheme", "Tree", Ref<ColorScheme>());
	theme->set_color_scheme("guide_color_scheme", "Tree", Ref<ColorScheme>());
	theme->set_color_scheme("drop_position_color_scheme", "Tree", Ref<ColorScheme>());
	theme->set_color_scheme("relationship_line_color_scheme", "Tree", Ref<ColorScheme>());
	theme->set_color_scheme("parent_hl_line_color_scheme", "Tree", Ref<ColorScheme>());
	theme->set_color_scheme("children_hl_line_color_scheme", "Tree", Ref<ColorScheme>());
	theme->set_color_scheme("custom_button_font_highlight_scheme", "Tree", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "Tree", Ref<ColorScheme>());

	theme->set_color_role("title_button_color_role", "Tree", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_color_role", "Tree", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_selected_color_role", "Tree", ColorRole::ON_PRIMARY);
	theme->set_color_role("guide_color_role", "Tree", ColorRole::ON_PRIMARY);
	theme->set_color_role("drop_position_color_role", "Tree", ColorRole::ON_PRIMARY);
	theme->set_color_role("relationship_line_color_role", "Tree", ColorRole::ON_PRIMARY);
	theme->set_color_role("parent_hl_line_color_role", "Tree", ColorRole::ON_PRIMARY);
	theme->set_color_role("children_hl_line_color_role", "Tree", ColorRole::ON_PRIMARY);
	theme->set_color_role("custom_button_font_highlight_role", "Tree", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_outline_color_role", "Tree", ColorRole::OUTLINE);

	theme->set_color("title_button_color_scale", "Tree", control_font_color_scale);
	theme->set_color("font_color_scale", "Tree", control_font_low_color_scale);
	theme->set_color("font_selected_color_scale", "Tree", control_font_pressed_color_scale);
	theme->set_color("font_outline_color_scale", "Tree", Color(1, 1, 1));
	theme->set_color("guide_color_scale", "Tree", Color(0.7, 0.7, 0.7, 0.25));
	theme->set_color("drop_position_color_scale", "Tree", Color(1, 1, 1));
	theme->set_color("relationship_line_color_scale", "Tree", Color(0.27, 0.27, 0.27));
	theme->set_color("parent_hl_line_color_scale", "Tree", Color(0.27, 0.27, 0.27));
	theme->set_color("children_hl_line_color_scale", "Tree", Color(0.27, 0.27, 0.27));
	theme->set_color("custom_button_font_highlight_scale", "Tree", control_font_hover_color_scale);

	theme->set_color("title_button_color", "Tree", control_font_color);
	theme->set_color("font_color", "Tree", control_font_low_color);
	theme->set_color("font_selected_color", "Tree", control_font_pressed_color);
	theme->set_color("font_outline_color", "Tree", Color(1, 1, 1));
	theme->set_color("guide_color", "Tree", Color(0.7, 0.7, 0.7, 0.25));
	theme->set_color("drop_position_color", "Tree", Color(1, 1, 1));
	theme->set_color("relationship_line_color", "Tree", Color(0.27, 0.27, 0.27));
	theme->set_color("parent_hl_line_color", "Tree", Color(0.27, 0.27, 0.27));
	theme->set_color("children_hl_line_color", "Tree", Color(0.27, 0.27, 0.27));
	theme->set_color("custom_button_font_highlight", "Tree", control_font_hover_color);

	theme->set_constant("h_separation", "Tree", Math::round(4 * scale));
	theme->set_constant("v_separation", "Tree", Math::round(4 * scale));
	theme->set_constant("item_margin", "Tree", Math::round(16 * scale));
	theme->set_constant("inner_item_margin_bottom", "Tree", 0);
	theme->set_constant("inner_item_margin_left", "Tree", 0);
	theme->set_constant("inner_item_margin_right", "Tree", 0);
	theme->set_constant("inner_item_margin_top", "Tree", 0);
	theme->set_constant("button_margin", "Tree", Math::round(4 * scale));
	theme->set_constant("draw_relationship_lines", "Tree", 0);
	theme->set_constant("relationship_line_width", "Tree", 1);
	theme->set_constant("parent_hl_line_width", "Tree", 1);
	theme->set_constant("children_hl_line_width", "Tree", 1);
	theme->set_constant("parent_hl_line_margin", "Tree", 0);
	theme->set_constant("draw_guides", "Tree", 1);
	theme->set_constant("scroll_border", "Tree", Math::round(4 * scale));
	theme->set_constant("scroll_speed", "Tree", 12);
	theme->set_constant("outline_size", "Tree", 0);
	theme->set_constant("icon_max_width", "Tree", 0);
	theme->set_constant("scrollbar_margin_left", "Tree", -1);
	theme->set_constant("scrollbar_margin_top", "Tree", -1);
	theme->set_constant("scrollbar_margin_right", "Tree", -1);
	theme->set_constant("scrollbar_margin_bottom", "Tree", -1);
	theme->set_constant("scrollbar_h_separation", "Tree", Math::round(4 * scale));
	theme->set_constant("scrollbar_v_separation", "Tree", Math::round(4 * scale));

	// ItemList

	theme->set_stylebox("panel", "ItemList", make_color_role_flat_stylebox(ColorRole::PRIMARY, style_normal_color));
	theme->set_stylebox("focus", "ItemList", focus);
	theme->set_constant("h_separation", "ItemList", Math::round(4 * scale));
	theme->set_constant("v_separation", "ItemList", Math::round(2 * scale));
	theme->set_constant("icon_margin", "ItemList", Math::round(4 * scale));
	theme->set_constant("line_separation", "ItemList", Math::round(2 * scale));

	theme->set_font("font", "ItemList", Ref<Font>());
	theme->set_font_size("font_size", "ItemList", -1);

	theme->set_color_scheme("default_color_scheme", "ItemList", default_color_scheme);

	theme->set_color_scheme("font_color_scheme", "ItemList", Ref<ColorScheme>());
	theme->set_color_scheme("font_hovered_color_scheme", "ItemList", Ref<ColorScheme>());
	theme->set_color_scheme("font_selected_color_scheme", "ItemList", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "ItemList", Ref<ColorScheme>());
	theme->set_color_scheme("guide_color_scheme", "ItemList", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "ItemList", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hovered_color_role", "ItemList", ColorRole::SHADOW);
	theme->set_color_role("font_selected_color_role", "ItemList", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_outline_color_role", "ItemList", ColorRole::OUTLINE);
	theme->set_color_role("guide_color_role", "ItemList", ColorRole::SHADOW);

	theme->set_color("font_color_scale", "ItemList", control_font_lower_color_scale);
	theme->set_color("font_hovered_color_scale", "ItemList", control_font_hover_color_scale);
	theme->set_color("font_selected_color_scale", "ItemList", control_font_pressed_color_scale);
	theme->set_color("font_outline_color_scale", "ItemList", Color(1, 1, 1));
	theme->set_color("guide_color_scale", "ItemList", Color(0.7, 0.7, 0.7, 0.25));

	theme->set_color("font_color", "ItemList", control_font_lower_color);
	theme->set_color("font_hovered_color", "ItemList", control_font_hover_color);
	theme->set_color("font_selected_color", "ItemList", control_font_pressed_color);
	theme->set_color("font_outline_color", "ItemList", Color(1, 1, 1));
	theme->set_color("guide_color", "ItemList", Color(0.7, 0.7, 0.7, 0.25));

	theme->set_stylebox("hovered", "ItemList", make_color_role_flat_stylebox(ColorRole::PRIMARY, Color(1, 1, 1, 0.07)));
	theme->set_stylebox("selected", "ItemList", make_color_role_flat_stylebox(ColorRole::PRIMARY, style_selected_color_scale));
	theme->set_stylebox("selected_focus", "ItemList", make_color_role_flat_stylebox(ColorRole::PRIMARY, style_selected_color_scale));
	theme->set_stylebox("cursor", "ItemList", focus);
	theme->set_stylebox("cursor_unfocused", "ItemList", focus);

	theme->set_constant("outline_size", "ItemList", 0);

	// TabContainer

	Ref<StyleBoxFlat> style_tab_selected = make_color_role_flat_stylebox(ColorRole::SECONDARY_CONTAINER, style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0,10, 4, 10, 4, 0);
	style_tab_selected->set_border_width(SIDE_TOP, Math::round(2 * scale));
	style_tab_selected->set_border_color(style_focus_color);
	style_tab_selected->set_border_color_scale(style_focus_color_scale);
	Ref<StyleBoxFlat> style_tab_unselected = make_color_role_flat_stylebox(ColorRole::SECONDARY_CONTAINER, style_pressed_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 10, 4, 10, 4, 0);
	// Add some spacing between unselected tabs to make them easier to distinguish from each other.
	style_tab_unselected->set_border_width(SIDE_LEFT, Math::round(scale));
	style_tab_unselected->set_border_width(SIDE_RIGHT, Math::round(scale));
	style_tab_unselected->set_border_color(style_popup_border_color);
	style_tab_unselected->set_border_color_scale(style_popup_border_color_scale);
	Ref<StyleBoxFlat> style_tab_disabled = style_tab_unselected->duplicate();
	style_tab_disabled->set_bg_color(style_disabled_color);
	style_tab_disabled->set_bg_color_scale(style_disabled_color_scale);
	Ref<StyleBoxFlat> style_tab_hovered = style_tab_unselected->duplicate();
	style_tab_hovered->set_bg_color(Color(0.1, 0.1, 0.1, 0.3));
	style_tab_hovered->set_bg_color_scale(Color(0.1, 0.1, 0.1, 0.3));
	Ref<StyleBoxFlat> style_tab_focus = focus->duplicate();

	theme->set_stylebox("tab_selected", "TabContainer", style_tab_selected);
	theme->set_stylebox("tab_hovered", "TabContainer", style_tab_hovered);
	theme->set_stylebox("tab_unselected", "TabContainer", style_tab_unselected);
	theme->set_stylebox("tab_disabled", "TabContainer", style_tab_disabled);
	theme->set_stylebox("tab_focus", "TabContainer", style_tab_focus);
	theme->set_stylebox("panel", "TabContainer", make_color_role_flat_stylebox(ColorRole::PRIMARY_CONTAINER, style_normal_color,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0));
	theme->set_stylebox("tabbar_background", "TabContainer", make_empty_stylebox(0, 0, 0, 0));

	theme->set_icon("increment", "TabContainer", icons["scroll_button_right"]);
	theme->set_icon("increment_highlight", "TabContainer", icons["scroll_button_right_hl"]);
	theme->set_icon("decrement", "TabContainer", icons["scroll_button_left"]);
	theme->set_icon("decrement_highlight", "TabContainer", icons["scroll_button_left_hl"]);
	theme->set_icon("drop_mark", "TabContainer", icons["tabs_drop_mark"]);
	theme->set_icon("menu", "TabContainer", icons["tabs_menu"]);
	theme->set_icon("menu_highlight", "TabContainer", icons["tabs_menu_hl"]);

	theme->set_font("font", "TabContainer", Ref<Font>());
	theme->set_font_size("font_size", "TabContainer", -1);

	theme->set_color_scheme("default_color_scheme", "TabContainer", default_color_scheme);

	theme->set_color_scheme("drop_mark_color_scheme", "TabContainer", Ref<ColorScheme>());
	theme->set_color_scheme("font_selected_color_scheme", "TabContainer", Ref<ColorScheme>());
	theme->set_color_scheme("font_hovered_color_scheme", "TabContainer", Ref<ColorScheme>());
	theme->set_color_scheme("font_unselected_color_scheme", "TabContainer", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "TabContainer", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "TabContainer", Ref<ColorScheme>());

	theme->set_color_role("drop_mark_color_role", "TabContainer", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_selected_color_role", "TabContainer", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_hovered_color_role", "TabContainer", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_unselected_color_role", "TabContainer", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_disabled_color_role", "TabContainer", ColorRole::ON_PRIMARY_CONTAINER);
	theme->set_color_role("font_outline_color_role", "TabContainer", ColorRole::OUTLINE);

	theme->set_color("font_selected_color_scale", "TabContainer", control_font_hover_color_scale);
	theme->set_color("font_hovered_color_scale", "TabContainer", control_font_hover_color_scale);
	theme->set_color("font_unselected_color_scale", "TabContainer", control_font_low_color_scale);
	theme->set_color("font_disabled_color_scale", "TabContainer", control_font_disabled_color_scale);
	theme->set_color("font_outline_color_scale", "TabContainer", Color(1, 1, 1));
	theme->set_color("drop_mark_color_scale", "TabContainer", Color(1, 1, 1));

	theme->set_color("font_selected_color", "TabContainer", control_font_hover_color);
	theme->set_color("font_hovered_color", "TabContainer", control_font_hover_color);
	theme->set_color("font_unselected_color", "TabContainer", control_font_low_color);
	theme->set_color("font_disabled_color", "TabContainer", control_font_disabled_color);
	theme->set_color("font_outline_color", "TabContainer", Color(1, 1, 1));
	theme->set_color("drop_mark_color", "TabContainer", Color(1, 1, 1));

	theme->set_constant("side_margin", "TabContainer", Math::round(8 * scale));
	theme->set_constant("icon_separation", "TabContainer", Math::round(4 * scale));
	theme->set_constant("icon_max_width", "TabContainer", 0);
	theme->set_constant("outline_size", "TabContainer", 0);

	// TabBar

	theme->set_stylebox("tab_selected", "TabBar", style_tab_selected);
	theme->set_stylebox("tab_hovered", "TabBar", style_tab_hovered);
	theme->set_stylebox("tab_unselected", "TabBar", style_tab_unselected);
	theme->set_stylebox("tab_disabled", "TabBar", style_tab_disabled);
	theme->set_stylebox("tab_focus", "TabBar", style_tab_focus);
	theme->set_stylebox("button_pressed", "TabBar", button_pressed);
	theme->set_stylebox("button_highlight", "TabBar", button_normal);

	theme->set_icon("increment", "TabBar", icons["scroll_button_right"]);
	theme->set_icon("increment_highlight", "TabBar", icons["scroll_button_right_hl"]);
	theme->set_icon("decrement", "TabBar", icons["scroll_button_left"]);
	theme->set_icon("decrement_highlight", "TabBar", icons["scroll_button_left_hl"]);
	theme->set_icon("drop_mark", "TabBar", icons["tabs_drop_mark"]);
	theme->set_icon("close", "TabBar", icons["close"]);

	theme->set_font("font", "TabBar", Ref<Font>());
	theme->set_font_size("font_size", "TabBar", -1);

	theme->set_color_scheme("default_color_scheme", "TabBar", default_color_scheme);

	theme->set_color_scheme("drop_mark_color_scheme", "TabBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_selected_color_scheme", "TabBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_hovered_color_scheme", "TabBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_unselected_color_scheme", "TabBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "TabBar", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "TabBar", Ref<ColorScheme>());

	theme->set_color_role("drop_mark_color_role", "TabBar", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_selected_color_role", "TabBar", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hovered_color_role", "TabBar", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_unselected_color_role", "TabBar", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_disabled_color_role", "TabBar", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_outline_color_role", "TabBar", ColorRole::SHADOW);

	theme->set_color("font_selected_color_scale", "TabBar", control_font_hover_color_scale);
	theme->set_color("font_hovered_color_scale", "TabBar", control_font_hover_color_scale);
	theme->set_color("font_unselected_color_scale", "TabBar", control_font_low_color_scale);
	theme->set_color("font_disabled_color_scale", "TabBar", control_font_disabled_color_scale);
	theme->set_color("font_outline_color_scale", "TabBar", Color(1, 1, 1));
	theme->set_color("drop_mark_color_scale", "TabBar", Color(1, 1, 1));

	theme->set_color("font_selected_color", "TabBar", control_font_hover_color);
	theme->set_color("font_hovered_color", "TabBar", control_font_hover_color);
	theme->set_color("font_unselected_color", "TabBar", control_font_low_color);
	theme->set_color("font_disabled_color", "TabBar", control_font_disabled_color);
	theme->set_color("font_outline_color", "TabBar", Color(1, 1, 1));
	theme->set_color("drop_mark_color", "TabBar", Color(1, 1, 1));

	theme->set_constant("h_separation", "TabBar", Math::round(4 * scale));
	theme->set_constant("icon_max_width", "TabBar", 0);
	theme->set_constant("outline_size", "TabBar", 0);

	// Separators
	theme->set_stylebox("separator", "HSeparator", separator_horizontal);
	theme->set_stylebox("separator", "VSeparator", separator_vertical);

	theme->set_icon("close", "Icons", icons["close"]);
	theme->set_font("normal", "Fonts", Ref<Font>());
	theme->set_font("large", "Fonts", Ref<Font>());

	theme->set_constant("separation", "HSeparator", Math::round(4 * scale));
	theme->set_constant("separation", "VSeparator", Math::round(4 * scale));

	// ColorPicker

	theme->set_constant("margin", "ColorPicker", Math::round(4 * scale));
	theme->set_constant("sv_width", "ColorPicker", Math::round(256 * scale));
	theme->set_constant("sv_height", "ColorPicker", Math::round(256 * scale));
	theme->set_constant("h_width", "ColorPicker", Math::round(30 * scale));
	theme->set_constant("label_width", "ColorPicker", Math::round(10 * scale));
	theme->set_constant("center_slider_grabbers", "ColorPicker", 1);

	theme->set_icon("folded_arrow", "ColorPicker", icons["arrow_right"]);
	theme->set_icon("expanded_arrow", "ColorPicker", icons["arrow_down"]);
	theme->set_icon("screen_picker", "ColorPicker", icons["color_picker_pipette"]);
	theme->set_icon("shape_circle", "ColorPicker", icons["picker_shape_circle"]);
	theme->set_icon("shape_rect", "ColorPicker", icons["picker_shape_rectangle"]);
	theme->set_icon("shape_rect_wheel", "ColorPicker", icons["picker_shape_rectangle_wheel"]);
	theme->set_icon("add_preset", "ColorPicker", icons["add"]);
	theme->set_icon("sample_bg", "ColorPicker", icons["mini_checkerboard"]);
	theme->set_icon("overbright_indicator", "ColorPicker", icons["color_picker_overbright"]);
	theme->set_icon("bar_arrow", "ColorPicker", icons["color_picker_bar_arrow"]);
	theme->set_icon("picker_cursor", "ColorPicker", icons["color_picker_cursor"]);

	{
		const int precision = 7;

		Ref<Gradient> hue_gradient;
		hue_gradient.instantiate();
		PackedFloat32Array offsets;
		offsets.resize(precision);
		PackedColorArray colors;
		colors.resize(precision);

		for (int i = 0; i < precision; i++) {
			float h = i / float(precision - 1);
			offsets.write[i] = h;
			colors.write[i] = Color::from_hsv(h, 1, 1);
		}
		hue_gradient->set_offsets(offsets);
		hue_gradient->set_colors(colors);

		Ref<GradientTexture2D> hue_texture;
		hue_texture.instantiate();
		hue_texture->set_width(800);
		hue_texture->set_height(6);
		hue_texture->set_gradient(hue_gradient);

		theme->set_icon("color_hue", "ColorPicker", hue_texture);
	}

	{
		const int precision = 7;

		Ref<Gradient> hue_gradient;
		hue_gradient.instantiate();
		PackedFloat32Array offsets;
		offsets.resize(precision);
		PackedColorArray colors;
		colors.resize(precision);

		for (int i = 0; i < precision; i++) {
			float h = i / float(precision - 1);
			offsets.write[i] = h;
			colors.write[i] = Color::from_ok_hsl(h, 1, 0.5);
		}
		hue_gradient->set_offsets(offsets);
		hue_gradient->set_colors(colors);

		Ref<GradientTexture2D> hue_texture;
		hue_texture.instantiate();
		hue_texture->set_width(800);
		hue_texture->set_height(6);
		hue_texture->set_gradient(hue_gradient);

		theme->set_icon("color_okhsl_hue", "ColorPicker", hue_texture);
	}

	// ColorPickerButton

	theme->set_icon("bg", "ColorPickerButton", icons["mini_checkerboard"]);
	theme->set_stylebox("normal", "ColorPickerButton", button_normal);
	theme->set_stylebox("pressed", "ColorPickerButton", button_pressed);
	theme->set_stylebox("hover", "ColorPickerButton", button_hover);
	theme->set_stylebox("disabled", "ColorPickerButton", button_disabled);
	theme->set_stylebox("focus", "ColorPickerButton", focus);

	theme->set_font("font", "ColorPickerButton", Ref<Font>());
	theme->set_font_size("font_size", "ColorPickerButton", -1);
	theme->set_color_scheme("default_color_scheme", "TabBar", default_color_scheme);

	theme->set_color("font_color_scale", "ColorPickerButton", control_font_color_scale);
	theme->set_color("font_focus_color_scale", "ColorPickerButton", control_font_focus_color_scale);
	theme->set_color("font_pressed_color_scale", "ColorPickerButton", control_font_pressed_color_scale);
	theme->set_color("font_hover_color_scale", "ColorPickerButton", control_font_hover_color_scale);
	theme->set_color("font_hover_pressed_color_scale", "ColorPickerButton", control_font_hover_pressed_color_scale);
	theme->set_color("font_disabled_color_scale", "ColorPickerButton", control_font_disabled_color_scale);
	theme->set_color("font_outline_color_scale", "ColorPickerButton", control_font_outline_color_scale);

	theme->set_color_role("font_color_role", "ColorPickerButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_pressed_color_role", "ColorPickerButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hover_color_role", "ColorPickerButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_focus_color_role", "ColorPickerButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_hover_pressed_color_role", "ColorPickerButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_disabled_color_role", "ColorPickerButton", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_outline_color_role", "ColorPickerButton", ColorRole::OUTLINE);

	theme->set_color_scheme("font_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_pressed_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_focus_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_hover_pressed_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_disabled_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "ColorPickerButton", Ref<ColorScheme>());

	theme->set_color("font_color", "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color("font_pressed_color", "ColorPickerButton", Color(0.8, 0.8, 0.8, 1));
	theme->set_color("font_hover_color", "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color("font_focus_color", "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color("font_hover_pressed_color", "ColorPickerButton", Color(0.9, 0.9, 0.9, 0.3));
	theme->set_color("font_disabled_color", "ColorPickerButton", Color(0.9, 0.9, 0.9, 0.3));
	theme->set_color("font_outline_color", "ColorPickerButton", Color(1, 1, 1));

	theme->set_constant("h_separation", "ColorPickerButton", Math::round(4 * scale));
	theme->set_constant("outline_size", "ColorPickerButton", 0);

	// ColorPresetButton

	Ref<StyleBoxFlat> preset_sb = make_color_role_flat_stylebox(ColorRole::PRIMARY, Color(1, 1, 1),StyleBoxFlat::ElevationLevel::Elevation_Level_0, 2, 2, 2, 2);
	preset_sb->set_corner_radius_all(Math::round(2 * scale));
	preset_sb->set_corner_detail(Math::round(2 * scale));
	preset_sb->set_anti_aliased(false);

	theme->set_stylebox("preset_fg", "ColorPresetButton", preset_sb);
	theme->set_icon("preset_bg", "ColorPresetButton", icons["mini_checkerboard"]);
	theme->set_icon("overbright_indicator", "ColorPresetButton", icons["color_picker_overbright"]);

	// TooltipPanel + TooltipLabel

	theme->set_type_variation("TooltipPanel", "PopupPanel");
	theme->set_stylebox("panel", "TooltipPanel",
			make_color_role_flat_stylebox(ColorRole::SURFACE_CONTAINER, Color(1, 1, 1, 0.5), StyleBoxFlat::ElevationLevel::Elevation_Level_0,2 * default_margin, 0.5 * default_margin, 2 * default_margin, 0.5 * default_margin));

	theme->set_type_variation("TooltipLabel", "Label");
	theme->set_font_size("font_size", "TooltipLabel", -1);
	theme->set_font("font", "TooltipLabel", Ref<Font>());

	theme->set_color_scheme("font_color_scheme", "TooltipLabel", Ref<ColorScheme>());
	theme->set_color_scheme("font_shadow_color_scheme", "TooltipLabel", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "TooltipLabel", Ref<ColorScheme>());

	theme->set_color_role("font_color_role", "TooltipLabel", ColorRole::PRIMARY);
	theme->set_color_role("font_shadow_color_role", "TooltipLabel", ColorRole::SHADOW);
	theme->set_color_role("font_outline_color_role", "TooltipLabel", ColorRole::PRIMARY);

	theme->set_color("font_color_scale", "TooltipLabel", control_font_color_scale);
	theme->set_color("font_shadow_color_scale", "TooltipLabel", Color(1, 1, 1, 1));
	theme->set_color("font_outline_color_scale", "TooltipLabel", Color(1, 1, 1, 1));

	theme->set_color("font_color", "TooltipLabel", control_font_color);
	theme->set_color("font_shadow_color", "TooltipLabel", Color(0, 0, 0, 0));
	theme->set_color("font_outline_color", "TooltipLabel", Color(0, 0, 0, 0));

	theme->set_constant("shadow_offset_x", "TooltipLabel", 1);
	theme->set_constant("shadow_offset_y", "TooltipLabel", 1);
	theme->set_constant("outline_size", "TooltipLabel", 0);

	// RichTextLabel

	theme->set_stylebox("focus", "RichTextLabel", focus);
	theme->set_stylebox("normal", "RichTextLabel", make_empty_stylebox(0, 0, 0, 0));

	theme->set_font("normal_font", "RichTextLabel", Ref<Font>());
	theme->set_font("bold_font", "RichTextLabel", bold_font);
	theme->set_font("italics_font", "RichTextLabel", italics_font);
	theme->set_font("bold_italics_font", "RichTextLabel", bold_italics_font);
	theme->set_font("mono_font", "RichTextLabel", Ref<Font>());
	theme->set_font_size("normal_font_size", "RichTextLabel", -1);
	theme->set_font_size("bold_font_size", "RichTextLabel", -1);
	theme->set_font_size("italics_font_size", "RichTextLabel", -1);
	theme->set_font_size("bold_italics_font_size", "RichTextLabel", -1);
	theme->set_font_size("mono_font_size", "RichTextLabel", -1);

	theme->set_color_scheme("default_color_scheme", "RichTextLabel", default_color_scheme);

	theme->set_color_scheme("font_default_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	theme->set_color_scheme("font_selected_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	theme->set_color_scheme("selection_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	theme->set_color_scheme("font_outline_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	theme->set_color_scheme("font_shadow_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	theme->set_color_scheme("outline_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	theme->set_color_scheme("table_odd_row_bg_scheme", "RichTextLabel", Ref<ColorScheme>());
	theme->set_color_scheme("table_even_row_bg_scheme", "RichTextLabel", Ref<ColorScheme>());
	theme->set_color_scheme("table_border_scheme", "RichTextLabel", Ref<ColorScheme>());

	theme->set_color("font_default_color_scale", "RichTextLabel", Color(1, 1, 1));
	theme->set_color("font_selected_color_scale", "RichTextLabel", Color(1, 1, 1));
	theme->set_color("selection_color_scale", "RichTextLabel", Color(1, 1, 1, 0.8));
	theme->set_color("font_outline_color_scale", "RichTextLabel", Color(1, 1, 1));
	theme->set_color("font_shadow_color_scale", "RichTextLabel", Color(1, 1, 1, 1));
	theme->set_color("outline_color_scale_scale", "RichTextLabel", Color(1, 1, 1, 1));
	theme->set_color("table_odd_row_bg_scale", "RichTextLabel", Color(1, 1, 1, 1));
	theme->set_color("table_even_row_bg_scale", "RichTextLabel", Color(1, 1, 1, 1));
	theme->set_color("table_border_scale", "RichTextLabel", Color(1, 1, 1, 1));

	theme->set_color("font_default_color", "RichTextLabel", Color(1, 1, 1));
	theme->set_color("font_selected_color", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("selection_color", "RichTextLabel", Color(0.1, 0.1, 1, 0.8));
	theme->set_color("font_outline_color", "RichTextLabel", Color(1, 1, 1));
	theme->set_color("font_shadow_color", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("outline_color_scale", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("table_odd_row_bg", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("table_even_row_bg", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("table_border", "RichTextLabel", Color(0, 0, 0, 0));

	theme->set_color_role("font_default_color_role", "RichTextLabel", ColorRole::ON_PRIMARY);
	theme->set_color_role("font_selected_color_role", "RichTextLabel", ColorRole::ON_PRIMARY);
	theme->set_color_role("selection_color_role", "RichTextLabel", ColorRole::PRIMARY);
	theme->set_color_role("font_outline_color_role", "RichTextLabel", ColorRole::OUTLINE);
	theme->set_color_role("font_shadow_color_role", "RichTextLabel", ColorRole::SHADOW);
	theme->set_color_role("outline_color_role", "RichTextLabel", ColorRole::OUTLINE);
	theme->set_color_role("table_odd_row_bg_role", "RichTextLabel", ColorRole::ON_PRIMARY);
	theme->set_color_role("table_even_row_bg_role", "RichTextLabel", ColorRole::ON_PRIMARY);
	theme->set_color_role("table_border_role", "RichTextLabel", ColorRole::ON_PRIMARY);

	theme->set_color("font_default_color", "RichTextLabel", Color(1, 1, 1));
	theme->set_color("font_selected_color", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("selection_color", "RichTextLabel", Color(0.1, 0.1, 1, 0.8));
	theme->set_color("font_shadow_color", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("font_outline_color", "RichTextLabel", Color(1, 1, 1));
	theme->set_color("outline_color", "RichTextLabel", Color(1, 1, 1));

	theme->set_constant("shadow_offset_x", "RichTextLabel", Math::round(1 * scale));
	theme->set_constant("shadow_offset_y", "RichTextLabel", Math::round(1 * scale));
	theme->set_constant("shadow_outline_size", "RichTextLabel", Math::round(1 * scale));

	theme->set_constant("line_separation", "RichTextLabel", 0);
	theme->set_constant("table_h_separation", "RichTextLabel", Math::round(3 * scale));
	theme->set_constant("table_v_separation", "RichTextLabel", Math::round(3 * scale));

	theme->set_constant("outline_size", "RichTextLabel", 0);

	theme->set_color("table_odd_row_bg", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("table_even_row_bg", "RichTextLabel", Color(0, 0, 0, 0));
	theme->set_color("table_border", "RichTextLabel", Color(0, 0, 0, 0));

	theme->set_constant("text_highlight_h_padding", "RichTextLabel", Math::round(3 * scale));
	theme->set_constant("text_highlight_v_padding", "RichTextLabel", Math::round(3 * scale));

	// Containers

	theme->set_icon("h_grabber", "SplitContainer", icons["hsplitter"]);
	theme->set_icon("v_grabber", "SplitContainer", icons["vsplitter"]);
	theme->set_icon("grabber", "VSplitContainer", icons["vsplitter"]);
	theme->set_icon("grabber", "HSplitContainer", icons["hsplitter"]);

	theme->set_constant("separation", "BoxContainer", Math::round(4 * scale));
	theme->set_constant("separation", "HBoxContainer", Math::round(4 * scale));
	theme->set_constant("separation", "VBoxContainer", Math::round(4 * scale));
	theme->set_constant("margin_left", "MarginContainer", 0);
	theme->set_constant("margin_top", "MarginContainer", 0);
	theme->set_constant("margin_right", "MarginContainer", 0);
	theme->set_constant("margin_bottom", "MarginContainer", 0);
	theme->set_constant("h_separation", "GridContainer", Math::round(4 * scale));
	theme->set_constant("v_separation", "GridContainer", Math::round(4 * scale));
	theme->set_constant("separation", "SplitContainer", Math::round(12 * scale));
	theme->set_constant("separation", "HSplitContainer", Math::round(12 * scale));
	theme->set_constant("separation", "VSplitContainer", Math::round(12 * scale));
	theme->set_constant("minimum_grab_thickness", "SplitContainer", Math::round(6 * scale));
	theme->set_constant("minimum_grab_thickness", "HSplitContainer", Math::round(6 * scale));
	theme->set_constant("minimum_grab_thickness", "VSplitContainer", Math::round(6 * scale));
	theme->set_constant("autohide", "SplitContainer", 1);
	theme->set_constant("autohide", "HSplitContainer", 1);
	theme->set_constant("autohide", "VSplitContainer", 1);
	theme->set_constant("h_separation", "FlowContainer", Math::round(4 * scale));
	theme->set_constant("v_separation", "FlowContainer", Math::round(4 * scale));
	theme->set_constant("h_separation", "HFlowContainer", Math::round(4 * scale));
	theme->set_constant("v_separation", "HFlowContainer", Math::round(4 * scale));
	theme->set_constant("h_separation", "VFlowContainer", Math::round(4 * scale));
	theme->set_constant("v_separation", "VFlowContainer", Math::round(4 * scale));

	theme->set_stylebox("panel", "PanelContainer", make_color_role_flat_stylebox(ColorRole::PRIMARY_CONTAINER, style_normal_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0));

	theme->set_icon("zoom_out", "GraphEdit", icons["zoom_less"]);
	theme->set_icon("zoom_in", "GraphEdit", icons["zoom_more"]);
	theme->set_icon("zoom_reset", "GraphEdit", icons["zoom_reset"]);
	theme->set_icon("grid_toggle", "GraphEdit", icons["grid_toggle"]);
	theme->set_icon("minimap_toggle", "GraphEdit", icons["grid_minimap"]);
	theme->set_icon("snapping_toggle", "GraphEdit", icons["grid_snap"]);
	theme->set_icon("layout", "GraphEdit", icons["grid_layout"]);

	theme->set_stylebox("panel", "GraphEdit", make_color_role_flat_stylebox(ColorRole::PRIMARY_CONTAINER, style_normal_color_scale,StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 5));

	Ref<StyleBoxFlat> graph_toolbar_style = make_color_role_flat_stylebox(ColorRole::PRIMARY, Color(0.24, 0.24, 0.24, 0.6),StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 2, 4, 2);
	theme->set_stylebox("menu_panel", "GraphEdit", graph_toolbar_style);

	theme->set_color_scheme("default_color_scheme", "GraphEdit", default_color_scheme);
	theme->set_color_scheme("grid_major_scheme", "GraphEdit", Ref<ColorScheme>());
	theme->set_color_scheme("grid_minor_scheme", "GraphEdit", Ref<ColorScheme>());
	theme->set_color_scheme("activity_color_scheme", "GraphEdit", Ref<ColorScheme>());
	theme->set_color_scheme("selection_fill_scheme", "GraphEdit", Ref<ColorScheme>());
	theme->set_color_scheme("selection_stroke_scheme", "GraphEdit", Ref<ColorScheme>());
	theme->set_color_role("grid_major_role", "GraphEdit", ColorRole::PRIMARY);
	theme->set_color_role("grid_minor_role", "GraphEdit", ColorRole::PRIMARY);
	theme->set_color_role("activity_color_role", "GraphEdit", ColorRole::PRIMARY);
	theme->set_color_role("selection_fill_role", "GraphEdit", ColorRole::PRIMARY);
	theme->set_color_role("selection_stroke_role", "GraphEdit", ColorRole::PRIMARY);

	theme->set_color("grid_minor", "GraphEdit", Color(1, 1, 1, 0.05));
	theme->set_color("grid_major", "GraphEdit", Color(1, 1, 1, 0.2));
	theme->set_color("selection_fill", "GraphEdit", Color(1, 1, 1, 0.3));
	theme->set_color("selection_stroke", "GraphEdit", Color(1, 1, 1, 0.8));
	theme->set_color("activity", "GraphEdit", Color(1, 1, 1));

	theme->set_color("grid_minor_scale", "GraphEdit", Color(1, 1, 1, 0.05));
	theme->set_color("grid_major_scale", "GraphEdit", Color(1, 1, 1, 0.2));
	theme->set_color("selection_fill_scale", "GraphEdit", Color(1, 1, 1, 0.3));
	theme->set_color("selection_stroke_scale", "GraphEdit", Color(1, 1, 1, 0.8));
	theme->set_color("activity_scale", "GraphEdit", Color(1, 1, 1));

	// Visual Node Ports

	theme->set_constant("port_hotzone_inner_extent", "GraphEdit", 22 * scale);
	theme->set_constant("port_hotzone_outer_extent", "GraphEdit", 26 * scale);

	theme->set_stylebox("panel", "GraphEditMinimap", make_color_role_flat_stylebox(ColorRole::PRIMARY, Color(0.24, 0.24, 0.24), StyleBoxFlat::ElevationLevel::Elevation_Level_0,0, 0, 0, 0));
	Ref<StyleBoxFlat> style_minimap_camera = make_color_role_flat_stylebox(ColorRole::PRIMARY, Color(0.65, 0.65, 0.65, 0.2), StyleBoxFlat::ElevationLevel::Elevation_Level_0,0, 0, 0, 0, 0);
	style_minimap_camera->set_border_color(Color(0.65, 0.65, 0.65, 0.45));
	style_minimap_camera->set_border_width_all(1);
	theme->set_stylebox("camera", "GraphEditMinimap", style_minimap_camera);
	theme->set_stylebox("node", "GraphEditMinimap", make_color_role_flat_stylebox(ColorRole::PRIMARY, Color(1, 1, 1),StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0, 2));

	theme->set_color_scheme("default_color_scheme", "GraphEditMinimap", default_color_scheme);
	theme->set_color_scheme("resizer_color_scheme", "GraphEditMinimap", Ref<ColorScheme>());
	theme->set_color_role("resizer_color_role", "GraphEditMinimap", ColorRole::PRIMARY);

	theme->set_icon("resizer", "GraphEditMinimap", icons["resizer_nw"]);
	theme->set_color("resizer_color", "GraphEditMinimap", Color(1, 1, 1, 0.85));
	theme->set_color("resizer_color_scale", "GraphEditMinimap", Color(1, 1, 1, 0.85));

	// Theme

	default_icon = icons["error_icon"];
	// Same color as the error icon.
	default_style = make_color_role_flat_stylebox(ColorRole::PRIMARY, Color(1, 0.365, 0.365), StyleBoxFlat::ElevationLevel::Elevation_Level_0,4, 4, 4, 4, 0, false, 2);
}
void make_default_theme(float p_scale, Ref<Font> p_font, Ref<Font> p_icon_font, Ref<ColorScheme> p_color_scheme, TextServer::SubpixelPositioning p_font_subpixel, TextServer::Hinting p_font_hinting, TextServer::FontAntialiasing p_font_antialiasing, bool p_font_msdf, bool p_font_generate_mipmaps) {
	Ref<Theme> t;
	t.instantiate();

	Ref<StyleBox> default_style;
	Ref<Texture2D> default_icon;
	Ref<Font> default_font;
	Ref<Font> default_icon_font;

	Ref<FontVariation> bold_font;
	Ref<FontVariation> bold_italics_font;
	Ref<FontVariation> italics_font;
	float default_scale = CLAMP(p_scale, 0.5, 8.0);

	Ref<ColorScheme> default_color_scheme;
	if (p_color_scheme.is_valid()) {
		default_color_scheme = p_color_scheme;
	} else {
		default_color_scheme.instantiate();
		default_color_scheme->set_source_color(Color("#6750A4"));
	}

	if (p_font.is_valid()) {
		// Use the custom font defined in the Project Settings.
		default_font = p_font;
	} else {
		// Use the default DynamicFont (separate from the editor font).
		// The default DynamicFont is chosen to have a small file size since it's
		// embedded in both editor and export template binaries.
		Ref<FontFile> dynamic_font;
		dynamic_font.instantiate();
		dynamic_font->set_data_ptr(_font_OpenSans_SemiBold, _font_OpenSans_SemiBold_size);
		dynamic_font->set_subpixel_positioning(p_font_subpixel);
		dynamic_font->set_hinting(p_font_hinting);
		dynamic_font->set_antialiasing(p_font_antialiasing);
		dynamic_font->set_multichannel_signed_distance_field(p_font_msdf);
		dynamic_font->set_generate_mipmaps(p_font_generate_mipmaps);

		default_font = dynamic_font;
	}

	if (p_icon_font.is_valid()) {
		// Use the custom font defined in the Project Settings.
		default_icon_font = p_icon_font;
	}

	if (default_font.is_valid()) {
		bold_font.instantiate();
		bold_font->set_base_font(default_font);
		bold_font->set_variation_embolden(1.2);

		bold_italics_font.instantiate();
		bold_italics_font->set_base_font(default_font);
		bold_italics_font->set_variation_embolden(1.2);
		bold_italics_font->set_variation_transform(Transform2D(1.0, 0.2, 0.0, 1.0, 0.0, 0.0));

		italics_font.instantiate();
		italics_font->set_base_font(default_font);
		italics_font->set_variation_transform(Transform2D(1.0, 0.2, 0.0, 1.0, 0.0, 0.0));
	}

	fill_default_theme(t, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, default_scale, default_color_scheme);

	ThemeDB::get_singleton()->set_default_theme(t);

	ThemeDB::get_singleton()->set_fallback_base_scale(default_scale);
	ThemeDB::get_singleton()->set_fallback_icon(default_icon);
	ThemeDB::get_singleton()->set_fallback_stylebox(default_style);
	ThemeDB::get_singleton()->set_fallback_font(default_font);
	ThemeDB::get_singleton()->set_fallback_font_size(default_font_size * default_scale);
	ThemeDB::get_singleton()->set_fallback_color_scheme(default_color_scheme);
}

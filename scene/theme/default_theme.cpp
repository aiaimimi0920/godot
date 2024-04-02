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

static Ref<ColorRole> make_color_role(ColorRoleEnum p_color_role_enum, Color &p_color_scale = Color(1, 1, 1, 1), bool p_inverted = false, float p_darkened_amount = 0.0, float p_lightened_amount = 0.0, float p_lerp_weight = 0.0, Ref<ColorRole> p_lerp_color_role = Ref<ColorRole>(), Color &p_lerp_color = Color(1, 1, 1, 1), bool p_clamp = false) {
	Ref<ColorRole> color_role(memnew(ColorRole));
	color_role->set_color_role_enum(p_color_role_enum);
	color_role->set_color_scale(p_color_scale);
	color_role->set_inverted(p_inverted);
	color_role->set_darkened_amount(p_darkened_amount);
	color_role->set_lightened_amount(p_lightened_amount);
	color_role->set_lerp_weight(p_lerp_weight);
	color_role->set_lerp_color_role(p_lerp_color_role);
	color_role->set_lerp_color(p_lerp_color);
	color_role->set_clamp(p_clamp);
	return color_role;
}

static Ref<StyleBoxFlat> make_color_role_flat_stylebox(Ref<ColorRole> p_color_role, StyleBoxFlat::ElevationLevel p_level = StyleBoxFlat::ElevationLevel::Elevation_Level_0, const Ref<ColorScheme> &default_color_scheme = Ref<ColorScheme>(), float p_margin_left = default_margin, float p_margin_top = default_margin, float p_margin_right = default_margin, float p_margin_bottom = default_margin, int p_corner_radius = default_corner_radius, bool p_draw_center = true, int p_border_width = 0) {
	Ref<StyleBoxFlat> style(memnew(StyleBoxFlat));
	if (default_color_scheme.is_valid()) {
		style->set_default_color_scheme(default_color_scheme);
	}

	style->set_bg_color_role(p_color_role);
	style->set_dynamic_shadow(true);
	style->set_elevation_level(p_level);

	style->set_shadow_color_role(make_color_role(ColorRoleEnum::SHADOW));
	style->set_border_color_role(make_color_role(ColorRoleEnum::SECONDARY));

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

void fill_default_theme_panel(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	theme->set_color_scheme("panel", "Panel", default_color_scheme);
	theme->set_stylebox("panel", "Panel", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY, Color(1, 1, 1, 1), false, 0.0, 0.0, 0.3, Ref<ColorRole>(), Color(0, 0, 0, 1), true), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0));
}

void fill_default_theme_button_variations(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_type_variation("FlatButton", "Button");
	Ref<StyleBoxEmpty> flat_button_normal = make_empty_stylebox();
	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	for (int i = 0; i < 4; i++) {
		flat_button_normal->set_content_margin((Side)i, default_stylebox_normal->get_margin((Side)i) + default_stylebox_normal->get_border_width((Side)i));
	}
	const Ref<StyleBoxFlat> button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> flat_button_pressed = button_pressed->duplicate();
	flat_button_pressed->set_bg_color_role(make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.38)));

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FlatButton", flat_button_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "FlatButton", flat_button_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "FlatButton", flat_button_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "FlatButton", flat_button_normal);

	theme->set_type_variation("FlatMenuButton", "MenuButton");
	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FlatMenuButton", flat_button_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "FlatMenuButton", flat_button_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "FlatMenuButton", flat_button_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "FlatMenuButton", flat_button_normal);
}

void fill_default_theme_label(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "Label", default_color_scheme);

	scale = p_scale;

	theme->set_stylebox("normal", "Label", memnew(StyleBoxEmpty));

	theme->set_font("font", "Label", default_font);

	theme->set_font_size("font_size", "Label", -1);
	theme->set_type_variation("HeaderSmall", "Label");
	theme->set_font_size("font_size", "HeaderSmall", default_font_size + 4);

	theme->set_type_variation("HeaderMedium", "Label");
	theme->set_font_size("font_size", "HeaderMedium", default_font_size + 8);

	theme->set_type_variation("HeaderLarge", "Label");
	theme->set_font_size("font_size", "HeaderLarge", default_font_size + 12);

	theme->set_constant("line_spacing", "Label", Math::round(3 * scale));

	theme->set_color_role("font_color_role", "Label", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("font_color", "Label", Color(1, 1, 1));

	theme->set_color_role("font_shadow_color_role", "Label", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
	theme->set_color("font_shadow_color", "Label", Color(1, 1, 1));

	theme->set_color_role("font_outline_color_role", "Label", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "Label", Color(1, 1, 1));

	theme->set_constant("shadow_offset_x", "Label", Math::round(1 * scale));
	theme->set_constant("shadow_offset_y", "Label", Math::round(1 * scale));
	theme->set_constant("outline_size", "Label", 0);
	theme->set_constant("shadow_outline_size", "Label", Math::round(1 * scale));

	theme->set_type_variation("TooltipLabel", "Label");
	theme->set_font_size("font_size", "TooltipLabel", -1);
	theme->set_font("font", "TooltipLabel", Ref<Font>());

	theme->set_color_role("font_color_role", "TooltipLabel", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("font_color", "TooltipLabel", Color(1, 1, 1));

	theme->set_color_role("font_shadow_color_role", "TooltipLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
	theme->set_color("font_shadow_color", "TooltipLabel", Color(1, 1, 1));

	theme->set_color_role("font_outline_color_role", "TooltipLabel", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "TooltipLabel", Color(1, 1, 1));

	theme->set_constant("shadow_offset_x", "TooltipLabel", 1);
	theme->set_constant("shadow_offset_y", "TooltipLabel", 1);
	theme->set_constant("outline_size", "TooltipLabel", 0);
}

void fill_default_theme_rich_text_label(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "RichTextLabel", default_color_scheme);

	scale = p_scale;

	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_LOW), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "RichTextLabel", make_empty_stylebox(0, 0, 0, 0));
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "RichTextLabel", default_stylebox_focus);

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

	theme->set_color_role("default_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("default_color", "RichTextLabel", Color(1, 1, 1, 1));

	theme->set_color_role("font_selected_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("font_selected_color", "RichTextLabel", Color(0, 0, 0, 0));

	theme->set_color_role("selection_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("selection_color", "RichTextLabel", Color(0.1, 0.1, 1, 0.8));

	theme->set_color_role("font_shadow_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::SHADOW));
	theme->set_color("font_shadow_color", "RichTextLabel", Color(0, 0, 0, 0));

	theme->set_color_role("font_outline_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "RichTextLabel", Color(1, 1, 1, 1));

	theme->set_color_role("outline_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("outline_color", "RichTextLabel", Color(1, 1, 1));

	theme->set_constant("shadow_offset_x", "RichTextLabel", Math::round(1 * scale));
	theme->set_constant("shadow_offset_y", "RichTextLabel", Math::round(1 * scale));
	theme->set_constant("shadow_outline_size", "RichTextLabel", Math::round(1 * scale));

	theme->set_constant("line_separation", "RichTextLabel", 0);
	theme->set_constant("table_h_separation", "RichTextLabel", Math::round(3 * scale));
	theme->set_constant("table_v_separation", "RichTextLabel", Math::round(3 * scale));

	theme->set_constant("outline_size", "RichTextLabel", 0);

	theme->set_color_role("table_odd_row_bg_role", "RichTextLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
	theme->set_color("table_odd_row_bg", "RichTextLabel", Color(0, 0, 0, 0));

	theme->set_color_role("table_even_row_bg_role", "RichTextLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
	theme->set_color("table_even_row_bg", "RichTextLabel", Color(0, 0, 0, 0));

	theme->set_color_role("table_border_role", "RichTextLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
	theme->set_color("table_border", "RichTextLabel", Color(0, 0, 0, 0));

	theme->set_constant("text_highlight_h_padding", "RichTextLabel", Math::round(3 * scale));
	theme->set_constant("text_highlight_v_padding", "RichTextLabel", Math::round(3 * scale));
}

void fill_default_theme_texture_button(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "TextureButton", default_color_scheme);

	scale = p_scale;

	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TextureButton", make_empty_stylebox(0, 0, 0, 0));
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TextureButton", make_empty_stylebox(0, 0, 0, 0));
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "TextureButton", make_empty_stylebox(0, 0, 0, 0));
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TextureButton", make_empty_stylebox(0, 0, 0, 0));
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TextureButton", default_stylebox_focus);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TextureButton", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "TextureButton", state_layer_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TextureButton", state_layer_stylebox_focus);

	theme->set_font("text_icon_font", "TextureButton", default_icon_font);

	theme->set_font_size("text_icon_font_size", "TextureButton", -1);

	theme->set_constant("icon_max_width", "TextureButton", 0);

	cur_theme_data.set_data_name("text_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TextureButton", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TextureButton", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "TextureButton", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TextureButton", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TextureButton", make_color_role(ColorRoleEnum::PRIMARY));

	cur_theme_data.set_data_name("text_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TextureButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TextureButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "TextureButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TextureButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TextureButton", Color(1, 1, 1, 1));
}

void fill_default_theme_popup_menu(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "PopupMenu", default_color_scheme);
	scale = p_scale;
	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	default_stylebox_normal->set_border_width_all(2);
	default_stylebox_normal->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));
	const Ref<StyleBoxFlat> default_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	const Ref<StyleBoxFlat> default_stylebox_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	default_stylebox_disabled->set_border_width_all(2);
	default_stylebox_disabled->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "PopupMenu", default_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "PopupMenu", default_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "PopupMenu", default_stylebox_disabled);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "PopupMenu", default_stylebox_focus);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "PopupMenu", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "PopupMenu", state_layer_stylebox_focus);

	Ref<StyleBoxLine> separator_horizontal = memnew(StyleBoxLine);
	separator_horizontal->set_thickness(Math::round(scale));
	separator_horizontal->set_color_role(make_color_role(ColorRoleEnum::OUTLINE));
	separator_horizontal->set_content_margin_individual(default_margin, 0, default_margin, 0);
	Ref<StyleBoxLine> separator_vertical = separator_horizontal->duplicate();
	separator_vertical->set_vertical(true);
	separator_vertical->set_content_margin_individual(0, default_margin, 0, default_margin);

	theme->set_stylebox("separator", "PopupMenu", separator_horizontal);
	theme->set_stylebox("labeled_separator_left", "PopupMenu", separator_horizontal);
	theme->set_stylebox("labeled_separator_right", "PopupMenu", separator_horizontal);

	cur_theme_data.set_data_name("icon");
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "PopupMenu", icons["checked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "PopupMenu", icons["checked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "PopupMenu", icons["unchecked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "PopupMenu", icons["unchecked"]);
	cur_theme_data.set_data_name("radio_icon");
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "PopupMenu", icons["radio_checked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "PopupMenu", icons["radio_checked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "PopupMenu", icons["radio_unchecked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "PopupMenu", icons["radio_unchecked"]);
	cur_theme_data.set_data_name("submenu");
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "PopupMenu", icons["popup_menu_arrow_right"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneRTL), "PopupMenu", icons["popup_menu_arrow_left"]);

	theme->set_font("font", "PopupMenu", Ref<Font>());
	theme->set_font("font_separator", "PopupMenu", Ref<Font>());
	theme->set_font_size("font_size", "PopupMenu", -1);
	theme->set_font_size("font_separator_size", "PopupMenu", -1);

	theme->set_constant("indent", "PopupMenu", Math::round(10 * scale));
	theme->set_constant("h_separation", "PopupMenu", Math::round(4 * scale));
	theme->set_constant("v_separation", "PopupMenu", Math::round(4 * scale));
	theme->set_constant("outline_size", "PopupMenu", 0);
	theme->set_constant("separator_outline_size", "PopupMenu", 0);
	theme->set_constant("item_start_padding", "PopupMenu", Math::round(2 * scale));
	theme->set_constant("item_end_padding", "PopupMenu", Math::round(2 * scale));
	theme->set_constant("icon_max_width", "PopupMenu", 0);

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "PopupMenu", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "PopupMenu", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "PopupMenu", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "PopupMenu", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "PopupMenu", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "PopupMenu", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "PopupMenu", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "PopupMenu", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "PopupMenu", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "PopupMenu", Color(1, 1, 1, 1));

	theme->set_color_role("font_accelerator_color_role", "PopupMenu", make_color_role(ColorRoleEnum::ON_PRIMARY_FIXED));
	theme->set_color("font_accelerator_color", "PopupMenu", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "PopupMenu", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "PopupMenu", Color(1, 1, 1, 1));

	theme->set_color_role("font_separator_color_role", "PopupMenu", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_separator_color", "PopupMenu", Color(1, 1, 1, 1));

	theme->set_color_role("font_separator_outline_color_role", "PopupMenu", make_color_role(ColorRoleEnum::OUTLINE_VARIANT));
	theme->set_color("font_separator_outline_color", "PopupMenu", Color(1, 1, 1, 1));
}

void fill_default_theme_checkbox(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "CheckBox", default_color_scheme);

	scale = p_scale;

	Ref<StyleBox> cbx_empty = memnew(StyleBoxEmpty);
	cbx_empty->set_content_margin_all(Math::round(4 * scale));
	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));
	default_stylebox_focus->set_content_margin_all(Math::round(4 * scale));

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	state_layer_stylebox_hover->set_content_margin_all(Math::round(4 * scale));
	const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	state_layer_stylebox_pressed->set_content_margin_all(Math::round(4 * scale));
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	state_layer_stylebox_focus->set_content_margin_all(Math::round(4 * scale));

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", cbx_empty);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", cbx_empty);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", cbx_empty);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", cbx_empty);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", default_stylebox_focus);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", state_layer_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", state_layer_stylebox_focus);

	theme->set_font("font", "CheckBox", default_font);
	theme->set_font("text_icon_font", "CheckBox", default_icon_font);

	theme->set_font_size("font_size", "CheckBox", -1);
	theme->set_font_size("text_icon_font_size", "CheckBox", -1);
	theme->set_constant("font_outline_size", "CheckBox", 0);

	theme->set_constant("icon_max_width", "CheckBox", 0);
	theme->set_constant("h_separation", "CheckBox", Math::round(4 * scale));

	cur_theme_data.set_data_name("text_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("text_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));

	cur_theme_data.set_data_name("icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", Color(1, 1, 1, 0.4));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "CheckBox", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "CheckBox", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon");
	Ref<Texture2D> empty_icon = memnew(ImageTexture);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", empty_icon);

	theme->set_constant("check_v_offset", "CheckBox", 0);

	cur_theme_data.set_data_name("check_icon");
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckBox", icons["checked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckBox", icons["unchecked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckBox", icons["checked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckBox", icons["unchecked"]);

	cur_theme_data.set_data_name("radio_check_icon");
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckBox", icons["radio_checked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckBox", icons["radio_unchecked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckBox", icons["radio_checked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckBox", icons["radio_unchecked"]);

	cur_theme_data.set_data_name("check_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("check_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("text_check_icon");
	theme->set_str(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckBox", "checkbox-marked");
	theme->set_str(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckBox", "checkbox-blank-outline");
	theme->set_str(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckBox", "checkbox-marked");
	theme->set_str(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckBox", "checkbox-blank-outline");

	cur_theme_data.set_data_name("text_radio_check_icon");
	theme->set_str(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckBox", "radiobox-marked");
	theme->set_str(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckBox", "radiobox-blank");
	theme->set_str(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckBox", "radiobox-marked");
	theme->set_str(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckBox", "radiobox-blank");

	cur_theme_data.set_data_name("text_check_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("text_check_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", Color(1, 1, 1, 1));
}

void fill_default_theme_check_button(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "CheckBox", default_color_scheme);

	scale = p_scale;

	Ref<StyleBox> cb_empty = memnew(StyleBoxEmpty);
	cb_empty->set_content_margin_individual(Math::round(6 * scale), Math::round(4 * scale), Math::round(6 * scale), Math::round(4 * scale));
	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	state_layer_stylebox_hover->set_content_margin_all(Math::round(4 * scale));
	const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	state_layer_stylebox_pressed->set_content_margin_all(Math::round(4 * scale));
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	state_layer_stylebox_focus->set_content_margin_all(Math::round(4 * scale));

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", cb_empty);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", cb_empty);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", cb_empty);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", cb_empty);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", default_stylebox_focus);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", state_layer_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", state_layer_stylebox_focus);

	theme->set_font("font", "CheckButton", default_font);
	theme->set_font("text_icon_font", "CheckButton", default_icon_font);

	theme->set_font_size("font_size", "CheckButton", -1);
	theme->set_font_size("text_icon_font_size", "CheckButton", -1);
	theme->set_constant("font_outline_size", "CheckButton", 0);

	theme->set_constant("icon_max_width", "CheckButton", 0);
	theme->set_constant("h_separation", "CheckButton", Math::round(4 * scale));

	cur_theme_data.set_data_name("text_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("text_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	cur_theme_data.set_data_name("icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", Color(1, 1, 1, 0.4));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "CheckButton", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "CheckButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon");
	Ref<Texture2D> empty_icon = memnew(ImageTexture);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", empty_icon);

	theme->set_constant("check_v_offset", "CheckButton", 0);

	cur_theme_data.set_data_name("check_icon");
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckButton", icons["toggle_on"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckButton", icons["toggle_off"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckButton", icons["toggle_on_disabled"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckButton", icons["toggle_off_disabled"]);

	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedRTL), "CheckButton", icons["toggle_on_mirrored"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedRTL), "CheckButton", icons["toggle_off_mirrored"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedRTL), "CheckButton", icons["toggle_on_disabled_mirrored"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedRTL), "CheckButton", icons["toggle_off_disabled_mirrored"]);

	cur_theme_data.set_data_name("check_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("check_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("text_check_icon");
	theme->set_str(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckButton", "toggle-switch");
	theme->set_str(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckButton", "toggle-switch-off-outline");

	theme->set_str(cur_theme_data.get_state_data_name(State::NormalCheckedRTL), "CheckButton", "toggle-switch-off");
	theme->set_str(cur_theme_data.get_state_data_name(State::NormalUncheckedRTL), "CheckButton", "toggle-switch-outline");

	cur_theme_data.set_data_name("text_check_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverCheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverUncheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedCheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedUncheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusCheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusUncheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.38)));

	cur_theme_data.set_data_name("text_check_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("text_check_icon_bg_1");
	theme->set_str(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", "toggle-switch-off");

	cur_theme_data.set_data_name("text_check_icon_bg_2");
	theme->set_str(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", "toggle-switch");

	cur_theme_data.set_data_name("text_check_icon_bg_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverCheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::PRIMARY_CONTAINER));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverUncheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedCheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::PRIMARY_CONTAINER));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedUncheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusCheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::PRIMARY_CONTAINER));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusUncheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::SURFACE, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckButton", make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST, Color(1, 1, 1, 0.38)));

	cur_theme_data.set_data_name("text_check_icon_bg_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckButton", Color(1, 1, 1, 0.38));
}

void fill_default_theme_button(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "Button", default_color_scheme);

	scale = p_scale;

	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", default_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", default_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", default_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", default_stylebox_disabled);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", default_stylebox_focus);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", state_layer_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", state_layer_stylebox_focus);

	theme->set_font("font", "Button", default_font);
	theme->set_font("text_icon_font", "Button", default_icon_font);

	theme->set_font_size("font_size", "Button", -1);
	theme->set_font_size("text_icon_font_size", "Button", -1);
	theme->set_constant("font_outline_size", "Button", 0);

	theme->set_constant("icon_max_width", "Button", 0);
	theme->set_constant("h_separation", "Button", Math::round(4 * scale));

	cur_theme_data.set_data_name("text_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("text_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", make_color_role(ColorRoleEnum::STATIC_COLOR));

	cur_theme_data.set_data_name("icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", Color(1, 1, 1, 0.4));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "Button", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "Button", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon");
	Ref<Texture2D> empty_icon = memnew(ImageTexture);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", empty_icon);
	// theme->set_icon(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", empty_icon);
}

void fill_default_theme_menu_button(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "MenuButton", default_color_scheme);

	scale = p_scale;

	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", default_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", default_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", default_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", default_stylebox_disabled);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", default_stylebox_focus);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", state_layer_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", state_layer_stylebox_focus);

	theme->set_font("font", "MenuButton", default_font);
	theme->set_font("text_icon_font", "MenuButton", default_icon_font);

	theme->set_font_size("font_size", "MenuButton", -1);
	theme->set_font_size("text_icon_font_size", "MenuButton", -1);
	theme->set_constant("font_outline_size", "MenuButton", 0);

	theme->set_constant("icon_max_width", "MenuButton", 0);
	theme->set_constant("h_separation", "MenuButton", Math::round(4 * scale));

	cur_theme_data.set_data_name("text_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("text_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	cur_theme_data.set_data_name("icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", Color(1, 1, 1, 0.4));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "MenuButton", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "MenuButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon");
	Ref<Texture2D> empty_icon = memnew(ImageTexture);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", empty_icon);
}

void fill_default_theme_link_button(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "LinkButton", default_color_scheme);

	scale = p_scale;

	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	theme->set_stylebox("focus", "LinkButton", default_stylebox_focus);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "LinkButton", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "LinkButton", state_layer_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "LinkButton", state_layer_stylebox_focus);

	theme->set_font("font", "LinkButton", default_font);

	theme->set_font_size("font_size", "LinkButton", -1);
	theme->set_constant("outline_size", "LinkButton", 0);

	theme->set_constant("underline_spacing", "LinkButton", Math::round(2 * scale));

	theme->set_color_role("font_outline_color_role", "LinkButton", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "LinkButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LinkButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "LinkButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "LinkButton", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "LinkButton", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "LinkButton", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LinkButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "LinkButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "LinkButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "LinkButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "LinkButton", Color(1, 1, 1, 1));
}

void fill_default_theme_elevated_button(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	theme->set_type_variation("ElevatedButton", "Button");
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "ElevatedButton", default_color_scheme);

	scale = p_scale;

	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE), StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE), StyleBoxFlat::ElevationLevel::Elevation_Level_2, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE), StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.10)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.10)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ElevatedButton", default_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ElevatedButton", default_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ElevatedButton", default_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ElevatedButton", default_stylebox_disabled);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ElevatedButton", default_stylebox_focus);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ElevatedButton", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ElevatedButton", state_layer_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ElevatedButton", state_layer_stylebox_focus);

	theme->set_font("font", "ElevatedButton", default_font);
	theme->set_font("text_icon_font", "ElevatedButton", default_icon_font);

	theme->set_font_size("font_size", "ElevatedButton", -1);
	theme->set_font_size("text_icon_font_size", "ElevatedButton", -1);
	theme->set_constant("font_outline_size", "ElevatedButton", 0);

	theme->set_constant("icon_max_width", "ElevatedButton", 0);
	theme->set_constant("h_separation", "ElevatedButton", Math::round(4 * scale));

	cur_theme_data.set_data_name("text_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::INVERSE_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	cur_theme_data.set_data_name("text_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ElevatedButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	cur_theme_data.set_data_name("icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ElevatedButton", Color(1, 1, 1, 0.4));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "ElevatedButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::INVERSE_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ElevatedButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ElevatedButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ElevatedButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon");
	Ref<Texture2D> empty_icon = memnew(ImageTexture);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", empty_icon);
}

void fill_default_theme_color_picker(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "ColorPickerButton", default_color_scheme);

	scale = p_scale;

	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE), StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE), StyleBoxFlat::ElevationLevel::Elevation_Level_2, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE), StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.10)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.10)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ColorPickerButton", default_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ColorPickerButton", default_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ColorPickerButton", default_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ColorPickerButton", default_stylebox_disabled);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ColorPickerButton", default_stylebox_focus);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ColorPickerButton", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ColorPickerButton", state_layer_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ColorPickerButton", state_layer_stylebox_focus);

	theme->set_font("font", "ColorPickerButton", default_font);
	theme->set_font("text_icon_font", "ColorPickerButton", default_icon_font);

	theme->set_font_size("font_size", "ColorPickerButton", -1);
	theme->set_font_size("text_icon_font_size", "ColorPickerButton", -1);
	theme->set_constant("font_outline_size", "ColorPickerButton", 0);

	theme->set_constant("icon_max_width", "ColorPickerButton", 0);
	theme->set_constant("h_separation", "ColorPickerButton", Math::round(4 * scale));

	cur_theme_data.set_data_name("text_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::INVERSE_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	cur_theme_data.set_data_name("text_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ColorPickerButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	cur_theme_data.set_data_name("icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ColorPickerButton", Color(1, 1, 1, 0.4));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "ColorPickerButton", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "ColorPickerButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::INVERSE_SURFACE));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ColorPickerButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "ColorPickerButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ColorPickerButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon");
	Ref<Texture2D> empty_icon = memnew(ImageTexture);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", empty_icon);

	theme->set_icon("bg", "ColorPickerButton", icons["mini_checkerboard"]);

	theme->set_icon("overbright_indicator", "ColorPickerButton", icons["color_picker_overbright"]);

	// ColorPresetButton
	Ref<StyleBoxFlat> preset_sb = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2, 2, 2, 2);
	preset_sb->set_corner_radius_all(Math::round(2 * scale));
	preset_sb->set_corner_detail(Math::round(2 * scale));
	preset_sb->set_anti_aliased(false);

	theme->set_stylebox("preset_fg", "ColorPresetButton", preset_sb);
	theme->set_icon("preset_bg", "ColorPresetButton", icons["mini_checkerboard"]);
	theme->set_icon("overbright_indicator", "ColorPresetButton", icons["color_picker_overbright"]);

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
}

void fill_default_theme_option_button(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "OptionButton", default_color_scheme);

	scale = p_scale;

	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	Ref<StyleBox> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2 * default_margin, default_margin, 21, default_margin);
	Ref<StyleBox> default_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2 * default_margin, default_margin, 21, default_margin);
	Ref<StyleBox> default_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2 * default_margin, default_margin, 21, default_margin);
	Ref<StyleBox> default_stylebox_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.012)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2 * default_margin, default_margin, 21, default_margin);
	Ref<StyleBox> default_stylebox_normal_mirrored = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 21, default_margin, 2 * default_margin, default_margin);
	Ref<StyleBox> default_stylebox_hover_mirrored = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 21, default_margin, 2 * default_margin, default_margin);
	Ref<StyleBox> default_stylebox_pressed_mirrored = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 21, default_margin, 2 * default_margin, default_margin);
	Ref<StyleBox> default_stylebox_disabled_mirrored = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 21, default_margin, 2 * default_margin, default_margin);

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", default_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", default_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", default_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", default_stylebox_disabled);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneRTL), "OptionButton", default_stylebox_normal_mirrored);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneRTL), "OptionButton", default_stylebox_hover_mirrored);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneRTL), "OptionButton", default_stylebox_pressed_mirrored);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneRTL), "OptionButton", default_stylebox_disabled_mirrored);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", default_stylebox_focus);

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", state_layer_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", state_layer_stylebox_focus);

	theme->set_font("font", "OptionButton", default_font);
	theme->set_font("text_icon_font", "OptionButton", default_icon_font);

	theme->set_font_size("font_size", "OptionButton", -1);
	theme->set_font_size("text_icon_font_size", "OptionButton", -1);
	theme->set_constant("font_outline_size", "OptionButton", 0);

	theme->set_constant("icon_max_width", "OptionButton", 0);
	theme->set_constant("h_separation", "OptionButton", Math::round(4 * scale));

	cur_theme_data.set_data_name("text_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("text_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	cur_theme_data.set_data_name("icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", Color(1, 1, 1, 0.4));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "OptionButton", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "OptionButton", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", Color(1, 1, 1, 1));

	theme->set_icon("arrow", "OptionButton", icons["option_button_arrow"]);
	theme->set_constant("arrow_margin", "OptionButton", Math::round(4 * scale));
	theme->set_constant("modulate_arrow", "OptionButton", false);
}

void fill_default_theme_scroll_container(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	Ref<StyleBoxEmpty> empty;
	empty.instantiate();
	theme->set_stylebox("panel", "ScrollContainer", empty);
}

void fill_default_theme_separators(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;

	Ref<StyleBoxLine> separator_horizontal = memnew(StyleBoxLine);
	separator_horizontal->set_thickness(Math::round(scale));
	separator_horizontal->set_color(Color(0.5, 0.5, 0.5));
	separator_horizontal->set_content_margin_individual(default_margin, 0, default_margin, 0);
	separator_horizontal->set_color_scheme(default_color_scheme);
	separator_horizontal->set_color_role(make_color_role(ColorRoleEnum::OUTLINE_VARIANT));

	Ref<StyleBoxLine> separator_vertical = separator_horizontal->duplicate();
	separator_vertical->set_vertical(true);
	separator_vertical->set_content_margin_individual(0, default_margin, 0, default_margin);

	cur_theme_data.set_data_name("separator");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "HSeparator", separator_horizontal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "VSeparator", separator_vertical);

	theme->set_icon("close", "Icons", icons["close"]);
	theme->set_font("normal", "Fonts", Ref<Font>());
	theme->set_font("large", "Fonts", Ref<Font>());

	theme->set_constant("separation", "HSeparator", Math::round(4 * scale));
	theme->set_constant("separation", "VSeparator", Math::round(4 * scale));
}

void fill_default_theme_scrollbar(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;

	const Ref<StyleBoxFlat> style_h_scrollbar = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 4, 0, 4, 10);
	const Ref<StyleBoxFlat> style_v_scrollbar = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 0, 4, 0, 10);
	const Ref<StyleBoxFlat> style_scrollbar_grabber = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY, Color(1, 1, 1, 0.38)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4, 10);
	const Ref<StyleBoxFlat> style_scrollbar_grabber_highlight = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4, 10);
	const Ref<StyleBoxFlat> style_scrollbar_grabber_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY, Color(1, 1, 1, 0.6)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4, 10);

	Ref<StyleBoxFlat> default_scrollbar_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_scrollbar_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	Ref<Texture2D> empty_icon = memnew(ImageTexture);

	// HScrollBar
	cur_theme_data.set_data_name("scroll_style");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "HScrollBar", style_h_scrollbar);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "HScrollBar", default_scrollbar_stylebox_focus);
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
	cur_theme_data.set_data_name("scroll_style");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "VScrollBar", style_v_scrollbar);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "VScrollBar", default_scrollbar_stylebox_focus);
	theme->set_stylebox("grabber", "VScrollBar", style_scrollbar_grabber);
	theme->set_stylebox("grabber_highlight", "VScrollBar", style_scrollbar_grabber_highlight);
	theme->set_stylebox("grabber_pressed", "VScrollBar", style_scrollbar_grabber_pressed);

	theme->set_icon("increment", "VScrollBar", empty_icon);
	theme->set_icon("increment_highlight", "VScrollBar", empty_icon);
	theme->set_icon("increment_pressed", "VScrollBar", empty_icon);
	theme->set_icon("decrement", "VScrollBar", empty_icon);
	theme->set_icon("decrement_highlight", "VScrollBar", empty_icon);
	theme->set_icon("decrement_pressed", "VScrollBar", empty_icon);
}

void fill_default_theme_menu_bar(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "MenuBar", default_color_scheme);

	scale = p_scale;

	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuBar", default_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuBar", default_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuBar", default_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuBar", default_stylebox_disabled);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuBar", default_stylebox_focus);

	cur_theme_data.set_data_name("state_layer_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuBar", state_layer_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuBar", state_layer_stylebox_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuBar", state_layer_stylebox_focus);

	theme->set_font("font", "MenuBar", default_font);

	theme->set_font_size("font_size", "MenuBar", -1);
	theme->set_constant("outline_size", "MenuBar", 0);

	theme->set_constant("h_separation", "MenuBar", Math::round(4 * scale));

	theme->set_color_role("font_outline_color_role", "MenuBar", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "MenuBar", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "MenuBar", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverCheckedLTR), "MenuBar", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedCheckedLTR), "MenuBar", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuBar", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusCheckedLTR), "MenuBar", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuBar", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuBar", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuBar", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuBar", Color(1, 1, 1, 0.38));
	theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuBar", Color(1, 1, 1, 1));
}

void fill_default_theme_popup_panel(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "PopupPanel", default_color_scheme);

	scale = p_scale;

	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	theme->set_stylebox("panel", "PopupPanel", default_stylebox_normal);

	theme->set_type_variation("TooltipPanel", "PopupPanel");
	const Ref<StyleBoxFlat> tooltip_default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2 * default_margin, 0.5 * default_margin, 2 * default_margin, 0.5 * default_margin);

	theme->set_stylebox("panel", "TooltipPanel",
			tooltip_default_stylebox_normal);
}

// Dialogs
void fill_default_theme_dialogs(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	// AcceptDialog is currently the base dialog, so this defines styles for all extending nodes.
	theme->set_color_scheme("default_color_scheme", "AcceptDialog", default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, Math::round(8 * scale), Math::round(8 * scale), Math::round(8 * scale), Math::round(8 * scale));
	theme->set_stylebox("panel", "AcceptDialog", default_stylebox_normal);

	theme->set_constant("buttons_separation", "AcceptDialog", Math::round(10 * scale));

	// File Dialog

	theme->set_icon("parent_folder", "FileDialog", icons["folder_up"]);
	theme->set_icon("back_folder", "FileDialog", icons["arrow_left"]);
	theme->set_icon("forward_folder", "FileDialog", icons["arrow_right"]);
	theme->set_icon("reload", "FileDialog", icons["reload"]);
	theme->set_icon("toggle_hidden", "FileDialog", icons["visibility_visible"]);
	theme->set_icon("folder", "FileDialog", icons["folder"]);
	theme->set_icon("file", "FileDialog", icons["file"]);

	theme->set_color_role("folder_icon_color_role", "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("folder_icon_color", "FileDialog", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("file_icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));

	cur_theme_data.set_data_name("file_icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FileDialog", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "FileDialog", Color(1, 1, 1, 0.25));

	cur_theme_data.set_data_name("icon_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("icon_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FileDialog", Color(1, 1, 1, 1));
}

// Containers
void fill_default_theme_containers(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "Container", default_color_scheme);
	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0);

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

	theme->set_stylebox("panel", "PanelContainer", default_stylebox_normal);
}

// ProgressBar
void fill_default_theme_progress_bar(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "ProgressBar", default_color_scheme);
	const Ref<StyleBoxFlat> background_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2, 2, 2, 2, 6);
	const Ref<StyleBoxFlat> fill_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2, 2, 2, 2, 6);

	theme->set_stylebox("background", "ProgressBar", background_stylebox_normal);
	theme->set_stylebox("fill", "ProgressBar", fill_stylebox_normal);

	theme->set_font("font", "ProgressBar", Ref<Font>());
	theme->set_font_size("font_size", "ProgressBar", -1);

	theme->set_constant("outline_size", "ProgressBar", 0);

	theme->set_color_role("font_color_role", "ProgressBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("font_color", "ProgressBar", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "ProgressBar", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "ProgressBar", Color(1, 1, 1, 1));
}

// ItemList
void fill_default_theme_item_list(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "ItemList", default_color_scheme);
	theme->set_constant("h_separation", "ItemList", Math::round(4 * scale));
	theme->set_constant("v_separation", "ItemList", Math::round(2 * scale));

	cur_theme_data.set_data_name("default_panel");
	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ItemList", default_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ItemList", default_stylebox_focus);

	theme->set_font("font", "ItemList", Ref<Font>());
	theme->set_font_size("font_size", "ItemList", -1);

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ItemList", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ItemList", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "ItemList", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ItemList", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ItemList", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "ItemList", Color(1, 1, 1, 1));

	theme->set_constant("outline_size", "ItemList", 0);

	theme->set_color_role("font_outline_color_role", "ItemList", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "ItemList", Color(1, 1, 1, 1));

	theme->set_constant("icon_margin", "ItemList", Math::round(4 * scale));
	theme->set_constant("line_separation", "ItemList", Math::round(2 * scale));

	cur_theme_data.set_data_name("item_style");
	const Ref<StyleBoxFlat> item_style_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> item_style_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	item_style_focus->set_expand_margin_all(Math::round(2 * scale));
	Ref<StyleBoxFlat> item_style_selected_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	item_style_selected_focus->set_expand_margin_all(Math::round(2 * scale));

	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ItemList", item_style_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ItemList", item_style_focus);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusCheckedLTR), "ItemList", item_style_selected_focus);

	cur_theme_data.set_data_name("cursor");
	const Ref<StyleBoxFlat> cursor_stylebox = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> cursor_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	cursor_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ItemList", cursor_stylebox);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ItemList", cursor_stylebox_focus);

	theme->set_color_role("guide_color_role", "ItemList", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.16)));
	theme->set_color("guide_color", "ItemList", Color(0.7, 0.7, 0.7, 0.25));
}

// TabContainer
void fill_default_theme_tab_container(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "TabContainer", default_color_scheme);

	Ref<StyleBoxFlat> style_tab_selected = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 10, 4, 10, 4, 0);
	style_tab_selected->set_border_width(SIDE_TOP, Math::round(2 * scale));
	style_tab_selected->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));
	Ref<StyleBoxFlat> style_tab_unselected = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 10, 4, 10, 4, 0);
	// Add some spacing between unselected tabs to make them easier to distinguish from each other.
	style_tab_unselected->set_border_width(SIDE_LEFT, Math::round(scale));
	style_tab_unselected->set_border_width(SIDE_RIGHT, Math::round(scale));
	style_tab_unselected->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));
	Ref<StyleBoxFlat> style_tab_disabled = style_tab_unselected->duplicate();
	style_tab_disabled->set_bg_color_role(make_color_role(ColorRoleEnum::BACKGROUND));
	Ref<StyleBoxFlat> style_tab_hovered = style_tab_unselected->duplicate();
	style_tab_hovered->set_bg_color_role(make_color_role(ColorRoleEnum::BACKGROUND, Color(1, 1, 1, 0.16)));

	Ref<StyleBoxFlat> focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	focus->set_expand_margin_all(Math::round(2 * scale));
	Ref<StyleBoxFlat> style_tab_focus = focus->duplicate();

	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0);
	theme->set_stylebox("panel", "TabContainer", default_stylebox_normal);
	theme->set_stylebox("tabbar_background", "TabContainer", make_empty_stylebox(0, 0, 0, 0));

	theme->set_constant("side_margin", "TabContainer", Math::round(8 * scale));

	theme->set_icon("menu", "TabContainer", icons["tabs_menu"]);
	theme->set_icon("menu_highlight", "TabContainer", icons["tabs_menu_hl"]);

	theme->set_constant("icon_separation", "TabContainer", Math::round(4 * scale));
	theme->set_constant("icon_max_width", "TabContainer", 0);
	theme->set_constant("outline_size", "TabContainer", 0);

	cur_theme_data.set_data_name("tab");

	const Ref<StyleBoxFlat> tab_stylebox_selected = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> tab_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> tab_stylebox_unselected = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> tab_stylebox_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> tab_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	tab_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "TabContainer", tab_stylebox_selected);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TabContainer", tab_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "TabContainer", tab_stylebox_unselected);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TabContainer", tab_stylebox_disabled);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TabContainer", tab_stylebox_focus);

	theme->set_icon("increment", "TabContainer", icons["scroll_button_right"]);
	theme->set_icon("increment_highlight", "TabContainer", icons["scroll_button_right_hl"]);
	theme->set_icon("decrement", "TabContainer", icons["scroll_button_left"]);
	theme->set_icon("decrement_highlight", "TabContainer", icons["scroll_button_left_hl"]);
	theme->set_icon("drop_mark", "TabContainer", icons["tabs_drop_mark"]);

	theme->set_color_role("drop_mark_color_role", "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("drop_mark_color", "TabContainer", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "TabContainer", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TabContainer", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "TabContainer", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TabContainer", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "TabContainer", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "TabContainer", Color(1, 1, 1, 1));

	theme->set_font("font", "TabContainer", Ref<Font>());
	theme->set_font_size("font_size", "TabContainer", -1);
}

// TabBar
void fill_default_theme_tab_bar(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "TabBar", default_color_scheme);

	cur_theme_data.set_data_name("tab");

	const Ref<StyleBoxFlat> tab_stylebox_selected = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> tab_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> tab_stylebox_unselected = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> tab_stylebox_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> tab_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	tab_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "TabBar", tab_stylebox_selected);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TabBar", tab_stylebox_hover);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "TabBar", tab_stylebox_unselected);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TabBar", tab_stylebox_disabled);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TabBar", tab_stylebox_focus);

	const Ref<StyleBoxFlat> button_pressed_stylebox = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> button_highlight_stylebox = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	theme->set_stylebox("button_pressed", "TabBar", button_pressed_stylebox);
	theme->set_stylebox("button_highlight", "TabBar", button_highlight_stylebox);

	theme->set_icon("increment", "TabBar", icons["scroll_button_right"]);
	theme->set_icon("increment_highlight", "TabBar", icons["scroll_button_right_hl"]);
	theme->set_icon("decrement", "TabBar", icons["scroll_button_left"]);
	theme->set_icon("decrement_highlight", "TabBar", icons["scroll_button_left_hl"]);
	theme->set_icon("drop_mark", "TabBar", icons["tabs_drop_mark"]);
	theme->set_icon("close", "TabBar", icons["close"]);

	theme->set_font("font", "TabBar", Ref<Font>());
	theme->set_font_size("font_size", "TabBar", -1);
	theme->set_constant("h_separation", "TabBar", Math::round(4 * scale));
	theme->set_constant("icon_max_width", "TabBar", 0);
	theme->set_constant("outline_size", "TabBar", 0);

	theme->set_color_role("drop_mark_color_role", "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("drop_mark_color", "TabBar", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "TabBar", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TabBar", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "TabBar", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TabBar", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "TabBar", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "TabBar", Color(1, 1, 1, 1));
}

// Tree
void fill_default_theme_tree(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "Tree", default_color_scheme);

	cur_theme_data.set_data_name("default_stylebox");

	const Ref<StyleBoxFlat> default_style_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 5);
	Ref<StyleBoxFlat> default_style_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_style_focus->set_expand_margin_all(Math::round(2 * scale));

	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", default_style_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Tree", default_style_focus);

	theme->set_font("font", "Tree", Ref<Font>());
	theme->set_font_size("font_size", "Tree", -1);
	theme->set_font("title_button_font", "Tree", Ref<Font>());
	theme->set_font_size("title_button_font_size", "Tree", -1);

	cur_theme_data.set_data_name("selected");
	const Ref<StyleBoxFlat> selected_style_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> selected_style_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	selected_style_focus->set_expand_margin_all(Math::round(2 * scale));

	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", selected_style_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Tree", selected_style_focus);

	cur_theme_data.set_data_name("cursor");
	const Ref<StyleBoxFlat> cursor_style_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	Ref<StyleBoxFlat> cursor_style_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	cursor_style_focus->set_expand_margin_all(Math::round(2 * scale));
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", cursor_style_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Tree", cursor_style_focus);

	theme->set_stylebox("button_pressed", "Tree", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme));

	cur_theme_data.set_data_name("title_button");
	const Ref<StyleBoxFlat> title_button_style_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
	const Ref<StyleBoxFlat> title_button_style_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
	const Ref<StyleBoxFlat> title_button_style_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);

	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", title_button_style_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Tree", title_button_style_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Tree", title_button_style_hover);

	cur_theme_data.set_data_name("custom_button");
	const Ref<StyleBoxFlat> custom_button_style_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
	const Ref<StyleBoxFlat> custom_button_style_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
	const Ref<StyleBoxFlat> custom_button_style_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);

	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", custom_button_style_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Tree", custom_button_style_pressed);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Tree", custom_button_style_hover);

	theme->set_color_role("title_button_color_role", "Tree", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color("title_button_color", "Tree", Color(1, 1, 1, 1));

	cur_theme_data.set_data_name("checked");
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "Tree", icons["checked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "Tree", icons["unchecked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "Tree", icons["checked"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "Tree", icons["unchecked"]);

	cur_theme_data.set_data_name("indeterminate");
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", icons["indeterminate"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Tree", icons["indeterminate"]);

	theme->set_icon("arrow", "Tree", icons["arrow_down"]);
	theme->set_icon("arrow_collapsed", "Tree", icons["arrow_right"]);
	theme->set_icon("arrow_collapsed_mirrored", "Tree", icons["arrow_left"]);
	theme->set_icon("select_arrow", "Tree", icons["option_button_arrow"]);
	theme->set_icon("updown", "Tree", icons["updown"]);

	cur_theme_data.set_data_name("font_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "Tree", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "Tree", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Tree", make_color_role(ColorRoleEnum::PRIMARY));

	cur_theme_data.set_data_name("font_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "Tree", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "Tree", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Tree", Color(1, 1, 1, 1));

	theme->set_color_role("guide_color_role", "Tree", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.16)));
	theme->set_color("guide_color", "Tree", Color(0.7, 0.7, 0.7, 0.25));

	theme->set_color_role("drop_position_color_role", "Tree", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color("drop_position_color", "Tree", Color(1, 1, 1));

	theme->set_color_role("relationship_line_color_role", "Tree", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color("relationship_line_color", "Tree", Color(0.27, 0.27, 0.27));

	theme->set_color_role("parent_hl_line_color_role", "Tree", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color("parent_hl_line_color", "Tree", Color(0.27, 0.27, 0.27));

	theme->set_color_role("children_hl_line_color_role", "Tree", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color("children_hl_line_color", "Tree", Color(0.27, 0.27, 0.27));

	theme->set_color_role("custom_button_font_highlight_role", "Tree", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color("custom_button_font_highlight", "Tree", Color(0.27, 0.27, 0.27));

	theme->set_color_role("font_outline_color_role", "Tree", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "Tree", Color(1, 1, 1));

	theme->set_constant("base_scale", "Tree", 1.0);
	theme->set_constant("outline_size", "Tree", 0);

	theme->set_constant("h_separation", "Tree", Math::round(4 * scale));
	theme->set_constant("v_separation", "Tree", Math::round(4 * scale));

	theme->set_constant("inner_item_margin_bottom", "Tree", 0);
	theme->set_constant("inner_item_margin_left", "Tree", 0);
	theme->set_constant("inner_item_margin_right", "Tree", 0);
	theme->set_constant("inner_item_margin_top", "Tree", 0);

	theme->set_constant("item_margin", "Tree", Math::round(16 * scale));
	theme->set_constant("button_margin", "Tree", Math::round(4 * scale));
	theme->set_constant("icon_max_width", "Tree", 0);

	theme->set_constant("draw_relationship_lines", "Tree", 0);
	theme->set_constant("relationship_line_width", "Tree", 1);
	theme->set_constant("parent_hl_line_width", "Tree", 1);
	theme->set_constant("children_hl_line_width", "Tree", 1);
	theme->set_constant("parent_hl_line_margin", "Tree", 0);
	theme->set_constant("draw_guides", "Tree", 1);

	theme->set_constant("scroll_border", "Tree", Math::round(4 * scale));
	theme->set_constant("scroll_speed", "Tree", 12);

	theme->set_constant("scrollbar_margin_left", "Tree", -1);
	theme->set_constant("scrollbar_margin_top", "Tree", -1);
	theme->set_constant("scrollbar_margin_right", "Tree", -1);
	theme->set_constant("scrollbar_margin_bottom", "Tree", -1);
	theme->set_constant("scrollbar_h_separation", "Tree", Math::round(4 * scale));
	theme->set_constant("scrollbar_v_separation", "Tree", Math::round(4 * scale));
}

// GraphEdit
void fill_default_theme_graph_edit(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	// GraphNode
	theme->set_color_scheme("default_color_scheme", "GraphNode", default_color_scheme);
	const Ref<StyleBoxFlat> graphnode_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 18, 12, 18, 12);
	graphnode_normal->set_border_color(Color(0.325, 0.325, 0.325, 0.6));
	graphnode_normal->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	Ref<StyleBoxFlat> graphnode_selected = graphnode_normal->duplicate();
	graphnode_selected->set_border_color(Color(0.625, 0.625, 0.625, 0.6));
	graphnode_selected->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	Ref<StyleBoxFlat> graphn_sb_titlebar = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
	Ref<StyleBoxFlat> graphn_sb_titlebar_selected = graphnode_normal->duplicate();
	graphn_sb_titlebar_selected->set_bg_color(Color(1.0, 0.625, 0.625, 0.6));
	graphn_sb_titlebar_selected->set_bg_color_role(make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.38)));

	Ref<StyleBoxEmpty> graphnode_slot = make_empty_stylebox(0, 0, 0, 0);

	cur_theme_data.set_data_name("panel");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "GraphNode", graphnode_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "GraphNode", graphnode_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "GraphNode", graphnode_selected);

	cur_theme_data.set_data_name("titlebar");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "GraphNode", graphn_sb_titlebar);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "GraphNode", graphn_sb_titlebar);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "GraphNode", graphn_sb_titlebar_selected);

	theme->set_stylebox("slot", "GraphNode", graphnode_slot);

	theme->set_icon("port", "GraphNode", icons["graph_port"]);
	theme->set_icon("resizer", "GraphNode", icons["resizer_se"]);

	theme->set_color_role("resizer_color_role", "GraphNode", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color("resizer_color", "GraphNode", Color(1, 1, 1));

	theme->set_constant("separation", "GraphNode", Math::round(2 * scale));
	theme->set_constant("port_h_offset", "GraphNode", 0);

	// GraphNodes's title Label.

	theme->set_type_variation("GraphNodeTitleLabel", "Label");
	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "GraphNodeTitleLabel", make_empty_stylebox(0, 0, 0, 0));

	theme->set_font("font", "GraphNodeTitleLabel", Ref<Font>());
	theme->set_font_size("font_size", "GraphNodeTitleLabel", -1);

	theme->set_color_role("font_color_role", "GraphNodeTitleLabel", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("font_color", "GraphNodeTitleLabel", Color(1, 1, 1, 1));

	theme->set_color_role("font_shadow_color_role", "GraphNodeTitleLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
	theme->set_color("font_shadow_color", "GraphNodeTitleLabel", Color(1, 1, 1));

	theme->set_color_role("font_outline_color_role", "GraphNodeTitleLabel", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "GraphNodeTitleLabel", Color(1, 1, 1));

	theme->set_constant("shadow_offset_x", "GraphNodeTitleLabel", Math::round(1 * scale));
	theme->set_constant("shadow_offset_y", "GraphNodeTitleLabel", Math::round(1 * scale));
	theme->set_constant("outline_size", "GraphNodeTitleLabel", 0);
	theme->set_constant("shadow_outline_size", "GraphNodeTitleLabel", Math::round(1 * scale));
	theme->set_constant("line_spacing", "GraphNodeTitleLabel", Math::round(3 * scale));

	theme->set_color_scheme("default_color_scheme", "GraphEditMinimap", default_color_scheme);
	const Ref<StyleBoxFlat> panel_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0);

	Ref<StyleBoxFlat> style_minimap_camera = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0, 0);
	style_minimap_camera->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));
	style_minimap_camera->set_border_width_all(1);

	Ref<StyleBoxFlat> style_node = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0, 2);

	theme->set_stylebox("panel", "GraphEditMinimap", panel_stylebox_normal);
	theme->set_stylebox("camera", "GraphEditMinimap", style_minimap_camera);
	theme->set_stylebox("node", "GraphEditMinimap", style_node);

	theme->set_icon("resizer", "GraphEditMinimap", icons["resizer_nw"]);

	theme->set_color_role("resizer_color_role", "GraphEditMinimap", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("resizer_color", "GraphEditMinimap", Color(1, 1, 1, 0.85));

	theme->set_color_scheme("default_color_scheme", "GraphEdit", default_color_scheme);

	theme->set_icon("zoom_out", "GraphEdit", icons["zoom_less"]);
	theme->set_icon("zoom_in", "GraphEdit", icons["zoom_more"]);
	theme->set_icon("zoom_reset", "GraphEdit", icons["zoom_reset"]);
	theme->set_icon("grid_toggle", "GraphEdit", icons["grid_toggle"]);
	theme->set_icon("minimap_toggle", "GraphEdit", icons["grid_minimap"]);
	theme->set_icon("snapping_toggle", "GraphEdit", icons["grid_snap"]);
	theme->set_icon("layout", "GraphEdit", icons["grid_layout"]);

	const Ref<StyleBoxFlat> graph_edit_panel_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 5);

	const Ref<StyleBoxFlat> graph_toolbar_style = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 2, 4, 2);

	theme->set_stylebox("panel", "GraphEdit", graph_edit_panel_stylebox_normal);
	theme->set_stylebox("menu_panel", "GraphEdit", graph_toolbar_style);

	theme->set_color_role("grid_minor_role", "GraphEdit", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.08)));
	theme->set_color("grid_minor", "GraphEdit", Color(1, 1, 1, 0.05));

	theme->set_color_role("grid_major_role", "GraphEdit", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.16)));
	theme->set_color("grid_major", "GraphEdit", Color(1, 1, 1, 0.2));

	theme->set_color_role("selection_fill_role", "GraphEdit", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color("selection_fill", "GraphEdit", Color(1, 1, 1, 0.3));

	theme->set_color_role("selection_stroke_role", "GraphEdit", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color("selection_stroke", "GraphEdit", Color(1, 1, 1, 0.8));

	theme->set_color_role("activity_role", "GraphEdit", make_color_role(ColorRoleEnum::PRIMARY));
	theme->set_color("activity", "GraphEdit", Color(1, 1, 1));

	// Visual Node Ports

	theme->set_constant("port_hotzone_inner_extent", "GraphEdit", 22 * scale);
	theme->set_constant("port_hotzone_outer_extent", "GraphEdit", 26 * scale);
}

// Window
void fill_default_theme_window(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "Window", default_color_scheme);

	const Ref<StyleBoxFlat> embedded_border_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 10, 28, 10, 8);
	sb_expand(embedded_border_stylebox_normal, 8, 32, 8, 6);
	const Ref<StyleBoxFlat> embedded_unfocused_border_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 10, 28, 10, 8);
	sb_expand(embedded_unfocused_border_stylebox_normal, 8, 32, 8, 6);

	cur_theme_data.set_data_name("embedded_border");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Window", embedded_unfocused_border_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Window", embedded_border_stylebox_normal);

	theme->set_font("title_font", "Window", Ref<Font>());
	theme->set_font_size("title_font_size", "Window", -1);

	theme->set_color_role("title_color_role", "Window", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	theme->set_color("title_color", "Window", Color(1, 1, 1, 1));

	theme->set_color_role("title_outline_modulate_role", "Window", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("title_outline_modulate", "Window", Color(1, 1, 1, 1));

	theme->set_constant("title_outline_size", "Window", 0);
	theme->set_constant("title_height", "Window", 36 * scale);
	theme->set_constant("resize_margin", "Window", Math::round(4 * scale));

	cur_theme_data.set_data_name("close");
	theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Window", icons["close"]);
	theme->set_icon(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Window", icons["close_hl"]);
	theme->set_constant("close_h_offset", "Window", 18 * scale);
	theme->set_constant("close_v_offset", "Window", 24 * scale);
}

// LineEdit
void fill_default_theme_line_edit(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "LineEdit", default_color_scheme);

	scale = p_scale;

	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	default_stylebox_normal->set_border_width(SIDE_BOTTOM, 2);
	default_stylebox_normal->set_border_color_role(make_color_role(ColorRoleEnum::ON_PRIMARY));

	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_LOW), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LineEdit", default_stylebox_normal);

	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "LineEdit", default_stylebox_focus);

	Ref<StyleBoxFlat> read_only_stylebox = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	read_only_stylebox->set_border_width(SIDE_BOTTOM, 2);
	read_only_stylebox->set_border_color_role(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_stylebox("read_only", "LineEdit", read_only_stylebox);

	theme->set_font("font", "LineEdit", default_font);

	theme->set_font_size("font_size", "LineEdit", -1);

	theme->set_color_role("font_color_role", "LineEdit", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("font_color", "LineEdit", Color(1, 1, 1, 1));

	theme->set_color_role("font_uneditable_color_role", "LineEdit", make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.38)));
	theme->set_color("font_uneditable_color", "LineEdit", Color(1, 1, 1, 1));

	theme->set_color_role("font_selected_color_role", "LineEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	theme->set_color("font_selected_color", "LineEdit", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "LineEdit", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "LineEdit", Color(1, 1, 1, 1));

	theme->set_constant("outline_size", "LineEdit", 0);

	cur_theme_data.set_data_name("font_placeholder_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LineEdit", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.38)));

	cur_theme_data.set_data_name("font_placeholder_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LineEdit", Color(1, 1, 1, 1));

	theme->set_constant("caret_width", "LineEdit", 0);

	theme->set_color_role("caret_color_role", "LineEdit", make_color_role(ColorRoleEnum::SECONDARY));
	theme->set_color("caret_color", "LineEdit", Color(1, 1, 1, 1));

	theme->set_color_role("selection_color_role", "LineEdit", make_color_role(ColorRoleEnum::PRIMARY_CONTAINER));
	theme->set_color("selection_color", "LineEdit", Color(1, 1, 1, 1));

	theme->set_icon("clear", "LineEdit", icons["line_edit_clear"]);

	cur_theme_data.set_data_name("clear_button_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LineEdit", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "LineEdit", make_color_role(ColorRoleEnum::ON_PRIMARY));

	cur_theme_data.set_data_name("clear_button_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LineEdit", Color(1, 1, 1, 1));
	theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "LineEdit", Color(1, 1, 1, 1));

	theme->set_constant("base_scale", "LineEdit", 1.0);
}

// TextEdit
void fill_default_theme_text_edit(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "TextEdit", default_color_scheme);

	scale = p_scale;

	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	default_stylebox_normal->set_border_width(SIDE_BOTTOM, 2);
	default_stylebox_normal->set_border_color_role(make_color_role(ColorRoleEnum::ON_PRIMARY));

	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_LOW), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	cur_theme_data.set_data_name("default_stylebox");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TextEdit", default_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TextEdit", default_stylebox_focus);

	Ref<StyleBoxFlat> read_only_stylebox = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	read_only_stylebox->set_border_width(SIDE_BOTTOM, 2);
	read_only_stylebox->set_border_color_role(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	cur_theme_data.set_data_name("read_only");
	theme->set_stylebox("read_only", "TextEdit", read_only_stylebox);

	theme->set_constant("base_scale", "TextEdit", 1);

	theme->set_color_role("search_result_color_role", "TextEdit", make_color_role(ColorRoleEnum::ON_SECONDARY));
	theme->set_color("search_result_color", "TextEdit", Color(0.3, 0.3, 0.3));

	theme->set_color_role("search_result_border_color_role", "TextEdit", make_color_role(ColorRoleEnum::ON_SECONDARY, Color(1, 1, 1, 0.38)));
	theme->set_color("search_result_border_color", "TextEdit", Color(0.3, 0.3, 0.3, 0.4));

	theme->set_constant("caret_width", "TextEdit", 1);

	theme->set_color_role("caret_color_role", "TextEdit", make_color_role(ColorRoleEnum::SECONDARY));
	theme->set_color("caret_color", "TextEdit", Color(1, 1, 1));

	theme->set_color_role("caret_background_color_role", "TextEdit", make_color_role(ColorRoleEnum::SECONDARY_CONTAINER));
	theme->set_color("caret_background_color", "TextEdit", Color(0, 0, 0));

	theme->set_color_role("font_selected_color_role", "TextEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	theme->set_color("font_selected_color", "TextEdit", Color(1, 1, 1, 1));

	theme->set_color_role("selection_color_role", "TextEdit", make_color_role(ColorRoleEnum::PRIMARY_CONTAINER));
	theme->set_color("selection_color", "TextEdit", Color(1, 1, 1, 1));

	theme->set_icon("tab", "TextEdit", icons["text_edit_tab"]);
	theme->set_icon("space", "TextEdit", icons["text_edit_space"]);

	theme->set_font("font", "TextEdit", Ref<Font>());
	theme->set_font_size("font_size", "TextEdit", -1);

	theme->set_color_role("font_color_role", "TextEdit", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("font_color", "TextEdit", Color(1, 1, 1, 1));

	theme->set_color_role("font_readonly_color_role", "TextEdit", make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.38)));
	theme->set_color("font_readonly_color", "TextEdit", Color(1, 1, 1, 1));

	theme->set_color_role("font_placeholder_color_role", "TextEdit", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color("font_placeholder_color", "TextEdit", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "TextEdit", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "TextEdit", Color(1, 1, 1, 1));

	theme->set_constant("outline_size", "TextEdit", 0);

	theme->set_constant("line_spacing", "TextEdit", Math::round(4 * scale));
}

// CodeEdit
void fill_default_theme_code_edit(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	theme->set_color_scheme("default_color_scheme", "CodeEdit", default_color_scheme);

	scale = p_scale;

	const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	default_stylebox_normal->set_border_width(SIDE_BOTTOM, 2);
	default_stylebox_normal->set_border_color_role(make_color_role(ColorRoleEnum::ON_PRIMARY));

	Ref<StyleBoxFlat> default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_LOW), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

	cur_theme_data.set_data_name("default_style");
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CodeEdit", default_stylebox_normal);
	theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CodeEdit", default_stylebox_focus);

	Ref<StyleBoxFlat> read_only_stylebox = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE, Color(1, 1, 1, 0.12)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	read_only_stylebox->set_border_width(SIDE_BOTTOM, 2);
	read_only_stylebox->set_border_color_role(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_stylebox("read_only", "CodeEdit", read_only_stylebox);

	theme->set_stylebox("completion", "CodeEdit", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0));

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

	theme->set_color_role("background_color_role", "CodeEdit", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
	theme->set_color("background_color", "CodeEdit", Color(0, 0, 0, 0));

	theme->set_color_role("completion_background_color_role", "CodeEdit", make_color_role(ColorRoleEnum::TERTIARY_CONTAINER));
	theme->set_color("completion_background_color", "CodeEdit", Color(0.17, 0.16, 0.2));

	theme->set_color_role("completion_selected_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	theme->set_color("completion_selected_color", "CodeEdit", Color(0.26, 0.26, 0.27));

	theme->set_color_role("completion_existing_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER, Color(1, 1, 1, 0.12)));
	theme->set_color("completion_existing_color", "CodeEdit", Color(0.87, 0.87, 0.87, 0.13));

	cur_theme_data.set_data_name("completion_scroll_color_role");
	theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CodeEdit", make_color_role(ColorRoleEnum::INVERSE_PRIMARY, Color(1, 1, 1, 0.16)));
	theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CodeEdit", make_color_role(ColorRoleEnum::INVERSE_PRIMARY, Color(1, 1, 1, 0.38)));
	cur_theme_data.set_data_name("completion_scroll_color");
	theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CodeEdit", Color(1, 1, 1, 0.29));
	theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CodeEdit", Color(1, 1, 1, 0.4));

	theme->set_color_role("font_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY));
	theme->set_color("font_color", "CodeEdit", Color(1, 1, 1, 1));

	theme->set_color_role("font_selected_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	theme->set_color("font_selected_color", "CodeEdit", Color(1, 1, 1, 1));

	theme->set_color_role("font_readonly_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_SURFACE, Color(1, 1, 1, 0.38)));
	theme->set_color("font_readonly_color", "CodeEdit", Color(1, 1, 1, 1));

	theme->set_color_role("font_placeholder_color_role", "CodeEdit", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color("font_placeholder_color", "CodeEdit", Color(1, 1, 1, 1));

	theme->set_color_role("font_outline_color_role", "CodeEdit", make_color_role(ColorRoleEnum::OUTLINE));
	theme->set_color("font_outline_color", "CodeEdit", Color(1, 1, 1, 1));

	theme->set_color_role("selection_color_role", "CodeEdit", make_color_role(ColorRoleEnum::PRIMARY_CONTAINER));
	theme->set_color("selection_color", "CodeEdit", Color(1, 1, 1, 1));

	theme->set_color_role("bookmark_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_TERTIARY_CONTAINER));
	theme->set_color("bookmark_color", "CodeEdit", Color(0.5, 0.64, 1, 0.8));

	theme->set_color_role("breakpoint_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ERROR));
	theme->set_color("breakpoint_color", "CodeEdit", Color(0.9, 0.29, 0.3));

	theme->set_color_role("executing_line_color_role", "CodeEdit", make_color_role(ColorRoleEnum::TERTIARY_CONTAINER));
	theme->set_color("executing_line_color", "CodeEdit", Color(0.98, 0.89, 0.27));

	theme->set_color_role("current_line_color_role", "CodeEdit", make_color_role(ColorRoleEnum::TERTIARY, Color(1, 1, 1, 0.38)));
	theme->set_color("current_line_color", "CodeEdit", Color(0.25, 0.25, 0.26, 0.8));

	theme->set_color_role("code_folding_color_role", "CodeEdit", make_color_role(ColorRoleEnum::TERTIARY, Color(1, 1, 1, 0.38)));
	theme->set_color("code_folding_color", "CodeEdit", Color(0.8, 0.8, 0.8, 0.8));

	theme->set_color_role("folded_code_region_color_role", "CodeEdit", make_color_role(ColorRoleEnum::TERTIARY, Color(1, 1, 1, 0.16)));
	theme->set_color("folded_code_region_color", "CodeEdit", Color(0.68, 0.46, 0.77, 0.2));

	theme->set_color_role("caret_color_role", "CodeEdit", make_color_role(ColorRoleEnum::SECONDARY));
	theme->set_color("caret_color", "CodeEdit", Color(1, 1, 1));

	theme->set_color_role("caret_background_color_role", "CodeEdit", make_color_role(ColorRoleEnum::SECONDARY_CONTAINER));
	theme->set_color("caret_background_color", "CodeEdit", Color(0, 0, 0));

	theme->set_color_role("brace_mismatch_color_role", "CodeEdit", make_color_role(ColorRoleEnum::SECONDARY_CONTAINER));
	theme->set_color("brace_mismatch_color", "CodeEdit", Color(1, 0.2, 0.2));

	theme->set_color_role("line_number_color_role", "CodeEdit", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.38)));
	theme->set_color("line_number_color", "CodeEdit", Color(0.67, 0.67, 0.67, 0.4));

	theme->set_color_role("word_highlighted_color_role", "CodeEdit", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.16)));
	theme->set_color("word_highlighted_color", "CodeEdit", Color(0.8, 0.9, 0.9, 0.15));

	theme->set_color_role("line_length_guideline_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)));
	theme->set_color("line_length_guideline_color", "CodeEdit", Color(0.3, 0.5, 0.8, 0.1));

	theme->set_color_role("search_result_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_SECONDARY));
	theme->set_color("search_result_color", "CodeEdit", Color(0.3, 0.3, 0.3));

	theme->set_color_role("search_result_border_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_SECONDARY, Color(1, 1, 1, 0.38)));
	theme->set_color("search_result_border_color", "CodeEdit", Color(0.3, 0.3, 0.3, 0.4));

	theme->set_constant("completion_lines", "CodeEdit", 7);
	theme->set_constant("completion_max_width", "CodeEdit", 50);
	theme->set_constant("completion_scroll_width", "CodeEdit", 6);
	theme->set_constant("line_spacing", "CodeEdit", Math::round(4 * scale));
	theme->set_constant("outline_size", "CodeEdit", 0);
}

void fill_default_theme_slider(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme, Dictionary icons) {
	ThemeIntData cur_theme_data;
	const Ref<StyleBoxFlat> style_slider = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
	const Ref<StyleBoxFlat> style_slider_grabber = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
	const Ref<StyleBoxFlat> style_slider_grabber_highlight = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY, Color(1, 1, 1, 0.6)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
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
}

void fill_default_theme(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &bold_font, const Ref<Font> &bold_italics_font, const Ref<Font> &italics_font, Ref<Texture2D> &default_icon, const Ref<Font> &default_icon_font, Ref<StyleBox> &default_style, float p_scale, const Ref<ColorScheme> &default_color_scheme) {
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

	Color base_color(0.2, 0.23, 0.31);
	Color accent_color(0.41, 0.61, 0.91);

	// Font colors
	Ref<ColorRole> mono_color_role = make_color_role(ColorRoleEnum::ON_SURFACE);
	Ref<ColorRole> base_color_role = make_color_role(ColorRoleEnum::SECONDARY);
	Ref<ColorRole> accent_color_role = make_color_role(ColorRoleEnum::PRIMARY);
	
	Ref<ColorRole> outline_color_role = make_color_role(ColorRoleEnum::OUTLINE);

	const Color control_font_color = Color(0.875, 0.875, 0.875);
	Ref<ColorRole> control_font_color_role = mono_color_role->duplicate();
	control_font_color_role->set_lerp_color_role(base_color_role);
	control_font_color_role->set_lerp_weight(0.25);

	const Color control_font_low_color = Color(0.7, 0.7, 0.7);
	Ref<ColorRole> control_font_low_color_role = mono_color_role->duplicate();
	control_font_low_color_role->set_lerp_color_role(base_color_role);
	control_font_low_color_role->set_lerp_weight(0.2);

	const Color control_font_lower_color = Color(0.65, 0.65, 0.65);
	Ref<ColorRole> control_font_lower_color_role = mono_color_role->duplicate();
	control_font_lower_color_role->set_lerp_color_role(base_color_role);
	control_font_lower_color_role->set_lerp_weight(0.1);


	const Color control_font_hover_color = Color(0.95, 0.95, 0.95);
	Ref<ColorRole> control_font_hover_color_role = mono_color_role->duplicate();
	control_font_hover_color_role->set_lerp_color_role(base_color_role);
	control_font_hover_color_role->set_lerp_weight(0.125);

	const Color control_font_focus_color = Color(0.95, 0.95, 0.95);
	Ref<ColorRole> control_font_focus_color_role = mono_color_role->duplicate();
	control_font_focus_color_role->set_lerp_color_role(base_color_role);
	control_font_focus_color_role->set_lerp_weight(0.125);

	const Color control_font_disabled_color = control_font_color * Color(1, 1, 1, 0.5);
	Ref<ColorRole> control_font_disabled_color_role = control_font_color_role->duplicate();
	control_font_disabled_color_role->set_color_scale(control_font_disabled_color_role->get_color_scale() * Color(1, 1, 1, 0.5));

	const Color control_font_placeholder_color = Color(control_font_color.r, control_font_color.g, control_font_color.b, 0.6f);
	Ref<ColorRole> control_font_placeholder_color_role = control_font_color_role->duplicate();
	control_font_placeholder_color_role->set_color_scale(Color(control_font_placeholder_color_role->get_color_scale(), 0.6));

	const Color control_font_pressed_color = Color(1, 1, 1);
	Ref<ColorRole> control_font_pressed_color_role = accent_color_role;

	const Color control_font_hover_pressed_color = control_font_hover_color.lerp(accent_color, 0.74);
	Ref<ColorRole> control_font_hover_pressed_color_role = accent_color_role->duplicate();
	control_font_hover_pressed_color_role->set_lerp_color_role(control_font_hover_color_role);
	control_font_hover_pressed_color_role->set_lerp_weight(0.26);


	const Color control_selection_color = Color(0.5, 0.5, 0.5);
	const Ref<ColorRole> control_selection_color_role = make_color_role(ColorRoleEnum::OUTLINE);

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

	const Ref<ColorRole> button_style_normal_color_role = make_color_role(ColorRoleEnum::PRIMARY);
	button_style_normal_color_role->set_color_scale(Color(1,1,1,0.6));

	const Ref<ColorRole> button_style_hover_color_role = make_color_role(ColorRoleEnum::PRIMARY);
	button_style_hover_color_role->set_lightened_amount(0.25);
	button_style_hover_color_role->set_color_scale(Color(1,1,1,0.6));

	const Ref<ColorRole> button_style_pressed_color_role = make_color_role(ColorRoleEnum::PRIMARY);
	button_style_pressed_color_role->set_darkened_amount(0.25);
	button_style_pressed_color_role->set_color_scale(Color(1,1,1,0.6));

	const Ref<ColorRole> button_style_disabled_color_role = make_color_role(ColorRoleEnum::PRIMARY);
	button_style_disabled_color_role->set_color_scale(Color(1,1,1,0.3));

	const Ref<ColorRole> button_style_focus_color_role = make_color_role(ColorRoleEnum::PRIMARY);
	button_style_focus_color_role->set_color_scale(Color(1,1,1,0.75));

	const Ref<StyleBoxFlat> button_style_normal = make_color_role_flat_stylebox(button_style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> button_style_hover = make_color_role_flat_stylebox(button_style_hover_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> button_style_pressed = make_color_role_flat_stylebox(button_style_pressed_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> button_style_disabled = make_color_role_flat_stylebox(button_style_disabled_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> button_style_focus = make_color_role_flat_stylebox(button_style_focus_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	button_style_focus->set_expand_margin_all(Math::round(2 * scale));
	button_style_focus->set_border_color_role(make_color_role(ColorRoleEnum::INVERSE_PRIMARY));


	Ref<ColorRole> button_mono_color_role = make_color_role(ColorRoleEnum::ON_PRIMARY);
	Ref<ColorRole> button_base_color_role = make_color_role(ColorRoleEnum::ON_SECONDARY);
	Ref<ColorRole> button_accent_color_role = make_color_role(ColorRoleEnum::INVERSE_PRIMARY);

	Ref<ColorRole> button_font_color_role = button_mono_color_role->duplicate();
	button_font_color_role->set_lerp_color_role(button_base_color_role);
	button_font_color_role->set_lerp_weight(0.25);

	Ref<ColorRole> button_font_hover_color_role = button_mono_color_role->duplicate();
	button_font_hover_color_role->set_lerp_color_role(button_base_color_role);
	button_font_hover_color_role->set_lerp_weight(0.125);

	Ref<ColorRole> button_font_focus_color_role = button_mono_color_role->duplicate();
	button_font_focus_color_role->set_lerp_color_role(button_base_color_role);
	button_font_focus_color_role->set_lerp_weight(0.125);

	Ref<ColorRole> button_font_disabled_color_role = button_mono_color_role->duplicate();
	button_font_disabled_color_role->set_color_scale(button_font_disabled_color_role->get_color_scale() * Color(1, 1, 1, 0.5));

	Ref<ColorRole> button_font_pressed_color_role = button_accent_color_role;

	Ref<ColorRole> button_font_hover_pressed_color_role = button_accent_color_role->duplicate();
	button_font_hover_pressed_color_role->set_lerp_color_role(button_font_hover_color_role);
	button_font_hover_pressed_color_role->set_lerp_weight(0.26);


	const Ref<ColorRole> style_normal_color_role = make_color_role(ColorRoleEnum::PRIMARY_CONTAINER);
	style_normal_color_role->set_color_scale(Color(1,1,1,0.6));

	const Ref<ColorRole> style_hover_color_role = make_color_role(ColorRoleEnum::PRIMARY_CONTAINER);
	style_hover_color_role->set_lightened_amount(0.25);
	style_hover_color_role->set_color_scale(Color(1,1,1,0.6));

	const Ref<ColorRole> style_pressed_color_role = make_color_role(ColorRoleEnum::PRIMARY_CONTAINER);
	style_pressed_color_role->set_darkened_amount(0.25);
	style_pressed_color_role->set_color_scale(Color(1,1,1,0.6));

	const Ref<ColorRole> style_disabled_color_role = make_color_role(ColorRoleEnum::PRIMARY_CONTAINER);
	style_disabled_color_role->set_color_scale(Color(1,1,1,0.3));

	const Ref<ColorRole> style_focus_color_role = make_color_role(ColorRoleEnum::PRIMARY_CONTAINER);
	style_focus_color_role->set_color_scale(Color(1,1,1,0.75));

	const Ref<StyleBoxFlat> style_normal = make_color_role_flat_stylebox(style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> style_hover = make_color_role_flat_stylebox(style_hover_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
	const Ref<StyleBoxFlat> style_pressed = make_color_role_flat_stylebox(style_pressed_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> style_disabled = make_color_role_flat_stylebox(style_disabled_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> style_focus = make_color_role_flat_stylebox(style_focus_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	style_focus->set_expand_margin_all(Math::round(2 * scale));

	const Ref<ColorRole> state_layer_style_hover_color_role = make_color_role(ColorRoleEnum::ON_PRIMARY);
	state_layer_style_hover_color_role->set_lightened_amount(0.25);
	state_layer_style_hover_color_role->set_color_scale(Color(1,1,1,0.08));

	const Ref<ColorRole> state_layer_style_pressed_color_role = make_color_role(ColorRoleEnum::ON_PRIMARY);
	state_layer_style_pressed_color_role->set_darkened_amount(0.25);
	state_layer_style_pressed_color_role->set_color_scale(Color(1,1,1,0.1));

	const Ref<ColorRole> state_layer_style_focus_color_role = make_color_role(ColorRoleEnum::ON_PRIMARY);
	state_layer_style_focus_color_role->set_color_scale(Color(1,1,1,0.1));

	const Ref<StyleBoxFlat> state_layer_style_hover = make_color_role_flat_stylebox(state_layer_style_hover_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> state_layer_style_pressed = make_color_role_flat_stylebox(state_layer_style_pressed_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	const Ref<StyleBoxFlat> state_layer_style_focus = make_color_role_flat_stylebox(state_layer_style_focus_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

	const Ref<ColorRole> style_popup_color_role = make_color_role(ColorRoleEnum::SECONDARY_CONTAINER);

	const Ref<ColorRole> style_popup_border_color_role = make_color_role(ColorRoleEnum::OUTLINE);

	const Ref<ColorRole> style_popup_hover_color_role = make_color_role(ColorRoleEnum::SECONDARY_CONTAINER);
	style_popup_hover_color_role->set_lightened_amount(0.25);

	const Ref<ColorRole> style_selected_color_role = make_color_role(ColorRoleEnum::TERTIARY);
	style_focus_color_role->set_color_scale(Color(1,1,1,0.3));

	// Don't use a color too bright to keep the percentage readable.
	const Ref<ColorRole> style_progress_color_role = make_color_role(ColorRoleEnum::TERTIARY);
	style_progress_color_role->set_color_scale(Color(1,1,1,0.4));

	const Ref<ColorRole> style_separator_color_role = make_color_role(ColorRoleEnum::OUTLINE);

	// Convert the generated icon sources to a dictionary for easier access.
	// Unlike the editor icons, there is no central repository of icons in the Theme resource itself to keep it tidy.
	Dictionary icons;
	for (int i = 0; i < default_theme_icons_count; i++) {
		icons[default_theme_icons_names[i]] = generate_icon(i);
	}

	// Window
	ThemeIntData cur_theme_data;
	{
		theme->set_color_scheme("default_color_scheme", "Window", default_color_scheme);

		const Ref<StyleBoxFlat> embedded_border_stylebox_normal = make_color_role_flat_stylebox(style_popup_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 10, 28, 10, 8);
		sb_expand(embedded_border_stylebox_normal, 8, 32, 8, 6);

		const Ref<StyleBoxFlat> embedded_unfocused_border_stylebox_normal = make_color_role_flat_stylebox(style_popup_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 10, 28, 10, 8);
		sb_expand(embedded_unfocused_border_stylebox_normal, 8, 32, 8, 6);

		cur_theme_data.set_data_name("embedded_border");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Window", embedded_unfocused_border_stylebox_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Window", embedded_border_stylebox_normal);

		theme->set_font("title_font", "Window", Ref<Font>());
		theme->set_font_size("title_font_size", "Window", -1);

		theme->set_color_role("title_color_role", "Window", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
		theme->set_color("title_color", "Window", Color(1, 1, 1, 1));

		theme->set_color_role("title_outline_modulate_role", "Window", make_color_role(ColorRoleEnum::OUTLINE));
		theme->set_color("title_outline_modulate", "Window", Color(1, 1, 1, 1));

		theme->set_constant("title_outline_size", "Window", 0);
		theme->set_constant("title_height", "Window", 36 * scale);
		theme->set_constant("resize_margin", "Window", Math::round(4 * scale));

		cur_theme_data.set_data_name("close");
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Window", icons["close"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Window", icons["close_hl"]);

		theme->set_constant("close_h_offset", "Window", 18 * scale);
		theme->set_constant("close_v_offset", "Window", 24 * scale);
	}

	// Window
	// fill_default_theme_window(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);

	// Panel
	// fill_default_theme_panel(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_stylebox("panel", "Panel", make_color_role_flat_stylebox(style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0));
	}

	// ColorRect
	{
		theme->set_color_scheme("default_color_scheme", "ColorRect", default_color_scheme);
	}

	// Button
	// fill_default_theme_button(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "Button", default_color_scheme);

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", button_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", button_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", button_style_disabled);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", button_style_focus);

		cur_theme_data.set_data_name("state_layer_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", state_layer_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", state_layer_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", state_layer_style_focus);

		theme->set_font("font", "Button", default_font);
		theme->set_font("text_icon_font", "Button", default_icon_font);

		theme->set_font_size("font_size", "Button", -1);
		theme->set_font_size("text_icon_font_size", "Button", -1);
		theme->set_constant("font_outline_size", "Button", 0);

		theme->set_constant("icon_max_width", "Button", 0);
		theme->set_constant("h_separation", "Button", Math::round(4 * scale));

		Ref<ColorRole> button_mono_color_role = make_color_role(ColorRoleEnum::ON_PRIMARY);
		Ref<ColorRole> button_base_color_role = make_color_role(ColorRoleEnum::ON_SECONDARY);
		Ref<ColorRole> button_accent_color_role = make_color_role(ColorRoleEnum::INVERSE_PRIMARY);

		cur_theme_data.set_data_name("text_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "Button", button_font_hover_pressed_color_role);

		cur_theme_data.set_data_name("text_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "Button", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", make_color_role(ColorRoleEnum::STATIC_COLOR));

		cur_theme_data.set_data_name("icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", Color(1, 1, 1, 0.4));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", Color(1, 1, 1, 1));

		theme->set_color_role("font_outline_color_role", "Button", make_color_role(ColorRoleEnum::OUTLINE));
		theme->set_color("font_outline_color", "Button", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "Button", button_font_hover_pressed_color_role);


		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Button", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Button", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "Button", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("icon");
		Ref<Texture2D> empty_icon = memnew(ImageTexture);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Button", empty_icon);
	}

	// fill_default_theme_elevated_button(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);

	// TextureButton
	// fill_default_theme_texture_button(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "TextureButton", default_color_scheme);

		Ref<StyleBoxFlat> texture_button_default_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		texture_button_default_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TextureButton", make_empty_stylebox(0, 0, 0, 0));
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TextureButton", make_empty_stylebox(0, 0, 0, 0));
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "TextureButton", make_empty_stylebox(0, 0, 0, 0));
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TextureButton", make_empty_stylebox(0, 0, 0, 0));
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TextureButton", texture_button_default_stylebox_focus);

		cur_theme_data.set_data_name("state_layer_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TextureButton", state_layer_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "TextureButton", state_layer_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TextureButton", state_layer_style_focus);

		theme->set_font("text_icon_font", "TextureButton", default_icon_font);

		theme->set_font_size("text_icon_font_size", "TextureButton", -1);

		theme->set_constant("icon_max_width", "TextureButton", 0);

		cur_theme_data.set_data_name("text_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TextureButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TextureButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "TextureButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TextureButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TextureButton", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "TextureButton", button_font_hover_pressed_color_role);

		cur_theme_data.set_data_name("text_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TextureButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TextureButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "TextureButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TextureButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TextureButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "TextureButton", Color(1, 1, 1, 1));
	}

	// OptionButton
	// fill_default_theme_option_button(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "OptionButton", default_color_scheme);

		const Ref<StyleBox> option_button_style_normal = make_color_role_flat_stylebox(button_style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2 * default_margin, default_margin, 21, default_margin);
		
		const Ref<StyleBox> option_button_style_hover = make_color_role_flat_stylebox(button_style_hover_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2 * default_margin, default_margin, 21, default_margin);

		const Ref<StyleBox> option_button_style_pressed = make_color_role_flat_stylebox(button_style_pressed_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2 * default_margin, default_margin, 21, default_margin);

		const Ref<StyleBox> option_button_style_disabled = make_color_role_flat_stylebox(button_style_disabled_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2 * default_margin, default_margin, 21, default_margin);


		const Ref<StyleBox> option_button_style_normal_mirrored = make_color_role_flat_stylebox(button_style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 21, default_margin, 2 * default_margin, default_margin);

		const Ref<StyleBox> option_button_style_hover_mirrored = make_color_role_flat_stylebox(button_style_hover_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 21, default_margin, 2 * default_margin, default_margin);


		const Ref<StyleBox> option_button_style_pressed_mirrored = make_color_role_flat_stylebox(button_style_pressed_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 21, default_margin, 2 * default_margin, default_margin);


		const Ref<StyleBox> option_button_style_disabled_mirrored = make_color_role_flat_stylebox(button_style_disabled_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 21, default_margin, 2 * default_margin, default_margin);

		const Ref<StyleBoxFlat> option_button_style_focus = make_color_role_flat_stylebox(button_style_focus_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		option_button_style_focus->set_expand_margin_all(Math::round(2 * scale));

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", option_button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", option_button_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", option_button_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", option_button_style_disabled);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneRTL), "OptionButton", option_button_style_normal_mirrored);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneRTL), "OptionButton", option_button_style_hover_mirrored);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneRTL), "OptionButton", option_button_style_pressed_mirrored);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneRTL), "OptionButton", option_button_style_disabled_mirrored);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", option_button_style_focus);

		cur_theme_data.set_data_name("state_layer_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", state_layer_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", state_layer_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", state_layer_style_focus);

		theme->set_font("font", "OptionButton", default_font);
		theme->set_font("text_icon_font", "OptionButton", default_icon_font);

		theme->set_font_size("font_size", "OptionButton", -1);
		theme->set_font_size("text_icon_font_size", "OptionButton", -1);
		theme->set_constant("font_outline_size", "OptionButton", 0);

		theme->set_constant("icon_max_width", "OptionButton", 0);
		theme->set_constant("h_separation", "OptionButton", Math::round(4 * scale));

		cur_theme_data.set_data_name("text_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "OptionButton", button_font_hover_pressed_color_role);

		cur_theme_data.set_data_name("text_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "OptionButton", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

		cur_theme_data.set_data_name("icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", Color(1, 1, 1, 0.4));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", Color(1, 1, 1, 1));

		theme->set_color_role("font_outline_color_role", "OptionButton", make_color_role(ColorRoleEnum::OUTLINE));
		theme->set_color("font_outline_color", "OptionButton", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "OptionButton", button_font_hover_pressed_color_role);

		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "OptionButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "OptionButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "OptionButton", Color(1, 1, 1, 1));

		theme->set_icon("arrow", "OptionButton", icons["option_button_arrow"]);
		theme->set_constant("arrow_margin", "OptionButton", Math::round(4 * scale));
		theme->set_constant("modulate_arrow", "OptionButton", false);
	}


	// ScrollContainer
	// fill_default_theme_scroll_container(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_stylebox("panel", "ScrollContainer", make_empty_stylebox(0,0,0,0));
	}

	// Separators
	// fill_default_theme_separators(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		Ref<StyleBoxLine> separator_horizontal = memnew(StyleBoxLine);
		separator_horizontal->set_thickness(Math::round(scale));
		separator_horizontal->set_color(Color(0.5, 0.5, 0.5));
		separator_horizontal->set_content_margin_individual(default_margin, 0, default_margin, 0);
		separator_horizontal->set_color_scheme(default_color_scheme);
		separator_horizontal->set_color_role(make_color_role(ColorRoleEnum::OUTLINE_VARIANT));

		Ref<StyleBoxLine> separator_vertical = separator_horizontal->duplicate();
		separator_vertical->set_vertical(true);
		separator_vertical->set_content_margin_individual(0, default_margin, 0, default_margin);

		cur_theme_data.set_data_name("separator");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "HSeparator", separator_horizontal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "VSeparator", separator_vertical);

		theme->set_icon("close", "Icons", icons["close"]);
		theme->set_font("normal", "Fonts", Ref<Font>());
		theme->set_font("large", "Fonts", Ref<Font>());

		theme->set_constant("separation", "HSeparator", Math::round(4 * scale));
		theme->set_constant("separation", "VSeparator", Math::round(4 * scale));
	}


	// Scrollbars
	// fill_default_theme_scrollbar(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		const Ref<StyleBoxFlat> style_h_scrollbar = make_color_role_flat_stylebox(accent_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 4, 0, 4, 10);
		const Ref<StyleBoxFlat> style_v_scrollbar = make_color_role_flat_stylebox(accent_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 0, 4, 0, 10);
		
		const Ref<ColorRole> style_scrollbar_grabber_normal_role = make_color_role(ColorRoleEnum::INVERSE_PRIMARY);
		style_scrollbar_grabber_normal_role->set_color_scale(Color(1,1,1,0.6));

		const Ref<ColorRole> style_scrollbar_grabber_hover_role = make_color_role(ColorRoleEnum::INVERSE_PRIMARY);
		style_scrollbar_grabber_hover_role->set_lightened_amount(0.25);
		style_scrollbar_grabber_hover_role->set_color_scale(Color(1,1,1,0.6));

		const Ref<ColorRole> style_scrollbar_grabber_pressed_role = make_color_role(ColorRoleEnum::INVERSE_PRIMARY);
		style_scrollbar_grabber_pressed_role->set_darkened_amount(0.25);
		style_scrollbar_grabber_pressed_role->set_color_scale(Color(1,1,1,0.6));

		const Ref<StyleBoxFlat> style_scrollbar_grabber = make_color_role_flat_stylebox(style_scrollbar_grabber_normal_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4, 10);
		const Ref<StyleBoxFlat> style_scrollbar_grabber_highlight = make_color_role_flat_stylebox(style_scrollbar_grabber_hover_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4, 10);
		const Ref<StyleBoxFlat> style_scrollbar_grabber_pressed = make_color_role_flat_stylebox(style_scrollbar_grabber_pressed_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4, 10);

		Ref<StyleBoxFlat> default_scrollbar_stylebox_focus = make_color_role_flat_stylebox(accent_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		default_scrollbar_stylebox_focus->set_expand_margin_all(Math::round(2 * scale));
		default_scrollbar_stylebox_focus->set_border_color_role(make_color_role(ColorRoleEnum::INVERSE_PRIMARY));

		Ref<Texture2D> empty_icon = memnew(ImageTexture);

		// HScrollBar
		cur_theme_data.set_data_name("scroll_style");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "HScrollBar", style_h_scrollbar);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "HScrollBar", default_scrollbar_stylebox_focus);
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
		cur_theme_data.set_data_name("scroll_style");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "VScrollBar", style_v_scrollbar);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "VScrollBar", default_scrollbar_stylebox_focus);
		theme->set_stylebox("grabber", "VScrollBar", style_scrollbar_grabber);
		theme->set_stylebox("grabber_highlight", "VScrollBar", style_scrollbar_grabber_highlight);
		theme->set_stylebox("grabber_pressed", "VScrollBar", style_scrollbar_grabber_pressed);

		theme->set_icon("increment", "VScrollBar", empty_icon);
		theme->set_icon("increment_highlight", "VScrollBar", empty_icon);
		theme->set_icon("increment_pressed", "VScrollBar", empty_icon);
		theme->set_icon("decrement", "VScrollBar", empty_icon);
		theme->set_icon("decrement_highlight", "VScrollBar", empty_icon);
		theme->set_icon("decrement_pressed", "VScrollBar", empty_icon);
	}

	// Label
	// fill_default_theme_label(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "Label", default_color_scheme);

		theme->set_stylebox("normal", "Label", memnew(StyleBoxEmpty));

		theme->set_font("font", "Label", default_font);

		theme->set_font_size("font_size", "Label", -1);
		theme->set_type_variation("HeaderSmall", "Label");
		theme->set_font_size("font_size", "HeaderSmall", default_font_size + 4);

		theme->set_type_variation("HeaderMedium", "Label");
		theme->set_font_size("font_size", "HeaderMedium", default_font_size + 8);

		theme->set_type_variation("HeaderLarge", "Label");
		theme->set_font_size("font_size", "HeaderLarge", default_font_size + 12);

		theme->set_constant("line_spacing", "Label", Math::round(3 * scale));

		theme->set_color_role("font_color_role", "Label", button_font_color_role);
		theme->set_color("font_color", "Label", Color(1, 1, 1));

		theme->set_color_role("font_shadow_color_role", "Label", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("font_shadow_color", "Label", Color(0, 0, 0, 0));

		theme->set_color_role("font_outline_color_role", "Label", outline_color_role);
		theme->set_color("font_outline_color", "Label", Color(1, 1, 1));

		theme->set_constant("shadow_offset_x", "Label", Math::round(1 * scale));
		theme->set_constant("shadow_offset_y", "Label", Math::round(1 * scale));
		theme->set_constant("outline_size", "Label", 0);
		theme->set_constant("shadow_outline_size", "Label", Math::round(1 * scale));

		theme->set_type_variation("TooltipLabel", "Label");
		theme->set_font_size("font_size", "TooltipLabel", -1);
		theme->set_font("font", "TooltipLabel", Ref<Font>());

		theme->set_color_role("font_color_role", "TooltipLabel", button_font_color_role);
		theme->set_color("font_color", "TooltipLabel", Color(1, 1, 1));

		theme->set_color_role("font_shadow_color_role", "TooltipLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("font_shadow_color", "TooltipLabel", Color(1, 1, 1));

		theme->set_color_role("font_outline_color_role", "TooltipLabel", outline_color_role);
		theme->set_color("font_outline_color", "TooltipLabel", Color(1, 1, 1));

		theme->set_constant("shadow_offset_x", "TooltipLabel", 1);
		theme->set_constant("shadow_offset_y", "TooltipLabel", 1);
		theme->set_constant("outline_size", "TooltipLabel", 0);
	}

	// RichTextLabel
	// fill_default_theme_rich_text_label(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "RichTextLabel", default_color_scheme);

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "RichTextLabel", make_empty_stylebox(0, 0, 0, 0));
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "RichTextLabel", button_style_focus);

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

		theme->set_color_role("default_color_role", "RichTextLabel", button_font_color_role);
		theme->set_color("default_color", "RichTextLabel", Color(1, 1, 1, 1));

		Ref<ColorRole> font_selected_color_role = button_font_color_role->duplicate();
		font_selected_color_role->set_lightened_amount(0.2);
		theme->set_color_role("font_selected_color_role", "RichTextLabel", font_selected_color_role);
		theme->set_color("font_selected_color", "RichTextLabel", Color(0, 0, 0, 0));

		Ref<ColorRole> selection_color_role = make_color_role(ColorRoleEnum::PRIMARY,Color(1,1,1,0.8));
		selection_color_role->set_darkened_amount(0.2);
		theme->set_color_role("selection_color_role", "RichTextLabel", selection_color_role);
		theme->set_color("selection_color", "RichTextLabel", Color(0.1, 0.1, 1, 0.8));

		theme->set_color_role("font_shadow_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("font_shadow_color", "RichTextLabel", Color(0, 0, 0, 0));

		theme->set_color_role("font_outline_color_role", "RichTextLabel", outline_color_role);
		theme->set_color("font_outline_color", "RichTextLabel", Color(1, 1, 1, 1));

		theme->set_color_role("outline_color_role", "RichTextLabel", outline_color_role);
		theme->set_color("outline_color", "RichTextLabel", Color(1, 1, 1));

		theme->set_constant("shadow_offset_x", "RichTextLabel", Math::round(1 * scale));
		theme->set_constant("shadow_offset_y", "RichTextLabel", Math::round(1 * scale));
		theme->set_constant("shadow_outline_size", "RichTextLabel", Math::round(1 * scale));

		theme->set_constant("line_separation", "RichTextLabel", 0);
		theme->set_constant("table_h_separation", "RichTextLabel", Math::round(3 * scale));
		theme->set_constant("table_v_separation", "RichTextLabel", Math::round(3 * scale));

		theme->set_constant("outline_size", "RichTextLabel", 0);

		theme->set_color_role("table_odd_row_bg_role", "RichTextLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("table_odd_row_bg", "RichTextLabel", Color(0, 0, 0, 0));

		theme->set_color_role("table_even_row_bg_role", "RichTextLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("table_even_row_bg", "RichTextLabel", Color(0, 0, 0, 0));

		theme->set_color_role("table_border_role", "RichTextLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("table_border", "RichTextLabel", Color(0, 0, 0, 0));

		theme->set_constant("text_highlight_h_padding", "RichTextLabel", Math::round(3 * scale));
		theme->set_constant("text_highlight_v_padding", "RichTextLabel", Math::round(3 * scale));
	}


	// MenuBar
	// fill_default_theme_menu_bar(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "MenuBar", default_color_scheme);

		const Ref<StyleBoxFlat> state_layer_stylebox_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.08)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
		const Ref<StyleBoxFlat> state_layer_stylebox_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
		Ref<StyleBoxFlat> state_layer_stylebox_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY, Color(1, 1, 1, 0.1)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuBar", button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuBar", button_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuBar", button_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuBar", button_style_disabled);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuBar", button_style_focus);

		cur_theme_data.set_data_name("state_layer_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuBar", state_layer_style_hover_color_role);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuBar", state_layer_style_pressed_color_role);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuBar", state_layer_style_focus_color_role);

		theme->set_font("font", "MenuBar", default_font);

		theme->set_font_size("font_size", "MenuBar", -1);
		theme->set_constant("outline_size", "MenuBar", 0);

		theme->set_constant("h_separation", "MenuBar", Math::round(4 * scale));

		theme->set_color_role("font_outline_color_role", "MenuBar", make_color_role(ColorRoleEnum::OUTLINE));
		theme->set_color("font_outline_color", "MenuBar", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuBar", button_font_color_role);
		Ref<ColorRole> button_font_color_role_checked = button_font_color_role->duplicate();
		button_font_color_role_checked->set_lightened_amount(0.5);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "MenuBar", button_font_color_role_checked);

		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuBar", button_font_hover_color_role);
		Ref<ColorRole> button_font_hover_color_role_checked = button_font_hover_color_role->duplicate();
		button_font_hover_color_role_checked->set_lightened_amount(0.5);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverCheckedLTR), "MenuBar", button_font_hover_color_role_checked);

		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuBar", button_font_pressed_color_role);
		Ref<ColorRole> button_font_pressed_color_role_checked = button_font_pressed_color_role->duplicate();
		button_font_pressed_color_role_checked->set_lightened_amount(0.5);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedCheckedLTR), "MenuBar", button_font_pressed_color_role_checked);

		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuBar", button_font_disabled_color_role);

		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuBar", button_font_focus_color_role);
		Ref<ColorRole> button_font_focus_color_role_checked = button_font_focus_color_role->duplicate();
		button_font_focus_color_role_checked->set_lightened_amount(0.5);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusCheckedLTR), "MenuBar", button_font_focus_color_role_checked);

		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "MenuBar", button_font_hover_pressed_color_role);
		Ref<ColorRole> button_font_hover_pressed_color_role_checked = button_font_hover_pressed_color_role->duplicate();
		button_font_hover_pressed_color_role_checked->set_lightened_amount(0.5);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedCheckedLTR), "MenuBar", button_font_hover_pressed_color_role_checked);

		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuBar", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuBar", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuBar", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuBar", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuBar", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "MenuBar", Color(1, 1, 1, 1));
	}



	// LinkButton
	// fill_default_theme_link_button(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "LinkButton", default_color_scheme);

		theme->set_stylebox("focus", "LinkButton", button_style_focus);

		cur_theme_data.set_data_name("state_layer_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "LinkButton", state_layer_style_hover_color_role);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "LinkButton", state_layer_style_pressed_color_role);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "LinkButton", state_layer_style_focus_color_role);

		theme->set_font("font", "LinkButton", default_font);

		theme->set_font_size("font_size", "LinkButton", -1);
		theme->set_constant("outline_size", "LinkButton", 0);

		theme->set_constant("underline_spacing", "LinkButton", Math::round(2 * scale));

		theme->set_color_role("font_outline_color_role", "LinkButton", outline_color_role);
		theme->set_color("font_outline_color", "LinkButton", Color(0, 0, 0, 1));

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LinkButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "LinkButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "LinkButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "LinkButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "LinkButton", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "LinkButton", button_font_hover_pressed_color_role);


		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LinkButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "LinkButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "LinkButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "LinkButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "LinkButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "LinkButton", Color(1, 1, 1, 1));
	}


	// MenuButton
	// fill_default_theme_menu_button(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "MenuButton", default_color_scheme);

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", button_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", button_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", button_style_disabled);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", button_style_focus);

		cur_theme_data.set_data_name("state_layer_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", state_layer_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", state_layer_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", state_layer_style_focus);

		theme->set_font("font", "MenuButton", default_font);
		theme->set_font("text_icon_font", "MenuButton", default_icon_font);

		theme->set_font_size("font_size", "MenuButton", -1);
		theme->set_font_size("text_icon_font_size", "MenuButton", -1);
		theme->set_constant("font_outline_size", "MenuButton", 0);

		theme->set_constant("icon_max_width", "MenuButton", 0);
		theme->set_constant("h_separation", "MenuButton", Math::round(4 * scale));

		cur_theme_data.set_data_name("text_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "MenuButton", button_font_hover_pressed_color_role);

		cur_theme_data.set_data_name("text_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "MenuButton", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "MenuButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

		cur_theme_data.set_data_name("icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", Color(1, 1, 1, 0.4));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "MenuButton", Color(1, 1, 1, 1));

		theme->set_color_role("font_outline_color_role", "MenuButton", make_color_role(ColorRoleEnum::OUTLINE));
		theme->set_color("font_outline_color", "MenuButton", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "MenuButton", button_font_hover_pressed_color_role);


		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "MenuButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "MenuButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "MenuButton", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("icon");
		Ref<Texture2D> empty_icon = memnew(ImageTexture);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "MenuButton", empty_icon);
	}


	// PopupMenu
	// fill_default_theme_popup_menu(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "PopupMenu", default_color_scheme);
		
		const Ref<ColorRole> popup_menu_style_normal_color_role = make_color_role(ColorRoleEnum::SECONDARY_CONTAINER);
		popup_menu_style_normal_color_role->set_color_scale(Color(1,1,1,0.6));

		const Ref<ColorRole> popup_menu_style_hover_color_role = make_color_role(ColorRoleEnum::SECONDARY_CONTAINER);
		popup_menu_style_hover_color_role->set_lightened_amount(0.25);
		popup_menu_style_hover_color_role->set_color_scale(Color(1,1,1,0.6));


		const Ref<ColorRole> popup_menu_style_disabled_color_role = make_color_role(ColorRoleEnum::SECONDARY_CONTAINER);
		popup_menu_style_disabled_color_role->set_color_scale(Color(1,1,1,0.3));

		const Ref<ColorRole> popup_menu_style_focus_color_role = make_color_role(ColorRoleEnum::SECONDARY_CONTAINER);
		popup_menu_style_focus_color_role->set_color_scale(Color(1,1,1,0.75));


		const Ref<StyleBoxFlat> popup_menu_style_normal = make_color_role_flat_stylebox(popup_menu_style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
		popup_menu_style_normal->set_border_width_all(2);
		popup_menu_style_normal->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

		const Ref<StyleBoxFlat> popup_menu_style_hover = make_color_role_flat_stylebox(popup_menu_style_hover_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_1, default_color_scheme);
		popup_menu_style_hover->set_border_width_all(2);
		popup_menu_style_hover->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

		const Ref<StyleBoxFlat> popup_menu_style_disabled = make_color_role_flat_stylebox(popup_menu_style_disabled_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
		popup_menu_style_disabled->set_border_width_all(2);
		popup_menu_style_disabled->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));
		
		const Ref<StyleBoxFlat> popup_menu_style_focus = make_color_role_flat_stylebox(popup_menu_style_focus_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		popup_menu_style_focus->set_expand_margin_all(Math::round(2 * scale));

		const Ref<ColorRole> popup_menu_state_layer_style_hover_color_role = make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER);
		popup_menu_state_layer_style_hover_color_role->set_lightened_amount(0.25);
		popup_menu_state_layer_style_hover_color_role->set_color_scale(Color(1,1,1,0.08));

		const Ref<ColorRole> popup_menu_state_layer_style_focus_color_role = make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER);
		popup_menu_state_layer_style_focus_color_role->set_color_scale(Color(1,1,1,0.1));

		const Ref<StyleBoxFlat> popup_menu_state_layer_style_hover = make_color_role_flat_stylebox(popup_menu_state_layer_style_hover_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

		const Ref<StyleBoxFlat> popup_menu_state_layer_style_focus = make_color_role_flat_stylebox(popup_menu_state_layer_style_focus_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "PopupMenu", popup_menu_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "PopupMenu", popup_menu_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "PopupMenu", popup_menu_style_disabled);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "PopupMenu", popup_menu_style_focus);

		cur_theme_data.set_data_name("state_layer_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "PopupMenu", popup_menu_state_layer_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "PopupMenu", popup_menu_state_layer_style_focus);

		Ref<StyleBoxLine> separator_horizontal = memnew(StyleBoxLine);
		separator_horizontal->set_thickness(Math::round(scale));
		separator_horizontal->set_color_role(make_color_role(ColorRoleEnum::OUTLINE));
		separator_horizontal->set_content_margin_individual(default_margin, 0, default_margin, 0);
		Ref<StyleBoxLine> separator_vertical = separator_horizontal->duplicate();
		separator_vertical->set_vertical(true);
		separator_vertical->set_content_margin_individual(0, default_margin, 0, default_margin);

		theme->set_stylebox("separator", "PopupMenu", separator_horizontal);
		theme->set_stylebox("labeled_separator_left", "PopupMenu", separator_horizontal);
		theme->set_stylebox("labeled_separator_right", "PopupMenu", separator_horizontal);

		cur_theme_data.set_data_name("icon");
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "PopupMenu", icons["checked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "PopupMenu", icons["checked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "PopupMenu", icons["unchecked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "PopupMenu", icons["unchecked"]);
		cur_theme_data.set_data_name("radio_icon");
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "PopupMenu", icons["radio_checked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "PopupMenu", icons["radio_checked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "PopupMenu", icons["radio_unchecked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "PopupMenu", icons["radio_unchecked"]);
		cur_theme_data.set_data_name("submenu");
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "PopupMenu", icons["popup_menu_arrow_right"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneRTL), "PopupMenu", icons["popup_menu_arrow_left"]);

		theme->set_font("font", "PopupMenu", Ref<Font>());
		theme->set_font("font_separator", "PopupMenu", Ref<Font>());
		theme->set_font_size("font_size", "PopupMenu", -1);
		theme->set_font_size("font_separator_size", "PopupMenu", -1);

		theme->set_constant("indent", "PopupMenu", Math::round(10 * scale));
		theme->set_constant("h_separation", "PopupMenu", Math::round(4 * scale));
		theme->set_constant("v_separation", "PopupMenu", Math::round(4 * scale));
		theme->set_constant("outline_size", "PopupMenu", 0);
		theme->set_constant("separator_outline_size", "PopupMenu", 0);
		theme->set_constant("item_start_padding", "PopupMenu", Math::round(2 * scale));
		theme->set_constant("item_end_padding", "PopupMenu", Math::round(2 * scale));
		theme->set_constant("icon_max_width", "PopupMenu", 0);

		Ref<ColorRole> popup_menu_mono_color_role = make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER);
		Ref<ColorRole> popup_menu_base_color_role = make_color_role(ColorRoleEnum::ON_PRIMARY);
		Ref<ColorRole> popup_menu_accent_color_role = make_color_role(ColorRoleEnum::INVERSE_PRIMARY);

		Ref<ColorRole> popup_menu_font_color_role = popup_menu_mono_color_role->duplicate();
		popup_menu_font_color_role->set_lerp_color_role(popup_menu_base_color_role);
		popup_menu_font_color_role->set_lerp_weight(0.25);

		Ref<ColorRole> popup_menu_font_hover_color_role = popup_menu_mono_color_role->duplicate();
		popup_menu_font_hover_color_role->set_lerp_color_role(popup_menu_base_color_role);
		popup_menu_font_hover_color_role->set_lerp_weight(0.125);

		Ref<ColorRole> popup_menu_font_focus_color_role = popup_menu_mono_color_role->duplicate();
		popup_menu_font_focus_color_role->set_lerp_color_role(popup_menu_base_color_role);
		popup_menu_font_focus_color_role->set_lerp_weight(0.125);

		Ref<ColorRole> popup_menu_font_disabled_color_role = popup_menu_mono_color_role->duplicate();
		popup_menu_font_disabled_color_role->set_color_scale(popup_menu_font_disabled_color_role->get_color_scale() * Color(1, 1, 1, 0.5));

		Ref<ColorRole> popup_menu_font_pressed_color_role = popup_menu_accent_color_role;

		Ref<ColorRole> popup_menu_font_hover_pressed_color_role = popup_menu_accent_color_role->duplicate();
		popup_menu_font_hover_pressed_color_role->set_lerp_color_role(popup_menu_font_hover_color_role);
		popup_menu_font_hover_pressed_color_role->set_lerp_weight(0.26);

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "PopupMenu", popup_menu_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "PopupMenu", popup_menu_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "PopupMenu", popup_menu_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "PopupMenu", popup_menu_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "PopupMenu", popup_menu_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "PopupMenu", popup_menu_font_hover_pressed_color_role);

		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "PopupMenu", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "PopupMenu", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "PopupMenu", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "PopupMenu", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "PopupMenu", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "PopupMenu", Color(1, 1, 1, 1));


		theme->set_color_role("font_accelerator_color_role", "PopupMenu", make_color_role(ColorRoleEnum::ON_SECONDARY));
		theme->set_color("font_accelerator_color", "PopupMenu", Color(1, 1, 1, 1));

		theme->set_color_role("font_outline_color_role", "PopupMenu", make_color_role(ColorRoleEnum::OUTLINE));
		theme->set_color("font_outline_color", "PopupMenu", Color(1, 1, 1, 1));

		theme->set_color_role("font_separator_color_role", "PopupMenu", make_color_role(ColorRoleEnum::OUTLINE));
		theme->set_color("font_separator_color", "PopupMenu", Color(1, 1, 1, 1));

		theme->set_color_role("font_separator_outline_color_role", "PopupMenu", make_color_role(ColorRoleEnum::OUTLINE_VARIANT));
		theme->set_color("font_separator_outline_color", "PopupMenu", Color(1, 1, 1, 1));
	}

	// CheckBox
	// fill_default_theme_checkbox(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "CheckBox", default_color_scheme);

		const Ref<StyleBox> cbx_empty = memnew(StyleBoxEmpty);
		cbx_empty->set_content_margin_all(Math::round(4 * scale));

		const Ref<StyleBoxFlat> check_box_style_focus = make_color_role_flat_stylebox(style_focus_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		check_box_style_focus->set_expand_margin_all(Math::round(2 * scale));
		check_box_style_focus->set_content_margin_all(Math::round(4 * scale));

		const Ref<StyleBoxFlat> check_box_state_layer_style_hover = state_layer_style_hover->duplicate();
		check_box_state_layer_style_hover->set_content_margin_all(Math::round(4 * scale));

		const Ref<StyleBoxFlat> check_box_state_layer_style_pressed = state_layer_style_pressed->duplicate();
		check_box_state_layer_style_pressed->set_content_margin_all(Math::round(4 * scale));

		const Ref<StyleBoxFlat> check_box_state_layer_style_focus = state_layer_style_focus->duplicate();
		check_box_state_layer_style_focus->set_content_margin_all(Math::round(4 * scale));

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", cbx_empty);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", cbx_empty);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", cbx_empty);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", cbx_empty);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", check_box_style_focus);

		cur_theme_data.set_data_name("state_layer_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", check_box_state_layer_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", check_box_state_layer_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", check_box_state_layer_style_focus);

		theme->set_font("font", "CheckBox", default_font);
		theme->set_font("text_icon_font", "CheckBox", default_icon_font);

		theme->set_font_size("font_size", "CheckBox", -1);
		theme->set_font_size("text_icon_font_size", "CheckBox", -1);
		theme->set_constant("font_outline_size", "CheckBox", 0);

		theme->set_constant("icon_max_width", "CheckBox", 0);
		theme->set_constant("h_separation", "CheckBox", Math::round(4 * scale));

		cur_theme_data.set_data_name("text_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckBox", button_font_hover_pressed_color_role);


		cur_theme_data.set_data_name("text_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckBox", make_color_role(ColorRoleEnum::STATIC_COLOR));


		cur_theme_data.set_data_name("icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", Color(1, 1, 1, 0.4));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));

		theme->set_color_role("font_outline_color_role", "CheckBox", outline_color_role);
		theme->set_color("font_outline_color", "CheckBox", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckBox", button_font_hover_pressed_color_role);


		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("icon");
		Ref<Texture2D> empty_icon = memnew(ImageTexture);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", empty_icon);

		theme->set_constant("check_v_offset", "CheckBox", 0);

		cur_theme_data.set_data_name("check_icon");
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckBox", icons["checked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckBox", icons["unchecked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckBox", icons["checked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckBox", icons["unchecked"]);

		cur_theme_data.set_data_name("radio_check_icon");
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckBox", icons["radio_checked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckBox", icons["radio_unchecked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckBox", icons["radio_checked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckBox", icons["radio_unchecked"]);

		cur_theme_data.set_data_name("check_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckBox", button_font_hover_pressed_color_role);


		cur_theme_data.set_data_name("check_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("text_check_icon");
		theme->set_str(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckBox", "checkbox-marked");
		theme->set_str(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckBox", "checkbox-blank-outline");
		theme->set_str(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckBox", "checkbox-marked");
		theme->set_str(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckBox", "checkbox-blank-outline");

		cur_theme_data.set_data_name("text_radio_check_icon");
		theme->set_str(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckBox", "radiobox-marked");
		theme->set_str(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckBox", "radiobox-blank");
		theme->set_str(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckBox", "radiobox-marked");
		theme->set_str(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckBox", "radiobox-blank");

		cur_theme_data.set_data_name("text_check_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckBox", button_font_hover_pressed_color_role);

		cur_theme_data.set_data_name("text_check_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckBox", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckBox", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckBox", Color(1, 1, 1, 1));
	}

	// CheckButton
	// fill_default_theme_check_button(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "CheckBox", default_color_scheme);

		Ref<StyleBox> cb_empty = memnew(StyleBoxEmpty);
		cb_empty->set_content_margin_individual(Math::round(6 * scale), Math::round(4 * scale), 
		Math::round(6 * scale), Math::round(4 * scale));

		const Ref<StyleBoxFlat> check_button_style_focus = make_color_role_flat_stylebox(style_focus_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
		check_button_style_focus->set_expand_margin_all(Math::round(2 * scale));


		const Ref<StyleBoxFlat> check_button_state_layer_style_hover = state_layer_style_hover->duplicate();
		check_button_state_layer_style_hover->set_content_margin_all(Math::round(4 * scale));

		const Ref<StyleBoxFlat> check_button_state_layer_style_pressed = state_layer_style_pressed->duplicate();
		check_button_state_layer_style_pressed->set_content_margin_all(Math::round(4 * scale));

		const Ref<StyleBoxFlat> check_button_state_layer_style_focus = state_layer_style_focus->duplicate();
		check_button_state_layer_style_focus->set_content_margin_all(Math::round(4 * scale));

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", cb_empty);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", cb_empty);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", cb_empty);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", cb_empty);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", check_button_style_focus);

		cur_theme_data.set_data_name("state_layer_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", check_button_state_layer_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", check_button_state_layer_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", check_button_state_layer_style_focus);

		theme->set_font("font", "CheckButton", default_font);
		theme->set_font("text_icon_font", "CheckButton", default_icon_font);

		theme->set_font_size("font_size", "CheckButton", -1);
		theme->set_font_size("text_icon_font_size", "CheckButton", -1);
		theme->set_constant("font_outline_size", "CheckButton", 0);

		theme->set_constant("icon_max_width", "CheckButton", 0);
		theme->set_constant("h_separation", "CheckButton", Math::round(4 * scale));

		cur_theme_data.set_data_name("text_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckButton", button_font_hover_pressed_color_role);

		cur_theme_data.set_data_name("text_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

		cur_theme_data.set_data_name("icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", Color(1, 1, 1, 0.4));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));

		theme->set_color_role("font_outline_color_role", "CheckButton", outline_color_role);
		theme->set_color("font_outline_color", "CheckButton", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckButton", button_font_hover_pressed_color_role);


		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverPressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("icon");
		Ref<Texture2D> empty_icon = memnew(ImageTexture);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", empty_icon);

		theme->set_constant("check_v_offset", "CheckButton", 0);

		cur_theme_data.set_data_name("check_icon");
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckButton", icons["toggle_on"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckButton", icons["toggle_off"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckButton", icons["toggle_on_disabled"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckButton", icons["toggle_off_disabled"]);

		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedRTL), "CheckButton", icons["toggle_on_mirrored"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedRTL), "CheckButton", icons["toggle_off_mirrored"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedRTL), "CheckButton", icons["toggle_on_disabled_mirrored"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedRTL), "CheckButton", icons["toggle_off_disabled_mirrored"]);

		cur_theme_data.set_data_name("check_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", button_font_focus_color_role);

		cur_theme_data.set_data_name("check_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("text_check_icon");
		theme->set_str(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckButton", "toggle-switch");
		theme->set_str(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckButton", "toggle-switch-off-outline");

		theme->set_str(cur_theme_data.get_state_data_name(State::NormalCheckedRTL), "CheckButton", "toggle-switch-off");
		theme->set_str(cur_theme_data.get_state_data_name(State::NormalUncheckedRTL), "CheckButton", "toggle-switch-outline");

		cur_theme_data.set_data_name("text_check_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverCheckedLTR), "CheckButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedCheckedLTR), "CheckButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusCheckedLTR), "CheckButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckButton", button_font_focus_color_role);

		Ref<ColorRole> check_button_mono_color_role = make_color_role(ColorRoleEnum::PRIMARY);
		Ref<ColorRole> check_button_base_color_role = make_color_role(ColorRoleEnum::SECONDARY);
		Ref<ColorRole> check_button_accent_color_role = make_color_role(ColorRoleEnum::PRIMARY);

		Ref<ColorRole> check_button_font_color_role = check_button_mono_color_role->duplicate();
		check_button_font_color_role->set_lerp_color_role(check_button_base_color_role);
		check_button_font_color_role->set_lerp_weight(0.25);
		Ref<ColorRole> check_button_font_hover_color_role = check_button_mono_color_role->duplicate();
		check_button_font_hover_color_role->set_lerp_color_role(check_button_base_color_role);
		check_button_font_hover_color_role->set_lerp_weight(0.125);
		Ref<ColorRole> check_button_font_focus_color_role = check_button_mono_color_role->duplicate();
		check_button_font_focus_color_role->set_lerp_color_role(check_button_base_color_role);
		check_button_font_focus_color_role->set_lerp_weight(0.125);
		Ref<ColorRole> check_button_font_disabled_color_role = check_button_mono_color_role->duplicate();
		check_button_font_disabled_color_role->set_color_scale(check_button_font_disabled_color_role->get_color_scale() * Color(1, 1, 1, 0.5));
		Ref<ColorRole> check_button_font_pressed_color_role = check_button_accent_color_role;

		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckButton", check_button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverUncheckedLTR), "CheckButton", check_button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedUncheckedLTR), "CheckButton", check_button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusUncheckedLTR), "CheckButton", check_button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckButton", check_button_font_disabled_color_role);

		cur_theme_data.set_data_name("text_check_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "CheckButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CheckButton", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("text_check_icon_bg_1");
		theme->set_str(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", "toggle-switch-off");

		cur_theme_data.set_data_name("text_check_icon_bg_2");
		theme->set_str(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CheckButton", "toggle-switch");

		cur_theme_data.set_data_name("text_check_icon_bg_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckButton", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverCheckedLTR), "CheckButton", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedCheckedLTR), "CheckButton", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusCheckedLTR), "CheckButton", button_font_disabled_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckButton", button_font_focus_color_role);

		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckButton", check_button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverUncheckedLTR), "CheckButton", check_button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedUncheckedLTR), "CheckButton", check_button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::FocusUncheckedLTR), "CheckButton", check_button_font_focus_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckButton", check_button_font_disabled_color_role);

		cur_theme_data.set_data_name("text_check_icon_bg_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "CheckButton", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "CheckButton", Color(1, 1, 1, 0.38));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "CheckButton", Color(1, 1, 1, 0.38));
	}

	// Popup
	// fill_default_theme_popup_panel(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "PopupPanel", default_color_scheme);

		const Ref<StyleBoxFlat> popup_style_normal = make_color_role_flat_stylebox(style_popup_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
		theme->set_stylebox("panel", "PopupPanel", popup_style_normal);

		theme->set_type_variation("TooltipPanel", "PopupPanel");
		const Ref<StyleBoxFlat> tooltip_default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::TERTIARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2 * default_margin, 0.5 * default_margin, 2 * default_margin, 0.5 * default_margin);

		theme->set_stylebox("panel", "TooltipPanel", tooltip_default_stylebox_normal);
	}

	// Button variations
	// fill_default_theme_button_variations(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_type_variation("FlatButton", "Button");
		const Ref<StyleBoxEmpty> flat_button_style_normal = make_empty_stylebox();

		for (int i = 0; i < 4; i++) {
			flat_button_style_normal->set_content_margin((Side)i, button_style_normal->get_margin((Side)i) + button_style_normal->get_border_width((Side)i));
		}

		const Ref<StyleBoxFlat> flat_button_style_pressed = button_style_pressed->duplicate();

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FlatButton", flat_button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "FlatButton", flat_button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "FlatButton", flat_button_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "FlatButton", flat_button_style_normal);

		theme->set_type_variation("FlatMenuButton", "MenuButton");
		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FlatMenuButton", flat_button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "FlatMenuButton", flat_button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "FlatMenuButton", flat_button_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "FlatMenuButton", flat_button_style_normal);
	}

	// Sliders
	// fill_default_theme_slider(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		const Ref<StyleBoxFlat> style_slider = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);

		const Ref<StyleBoxFlat> style_slider_grabber = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
		
		const Ref<StyleBoxFlat> style_slider_grabber_highlight = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY, Color(1, 1, 1, 0.6)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
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
	}


	// SpinBox
	{
		theme->set_icon("updown", "SpinBox", icons["updown"]);
	}
	
	// ProgressBar
	// fill_default_theme_progress_bar(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "ProgressBar", default_color_scheme);
		const Ref<StyleBoxFlat> background_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2, 2, 2, 2, 6);
		const Ref<StyleBoxFlat> fill_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 2, 2, 2, 2, 6);

		theme->set_stylebox("background", "ProgressBar", background_stylebox_normal);
		theme->set_stylebox("fill", "ProgressBar", fill_stylebox_normal);

		theme->set_font("font", "ProgressBar", Ref<Font>());
		theme->set_font_size("font_size", "ProgressBar", -1);

		theme->set_constant("outline_size", "ProgressBar", 0);

		theme->set_color_role("font_color_role", "ProgressBar", make_color_role(ColorRoleEnum::INVERSE_PRIMARY));
		theme->set_color("font_color", "ProgressBar", Color(1, 1, 1, 1));

		theme->set_color_role("font_outline_color_role", "ProgressBar", make_color_role(ColorRoleEnum::OUTLINE));
		theme->set_color("font_outline_color", "ProgressBar", Color(1, 1, 1, 1));
	}

	// TabBar
	// fill_default_theme_tab_bar(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	Ref<StyleBoxFlat> style_tab_selected = make_color_role_flat_stylebox(style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme,10, 4, 10, 4, 0);
	style_tab_selected->set_border_width(SIDE_TOP, Math::round(2 * scale));
	style_tab_selected->set_border_color_role(style_focus_color_role);
	Ref<StyleBoxFlat> style_tab_unselected = make_color_role_flat_stylebox(style_pressed_color_role,StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 10, 4, 10, 4, 0);
	// Add some spacing between unselected tabs to make them easier to distinguish from each other.
	style_tab_unselected->set_border_width(SIDE_LEFT, Math::round(scale));
	style_tab_unselected->set_border_width(SIDE_RIGHT, Math::round(scale));
	style_tab_unselected->set_border_color_role(style_popup_border_color_role);
	Ref<StyleBoxFlat> style_tab_disabled = style_tab_unselected->duplicate();
	style_tab_disabled->set_bg_color_role(style_disabled_color_role);
	Ref<StyleBoxFlat> style_tab_hovered = style_tab_unselected->duplicate();
	Ref<ColorRole> style_tab_hovered_color_role = style_pressed_color_role->duplicate();
	style_tab_hovered_color_role->set_darkened_amount(0.25);
	style_tab_hovered->set_bg_color_role(style_tab_hovered_color_role);
	{
		theme->set_color_scheme("default_color_scheme", "TabBar", default_color_scheme);

		cur_theme_data.set_data_name("tab");

		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "TabBar", style_tab_selected);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TabBar", style_tab_selected);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "TabBar", style_tab_unselected);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TabBar", style_tab_hovered);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TabBar", button_style_focus);

		theme->set_stylebox("button_pressed", "TabBar", button_style_pressed);
		theme->set_stylebox("button_highlight", "TabBar", button_style_normal);
		
		theme->set_icon("increment", "TabBar", icons["scroll_button_right"]);
		theme->set_icon("increment_highlight", "TabBar", icons["scroll_button_right_hl"]);
		theme->set_icon("decrement", "TabBar", icons["scroll_button_left"]);
		theme->set_icon("decrement_highlight", "TabBar", icons["scroll_button_left_hl"]);
		theme->set_icon("drop_mark", "TabBar", icons["tabs_drop_mark"]);
		theme->set_icon("close", "TabBar", icons["close"]);

		theme->set_font("font", "TabBar", Ref<Font>());
		theme->set_font_size("font_size", "TabBar", -1);
		theme->set_constant("h_separation", "TabBar", Math::round(4 * scale));
		theme->set_constant("icon_max_width", "TabBar", 0);
		theme->set_constant("outline_size", "TabBar", 0);

		theme->set_color_role("drop_mark_color_role", "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
		theme->set_color("drop_mark_color", "TabBar", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TabBar", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TabBar", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "TabBar", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TabBar", button_font_disabled_color_role);

		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "TabBar", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TabBar", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "TabBar", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TabBar", Color(1, 1, 1, 1));

		theme->set_color_role("font_outline_color_role", "TabBar", outline_color_role);
		theme->set_color("font_outline_color", "TabBar", Color(1, 1, 1, 1));
	}

	// TabContainer
	// fill_default_theme_tab_container(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "TabContainer", default_color_scheme);

		const Ref<StyleBoxFlat> default_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0);
		theme->set_stylebox("panel", "TabContainer", default_stylebox_normal);
		theme->set_stylebox("tabbar_background", "TabContainer", make_empty_stylebox(0, 0, 0, 0));

		theme->set_constant("side_margin", "TabContainer", Math::round(8 * scale));

		theme->set_icon("menu", "TabContainer", icons["tabs_menu"]);
		theme->set_icon("menu_highlight", "TabContainer", icons["tabs_menu_hl"]);

		theme->set_constant("icon_separation", "TabContainer", Math::round(4 * scale));
		theme->set_constant("icon_max_width", "TabContainer", 0);
		theme->set_constant("outline_size", "TabContainer", 0);

		cur_theme_data.set_data_name("tab");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "TabContainer", style_tab_selected);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TabContainer", style_tab_selected);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "TabContainer", style_tab_unselected);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TabContainer", style_tab_hovered);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TabContainer", button_style_focus);

		theme->set_icon("increment", "TabContainer", icons["scroll_button_right"]);
		theme->set_icon("increment_highlight", "TabContainer", icons["scroll_button_right_hl"]);
		theme->set_icon("decrement", "TabContainer", icons["scroll_button_left"]);
		theme->set_icon("decrement_highlight", "TabContainer", icons["scroll_button_left_hl"]);
		theme->set_icon("drop_mark", "TabContainer", icons["tabs_drop_mark"]);

		theme->set_color_role("drop_mark_color_role", "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY));
		theme->set_color("drop_mark_color", "TabContainer", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TabContainer", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TabContainer", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "TabContainer", button_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TabContainer", button_font_disabled_color_role);

		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "TabContainer", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "TabContainer", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "TabContainer", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "TabContainer", Color(1, 1, 1, 1));

		theme->set_color_role("font_outline_color_role", "TabContainer", outline_color_role);
		theme->set_color("font_outline_color", "TabContainer", Color(1, 1, 1, 1));

		theme->set_font("font", "TabContainer", Ref<Font>());
		theme->set_font_size("font_size", "TabContainer", -1);
	}

	// Containers
	// fill_default_theme_containers(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "Container", default_color_scheme);
		const Ref<StyleBoxFlat> container_style_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0);

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

		theme->set_stylebox("panel", "PanelContainer", container_style_normal);
	}

	// ItemList
	// fill_default_theme_item_list(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "ItemList", default_color_scheme);
		theme->set_constant("h_separation", "ItemList", Math::round(4 * scale));
		theme->set_constant("v_separation", "ItemList", Math::round(2 * scale));

		cur_theme_data.set_data_name("default_panel");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ItemList", button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ItemList", button_style_focus);

		theme->set_font("font", "ItemList", Ref<Font>());
		theme->set_font_size("font_size", "ItemList", -1);

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ItemList", button_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ItemList", button_font_hover_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "ItemList", button_font_pressed_color_role);

		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ItemList", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ItemList", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "ItemList", Color(1, 1, 1, 1));

		theme->set_constant("outline_size", "ItemList", 0);

		theme->set_color_role("font_outline_color_role", "ItemList", outline_color_role);
		theme->set_color("font_outline_color", "ItemList", Color(1, 1, 1, 1));

		theme->set_constant("icon_margin", "ItemList", Math::round(4 * scale));
		theme->set_constant("line_separation", "ItemList", Math::round(2 * scale));

		const Ref<StyleBoxFlat> itemList_style_hover = make_color_role_flat_stylebox(button_style_hover_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);

		cur_theme_data.set_data_name("item_style");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "ItemList", itemList_style_hover);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ItemList", make_color_role_flat_stylebox(style_selected_color_role));
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusCheckedLTR), "ItemList", make_color_role_flat_stylebox(style_selected_color_role));

		cur_theme_data.set_data_name("cursor");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "ItemList", button_style_focus);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "ItemList", button_style_focus);

		theme->set_color_role("guide_color_role", "ItemList", make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.16)));
		theme->set_color("guide_color", "ItemList", Color(0.7, 0.7, 0.7, 0.25));
	}


	// Dialogs
	// fill_default_theme_dialogs(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		// AcceptDialog is currently the base dialog, so this defines styles for all extending nodes.
		theme->set_color_scheme("default_color_scheme", "AcceptDialog", default_color_scheme);
		const Ref<StyleBoxFlat> dialog_style_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, Math::round(8 * scale), Math::round(8 * scale), Math::round(8 * scale), Math::round(8 * scale));
		theme->set_stylebox("panel", "AcceptDialog", dialog_style_normal);
		theme->set_constant("buttons_separation", "AcceptDialog", Math::round(10 * scale));

		// File Dialog
		theme->set_icon("parent_folder", "FileDialog", icons["folder_up"]);
		theme->set_icon("back_folder", "FileDialog", icons["arrow_left"]);
		theme->set_icon("forward_folder", "FileDialog", icons["arrow_right"]);
		theme->set_icon("reload", "FileDialog", icons["reload"]);
		theme->set_icon("toggle_hidden", "FileDialog", icons["visibility_visible"]);
		theme->set_icon("folder", "FileDialog", icons["folder"]);
		theme->set_icon("file", "FileDialog", icons["file"]);


		theme->set_color("folder_icon_color", "FileDialog", Color(1, 1, 1, 1));
		theme->set_color_role("folder_icon_color_role", "FileDialog", make_color_role(ColorRoleEnum::STATIC_COLOR));

		cur_theme_data.set_data_name("file_icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FileDialog", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "FileDialog", Color(1, 1, 1, 0.25));

		cur_theme_data.set_data_name("file_icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FileDialog", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "FileDialog", make_color_role(ColorRoleEnum::STATIC_COLOR));


		cur_theme_data.set_data_name("icon_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FileDialog", Color(1, 1, 1, 1));
		cur_theme_data.set_data_name("icon_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "FileDialog", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	}

	// Tree
	// fill_default_theme_tree(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "Tree", default_color_scheme);
		cur_theme_data.set_data_name("default_stylebox");

		const Ref<StyleBoxFlat> tree_style_normal = make_color_role_flat_stylebox(style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 5);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", tree_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Tree", style_focus);

		theme->set_font("font", "Tree", Ref<Font>());
		theme->set_font_size("font_size", "Tree", -1);
		theme->set_font("title_button_font", "Tree", Ref<Font>());
		theme->set_font_size("title_button_font_size", "Tree", -1);

		cur_theme_data.set_data_name("selected");
		const Ref<StyleBoxFlat> selected_style_normal = make_color_role_flat_stylebox(style_selected_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", selected_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Tree", selected_style_normal);

		cur_theme_data.set_data_name("cursor");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", style_focus);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "Tree", style_focus);


		theme->set_stylebox("button_pressed", "Tree", button_style_pressed);

		cur_theme_data.set_data_name("title_button");
		const Ref<StyleBoxFlat> title_button_style_normal = make_color_role_flat_stylebox(button_style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);

		const Ref<StyleBoxFlat> title_button_style_pressed = 
		make_color_role_flat_stylebox(button_style_pressed_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);
		const Ref<StyleBoxFlat> title_button_style_hover = make_color_role_flat_stylebox(button_style_hover_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);

		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", title_button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Tree", title_button_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Tree", title_button_style_hover);

		cur_theme_data.set_data_name("custom_button");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", button_style_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "Tree", button_style_pressed);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "Tree", button_style_hover);

		theme->set_color_role("title_button_color_role", "Tree", control_font_color_role);
		theme->set_color("title_button_color", "Tree", Color(1, 1, 1, 1));

		cur_theme_data.set_data_name("checked");
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "Tree", icons["checked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "Tree", icons["unchecked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledCheckedLTR), "Tree", icons["checked"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledUncheckedLTR), "Tree", icons["unchecked"]);

		cur_theme_data.set_data_name("indeterminate");
		theme->set_icon(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", icons["indeterminate"]);
		theme->set_icon(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Tree", icons["indeterminate"]);

		theme->set_icon("arrow", "Tree", icons["arrow_down"]);
		theme->set_icon("arrow_collapsed", "Tree", icons["arrow_right"]);
		theme->set_icon("arrow_collapsed_mirrored", "Tree", icons["arrow_left"]);
		theme->set_icon("select_arrow", "Tree", icons["option_button_arrow"]);
		theme->set_icon("updown", "Tree", icons["updown"]);

		cur_theme_data.set_data_name("font_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", control_font_low_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "Tree", control_font_pressed_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "Tree", control_font_low_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Tree", control_font_disabled_color_role);

		cur_theme_data.set_data_name("font_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "Tree", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "Tree", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "Tree", Color(1, 1, 1, 1));
		theme->set_color(cur_theme_data.get_state_data_name(State::DisabledNoneLTR), "Tree", Color(1, 1, 1, 1));

		Ref<ColorRole> guide_color_role = make_color_role(ColorRoleEnum::PRIMARY, Color(1, 1, 1, 0.25));
		theme->set_color_role("guide_color_role", "Tree", guide_color_role);
		theme->set_color("guide_color", "Tree", Color(0.7, 0.7, 0.7, 0.25));

		theme->set_color_role("drop_position_color_role", "Tree", mono_color_role);
		theme->set_color("drop_position_color", "Tree", Color(1, 1, 1));

		Ref<ColorRole> relationship_line_color_role = make_color_role(ColorRoleEnum::SECONDARY, Color(1, 1, 1, 0.25));
		theme->set_color_role("relationship_line_color_role", "Tree", relationship_line_color_role);
		theme->set_color("relationship_line_color", "Tree", Color(0.27, 0.27, 0.27));

		theme->set_color_role("parent_hl_line_color_role", "Tree", relationship_line_color_role);
		theme->set_color("parent_hl_line_color", "Tree", Color(0.27, 0.27, 0.27));

		theme->set_color_role("children_hl_line_color_role", "Tree", relationship_line_color_role);
		theme->set_color("children_hl_line_color", "Tree", Color(0.27, 0.27, 0.27));

		theme->set_color_role("custom_button_font_highlight_role", "Tree", relationship_line_color_role);
		theme->set_color("custom_button_font_highlight", "Tree", Color(0.27, 0.27, 0.27));

		theme->set_color_role("font_outline_color_role", "Tree", outline_color_role);
		theme->set_color("font_outline_color", "Tree", Color(1, 1, 1));

		theme->set_constant("base_scale", "Tree", 1.0);
		theme->set_constant("outline_size", "Tree", 0);

		theme->set_constant("h_separation", "Tree", Math::round(4 * scale));
		theme->set_constant("v_separation", "Tree", Math::round(4 * scale));

		theme->set_constant("inner_item_margin_bottom", "Tree", 0);
		theme->set_constant("inner_item_margin_left", "Tree", 0);
		theme->set_constant("inner_item_margin_right", "Tree", 0);
		theme->set_constant("inner_item_margin_top", "Tree", 0);

		theme->set_constant("item_margin", "Tree", Math::round(16 * scale));
		theme->set_constant("button_margin", "Tree", Math::round(4 * scale));
		theme->set_constant("icon_max_width", "Tree", 0);

		theme->set_constant("draw_relationship_lines", "Tree", 0);
		theme->set_constant("relationship_line_width", "Tree", 1);
		theme->set_constant("parent_hl_line_width", "Tree", 1);
		theme->set_constant("children_hl_line_width", "Tree", 1);
		theme->set_constant("parent_hl_line_margin", "Tree", 0);
		theme->set_constant("draw_guides", "Tree", 1);

		theme->set_constant("scroll_border", "Tree", Math::round(4 * scale));
		theme->set_constant("scroll_speed", "Tree", 12);

		theme->set_constant("scrollbar_margin_left", "Tree", -1);
		theme->set_constant("scrollbar_margin_top", "Tree", -1);
		theme->set_constant("scrollbar_margin_right", "Tree", -1);
		theme->set_constant("scrollbar_margin_bottom", "Tree", -1);
		theme->set_constant("scrollbar_h_separation", "Tree", Math::round(4 * scale));
		theme->set_constant("scrollbar_v_separation", "Tree", Math::round(4 * scale));
	}

	// GraphEdit
	// fill_default_theme_graph_edit(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		ThemeIntData cur_theme_data;
		// GraphNode
		theme->set_color_scheme("default_color_scheme", "GraphNode", default_color_scheme);
		const Ref<StyleBoxFlat> graphnode_normal = make_color_role_flat_stylebox(style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 18, 12, 18, 12);
		Ref<ColorRole> graphnode_normal_border_color_role = make_color_role(ColorRoleEnum::SECONDARY, Color(1, 1, 1, 0.6));	
		graphnode_normal->set_border_color(Color(0.325, 0.325, 0.325, 0.6));
		graphnode_normal->set_border_color_role(graphnode_normal_border_color_role);

		Ref<StyleBoxFlat> graphnode_selected = graphnode_normal->duplicate();
		Ref<ColorRole> graphnode_selected_border_color_role = make_color_role(ColorRoleEnum::SECONDARY_CONTAINER, Color(1, 1, 1, 0.6));	
		graphnode_selected->set_border_color(Color(0.625, 0.625, 0.625, 0.6));
		graphnode_selected->set_border_color_role(graphnode_selected_border_color_role);

		Ref<ColorRole> graphn_sb_titlebar_color_role = style_normal_color_role->duplicate();
		graphn_sb_titlebar_color_role->set_lightened_amount(0.3);

		Ref<StyleBoxFlat> graphn_sb_titlebar = make_color_role_flat_stylebox(graphn_sb_titlebar_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 4);

		Ref<StyleBoxFlat> graphn_sb_titlebar_selected = graphnode_normal->duplicate();
		graphn_sb_titlebar_selected->set_bg_color(Color(1.0, 0.625, 0.625, 0.6));
		graphn_sb_titlebar_selected->set_bg_color_role(graphnode_selected_border_color_role);

		Ref<StyleBoxEmpty> graphnode_slot = make_empty_stylebox(0, 0, 0, 0);

		cur_theme_data.set_data_name("panel");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "GraphNode", graphnode_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "GraphNode", graphnode_normal);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "GraphNode", graphnode_selected);

		cur_theme_data.set_data_name("titlebar");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "GraphNode", graphn_sb_titlebar);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalUncheckedLTR), "GraphNode", graphn_sb_titlebar);
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalCheckedLTR), "GraphNode", graphn_sb_titlebar_selected);

		theme->set_stylebox("slot", "GraphNode", graphnode_slot);

		theme->set_icon("port", "GraphNode", icons["graph_port"]);
		theme->set_icon("resizer", "GraphNode", icons["resizer_se"]);

		theme->set_color_role("resizer_color_role", "GraphNode", make_color_role(ColorRoleEnum::SCRIM, Color(1,1,1,0.85)));
		theme->set_color("resizer_color", "GraphNode", Color(1, 1, 1, 0.85));

		theme->set_constant("separation", "GraphNode", Math::round(2 * scale));
		theme->set_constant("port_h_offset", "GraphNode", 0);

		// GraphNodes's title Label.

		theme->set_type_variation("GraphNodeTitleLabel", "Label");
		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "GraphNodeTitleLabel", make_empty_stylebox(0, 0, 0, 0));

		theme->set_font("font", "GraphNodeTitleLabel", Ref<Font>());
		theme->set_font_size("font_size", "GraphNodeTitleLabel", -1);

		theme->set_color_role("font_color_role", "GraphNodeTitleLabel", control_font_color_role);
		theme->set_color("font_color", "GraphNodeTitleLabel", Color(1, 1, 1, 1));

		theme->set_color_role("font_shadow_color_role", "GraphNodeTitleLabel", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("font_shadow_color", "GraphNodeTitleLabel", Color(0, 0, 0, 0));

		theme->set_color_role("font_outline_color_role", "GraphNodeTitleLabel", outline_color_role);
		theme->set_color("font_outline_color", "GraphNodeTitleLabel", Color(0, 0, 0));

		theme->set_constant("shadow_offset_x", "GraphNodeTitleLabel", Math::round(1 * scale));
		theme->set_constant("shadow_offset_y", "GraphNodeTitleLabel", Math::round(1 * scale));
		theme->set_constant("outline_size", "GraphNodeTitleLabel", 0);
		theme->set_constant("shadow_outline_size", "GraphNodeTitleLabel", Math::round(1 * scale));
		theme->set_constant("line_spacing", "GraphNodeTitleLabel", Math::round(3 * scale));

		theme->set_color_scheme("default_color_scheme", "GraphEditMinimap", default_color_scheme);
		const Ref<StyleBoxFlat> panel_stylebox_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER,Color(1,1,1,0.24)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0);

		Ref<StyleBoxFlat> style_minimap_camera = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER,Color(1,1,1,0.2)), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0, 0);
		style_minimap_camera->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE, Color(1,1,1,0.45)));
		style_minimap_camera->set_border_width_all(1);

		Ref<StyleBoxFlat> style_node = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0, 2);

		theme->set_stylebox("panel", "GraphEditMinimap", panel_stylebox_normal);
		theme->set_stylebox("camera", "GraphEditMinimap", style_minimap_camera);
		theme->set_stylebox("node", "GraphEditMinimap", style_node);

		theme->set_icon("resizer", "GraphEditMinimap", icons["resizer_nw"]);

		theme->set_color_role("resizer_color_role", "GraphEditMinimap", make_color_role(ColorRoleEnum::ON_PRIMARY));
		theme->set_color("resizer_color", "GraphEditMinimap", Color(1, 1, 1, 0.85));

		theme->set_color_scheme("default_color_scheme", "GraphEdit", default_color_scheme);

		theme->set_icon("zoom_out", "GraphEdit", icons["zoom_less"]);
		theme->set_icon("zoom_in", "GraphEdit", icons["zoom_more"]);
		theme->set_icon("zoom_reset", "GraphEdit", icons["zoom_reset"]);
		theme->set_icon("grid_toggle", "GraphEdit", icons["grid_toggle"]);
		theme->set_icon("minimap_toggle", "GraphEdit", icons["grid_minimap"]);
		theme->set_icon("snapping_toggle", "GraphEdit", icons["grid_snap"]);
		theme->set_icon("layout", "GraphEdit", icons["grid_layout"]);

		const Ref<StyleBoxFlat> graph_edit_panel_stylebox_normal = make_color_role_flat_stylebox(style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 4, 4, 5);

		const Ref<StyleBoxFlat> graph_toolbar_style = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 4, 2, 4, 2);

		theme->set_stylebox("panel", "GraphEdit", graph_edit_panel_stylebox_normal);
		theme->set_stylebox("menu_panel", "GraphEdit", graph_toolbar_style);

		theme->set_color_role("grid_minor_role", "GraphEdit", make_color_role(ColorRoleEnum::SCRIM, Color(1, 1, 1, 0.05), true));
		theme->set_color("grid_minor", "GraphEdit", Color(1, 1, 1, 0.05));

		theme->set_color_role("grid_major_role", "GraphEdit", make_color_role(ColorRoleEnum::SCRIM, Color(1, 1, 1, 0.2), true));
		theme->set_color("grid_major", "GraphEdit", Color(1, 1, 1, 0.2));

		theme->set_color_role("selection_fill_role", "GraphEdit", make_color_role(ColorRoleEnum::SCRIM, Color(1, 1, 1, 0.3), true));
		theme->set_color("selection_fill", "GraphEdit", Color(1, 1, 1, 0.3));

		theme->set_color_role("selection_stroke_role", "GraphEdit", make_color_role(ColorRoleEnum::SCRIM, Color(1, 1, 1, 0.8), true));
		theme->set_color("selection_stroke", "GraphEdit", Color(1, 1, 1, 0.8));

		theme->set_color_role("activity_role", "GraphEdit", make_color_role(ColorRoleEnum::SCRIM, Color(1, 1, 1, 1), true));
		theme->set_color("activity", "GraphEdit", Color(1, 1, 1));

		// Visual Node Ports

		theme->set_constant("port_hotzone_inner_extent", "GraphEdit", 22 * scale);
		theme->set_constant("port_hotzone_outer_extent", "GraphEdit", 26 * scale);
	}

	// LineEdit
	// fill_default_theme_line_edit(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	
	const Ref<StyleBoxFlat> style_line_edit_normal = make_color_role_flat_stylebox(style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme);
	style_line_edit_normal->set_border_width(SIDE_BOTTOM, 2);
	style_line_edit_normal->set_border_color_role(style_pressed_color_role);

	Ref<StyleBoxFlat> read_only_stylebox = make_color_role_flat_stylebox(style_disabled_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	read_only_stylebox->set_border_width(SIDE_BOTTOM, 2);
	Ref<ColorRole> read_only_stylebox_style_pressed_color_role = style_pressed_color_role->duplicate();
	read_only_stylebox_style_pressed_color_role->set_color_scale(read_only_stylebox_style_pressed_color_role->get_color_scale() * Color(1, 1, 1, 0.5));
	read_only_stylebox->set_border_color_role(read_only_stylebox_style_pressed_color_role);
	
	{
		theme->set_color_scheme("default_color_scheme", "LineEdit", default_color_scheme);


		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LineEdit", style_line_edit_normal);

		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "LineEdit", style_focus);

		theme->set_stylebox("read_only", "LineEdit", read_only_stylebox);

		theme->set_font("font", "LineEdit", Ref<Font>());
		theme->set_font_size("font_size", "LineEdit", -1);

		theme->set_color_role("font_color_role", "LineEdit", control_font_color_role);
		theme->set_color("font_color", "LineEdit", control_font_color);

		theme->set_color_role("font_selected_color_role", "LineEdit", control_font_pressed_color_role);
		theme->set_color("font_selected_color", "LineEdit", control_font_pressed_color);

		theme->set_color_role("font_uneditable_color_role", "LineEdit", control_font_disabled_color_role);
		theme->set_color("font_uneditable_color", "LineEdit", control_font_disabled_color);

		theme->set_color_role("font_placeholder_color_role", "LineEdit", control_font_placeholder_color_role);
		theme->set_color("font_placeholder_color", "LineEdit", control_font_placeholder_color);

		theme->set_color_role("font_outline_color_role", "LineEdit", make_color_role(ColorRoleEnum::SCRIM));
		theme->set_color("font_outline_color", "LineEdit", Color(0, 0, 0));

		theme->set_color_role("caret_color_role", "LineEdit", control_font_hover_color_role);
		theme->set_color("caret_color", "LineEdit", control_font_hover_color);

		theme->set_color_role("selection_color_role", "LineEdit", control_selection_color_role);
		theme->set_color("selection_color", "LineEdit", control_selection_color);

		cur_theme_data.set_data_name("clear_button_color_role");
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LineEdit", control_font_color_role);
		theme->set_color_role(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "LineEdit", control_font_pressed_color_role);

		cur_theme_data.set_data_name("clear_button_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "LineEdit", control_font_color);
		theme->set_color(cur_theme_data.get_state_data_name(State::PressedNoneLTR), "LineEdit", control_font_pressed_color);

		theme->set_constant("minimum_character_width", "LineEdit", 4);
		theme->set_constant("outline_size", "LineEdit", 0);

		theme->set_constant("caret_width", "LineEdit", 1);

		theme->set_icon("clear", "LineEdit", icons["line_edit_clear"]);

		theme->set_constant("base_scale", "LineEdit", 1.0);
	}

	// TextEdit
	fill_default_theme_text_edit(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{

		theme->set_color_scheme("default_color_scheme", "TextEdit", default_color_scheme);


		theme->set_constant("base_scale", "TextEdit", 1);

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "TextEdit", style_line_edit_normal);

		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "TextEdit", style_focus);

		theme->set_stylebox("read_only", "TextEdit", read_only_stylebox);

		theme->set_icon("tab", "TextEdit", icons["text_edit_tab"]);
		theme->set_icon("space", "TextEdit", icons["text_edit_space"]);

		theme->set_font("font", "TextEdit", Ref<Font>());
		theme->set_font_size("font_size", "TextEdit", -1);

		theme->set_color_role("background_color_role", "TextEdit", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("background_color", "TextEdit", Color(0, 0, 0, 0));

		theme->set_color_role("font_color_role", "TextEdit", control_font_color_role);
		theme->set_color("font_color", "TextEdit", control_font_color);
		
		theme->set_color_role("font_selected_color_role", "TextEdit", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("font_selected_color", "TextEdit", Color(0, 0, 0, 0));


		theme->set_color_role("font_readonly_color_role", "TextEdit", control_font_disabled_color_role);
		theme->set_color("font_readonly_color", "TextEdit", control_font_disabled_color);

		theme->set_color_role("font_placeholder_color_role", "TextEdit", control_font_placeholder_color_role);
		theme->set_color("font_placeholder_color", "TextEdit", control_font_placeholder_color);

		theme->set_color_role("font_outline_color_role", "TextEdit", make_color_role(ColorRoleEnum::SCRIM));
		theme->set_color("font_outline_color", "TextEdit",  Color(0, 0, 0));

		theme->set_color_role("selection_color_role", "TextEdit", control_selection_color_role);
		theme->set_color("selection_color", "TextEdit", control_selection_color);

		theme->set_color_role("current_line_color_role", "TextEdit", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color("current_line_color", "TextEdit", Color(0.25, 0.25, 0.26, 0.8));

		theme->set_color_role("caret_color_role", "TextEdit", control_font_color_role);
		theme->set_color("caret_color", "TextEdit", control_font_color);

		theme->set_color_role("caret_background_color_role", "TextEdit", make_color_role(ColorRoleEnum::SCRIM));
		theme->set_color("caret_background_color", "TextEdit", Color(0, 0, 0));

		theme->set_color_role("word_highlighted_color_role", "TextEdit", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color("word_highlighted_color", "TextEdit", Color(0.5, 0.5, 0.5, 0.25));

		theme->set_color_role("search_result_color_role", "TextEdit", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color("search_result_color", "TextEdit", Color(0.3, 0.3, 0.3));


		theme->set_color_role("search_result_border_color_role", "TextEdit", make_color_role(ColorRoleEnum::STATIC_COLOR));
		theme->set_color("search_result_border_color", "TextEdit", Color(0.3, 0.3, 0.3, 0.4));

		theme->set_constant("line_spacing", "TextEdit", Math::round(4 * scale));

		theme->set_constant("outline_size", "TextEdit", 0);

		theme->set_constant("caret_width", "TextEdit", 1);
	}


	// CodeEdit
	// fill_default_theme_code_edit(theme, default_font, bold_font, bold_italics_font, italics_font, default_icon, default_icon_font, default_style, p_scale, default_color_scheme, icons);
	{
		theme->set_color_scheme("default_color_scheme", "CodeEdit", default_color_scheme);

		cur_theme_data.set_data_name("default_stylebox");
		theme->set_stylebox(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CodeEdit", style_line_edit_normal);

		theme->set_stylebox(cur_theme_data.get_state_data_name(State::FocusNoneLTR), "CodeEdit", style_focus);

		theme->set_stylebox("read_only", "CodeEdit", read_only_stylebox);

		theme->set_stylebox("completion", "CodeEdit", make_color_role_flat_stylebox(style_normal_color_role, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_color_scheme, 0, 0, 0, 0));


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

		Ref<ColorRole> static_color_role = make_color_role(ColorRoleEnum::STATIC_COLOR);

		theme->set_color_role("background_color_role", "CodeEdit", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("background_color", "CodeEdit", Color(0, 0, 0, 0));

		theme->set_color_role("completion_background_color_role", "CodeEdit", static_color_role);
		theme->set_color("completion_background_color", "CodeEdit", Color(0.17, 0.16, 0.2));

		theme->set_color_role("completion_selected_color_role", "CodeEdit", static_color_role);
		theme->set_color("completion_selected_color", "CodeEdit", Color(0.26, 0.26, 0.27));

		theme->set_color_role("completion_existing_color_role", "CodeEdit", static_color_role);
		theme->set_color("completion_existing_color", "CodeEdit", Color(0.87, 0.87, 0.87, 0.13));

		cur_theme_data.set_data_name("completion_scroll_color_role");
		Ref<ColorRole> completion_selected_color_role = control_font_pressed_color_role->duplicate();
		completion_selected_color_role->set_color_scale(completion_selected_color_role->get_color_scale()*Color(1, 1, 1, 0.29));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CodeEdit", completion_selected_color_role);

		Ref<ColorRole> completion_selected_hover_color_role = control_font_pressed_color_role->duplicate();
		completion_selected_hover_color_role->set_color_scale(completion_selected_hover_color_role->get_color_scale()*Color(1, 1, 1, 0.4));
		theme->set_color_role(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CodeEdit", completion_selected_hover_color_role);


		cur_theme_data.set_data_name("completion_scroll_color");
		theme->set_color(cur_theme_data.get_state_data_name(State::NormalNoneLTR), "CodeEdit", control_font_pressed_color * Color(1, 1, 1, 0.29));
		theme->set_color(cur_theme_data.get_state_data_name(State::HoverNoneLTR), "CodeEdit", control_font_pressed_color * Color(1, 1, 1, 0.4));

		theme->set_color_role("font_color_role", "CodeEdit", control_font_color_role);
		theme->set_color("font_color", "CodeEdit", control_font_color);

		theme->set_color_role("font_selected_color_role", "CodeEdit", make_color_role(ColorRoleEnum::STATIC_TRANSPARENT));
		theme->set_color("font_selected_color", "CodeEdit", Color(0, 0, 0, 0));

		Ref<ColorRole> font_readonly_color_role = control_font_color_role->duplicate();
		control_font_color_role->set_color_scale(Color(control_font_color_role->get_color_scale(),0.5));
		theme->set_color_role("font_readonly_color_role", "CodeEdit", control_font_color_role);
		theme->set_color("font_readonly_color", "CodeEdit", Color(control_font_color.r, control_font_color.g, control_font_color.b, 0.5f));

		theme->set_color_role("font_placeholder_color_role", "CodeEdit", control_font_placeholder_color_role);
		theme->set_color("font_placeholder_color", "CodeEdit", control_font_placeholder_color);

		theme->set_color_role("font_outline_color_role", "CodeEdit", make_color_role(ColorRoleEnum::SCRIM));
		theme->set_color("font_outline_color", "CodeEdit", Color(0, 0, 0));

		theme->set_color_role("selection_color_role", "CodeEdit", control_selection_color_role);
		theme->set_color("selection_color", "CodeEdit", control_selection_color);

		theme->set_color_role("bookmark_color_role", "CodeEdit", static_color_role);
		theme->set_color("bookmark_color", "CodeEdit", Color(0.5, 0.64, 1, 0.8));

		theme->set_color_role("breakpoint_color_role", "CodeEdit", static_color_role);
		theme->set_color("breakpoint_color", "CodeEdit", Color(0.9, 0.29, 0.3));

		theme->set_color_role("executing_line_color_role", "CodeEdit", static_color_role);
		theme->set_color("executing_line_color", "CodeEdit", Color(0.98, 0.89, 0.27));

		theme->set_color_role("current_line_color_role", "CodeEdit", static_color_role);
		theme->set_color("current_line_color", "CodeEdit", Color(0.25, 0.25, 0.26, 0.8));

		theme->set_color_role("code_folding_color_role", "CodeEdit", static_color_role);
		theme->set_color("code_folding_color", "CodeEdit", Color(0.8, 0.8, 0.8, 0.8));

		theme->set_color_role("folded_code_region_color_role", "CodeEdit", static_color_role);
		theme->set_color("folded_code_region_color", "CodeEdit", Color(0.68, 0.46, 0.77, 0.2));

		theme->set_color_role("caret_color_role", "CodeEdit", control_font_color_role);
		theme->set_color("caret_color", "CodeEdit", control_font_color);


		theme->set_color_role("caret_background_color_role", "CodeEdit", make_color_role(ColorRoleEnum::SCRIM));
		theme->set_color("caret_background_color", "CodeEdit", Color(0, 0, 0));


		theme->set_color_role("brace_mismatch_color_role", "CodeEdit", static_color_role);
		theme->set_color("brace_mismatch_color", "CodeEdit", Color(1, 0.2, 0.2));

		theme->set_color_role("line_number_color_role", "CodeEdit", static_color_role);
		theme->set_color("line_number_color", "CodeEdit", Color(0.67, 0.67, 0.67, 0.4));

		theme->set_color_role("word_highlighted_color_role", "CodeEdit", static_color_role);
		theme->set_color("word_highlighted_color", "CodeEdit", Color(0.8, 0.9, 0.9, 0.15));

		theme->set_color_role("line_length_guideline_color_role", "CodeEdit", static_color_role);
		theme->set_color("line_length_guideline_color", "CodeEdit", Color(0.3, 0.5, 0.8, 0.1));

		theme->set_color_role("search_result_color_role", "CodeEdit", static_color_role);
		theme->set_color("search_result_color", "CodeEdit", Color(0.3, 0.3, 0.3));

		theme->set_color_role("search_result_border_color_role", "CodeEdit", static_color_role);
		theme->set_color("search_result_border_color", "CodeEdit", Color(0.3, 0.3, 0.3, 0.4));

		theme->set_constant("completion_lines", "CodeEdit", 7);
		theme->set_constant("completion_max_width", "CodeEdit", 50);
		theme->set_constant("completion_scroll_width", "CodeEdit", 6);
		theme->set_constant("line_spacing", "CodeEdit", Math::round(4 * scale));
		theme->set_constant("outline_size", "CodeEdit", 0);
	}

	// Theme
	default_icon = icons["error_icon"];
	// Same color as the error icon.
	default_style = make_flat_stylebox(Color(1, 0.365, 0.365), 4, 4, 4, 4, 0, false, 2);


	// // ElevatedButton
	// {
	// 	theme->set_type_variation("ElevatedButton", "Button");
	// 	const Ref<StyleBoxFlat> elevated_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_LOW), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_1);
	// 	const Ref<StyleBoxFlat> elevated_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_LOW), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_2);
	// 	const Ref<StyleBoxFlat> elevated_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_LOW), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_1);
	// 	const Ref<StyleBoxFlat> elevated_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> elevated_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	elevated_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> elevated_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	const Ref<StyleBoxFlat> elevated_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	Ref<StyleBoxFlat> elevated_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	theme->set_stylebox("normal", "ElevatedButton", elevated_button_normal);
	// 	theme->set_stylebox("hover", "ElevatedButton", elevated_button_hover);
	// 	theme->set_stylebox("pressed", "ElevatedButton", elevated_button_pressed);
	// 	theme->set_stylebox("disabled", "ElevatedButton", elevated_button_disabled);
	// 	theme->set_stylebox("focus", "ElevatedButton", elevated_focus);

	// 	theme->set_stylebox("hover_state_layer", "ElevatedButton", elevated_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "ElevatedButton", elevated_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "ElevatedButton", elevated_focus_state_layer);

	// 	theme->set_font("font", "ElevatedButton", default_font);
	// 	theme->set_font("text_icon_font", "ElevatedButton", default_icon_font);
	// 	theme->set_font_size("font_size", "ElevatedButton", -1);
	// 	theme->set_font_size("text_icon_font_size", "ElevatedButton", 18);
	// 	theme->set_constant("outline_size", "ElevatedButton", 0);

	// 	theme->set_color_scheme("default_color_scheme", "ElevatedButton", default_color_scheme);

	// 	theme->set_color("font_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("font_focus_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("font_pressed_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("font_hover_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("font_hover_pressed_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("font_disabled_color_scale", "ElevatedButton", Color(one_color_scale, 0.38));
	// 	theme->set_color("font_outline_color_scale", "ElevatedButton", one_color_scale);

	// 	theme->set_color("icon_normal_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("icon_focus_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("icon_pressed_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("icon_hover_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("icon_hover_pressed_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("icon_disabled_color_scale", "ElevatedButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("text_icon_normal_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "ElevatedButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "ElevatedButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("font_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_focus_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_disabled_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_outline_color_scheme", "ElevatedButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("icon_normal_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_focus_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_disabled_color_scheme", "ElevatedButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "ElevatedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "ElevatedButton", Ref<ColorScheme>());

	// 	theme->set_color_role("font_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_pressed_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_hover_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_focus_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_hover_pressed_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_disabled_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	theme->set_color_role("font_outline_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::OUTLINE));

	// 	theme->set_color_role("icon_normal_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_pressed_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_pressed_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_focus_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_disabled_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	// 	theme->set_color_role("text_icon_normal_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_pressed_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_hover_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_focus_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_disabled_color_role", "ElevatedButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("font_color", "ElevatedButton", control_font_color);
	// 	theme->set_color("font_pressed_color", "ElevatedButton", control_font_pressed_color);
	// 	theme->set_color("font_hover_color", "ElevatedButton", control_font_hover_color);
	// 	theme->set_color("font_focus_color", "ElevatedButton", control_font_focus_color);
	// 	theme->set_color("font_hover_pressed_color", "ElevatedButton", control_font_pressed_color);
	// 	theme->set_color("font_disabled_color", "ElevatedButton", control_font_disabled_color);
	// 	theme->set_color("font_outline_color", "ElevatedButton", Color(1, 1, 1));

	// 	theme->set_color("icon_normal_color", "ElevatedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_pressed_color", "ElevatedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_color", "ElevatedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_pressed_color", "ElevatedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_focus_color", "ElevatedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_disabled_color", "ElevatedButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("text_icon_normal_color", "ElevatedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "ElevatedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "ElevatedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "ElevatedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "ElevatedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "ElevatedButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("h_separation", "ElevatedButton", Math::round(4 * scale));
	// 	theme->set_constant("icon_max_width", "ElevatedButton", 0);
	// }

	// // FilledButton
	// {
	// 	theme->set_type_variation("FilledButton", "Button");
	// 	const Ref<StyleBoxFlat> filled_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	const Ref<StyleBoxFlat> filled_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_1);

	// 	const Ref<StyleBoxFlat> filled_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	const Ref<StyleBoxFlat> filled_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> filled_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	filled_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> filled_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	const Ref<StyleBoxFlat> filled_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	Ref<StyleBoxFlat> filled_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	theme->set_stylebox("normal", "FilledButton", filled_button_normal);
	// 	theme->set_stylebox("hover", "FilledButton", filled_button_hover);
	// 	theme->set_stylebox("pressed", "FilledButton", filled_button_pressed);
	// 	theme->set_stylebox("disabled", "FilledButton", filled_button_disabled);
	// 	theme->set_stylebox("focus", "FilledButton", filled_focus);

	// 	theme->set_stylebox("hover_state_layer", "FilledButton", filled_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "FilledButton", filled_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "FilledButton", filled_focus_state_layer);

	// 	theme->set_font("font", "FilledButton", default_font);
	// 	theme->set_font("text_icon_font", "FilledButton", default_icon_font);
	// 	theme->set_font_size("font_size", "FilledButton", -1);
	// 	theme->set_font_size("text_icon_font_size", "FilledButton", 18);
	// 	theme->set_constant("outline_size", "FilledButton", 0);

	// 	theme->set_color_scheme("default_color_scheme", "FilledButton", default_color_scheme);

	// 	theme->set_color("font_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("font_focus_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("font_pressed_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("font_hover_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("font_hover_pressed_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("font_disabled_color_scale", "FilledButton", Color(one_color_scale, 0.38));
	// 	theme->set_color("font_outline_color_scale", "FilledButton", one_color_scale);

	// 	theme->set_color("icon_normal_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("icon_focus_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("icon_pressed_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("icon_hover_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("icon_hover_pressed_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("icon_disabled_color_scale", "FilledButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("text_icon_normal_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "FilledButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "FilledButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("font_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_focus_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_disabled_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_outline_color_scheme", "FilledButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("icon_normal_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_focus_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_disabled_color_scheme", "FilledButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "FilledButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "FilledButton", Ref<ColorScheme>());

	// 	theme->set_color_role("font_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("font_pressed_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("font_hover_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("font_focus_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("font_hover_pressed_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("font_disabled_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	theme->set_color_role("font_outline_color_role", "FilledButton", make_color_role(ColorRoleEnum::OUTLINE));

	// 	theme->set_color_role("icon_normal_color_role", "FilledButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_pressed_color_role", "FilledButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_color_role", "FilledButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_pressed_color_role", "FilledButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_focus_color_role", "FilledButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_disabled_color_role", "FilledButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	// 	theme->set_color_role("text_icon_normal_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("text_icon_pressed_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("text_icon_hover_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("text_icon_focus_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("text_icon_disabled_color_role", "FilledButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("font_color", "FilledButton", control_font_color);
	// 	theme->set_color("font_pressed_color", "FilledButton", control_font_pressed_color);
	// 	theme->set_color("font_hover_color", "FilledButton", control_font_hover_color);
	// 	theme->set_color("font_focus_color", "FilledButton", control_font_focus_color);
	// 	theme->set_color("font_hover_pressed_color", "FilledButton", control_font_pressed_color);
	// 	theme->set_color("font_disabled_color", "FilledButton", control_font_disabled_color);
	// 	theme->set_color("font_outline_color", "FilledButton", Color(1, 1, 1));

	// 	theme->set_color("icon_normal_color", "FilledButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_pressed_color", "FilledButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_color", "FilledButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_pressed_color", "FilledButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_focus_color", "FilledButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_disabled_color", "FilledButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("text_icon_normal_color", "FilledButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "FilledButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "FilledButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "FilledButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "FilledButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "FilledButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("h_separation", "FilledButton", Math::round(4 * scale));
	// 	theme->set_constant("icon_max_width", "FilledButton", 0);
	// }

	// // FilledTonalButton
	// {
	// 	theme->set_type_variation("FilledTonalButton", "Button");
	// 	const Ref<StyleBoxFlat> filled_tonal_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	const Ref<StyleBoxFlat> filled_tonal_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_1);

	// 	const Ref<StyleBoxFlat> filled_tonal_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	const Ref<StyleBoxFlat> filled_tonal_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> filled_tonal_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	filled_tonal_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> filled_tonal_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	const Ref<StyleBoxFlat> filled_tonal_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	Ref<StyleBoxFlat> filled_tonal_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	theme->set_stylebox("normal", "FilledTonalButton", filled_tonal_button_normal);
	// 	theme->set_stylebox("hover", "FilledTonalButton", filled_tonal_button_hover);
	// 	theme->set_stylebox("pressed", "FilledTonalButton", filled_tonal_button_pressed);
	// 	theme->set_stylebox("disabled", "FilledTonalButton", filled_tonal_button_disabled);
	// 	theme->set_stylebox("focus", "FilledTonalButton", filled_tonal_focus);

	// 	theme->set_stylebox("hover_state_layer", "FilledTonalButton", filled_tonal_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "FilledTonalButton", filled_tonal_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "FilledTonalButton", filled_tonal_focus_state_layer);

	// 	theme->set_font("font", "FilledTonalButton", default_font);
	// 	theme->set_font("text_icon_font", "FilledTonalButton", default_icon_font);
	// 	theme->set_font_size("font_size", "FilledTonalButton", -1);
	// 	theme->set_font_size("text_icon_font_size", "FilledTonalButton", 18);
	// 	theme->set_constant("outline_size", "FilledTonalButton", 0);

	// 	theme->set_color_scheme("default_color_scheme", "FilledTonalButton", default_color_scheme);

	// 	theme->set_color("font_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("font_focus_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("font_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("font_hover_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("font_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("font_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));
	// 	theme->set_color("font_outline_color_scale", "FilledTonalButton", one_color_scale);

	// 	theme->set_color("icon_normal_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("icon_focus_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("icon_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("icon_hover_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("icon_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("icon_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("text_icon_normal_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("font_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_outline_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("icon_normal_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

	// 	theme->set_color_role("font_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("font_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("font_hover_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("font_focus_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("font_hover_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("font_disabled_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	theme->set_color_role("font_outline_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::OUTLINE));

	// 	theme->set_color_role("icon_normal_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_focus_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_disabled_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	// 	theme->set_color_role("text_icon_normal_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_hover_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_focus_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_disabled_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("font_color", "FilledTonalButton", control_font_color);
	// 	theme->set_color("font_pressed_color", "FilledTonalButton", control_font_pressed_color);
	// 	theme->set_color("font_hover_color", "FilledTonalButton", control_font_hover_color);
	// 	theme->set_color("font_focus_color", "FilledTonalButton", control_font_focus_color);
	// 	theme->set_color("font_hover_pressed_color", "FilledTonalButton", control_font_pressed_color);
	// 	theme->set_color("font_disabled_color", "FilledTonalButton", control_font_disabled_color);
	// 	theme->set_color("font_outline_color", "FilledTonalButton", Color(1, 1, 1));

	// 	theme->set_color("icon_normal_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_focus_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_disabled_color", "FilledTonalButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("text_icon_normal_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "FilledTonalButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("h_separation", "FilledTonalButton", Math::round(4 * scale));
	// 	theme->set_constant("icon_max_width", "FilledTonalButton", 0);
	// }

	// // OutlinedButton
	// {
	// 	theme->set_type_variation("OutlinedButton", "Button");
	// 	const Ref<StyleBoxFlat> filled_tonal_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	const Ref<StyleBoxFlat> filled_tonal_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_1);

	// 	const Ref<StyleBoxFlat> filled_tonal_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	const Ref<StyleBoxFlat> filled_tonal_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> filled_tonal_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	filled_tonal_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> filled_tonal_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	const Ref<StyleBoxFlat> filled_tonal_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	Ref<StyleBoxFlat> filled_tonal_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	theme->set_stylebox("normal", "FilledTonalButton", filled_tonal_button_normal);
	// 	theme->set_stylebox("hover", "FilledTonalButton", filled_tonal_button_hover);
	// 	theme->set_stylebox("pressed", "FilledTonalButton", filled_tonal_button_pressed);
	// 	theme->set_stylebox("disabled", "FilledTonalButton", filled_tonal_button_disabled);
	// 	theme->set_stylebox("focus", "FilledTonalButton", filled_tonal_focus);

	// 	theme->set_stylebox("hover_state_layer", "FilledTonalButton", filled_tonal_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "FilledTonalButton", filled_tonal_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "FilledTonalButton", filled_tonal_focus_state_layer);

	// 	theme->set_font("font", "FilledTonalButton", default_font);
	// 	theme->set_font("text_icon_font", "FilledTonalButton", default_icon_font);
	// 	theme->set_font_size("font_size", "FilledTonalButton", -1);
	// 	theme->set_font_size("text_icon_font_size", "FilledTonalButton", 18);
	// 	theme->set_constant("outline_size", "FilledTonalButton", 0);

	// 	theme->set_color_scheme("default_color_scheme", "FilledTonalButton", default_color_scheme);

	// 	theme->set_color("font_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("font_focus_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("font_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("font_hover_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("font_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("font_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));
	// 	theme->set_color("font_outline_color_scale", "FilledTonalButton", one_color_scale);

	// 	theme->set_color("icon_normal_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("icon_focus_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("icon_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("icon_hover_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("icon_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("icon_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("text_icon_normal_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "FilledTonalButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "FilledTonalButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("font_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_outline_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("icon_normal_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "FilledTonalButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "FilledTonalButton", Ref<ColorScheme>());

	// 	theme->set_color_role("font_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("font_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("font_hover_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("font_focus_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("font_hover_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("font_disabled_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	theme->set_color_role("font_outline_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::OUTLINE));

	// 	theme->set_color_role("icon_normal_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_focus_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_disabled_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	// 	theme->set_color_role("text_icon_normal_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_hover_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_focus_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_disabled_color_role", "FilledTonalButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("font_color", "FilledTonalButton", control_font_color);
	// 	theme->set_color("font_pressed_color", "FilledTonalButton", control_font_pressed_color);
	// 	theme->set_color("font_hover_color", "FilledTonalButton", control_font_hover_color);
	// 	theme->set_color("font_focus_color", "FilledTonalButton", control_font_focus_color);
	// 	theme->set_color("font_hover_pressed_color", "FilledTonalButton", control_font_pressed_color);
	// 	theme->set_color("font_disabled_color", "FilledTonalButton", control_font_disabled_color);
	// 	theme->set_color("font_outline_color", "FilledTonalButton", Color(1, 1, 1));

	// 	theme->set_color("icon_normal_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_focus_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_disabled_color", "FilledTonalButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("text_icon_normal_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "FilledTonalButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "FilledTonalButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("h_separation", "FilledTonalButton", Math::round(4 * scale));
	// 	theme->set_constant("icon_max_width", "FilledTonalButton", 0);
	// }

	// // OutlinedButton
	// {
	// 	theme->set_type_variation("OutlinedButton", "Button");

	// 	Ref<StyleBoxFlat> outlined_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	outlined_button_normal->set_expand_margin_all(Math::round(2 * scale));
	// 	outlined_button_normal->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> outlined_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	outlined_button_hover->set_expand_margin_all(Math::round(2 * scale));
	// 	outlined_button_hover->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> outlined_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	outlined_button_pressed->set_expand_margin_all(Math::round(2 * scale));
	// 	outlined_button_pressed->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> outlined_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	outlined_button_disabled->set_expand_margin_all(Math::round(2 * scale));
	// 	outlined_button_disabled->set_border_color_role(make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	outlined_button_disabled->set_border_color_scale(Color(1, 1, 1, 0.12));

	// 	Ref<StyleBoxFlat> outlined_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	outlined_focus->set_expand_margin_all(Math::round(2 * scale));
	// 	outlined_focus->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));
	// 	outlined_focus->set_border_color_scale(Color(1, 1, 1, 1));

	// 	const Ref<StyleBoxFlat> outlined_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	const Ref<StyleBoxFlat> outlined_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> outlined_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	theme->set_stylebox("normal", "OutlinedButton", outlined_button_normal);
	// 	theme->set_stylebox("hover", "OutlinedButton", outlined_button_hover);
	// 	theme->set_stylebox("pressed", "OutlinedButton", outlined_button_pressed);
	// 	theme->set_stylebox("disabled", "OutlinedButton", outlined_button_disabled);
	// 	theme->set_stylebox("focus", "OutlinedButton", outlined_focus);

	// 	theme->set_stylebox("hover_state_layer", "OutlinedButton", outlined_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "OutlinedButton", outlined_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "OutlinedButton", outlined_focus_state_layer);

	// 	theme->set_font("font", "OutlinedButton", default_font);
	// 	theme->set_font("text_icon_font", "OutlinedButton", default_icon_font);
	// 	theme->set_font_size("font_size", "OutlinedButton", -1);
	// 	theme->set_font_size("text_icon_font_size", "OutlinedButton", 18);
	// 	theme->set_constant("outline_size", "OutlinedButton", 0);

	// 	theme->set_color_scheme("default_color_scheme", "OutlinedButton", default_color_scheme);

	// 	theme->set_color("font_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("font_focus_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("font_pressed_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("font_hover_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("font_hover_pressed_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("font_disabled_color_scale", "OutlinedButton", Color(one_color_scale, 0.38));
	// 	theme->set_color("font_outline_color_scale", "OutlinedButton", one_color_scale);

	// 	theme->set_color("icon_normal_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("icon_focus_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("icon_pressed_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("icon_hover_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("icon_hover_pressed_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("icon_disabled_color_scale", "OutlinedButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("text_icon_normal_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "OutlinedButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "OutlinedButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("font_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_focus_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_disabled_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_outline_color_scheme", "OutlinedButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("icon_normal_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_focus_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_disabled_color_scheme", "OutlinedButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "OutlinedButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "OutlinedButton", Ref<ColorScheme>());

	// 	theme->set_color_role("font_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_pressed_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_hover_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_focus_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_hover_pressed_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_disabled_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	theme->set_color_role("font_outline_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::OUTLINE));

	// 	theme->set_color_role("icon_normal_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_pressed_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_pressed_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_focus_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_disabled_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	// 	theme->set_color_role("text_icon_normal_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_pressed_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_hover_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_focus_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_disabled_color_role", "OutlinedButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("font_color", "OutlinedButton", control_font_color);
	// 	theme->set_color("font_pressed_color", "OutlinedButton", control_font_pressed_color);
	// 	theme->set_color("font_hover_color", "OutlinedButton", control_font_hover_color);
	// 	theme->set_color("font_focus_color", "OutlinedButton", control_font_focus_color);
	// 	theme->set_color("font_hover_pressed_color", "OutlinedButton", control_font_pressed_color);
	// 	theme->set_color("font_disabled_color", "OutlinedButton", control_font_disabled_color);
	// 	theme->set_color("font_outline_color", "OutlinedButton", Color(1, 1, 1));

	// 	theme->set_color("icon_normal_color", "OutlinedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_pressed_color", "OutlinedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_color", "OutlinedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_pressed_color", "OutlinedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_focus_color", "OutlinedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_disabled_color", "OutlinedButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("text_icon_normal_color", "OutlinedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "OutlinedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "OutlinedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "OutlinedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "OutlinedButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "OutlinedButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("h_separation", "OutlinedButton", Math::round(4 * scale));
	// 	theme->set_constant("icon_max_width", "OutlinedButton", 0);
	// }

	// // FabButton
	// {
	// 	theme->set_type_variation("FabButton", "Button");
	// 	const Ref<StyleBoxFlat> fab_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_3);
	// 	const Ref<StyleBoxFlat> fab_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_4);
	// 	const Ref<StyleBoxFlat> fab_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_3);
	// 	const Ref<StyleBoxFlat> fab_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> fab_button_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	fab_button_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> fab_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	const Ref<StyleBoxFlat> fab_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	Ref<StyleBoxFlat> fab_button_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	theme->set_stylebox("normal", "FabButton", fab_button_normal);
	// 	theme->set_stylebox("hover", "FabButton", fab_button_hover);
	// 	theme->set_stylebox("pressed", "FabButton", fab_button_pressed);
	// 	theme->set_stylebox("disabled", "FabButton", fab_button_disabled);
	// 	theme->set_stylebox("focus", "FabButton", fab_button_focus);

	// 	theme->set_stylebox("hover_state_layer", "FabButton", fab_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "FabButton", fab_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "FabButton", fab_button_focus_state_layer);

	// 	theme->set_font("font", "FabButton", default_font);
	// 	theme->set_font("text_icon_font", "FabButton", default_icon_font);
	// 	theme->set_font_size("font_size", "FabButton", -1);
	// 	theme->set_font_size("text_icon_font_size", "FabButton", 24);
	// 	theme->set_constant("outline_size", "FabButton", 0);

	// 	theme->set_color_scheme("default_color_scheme", "FabButton", default_color_scheme);

	// 	theme->set_color("font_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("font_focus_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("font_pressed_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("font_hover_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("font_hover_pressed_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("font_disabled_color_scale", "FabButton", Color(one_color_scale, 0.38));
	// 	theme->set_color("font_outline_color_scale", "FabButton", one_color_scale);

	// 	theme->set_color("icon_normal_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("icon_focus_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("icon_pressed_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("icon_hover_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("icon_hover_pressed_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("icon_disabled_color_scale", "FabButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("text_icon_normal_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "FabButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "FabButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("font_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_pressed_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_focus_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_pressed_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_disabled_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_outline_color_scheme", "FabButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("icon_normal_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_pressed_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_focus_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_pressed_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_disabled_color_scheme", "FabButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "FabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "FabButton", Ref<ColorScheme>());

	// 	theme->set_color_role("font_color_role", "FabButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_pressed_color_role", "FabButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_hover_color_role", "FabButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_focus_color_role", "FabButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_hover_pressed_color_role", "FabButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_disabled_color_role", "FabButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	theme->set_color_role("font_outline_color_role", "FabButton", make_color_role(ColorRoleEnum::OUTLINE));

	// 	theme->set_color_role("icon_normal_color_role", "FabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_pressed_color_role", "FabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_color_role", "FabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_pressed_color_role", "FabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_focus_color_role", "FabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_disabled_color_role", "FabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	// 	theme->set_color_role("text_icon_normal_color_role", "FabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("text_icon_pressed_color_role", "FabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("text_icon_hover_color_role", "FabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "FabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("text_icon_focus_color_role", "FabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("text_icon_disabled_color_role", "FabButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("font_color", "FabButton", control_font_color);
	// 	theme->set_color("font_pressed_color", "FabButton", control_font_pressed_color);
	// 	theme->set_color("font_hover_color", "FabButton", control_font_hover_color);
	// 	theme->set_color("font_focus_color", "FabButton", control_font_focus_color);
	// 	theme->set_color("font_hover_pressed_color", "FabButton", control_font_pressed_color);
	// 	theme->set_color("font_disabled_color", "FabButton", control_font_disabled_color);
	// 	theme->set_color("font_outline_color", "FabButton", Color(1, 1, 1));

	// 	theme->set_color("icon_normal_color", "FabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_pressed_color", "FabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_color", "FabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_pressed_color", "FabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_focus_color", "FabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_disabled_color", "FabButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("text_icon_normal_color", "FabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "FabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "FabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "FabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "FabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "FabButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("h_separation", "FabButton", Math::round(4 * scale));
	// 	theme->set_constant("icon_max_width", "FabButton", 0);
	// }

	// // ExtendedFabButton
	// {
	// 	theme->set_type_variation("ExtendedFabButton", "Button");
	// 	const Ref<StyleBoxFlat> extended_fab_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_3);
	// 	const Ref<StyleBoxFlat> extended_fab_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_4);
	// 	const Ref<StyleBoxFlat> extended_fab_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_3);
	// 	const Ref<StyleBoxFlat> extended_fab_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> extended_fab_button_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	extended_fab_button_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> extended_fab_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	const Ref<StyleBoxFlat> extended_fab_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	Ref<StyleBoxFlat> extended_fab_button_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	theme->set_stylebox("normal", "ExtendedFabButton", extended_fab_button_normal);
	// 	theme->set_stylebox("hover", "ExtendedFabButton", extended_fab_button_hover);
	// 	theme->set_stylebox("pressed", "ExtendedFabButton", extended_fab_button_pressed);
	// 	theme->set_stylebox("disabled", "ExtendedFabButton", extended_fab_button_disabled);
	// 	theme->set_stylebox("focus", "ExtendedFabButton", extended_fab_button_focus);

	// 	theme->set_stylebox("hover_state_layer", "ExtendedFabButton", extended_fab_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "ExtendedFabButton", extended_fab_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "ExtendedFabButton", extended_fab_button_focus_state_layer);

	// 	theme->set_font("font", "ExtendedFabButton", default_font);
	// 	theme->set_font("text_icon_font", "ExtendedFabButton", default_icon_font);
	// 	theme->set_font_size("font_size", "ExtendedFabButton", -1);
	// 	theme->set_font_size("text_icon_font_size", "ExtendedFabButton", 24);
	// 	theme->set_constant("outline_size", "ExtendedFabButton", 0);

	// 	theme->set_color_scheme("default_color_scheme", "ExtendedFabButton", default_color_scheme);

	// 	theme->set_color("font_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("font_focus_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("font_pressed_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("font_hover_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("font_hover_pressed_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("font_disabled_color_scale", "ExtendedFabButton", Color(one_color_scale, 0.38));
	// 	theme->set_color("font_outline_color_scale", "ExtendedFabButton", one_color_scale);

	// 	theme->set_color("icon_normal_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("icon_focus_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("icon_pressed_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("icon_hover_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("icon_hover_pressed_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("icon_disabled_color_scale", "ExtendedFabButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("text_icon_normal_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "ExtendedFabButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "ExtendedFabButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("font_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_pressed_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_focus_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_pressed_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_disabled_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_outline_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("icon_normal_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_pressed_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_focus_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_pressed_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_disabled_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "ExtendedFabButton", Ref<ColorScheme>());

	// 	theme->set_color_role("font_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("font_pressed_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("font_hover_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("font_focus_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("font_hover_pressed_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("font_disabled_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	theme->set_color_role("font_outline_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::OUTLINE));

	// 	theme->set_color_role("icon_normal_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_pressed_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_pressed_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_focus_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_disabled_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	// 	theme->set_color_role("text_icon_normal_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("text_icon_pressed_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("text_icon_hover_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("text_icon_focus_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// 	theme->set_color_role("text_icon_disabled_color_role", "ExtendedFabButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("font_color", "ExtendedFabButton", control_font_color);
	// 	theme->set_color("font_pressed_color", "ExtendedFabButton", control_font_pressed_color);
	// 	theme->set_color("font_hover_color", "ExtendedFabButton", control_font_hover_color);
	// 	theme->set_color("font_focus_color", "ExtendedFabButton", control_font_focus_color);
	// 	theme->set_color("font_hover_pressed_color", "ExtendedFabButton", control_font_pressed_color);
	// 	theme->set_color("font_disabled_color", "ExtendedFabButton", control_font_disabled_color);
	// 	theme->set_color("font_outline_color", "ExtendedFabButton", Color(1, 1, 1));

	// 	theme->set_color("icon_normal_color", "ExtendedFabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_pressed_color", "ExtendedFabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_color", "ExtendedFabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_pressed_color", "ExtendedFabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_focus_color", "ExtendedFabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_disabled_color", "ExtendedFabButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("text_icon_normal_color", "ExtendedFabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "ExtendedFabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "ExtendedFabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "ExtendedFabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "ExtendedFabButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "ExtendedFabButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("h_separation", "ExtendedFabButton", Math::round(4 * scale));
	// 	theme->set_constant("icon_max_width", "ExtendedFabButton", 0);
	// }

	// // TextButton
	// {
	// 	theme->set_type_variation("TextButton", "Button");

	// 	Ref<StyleBoxFlat> text_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> text_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> text_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> text_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	Ref<StyleBoxFlat> text_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, default_corner_radius, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	text_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> text_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	const Ref<StyleBoxFlat> text_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);
	// 	Ref<StyleBoxFlat> text_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0);

	// 	theme->set_stylebox("normal", "TextButton", text_button_normal);
	// 	theme->set_stylebox("hover", "TextButton", text_button_hover);
	// 	theme->set_stylebox("pressed", "TextButton", text_button_pressed);
	// 	theme->set_stylebox("disabled", "TextButton", text_button_disabled);
	// 	theme->set_stylebox("focus", "TextButton", text_focus);

	// 	theme->set_stylebox("hover_state_layer", "TextButton", text_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "TextButton", text_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "TextButton", text_focus_state_layer);

	// 	theme->set_font("font", "TextButton", default_font);
	// 	theme->set_font("text_icon_font", "TextButton", default_icon_font);
	// 	theme->set_font_size("font_size", "TextButton", -1);
	// 	theme->set_font_size("text_icon_font_size", "TextButton", 18);
	// 	theme->set_constant("outline_size", "TextButton", 0);

	// 	theme->set_color_scheme("default_color_scheme", "TextButton", default_color_scheme);

	// 	theme->set_color("font_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("font_focus_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("font_pressed_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("font_hover_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("font_hover_pressed_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("font_disabled_color_scale", "TextButton", Color(one_color_scale, 0.38));
	// 	theme->set_color("font_outline_color_scale", "TextButton", one_color_scale);

	// 	theme->set_color("icon_normal_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("icon_focus_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("icon_pressed_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("icon_hover_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("icon_hover_pressed_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("icon_disabled_color_scale", "TextButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("text_icon_normal_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "TextButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "TextButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("font_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_focus_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_hover_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_disabled_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("font_outline_color_scheme", "TextButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("icon_normal_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_focus_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_hover_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("icon_disabled_color_scheme", "TextButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "TextButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "TextButton", Ref<ColorScheme>());

	// 	theme->set_color_role("font_color_role", "TextButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_pressed_color_role", "TextButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_hover_color_role", "TextButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_focus_color_role", "TextButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_hover_pressed_color_role", "TextButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("font_disabled_color_role", "TextButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	theme->set_color_role("font_outline_color_role", "TextButton", make_color_role(ColorRoleEnum::OUTLINE));

	// 	theme->set_color_role("icon_normal_color_role", "TextButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_pressed_color_role", "TextButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_color_role", "TextButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_hover_pressed_color_role", "TextButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_focus_color_role", "TextButton", make_color_role(ColorRoleEnum::STATIC_COLOR));
	// 	theme->set_color_role("icon_disabled_color_role", "TextButton", make_color_role(ColorRoleEnum::STATIC_COLOR));

	// 	theme->set_color_role("text_icon_normal_color_role", "TextButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_pressed_color_role", "TextButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_hover_color_role", "TextButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "TextButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_focus_color_role", "TextButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_disabled_color_role", "TextButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("font_color", "TextButton", control_font_color);
	// 	theme->set_color("font_pressed_color", "TextButton", control_font_pressed_color);
	// 	theme->set_color("font_hover_color", "TextButton", control_font_hover_color);
	// 	theme->set_color("font_focus_color", "TextButton", control_font_focus_color);
	// 	theme->set_color("font_hover_pressed_color", "TextButton", control_font_pressed_color);
	// 	theme->set_color("font_disabled_color", "TextButton", control_font_disabled_color);
	// 	theme->set_color("font_outline_color", "TextButton", Color(1, 1, 1));

	// 	theme->set_color("icon_normal_color", "TextButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_pressed_color", "TextButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_color", "TextButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_hover_pressed_color", "TextButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_focus_color", "TextButton", Color(1, 1, 1, 1));
	// 	theme->set_color("icon_disabled_color", "TextButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("text_icon_normal_color", "TextButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "TextButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "TextButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "TextButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "TextButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "TextButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("h_separation", "TextButton", Math::round(4 * scale));
	// 	theme->set_constant("icon_max_width", "TextButton", 0);
	// }

	// // TextureButton
	// {
	// 	// theme->set_type_variation("IconButton", "TextureButton");

	// 	Ref<StyleBoxFlat> texture_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> texture_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> texture_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> texture_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> texture_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	texture_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> texture_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> texture_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> texture_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	theme->set_stylebox("normal", "TextureButton", texture_button_normal);
	// 	theme->set_stylebox("hover", "TextureButton", texture_button_hover);
	// 	theme->set_stylebox("pressed", "TextureButton", texture_button_pressed);
	// 	theme->set_stylebox("disabled", "TextureButton", texture_button_disabled);
	// 	theme->set_stylebox("focus", "TextureButton", texture_focus);

	// 	theme->set_stylebox("hover_state_layer", "TextureButton", texture_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "TextureButton", texture_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "TextureButton", texture_focus_state_layer);

	// 	theme->set_font("text_icon_font", "TextureButton", default_icon_font);
	// 	theme->set_font_size("text_icon_font_size", "TextureButton", 24);

	// 	theme->set_color_scheme("default_color_scheme", "TextureButton", default_color_scheme);

	// 	theme->set_color("text_icon_normal_color_scale", "TextureButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "TextureButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "TextureButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "TextureButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "TextureButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "TextureButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "TextureButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "TextureButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "TextureButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "TextureButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "TextureButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "TextureButton", Ref<ColorScheme>());

	// 	theme->set_color_role("text_icon_normal_color_role", "TextureButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_pressed_color_role", "TextureButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_hover_color_role", "TextureButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "TextureButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_focus_color_role", "TextureButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_disabled_color_role", "TextureButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("text_icon_normal_color", "TextureButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "TextureButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "TextureButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "TextureButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "TextureButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "TextureButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("icon_max_width", "TextureButton", 0);
	// }

	// // FilledIconButton
	// {
	// 	theme->set_type_variation("FilledIconButton", "TextureButton");

	// 	Ref<StyleBoxFlat> filled_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> filled_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> filled_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> filled_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> filled_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	filled_icon_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> filled_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> filled_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> filled_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	filled_icon_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> selected_filled_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> selected_filled_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_PRIMARY), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	filled_icon_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> unselected_filled_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> unselected_filled_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	theme->set_stylebox("normal", "FilledIconButton", filled_icon_button_normal);
	// 	theme->set_stylebox("hover", "FilledIconButton", filled_icon_button_hover);
	// 	theme->set_stylebox("pressed", "FilledIconButton", filled_icon_button_pressed);
	// 	theme->set_stylebox("disabled", "FilledIconButton", filled_icon_button_disabled);
	// 	theme->set_stylebox("focus", "FilledIconButton", filled_icon_focus);

	// 	theme->set_stylebox("hover_state_layer", "FilledIconButton", filled_icon_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "FilledIconButton", filled_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "FilledIconButton", filled_icon_focus_state_layer);

	// 	theme->set_stylebox("selected_normal", "FilledIconButton", selected_filled_icon_button_normal);
	// 	theme->set_stylebox("selected_hover", "FilledIconButton", selected_filled_icon_button_hover);
	// 	theme->set_stylebox("selected_pressed", "FilledIconButton", selected_filled_icon_button_pressed);
	// 	theme->set_stylebox("selected_disabled", "FilledIconButton", selected_filled_icon_button_disabled);
	// 	theme->set_stylebox("selected_focus", "FilledIconButton", selected_filled_icon_focus);

	// 	theme->set_stylebox("selected_hover_state_layer", "FilledIconButton", selected_filled_icon_button_hover_state_layer);
	// 	theme->set_stylebox("selected_pressed_state_layer", "FilledIconButton", selected_filled_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("selected_focus_state_layer", "FilledIconButton", selected_filled_icon_focus_state_layer);

	// 	theme->set_stylebox("unselected_normal", "FilledIconButton", unselected_filled_icon_button_normal);
	// 	theme->set_stylebox("unselected_hover", "FilledIconButton", unselected_filled_icon_button_hover);
	// 	theme->set_stylebox("unselected_pressed", "FilledIconButton", unselected_filled_icon_button_pressed);
	// 	theme->set_stylebox("unselected_disabled", "FilledIconButton", unselected_filled_icon_button_disabled);
	// 	theme->set_stylebox("unselected_focus", "FilledIconButton", unselected_filled_icon_focus);

	// 	theme->set_stylebox("unselected_hover_state_layer", "FilledIconButton", unselected_filled_icon_button_hover_state_layer);
	// 	theme->set_stylebox("unselected_pressed_state_layer", "FilledIconButton", unselected_filled_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("unselected_focus_state_layer", "FilledIconButton", unselected_filled_icon_focus_state_layer);

	// 	theme->set_font("text_icon_font", "FilledIconButton", default_icon_font);
	// 	theme->set_font_size("text_icon_font_size", "FilledIconButton", 24);

	// 	theme->set_color_scheme("default_color_scheme", "FilledIconButton", default_color_scheme);

	// 	theme->set_color("text_icon_normal_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "FilledIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("selected_text_icon_normal_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_focus_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_pressed_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_hover_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_hover_pressed_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_disabled_color_scale", "FilledIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("unselected_text_icon_normal_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_focus_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_pressed_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_hover_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_hover_pressed_color_scale", "FilledIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_disabled_color_scale", "FilledIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "FilledIconButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("selected_text_icon_normal_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_pressed_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_hover_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_focus_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_hover_pressed_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_disabled_color_scheme", "FilledIconButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("unselected_text_icon_normal_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_pressed_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_hover_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_focus_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_hover_pressed_color_scheme", "FilledIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_disabled_color_scheme", "FilledIconButton", Ref<ColorScheme>());

	// 	theme->set_color_role("text_icon_normal_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("text_icon_pressed_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("text_icon_hover_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("text_icon_focus_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("text_icon_disabled_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color_role("selected_text_icon_normal_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("selected_text_icon_pressed_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("selected_text_icon_hover_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("selected_text_icon_hover_pressed_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("selected_text_icon_focus_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// 	theme->set_color_role("selected_text_icon_disabled_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color_role("unselected_text_icon_normal_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("unselected_text_icon_pressed_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("unselected_text_icon_hover_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("unselected_text_icon_hover_pressed_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("unselected_text_icon_focus_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("unselected_text_icon_disabled_color_role", "FilledIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("text_icon_normal_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "FilledIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("selected_text_icon_normal_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_pressed_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_hover_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_hover_pressed_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_focus_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_disabled_color", "FilledIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("unselected_text_icon_normal_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_pressed_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_hover_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_hover_pressed_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_focus_color", "FilledIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_disabled_color", "FilledIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("icon_max_width", "FilledIconButton", 0);
	// }

	// // FilledTonalIconButton
	// {
	// 	theme->set_type_variation("FilledTonalIconButton", "TextureButton");

	// 	Ref<StyleBoxFlat> filled_tonal_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> filled_tonal_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> filled_tonal_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> filled_tonal_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> filled_tonal_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	filled_tonal_icon_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> filled_tonal_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> filled_tonal_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> filled_tonal_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_tonal_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_tonal_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_tonal_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_tonal_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_tonal_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	filled_tonal_icon_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> selected_filled_tonal_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> selected_filled_tonal_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_filled_tonal_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_tonal_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_tonal_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_tonal_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER_HIGHEST), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_tonal_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), Color(style_one_color_scale, 0.12), StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_tonal_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	// Make the focus outline appear to be flush with the buttons it's focusing.
	// 	filled_tonal_icon_focus->set_expand_margin_all(Math::round(2 * scale));

	// 	const Ref<StyleBoxFlat> unselected_filled_tonal_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> unselected_filled_tonal_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_filled_tonal_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	theme->set_stylebox("normal", "FilledTonalIconButton", filled_tonal_icon_button_normal);
	// 	theme->set_stylebox("hover", "FilledTonalIconButton", filled_tonal_icon_button_hover);
	// 	theme->set_stylebox("pressed", "FilledTonalIconButton", filled_tonal_icon_button_pressed);
	// 	theme->set_stylebox("disabled", "FilledTonalIconButton", filled_tonal_icon_button_disabled);
	// 	theme->set_stylebox("focus", "FilledTonalIconButton", filled_tonal_icon_focus);

	// 	theme->set_stylebox("hover_state_layer", "FilledTonalIconButton", filled_tonal_icon_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "FilledTonalIconButton", filled_tonal_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "FilledTonalIconButton", filled_tonal_icon_focus_state_layer);

	// 	theme->set_stylebox("selected_normal", "FilledTonalIconButton", selected_filled_tonal_icon_button_normal);
	// 	theme->set_stylebox("selected_hover", "FilledTonalIconButton", selected_filled_tonal_icon_button_hover);
	// 	theme->set_stylebox("selected_pressed", "FilledTonalIconButton", selected_filled_tonal_icon_button_pressed);
	// 	theme->set_stylebox("selected_disabled", "FilledTonalIconButton", selected_filled_tonal_icon_button_disabled);
	// 	theme->set_stylebox("selected_focus", "FilledTonalIconButton", selected_filled_tonal_icon_focus);

	// 	theme->set_stylebox("selected_hover_state_layer", "FilledTonalIconButton", selected_filled_tonal_icon_button_hover_state_layer);
	// 	theme->set_stylebox("selected_pressed_state_layer", "FilledTonalIconButton", selected_filled_tonal_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("selected_focus_state_layer", "FilledTonalIconButton", selected_filled_tonal_icon_focus_state_layer);

	// 	theme->set_stylebox("unselected_normal", "FilledTonalIconButton", unselected_filled_tonal_icon_button_normal);
	// 	theme->set_stylebox("unselected_hover", "FilledTonalIconButton", unselected_filled_tonal_icon_button_hover);
	// 	theme->set_stylebox("unselected_pressed", "FilledTonalIconButton", unselected_filled_tonal_icon_button_pressed);
	// 	theme->set_stylebox("unselected_disabled", "FilledTonalIconButton", unselected_filled_tonal_icon_button_disabled);
	// 	theme->set_stylebox("unselected_focus", "FilledTonalIconButton", unselected_filled_tonal_icon_focus);

	// 	theme->set_stylebox("unselected_hover_state_layer", "FilledTonalIconButton", unselected_filled_tonal_icon_button_hover_state_layer);
	// 	theme->set_stylebox("unselected_pressed_state_layer", "FilledTonalIconButton", unselected_filled_tonal_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("unselected_focus_state_layer", "FilledTonalIconButton", unselected_filled_tonal_icon_focus_state_layer);

	// 	theme->set_font("text_icon_font", "FilledTonalIconButton", default_icon_font);
	// 	theme->set_font_size("text_icon_font_size", "FilledTonalIconButton", 24);

	// 	theme->set_color_scheme("default_color_scheme", "FilledTonalIconButton", default_color_scheme);

	// 	theme->set_color("text_icon_normal_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "FilledTonalIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("selected_text_icon_normal_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_focus_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_pressed_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_hover_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_hover_pressed_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_disabled_color_scale", "FilledTonalIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("unselected_text_icon_normal_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_focus_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_pressed_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_hover_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_hover_pressed_color_scale", "FilledTonalIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_disabled_color_scale", "FilledTonalIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("selected_text_icon_normal_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_pressed_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_hover_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_focus_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_hover_pressed_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_disabled_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("unselected_text_icon_normal_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_pressed_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_hover_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_focus_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_hover_pressed_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_disabled_color_scheme", "FilledTonalIconButton", Ref<ColorScheme>());

	// 	theme->set_color_role("text_icon_normal_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_pressed_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_hover_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_focus_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("text_icon_disabled_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color_role("selected_text_icon_normal_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("selected_text_icon_pressed_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("selected_text_icon_hover_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("selected_text_icon_hover_pressed_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("selected_text_icon_focus_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SECONDARY_CONTAINER));
	// 	theme->set_color_role("selected_text_icon_disabled_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color_role("unselected_text_icon_normal_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_pressed_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_hover_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_hover_pressed_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_focus_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_disabled_color_role", "FilledTonalIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("text_icon_normal_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "FilledTonalIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("selected_text_icon_normal_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_pressed_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_hover_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_hover_pressed_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_focus_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_disabled_color", "FilledTonalIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("unselected_text_icon_normal_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_pressed_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_hover_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_hover_pressed_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_focus_color", "FilledTonalIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_disabled_color", "FilledTonalIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("icon_max_width", "FilledTonalIconButton", 0);
	// }

	// // OutlinedIconButton
	// {
	// 	theme->set_type_variation("OutlinedIconButton", "TextureButton");

	// 	Ref<StyleBoxFlat> outlined_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	outlined_icon_button_normal->set_expand_margin_all(Math::round(2 * scale));
	// 	outlined_icon_button_normal->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> outlined_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	outlined_icon_button_hover->set_expand_margin_all(Math::round(2 * scale));
	// 	outlined_icon_button_hover->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> outlined_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	outlined_icon_button_pressed->set_expand_margin_all(Math::round(2 * scale));
	// 	outlined_icon_button_pressed->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> outlined_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	outlined_icon_button_disabled->set_expand_margin_all(Math::round(2 * scale));
	// 	outlined_icon_button_disabled->set_border_color_role(make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	outlined_icon_button_disabled->set_border_color_scale(Color(1, 1, 1, 0.12));

	// 	Ref<StyleBoxFlat> outlined_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	outlined_icon_focus->set_expand_margin_all(Math::round(2 * scale));
	// 	outlined_icon_focus->set_border_color_role(make_color_role(ColorRoleEnum::SECONDARY));

	// 	const Ref<StyleBoxFlat> outlined_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> outlined_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> outlined_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_outlined_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_SURFACE), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 2);
	// 	selected_outlined_icon_button_normal->set_expand_margin_all(Math::round(2 * scale));
	// 	selected_outlined_icon_button_normal->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> selected_outlined_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_SURFACE), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 2);
	// 	selected_outlined_icon_button_hover->set_expand_margin_all(Math::round(2 * scale));
	// 	selected_outlined_icon_button_hover->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> selected_outlined_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_SURFACE), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 2);
	// 	selected_outlined_icon_button_pressed->set_expand_margin_all(Math::round(2 * scale));
	// 	selected_outlined_icon_button_pressed->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> selected_outlined_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 2);
	// 	selected_outlined_icon_button_disabled->set_expand_margin_all(Math::round(2 * scale));
	// 	selected_outlined_icon_button_disabled->set_border_color_role(make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	selected_outlined_icon_button_disabled->set_border_color_scale(Color(1, 1, 1, 0.12));

	// 	Ref<StyleBoxFlat> selected_outlined_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	selected_outlined_icon_focus->set_expand_margin_all(Math::round(2 * scale));
	// 	selected_outlined_icon_focus->set_border_color_role(make_color_role(ColorRoleEnum::SECONDARY));

	// 	const Ref<StyleBoxFlat> selected_outlined_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> selected_outlined_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_outlined_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_outlined_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	selected_outlined_icon_button_normal->set_expand_margin_all(Math::round(2 * scale));
	// 	selected_outlined_icon_button_normal->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> unselected_outlined_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	unselected_outlined_icon_button_hover->set_expand_margin_all(Math::round(2 * scale));
	// 	unselected_outlined_icon_button_hover->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> unselected_outlined_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	unselected_outlined_icon_button_pressed->set_expand_margin_all(Math::round(2 * scale));
	// 	unselected_outlined_icon_button_pressed->set_border_color_role(make_color_role(ColorRoleEnum::OUTLINE));

	// 	Ref<StyleBoxFlat> unselected_outlined_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	unselected_outlined_icon_button_disabled->set_expand_margin_all(Math::round(2 * scale));
	// 	unselected_outlined_icon_button_disabled->set_border_color_role(make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	unselected_outlined_icon_button_disabled->set_border_color_scale(Color(1, 1, 1, 0.12));

	// 	Ref<StyleBoxFlat> unselected_outlined_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	unselected_outlined_icon_focus->set_expand_margin_all(Math::round(2 * scale));
	// 	unselected_outlined_icon_focus->set_border_color_role(make_color_role(ColorRoleEnum::SECONDARY));

	// 	const Ref<StyleBoxFlat> unselected_outlined_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> unselected_outlined_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_outlined_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	theme->set_stylebox("normal", "OutlinedIconButton", outlined_icon_button_normal);
	// 	theme->set_stylebox("hover", "OutlinedIconButton", outlined_icon_button_hover);
	// 	theme->set_stylebox("pressed", "OutlinedIconButton", outlined_icon_button_pressed);
	// 	theme->set_stylebox("disabled", "OutlinedIconButton", outlined_icon_button_disabled);
	// 	theme->set_stylebox("focus", "OutlinedIconButton", outlined_icon_focus);

	// 	theme->set_stylebox("hover_state_layer", "OutlinedIconButton", outlined_icon_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "OutlinedIconButton", outlined_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "OutlinedIconButton", outlined_icon_focus_state_layer);

	// 	theme->set_stylebox("selected_normal", "OutlinedIconButton", selected_outlined_icon_button_normal);
	// 	theme->set_stylebox("selected_hover", "OutlinedIconButton", selected_outlined_icon_button_hover);
	// 	theme->set_stylebox("selected_pressed", "OutlinedIconButton", selected_outlined_icon_button_pressed);
	// 	theme->set_stylebox("selected_disabled", "OutlinedIconButton", selected_outlined_icon_button_disabled);
	// 	theme->set_stylebox("selected_focus", "OutlinedIconButton", selected_outlined_icon_focus);

	// 	theme->set_stylebox("selected_hover_state_layer", "OutlinedIconButton", selected_outlined_icon_button_hover_state_layer);
	// 	theme->set_stylebox("selected_pressed_state_layer", "OutlinedIconButton", selected_outlined_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("selected_focus_state_layer", "OutlinedIconButton", selected_outlined_icon_focus_state_layer);

	// 	theme->set_stylebox("unselected_normal", "OutlinedIconButton", unselected_outlined_icon_button_normal);
	// 	theme->set_stylebox("unselected_hover", "OutlinedIconButton", unselected_outlined_icon_button_hover);
	// 	theme->set_stylebox("unselected_pressed", "OutlinedIconButton", unselected_outlined_icon_button_pressed);
	// 	theme->set_stylebox("unselected_disabled", "OutlinedIconButton", unselected_outlined_icon_button_disabled);
	// 	theme->set_stylebox("unselected_focus", "OutlinedIconButton", unselected_outlined_icon_focus);

	// 	theme->set_stylebox("unselected_hover_state_layer", "OutlinedIconButton", unselected_outlined_icon_button_hover_state_layer);
	// 	theme->set_stylebox("unselected_pressed_state_layer", "OutlinedIconButton", unselected_outlined_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("unselected_focus_state_layer", "OutlinedIconButton", unselected_outlined_icon_focus_state_layer);

	// 	theme->set_font("text_icon_font", "OutlinedIconButton", default_icon_font);
	// 	theme->set_font_size("text_icon_font_size", "OutlinedIconButton", 24);

	// 	theme->set_color_scheme("default_color_scheme", "OutlinedIconButton", default_color_scheme);

	// 	theme->set_color("text_icon_normal_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "OutlinedIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("selected_text_icon_normal_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_focus_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_pressed_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_hover_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_hover_pressed_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_disabled_color_scale", "OutlinedIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("unselected_text_icon_normal_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_focus_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_pressed_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_hover_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_hover_pressed_color_scale", "OutlinedIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_disabled_color_scale", "OutlinedIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("selected_text_icon_normal_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_pressed_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_hover_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_focus_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_hover_pressed_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_disabled_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("unselected_text_icon_normal_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_pressed_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_hover_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_focus_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_hover_pressed_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_disabled_color_scheme", "OutlinedIconButton", Ref<ColorScheme>());

	// 	theme->set_color_role("text_icon_normal_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE));
	// 	theme->set_color_role("text_icon_pressed_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE));
	// 	theme->set_color_role("text_icon_hover_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE));
	// 	theme->set_color_role("text_icon_focus_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE));
	// 	theme->set_color_role("text_icon_disabled_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color_role("selected_text_icon_normal_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE));
	// 	theme->set_color_role("selected_text_icon_pressed_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE));
	// 	theme->set_color_role("selected_text_icon_hover_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE));
	// 	theme->set_color_role("selected_text_icon_hover_pressed_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE));
	// 	theme->set_color_role("selected_text_icon_focus_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::INVERSE_ON_SURFACE));
	// 	theme->set_color_role("selected_text_icon_disabled_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color_role("unselected_text_icon_normal_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_pressed_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	theme->set_color_role("unselected_text_icon_hover_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_hover_pressed_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));
	// 	theme->set_color_role("unselected_text_icon_focus_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_disabled_color_role", "OutlinedIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("text_icon_normal_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "OutlinedIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("selected_text_icon_normal_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_pressed_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_hover_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_hover_pressed_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_focus_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_disabled_color", "OutlinedIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("unselected_text_icon_normal_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_pressed_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_hover_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_hover_pressed_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_focus_color", "OutlinedIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_disabled_color", "OutlinedIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("icon_max_width", "OutlinedIconButton", 0);
	// }

	// // StandardIconButton
	// {
	// 	theme->set_type_variation("StandardIconButton", "TextureButton");

	// 	Ref<StyleBoxFlat> standard_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 0);

	// 	Ref<StyleBoxFlat> standard_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 0);

	// 	Ref<StyleBoxFlat> standard_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 0);

	// 	Ref<StyleBoxFlat> standard_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 0);

	// 	Ref<StyleBoxFlat> standard_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	standard_icon_focus->set_expand_margin_all(Math::round(2 * scale));
	// 	standard_icon_focus->set_border_color_role(make_color_role(ColorRoleEnum::SECONDARY));

	// 	const Ref<StyleBoxFlat> standard_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> standard_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> standard_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_standard_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_standard_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_standard_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_standard_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_standard_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	selected_standard_icon_focus->set_expand_margin_all(Math::round(2 * scale));
	// 	selected_standard_icon_focus->set_border_color_role(make_color_role(ColorRoleEnum::SECONDARY));

	// 	const Ref<StyleBoxFlat> selected_standard_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> selected_standard_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> selected_standard_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_standard_icon_button_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 0);

	// 	Ref<StyleBoxFlat> unselected_standard_icon_button_hover = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 0);

	// 	Ref<StyleBoxFlat> unselected_standard_icon_button_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 0);

	// 	Ref<StyleBoxFlat> unselected_standard_icon_button_disabled = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE), style_zero_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 0);

	// 	Ref<StyleBoxFlat> unselected_standard_icon_focus = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY), style_one_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, false, 2);
	// 	unselected_standard_icon_focus->set_expand_margin_all(Math::round(2 * scale));
	// 	unselected_standard_icon_focus->set_border_color_role(make_color_role(ColorRoleEnum::SECONDARY));

	// 	const Ref<StyleBoxFlat> unselected_standard_icon_button_hover_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT), hover_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	const Ref<StyleBoxFlat> unselected_standard_icon_button_pressed_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT), pressed_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	Ref<StyleBoxFlat> unselected_standard_icon_focus_state_layer = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT), focus_state_layer_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, default_margin, default_margin, default_margin, default_margin, 1000, true, 0);

	// 	theme->set_stylebox("normal", "StandardIconButton", standard_icon_button_normal);
	// 	theme->set_stylebox("hover", "StandardIconButton", standard_icon_button_hover);
	// 	theme->set_stylebox("pressed", "StandardIconButton", standard_icon_button_pressed);
	// 	theme->set_stylebox("disabled", "StandardIconButton", standard_icon_button_disabled);
	// 	theme->set_stylebox("focus", "StandardIconButton", standard_icon_focus);

	// 	theme->set_stylebox("hover_state_layer", "StandardIconButton", standard_icon_button_hover_state_layer);
	// 	theme->set_stylebox("pressed_state_layer", "StandardIconButton", standard_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("focus_state_layer", "StandardIconButton", standard_icon_focus_state_layer);

	// 	theme->set_stylebox("selected_normal", "StandardIconButton", selected_standard_icon_button_normal);
	// 	theme->set_stylebox("selected_hover", "StandardIconButton", selected_standard_icon_button_hover);
	// 	theme->set_stylebox("selected_pressed", "StandardIconButton", selected_standard_icon_button_pressed);
	// 	theme->set_stylebox("selected_disabled", "StandardIconButton", selected_standard_icon_button_disabled);
	// 	theme->set_stylebox("selected_focus", "StandardIconButton", selected_standard_icon_focus);

	// 	theme->set_stylebox("selected_hover_state_layer", "StandardIconButton", selected_standard_icon_button_hover_state_layer);
	// 	theme->set_stylebox("selected_pressed_state_layer", "StandardIconButton", selected_standard_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("selected_focus_state_layer", "StandardIconButton", selected_standard_icon_focus_state_layer);

	// 	theme->set_stylebox("unselected_normal", "StandardIconButton", unselected_standard_icon_button_normal);
	// 	theme->set_stylebox("unselected_hover", "StandardIconButton", unselected_standard_icon_button_hover);
	// 	theme->set_stylebox("unselected_pressed", "StandardIconButton", unselected_standard_icon_button_pressed);
	// 	theme->set_stylebox("unselected_disabled", "StandardIconButton", unselected_standard_icon_button_disabled);
	// 	theme->set_stylebox("unselected_focus", "StandardIconButton", unselected_standard_icon_focus);

	// 	theme->set_stylebox("unselected_hover_state_layer", "StandardIconButton", unselected_standard_icon_button_hover_state_layer);
	// 	theme->set_stylebox("unselected_pressed_state_layer", "StandardIconButton", unselected_standard_icon_button_pressed_state_layer);
	// 	theme->set_stylebox("unselected_focus_state_layer", "StandardIconButton", unselected_standard_icon_focus_state_layer);

	// 	theme->set_font("text_icon_font", "StandardIconButton", default_icon_font);
	// 	theme->set_font_size("text_icon_font_size", "StandardIconButton", 24);

	// 	theme->set_color_scheme("default_color_scheme", "StandardIconButton", default_color_scheme);

	// 	theme->set_color("text_icon_normal_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("text_icon_focus_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("text_icon_pressed_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("text_icon_hover_pressed_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("text_icon_disabled_color_scale", "StandardIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("selected_text_icon_normal_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_focus_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_pressed_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_hover_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_hover_pressed_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("selected_text_icon_disabled_color_scale", "StandardIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color("unselected_text_icon_normal_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_focus_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_pressed_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_hover_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_hover_pressed_color_scale", "StandardIconButton", one_color_scale);
	// 	theme->set_color("unselected_text_icon_disabled_color_scale", "StandardIconButton", Color(one_color_scale, 0.38));

	// 	theme->set_color_scheme("text_icon_normal_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_pressed_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_focus_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_hover_pressed_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("text_icon_disabled_color_scheme", "StandardIconButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("selected_text_icon_normal_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_pressed_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_hover_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_focus_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_hover_pressed_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("selected_text_icon_disabled_color_scheme", "StandardIconButton", Ref<ColorScheme>());

	// 	theme->set_color_scheme("unselected_text_icon_normal_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_pressed_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_hover_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_focus_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_hover_pressed_color_scheme", "StandardIconButton", Ref<ColorScheme>());
	// 	theme->set_color_scheme("unselected_text_icon_disabled_color_scheme", "StandardIconButton", Ref<ColorScheme>());

	// 	theme->set_color_role("text_icon_normal_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_pressed_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_hover_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_hover_pressed_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_focus_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("text_icon_disabled_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color_role("selected_text_icon_normal_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("selected_text_icon_pressed_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("selected_text_icon_hover_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("selected_text_icon_hover_pressed_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("selected_text_icon_focus_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::PRIMARY));
	// 	theme->set_color_role("selected_text_icon_disabled_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color_role("unselected_text_icon_normal_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_pressed_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_hover_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_hover_pressed_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_focus_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::ON_SURFACE_VARIANT));
	// 	theme->set_color_role("unselected_text_icon_disabled_color_role", "StandardIconButton", make_color_role(ColorRoleEnum::ON_SURFACE));

	// 	theme->set_color("text_icon_normal_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_pressed_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_hover_pressed_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_focus_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("text_icon_disabled_color", "StandardIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("selected_text_icon_normal_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_pressed_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_hover_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_hover_pressed_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_focus_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("selected_text_icon_disabled_color", "StandardIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_color("unselected_text_icon_normal_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_pressed_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_hover_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_hover_pressed_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_focus_color", "StandardIconButton", Color(1, 1, 1, 1));
	// 	theme->set_color("unselected_text_icon_disabled_color", "StandardIconButton", Color(1, 1, 1, 0.4));

	// 	theme->set_constant("icon_max_width", "StandardIconButton", 0);
	// }

	// // ProgressBar

	// theme->set_stylebox("background", "ProgressBar", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), Color(1, 1, 1, 1), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 2, 2, 2, 2, 6));
	// theme->set_stylebox("fill", "ProgressBar", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), Color(1, 1, 1, 1), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 2, 2, 2, 2, 6));

	// theme->set_font("font", "ProgressBar", Ref<Font>());
	// theme->set_font_size("font_size", "ProgressBar", -1);

	// theme->set_color_scheme("default_color_scheme", "ProgressBar", default_color_scheme);

	// theme->set_color("font_color_scale", "ProgressBar", control_font_hover_color_scale);
	// theme->set_color("font_outline_color_scale", "ProgressBar", Color(1, 1, 1));

	// theme->set_color_scheme("font_color_scheme", "ProgressBar", Ref<ColorScheme>());
	// theme->set_color_scheme("font_outline_color_scheme", "ProgressBar", Ref<ColorScheme>());

	// theme->set_color_role("font_color_role", "ProgressBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_outline_color_role", "ProgressBar", make_color_role(ColorRoleEnum::OUTLINE));

	// theme->set_color("font_color", "ProgressBar", control_font_hover_color);
	// theme->set_color("font_outline_color", "ProgressBar", Color(1, 1, 1));

	// theme->set_constant("outline_size", "ProgressBar", 0);

	// // CodeEdit

	// theme->set_stylebox("normal", "CodeEdit", style_line_edit);
	// theme->set_stylebox("focus", "CodeEdit", focus);
	// theme->set_stylebox("read_only", "CodeEdit", style_line_edit_read_only);
	// theme->set_stylebox("completion", "CodeEdit", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_normal_color, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0));

	// theme->set_color_scheme("default_color_scheme", "CodeEdit", default_color_scheme);

	// theme->set_icon("tab", "CodeEdit", icons["text_edit_tab"]);
	// theme->set_icon("space", "CodeEdit", icons["text_edit_space"]);
	// theme->set_icon("breakpoint", "CodeEdit", icons["breakpoint"]);
	// theme->set_icon("bookmark", "CodeEdit", icons["bookmark"]);
	// theme->set_icon("executing_line", "CodeEdit", icons["arrow_right"]);
	// theme->set_icon("can_fold", "CodeEdit", icons["arrow_down"]);
	// theme->set_icon("folded", "CodeEdit", icons["arrow_right"]);
	// theme->set_icon("can_fold_code_region", "CodeEdit", icons["region_unfolded"]);
	// theme->set_icon("folded_code_region", "CodeEdit", icons["region_folded"]);
	// theme->set_icon("folded_eol_icon", "CodeEdit", icons["text_edit_ellipsis"]);

	// theme->set_font("font", "CodeEdit", Ref<Font>());
	// theme->set_font_size("font_size", "CodeEdit", -1);

	// theme->set_color_scheme("background_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("completion_background_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("completion_selected_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("completion_existing_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("completion_scroll_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("completion_scroll_hovered_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("font_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("font_selected_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("font_readonly_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("font_placeholder_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("font_outline_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("selection_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("bookmark_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("breakpoint_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("executing_line_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("current_line_color_scheme", "CodeEdit", Ref<ColorScheme>());

	// theme->set_color_scheme("code_folding_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("folded_code_region_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("caret_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("caret_background_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("brace_mismatch_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("line_number_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("word_highlighted_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("line_length_guideline_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("search_result_color_scheme", "CodeEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("search_result_border_color_scheme", "CodeEdit", Ref<ColorScheme>());

	// theme->set_color_role("background_color_role", "CodeEdit", make_color_role(ColorRoleEnum::PRIMARY_CONTAINER));
	// theme->set_color_role("font_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("font_selected_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("font_readonly_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("font_placeholder_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("font_outline_color_role", "CodeEdit", make_color_role(ColorRoleEnum::OUTLINE));
	// theme->set_color_role("selection_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("current_line_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("caret_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("caret_background_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("word_highlighted_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("search_result_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("search_result_border_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));

	// theme->set_color_role("completion_background_color_role", "CodeEdit", make_color_role(ColorRoleEnum::PRIMARY_CONTAINER));
	// theme->set_color_role("completion_selected_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("completion_existing_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("completion_scroll_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("completion_scroll_hovered_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("bookmark_color_role", "CodeEdit", make_color_role(ColorRoleEnum::SECONDARY));
	// theme->set_color_role("breakpoint_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ERROR));
	// theme->set_color_role("executing_line_color_role", "CodeEdit", make_color_role(ColorRoleEnum::SECONDARY));
	// theme->set_color_role("code_folding_color_role", "CodeEdit", make_color_role(ColorRoleEnum::SECONDARY));
	// theme->set_color_role("folded_code_region_color_role", "CodeEdit", make_color_role(ColorRoleEnum::SECONDARY));
	// theme->set_color_role("brace_mismatch_color_role", "CodeEdit", make_color_role(ColorRoleEnum::SECONDARY));
	// theme->set_color_role("line_number_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_SECONDARY));
	// theme->set_color_role("line_length_guideline_color_role", "CodeEdit", make_color_role(ColorRoleEnum::ON_SECONDARY));

	// theme->set_color("background_color_scale", "CodeEdit", Color(1, 1, 1, 1));
	// theme->set_color("font_color_scale", "CodeEdit", control_font_color_scale);
	// theme->set_color("font_selected_color_scale", "CodeEdit", control_font_pressed_color_scale);
	// theme->set_color("font_readonly_color_scale", "CodeEdit", control_font_disabled_color_scale);
	// theme->set_color("font_placeholder_color_scale", "CodeEdit", control_font_placeholder_color_scale);
	// theme->set_color("font_outline_color_scale", "CodeEdit", Color(1, 1, 1));
	// theme->set_color("selection_color_scale", "CodeEdit", control_selection_color_scale);
	// theme->set_color("current_line_color_scale", "CodeEdit", Color(0.5, 0.5, 0.5, 0.8));
	// theme->set_color("caret_color_scale", "CodeEdit", control_font_hover_color_scale);
	// theme->set_color("caret_background_color_scale", "CodeEdit", Color(1, 1, 1));
	// theme->set_color("word_highlighted_color_scale", "CodeEdit", control_font_focus_color_scale);
	// theme->set_color("search_result_color_scale", "CodeEdit", Color(0.4, 0.4, 0.4));
	// theme->set_color("search_result_border_color_scale", "CodeEdit", Color(0.4, 0.4, 0.4, 0.5));

	// theme->set_color("completion_background_color_scale", "CodeEdit", Color(0.5, 0.5, 0.5));
	// theme->set_color("completion_selected_color_scale", "CodeEdit", Color(0.5, 0.5, 0.5));
	// theme->set_color("completion_existing_color_scale", "CodeEdit", Color(1.2, 1.2, 1.2, 0.13));
	// theme->set_color("completion_scroll_color_scale", "CodeEdit", Color(1, 1, 1, 0.29));
	// theme->set_color("completion_scroll_hovered_color_scale", "CodeEdit", Color(1, 1, 1, 0.4));
	// theme->set_color("bookmark_color_scale", "CodeEdit", Color(1, 1, 1, 0.8));
	// theme->set_color("breakpoint_color_scale", "CodeEdit", Color(1, 1, 1));
	// theme->set_color("executing_line_color_scale", "CodeEdit", Color(1, 1, 1));
	// theme->set_color("code_folding_color_scale", "CodeEdit", Color(1, 1, 1, 0.8));
	// theme->set_color("folded_code_region_color_scale", "CodeEdit", Color(1, 1, 1, 0.2));
	// theme->set_color("brace_mismatch_color_scale", "CodeEdit", Color(1, 1, 1));
	// theme->set_color("line_number_color_scale", "CodeEdit", Color(1, 1, 1, 0.4));
	// theme->set_color("line_length_guideline_color_scale", "CodeEdit", Color(1, 1, 1, 0.1));

	// theme->set_color("background_color", "CodeEdit", Color(0, 0, 0, 0));
	// theme->set_color("completion_background_color", "CodeEdit", Color(0.17, 0.16, 0.2));
	// theme->set_color("completion_selected_color", "CodeEdit", Color(0.26, 0.26, 0.27));
	// theme->set_color("completion_existing_color", "CodeEdit", Color(0.87, 0.87, 0.87, 0.13));
	// theme->set_color("completion_scroll_color", "CodeEdit", control_font_pressed_color * Color(1, 1, 1, 0.29));
	// theme->set_color("completion_scroll_hovered_color", "CodeEdit", control_font_pressed_color * Color(1, 1, 1, 0.4));
	// theme->set_color("font_color", "CodeEdit", control_font_color);
	// theme->set_color("font_selected_color", "CodeEdit", Color(0, 0, 0, 0));
	// theme->set_color("font_readonly_color", "CodeEdit", Color(control_font_color.r, control_font_color.g, control_font_color.b, 0.5f));
	// theme->set_color("font_placeholder_color", "CodeEdit", control_font_placeholder_color);
	// theme->set_color("font_outline_color", "CodeEdit", Color(1, 1, 1));
	// theme->set_color("selection_color", "CodeEdit", control_selection_color);
	// theme->set_color("bookmark_color", "CodeEdit", Color(0.5, 0.64, 1, 0.8));
	// theme->set_color("breakpoint_color", "CodeEdit", Color(0.9, 0.29, 0.3));
	// theme->set_color("executing_line_color", "CodeEdit", Color(0.98, 0.89, 0.27));
	// theme->set_color("current_line_color", "CodeEdit", Color(0.25, 0.25, 0.26, 0.8));
	// theme->set_color("code_folding_color", "CodeEdit", Color(0.8, 0.8, 0.8, 0.8));
	// theme->set_color("folded_code_region_color", "CodeEdit", Color(0.68, 0.46, 0.77, 0.2));
	// theme->set_color("caret_color", "CodeEdit", control_font_color);
	// theme->set_color("caret_background_color", "CodeEdit", Color(0, 0, 0));
	// theme->set_color("brace_mismatch_color", "CodeEdit", Color(1, 0.2, 0.2));
	// theme->set_color("line_number_color", "CodeEdit", Color(0.67, 0.67, 0.67, 0.4));
	// theme->set_color("word_highlighted_color", "CodeEdit", Color(0.8, 0.9, 0.9, 0.15));
	// theme->set_color("line_length_guideline_color", "CodeEdit", Color(0.3, 0.5, 0.8, 0.1));
	// theme->set_color("search_result_color", "CodeEdit", Color(0.3, 0.3, 0.3));
	// theme->set_color("search_result_border_color", "CodeEdit", Color(0.3, 0.3, 0.3, 0.4));

	// theme->set_constant("completion_lines", "CodeEdit", 7);
	// theme->set_constant("completion_max_width", "CodeEdit", 50);
	// theme->set_constant("completion_scroll_width", "CodeEdit", 6);
	// theme->set_constant("line_spacing", "CodeEdit", Math::round(4 * scale));
	// theme->set_constant("outline_size", "CodeEdit", 0);

	// Ref<Texture2D> empty_icon = memnew(ImageTexture);

	// const Ref<StyleBoxFlat> style_h_scrollbar = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 4, 0, 4, 10);
	// const Ref<StyleBoxFlat> style_v_scrollbar = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 0, 4, 0, 10);
	// Ref<StyleBoxFlat> style_scrollbar_grabber = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_progress_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 10);
	// Ref<StyleBoxFlat> style_scrollbar_grabber_highlight = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_focus_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 10);
	// Ref<StyleBoxFlat> style_scrollbar_grabber_pressed = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_focus_color_scale * Color(0.75, 0.75, 0.75, 1), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 10);

	// const Ref<StyleBoxFlat> style_slider = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 4);
	// const Ref<StyleBoxFlat> style_slider_grabber = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_progress_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 4);
	// const Ref<StyleBoxFlat> style_slider_grabber_highlight = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::INVERSE_PRIMARY), style_focus_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 4);

	// // HSlider

	// theme->set_stylebox("slider", "HSlider", style_slider);
	// theme->set_stylebox("grabber_area", "HSlider", style_slider_grabber);
	// theme->set_stylebox("grabber_area_highlight", "HSlider", style_slider_grabber_highlight);

	// theme->set_icon("grabber", "HSlider", icons["slider_grabber"]);
	// theme->set_icon("grabber_highlight", "HSlider", icons["slider_grabber_hl"]);
	// theme->set_icon("grabber_disabled", "HSlider", icons["slider_grabber_disabled"]);
	// theme->set_icon("tick", "HSlider", icons["hslider_tick"]);

	// theme->set_constant("center_grabber", "HSlider", 0);
	// theme->set_constant("grabber_offset", "HSlider", 0);

	// // VSlider

	// theme->set_stylebox("slider", "VSlider", style_slider);
	// theme->set_stylebox("grabber_area", "VSlider", style_slider_grabber);
	// theme->set_stylebox("grabber_area_highlight", "VSlider", style_slider_grabber_highlight);

	// theme->set_icon("grabber", "VSlider", icons["slider_grabber"]);
	// theme->set_icon("grabber_highlight", "VSlider", icons["slider_grabber_hl"]);
	// theme->set_icon("grabber_disabled", "VSlider", icons["slider_grabber_disabled"]);
	// theme->set_icon("tick", "VSlider", icons["vslider_tick"]);

	// theme->set_constant("center_grabber", "VSlider", 0);
	// theme->set_constant("grabber_offset", "VSlider", 0);

	// // SpinBox

	// theme->set_icon("updown", "SpinBox", icons["updown"]);

	// // Window

	// theme->set_stylebox("embedded_border", "Window", sb_expand(make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_popup_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 10, 28, 10, 8), 8, 32, 8, 6));
	// theme->set_stylebox("embedded_unfocused_border", "Window", sb_expand(make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_popup_hover_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 10, 28, 10, 8), 8, 32, 8, 6));

	// theme->set_color_scheme("default_color_scheme", "Window", default_color_scheme);

	// theme->set_color("title_color_scale", "Window", control_font_color_scale);
	// theme->set_color("title_outline_modulate_scale", "Window", Color(1, 1, 1));

	// theme->set_color_scheme("title_color_scheme", "Window", Ref<ColorScheme>());
	// theme->set_color_scheme("title_outline_modulate_scheme", "Window", Ref<ColorScheme>());

	// theme->set_color_role("title_color_role", "Window", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("title_outline_modulate_role", "Window", make_color_role(ColorRoleEnum::OUTLINE));

	// theme->set_font("title_font", "Window", Ref<Font>());
	// theme->set_font_size("title_font_size", "Window", -1);
	// theme->set_color("title_color", "Window", control_font_color);
	// theme->set_color("title_outline_modulate", "Window", Color(1, 1, 1));
	// theme->set_constant("title_outline_size", "Window", 0);
	// theme->set_constant("title_height", "Window", 36 * scale);
	// theme->set_constant("resize_margin", "Window", Math::round(4 * scale));

	// theme->set_icon("close", "Window", icons["close"]);
	// theme->set_icon("close_pressed", "Window", icons["close_hl"]);
	// theme->set_constant("close_h_offset", "Window", 18 * scale);
	// theme->set_constant("close_v_offset", "Window", 24 * scale);

	// // Dialogs

	// // AcceptDialog is currently the base dialog, so this defines styles for all extending nodes.
	// theme->set_stylebox("panel", "AcceptDialog", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_popup_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, Math::round(8 * scale), Math::round(8 * scale), Math::round(8 * scale), Math::round(8 * scale)));
	// theme->set_constant("buttons_separation", "AcceptDialog", Math::round(10 * scale));

	// // File Dialog

	// theme->set_icon("parent_folder", "FileDialog", icons["folder_up"]);
	// theme->set_icon("back_folder", "FileDialog", icons["arrow_left"]);
	// theme->set_icon("forward_folder", "FileDialog", icons["arrow_right"]);
	// theme->set_icon("reload", "FileDialog", icons["reload"]);
	// theme->set_icon("toggle_hidden", "FileDialog", icons["visibility_visible"]);
	// theme->set_icon("folder", "FileDialog", icons["folder"]);
	// theme->set_icon("file", "FileDialog", icons["file"]);

	// theme->set_color_scheme("default_color_scheme", "FileDialog", default_color_scheme);

	// theme->set_color_scheme("folder_icon_color_scheme", "FileDialog", Ref<ColorScheme>());
	// theme->set_color_scheme("file_icon_color_scheme", "FileDialog", Ref<ColorScheme>());
	// theme->set_color_scheme("file_disabled_color_scheme", "FileDialog", Ref<ColorScheme>());
	// theme->set_color_scheme("icon_normal_color_scheme", "FileDialog", Ref<ColorScheme>());
	// theme->set_color_scheme("icon_hover_color_scheme", "FileDialog", Ref<ColorScheme>());
	// theme->set_color_scheme("icon_focus_color_scheme", "FileDialog", Ref<ColorScheme>());
	// theme->set_color_scheme("icon_pressed_color_scheme", "FileDialog", Ref<ColorScheme>());

	// theme->set_color_role("folder_icon_color_role", "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("file_icon_color_role", "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("file_disabled_color_role", "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("icon_normal_color_role", "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("icon_hover_color_role", "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("icon_focus_color_role", "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("icon_pressed_color_role", "FileDialog", make_color_role(ColorRoleEnum::ON_PRIMARY));

	// theme->set_color("folder_icon_color", "FileDialog", Color(1, 1, 1));
	// theme->set_color("file_icon_color", "FileDialog", Color(1, 1, 1));
	// theme->set_color("file_disabled_color", "FileDialog", Color(1, 1, 1, 0.25));
	// theme->set_color("icon_normal_color", "FileDialog", Color(1, 1, 1, 0.25));
	// theme->set_color("icon_hover_color", "FileDialog", Color(1, 1, 1, 0.25));
	// theme->set_color("icon_focus_color", "FileDialog", Color(1, 1, 1, 0.25));
	// theme->set_color("icon_pressed_color", "FileDialog", Color(1, 1, 1, 0.25));

	// theme->set_color("folder_icon_color_scale", "FileDialog", Color(1, 1, 1));
	// theme->set_color("file_icon_color_scale", "FileDialog", Color(1, 1, 1));
	// theme->set_color("file_disabled_color_scale", "FileDialog", Color(1, 1, 1, 0.25));
	// theme->set_color("icon_normal_color_scale", "FileDialog", Color(1, 1, 1, 0.25));
	// theme->set_color("icon_hover_color_scale", "FileDialog", Color(1, 1, 1, 0.25));
	// theme->set_color("icon_focus_color_scale", "FileDialog", Color(1, 1, 1, 0.25));
	// theme->set_color("icon_pressed_color_scale", "FileDialog", Color(1, 1, 1, 0.25));

	// // Popup

	// theme->set_stylebox("panel", "PopupPanel", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_normal_color_scale));

	// // PopupDialog

	// theme->set_stylebox("panel", "PopupDialog", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_normal_color_scale));

	// // GraphNode

	// Ref<StyleBoxFlat> graphnode_normal = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 18, 12, 18, 12);
	// graphnode_normal->set_border_color(Color(0.325, 0.325, 0.325, 0.6));
	// graphnode_normal->set_border_color_scale(Color(0.325, 0.325, 0.325, 0.6));
	// Ref<StyleBoxFlat> graphnode_selected = graphnode_normal->duplicate();
	// graphnode_selected->set_border_color(Color(0.625, 0.625, 0.625, 0.6));
	// graphnode_selected->set_border_color_scale(Color(0.625, 0.625, 0.625, 0.6));

	// Ref<StyleBoxFlat> graphn_sb_titlebar = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_normal_color_scale.lightened(0.3), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4);
	// Ref<StyleBoxFlat> graphn_sb_titlebar_selected = graphnode_normal->duplicate();
	// graphn_sb_titlebar_selected->set_bg_color(Color(1.0, 0.625, 0.625, 0.6));
	// graphn_sb_titlebar_selected->set_bg_color_scale(Color(1.0, 0.625, 0.625, 0.6));
	// Ref<StyleBoxEmpty> graphnode_slot = make_empty_stylebox(0, 0, 0, 0);

	// theme->set_stylebox("panel", "GraphNode", graphnode_normal);
	// theme->set_stylebox("panel_selected", "GraphNode", graphnode_selected);
	// theme->set_stylebox("titlebar", "GraphNode", graphn_sb_titlebar);
	// theme->set_stylebox("titlebar_selected", "GraphNode", graphn_sb_titlebar_selected);
	// theme->set_stylebox("slot", "GraphNode", graphnode_slot);
	// theme->set_icon("port", "GraphNode", icons["graph_port"]);
	// theme->set_icon("resizer", "GraphNode", icons["resizer_se"]);

	// theme->set_color_scheme("default_color_scheme", "GraphNode", default_color_scheme);
	// theme->set_color_scheme("resizer_color_scheme", "GraphNode", Ref<ColorScheme>());
	// theme->set_color_role("resizer_color_role", "GraphNode", make_color_role(ColorRoleEnum::PRIMARY));
	// theme->set_color("resizer_color", "GraphNode", control_font_color);
	// theme->set_color("resizer_color_scale", "GraphNode", control_font_color_scale);
	// theme->set_constant("separation", "GraphNode", Math::round(2 * scale));
	// theme->set_constant("port_h_offset", "GraphNode", 0);

	// // GraphNodes's title Label.

	// theme->set_type_variation("GraphNodeTitleLabel", "Label");

	// theme->set_stylebox("normal", "GraphNodeTitleLabel", make_empty_stylebox(0, 0, 0, 0));
	// theme->set_font("font", "GraphNodeTitleLabel", Ref<Font>());
	// theme->set_font_size("font_size", "GraphNodeTitleLabel", -1);

	// theme->set_color_scheme("default_color_scheme", "GraphNodeTitleLabel", default_color_scheme);

	// theme->set_color_scheme("font_color_scheme", "GraphNodeTitleLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("font_shadow_color_scheme", "GraphNodeTitleLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("font_outline_color_scheme", "GraphNodeTitleLabel", Ref<ColorScheme>());

	// theme->set_color("font_color_scale", "GraphNodeTitleLabel", control_font_color_scale);
	// theme->set_color("font_shadow_color_scale", "GraphNodeTitleLabel", Color(1, 1, 1, 1));
	// theme->set_color("font_outline_color_scale", "GraphNodeTitleLabel", control_font_color_scale);

	// theme->set_color_role("font_color_role", "GraphNodeTitleLabel", make_color_role(ColorRoleEnum::PRIMARY));
	// theme->set_color_role("font_shadow_color_role", "GraphNodeTitleLabel", make_color_role(ColorRoleEnum::SHADOW));
	// theme->set_color_role("font_outline_color_role", "GraphNodeTitleLabel", make_color_role(ColorRoleEnum::PRIMARY));

	// theme->set_color("font_color", "GraphNodeTitleLabel", control_font_color);
	// theme->set_color("font_shadow_color", "GraphNodeTitleLabel", Color(0, 0, 0, 0));
	// theme->set_color("font_outline_color", "GraphNodeTitleLabel", control_font_color);

	// theme->set_constant("shadow_offset_x", "GraphNodeTitleLabel", Math::round(1 * scale));
	// theme->set_constant("shadow_offset_y", "GraphNodeTitleLabel", Math::round(1 * scale));
	// theme->set_constant("outline_size", "GraphNodeTitleLabel", 0);
	// theme->set_constant("shadow_outline_size", "GraphNodeTitleLabel", Math::round(1 * scale));
	// theme->set_constant("line_spacing", "GraphNodeTitleLabel", Math::round(3 * scale));

	// // Tree

	// theme->set_color_scheme("default_color_scheme", "Tree", default_color_scheme);

	// theme->set_stylebox("panel", "Tree", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 5));
	// theme->set_stylebox("focus", "Tree", focus);
	// theme->set_stylebox("selected", "Tree", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_selected_color_scale));
	// theme->set_stylebox("selected_focus", "Tree", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_selected_color_scale));
	// theme->set_stylebox("cursor", "Tree", focus);
	// theme->set_stylebox("cursor_unfocused", "Tree", focus);
	// theme->set_stylebox("button_pressed", "Tree", button_pressed);
	// theme->set_stylebox("title_button_normal", "Tree", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_pressed_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4));
	// theme->set_stylebox("title_button_pressed", "Tree", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_hover_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4));
	// theme->set_stylebox("title_button_hover", "Tree", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4));
	// theme->set_stylebox("custom_button", "Tree", button_normal);
	// theme->set_stylebox("custom_button_pressed", "Tree", button_pressed);
	// theme->set_stylebox("custom_button_hover", "Tree", button_hover);

	// theme->set_icon("checked", "Tree", icons["checked"]);
	// theme->set_icon("unchecked", "Tree", icons["unchecked"]);
	// theme->set_icon("indeterminate", "Tree", icons["indeterminate"]);
	// theme->set_icon("updown", "Tree", icons["updown"]);
	// theme->set_icon("select_arrow", "Tree", icons["option_button_arrow"]);
	// theme->set_icon("arrow", "Tree", icons["arrow_down"]);
	// theme->set_icon("arrow_collapsed", "Tree", icons["arrow_right"]);
	// theme->set_icon("arrow_collapsed_mirrored", "Tree", icons["arrow_left"]);

	// theme->set_font("title_button_font", "Tree", Ref<Font>());
	// theme->set_font("font", "Tree", Ref<Font>());
	// theme->set_font_size("font_size", "Tree", -1);
	// theme->set_font_size("title_button_font_size", "Tree", -1);

	// theme->set_color_scheme("title_button_color_scheme", "Tree", Ref<ColorScheme>());
	// theme->set_color_scheme("font_color_scheme", "Tree", Ref<ColorScheme>());
	// theme->set_color_scheme("font_selected_color_scheme", "Tree", Ref<ColorScheme>());
	// theme->set_color_scheme("guide_color_scheme", "Tree", Ref<ColorScheme>());
	// theme->set_color_scheme("drop_position_color_scheme", "Tree", Ref<ColorScheme>());
	// theme->set_color_scheme("relationship_line_color_scheme", "Tree", Ref<ColorScheme>());
	// theme->set_color_scheme("parent_hl_line_color_scheme", "Tree", Ref<ColorScheme>());
	// theme->set_color_scheme("children_hl_line_color_scheme", "Tree", Ref<ColorScheme>());
	// theme->set_color_scheme("custom_button_font_highlight_scheme", "Tree", Ref<ColorScheme>());
	// theme->set_color_scheme("font_outline_color_scheme", "Tree", Ref<ColorScheme>());

	// theme->set_color_role("title_button_color_role", "Tree", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_color_role", "Tree", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_selected_color_role", "Tree", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("guide_color_role", "Tree", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("drop_position_color_role", "Tree", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("relationship_line_color_role", "Tree", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("parent_hl_line_color_role", "Tree", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("children_hl_line_color_role", "Tree", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("custom_button_font_highlight_role", "Tree", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_outline_color_role", "Tree", make_color_role(ColorRoleEnum::OUTLINE));

	// theme->set_color("title_button_color_scale", "Tree", control_font_color_scale);
	// theme->set_color("font_color_scale", "Tree", control_font_low_color_scale);
	// theme->set_color("font_selected_color_scale", "Tree", control_font_pressed_color_scale);
	// theme->set_color("font_outline_color_scale", "Tree", Color(1, 1, 1));
	// theme->set_color("guide_color_scale", "Tree", Color(0.7, 0.7, 0.7, 0.25));
	// theme->set_color("drop_position_color_scale", "Tree", Color(1, 1, 1));
	// theme->set_color("relationship_line_color_scale", "Tree", Color(0.27, 0.27, 0.27));
	// theme->set_color("parent_hl_line_color_scale", "Tree", Color(0.27, 0.27, 0.27));
	// theme->set_color("children_hl_line_color_scale", "Tree", Color(0.27, 0.27, 0.27));
	// theme->set_color("custom_button_font_highlight_scale", "Tree", control_font_hover_color_scale);

	// theme->set_color("title_button_color", "Tree", control_font_color);
	// theme->set_color("font_color", "Tree", control_font_low_color);
	// theme->set_color("font_selected_color", "Tree", control_font_pressed_color);
	// theme->set_color("font_outline_color", "Tree", Color(1, 1, 1));
	// theme->set_color("guide_color", "Tree", Color(0.7, 0.7, 0.7, 0.25));
	// theme->set_color("drop_position_color", "Tree", Color(1, 1, 1));
	// theme->set_color("relationship_line_color", "Tree", Color(0.27, 0.27, 0.27));
	// theme->set_color("parent_hl_line_color", "Tree", Color(0.27, 0.27, 0.27));
	// theme->set_color("children_hl_line_color", "Tree", Color(0.27, 0.27, 0.27));
	// theme->set_color("custom_button_font_highlight", "Tree", control_font_hover_color);

	// theme->set_constant("h_separation", "Tree", Math::round(4 * scale));
	// theme->set_constant("v_separation", "Tree", Math::round(4 * scale));
	// theme->set_constant("item_margin", "Tree", Math::round(16 * scale));
	// theme->set_constant("inner_item_margin_bottom", "Tree", 0);
	// theme->set_constant("inner_item_margin_left", "Tree", 0);
	// theme->set_constant("inner_item_margin_right", "Tree", 0);
	// theme->set_constant("inner_item_margin_top", "Tree", 0);
	// theme->set_constant("button_margin", "Tree", Math::round(4 * scale));
	// theme->set_constant("draw_relationship_lines", "Tree", 0);
	// theme->set_constant("relationship_line_width", "Tree", 1);
	// theme->set_constant("parent_hl_line_width", "Tree", 1);
	// theme->set_constant("children_hl_line_width", "Tree", 1);
	// theme->set_constant("parent_hl_line_margin", "Tree", 0);
	// theme->set_constant("draw_guides", "Tree", 1);
	// theme->set_constant("scroll_border", "Tree", Math::round(4 * scale));
	// theme->set_constant("scroll_speed", "Tree", 12);
	// theme->set_constant("outline_size", "Tree", 0);
	// theme->set_constant("icon_max_width", "Tree", 0);
	// theme->set_constant("scrollbar_margin_left", "Tree", -1);
	// theme->set_constant("scrollbar_margin_top", "Tree", -1);
	// theme->set_constant("scrollbar_margin_right", "Tree", -1);
	// theme->set_constant("scrollbar_margin_bottom", "Tree", -1);
	// theme->set_constant("scrollbar_h_separation", "Tree", Math::round(4 * scale));
	// theme->set_constant("scrollbar_v_separation", "Tree", Math::round(4 * scale));

	// // ItemList

	// theme->set_stylebox("panel", "ItemList", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_normal_color));
	// theme->set_stylebox("focus", "ItemList", focus);
	// theme->set_constant("h_separation", "ItemList", Math::round(4 * scale));
	// theme->set_constant("v_separation", "ItemList", Math::round(2 * scale));
	// theme->set_constant("icon_margin", "ItemList", Math::round(4 * scale));
	// theme->set_constant("line_separation", "ItemList", Math::round(2 * scale));

	// theme->set_font("font", "ItemList", Ref<Font>());
	// theme->set_font_size("font_size", "ItemList", -1);

	// theme->set_color_scheme("default_color_scheme", "ItemList", default_color_scheme);

	// theme->set_color_scheme("font_color_scheme", "ItemList", Ref<ColorScheme>());
	// theme->set_color_scheme("font_hovered_color_scheme", "ItemList", Ref<ColorScheme>());
	// theme->set_color_scheme("font_selected_color_scheme", "ItemList", Ref<ColorScheme>());
	// theme->set_color_scheme("font_outline_color_scheme", "ItemList", Ref<ColorScheme>());
	// theme->set_color_scheme("guide_color_scheme", "ItemList", Ref<ColorScheme>());

	// theme->set_color_role("font_color_role", "ItemList", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_hovered_color_role", "ItemList", make_color_role(ColorRoleEnum::SHADOW));
	// theme->set_color_role("font_selected_color_role", "ItemList", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_outline_color_role", "ItemList", make_color_role(ColorRoleEnum::OUTLINE));
	// theme->set_color_role("guide_color_role", "ItemList", make_color_role(ColorRoleEnum::SHADOW));

	// theme->set_color("font_color_scale", "ItemList", control_font_lower_color_scale);
	// theme->set_color("font_hovered_color_scale", "ItemList", control_font_hover_color_scale);
	// theme->set_color("font_selected_color_scale", "ItemList", control_font_pressed_color_scale);
	// theme->set_color("font_outline_color_scale", "ItemList", Color(1, 1, 1));
	// theme->set_color("guide_color_scale", "ItemList", Color(0.7, 0.7, 0.7, 0.25));

	// theme->set_color("font_color", "ItemList", control_font_lower_color);
	// theme->set_color("font_hovered_color", "ItemList", control_font_hover_color);
	// theme->set_color("font_selected_color", "ItemList", control_font_pressed_color);
	// theme->set_color("font_outline_color", "ItemList", Color(1, 1, 1));
	// theme->set_color("guide_color", "ItemList", Color(0.7, 0.7, 0.7, 0.25));

	// theme->set_stylebox("hovered", "ItemList", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), Color(1, 1, 1, 0.07)));
	// theme->set_stylebox("selected", "ItemList", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_selected_color_scale));
	// theme->set_stylebox("selected_focus", "ItemList", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), style_selected_color_scale));
	// theme->set_stylebox("cursor", "ItemList", focus);
	// theme->set_stylebox("cursor_unfocused", "ItemList", focus);

	// theme->set_constant("outline_size", "ItemList", 0);

	// // TabContainer

	// Ref<StyleBoxFlat> style_tab_selected = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 10, 4, 10, 4, 0);
	// style_tab_selected->set_border_width(SIDE_TOP, Math::round(2 * scale));
	// style_tab_selected->set_border_color(style_focus_color);
	// style_tab_selected->set_border_color_scale(style_focus_color_scale);
	// Ref<StyleBoxFlat> style_tab_unselected = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SECONDARY_CONTAINER), style_pressed_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 10, 4, 10, 4, 0);
	// // Add some spacing between unselected tabs to make them easier to distinguish from each other.
	// style_tab_unselected->set_border_width(SIDE_LEFT, Math::round(scale));
	// style_tab_unselected->set_border_width(SIDE_RIGHT, Math::round(scale));
	// style_tab_unselected->set_border_color(style_popup_border_color);
	// style_tab_unselected->set_border_color_scale(style_popup_border_color_scale);
	// Ref<StyleBoxFlat> style_tab_disabled = style_tab_unselected->duplicate();
	// style_tab_disabled->set_bg_color(style_disabled_color);
	// style_tab_disabled->set_bg_color_scale(style_disabled_color_scale);
	// Ref<StyleBoxFlat> style_tab_hovered = style_tab_unselected->duplicate();
	// style_tab_hovered->set_bg_color(Color(0.1, 0.1, 0.1, 0.3));
	// style_tab_hovered->set_bg_color_scale(Color(0.1, 0.1, 0.1, 0.3));
	// Ref<StyleBoxFlat> style_tab_focus = focus->duplicate();

	// theme->set_stylebox("tab_selected", "TabContainer", style_tab_selected);
	// theme->set_stylebox("tab_hovered", "TabContainer", style_tab_hovered);
	// theme->set_stylebox("tab_unselected", "TabContainer", style_tab_unselected);
	// theme->set_stylebox("tab_disabled", "TabContainer", style_tab_disabled);
	// theme->set_stylebox("tab_focus", "TabContainer", style_tab_focus);
	// theme->set_stylebox("panel", "TabContainer", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_normal_color, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0));
	// theme->set_stylebox("tabbar_background", "TabContainer", make_empty_stylebox(0, 0, 0, 0));

	// theme->set_icon("increment", "TabContainer", icons["scroll_button_right"]);
	// theme->set_icon("increment_highlight", "TabContainer", icons["scroll_button_right_hl"]);
	// theme->set_icon("decrement", "TabContainer", icons["scroll_button_left"]);
	// theme->set_icon("decrement_highlight", "TabContainer", icons["scroll_button_left_hl"]);
	// theme->set_icon("drop_mark", "TabContainer", icons["tabs_drop_mark"]);
	// theme->set_icon("menu", "TabContainer", icons["tabs_menu"]);
	// theme->set_icon("menu_highlight", "TabContainer", icons["tabs_menu_hl"]);

	// theme->set_font("font", "TabContainer", Ref<Font>());
	// theme->set_font_size("font_size", "TabContainer", -1);

	// theme->set_color_scheme("default_color_scheme", "TabContainer", default_color_scheme);

	// theme->set_color_scheme("drop_mark_color_scheme", "TabContainer", Ref<ColorScheme>());
	// theme->set_color_scheme("font_selected_color_scheme", "TabContainer", Ref<ColorScheme>());
	// theme->set_color_scheme("font_hovered_color_scheme", "TabContainer", Ref<ColorScheme>());
	// theme->set_color_scheme("font_unselected_color_scheme", "TabContainer", Ref<ColorScheme>());
	// theme->set_color_scheme("font_disabled_color_scheme", "TabContainer", Ref<ColorScheme>());
	// theme->set_color_scheme("font_outline_color_scheme", "TabContainer", Ref<ColorScheme>());

	// theme->set_color_role("drop_mark_color_role", "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("font_selected_color_role", "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("font_hovered_color_role", "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("font_unselected_color_role", "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("font_disabled_color_role", "TabContainer", make_color_role(ColorRoleEnum::ON_PRIMARY_CONTAINER));
	// theme->set_color_role("font_outline_color_role", "TabContainer", make_color_role(ColorRoleEnum::OUTLINE));

	// theme->set_color("font_selected_color_scale", "TabContainer", control_font_hover_color_scale);
	// theme->set_color("font_hovered_color_scale", "TabContainer", control_font_hover_color_scale);
	// theme->set_color("font_unselected_color_scale", "TabContainer", control_font_low_color_scale);
	// theme->set_color("font_disabled_color_scale", "TabContainer", control_font_disabled_color_scale);
	// theme->set_color("font_outline_color_scale", "TabContainer", Color(1, 1, 1));
	// theme->set_color("drop_mark_color_scale", "TabContainer", Color(1, 1, 1));

	// theme->set_color("font_selected_color", "TabContainer", control_font_hover_color);
	// theme->set_color("font_hovered_color", "TabContainer", control_font_hover_color);
	// theme->set_color("font_unselected_color", "TabContainer", control_font_low_color);
	// theme->set_color("font_disabled_color", "TabContainer", control_font_disabled_color);
	// theme->set_color("font_outline_color", "TabContainer", Color(1, 1, 1));
	// theme->set_color("drop_mark_color", "TabContainer", Color(1, 1, 1));

	// theme->set_constant("side_margin", "TabContainer", Math::round(8 * scale));
	// theme->set_constant("icon_separation", "TabContainer", Math::round(4 * scale));
	// theme->set_constant("icon_max_width", "TabContainer", 0);
	// theme->set_constant("outline_size", "TabContainer", 0);

	// // TabBar

	// theme->set_stylebox("tab_selected", "TabBar", style_tab_selected);
	// theme->set_stylebox("tab_hovered", "TabBar", style_tab_hovered);
	// theme->set_stylebox("tab_unselected", "TabBar", style_tab_unselected);
	// theme->set_stylebox("tab_disabled", "TabBar", style_tab_disabled);
	// theme->set_stylebox("tab_focus", "TabBar", style_tab_focus);
	// theme->set_stylebox("button_pressed", "TabBar", button_pressed);
	// theme->set_stylebox("button_highlight", "TabBar", button_normal);

	// theme->set_icon("increment", "TabBar", icons["scroll_button_right"]);
	// theme->set_icon("increment_highlight", "TabBar", icons["scroll_button_right_hl"]);
	// theme->set_icon("decrement", "TabBar", icons["scroll_button_left"]);
	// theme->set_icon("decrement_highlight", "TabBar", icons["scroll_button_left_hl"]);
	// theme->set_icon("drop_mark", "TabBar", icons["tabs_drop_mark"]);
	// theme->set_icon("close", "TabBar", icons["close"]);

	// theme->set_font("font", "TabBar", Ref<Font>());
	// theme->set_font_size("font_size", "TabBar", -1);

	// theme->set_color_scheme("default_color_scheme", "TabBar", default_color_scheme);

	// theme->set_color_scheme("drop_mark_color_scheme", "TabBar", Ref<ColorScheme>());
	// theme->set_color_scheme("font_selected_color_scheme", "TabBar", Ref<ColorScheme>());
	// theme->set_color_scheme("font_hovered_color_scheme", "TabBar", Ref<ColorScheme>());
	// theme->set_color_scheme("font_unselected_color_scheme", "TabBar", Ref<ColorScheme>());
	// theme->set_color_scheme("font_disabled_color_scheme", "TabBar", Ref<ColorScheme>());
	// theme->set_color_scheme("font_outline_color_scheme", "TabBar", Ref<ColorScheme>());

	// theme->set_color_role("drop_mark_color_role", "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_selected_color_role", "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_hovered_color_role", "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_unselected_color_role", "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_disabled_color_role", "TabBar", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_outline_color_role", "TabBar", make_color_role(ColorRoleEnum::SHADOW));

	// theme->set_color("font_selected_color_scale", "TabBar", control_font_hover_color_scale);
	// theme->set_color("font_hovered_color_scale", "TabBar", control_font_hover_color_scale);
	// theme->set_color("font_unselected_color_scale", "TabBar", control_font_low_color_scale);
	// theme->set_color("font_disabled_color_scale", "TabBar", control_font_disabled_color_scale);
	// theme->set_color("font_outline_color_scale", "TabBar", Color(1, 1, 1));
	// theme->set_color("drop_mark_color_scale", "TabBar", Color(1, 1, 1));

	// theme->set_color("font_selected_color", "TabBar", control_font_hover_color);
	// theme->set_color("font_hovered_color", "TabBar", control_font_hover_color);
	// theme->set_color("font_unselected_color", "TabBar", control_font_low_color);
	// theme->set_color("font_disabled_color", "TabBar", control_font_disabled_color);
	// theme->set_color("font_outline_color", "TabBar", Color(1, 1, 1));
	// theme->set_color("drop_mark_color", "TabBar", Color(1, 1, 1));

	// theme->set_constant("h_separation", "TabBar", Math::round(4 * scale));
	// theme->set_constant("icon_max_width", "TabBar", 0);
	// theme->set_constant("outline_size", "TabBar", 0);

	// // ColorPicker

	// theme->set_constant("margin", "ColorPicker", Math::round(4 * scale));
	// theme->set_constant("sv_width", "ColorPicker", Math::round(256 * scale));
	// theme->set_constant("sv_height", "ColorPicker", Math::round(256 * scale));
	// theme->set_constant("h_width", "ColorPicker", Math::round(30 * scale));
	// theme->set_constant("label_width", "ColorPicker", Math::round(10 * scale));
	// theme->set_constant("center_slider_grabbers", "ColorPicker", 1);

	// theme->set_icon("folded_arrow", "ColorPicker", icons["arrow_right"]);
	// theme->set_icon("expanded_arrow", "ColorPicker", icons["arrow_down"]);
	// theme->set_icon("screen_picker", "ColorPicker", icons["color_picker_pipette"]);
	// theme->set_icon("shape_circle", "ColorPicker", icons["picker_shape_circle"]);
	// theme->set_icon("shape_rect", "ColorPicker", icons["picker_shape_rectangle"]);
	// theme->set_icon("shape_rect_wheel", "ColorPicker", icons["picker_shape_rectangle_wheel"]);
	// theme->set_icon("add_preset", "ColorPicker", icons["add"]);
	// theme->set_icon("sample_bg", "ColorPicker", icons["mini_checkerboard"]);
	// theme->set_icon("overbright_indicator", "ColorPicker", icons["color_picker_overbright"]);
	// theme->set_icon("bar_arrow", "ColorPicker", icons["color_picker_bar_arrow"]);
	// theme->set_icon("picker_cursor", "ColorPicker", icons["color_picker_cursor"]);

	// {
	// 	const int precision = 7;

	// 	Ref<Gradient> hue_gradient;
	// 	hue_gradient.instantiate();
	// 	PackedFloat32Array offsets;
	// 	offsets.resize(precision);
	// 	PackedColorArray colors;
	// 	colors.resize(precision);

	// 	for (int i = 0; i < precision; i++) {
	// 		float h = i / float(precision - 1);
	// 		offsets.write[i] = h;
	// 		colors.write[i] = Color::from_hsv(h, 1, 1);
	// 	}
	// 	hue_gradient->set_offsets(offsets);
	// 	hue_gradient->set_colors(colors);

	// 	Ref<GradientTexture2D> hue_texture;
	// 	hue_texture.instantiate();
	// 	hue_texture->set_width(800);
	// 	hue_texture->set_height(6);
	// 	hue_texture->set_gradient(hue_gradient);

	// 	theme->set_icon("color_hue", "ColorPicker", hue_texture);
	// }

	// {
	// 	const int precision = 7;

	// 	Ref<Gradient> hue_gradient;
	// 	hue_gradient.instantiate();
	// 	PackedFloat32Array offsets;
	// 	offsets.resize(precision);
	// 	PackedColorArray colors;
	// 	colors.resize(precision);

	// 	for (int i = 0; i < precision; i++) {
	// 		float h = i / float(precision - 1);
	// 		offsets.write[i] = h;
	// 		colors.write[i] = Color::from_ok_hsl(h, 1, 0.5);
	// 	}
	// 	hue_gradient->set_offsets(offsets);
	// 	hue_gradient->set_colors(colors);

	// 	Ref<GradientTexture2D> hue_texture;
	// 	hue_texture.instantiate();
	// 	hue_texture->set_width(800);
	// 	hue_texture->set_height(6);
	// 	hue_texture->set_gradient(hue_gradient);

	// 	theme->set_icon("color_okhsl_hue", "ColorPicker", hue_texture);
	// }

	// // ColorPickerButton

	// theme->set_icon("bg", "ColorPickerButton", icons["mini_checkerboard"]);
	// theme->set_stylebox("normal", "ColorPickerButton", button_normal);
	// theme->set_stylebox("pressed", "ColorPickerButton", button_pressed);
	// theme->set_stylebox("hover", "ColorPickerButton", button_hover);
	// theme->set_stylebox("disabled", "ColorPickerButton", button_disabled);
	// theme->set_stylebox("focus", "ColorPickerButton", focus);

	// theme->set_font("font", "ColorPickerButton", Ref<Font>());
	// theme->set_font_size("font_size", "ColorPickerButton", -1);
	// theme->set_color_scheme("default_color_scheme", "TabBar", default_color_scheme);

	// theme->set_color("font_color_scale", "ColorPickerButton", control_font_color_scale);
	// theme->set_color("font_focus_color_scale", "ColorPickerButton", control_font_focus_color_scale);
	// theme->set_color("font_pressed_color_scale", "ColorPickerButton", control_font_pressed_color_scale);
	// theme->set_color("font_hover_color_scale", "ColorPickerButton", control_font_hover_color_scale);
	// theme->set_color("font_hover_pressed_color_scale", "ColorPickerButton", control_font_hover_pressed_color_scale);
	// theme->set_color("font_disabled_color_scale", "ColorPickerButton", control_font_disabled_color_scale);
	// theme->set_color("font_outline_color_scale", "ColorPickerButton", control_font_outline_color_scale);

	// theme->set_color_role("font_color_role", "ColorPickerButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_pressed_color_role", "ColorPickerButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_hover_color_role", "ColorPickerButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_focus_color_role", "ColorPickerButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_hover_pressed_color_role", "ColorPickerButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_disabled_color_role", "ColorPickerButton", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_outline_color_role", "ColorPickerButton", make_color_role(ColorRoleEnum::OUTLINE));

	// theme->set_color_scheme("font_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	// theme->set_color_scheme("font_pressed_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	// theme->set_color_scheme("font_hover_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	// theme->set_color_scheme("font_focus_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	// theme->set_color_scheme("font_hover_pressed_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	// theme->set_color_scheme("font_disabled_color_scheme", "ColorPickerButton", Ref<ColorScheme>());
	// theme->set_color_scheme("font_outline_color_scheme", "ColorPickerButton", Ref<ColorScheme>());

	// theme->set_color("font_color", "ColorPickerButton", Color(1, 1, 1, 1));
	// theme->set_color("font_pressed_color", "ColorPickerButton", Color(0.8, 0.8, 0.8, 1));
	// theme->set_color("font_hover_color", "ColorPickerButton", Color(1, 1, 1, 1));
	// theme->set_color("font_focus_color", "ColorPickerButton", Color(1, 1, 1, 1));
	// theme->set_color("font_hover_pressed_color", "ColorPickerButton", Color(0.9, 0.9, 0.9, 0.3));
	// theme->set_color("font_disabled_color", "ColorPickerButton", Color(0.9, 0.9, 0.9, 0.3));
	// theme->set_color("font_outline_color", "ColorPickerButton", Color(1, 1, 1));

	// theme->set_constant("h_separation", "ColorPickerButton", Math::round(4 * scale));
	// theme->set_constant("outline_size", "ColorPickerButton", 0);

	// // ColorPresetButton

	// Ref<StyleBoxFlat> preset_sb = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), Color(1, 1, 1), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 2, 2, 2, 2);
	// preset_sb->set_corner_radius_all(Math::round(2 * scale));
	// preset_sb->set_corner_detail(Math::round(2 * scale));
	// preset_sb->set_anti_aliased(false);

	// theme->set_stylebox("preset_fg", "ColorPresetButton", preset_sb);
	// theme->set_icon("preset_bg", "ColorPresetButton", icons["mini_checkerboard"]);
	// theme->set_icon("overbright_indicator", "ColorPresetButton", icons["color_picker_overbright"]);

	// // TooltipPanel + TooltipLabel

	// theme->set_type_variation("TooltipPanel", "PopupPanel");
	// theme->set_stylebox("panel", "TooltipPanel",
	// 		make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::SURFACE_CONTAINER), Color(1, 1, 1, 0.5), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 2 * default_margin, 0.5 * default_margin, 2 * default_margin, 0.5 * default_margin));

	// theme->set_type_variation("TooltipLabel", "Label");
	// theme->set_font_size("font_size", "TooltipLabel", -1);
	// theme->set_font("font", "TooltipLabel", Ref<Font>());

	// theme->set_color_scheme("font_color_scheme", "TooltipLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("font_shadow_color_scheme", "TooltipLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("font_outline_color_scheme", "TooltipLabel", Ref<ColorScheme>());

	// theme->set_color_role("font_color_role", "TooltipLabel", make_color_role(ColorRoleEnum::PRIMARY));
	// theme->set_color_role("font_shadow_color_role", "TooltipLabel", make_color_role(ColorRoleEnum::SHADOW));
	// theme->set_color_role("font_outline_color_role", "TooltipLabel", make_color_role(ColorRoleEnum::PRIMARY));

	// theme->set_color("font_color_scale", "TooltipLabel", control_font_color_scale);
	// theme->set_color("font_shadow_color_scale", "TooltipLabel", Color(1, 1, 1, 1));
	// theme->set_color("font_outline_color_scale", "TooltipLabel", Color(1, 1, 1, 1));

	// theme->set_color("font_color", "TooltipLabel", control_font_color);
	// theme->set_color("font_shadow_color", "TooltipLabel", Color(0, 0, 0, 0));
	// theme->set_color("font_outline_color", "TooltipLabel", Color(0, 0, 0, 0));

	// theme->set_constant("shadow_offset_x", "TooltipLabel", 1);
	// theme->set_constant("shadow_offset_y", "TooltipLabel", 1);
	// theme->set_constant("outline_size", "TooltipLabel", 0);

	// // RichTextLabel

	// theme->set_stylebox("focus", "RichTextLabel", focus);
	// theme->set_stylebox("normal", "RichTextLabel", make_empty_stylebox(0, 0, 0, 0));

	// theme->set_font("normal_font", "RichTextLabel", Ref<Font>());
	// theme->set_font("bold_font", "RichTextLabel", bold_font);
	// theme->set_font("italics_font", "RichTextLabel", italics_font);
	// theme->set_font("bold_italics_font", "RichTextLabel", bold_italics_font);
	// theme->set_font("mono_font", "RichTextLabel", Ref<Font>());
	// theme->set_font_size("normal_font_size", "RichTextLabel", -1);
	// theme->set_font_size("bold_font_size", "RichTextLabel", -1);
	// theme->set_font_size("italics_font_size", "RichTextLabel", -1);
	// theme->set_font_size("bold_italics_font_size", "RichTextLabel", -1);
	// theme->set_font_size("mono_font_size", "RichTextLabel", -1);

	// theme->set_color_scheme("default_color_scheme", "RichTextLabel", default_color_scheme);

	// theme->set_color_scheme("font_default_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("font_selected_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("selection_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("font_outline_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("font_shadow_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("outline_color_scheme", "RichTextLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("table_odd_row_bg_scheme", "RichTextLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("table_even_row_bg_scheme", "RichTextLabel", Ref<ColorScheme>());
	// theme->set_color_scheme("table_border_scheme", "RichTextLabel", Ref<ColorScheme>());

	// theme->set_color("font_default_color_scale", "RichTextLabel", Color(1, 1, 1));
	// theme->set_color("font_selected_color_scale", "RichTextLabel", Color(1, 1, 1));
	// theme->set_color("selection_color_scale", "RichTextLabel", Color(1, 1, 1, 0.8));
	// theme->set_color("font_outline_color_scale", "RichTextLabel", Color(1, 1, 1));
	// theme->set_color("font_shadow_color_scale", "RichTextLabel", Color(1, 1, 1, 1));
	// theme->set_color("outline_color_scale_scale", "RichTextLabel", Color(1, 1, 1, 1));
	// theme->set_color("table_odd_row_bg_scale", "RichTextLabel", Color(1, 1, 1, 1));
	// theme->set_color("table_even_row_bg_scale", "RichTextLabel", Color(1, 1, 1, 1));
	// theme->set_color("table_border_scale", "RichTextLabel", Color(1, 1, 1, 1));

	// theme->set_color("font_default_color", "RichTextLabel", Color(1, 1, 1));
	// theme->set_color("font_selected_color", "RichTextLabel", Color(0, 0, 0, 0));
	// theme->set_color("selection_color", "RichTextLabel", Color(0.1, 0.1, 1, 0.8));
	// theme->set_color("font_outline_color", "RichTextLabel", Color(1, 1, 1));
	// theme->set_color("font_shadow_color", "RichTextLabel", Color(0, 0, 0, 0));
	// theme->set_color("outline_color_scale", "RichTextLabel", Color(0, 0, 0, 0));
	// theme->set_color("table_odd_row_bg", "RichTextLabel", Color(0, 0, 0, 0));
	// theme->set_color("table_even_row_bg", "RichTextLabel", Color(0, 0, 0, 0));
	// theme->set_color("table_border", "RichTextLabel", Color(0, 0, 0, 0));

	// theme->set_color_role("font_default_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("font_selected_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("selection_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::PRIMARY));
	// theme->set_color_role("font_outline_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::OUTLINE));
	// theme->set_color_role("font_shadow_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::SHADOW));
	// theme->set_color_role("outline_color_role", "RichTextLabel", make_color_role(ColorRoleEnum::OUTLINE));
	// theme->set_color_role("table_odd_row_bg_role", "RichTextLabel", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("table_even_row_bg_role", "RichTextLabel", make_color_role(ColorRoleEnum::ON_PRIMARY));
	// theme->set_color_role("table_border_role", "RichTextLabel", make_color_role(ColorRoleEnum::ON_PRIMARY));

	// theme->set_color("font_default_color", "RichTextLabel", Color(1, 1, 1));
	// theme->set_color("font_selected_color", "RichTextLabel", Color(0, 0, 0, 0));
	// theme->set_color("selection_color", "RichTextLabel", Color(0.1, 0.1, 1, 0.8));
	// theme->set_color("font_shadow_color", "RichTextLabel", Color(0, 0, 0, 0));
	// theme->set_color("font_outline_color", "RichTextLabel", Color(1, 1, 1));
	// theme->set_color("outline_color", "RichTextLabel", Color(1, 1, 1));

	// theme->set_constant("shadow_offset_x", "RichTextLabel", Math::round(1 * scale));
	// theme->set_constant("shadow_offset_y", "RichTextLabel", Math::round(1 * scale));
	// theme->set_constant("shadow_outline_size", "RichTextLabel", Math::round(1 * scale));

	// theme->set_constant("line_separation", "RichTextLabel", 0);
	// theme->set_constant("table_h_separation", "RichTextLabel", Math::round(3 * scale));
	// theme->set_constant("table_v_separation", "RichTextLabel", Math::round(3 * scale));

	// theme->set_constant("outline_size", "RichTextLabel", 0);

	// theme->set_color("table_odd_row_bg", "RichTextLabel", Color(0, 0, 0, 0));
	// theme->set_color("table_even_row_bg", "RichTextLabel", Color(0, 0, 0, 0));
	// theme->set_color("table_border", "RichTextLabel", Color(0, 0, 0, 0));

	// theme->set_constant("text_highlight_h_padding", "RichTextLabel", Math::round(3 * scale));
	// theme->set_constant("text_highlight_v_padding", "RichTextLabel", Math::round(3 * scale));

	// // Containers

	// theme->set_icon("h_grabber", "SplitContainer", icons["hsplitter"]);
	// theme->set_icon("v_grabber", "SplitContainer", icons["vsplitter"]);
	// theme->set_icon("grabber", "VSplitContainer", icons["vsplitter"]);
	// theme->set_icon("grabber", "HSplitContainer", icons["hsplitter"]);

	// theme->set_constant("separation", "BoxContainer", Math::round(4 * scale));
	// theme->set_constant("separation", "HBoxContainer", Math::round(4 * scale));
	// theme->set_constant("separation", "VBoxContainer", Math::round(4 * scale));
	// theme->set_constant("margin_left", "MarginContainer", 0);
	// theme->set_constant("margin_top", "MarginContainer", 0);
	// theme->set_constant("margin_right", "MarginContainer", 0);
	// theme->set_constant("margin_bottom", "MarginContainer", 0);
	// theme->set_constant("h_separation", "GridContainer", Math::round(4 * scale));
	// theme->set_constant("v_separation", "GridContainer", Math::round(4 * scale));
	// theme->set_constant("separation", "SplitContainer", Math::round(12 * scale));
	// theme->set_constant("separation", "HSplitContainer", Math::round(12 * scale));
	// theme->set_constant("separation", "VSplitContainer", Math::round(12 * scale));
	// theme->set_constant("minimum_grab_thickness", "SplitContainer", Math::round(6 * scale));
	// theme->set_constant("minimum_grab_thickness", "HSplitContainer", Math::round(6 * scale));
	// theme->set_constant("minimum_grab_thickness", "VSplitContainer", Math::round(6 * scale));
	// theme->set_constant("autohide", "SplitContainer", 1);
	// theme->set_constant("autohide", "HSplitContainer", 1);
	// theme->set_constant("autohide", "VSplitContainer", 1);
	// theme->set_constant("h_separation", "FlowContainer", Math::round(4 * scale));
	// theme->set_constant("v_separation", "FlowContainer", Math::round(4 * scale));
	// theme->set_constant("h_separation", "HFlowContainer", Math::round(4 * scale));
	// theme->set_constant("v_separation", "HFlowContainer", Math::round(4 * scale));
	// theme->set_constant("h_separation", "VFlowContainer", Math::round(4 * scale));
	// theme->set_constant("v_separation", "VFlowContainer", Math::round(4 * scale));

	// theme->set_stylebox("panel", "PanelContainer", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0));

	// theme->set_icon("zoom_out", "GraphEdit", icons["zoom_less"]);
	// theme->set_icon("zoom_in", "GraphEdit", icons["zoom_more"]);
	// theme->set_icon("zoom_reset", "GraphEdit", icons["zoom_reset"]);
	// theme->set_icon("grid_toggle", "GraphEdit", icons["grid_toggle"]);
	// theme->set_icon("minimap_toggle", "GraphEdit", icons["grid_minimap"]);
	// theme->set_icon("snapping_toggle", "GraphEdit", icons["grid_snap"]);
	// theme->set_icon("layout", "GraphEdit", icons["grid_layout"]);

	// theme->set_stylebox("panel", "GraphEdit", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY_CONTAINER), style_normal_color_scale, StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 5));

	// Ref<StyleBoxFlat> graph_toolbar_style = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), Color(0.24, 0.24, 0.24, 0.6), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 2, 4, 2);
	// theme->set_stylebox("menu_panel", "GraphEdit", graph_toolbar_style);

	// theme->set_color_scheme("default_color_scheme", "GraphEdit", default_color_scheme);
	// theme->set_color_scheme("grid_major_scheme", "GraphEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("grid_minor_scheme", "GraphEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("activity_color_scheme", "GraphEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("selection_fill_scheme", "GraphEdit", Ref<ColorScheme>());
	// theme->set_color_scheme("selection_stroke_scheme", "GraphEdit", Ref<ColorScheme>());
	// theme->set_color_role("grid_major_role", "GraphEdit", make_color_role(ColorRoleEnum::PRIMARY));
	// theme->set_color_role("grid_minor_role", "GraphEdit", make_color_role(ColorRoleEnum::PRIMARY));
	// theme->set_color_role("activity_color_role", "GraphEdit", make_color_role(ColorRoleEnum::PRIMARY));
	// theme->set_color_role("selection_fill_role", "GraphEdit", make_color_role(ColorRoleEnum::PRIMARY));
	// theme->set_color_role("selection_stroke_role", "GraphEdit", make_color_role(ColorRoleEnum::PRIMARY));

	// theme->set_color("grid_minor", "GraphEdit", Color(1, 1, 1, 0.05));
	// theme->set_color("grid_major", "GraphEdit", Color(1, 1, 1, 0.2));
	// theme->set_color("selection_fill", "GraphEdit", Color(1, 1, 1, 0.3));
	// theme->set_color("selection_stroke", "GraphEdit", Color(1, 1, 1, 0.8));
	// theme->set_color("activity", "GraphEdit", Color(1, 1, 1));

	// theme->set_color("grid_minor_scale", "GraphEdit", Color(1, 1, 1, 0.05));
	// theme->set_color("grid_major_scale", "GraphEdit", Color(1, 1, 1, 0.2));
	// theme->set_color("selection_fill_scale", "GraphEdit", Color(1, 1, 1, 0.3));
	// theme->set_color("selection_stroke_scale", "GraphEdit", Color(1, 1, 1, 0.8));
	// theme->set_color("activity_scale", "GraphEdit", Color(1, 1, 1));

	// // Visual Node Ports

	// theme->set_constant("port_hotzone_inner_extent", "GraphEdit", 22 * scale);
	// theme->set_constant("port_hotzone_outer_extent", "GraphEdit", 26 * scale);

	// theme->set_stylebox("panel", "GraphEditMinimap", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), Color(0.24, 0.24, 0.24), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0));
	// Ref<StyleBoxFlat> style_minimap_camera = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), Color(0.65, 0.65, 0.65, 0.2), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0, 0);
	// style_minimap_camera->set_border_color(Color(0.65, 0.65, 0.65, 0.45));
	// style_minimap_camera->set_border_width_all(1);
	// theme->set_stylebox("camera", "GraphEditMinimap", style_minimap_camera);
	// theme->set_stylebox("node", "GraphEditMinimap", make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), Color(1, 1, 1), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 0, 0, 0, 0, 2));

	// theme->set_color_scheme("default_color_scheme", "GraphEditMinimap", default_color_scheme);
	// theme->set_color_scheme("resizer_color_scheme", "GraphEditMinimap", Ref<ColorScheme>());
	// theme->set_color_role("resizer_color_role", "GraphEditMinimap", make_color_role(ColorRoleEnum::PRIMARY));

	// theme->set_icon("resizer", "GraphEditMinimap", icons["resizer_nw"]);
	// theme->set_color("resizer_color", "GraphEditMinimap", Color(1, 1, 1, 0.85));
	// theme->set_color("resizer_color_scale", "GraphEditMinimap", Color(1, 1, 1, 0.85));

	// Theme

	// default_icon = icons["error_icon"];
	// Same color as the error icon.
	// default_style = make_color_role_flat_stylebox(make_color_role(ColorRoleEnum::PRIMARY), Color(1, 0.365, 0.365), StyleBoxFlat::ElevationLevel::Elevation_Level_0, 4, 4, 4, 4, 0, false, 2);
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
	ThemeDB::get_singleton()->set_fallback_icon_font(default_icon_font);
	ThemeDB::get_singleton()->set_fallback_font_size(default_font_size * default_scale);
	ThemeDB::get_singleton()->set_fallback_color_scheme(default_color_scheme);
}

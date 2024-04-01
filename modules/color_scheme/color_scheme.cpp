/**************************************************************************/
/*  ogg_packet_sequence.cpp                                               */
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

#include "color_scheme.h"
#include "color_role.h"

void ColorScheme::_update_color_scheme() {
	if (dirty == false) {
		return;
	}
	if (source_color == Color() && !source_texture.is_valid()) {
		return;
	}
	scheme = _create_scheme_content();

	emit_signal("updated_color_scheme");

	notify_property_list_changed();
	emit_changed();

	dirty = false;
}

material_color_utilities::SchemeContent ColorScheme::_create_scheme_content() {
	Color cur_source_color = source_color;
	if (source_texture.is_valid()) {
		Ref<Image> cur_source_img = source_texture->get_image();
		cur_source_img->decompress();
		std::vector<material_color_utilities::Argb> pixels;
		for (auto j = 0; j < cur_source_img->get_height(); j++) {
			for (auto i = 0; i < cur_source_img->get_width(); i++) {
				material_color_utilities::Argb cur_color = cur_source_img->get_pixel(i, j).to_argb32();
				pixels.push_back(cur_color);
			}
		}
		material_color_utilities::QuantizerResult result = material_color_utilities::QuantizeCelebi(pixels, 256);

		material_color_utilities::ScoreOptions cur_options;
		cur_options.desired = 4;
		std::vector<material_color_utilities::Argb> ranked = material_color_utilities::RankedSuggestions(result.color_to_count, cur_options);
		material_color_utilities::Argb cur_color = ranked[0];
		cur_source_color = get_color_from_argb(cur_color);
	}

	material_color_utilities::Hct source_color_hct(cur_source_color.to_argb32());
	material_color_utilities::SchemeContent cur_color_scheme_content = material_color_utilities::SchemeContent(source_color_hct, dark, contrast_level);
	return cur_color_scheme_content;
}

void ColorScheme::set_source_color(Color p_source_color) {
	source_color = p_source_color;
	source_texture = Ref<Texture2D>();

	dirty = true;
	_update_color_scheme();
	// call_deferred(SNAME("_update_color_scheme"));
}

Color ColorScheme::get_source_color() const {
	return source_color;
}

void ColorScheme::set_source_texture(const Ref<Texture2D> &p_source_texture) {
	source_texture = p_source_texture;
	source_color = Color();

	dirty = true;
	_update_color_scheme();
	// call_deferred(SNAME("_update_color_scheme"));
}

Ref<Texture2D> ColorScheme::get_source_texture() const {
	return source_texture;
}

void ColorScheme::set_dark(bool p_dark) {
	dark = p_dark;
	dirty = true;
	_update_color_scheme();
	// call_deferred(SNAME("_update_color_scheme"));
}

bool ColorScheme::is_dark() const {
	return dark;
}

void ColorScheme::set_contrast_level(float p_contrast_level) {
	contrast_level = p_contrast_level;
	dirty = true;
	_update_color_scheme();
	// call_deferred(SNAME("_update_color_scheme"));
}

float ColorScheme::get_contrast_level() const {
	return contrast_level;
}


Color ColorScheme::get_color(ColorRoleEnum cur_color_role_enum) {
	switch (cur_color_role_enum) {
		case ColorRoleEnum::STATIC_COLOR:
			return Color(1, 1, 1, 1);
		case ColorRoleEnum::NO_LERP:
			return Color(1, 1, 1, 1);
		case ColorRoleEnum::PRIMARY_PALETTE_KEY:
			return get_primary_palette_key_color();
		case ColorRoleEnum::SECONDARY_PALETTE_KEY:
			return get_secondary_palette_key_color();
		case ColorRoleEnum::TERTIARY_PALETTE_KEY:
			return get_tertiary_palette_key_color();
		case ColorRoleEnum::NEUTRAL_PALETTE_KEY:
			return get_neutral_palette_key_color();
		case ColorRoleEnum::NEUTRAL_VARIANT_PALETTE_KEY:
			return get_neutral_variant_palette_key_color();
		case ColorRoleEnum::STATIC_TRANSPARENT:
			return Color(0, 0, 0, 0);
		case ColorRoleEnum::STATIC_ONE:
			return Color(1, 1, 1, 1);
		case ColorRoleEnum::BACKGROUND:
			return get_background();
		case ColorRoleEnum::ON_BACKGROUND:
			return get_on_background();
		case ColorRoleEnum::SURFACE:
			return get_surface();
		case ColorRoleEnum::SURFACE_DIM:
			return get_surface_dim();
		case ColorRoleEnum::SURFACE_BRIGHT:
			return get_surface_bright();
		case ColorRoleEnum::SURFACE_CONTAINER_LOWEST:
			return get_surface_container_lowest();
		case ColorRoleEnum::SURFACE_CONTAINER_LOW:
			return get_surface_container_low();
		case ColorRoleEnum::SURFACE_CONTAINER:
			return get_surface_container();
		case ColorRoleEnum::SURFACE_CONTAINER_HIGH:
			return get_surface_container_high();
		case ColorRoleEnum::SURFACE_CONTAINER_HIGHEST:
			return get_surface_container_highest();
		case ColorRoleEnum::ON_SURFACE:
			return get_on_surface();
		case ColorRoleEnum::SURFACE_VARIANT:
			return get_surface_variant();
		case ColorRoleEnum::ON_SURFACE_VARIANT:
			return get_on_surface_variant();
		case ColorRoleEnum::INVERSE_SURFACE:
			return get_inverse_surface();
		case ColorRoleEnum::INVERSE_ON_SURFACE:
			return get_inverse_on_surface();
		case ColorRoleEnum::OUTLINE:
			return get_outline();
		case ColorRoleEnum::OUTLINE_VARIANT:
			return get_outline_variant();
		case ColorRoleEnum::SHADOW:
			return get_shadow();
		case ColorRoleEnum::SCRIM:
			return get_scrim();
		case ColorRoleEnum::SURFACE_TINT:
			return get_surface_tint();
		case ColorRoleEnum::PRIMARY:
			return get_primary();
		case ColorRoleEnum::ON_PRIMARY:
			return get_on_primary();
		case ColorRoleEnum::PRIMARY_CONTAINER:
			return get_primary_container();
		case ColorRoleEnum::ON_PRIMARY_CONTAINER:
			return get_on_primary_container();
		case ColorRoleEnum::INVERSE_PRIMARY:
			return get_inverse_primary();
		case ColorRoleEnum::SECONDARY:
			return get_secondary();
		case ColorRoleEnum::ON_SECONDARY:
			return get_on_secondary();
		case ColorRoleEnum::SECONDARY_CONTAINER:
			return get_secondary_container();
		case ColorRoleEnum::ON_SECONDARY_CONTAINER:
			return get_on_secondary_container();
		case ColorRoleEnum::TERTIARY:
			return get_tertiary();
		case ColorRoleEnum::ON_TERTIARY:
			return get_on_tertiary();
		case ColorRoleEnum::TERTIARY_CONTAINER:
			return get_tertiary_container();
		case ColorRoleEnum::ON_TERTIARY_CONTAINER:
			return get_on_tertiary_container();
		case ColorRoleEnum::ERROR:
			return get_error();
		case ColorRoleEnum::ON_ERROR:
			return get_on_error();
		case ColorRoleEnum::ERROR_CONTAINER:
			return get_error_container();
		case ColorRoleEnum::ON_ERROR_CONTAINER:
			return get_on_error_container();
		case ColorRoleEnum::PRIMARY_FIXED:
			return get_primary_fixed();
		case ColorRoleEnum::PRIMARY_FIXED_DIM:
			return get_primary_fixed_dim();
		case ColorRoleEnum::ON_PRIMARY_FIXED:
			return get_on_primary_fixed();
		case ColorRoleEnum::ON_PRIMARY_FIXED_VARIANT:
			return get_on_primary_fixed_variant();
		case ColorRoleEnum::SECONDARY_FIXED:
			return get_secondary_fixed();
		case ColorRoleEnum::SECONDARY_FIXED_DIM:
			return get_secondary_fixed_dim();
		case ColorRoleEnum::ON_SECONDARY_FIXED:
			return get_on_secondary_fixed();
		case ColorRoleEnum::ON_SECONDARY_FIXED_VARIANT:
			return get_on_secondary_fixed_variant();
		case ColorRoleEnum::TERTIARY_FIXED:
			return get_tertiary_fixed();
		case ColorRoleEnum::TERTIARY_FIXED_DIM:
			return get_tertiary_fixed_dim();
		case ColorRoleEnum::ON_TERTIARY_FIXED:
			return get_on_tertiary_fixed();
		case ColorRoleEnum::ON_TERTIARY_FIXED_VARIANT:
			return get_on_tertiary_fixed_variant();
		default:
			return get_source_color_argb();
	}
}

Color ColorScheme::get_color_from_argb(material_color_utilities::Argb cur_argb) {
	return Color(material_color_utilities::RedFromInt(cur_argb) / 255.0, material_color_utilities::GreenFromInt(cur_argb) / 255.0, material_color_utilities::BlueFromInt(cur_argb) / 255.0, material_color_utilities::AlphaFromInt(cur_argb) / 255.0);
}

Color ColorScheme::get_source_color_argb() {
	return get_color_from_argb(scheme.SourceColorArgb());
}

Color ColorScheme::get_primary_palette_key_color() {
	return get_color_from_argb(scheme.GetPrimaryPaletteKeyColor());
}

Color ColorScheme::get_secondary_palette_key_color() {
	return get_color_from_argb(scheme.GetSecondaryPaletteKeyColor());
}

Color ColorScheme::get_tertiary_palette_key_color() {
	return get_color_from_argb(scheme.GetTertiaryPaletteKeyColor());
}

Color ColorScheme::get_neutral_palette_key_color() {
	return get_color_from_argb(scheme.GetNeutralPaletteKeyColor());
}

Color ColorScheme::get_neutral_variant_palette_key_color() {
	return get_color_from_argb(scheme.GetNeutralVariantPaletteKeyColor());
}

Color ColorScheme::get_background() {
	return get_color_from_argb(scheme.GetBackground());
}

Color ColorScheme::get_on_background() {
	return get_color_from_argb(scheme.GetOnBackground());
}

Color ColorScheme::get_surface() {
	return get_color_from_argb(scheme.GetSurface());
}

Color ColorScheme::get_surface_dim() {
	return get_color_from_argb(scheme.GetSurfaceDim());
}

Color ColorScheme::get_surface_bright() {
	return get_color_from_argb(scheme.GetSurfaceBright());
}

Color ColorScheme::get_surface_container_lowest() {
	return get_color_from_argb(scheme.GetSurfaceContainerLowest());
}

Color ColorScheme::get_surface_container_low() {
	return get_color_from_argb(scheme.GetSurfaceContainerLow());
}

Color ColorScheme::get_surface_container() {
	return get_color_from_argb(scheme.GetSurfaceContainer());
}

Color ColorScheme::get_surface_container_high() {
	return get_color_from_argb(scheme.GetSurfaceContainerHigh());
}

Color ColorScheme::get_surface_container_highest() {
	return get_color_from_argb(scheme.GetSurfaceContainerHighest());
}

Color ColorScheme::get_on_surface() {
	return get_color_from_argb(scheme.GetOnSurface());
}

Color ColorScheme::get_surface_variant() {
	return get_color_from_argb(scheme.GetSurfaceVariant());
}

Color ColorScheme::get_on_surface_variant() {
	return get_color_from_argb(scheme.GetOnSurfaceVariant());
}

Color ColorScheme::get_inverse_surface() {
	return get_color_from_argb(scheme.GetInverseSurface());
}

Color ColorScheme::get_inverse_on_surface() {
	return get_color_from_argb(scheme.GetInverseOnSurface());
}

Color ColorScheme::get_outline() {
	return get_color_from_argb(scheme.GetOutline());
}

Color ColorScheme::get_outline_variant() {
	return get_color_from_argb(scheme.GetOutlineVariant());
}

Color ColorScheme::get_shadow() {
	return get_color_from_argb(scheme.GetShadow());
}

Color ColorScheme::get_scrim() {
	return get_color_from_argb(scheme.GetScrim());
}

Color ColorScheme::get_surface_tint() {
	return get_color_from_argb(scheme.GetSurfaceTint());
}

Color ColorScheme::get_primary() {
	return get_color_from_argb(scheme.GetPrimary());
}

Color ColorScheme::get_on_primary() {
	return get_color_from_argb(scheme.GetOnPrimary());
}

Color ColorScheme::get_primary_container() {
	return get_color_from_argb(scheme.GetPrimaryContainer());
}

Color ColorScheme::get_on_primary_container() {
	return get_color_from_argb(scheme.GetOnPrimaryContainer());
}

Color ColorScheme::get_inverse_primary() {
	return get_color_from_argb(scheme.GetInversePrimary());
}

Color ColorScheme::get_secondary() {
	return get_color_from_argb(scheme.GetSecondary());
}

Color ColorScheme::get_on_secondary() {
	return get_color_from_argb(scheme.GetOnSecondary());
}

Color ColorScheme::get_secondary_container() {
	return get_color_from_argb(scheme.GetSecondaryContainer());
}

Color ColorScheme::get_on_secondary_container() {
	return get_color_from_argb(scheme.GetOnSecondaryContainer());
}

Color ColorScheme::get_tertiary() {
	return get_color_from_argb(scheme.GetTertiary());
}

Color ColorScheme::get_on_tertiary() {
	return get_color_from_argb(scheme.GetOnTertiary());
}

Color ColorScheme::get_tertiary_container() {
	return get_color_from_argb(scheme.GetTertiaryContainer());
}

Color ColorScheme::get_on_tertiary_container() {
	return get_color_from_argb(scheme.GetOnTertiaryContainer());
}

Color ColorScheme::get_error() {
	return get_color_from_argb(scheme.GetError());
}

Color ColorScheme::get_on_error() {
	return get_color_from_argb(scheme.GetOnError());
}

Color ColorScheme::get_error_container() {
	return get_color_from_argb(scheme.GetErrorContainer());
}

Color ColorScheme::get_on_error_container() {
	return get_color_from_argb(scheme.GetOnErrorContainer());
}

Color ColorScheme::get_primary_fixed() {
	return get_color_from_argb(scheme.GetPrimaryFixed());
}

Color ColorScheme::get_primary_fixed_dim() {
	return get_color_from_argb(scheme.GetPrimaryFixedDim());
}

Color ColorScheme::get_on_primary_fixed() {
	return get_color_from_argb(scheme.GetOnPrimaryFixed());
}

Color ColorScheme::get_on_primary_fixed_variant() {
	return get_color_from_argb(scheme.GetOnPrimaryFixedVariant());
}

Color ColorScheme::get_secondary_fixed() {
	return get_color_from_argb(scheme.GetSecondaryFixed());
}

Color ColorScheme::get_secondary_fixed_dim() {
	return get_color_from_argb(scheme.GetSecondaryFixedDim());
}

Color ColorScheme::get_on_secondary_fixed() {
	return get_color_from_argb(scheme.GetOnSecondaryFixed());
}

Color ColorScheme::get_on_secondary_fixed_variant() {
	return get_color_from_argb(scheme.GetOnSecondaryFixedVariant());
}

Color ColorScheme::get_tertiary_fixed() {
	return get_color_from_argb(scheme.GetTertiaryFixed());
}

Color ColorScheme::get_tertiary_fixed_dim() {
	return get_color_from_argb(scheme.GetTertiaryFixedDim());
}

Color ColorScheme::get_on_tertiary_fixed() {
	return get_color_from_argb(scheme.GetOnTertiaryFixed());
}

Color ColorScheme::get_on_tertiary_fixed_variant() {
	return get_color_from_argb(scheme.GetOnTertiaryFixedVariant());
}

void ColorScheme::_bind_methods() {
	ADD_SIGNAL(MethodInfo("updated_color_scheme"));

	ClassDB::bind_method(D_METHOD("set_source_color", "source_color"), &ColorScheme::set_source_color);
	ClassDB::bind_method(D_METHOD("set_source_texture", "source_texture"), &ColorScheme::set_source_texture);
	ClassDB::bind_method(D_METHOD("set_dark", "dark"), &ColorScheme::set_dark);
	ClassDB::bind_method(D_METHOD("set_contrast_level", "contrast_level"), &ColorScheme::set_contrast_level);

	ClassDB::bind_method(D_METHOD("get_source_color"), &ColorScheme::get_source_color);
	ClassDB::bind_method(D_METHOD("get_source_texture"), &ColorScheme::get_source_texture);
	ClassDB::bind_method(D_METHOD("is_dark"), &ColorScheme::is_dark);
	ClassDB::bind_method(D_METHOD("get_contrast_level"), &ColorScheme::get_contrast_level);

	// ClassDB::bind_method(D_METHOD("get_color", "color_role"), &ColorScheme::get_color);
	// ClassDB::bind_method(D_METHOD("get_color_from_role", "color_role"), &ColorScheme::get_color);

	ClassDB::bind_method(D_METHOD("get_primary_palette_key_color"), &ColorScheme::get_primary_palette_key_color);
	ClassDB::bind_method(D_METHOD("get_secondary_palette_key_color"), &ColorScheme::get_secondary_palette_key_color);
	ClassDB::bind_method(D_METHOD("get_tertiary_palette_key_color"), &ColorScheme::get_tertiary_palette_key_color);
	ClassDB::bind_method(D_METHOD("get_neutral_palette_key_color"), &ColorScheme::get_neutral_palette_key_color);
	ClassDB::bind_method(D_METHOD("get_neutral_variant_palette_key_color"), &ColorScheme::get_neutral_variant_palette_key_color);
	ClassDB::bind_method(D_METHOD("get_background"), &ColorScheme::get_background);
	ClassDB::bind_method(D_METHOD("get_on_background"), &ColorScheme::get_on_background);
	ClassDB::bind_method(D_METHOD("get_surface"), &ColorScheme::get_surface);
	ClassDB::bind_method(D_METHOD("get_surface_dim"), &ColorScheme::get_surface_dim);
	ClassDB::bind_method(D_METHOD("get_surface_bright"), &ColorScheme::get_surface_bright);
	ClassDB::bind_method(D_METHOD("get_surface_container_lowest"), &ColorScheme::get_surface_container_lowest);
	ClassDB::bind_method(D_METHOD("get_surface_container_low"), &ColorScheme::get_surface_container_low);
	ClassDB::bind_method(D_METHOD("get_surface_container"), &ColorScheme::get_surface_container);
	ClassDB::bind_method(D_METHOD("get_surface_container_high"), &ColorScheme::get_surface_container_high);
	ClassDB::bind_method(D_METHOD("get_surface_container_highest"), &ColorScheme::get_surface_container_highest);
	ClassDB::bind_method(D_METHOD("get_on_surface"), &ColorScheme::get_on_surface);
	ClassDB::bind_method(D_METHOD("get_surface_variant"), &ColorScheme::get_surface_variant);
	ClassDB::bind_method(D_METHOD("get_on_surface_variant"), &ColorScheme::get_on_surface_variant);
	ClassDB::bind_method(D_METHOD("get_inverse_surface"), &ColorScheme::get_inverse_surface);
	ClassDB::bind_method(D_METHOD("get_inverse_on_surface"), &ColorScheme::get_inverse_on_surface);
	ClassDB::bind_method(D_METHOD("get_outline"), &ColorScheme::get_outline);
	ClassDB::bind_method(D_METHOD("get_outline_variant"), &ColorScheme::get_outline_variant);
	ClassDB::bind_method(D_METHOD("get_shadow"), &ColorScheme::get_shadow);
	ClassDB::bind_method(D_METHOD("get_scrim"), &ColorScheme::get_scrim);
	ClassDB::bind_method(D_METHOD("get_surface_tint"), &ColorScheme::get_surface_tint);
	ClassDB::bind_method(D_METHOD("get_primary"), &ColorScheme::get_primary);
	ClassDB::bind_method(D_METHOD("get_on_primary"), &ColorScheme::get_on_primary);
	ClassDB::bind_method(D_METHOD("get_primary_container"), &ColorScheme::get_primary_container);
	ClassDB::bind_method(D_METHOD("get_on_primary_container"), &ColorScheme::get_on_primary_container);
	ClassDB::bind_method(D_METHOD("get_inverse_primary"), &ColorScheme::get_inverse_primary);
	ClassDB::bind_method(D_METHOD("get_secondary"), &ColorScheme::get_secondary);
	ClassDB::bind_method(D_METHOD("get_on_secondary"), &ColorScheme::get_on_secondary);
	ClassDB::bind_method(D_METHOD("get_secondary_container"), &ColorScheme::get_secondary_container);
	ClassDB::bind_method(D_METHOD("get_on_secondary_container"), &ColorScheme::get_on_secondary_container);
	ClassDB::bind_method(D_METHOD("get_tertiary"), &ColorScheme::get_tertiary);
	ClassDB::bind_method(D_METHOD("get_on_tertiary"), &ColorScheme::get_on_tertiary);
	ClassDB::bind_method(D_METHOD("get_tertiary_container"), &ColorScheme::get_tertiary_container);
	ClassDB::bind_method(D_METHOD("get_on_tertiary_container"), &ColorScheme::get_on_tertiary_container);
	ClassDB::bind_method(D_METHOD("get_error"), &ColorScheme::get_error);
	ClassDB::bind_method(D_METHOD("get_on_error"), &ColorScheme::get_on_error);
	ClassDB::bind_method(D_METHOD("get_error_container"), &ColorScheme::get_error_container);
	ClassDB::bind_method(D_METHOD("get_on_error_container"), &ColorScheme::get_on_error_container);
	ClassDB::bind_method(D_METHOD("get_primary_fixed"), &ColorScheme::get_primary_fixed);
	ClassDB::bind_method(D_METHOD("get_primary_fixed_dim"), &ColorScheme::get_primary_fixed_dim);
	ClassDB::bind_method(D_METHOD("get_on_primary_fixed"), &ColorScheme::get_on_primary_fixed);
	ClassDB::bind_method(D_METHOD("get_on_primary_fixed_variant"), &ColorScheme::get_on_primary_fixed_variant);
	ClassDB::bind_method(D_METHOD("get_secondary_fixed"), &ColorScheme::get_secondary_fixed);
	ClassDB::bind_method(D_METHOD("get_secondary_fixed_dim"), &ColorScheme::get_secondary_fixed_dim);
	ClassDB::bind_method(D_METHOD("get_on_secondary_fixed"), &ColorScheme::get_on_secondary_fixed);
	ClassDB::bind_method(D_METHOD("get_on_secondary_fixed_variant"), &ColorScheme::get_on_secondary_fixed_variant);
	ClassDB::bind_method(D_METHOD("get_tertiary_fixed"), &ColorScheme::get_tertiary_fixed);
	ClassDB::bind_method(D_METHOD("get_tertiary_fixed_dim"), &ColorScheme::get_tertiary_fixed_dim);
	ClassDB::bind_method(D_METHOD("get_on_tertiary_fixed"), &ColorScheme::get_on_tertiary_fixed);
	ClassDB::bind_method(D_METHOD("get_on_tertiary_fixed_variant"), &ColorScheme::get_on_tertiary_fixed_variant);

	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "source_color"), "set_source_color", "get_source_color");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source_texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_source_texture", "get_source_texture");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "dark"), "set_dark", "is_dark");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "contrast_level"), "set_contrast_level", "get_contrast_level");
}

ColorScheme::ColorScheme() :
		source_color(Color("#6750A4")),
		source_texture(Ref<Texture2D>()),
		dark(false),
		contrast_level(0),
		dirty(true),
		scheme(_create_scheme_content()) {
	_update_color_scheme();
	// call_deferred(SNAME("_update_color_scheme"));
}

ColorScheme::ColorScheme(Color cur_source_color, bool cur_dark, float cur_contrast_level) :
		source_color(cur_source_color),
		source_texture(Ref<Texture2D>()),
		dark(cur_dark),
		contrast_level(cur_contrast_level),
		dirty(true),
		scheme(_create_scheme_content()) {
	_update_color_scheme();
	// call_deferred(SNAME("_update_color_scheme"));
}

ColorScheme::ColorScheme(const Ref<Texture2D> &cur_source_texture, bool cur_dark, float cur_contrast_level) :
		source_color(Color()),
		source_texture(cur_source_texture),
		dark(cur_dark),
		contrast_level(cur_contrast_level),
		dirty(true),
		scheme(_create_scheme_content()) {
	_update_color_scheme();
	// call_deferred(SNAME("_update_color_scheme"));
}

ColorScheme::~ColorScheme() {
}

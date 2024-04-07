#include "color_role.h"
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

#include "color_role.h"
#include "color_scheme.h"

ColorRole::ColorRole() {
	dirty = true;
	_update_color();
}

ColorRole::ColorRole(ColorRoleEnum p_color_role_enum, Color &p_color_scale, bool p_inverted, float p_darkened_amount, float p_lightened_amount, float p_lerp_weight, Ref<ColorRole> p_lerp_color_role, Color &p_lerp_color, bool p_clamp) {
	color_role_enum = p_color_role_enum;
	color_scale = p_color_scale;
	inverted = p_inverted;
	darkened_amount = p_darkened_amount;
	lightened_amount = p_lightened_amount;
	lerp_weight = p_lerp_weight;
	lerp_color_role = p_lerp_color_role;
	lerp_color = p_lerp_color;
	clamp = p_clamp;
	dirty = true;
	_update_color();
}

ColorRole::~ColorRole() {
}

Color ColorRole::get_color(const Ref<ColorScheme> &p_color_scheme) {
	if (this == nullptr) {
		return Color(1, 1, 1, 1);
	}
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
	if(clamp){
		base_color = base_color.clamp();
	}
	return base_color;
}

void ColorRole::set_color_role_enum(ColorRoleEnum p_color_role_enum) {
	color_role_enum = p_color_role_enum;
	dirty = true;
	_update_color();
}

ColorRoleEnum ColorRole::get_color_role_enum() const {
	if (this == nullptr) {
		return ColorRoleEnum::STATIC_ONE;
	}

	return color_role_enum;
}

void ColorRole::set_color_scale(const Color &p_color_scale) {
	color_scale = p_color_scale;
	dirty = true;
	_update_color();
}

Color ColorRole::get_color_scale() const {
	return color_scale;
}

void ColorRole::set_inverted(bool p_inverted) {
	inverted = p_inverted;
	dirty = true;
	_update_color();
}

bool ColorRole::get_inverted() const {
	return inverted;
}

void ColorRole::set_clamp(bool p_clamp) {
	clamp = p_clamp;
	dirty = true;
	_update_color();
}

bool ColorRole::get_clamp() const {
	return clamp;
}


void ColorRole::set_darkened_amount(float p_darkened_amount) {
	darkened_amount = p_darkened_amount;
	dirty = true;
	_update_color();
}

float ColorRole::get_darkened_amount() const {
	return darkened_amount;
}

void ColorRole::set_lightened_amount(float p_lightened_amount) {
	lightened_amount = p_lightened_amount;
	dirty = true;
	_update_color();
}

float ColorRole::get_lightened_amount() const {
	return lightened_amount;
}

void ColorRole::set_lerp_weight(float p_lerp_weight) {
	lerp_weight = p_lerp_weight;
	dirty = true;
	_update_color();
}

float ColorRole::get_lerp_weight() const {
	return lerp_weight;
}

void ColorRole::set_lerp_color_role(const Ref<ColorRole> &p_lerp_color_role) {
	lerp_color_role = p_lerp_color_role;
	dirty = true;
	_update_color();
}

Ref<ColorRole> ColorRole::get_lerp_color_role() const {
	return lerp_color_role;
}

void ColorRole::set_lerp_color(const Color &p_lerp_color) {
	lerp_color = p_lerp_color;

	dirty = true;
	_update_color();
}

Color ColorRole::get_lerp_color() const {
	return lerp_color;
}

void ColorRole::_update_color() {
	if (dirty == false) {
		return;
	}
	emit_signal("updated_color");

	notify_property_list_changed();
	emit_changed();

	dirty = false;
}

void ColorRole::_bind_methods() {
	ADD_SIGNAL(MethodInfo("updated_color"));

	ClassDB::bind_method(D_METHOD("set_color_role_enum", "color_role_enum"), &ColorRole::set_color_role_enum);
	ClassDB::bind_method(D_METHOD("get_color_role_enum"), &ColorRole::get_color_role_enum);
	ClassDB::bind_method(D_METHOD("set_color_scale", "color_scale"), &ColorRole::set_color_scale);
	ClassDB::bind_method(D_METHOD("get_color_scale"), &ColorRole::get_color_scale);

	ClassDB::bind_method(D_METHOD("set_inverted", "inverted"), &ColorRole::set_inverted);
	ClassDB::bind_method(D_METHOD("get_inverted"), &ColorRole::get_inverted);

	ClassDB::bind_method(D_METHOD("set_darkened_amount", "darkened_amount"), &ColorRole::set_darkened_amount);
	ClassDB::bind_method(D_METHOD("get_darkened_amount"), &ColorRole::get_darkened_amount);
	ClassDB::bind_method(D_METHOD("set_lightened_amount", "lightened_amount"), &ColorRole::set_lightened_amount);
	ClassDB::bind_method(D_METHOD("get_lightened_amount"), &ColorRole::get_lightened_amount);

	ClassDB::bind_method(D_METHOD("set_lerp_weight", "lerp_weight"), &ColorRole::set_lerp_weight);
	ClassDB::bind_method(D_METHOD("get_lerp_weight"), &ColorRole::get_lerp_weight);
	ClassDB::bind_method(D_METHOD("set_lerp_color_role", "lerp_color_role"), &ColorRole::set_lerp_color_role);
	ClassDB::bind_method(D_METHOD("get_lerp_color_role"), &ColorRole::get_lerp_color_role);
	ClassDB::bind_method(D_METHOD("set_lerp_color", "lerp_color"), &ColorRole::set_lerp_color);
	ClassDB::bind_method(D_METHOD("get_lerp_color"), &ColorRole::get_lerp_color);

	ClassDB::bind_method(D_METHOD("set_clamp", "clamp"), &ColorRole::set_clamp);
	ClassDB::bind_method(D_METHOD("get_clamp"), &ColorRole::get_clamp);

	ClassDB::bind_method(D_METHOD("get_color", "color_scheme"), &ColorRole::get_color);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "color_role_enum", PROPERTY_HINT_ENUM, color_role_enum_hint), "set_color_role_enum", "get_color_role_enum");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color_scale"), "set_color_scale", "get_color_scale");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "inverted"), "set_inverted", "get_inverted");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "darkened_amount"), "set_darkened_amount", "get_darkened_amount");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lightened_amount"), "set_lightened_amount", "get_lightened_amount");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lerp_weight"), "set_lerp_weight", "get_lerp_weight");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "lerp_color_role"), "set_lerp_color_role", "get_lerp_color_role");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "lerp_color"), "set_lerp_color", "get_lerp_color");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "clamp"), "set_clamp", "get_clamp");
}

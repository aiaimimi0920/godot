/**************************************************************************/
/*  color_rect.cpp                                                        */
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

#include "color_rect.h"
#include "scene/theme/theme_db.h"

void ColorRect::set_color(const Color &p_color) {
	if (color == p_color) {
		return;
	}
	color = p_color;
	queue_redraw();
}

Color ColorRect::get_color() const {
	return color;
}

void ColorRect::set_color_scheme(const Ref<ColorScheme> &p_color_scheme) {
	if (color_scheme.is_valid()) {
		color_scheme->disconnect_changed(callable_mp(this, &ColorRect::_update_color));
	}
	color_scheme = p_color_scheme;
	if (color_scheme.is_valid()) {
		color_scheme->connect_changed(callable_mp(this, &ColorRect::_update_color), CONNECT_REFERENCE_COUNTED);
	}
	_update_color();
}

Ref<ColorScheme> ColorRect::get_color_scheme() const {
	return color_scheme;
}

void ColorRect::set_color_role(const ColorRole p_color_role) {
	color_role = p_color_role;
	_update_color();
}

ColorRole ColorRect::get_color_role() const {
	return color_role;
}

void ColorRect::_update_color() {
	if(color_role.color_role_enum!=ColorRoleEnum::STATIC_COLOR){
		if (color_scheme.is_valid()) {
			const Color target_color = color_scheme->get_color(color_role);
			if (target_color != color) {
				set_color(target_color);
			}
		} else {
			if (theme_cache.default_color_scheme.is_valid()) {
				const Color target_color = theme_cache.default_color_scheme->get_color(color_role);
				if (target_color != color) {
					set_color(target_color);
				}
			}
		}
	}
}

void ColorRect::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			draw_rect(Rect2(Point2(), get_size()), color);
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			_update_color();
		} break;
	}
}

void ColorRect::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_color", "color"), &ColorRect::set_color);
	ClassDB::bind_method(D_METHOD("get_color"), &ColorRect::get_color);
	ClassDB::bind_method(D_METHOD("set_color_scheme", "color_scheme"), &ColorRect::set_color_scheme);
	ClassDB::bind_method(D_METHOD("get_color_scheme"), &ColorRect::get_color_scheme);
	ClassDB::bind_method(D_METHOD("set_color_role", "color_role"), &ColorRect::set_color_role);
	ClassDB::bind_method(D_METHOD("get_color_role"), &ColorRect::get_color_role);

	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "color_scheme"), "set_color_scheme", "get_color_scheme");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "color_role", PROPERTY_HINT_ENUM, color_role_hint), "set_color_role", "get_color_role");

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, ColorRect, default_color_scheme);
}

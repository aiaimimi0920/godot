/**************************************************************************/
/*  separator.cpp                                                         */
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

#include "separator.h"

#include "scene/theme/theme_db.h"

Size2 Separator::get_minimum_size() const {
	Size2 ms(3, 3);
	if (orientation == VERTICAL) {
		ms.x = theme_cache.separation;
	} else { // HORIZONTAL
		ms.y = theme_cache.separation;
	}
	return ms;
}

void Separator::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			Size2i size = get_size();
			Ref<StyleBox> style = _get_current_default_stylebox_with_state(State::NormalNoneLTR);
			if (style.is_valid()) {
				Size2i ssize = style->get_minimum_size();
				if (orientation == VERTICAL) {
					style->draw(get_canvas_item(), Rect2((size.x - ssize.x) / 2, 0, ssize.x, size.y));
				} else {
					style->draw(get_canvas_item(), Rect2(0, (size.y - ssize.y) / 2, size.x, ssize.y));
				}
			}
		} break;
	}
}

Ref<StyleBox> Separator::_get_current_default_stylebox_with_state(State p_state) const {
	Ref<StyleBox> style;

	for (const State &E : theme_cache.separator_style.get_search_order(p_state)) {
		if (has_theme_stylebox(theme_cache.separator_style.get_state_data_name(E))) {
			style = theme_cache.separator_style.get_data(E);
			break;
		}
	}
	return style;
}

bool Separator::_has_current_default_stylebox() const {
	State cur_state = get_current_state();
	for (const State &E : theme_cache.separator_style.get_search_order(cur_state)) {
		if (has_theme_stylebox(theme_cache.separator_style.get_state_data_name(E))) {
			return true;
		}
	}
	return false;
}

Ref<StyleBox> Separator::_get_current_default_stylebox() const {
	State cur_state = get_current_state();
	Ref<StyleBox> style;
	style = _get_current_default_stylebox_with_state(cur_state);

	return style;
}

void Separator::_bind_methods() {
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, Separator, separation);
	BIND_THEME_ITEM_CUSTOM_MULTI(Theme::DATA_TYPE_STYLEBOX, Separator, separator_style, separator);
}

Separator::Separator() {
}

Separator::~Separator() {
}

HSeparator::HSeparator() {
	orientation = HORIZONTAL;
}

VSeparator::VSeparator() {
	orientation = VERTICAL;
}

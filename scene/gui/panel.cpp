/**************************************************************************/
/*  panel.cpp                                                             */
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

#include "panel.h"
#include "scene/theme/theme_db.h"

void Panel::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			RID ci = get_canvas_item();
			Ref<StyleBox> style = _get_current_default_stylebox();
			if(style.is_valid()){
				style->draw(ci, Rect2(Point2(), get_size()));
			}
		} break;
	}
}
bool Panel::_has_current_default_stylebox() const {
	State cur_state = get_current_state();
	for(const State &E : theme_cache.panel_style.get_search_order(cur_state)){
		if(has_theme_stylebox(theme_cache.panel_style.get_state_data_name(E))){
			return true;
		}
	}
	return false;
}

Ref<StyleBox> Panel::_get_current_default_stylebox() const {
	State cur_state = get_current_state();
	Ref<StyleBox> style;
	ThemeIntData cur_theme_data; 
	cur_theme_data.set_data_name("panel");
	for(const State &E : theme_cache.panel_style.get_search_order(cur_state)){
		if(has_theme_stylebox(cur_theme_data.get_state_data_name(E))){
			style = theme_cache.panel_style.get_data(E);
			break;
		}
	}
	return style;
}

void Panel::_bind_methods() {
	BIND_THEME_ITEM_CUSTOM_MULTI(Theme::DATA_TYPE_STYLEBOX, Panel, panel_style, panel);
}

Panel::Panel() {
	// Has visible stylebox, so stop by default.
	set_mouse_filter(MOUSE_FILTER_STOP);
}

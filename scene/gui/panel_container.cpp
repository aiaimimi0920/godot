/**************************************************************************/
/*  panel_container.cpp                                                   */
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

#include "panel_container.h"

#include "scene/theme/theme_db.h"

Size2 PanelContainer::get_minimum_size() const {
	Size2 ms;
	for (int i = 0; i < get_child_count(); i++) {
		Control *c = Object::cast_to<Control>(get_child(i));
		if (!c || !c->is_visible()) {
			continue;
		}
		if (c->is_set_as_top_level()) {
			continue;
		}

		Size2 minsize = c->get_combined_minimum_size();
		ms.width = MAX(ms.width, minsize.width);
		ms.height = MAX(ms.height, minsize.height);
	}

	Ref<StyleBox> panel_style = _get_current_default_stylebox_with_state(State::NormalNoneLTR);
	if (panel_style.is_valid()) {
		ms += panel_style->get_minimum_size();
	}
	return ms;
}

Vector<int> PanelContainer::get_allowed_size_flags_horizontal() const {
	Vector<int> flags;
	flags.append(SIZE_FILL);
	flags.append(SIZE_SHRINK_BEGIN);
	flags.append(SIZE_SHRINK_CENTER);
	flags.append(SIZE_SHRINK_END);
	return flags;
}

Vector<int> PanelContainer::get_allowed_size_flags_vertical() const {
	Vector<int> flags;
	flags.append(SIZE_FILL);
	flags.append(SIZE_SHRINK_BEGIN);
	flags.append(SIZE_SHRINK_CENTER);
	flags.append(SIZE_SHRINK_END);
	return flags;
}

void PanelContainer::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			Ref<StyleBox> panel_style = _get_current_default_stylebox();
			RID ci = get_canvas_item();
			if(panel_style.is_valid()){
				panel_style->draw(ci, Rect2(Point2(), get_size()));
			}
			
		} break;

		case NOTIFICATION_SORT_CHILDREN: {
			Size2 size = get_size();
			Point2 ofs;
			Ref<StyleBox> panel_style = _get_current_default_stylebox();
			if (panel_style.is_valid()) {
				size -= panel_style->get_minimum_size();
				ofs += panel_style->get_offset();
			}

			for (int i = 0; i < get_child_count(); i++) {
				Control *c = Object::cast_to<Control>(get_child(i));
				if (!c || !c->is_visible_in_tree()) {
					continue;
				}
				if (c->is_set_as_top_level()) {
					continue;
				}

				fit_child_in_rect(c, Rect2(ofs, size));
			}
		} break;
	}
}

bool PanelContainer::_has_current_default_stylebox_with_state(State p_state) const {
	for (const State &E : theme_cache.panel_style.get_search_order(p_state)) {
		if (has_theme_stylebox(theme_cache.panel_style.get_state_data_name(E))) {
			return true;
		}
	}
return false;
}


bool PanelContainer::_has_current_default_stylebox() const {
	State cur_state = get_current_state();
	return _has_current_default_stylebox_with_state(cur_state);
}

Ref<StyleBox> PanelContainer::_get_current_default_stylebox_with_state(State p_state) const {
	Ref<StyleBox> style;
	ThemeIntData cur_theme_data; 
	cur_theme_data.set_data_name("panel");
	for (const State &E : theme_cache.panel_style.get_search_order(p_state)) {
		if (has_theme_stylebox(cur_theme_data.get_state_data_name(E))) {
			style = theme_cache.panel_style.get_data(E);
			break; 
		}
	}
	return style;
}

Ref<StyleBox> PanelContainer::_get_current_default_stylebox() const {
	State cur_state = get_current_state();
	Ref<StyleBox> style;
	style = _get_current_default_stylebox_with_state(cur_state);
	return style;
}


void PanelContainer::_bind_methods() {
	BIND_THEME_ITEM_CUSTOM_MULTI(Theme::DATA_TYPE_STYLEBOX, PanelContainer, panel_style, panel);
}

PanelContainer::PanelContainer() {
	// Has visible stylebox, so stop by default.
	set_mouse_filter(MOUSE_FILTER_STOP);
}

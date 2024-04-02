/**************************************************************************/
/*  check_box.cpp                                                         */
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

#include "check_box.h"

#include "core/string/translation.h"
#include "scene/theme/theme_db.h"
#include "servers/rendering_server.h"

Size2 CheckBox::get_icon_size() const {
	Size2 tex_size = Size2(0, 0);

	Ref<Texture2D> checked = _get_current_check_icon_with_state(State::NormalCheckedLTR);
	Ref<Texture2D> unchecked = _get_current_check_icon_with_state(State::NormalUncheckedLTR);
	Ref<Texture2D> radio_checked = _get_current_radio_check_icon_with_state(State::NormalCheckedLTR);
	Ref<Texture2D> radio_unchecked = _get_current_radio_check_icon_with_state(State::NormalUncheckedLTR);

	Ref<Texture2D> checked_disabled = _get_current_check_icon_with_state(State::DisabledCheckedLTR);
	Ref<Texture2D> unchecked_disabled = _get_current_check_icon_with_state(State::DisabledUncheckedLTR);
	Ref<Texture2D> radio_checked_disabled = _get_current_radio_check_icon_with_state(State::DisabledCheckedLTR);
	Ref<Texture2D> radio_unchecked_disabled = _get_current_radio_check_icon_with_state(State::DisabledUncheckedLTR);

	if (!checked.is_null()) {
		tex_size = Size2(checked->get_width(), checked->get_height());
	}
	if (!unchecked.is_null()) {
		tex_size = Size2(MAX(tex_size.width, unchecked->get_width()), MAX(tex_size.height, unchecked->get_height()));
	}
	if (!radio_checked.is_null()) {
		tex_size = Size2(MAX(tex_size.width, radio_checked->get_width()), MAX(tex_size.height, radio_checked->get_height()));
	}
	if (!radio_unchecked.is_null()) {
		tex_size = Size2(MAX(tex_size.width, radio_unchecked->get_width()), MAX(tex_size.height, radio_unchecked->get_height()));
	}
	if (!checked_disabled.is_null()) {
		tex_size = Size2(MAX(tex_size.width, checked_disabled->get_width()), MAX(tex_size.height, checked_disabled->get_height()));
	}
	if (!unchecked_disabled.is_null()) {
		tex_size = Size2(MAX(tex_size.width, unchecked_disabled->get_width()), MAX(tex_size.height, unchecked_disabled->get_height()));
	}
	if (!radio_checked_disabled.is_null()) {
		tex_size = Size2(MAX(tex_size.width, radio_checked_disabled->get_width()), MAX(tex_size.height, radio_checked_disabled->get_height()));
	}
	if (!radio_unchecked_disabled.is_null()) {
		tex_size = Size2(MAX(tex_size.width, radio_unchecked_disabled->get_width()), MAX(tex_size.height, radio_unchecked_disabled->get_height()));
	}
	if (tex_size == Size2(0, 0)) {
		tex_size = Size2(theme_cache.text_icon_font_size, theme_cache.text_icon_font_size);

		if (theme_cache.icon_max_width > 0) {
			tex_size = Size2(MIN(tex_size.width, theme_cache.icon_max_width), MIN(tex_size.width, theme_cache.icon_max_width));
		}
	}
	return tex_size;
}

Size2 CheckBox::get_minimum_size() const {
	Size2 minsize = Button::get_minimum_size();
	const Size2 tex_size = get_icon_size();
	if (tex_size.width > 0 || tex_size.height > 0) {
		Ref<StyleBox> style = _get_current_default_stylebox();
		Size2 padding = Size2(0, 0);
		if (style.is_valid()) {
			padding = style->get_minimum_size();
		}
		Size2 content_size = minsize - padding;
		if (content_size.width > 0 && tex_size.width > 0) {
			content_size.width += MAX(0, theme_cache.h_separation);
		}
		content_size.width += tex_size.width;
		content_size.height = MAX(content_size.height, tex_size.height);
		minsize = content_size + padding;
	}
	return minsize;
}

void CheckBox::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_THEME_CHANGED: {
			update_minimum_size();
			queue_redraw();
		} break;
		case NOTIFICATION_LAYOUT_DIRECTION_CHANGED:
		case NOTIFICATION_TRANSLATION_CHANGED: {
			if (is_layout_rtl()) {
				_set_internal_margin(SIDE_LEFT, 0.f);
				_set_internal_margin(SIDE_RIGHT, get_icon_size().width);
			} else {
				_set_internal_margin(SIDE_LEFT, get_icon_size().width);
				_set_internal_margin(SIDE_RIGHT, 0.f);
			}
			update_minimum_size();
			queue_redraw();
		} break;

		case NOTIFICATION_DRAW: {
			RID ci = get_canvas_item();

			Ref<Texture2D> cur_tex;

			if (is_radio()) {
				cur_tex = _get_current_radio_check_icon();
			} else {
				cur_tex = _get_current_check_icon();
			}

			Vector2 ofs;
			Ref<StyleBox> normal_style = _get_current_default_stylebox_with_state(State::NormalNoneLTR);
			if (is_layout_rtl()) {
				ofs.x = get_size().x - normal_style->get_margin(SIDE_RIGHT) - get_icon_size().width;
			} else {
				ofs.x = normal_style->get_margin(SIDE_LEFT);
			}
			ofs.y = int((get_size().height - get_icon_size().height) / 2) + theme_cache.check_v_offset;

			String cur_text_icon;
			if (is_radio()) {
				cur_text_icon = _get_current_text_radio_check_icon();
			} else {
				cur_text_icon = _get_current_text_check_icon();
			}
			
			Color check_icon_color = _get_current_check_icon_color();

			Ref<Font> font = theme_cache.text_icon_font;

			if (cur_text_icon != "" && !font->get_path().get_file().get_basename().is_empty()) {
				Color text_icon_font_color = _get_current_text_check_icon_color();

				Size2 cur_size = get_icon_size();
				if (!cur_text_icon.is_empty()) {
					text_icon_buf->clear();
					text_icon_buf->set_width(cur_size.width);

					cur_text_icon = _get_trans_text(cur_text_icon);

					text_icon_buf->add_string(cur_text_icon, font, cur_size.width, "");
					text_icon_buf->draw(ci, ofs, text_icon_font_color);
				}
			} else {
				cur_tex->draw(ci, ofs, check_icon_color);
			}
		} break;
	}
}

bool CheckBox::is_radio() {
	return get_button_group().is_valid();
}

bool CheckBox::_has_current_check_icon_with_state(State p_state) const{
	for (const State &E : theme_cache.check_icon.get_search_order(p_state)) {
		if (has_theme_icon(theme_cache.check_icon.get_state_data_name(E))) {
			return true;
		}
	}
	return false;
}

bool CheckBox::_has_current_check_icon() const {
	State p_state = get_current_state_with_focus();
	return _has_current_check_icon_with_state(p_state);
}

Ref<Texture2D> CheckBox::_get_current_check_icon_with_state(State p_state) const {
	Ref<Texture2D> cur_check_icon;
	for (const State &E : theme_cache.check_icon.get_search_order(p_state)) {
		if (has_theme_icon(theme_cache.check_icon.get_state_data_name(E))) {
			cur_check_icon = theme_cache.check_icon.get_data(E);
			break;
		}
	}
	return cur_check_icon;
}

Ref<Texture2D> CheckBox::_get_current_check_icon() const {
	State p_state = get_current_state_with_focus();
	Ref<Texture2D> cur_check_icon;
	cur_check_icon = _get_current_check_icon_with_state(p_state);
	return cur_check_icon;
}

bool CheckBox::_has_current_radio_check_icon_with_state(State p_state) const {
	for (const State &E : theme_cache.radio_check_icon.get_search_order(p_state)) {
		if (has_theme_icon(theme_cache.radio_check_icon.get_state_data_name(E))) {
			return true;
		}
	}
	return false;
}


bool CheckBox::_has_current_radio_check_icon() const {
	State p_state = get_current_state_with_focus();
	return _has_current_radio_check_icon_with_state(p_state);
}

Ref<Texture2D> CheckBox::_get_current_radio_check_icon_with_state(State p_state) const {
	Ref<Texture2D> cur_radio_check_icon;
	for (const State &E : theme_cache.radio_check_icon.get_search_order(p_state)) {
		if (has_theme_icon(theme_cache.radio_check_icon.get_state_data_name(E))) {
			cur_radio_check_icon = theme_cache.radio_check_icon.get_data(E);
			break;
		}
	}
	return cur_radio_check_icon;
}


Ref<Texture2D> CheckBox::_get_current_radio_check_icon() const {
	State p_state = get_current_state_with_focus();
	Ref<Texture2D> cur_radio_check_icon;
	cur_radio_check_icon = _get_current_radio_check_icon_with_state(p_state);
	return cur_radio_check_icon;
}

bool CheckBox::_has_current_check_icon_color() const {
	State cur_state = get_current_state_with_focus();
	for (const State &E : theme_cache.check_icon_color.get_search_order(cur_state)) {
		if (has_theme_color(theme_cache.check_icon_color.get_state_data_name(E))) {
			return true;
		}
	}
	return false;
}

Color CheckBox::_get_current_check_icon_color() const {
	State cur_state = get_current_state_with_focus();
	Color cur_check_icon_color;

	for (const State &E : theme_cache.check_icon_color.get_search_order(cur_state)) {
		if (has_theme_color(theme_cache.check_icon_color.get_state_data_name(E))) {
			cur_check_icon_color = theme_cache.check_icon_color.get_data(E);
			break;
		}
	}
	return cur_check_icon_color;
}

bool CheckBox::_has_current_text_check_icon() const {
	State cur_state = get_current_state_with_focus();
	for (const State &E : theme_cache.text_check_icon.get_search_order(cur_state)) {
		if (has_theme_str(theme_cache.text_check_icon.get_state_data_name(E))) {
			return true;
		}
	}
	return false;
}

String CheckBox::_get_current_text_check_icon() const {
	State cur_state = get_current_state_with_focus();
	String cur_text_check_icon;

	for (const State &E : theme_cache.text_check_icon.get_search_order(cur_state)) {
		if (has_theme_str(theme_cache.text_check_icon.get_state_data_name(E))) {
			cur_text_check_icon = theme_cache.text_check_icon.get_data(E);
			break;
		}
	}
	return cur_text_check_icon;
}

bool CheckBox::_has_current_text_radio_check_icon() const {
	State cur_state = get_current_state_with_focus();
	for (const State &E : theme_cache.text_radio_check_icon.get_search_order(cur_state)) {
		if (has_theme_str(theme_cache.text_radio_check_icon.get_state_data_name(E))) {
			return true;
		}
	}
	return false;
}

String CheckBox::_get_current_text_radio_check_icon() const {
	State cur_state = get_current_state_with_focus();
	String cur_text_radio_check_icon;

	for (const State &E : theme_cache.text_radio_check_icon.get_search_order(cur_state)) {
		if (has_theme_str(theme_cache.text_radio_check_icon.get_state_data_name(E))) {
			cur_text_radio_check_icon = theme_cache.text_radio_check_icon.get_data(E);
			break;
		}
	}
	return cur_text_radio_check_icon;
}


bool CheckBox::_has_current_text_check_icon_color() const {
	State cur_state = get_current_state_with_focus();
	for (const State &E : theme_cache.text_check_icon_color.get_search_order(cur_state)) {
		if (has_theme_color(theme_cache.text_check_icon_color.get_state_data_name(E))) {
			return true;
		}
	}
	return false;
}

Color CheckBox::_get_current_text_check_icon_color() const {
	State cur_state = get_current_state_with_focus();
	Color cur_text_check_icon_color;

	for (const State &E : theme_cache.text_check_icon_color.get_search_order(cur_state)) {
		if (has_theme_color(theme_cache.text_check_icon_color.get_state_data_name(E))) {
			cur_text_check_icon_color = theme_cache.text_check_icon_color.get_data(E);
			break;
		}
	}
	return cur_text_check_icon_color;
}

void CheckBox::_bind_methods() {
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, CheckBox, h_separation);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, CheckBox, check_v_offset);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, CheckBox, icon_max_width);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, CheckBox, text_icon_font_size);

	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, CheckBox, text_icon_font);

	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_ICON, CheckBox, check_icon);
	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_ICON, CheckBox, radio_check_icon);
	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_COLOR_ROLE, CheckBox, check_icon_color_role);
	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_COLOR, CheckBox, check_icon_color);

	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_STR, CheckBox, text_check_icon);
	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_STR, CheckBox, text_radio_check_icon);
	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_COLOR_ROLE, CheckBox, text_check_icon_color_role);
	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_COLOR, CheckBox, text_check_icon_color);
}

CheckBox::CheckBox(const String &p_text) :
		Button(p_text) {
	set_toggle_mode(true);

	set_text_alignment(HORIZONTAL_ALIGNMENT_LEFT);

	if (is_layout_rtl()) {
		_set_internal_margin(SIDE_RIGHT, get_icon_size().width);
	} else {
		_set_internal_margin(SIDE_LEFT, get_icon_size().width);
	}

	text_icon_buf.instantiate();
	text_icon_buf->set_break_flags(TextServer::BREAK_MANDATORY | TextServer::BREAK_TRIM_EDGE_SPACES);
}

CheckBox::~CheckBox() {
}

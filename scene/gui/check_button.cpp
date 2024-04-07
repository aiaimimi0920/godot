/**************************************************************************/
/*  check_button.cpp                                                      */
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

#include "check_button.h"

#include "scene/theme/theme_db.h"
#include "servers/rendering_server.h"

Size2 CheckButton::get_icon_size() const {
	Ref<Texture2D> on_tex;
	Ref<Texture2D> off_tex;

	if (is_layout_rtl()) {
		if (is_disabled()) {
			on_tex = theme_cache.checked_disabled_mirrored;
			off_tex = theme_cache.unchecked_disabled_mirrored;
		} else {
			on_tex = theme_cache.checked_mirrored;
			off_tex = theme_cache.unchecked_mirrored;
		}
	} else {
		if (is_disabled()) {
			on_tex = theme_cache.checked_disabled;
			off_tex = theme_cache.unchecked_disabled;
		} else {
			on_tex = theme_cache.checked;
			off_tex = theme_cache.unchecked;
		}
	}

	Size2 tex_size = Size2(0, 0);
	if (!on_tex.is_null()) {
		tex_size = on_tex->get_size();
	}
	if (!off_tex.is_null()) {
		tex_size = tex_size.max(off_tex->get_size());
	}

	if (!theme_cache.text_checked.is_empty() || !theme_cache.text_unchecked.is_empty() ||!theme_cache.text_checked_disabled.is_empty() || !theme_cache.text_unchecked_disabled.is_empty()||!theme_cache.text_checked_mirrored.is_empty()||!theme_cache.text_unchecked_mirrored.is_empty()||!theme_cache.text_checked_disabled_mirrored.is_empty()||!theme_cache.text_unchecked_disabled_mirrored.is_empty()) {
		tex_size = Size2(theme_cache.text_icon_font_size, theme_cache.text_icon_font_size);
		if(theme_cache.icon_max_width>0){
			tex_size = tex_size.min(Size2(theme_cache.icon_max_width, theme_cache.icon_max_width));
		}
	}

	return tex_size;
}

Size2 CheckButton::get_minimum_size() const {
	Size2 minsize = Button::get_minimum_size();
	const Size2 tex_size = get_icon_size();
	if (tex_size.width > 0 || tex_size.height > 0) {
		const Size2 padding = _get_current_stylebox()->get_minimum_size();
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

void CheckButton::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_THEME_CHANGED:
		case NOTIFICATION_LAYOUT_DIRECTION_CHANGED:
		case NOTIFICATION_TRANSLATION_CHANGED: {
			if (is_layout_rtl()) {
				_set_internal_margin(SIDE_LEFT, get_icon_size().width);
				_set_internal_margin(SIDE_RIGHT, 0.f);
			} else {
				_set_internal_margin(SIDE_LEFT, 0.f);
				_set_internal_margin(SIDE_RIGHT, get_icon_size().width);
			}
			update_xl_text();
		} break;

		case NOTIFICATION_DRAW: {
			RID ci = get_canvas_item();
			bool rtl = is_layout_rtl();

			Ref<Texture2D> on_tex;
			Ref<Texture2D> off_tex;

			String on_text;
			String off_text;

			Color on_text_color;
			Color off_text_color;

			if (rtl) {
				if (is_disabled()) {
					on_tex = theme_cache.checked_disabled_mirrored;
					off_tex = theme_cache.unchecked_disabled_mirrored;
					on_text = xl_text_checked_disabled_mirrored;
					off_tex = xl_text_unchecked_disabled_mirrored;
					on_text_color = theme_cache.text_checked_disabled_color;
					off_text_color = theme_cache.text_unchecked_disabled_color;
				} else {
					on_tex = theme_cache.checked_mirrored;
					off_tex = theme_cache.unchecked_mirrored;
					on_text = xl_text_checked_mirrored;
					off_text = xl_text_unchecked_mirrored;
					on_text_color = theme_cache.text_checked_color;
					off_text_color = theme_cache.text_unchecked_color;
				}
			} else {
				if (is_disabled()) {
					on_tex = theme_cache.checked_disabled;
					off_tex = theme_cache.unchecked_disabled;
					on_text = xl_text_checked_disabled;
					off_text = xl_text_unchecked_disabled;
					on_text_color = theme_cache.text_checked_disabled_color;
					off_text_color = theme_cache.text_unchecked_disabled_color;
				} else {
					on_tex = theme_cache.checked;
					off_tex = theme_cache.unchecked;
					on_text = xl_text_checked;
					off_text = xl_text_unchecked;
					on_text_color = theme_cache.text_checked_color;
					off_text_color = theme_cache.text_unchecked_color;
				}
			}

			Vector2 ofs;
			Size2 tex_size = get_icon_size();

			if (rtl) {
				ofs.x = theme_cache.normal_style->get_margin(SIDE_LEFT);
			} else {
				ofs.x = get_size().width - (tex_size.width + theme_cache.normal_style->get_margin(SIDE_RIGHT));
			}
			ofs.y = (get_size().height - tex_size.height) / 2 + theme_cache.check_v_offset;

			if (is_pressed()) {
				if(on_text.is_empty()){
					on_tex->draw(ci, ofs);
				}else{
					Ref<Font> text_icon_font = theme_cache.text_icon_font;
					Size2 cur_size = get_icon_size();
					check_button_text_icon_buf->clear();
					check_button_text_icon_buf->set_width(cur_size.width);
					check_button_text_icon_buf->add_string(on_text, text_icon_font, cur_size.width, "");
					check_button_text_icon_buf->draw(ci, ofs, on_text_color);
				}
				
			} else {
				if(off_text.is_empty()){
					off_tex->draw(ci, ofs);
				}else{
					Ref<Font> text_icon_font = theme_cache.text_icon_font;
					Size2 cur_size = get_icon_size();
					check_button_text_icon_buf->clear();
					check_button_text_icon_buf->set_width(cur_size.width);
					check_button_text_icon_buf->add_string(off_text, text_icon_font, cur_size.width, "");
					check_button_text_icon_buf->draw(ci, ofs, off_text_color);
				}
			}
		} break;
	}
}

void CheckButton::update_xl_text(){
	xl_text_checked = _get_trans_text(theme_cache.text_checked);
	xl_text_unchecked = _get_trans_text(theme_cache.text_unchecked);
	xl_text_checked_mirrored = _get_trans_text(theme_cache.text_checked_mirrored);
	xl_text_unchecked_mirrored = _get_trans_text(theme_cache.text_unchecked_mirrored);
	xl_text_checked_disabled = _get_trans_text(theme_cache.text_checked_disabled);
	xl_text_unchecked_disabled = _get_trans_text(theme_cache.text_unchecked_disabled);
	xl_text_checked_disabled_mirrored = _get_trans_text(theme_cache.text_checked_disabled_mirrored);
	xl_text_unchecked_disabled_mirrored = _get_trans_text(theme_cache.text_unchecked_disabled_mirrored);
}

void CheckButton::_bind_methods() {
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, CheckButton, h_separation);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, CheckButton, icon_max_width);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, CheckButton, check_v_offset);
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, CheckButton, normal_style, "normal");

	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, CheckButton, text_icon_font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, CheckButton, text_icon_font_size);

	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, checked);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, unchecked);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, checked_disabled);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, unchecked_disabled);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, checked_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, unchecked_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, checked_disabled_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, unchecked_disabled_mirrored);

	BIND_THEME_ITEM(Theme::DATA_TYPE_STR, CheckButton, text_checked);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STR, CheckButton, text_unchecked);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STR, CheckButton, text_checked_disabled);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STR, CheckButton, text_unchecked_disabled);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STR, CheckButton, text_checked_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STR, CheckButton, text_unchecked_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STR, CheckButton, text_checked_disabled_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STR, CheckButton, text_unchecked_disabled_mirrored);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, text_checked_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, text_checked_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, text_unchecked_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, text_unchecked_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, text_checked_disabled_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, text_checked_disabled_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, text_unchecked_disabled_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, text_unchecked_disabled_color_role);


}

CheckButton::CheckButton(const String &p_text) :
		Button(p_text) {
	set_toggle_mode(true);

	set_text_alignment(HORIZONTAL_ALIGNMENT_LEFT);

	if (is_layout_rtl()) {
		_set_internal_margin(SIDE_LEFT, get_icon_size().width);
	} else {
		_set_internal_margin(SIDE_RIGHT, get_icon_size().width);
	}

	check_button_text_icon_buf.instantiate();
	check_button_text_icon_buf->set_break_flags(TextServer::BREAK_MANDATORY | TextServer::BREAK_TRIM_EDGE_SPACES);
}

CheckButton::~CheckButton() {
}

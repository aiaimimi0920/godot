/**************************************************************************/
/*  check_box.h                                                           */
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

#ifndef CHECK_BOX_H
#define CHECK_BOX_H

#include "scene/gui/button.h"
#include "scene/resources/text_paragraph.h"

class CheckBox : public Button {
	GDCLASS(CheckBox, Button);

	Ref<TextParagraph> text_icon_buf;

	struct ThemeCache {
		int h_separation = 0;
		int check_v_offset = 0;
		int icon_max_width;
		int text_icon_font_size;
		Ref<Font> text_icon_font;

		ThemeIconData check_icon{ "check_icon" };
		ThemeIconData radio_check_icon{ "radio_check_icon" };
		ThemeColorData check_icon_color{"check_icon_color"};
		ThemeColorRoleData check_icon_color_role{"check_icon_color_role"};

		ThemeStrData text_check_icon{"text_check_icon"};
		ThemeStrData text_radio_check_icon{"text_radio_check_icon"};
		ThemeColorData text_check_icon_color{"text_check_icon_color"};
		ThemeColorRoleData text_check_icon_color_role{"text_check_icon_color_role"};
	} theme_cache;

protected:
	Size2 get_icon_size() const;
	Size2 get_minimum_size() const override;

	void _notification(int p_what);
	static void _bind_methods();

	bool is_radio();

	bool _has_current_check_icon_with_state(State p_state) const;
	bool _has_current_check_icon() const;
	Ref<Texture2D> _get_current_check_icon_with_state(State p_state) const;
	Ref<Texture2D> _get_current_check_icon() const;

	bool _has_current_radio_check_icon_with_state(State p_state) const;
	bool _has_current_radio_check_icon() const;
	Ref<Texture2D> _get_current_radio_check_icon_with_state(State p_state) const;
	Ref<Texture2D> _get_current_radio_check_icon() const;

	bool _has_current_check_icon_color() const;
	Color _get_current_check_icon_color() const;

	bool _has_current_text_check_icon() const;
	String _get_current_text_check_icon() const;

	bool _has_current_text_radio_check_icon() const;
	String _get_current_text_radio_check_icon() const;

	bool _has_current_text_check_icon_color() const;
	Color _get_current_text_check_icon_color() const;

public:

	CheckBox(const String &p_text = String());
	~CheckBox();
};

#endif // CHECK_BOX_H

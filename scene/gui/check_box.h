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

	String code_selected_check_text_icon_normal;
	String selected_check_text_icon_normal = "checkbox-marked";
	String code_selected_check_text_icon_pressed;
	String selected_check_text_icon_pressed = "checkbox-marked";
	String code_selected_check_text_icon_hover;
	String selected_check_text_icon_hover = "checkbox-marked";
	String code_selected_check_text_icon_disabled;
	String selected_check_text_icon_disabled = "checkbox-marked";

	String code_unselected_check_text_icon_normal;
	String unselected_check_text_icon_normal = "checkbox-blank-outline";
	String code_unselected_check_text_icon_pressed;
	String unselected_check_text_icon_pressed = "checkbox-blank-outline";
	String code_unselected_check_text_icon_hover;
	String unselected_check_text_icon_hover = "checkbox-blank-outline";
	String code_unselected_check_text_icon_disabled;
	String unselected_check_text_icon_disabled = "checkbox-blank-outline";

	String code_selected_radio_check_text_icon_normal;
	String selected_radio_check_text_icon_normal = "radiobox-marked";
	String code_selected_radio_check_text_icon_pressed;
	String selected_radio_check_text_icon_pressed = "radiobox-marked";
	String code_selected_radio_check_text_icon_hover;
	String selected_radio_check_text_icon_hover = "radiobox-marked";
	String code_selected_radio_check_text_icon_disabled;
	String selected_radio_check_text_icon_disabled = "radiobox-marked";

	String code_unselected_radio_check_text_icon_normal;
	String unselected_radio_check_text_icon_normal = "radiobox-blank";
	String code_unselected_radio_check_text_icon_pressed;
	String unselected_radio_check_text_icon_pressed = "radiobox-blank";
	String code_unselected_radio_check_text_icon_hover;
	String unselected_radio_check_text_icon_hover = "radiobox-blank";
	String code_unselected_radio_check_text_icon_disabled;
	String unselected_radio_check_text_icon_disabled = "radiobox-blank";
	
	String _get_trans_text(const String &p_text_icon);


	struct ThemeCache {
		Ref<Font> text_icon_font;
		int text_icon_font_size = 0;
		int icon_max_width = 0;

		int h_separation = 0;
		int check_v_offset = 0;
		Ref<StyleBox> normal_style;

		Ref<Texture2D> checked;
		Ref<Texture2D> unchecked;
		Ref<Texture2D> radio_checked;
		Ref<Texture2D> radio_unchecked;
		Ref<Texture2D> checked_disabled;
		Ref<Texture2D> unchecked_disabled;
		Ref<Texture2D> radio_checked_disabled;
		Ref<Texture2D> radio_unchecked_disabled;

		Ref<StyleBox> selected_check_hover_state_layer;
		Ref<StyleBox> selected_check_hover_state_layer_mirrored;
		Ref<StyleBox> selected_check_pressed_state_layer;
		Ref<StyleBox> selected_check_pressed_state_layer_mirrored;
		Ref<StyleBox> selected_check_hover_pressed_state_layer;
		Ref<StyleBox> selected_check_hover_pressed_state_layer_mirrored;
		Ref<StyleBox> selected_check_focus_state_layer;

		Color selected_check_text_icon_normal_color_scale;
		Ref<ColorScheme> selected_check_text_icon_normal_color_scheme;
		ColorRole selected_check_text_icon_normal_color_role;
		Color selected_check_text_icon_normal_color;

		Color selected_check_text_icon_pressed_color_scale;
		Ref<ColorScheme> selected_check_text_icon_pressed_color_scheme;
		ColorRole selected_check_text_icon_pressed_color_role;
		Color selected_check_text_icon_pressed_color;

		Color selected_check_text_icon_hover_color_scale;
		Ref<ColorScheme> selected_check_text_icon_hover_color_scheme;
		ColorRole selected_check_text_icon_hover_color_role;
		Color selected_check_text_icon_hover_color;

		Color selected_check_text_icon_focus_color_scale;
		Ref<ColorScheme> selected_check_text_icon_focus_color_scheme;
		ColorRole selected_check_text_icon_focus_color_role;
		Color selected_check_text_icon_focus_color;

		Color selected_check_text_icon_hover_pressed_color_scale;
		Ref<ColorScheme> selected_check_text_icon_hover_pressed_color_scheme;
		ColorRole selected_check_text_icon_hover_pressed_color_role;
		Color selected_check_text_icon_hover_pressed_color;

		Color selected_check_text_icon_disabled_color_scale;
		Ref<ColorScheme> selected_check_text_icon_disabled_color_scheme;
		ColorRole selected_check_text_icon_disabled_color_role;
		Color selected_check_text_icon_disabled_color;

		Ref<StyleBox> unselected_check_hover_state_layer;
		Ref<StyleBox> unselected_check_hover_state_layer_mirrored;
		Ref<StyleBox> unselected_check_pressed_state_layer;
		Ref<StyleBox> unselected_check_pressed_state_layer_mirrored;
		Ref<StyleBox> unselected_check_hover_pressed_state_layer;
		Ref<StyleBox> unselected_check_hover_pressed_state_layer_mirrored;
		Ref<StyleBox> unselected_check_focus_state_layer;

		Color unselected_check_text_icon_normal_color_scale;
		Ref<ColorScheme> unselected_check_text_icon_normal_color_scheme;
		ColorRole unselected_check_text_icon_normal_color_role;
		Color unselected_check_text_icon_normal_color;

		Color unselected_check_text_icon_pressed_color_scale;
		Ref<ColorScheme> unselected_check_text_icon_pressed_color_scheme;
		ColorRole unselected_check_text_icon_pressed_color_role;
		Color unselected_check_text_icon_pressed_color;

		Color unselected_check_text_icon_hover_color_scale;
		Ref<ColorScheme> unselected_check_text_icon_hover_color_scheme;
		ColorRole unselected_check_text_icon_hover_color_role;
		Color unselected_check_text_icon_hover_color;

		Color unselected_check_text_icon_focus_color_scale;
		Ref<ColorScheme> unselected_check_text_icon_focus_color_scheme;
		ColorRole unselected_check_text_icon_focus_color_role;
		Color unselected_check_text_icon_focus_color;

		Color unselected_check_text_icon_hover_pressed_color_scale;
		Ref<ColorScheme> unselected_check_text_icon_hover_pressed_color_scheme;
		ColorRole unselected_check_text_icon_hover_pressed_color_role;
		Color unselected_check_text_icon_hover_pressed_color;

		Color unselected_check_text_icon_disabled_color_scale;
		Ref<ColorScheme> unselected_check_text_icon_disabled_color_scheme;
		ColorRole unselected_check_text_icon_disabled_color_role;
		Color unselected_check_text_icon_disabled_color;

	} theme_cache;

protected:
	Size2 get_icon_size() const;
	Size2 get_minimum_size() const override;

	void _notification(int p_what);
	static void _bind_methods();
	Ref<StyleBox> _get_focus_state_layer_stylebox() const;
	Ref<StyleBox> _get_current_state_layer_stylebox() const;

	bool is_radio();

public:
	void set_selected_check_text_icon_normal(const String &p_selected_check_text_icon_normal);
	String get_selected_check_text_icon_normal() const;
	void set_selected_check_text_icon_pressed(const String &p_selected_check_text_icon_pressed);
	String get_selected_check_text_icon_pressed() const;
	void set_selected_check_text_icon_hover(const String &p_selected_check_text_icon_hover);
	String get_selected_check_text_icon_hover() const;
	void set_selected_check_text_icon_disabled(const String &p_selected_check_text_icon_disabled);
	String get_selected_check_text_icon_disabled() const;

	void set_unselected_check_text_icon_normal(const String &p_unselected_check_text_icon_normal);
	String get_unselected_check_text_icon_normal() const;
	void set_unselected_check_text_icon_pressed(const String &p_unselected_check_text_icon_pressed);
	String get_unselected_check_text_icon_pressed() const;
	void set_unselected_check_text_icon_hover(const String &p_unselected_check_text_icon_hover);
	String get_unselected_check_text_icon_hover() const;
	void set_unselected_check_text_icon_disabled(const String &p_unselected_check_text_icon_disabled);
	String get_unselected_check_text_icon_disabled() const;

	void set_selected_radio_check_text_icon_normal(const String &p_selected_radio_check_text_icon_normal);
	String get_selected_radio_check_text_icon_normal() const;
	void set_selected_radio_check_text_icon_pressed(const String &p_selected_radio_check_text_icon_pressed);
	String get_selected_radio_check_text_icon_pressed() const;
	void set_selected_radio_check_text_icon_hover(const String &p_selected_radio_check_text_icon_hover);
	String get_selected_radio_check_text_icon_hover() const;
	void set_selected_radio_check_text_icon_disabled(const String &p_selected_radio_check_text_icon_disabled);
	String get_selected_radio_check_text_icon_disabled() const;

	void set_unselected_radio_check_text_icon_normal(const String &p_unselected_radio_check_text_icon_normal);
	String get_unselected_radio_check_text_icon_normal() const;
	void set_unselected_radio_check_text_icon_pressed(const String &p_unselected_radio_check_text_icon_pressed);
	String get_unselected_radio_check_text_icon_pressed() const;
	void set_unselected_radio_check_text_icon_hover(const String &p_unselected_radio_check_text_icon_hover);
	String get_unselected_radio_check_text_icon_hover() const;
	void set_unselected_radio_check_text_icon_disabled(const String &p_unselected_radio_check_text_icon_disabled);
	String get_unselected_radio_check_text_icon_disabled() const;

	CheckBox(const String &p_text = String());
	~CheckBox();
};

#endif // CHECK_BOX_H

/**************************************************************************/
/*  check_button.h                                                        */
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

#ifndef CHECK_BUTTON_H
#define CHECK_BUTTON_H

#include "scene/gui/button.h"
#include "scene/resources/text_paragraph.h"

class CheckButton : public Button {
	GDCLASS(CheckButton, Button);


	Ref<TextParagraph> text_icon_buf;
	Ref<TextParagraph> bg_text_icon_buf;

	String code_selected_check_text_icon_normal;
	String selected_check_text_icon_normal = "toggle-switch";
	String code_selected_check_text_icon_pressed;
	String selected_check_text_icon_pressed = "toggle-switch";
	String code_selected_check_text_icon_hover;
	String selected_check_text_icon_hover = "toggle-switch";
	String code_selected_check_text_icon_disabled;
	String selected_check_text_icon_disabled = "toggle-switch";

	String code_unselected_check_text_icon_normal;
	String unselected_check_text_icon_normal = "toggle-switch-off-outline";
	String code_unselected_check_text_icon_pressed;
	String unselected_check_text_icon_pressed = "toggle-switch-off-outline";
	String code_unselected_check_text_icon_hover;
	String unselected_check_text_icon_hover = "toggle-switch-off-outline";
	String code_unselected_check_text_icon_disabled;
	String unselected_check_text_icon_disabled = "toggle-switch-off-outline";

	String code_selected_bg_check_text_icon_normal;
	String selected_bg_check_text_icon_normal = "toggle-switch-outline";
	String code_selected_bg_check_text_icon_pressed;
	String selected_bg_check_text_icon_pressed = "toggle-switch-outline";
	String code_selected_bg_check_text_icon_hover;
	String selected_bg_check_text_icon_hover = "toggle-switch-outline";
	String code_selected_bg_check_text_icon_disabled;
	String selected_bg_check_text_icon_disabled = "toggle-switch-outline";

	String code_unselected_bg_check_text_icon_normal;
	String unselected_bg_check_text_icon_normal = "toggle-switch-off";
	String code_unselected_bg_check_text_icon_pressed;
	String unselected_bg_check_text_icon_pressed = "toggle-switch-off";
	String code_unselected_bg_check_text_icon_hover;
	String unselected_bg_check_text_icon_hover = "toggle-switch-off";
	String code_unselected_bg_check_text_icon_disabled;
	String unselected_bg_check_text_icon_disabled = "toggle-switch-off";

	String code_selected_check_text_icon_normal_mirrored;
	String selected_check_text_icon_normal_mirrored = "toggle-switch-off";
	String code_selected_check_text_icon_pressed_mirrored;
	String selected_check_text_icon_pressed_mirrored = "toggle-switch-off";
	String code_selected_check_text_icon_hover_mirrored;
	String selected_check_text_icon_hover_mirrored = "toggle-switch-off";
	String code_selected_check_text_icon_disabled_mirrored;
	String selected_check_text_icon_disabled_mirrored = "toggle-switch-off";

	String code_unselected_check_text_icon_normal_mirrored;
	String unselected_check_text_icon_normal_mirrored = "toggle-switch-outline";
	String code_unselected_check_text_icon_pressed_mirrored;
	String unselected_check_text_icon_pressed_mirrored = "toggle-switch-outline";
	String code_unselected_check_text_icon_hover_mirrored;
	String unselected_check_text_icon_hover_mirrored = "toggle-switch-outline";
	String code_unselected_check_text_icon_disabled_mirrored;
	String unselected_check_text_icon_disabled_mirrored = "toggle-switch-outline";

	String code_selected_bg_check_text_icon_normal_mirrored;
	String selected_bg_check_text_icon_normal_mirrored = "toggle-switch-off-outline";
	String code_selected_bg_check_text_icon_pressed_mirrored;
	String selected_bg_check_text_icon_pressed_mirrored = "toggle-switch-off-outline";
	String code_selected_bg_check_text_icon_hover_mirrored;
	String selected_bg_check_text_icon_hover_mirrored = "toggle-switch-off-outline";
	String code_selected_bg_check_text_icon_disabled_mirrored;
	String selected_bg_check_text_icon_disabled_mirrored = "toggle-switch-off-outline";

	String code_unselected_bg_check_text_icon_normal_mirrored;
	String unselected_bg_check_text_icon_normal_mirrored = "toggle-switch";
	String code_unselected_bg_check_text_icon_pressed_mirrored;
	String unselected_bg_check_text_icon_pressed_mirrored = "toggle-switch";
	String code_unselected_bg_check_text_icon_hover_mirrored;
	String unselected_bg_check_text_icon_hover_mirrored = "toggle-switch";
	String code_unselected_bg_check_text_icon_disabled_mirrored;
	String unselected_bg_check_text_icon_disabled_mirrored = "toggle-switch";


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
		Ref<Texture2D> checked_disabled;
		Ref<Texture2D> unchecked_disabled;
		Ref<Texture2D> checked_mirrored;
		Ref<Texture2D> unchecked_mirrored;
		Ref<Texture2D> checked_disabled_mirrored;
		Ref<Texture2D> unchecked_disabled_mirrored;

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

		Color selected_bg_check_text_icon_normal_color_scale;
		Ref<ColorScheme> selected_bg_check_text_icon_normal_color_scheme;
		ColorRole selected_bg_check_text_icon_normal_color_role;
		Color selected_bg_check_text_icon_normal_color;

		Color selected_bg_check_text_icon_pressed_color_scale;
		Ref<ColorScheme> selected_bg_check_text_icon_pressed_color_scheme;
		ColorRole selected_bg_check_text_icon_pressed_color_role;
		Color selected_bg_check_text_icon_pressed_color;

		Color selected_bg_check_text_icon_hover_color_scale;
		Ref<ColorScheme> selected_bg_check_text_icon_hover_color_scheme;
		ColorRole selected_bg_check_text_icon_hover_color_role;
		Color selected_bg_check_text_icon_hover_color;

		Color selected_bg_check_text_icon_focus_color_scale;
		Ref<ColorScheme> selected_bg_check_text_icon_focus_color_scheme;
		ColorRole selected_bg_check_text_icon_focus_color_role;
		Color selected_bg_check_text_icon_focus_color;

		Color selected_bg_check_text_icon_hover_pressed_color_scale;
		Ref<ColorScheme> selected_bg_check_text_icon_hover_pressed_color_scheme;
		ColorRole selected_bg_check_text_icon_hover_pressed_color_role;
		Color selected_bg_check_text_icon_hover_pressed_color;

		Color selected_bg_check_text_icon_disabled_color_scale;
		Ref<ColorScheme> selected_bg_check_text_icon_disabled_color_scheme;
		ColorRole selected_bg_check_text_icon_disabled_color_role;
		Color selected_bg_check_text_icon_disabled_color;

		Color unselected_bg_check_text_icon_normal_color_scale;
		Ref<ColorScheme> unselected_bg_check_text_icon_normal_color_scheme;
		ColorRole unselected_bg_check_text_icon_normal_color_role;
		Color unselected_bg_check_text_icon_normal_color;

		Color unselected_bg_check_text_icon_pressed_color_scale;
		Ref<ColorScheme> unselected_bg_check_text_icon_pressed_color_scheme;
		ColorRole unselected_bg_check_text_icon_pressed_color_role;
		Color unselected_bg_check_text_icon_pressed_color;

		Color unselected_bg_check_text_icon_hover_color_scale;
		Ref<ColorScheme> unselected_bg_check_text_icon_hover_color_scheme;
		ColorRole unselected_bg_check_text_icon_hover_color_role;
		Color unselected_bg_check_text_icon_hover_color;

		Color unselected_bg_check_text_icon_focus_color_scale;
		Ref<ColorScheme> unselected_bg_check_text_icon_focus_color_scheme;
		ColorRole unselected_bg_check_text_icon_focus_color_role;
		Color unselected_bg_check_text_icon_focus_color;

		Color unselected_bg_check_text_icon_hover_pressed_color_scale;
		Ref<ColorScheme> unselected_bg_check_text_icon_hover_pressed_color_scheme;
		ColorRole unselected_bg_check_text_icon_hover_pressed_color_role;
		Color unselected_bg_check_text_icon_hover_pressed_color;

		Color unselected_bg_check_text_icon_disabled_color_scale;
		Ref<ColorScheme> unselected_bg_check_text_icon_disabled_color_scheme;
		ColorRole unselected_bg_check_text_icon_disabled_color_role;
		Color unselected_bg_check_text_icon_disabled_color;

	} theme_cache;

protected:
	Size2 get_icon_size() const;
	virtual Size2 get_minimum_size() const override;

	void _notification(int p_what);
	static void _bind_methods();

	Ref<StyleBox> _get_focus_state_layer_stylebox() const;
	Ref<StyleBox> _get_current_state_layer_stylebox() const;


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

	void set_selected_bg_check_text_icon_normal(const String &p_selected_bg_check_text_icon_normal);
	String get_selected_bg_check_text_icon_normal() const;
	void set_selected_bg_check_text_icon_pressed(const String &p_selected_bg_check_text_icon_pressed);
	String get_selected_bg_check_text_icon_pressed() const;
	void set_selected_bg_check_text_icon_hover(const String &p_selected_bg_check_text_icon_hover);
	String get_selected_bg_check_text_icon_hover() const;
	void set_selected_bg_check_text_icon_disabled(const String &p_selected_bg_check_text_icon_disabled);
	String get_selected_bg_check_text_icon_disabled() const;

	void set_unselected_bg_check_text_icon_normal(const String &p_unselected_bg_check_text_icon_normal);
	String get_unselected_bg_check_text_icon_normal() const;
	void set_unselected_bg_check_text_icon_pressed(const String &p_unselected_bg_check_text_icon_pressed);
	String get_unselected_bg_check_text_icon_pressed() const;
	void set_unselected_bg_check_text_icon_hover(const String &p_unselected_bg_check_text_icon_hover);
	String get_unselected_bg_check_text_icon_hover() const;
	void set_unselected_bg_check_text_icon_disabled(const String &p_unselected_bg_check_text_icon_disabled);
	String get_unselected_bg_check_text_icon_disabled() const;

	void set_selected_check_text_icon_normal_mirrored(const String &p_selected_check_text_icon_normal);
	String get_selected_check_text_icon_normal_mirrored() const;
	void set_selected_check_text_icon_pressed_mirrored(const String &p_selected_check_text_icon_pressed);
	String get_selected_check_text_icon_pressed_mirrored() const;
	void set_selected_check_text_icon_hover_mirrored(const String &p_selected_check_text_icon_hover);
	String get_selected_check_text_icon_hover_mirrored() const;
	void set_selected_check_text_icon_disabled_mirrored(const String &p_selected_check_text_icon_disabled);
	String get_selected_check_text_icon_disabled_mirrored() const;

	void set_unselected_check_text_icon_normal_mirrored(const String &p_unselected_check_text_icon_normal);
	String get_unselected_check_text_icon_normal_mirrored() const;
	void set_unselected_check_text_icon_pressed_mirrored(const String &p_unselected_check_text_icon_pressed);
	String get_unselected_check_text_icon_pressed_mirrored() const;
	void set_unselected_check_text_icon_hover_mirrored(const String &p_unselected_check_text_icon_hover);
	String get_unselected_check_text_icon_hover_mirrored() const;
	void set_unselected_check_text_icon_disabled_mirrored(const String &p_unselected_check_text_icon_disabled);
	String get_unselected_check_text_icon_disabled_mirrored() const;

	void set_selected_bg_check_text_icon_normal_mirrored(const String &p_selected_bg_check_text_icon_normal);
	String get_selected_bg_check_text_icon_normal_mirrored() const;
	void set_selected_bg_check_text_icon_pressed_mirrored(const String &p_selected_bg_check_text_icon_pressed);
	String get_selected_bg_check_text_icon_pressed_mirrored() const;
	void set_selected_bg_check_text_icon_hover_mirrored(const String &p_selected_bg_check_text_icon_hover);
	String get_selected_bg_check_text_icon_hover_mirrored() const;
	void set_selected_bg_check_text_icon_disabled_mirrored(const String &p_selected_bg_check_text_icon_disabled);
	String get_selected_bg_check_text_icon_disabled_mirrored() const;

	void set_unselected_bg_check_text_icon_normal_mirrored(const String &p_unselected_bg_check_text_icon_normal);
	String get_unselected_bg_check_text_icon_normal_mirrored() const;
	void set_unselected_bg_check_text_icon_pressed_mirrored(const String &p_unselected_bg_check_text_icon_pressed);
	String get_unselected_bg_check_text_icon_pressed_mirrored() const;
	void set_unselected_bg_check_text_icon_hover_mirrored(const String &p_unselected_bg_check_text_icon_hover);
	String get_unselected_bg_check_text_icon_hover_mirrored() const;
	void set_unselected_bg_check_text_icon_disabled_mirrored(const String &p_unselected_bg_check_text_icon_disabled);
	String get_unselected_bg_check_text_icon_disabled_mirrored() const;

	CheckButton(const String &p_text = String());
	~CheckButton();
};

#endif // CHECK_BUTTON_H

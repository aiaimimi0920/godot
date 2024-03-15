/**************************************************************************/
/*  button.h                                                              */
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

#ifndef BUTTON_H
#define BUTTON_H

#include "scene/gui/base_button.h"
#include "scene/resources/text_paragraph.h"

class Button : public BaseButton {
	GDCLASS(Button, BaseButton);

private:
	bool flat = false;
	String text;
	String xl_text;

	Ref<TextParagraph> text_buf;
	Ref<TextParagraph> text_icon_buf;

	String language;
	TextDirection text_direction = TEXT_DIRECTION_AUTO;
	TextServer::AutowrapMode autowrap_mode = TextServer::AUTOWRAP_OFF;
	TextServer::OverrunBehavior overrun_behavior = TextServer::OVERRUN_NO_TRIMMING;

	Ref<Texture2D> icon;

	String code_text_icon;
	String text_icon = "";

	bool expand_icon = false;
	bool clip_text = false;
	HorizontalAlignment alignment = HORIZONTAL_ALIGNMENT_CENTER;
	HorizontalAlignment horizontal_icon_alignment = HORIZONTAL_ALIGNMENT_LEFT;
	VerticalAlignment vertical_icon_alignment = VERTICAL_ALIGNMENT_CENTER;
	float _internal_margin[4] = {};

	struct ThemeCache {
		Ref<ColorScheme> default_color_scheme;

		Ref<StyleBox> normal;
		Ref<StyleBox> normal_mirrored;
		Ref<StyleBox> pressed;
		Ref<StyleBox> pressed_mirrored;
		Ref<StyleBox> hover;
		Ref<StyleBox> hover_mirrored;
		Ref<StyleBox> hover_pressed;
		Ref<StyleBox> hover_pressed_mirrored;
		Ref<StyleBox> disabled;
		Ref<StyleBox> disabled_mirrored;
		Ref<StyleBox> focus;

		Ref<StyleBox> hover_state_layer;
		Ref<StyleBox> hover_state_layer_mirrored;
		Ref<StyleBox> pressed_state_layer;
		Ref<StyleBox> pressed_state_layer_mirrored;
		Ref<StyleBox> hover_pressed_state_layer;
		Ref<StyleBox> hover_pressed_state_layer_mirrored;
		Ref<StyleBox> focus_state_layer;

		Color font_color_scale;
		Ref<ColorScheme> font_color_scheme;
		ColorRole font_color_role;
		Color font_color;

		Color font_pressed_color_scale;
		Ref<ColorScheme> font_pressed_color_scheme;
		ColorRole font_pressed_color_role;
		Color font_pressed_color;

		Color font_hover_color_scale;
		Ref<ColorScheme> font_hover_color_scheme;
		ColorRole font_hover_color_role;
		Color font_hover_color;

		Color font_focus_color_scale;
		Ref<ColorScheme> font_focus_color_scheme;
		ColorRole font_focus_color_role;
		Color font_focus_color;

		Color font_hover_pressed_color_scale;
		Ref<ColorScheme> font_hover_pressed_color_scheme;
		ColorRole font_hover_pressed_color_role;
		Color font_hover_pressed_color;

		Color font_disabled_color_scale;
		Ref<ColorScheme> font_disabled_color_scheme;
		ColorRole font_disabled_color_role;
		Color font_disabled_color;

		Ref<Font> font;
		int font_size = 0;
		int outline_size = 0;

		Color font_outline_color_scale;
		Ref<ColorScheme> font_outline_color_scheme;
		ColorRole font_outline_color_role;
		Color font_outline_color;

		Color icon_normal_color_scale;
		Ref<ColorScheme> icon_normal_color_scheme;
		ColorRole icon_normal_color_role;
		Color icon_normal_color;

		Color icon_pressed_color_scale;
		Ref<ColorScheme> icon_pressed_color_scheme;
		ColorRole icon_pressed_color_role;
		Color icon_pressed_color;

		Color icon_hover_color_scale;
		Ref<ColorScheme> icon_hover_color_scheme;
		ColorRole icon_hover_color_role;
		Color icon_hover_color;

		Color icon_focus_color_scale;
		Ref<ColorScheme> icon_focus_color_scheme;
		ColorRole icon_focus_color_role;
		Color icon_focus_color;

		Color icon_hover_pressed_color_scale;
		Ref<ColorScheme> icon_hover_pressed_color_scheme;
		ColorRole icon_hover_pressed_color_role;
		Color icon_hover_pressed_color;

		Color icon_disabled_color_scale;
		Ref<ColorScheme> icon_disabled_color_scheme;
		ColorRole icon_disabled_color_role;
		Color icon_disabled_color;

		Ref<Texture2D> icon;

		Color text_icon_normal_color_scale;
		Ref<ColorScheme> text_icon_normal_color_scheme;
		ColorRole text_icon_normal_color_role;
		Color text_icon_normal_color;

		Color text_icon_pressed_color_scale;
		Ref<ColorScheme> text_icon_pressed_color_scheme;
		ColorRole text_icon_pressed_color_role;
		Color text_icon_pressed_color;

		Color text_icon_hover_color_scale;
		Ref<ColorScheme> text_icon_hover_color_scheme;
		ColorRole text_icon_hover_color_role;
		Color text_icon_hover_color;

		Color text_icon_focus_color_scale;
		Ref<ColorScheme> text_icon_focus_color_scheme;
		ColorRole text_icon_focus_color_role;
		Color text_icon_focus_color;

		Color text_icon_hover_pressed_color_scale;
		Ref<ColorScheme> text_icon_hover_pressed_color_scheme;
		ColorRole text_icon_hover_pressed_color_role;
		Color text_icon_hover_pressed_color;

		Color text_icon_disabled_color_scale;
		Ref<ColorScheme> text_icon_disabled_color_scheme;
		ColorRole text_icon_disabled_color_role;
		Color text_icon_disabled_color;

		Ref<Font> text_icon_font;
		int text_icon_font_size = 0;

		int h_separation = 0;
		int icon_max_width = 0;
	} theme_cache;

	Size2 _fit_icon_size(const Size2 &p_size) const;

	void _shape(Ref<TextParagraph> p_paragraph = Ref<TextParagraph>(), String p_text = "");
	void _icon_shape(Ref<TextParagraph> p_paragraph = Ref<TextParagraph>(), String p_text = "", int expand_icon_size = 0);

	void _texture_changed();

protected:
	void _set_internal_margin(Side p_side, float p_value);
	virtual void _queue_update_size_cache();

	Ref<StyleBox> _get_focus_state_layer_stylebox() const;
	Ref<StyleBox> _get_current_stylebox() const;
	Ref<StyleBox> _get_current_state_layer_stylebox() const;
	bool _is_show_state_layer();
	void _notification(int p_what);
	static void _bind_methods();

public:
	virtual Size2 get_minimum_size() const override;

	Size2 get_minimum_size_for_text_and_icon(const String &p_text, Ref<Texture2D> p_icon, const String &p_text_icon = "") const;

	void set_text(const String &p_text);
	String get_text() const;

	void set_text_icon(const String &p_text_icon);
	String get_text_icon() const;

	void set_text_overrun_behavior(TextServer::OverrunBehavior p_behavior);
	TextServer::OverrunBehavior get_text_overrun_behavior() const;

	void set_autowrap_mode(TextServer::AutowrapMode p_mode);
	TextServer::AutowrapMode get_autowrap_mode() const;

	void set_text_direction(TextDirection p_text_direction);
	TextDirection get_text_direction() const;

	void set_language(const String &p_language);
	String get_language() const;

	void set_icon(const Ref<Texture2D> &p_icon);
	Ref<Texture2D> get_icon() const;

	void set_expand_icon(bool p_enabled);
	bool is_expand_icon() const;

	void set_flat(bool p_enabled);
	bool is_flat() const;

	void set_clip_text(bool p_enabled);
	bool get_clip_text() const;

	void set_text_alignment(HorizontalAlignment p_alignment);
	HorizontalAlignment get_text_alignment() const;

	void set_icon_alignment(HorizontalAlignment p_alignment);
	void set_vertical_icon_alignment(VerticalAlignment p_alignment);
	HorizontalAlignment get_icon_alignment() const;
	VerticalAlignment get_vertical_icon_alignment() const;

	Button(const String &p_text = String());
	~Button();
};

class ElevatedButton : public Button {
	GDCLASS(ElevatedButton, Button);
};

class FilledButton : public Button {
	GDCLASS(FilledButton, Button);
};

class FilledTonalButton : public Button {
	GDCLASS(FilledTonalButton, Button);
};

class OutlinedButton : public Button {
	GDCLASS(OutlinedButton, Button);
};

class TextButton : public Button {
	GDCLASS(TextButton, Button);
};

class FabButton : public Button {
	GDCLASS(FabButton, Button);
};

class ExtendedFabButton : public Button {
	GDCLASS(ExtendedFabButton, Button);
};


#endif // BUTTON_H

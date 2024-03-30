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
	// String text_icon;
	// String code_text_icon;

	bool expand_icon = false;
	bool clip_text = false;
	HorizontalAlignment alignment = HORIZONTAL_ALIGNMENT_CENTER;
	HorizontalAlignment horizontal_icon_alignment = HORIZONTAL_ALIGNMENT_LEFT;
	VerticalAlignment vertical_icon_alignment = VERTICAL_ALIGNMENT_CENTER;
	float _internal_margin[4] = {};

	struct ThemeCache {
		Ref<ColorScheme> default_color_scheme;
		ThemeStyleboxData default_stylebox{ "default_stylebox" };
		ThemeStyleboxData state_layer_stylebox{ "state_layer_stylebox" };

		ThemeColorData font_color{ "font_color" };
		ThemeColorRoleData font_color_role{ "font_color_role" };
		Ref<Font> font;
		int font_size;

		Color font_outline_color;
		ColorRole font_outline_color_role;
		int font_outline_size;

		Ref<Font> text_icon_font;
		int text_icon_font_size;

		ThemeIconData icon{ "icon" };
		ThemeColorData icon_color{"icon_color"};
		ThemeColorRoleData icon_color_role{"icon_color_role"};

		int icon_max_width;

		ThemeStrData text_icon{"text_icon"};
		ThemeColorData text_icon_color{"text_icon_color"};
		ThemeColorRoleData text_icon_color_role{"text_icon_color_role"};

		int h_separation;
	} theme_cache;

	Size2 _fit_icon_size(const Size2 &p_size) const;

	void _shape(Ref<TextParagraph> p_paragraph = Ref<TextParagraph>(), String p_text = "");
	void _icon_shape(Ref<TextParagraph> p_paragraph = Ref<TextParagraph>(), String p_text = "", int expand_icon_size = 0);

	void _texture_changed();

protected:
	void _set_internal_margin(Side p_side, float p_value);
	virtual void _queue_update_size_cache();

	void _notification(int p_what);
	static void _bind_methods();

	bool _has_current_icon() const;
	Ref<Texture2D> _get_current_icon() const;
	bool _has_current_text_icon() const;
	String _get_current_text_icon() const;
	bool _has_current_default_stylebox_with_state(State p_state) const;
	bool _has_current_default_stylebox() const;
	Ref<StyleBox> _get_current_default_stylebox_with_state(State p_state) const;
	Ref<StyleBox> _get_current_default_stylebox() const;
	bool _has_current_focus_default_stylebox() const;
	Ref<StyleBox> _get_current_focus_default_stylebox() const;
	bool _has_current_state_layer_stylebox() const;
	Ref<StyleBox> _get_current_state_layer_stylebox() const;
	bool _has_current_font_color() const;
	Color _get_current_font_color() const;
	bool _has_current_icon_color() const;
	Color _get_current_icon_color() const;
	bool _has_current_text_icon_color() const;
	Color _get_current_text_icon_color() const;

	String _get_trans_text(const String &p_text_icon);
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

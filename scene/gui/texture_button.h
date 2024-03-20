/**************************************************************************/
/*  texture_button.h                                                      */
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

#ifndef TEXTURE_BUTTON_H
#define TEXTURE_BUTTON_H

#include "scene/gui/base_button.h"
#include "scene/resources/bit_map.h"
#include "scene/resources/text_paragraph.h"
class TextureButton : public BaseButton {
	GDCLASS(TextureButton, BaseButton);

public:
	enum StretchMode {
		STRETCH_SCALE,
		STRETCH_TILE,
		STRETCH_KEEP,
		STRETCH_KEEP_CENTERED,
		STRETCH_KEEP_ASPECT,
		STRETCH_KEEP_ASPECT_CENTERED,
		STRETCH_KEEP_ASPECT_COVERED,
	};

private:
	Ref<Texture2D> normal;
	Ref<Texture2D> pressed;
	Ref<Texture2D> hover;
	Ref<Texture2D> disabled;
	Ref<Texture2D> focused;
	Ref<BitMap> click_mask;
	bool ignore_texture_size = false;
	StretchMode stretch_mode = STRETCH_KEEP;

	Ref<TextParagraph> text_icon_buf;
	Ref<TextParagraph> text_icon_focus_buf;

	String text_icon_normal = "";
	String code_text_icon_normal;
	
	String text_icon_pressed = "";
	String code_text_icon_pressed;
	
	String text_icon_hover = "";
	String code_text_icon_hover;
	
	String text_icon_disabled = "";
	String code_text_icon_disabled;
	
	String text_icon_focused = "";
	String code_text_icon_focused;
	
	Rect2 _texture_region;
	Rect2 _position_rect;
	bool _tile = false;

	bool hflip = false;
	bool vflip = false;

	void _set_texture(Ref<Texture2D> *p_destination, const Ref<Texture2D> &p_texture);
	void _texture_changed();
	String _get_trans_text(const String &p_text_icon);

	struct ThemeCache {
		Ref<ColorScheme> default_color_scheme;
		ThemeStyleboxData default_stylebox{"default_stylebox"};
		ThemeStyleboxData state_layer_stylebox{"state_layer_stylebox"};

		Ref<Font> text_icon_font;
		int text_icon_font_size = 0;

		int icon_max_width = 0;

		ThemeColorData text_icon_color{"text_icon_color"};
		ThemeColorRoleData text_icon_color_role{"text_icon_color_role"};

	} theme_cache;

protected:
	virtual Size2 get_minimum_size() const override;
	virtual bool has_point(const Point2 &p_point) const override;

	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_texture_normal(const Ref<Texture2D> &p_normal);
	void set_texture_pressed(const Ref<Texture2D> &p_pressed);
	void set_texture_hover(const Ref<Texture2D> &p_hover);
	void set_texture_disabled(const Ref<Texture2D> &p_disabled);
	void set_texture_focused(const Ref<Texture2D> &p_focused);
	void set_click_mask(const Ref<BitMap> &p_click_mask);

	Ref<Texture2D> get_texture_normal() const;
	Ref<Texture2D> get_texture_pressed() const;
	Ref<Texture2D> get_texture_hover() const;
	Ref<Texture2D> get_texture_disabled() const;
	Ref<Texture2D> get_texture_focused() const;
	Ref<BitMap> get_click_mask() const;

	void set_text_icon_normal(const String &p_text_icon_normal);
	String get_text_icon_normal() const;
	void set_text_icon_pressed(const String &p_text_icon_pressed);
	String get_text_icon_pressed() const;
	void set_text_icon_hover(const String &p_text_icon_hover);
	String get_text_icon_hover() const;
	void set_text_icon_disabled(const String &p_text_icon_disabled);
	String get_text_icon_disabled() const;
	void set_text_icon_focused(const String &p_text_icon_focused);
	String get_text_icon_focused() const;

	bool get_ignore_texture_size() const;
	void set_ignore_texture_size(bool p_ignore);

	void set_stretch_mode(StretchMode p_stretch_mode);
	StretchMode get_stretch_mode() const;

	void set_flip_h(bool p_flip);
	bool is_flipped_h() const;

	void set_flip_v(bool p_flip);
	bool is_flipped_v() const;


	bool _has_current_default_stylebox() const;
	Ref<StyleBox> _get_current_default_stylebox() const;
	bool _has_current_focus_default_stylebox() const;
	Ref<StyleBox> _get_current_focus_default_stylebox() const;
	bool _has_current_state_layer_stylebox() const;
	Ref<StyleBox> _get_current_state_layer_stylebox() const;

	bool _has_current_text_icon_color() const;
	Color _get_current_text_icon_color() const;

	bool _has_current_focus_text_icon_color() const;
	Color _get_current_focus_text_icon_color() const;

	TextureButton();
};

VARIANT_ENUM_CAST(TextureButton::StretchMode);


class FilledIconButton : public TextureButton {
	GDCLASS(FilledIconButton, TextureButton);
};

class FilledTonalIconButton : public TextureButton {
	GDCLASS(FilledTonalIconButton, TextureButton);
};

class OutlinedIconButton : public TextureButton {
	GDCLASS(OutlinedIconButton, TextureButton);
};

class StandardIconButton : public TextureButton {
	GDCLASS(StandardIconButton, TextureButton);
};

#endif // TEXTURE_BUTTON_H

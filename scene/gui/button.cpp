/**************************************************************************/
/*  button.cpp                                                            */
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

#include "button.h"

#include "core/string/translation.h"
#include "scene/theme/theme_db.h"
#include "servers/rendering_server.h"

Size2 Button::get_minimum_size() const {
	Ref<Texture2D> _icon;
	String _text_icon;
	if (_icon.is_null()) {
		if(_has_current_icon()){
			_icon = _get_current_icon();
		}
		if(_icon.is_null()){
			if(_text_icon.is_empty()){
				if(_has_current_text_icon()){
					_text_icon = _get_current_text_icon();
				}
			}
		}
	}
	return get_minimum_size_for_text_and_icon("", _icon, _text_icon);
}

void Button::_set_internal_margin(Side p_side, float p_value) {
	_internal_margin[p_side] = p_value;
}

void Button::_queue_update_size_cache() {
}

bool Button::_has_current_icon() const{
	State cur_state = get_current_state_with_focus();
	if(theme_cache.icon.has_loop_data(cur_state)){
		return true;
	}
	return false;
}

Ref<Texture2D> Button::_get_current_icon() const{
	State cur_state = get_current_state_with_focus();
	Ref<Texture2D> cur_icon;
	if(theme_cache.icon.has_loop_data(cur_state)){
		cur_icon = theme_cache.icon.get_loop_data(cur_state);
	}
	return cur_icon;
}

bool Button::_has_current_text_icon() const{
	State cur_state = get_current_state_with_focus();
	if(theme_cache.text_icon.has_loop_data(cur_state)){
		return true;
	}
	return false;
}

String Button::_get_current_text_icon() const{
	State cur_state = get_current_state_with_focus();
	String cur_text_icon;
	if(theme_cache.text_icon.has_loop_data(cur_state)){
		cur_text_icon = theme_cache.text_icon.get_loop_data(cur_state);
	}
	return cur_text_icon;
}

bool Button::_has_current_default_stylebox() const{
	State cur_state = get_current_state();
	if(theme_cache.default_stylebox.has_loop_data(cur_state)){
		return true;
	}
	return false;
}

Ref<StyleBox> Button::_get_current_default_stylebox() const{
	State cur_state = get_current_state();
	Ref<StyleBox> style;
	if(theme_cache.default_stylebox.has_loop_data(cur_state)){
		style = theme_cache.default_stylebox.get_loop_data(cur_state);
	}
	return style;
}

bool Button::_has_current_focus_default_stylebox() const{
	State cur_state = get_current_focus_state();
	if(theme_cache.default_stylebox.has_loop_data(cur_state)){
		return true;
	}
	return false;
}

Ref<StyleBox> Button::_get_current_focus_default_stylebox() const{
	State cur_state = get_current_focus_state();
	Ref<StyleBox> style;
	if(theme_cache.default_stylebox.has_loop_data(cur_state)){
		style = theme_cache.default_stylebox.get_loop_data(cur_state);
	}
	return style;
}

bool Button::_has_current_state_layer_stylebox() const{
	State cur_state = get_current_state_with_focus();
	if(theme_cache.state_layer_stylebox.has_loop_data(cur_state)){
		return true;
	}
	return false;
}

Ref<StyleBox> Button::_get_current_state_layer_stylebox() const{
	State cur_state = get_current_state_with_focus();
	Ref<StyleBox> style;
	if(theme_cache.state_layer_stylebox.has_loop_data(cur_state)){
		style = theme_cache.state_layer_stylebox.get_loop_data(cur_state);
	}
	return style;
}

bool Button::_has_current_font_color() const{
	State cur_state = get_current_state_with_focus();
	if(theme_cache.font_color.has_loop_data(cur_state)){
		return true;
	}
	return false;
}

Color Button::_get_current_font_color() const{
	State cur_state = get_current_state_with_focus();
	Color cur_font_color;
	if(theme_cache.font_color.has_loop_data(cur_state)){
		cur_font_color = theme_cache.font_color.get_loop_data(cur_state);
	}
	return cur_font_color;
}

bool Button::_has_current_font_outline_color() const{
	State cur_state = get_current_state_with_focus();
	if(theme_cache.font_outline_color.has_loop_data(cur_state)){
		return true;
	}
	return false;
}

Color Button::_get_current_font_outline_color() const{
	State cur_state = get_current_state_with_focus();
	Color cur_font_outline_color;
	if(theme_cache.font_outline_color.has_loop_data(cur_state)){
		cur_font_outline_color = theme_cache.font_outline_color.get_loop_data(cur_state);
	}
	return cur_font_outline_color;
}


bool Button::_has_current_icon_color() const{
	State cur_state = get_current_state_with_focus();
	if(theme_cache.icon_color.has_loop_data(cur_state)){
		return true;
	}
	return false;
}

Color Button::_get_current_icon_color() const{
	State cur_state = get_current_state_with_focus();
	Color cur_icon_color;
	if(theme_cache.icon_color.has_loop_data(cur_state)){
		cur_icon_color = theme_cache.icon_color.get_loop_data(cur_state);
	}
	return cur_icon_color;
}

bool Button::_has_current_text_icon_color() const{
	State cur_state = get_current_state_with_focus();
	if(theme_cache.text_icon_color.has_loop_data(cur_state)){
		return true;
	}
	return false;
}

Color Button::_get_current_text_icon_color() const{
	State cur_state = get_current_state_with_focus();
	Color cur_text_icon_color;
	if(theme_cache.text_icon_color.has_loop_data(cur_state)){
		cur_text_icon_color = theme_cache.text_icon_color.get_loop_data(cur_state);
	}
	return cur_text_icon_color;
}

void Button::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_LAYOUT_DIRECTION_CHANGED: {
			queue_redraw();
		} break;

		case NOTIFICATION_TRANSLATION_CHANGED: {
			xl_text = atr(text);
			_shape();
			update_minimum_size();
			queue_redraw();
		} break;

		case NOTIFICATION_THEME_CHANGED: {
			_shape();
			_icon_shape();
			update_minimum_size();
			queue_redraw();
		} break;

		case NOTIFICATION_RESIZED: {
			if (autowrap_mode != TextServer::AUTOWRAP_OFF) {
				_shape();
				_icon_shape();

				update_minimum_size();
				queue_redraw();
			}
		} break;

		case NOTIFICATION_DRAW: {
			const RID ci = get_canvas_item();
			const Size2 size = get_size();
			Ref<Font> text_icon_font = theme_cache.text_icon_font;

			const Ref<StyleBox> style = _get_current_default_stylebox();
			{ // Draws the stylebox in the current state.
				if (!flat) {
					style->draw(ci, Rect2(Point2(), size));
				}

				if (is_draw_state_layer_enabled()) {
					if(_has_current_state_layer_stylebox()){
						const Ref<StyleBox> state_layer_style = _get_current_state_layer_stylebox();
						state_layer_style->draw(ci, Rect2(Point2(), size));
					}
				}

				if (has_focus()) {
					if(_has_current_focus_default_stylebox()){
						Ref<StyleBox> style2 = _get_current_focus_default_stylebox();
						style2->draw(ci, Rect2(Point2(), size));
					}
				}
			}

			Ref<Texture2D> _icon;
			if(_has_current_icon()){
				_icon = _get_current_icon();
			}


			String _text_icon;
			if (_text_icon.is_empty()) {
				if(_has_current_text_icon()){
					_text_icon = _get_current_text_icon();
					String local_name = text_icon_font->get_path().get_file().get_basename();
					Ref<Translation> trans = TranslationServer::get_singleton()->get_translation_object(local_name);
					if (trans.is_valid()) {
						_text_icon = trans->get_message(_text_icon);
						if (!_text_icon.is_empty()) {
							_text_icon = String::chr(("0x" + _text_icon.to_lower()).hex_to_int());
						}
					}

				}
			}

			if (xl_text.is_empty() && _icon.is_null() && _text_icon.is_empty()) {
				break;
			}

			const float style_margin_left = style->get_margin(SIDE_LEFT);
			const float style_margin_right = style->get_margin(SIDE_RIGHT);
			const float style_margin_top = style->get_margin(SIDE_TOP);
			const float style_margin_bottom = style->get_margin(SIDE_BOTTOM);

			Size2 drawable_size_remained = size;

			{ // The size after the stelybox is stripped.
				drawable_size_remained.width -= style_margin_left + style_margin_right;
				drawable_size_remained.height -= style_margin_top + style_margin_bottom;
			}

			const int h_separation = MAX(0, theme_cache.h_separation);

			float left_internal_margin_with_h_separation = _internal_margin[SIDE_LEFT];
			float right_internal_margin_with_h_separation = _internal_margin[SIDE_RIGHT];
			{ // The width reserved for internal element in derived classes (and h_separation if need).

				if (_internal_margin[SIDE_LEFT] > 0.0f) {
					left_internal_margin_with_h_separation += h_separation;
				}

				if (_internal_margin[SIDE_RIGHT] > 0.0f) {
					right_internal_margin_with_h_separation += h_separation;
				}

				drawable_size_remained.width -= left_internal_margin_with_h_separation + right_internal_margin_with_h_separation; // The size after the internal element is stripped.
			}

			HorizontalAlignment icon_align_rtl_checked = horizontal_icon_alignment;
			HorizontalAlignment align_rtl_checked = alignment;
			// Swap icon and text alignment sides if right-to-left layout is set.
			if (is_layout_rtl()) {
				if (horizontal_icon_alignment == HORIZONTAL_ALIGNMENT_RIGHT) {
					icon_align_rtl_checked = HORIZONTAL_ALIGNMENT_LEFT;
				} else if (horizontal_icon_alignment == HORIZONTAL_ALIGNMENT_LEFT) {
					icon_align_rtl_checked = HORIZONTAL_ALIGNMENT_RIGHT;
				}
				if (alignment == HORIZONTAL_ALIGNMENT_RIGHT) {
					align_rtl_checked = HORIZONTAL_ALIGNMENT_LEFT;
				} else if (alignment == HORIZONTAL_ALIGNMENT_LEFT) {
					align_rtl_checked = HORIZONTAL_ALIGNMENT_RIGHT;
				}
			}

			Color _font_color;
			if(_has_current_font_color()){
				_font_color = _get_current_font_color();
			}

			Color _icon_color;
			if(_has_current_icon_color()){
				_icon_color = _get_current_icon_color();
			}

			Color _text_icon_color;
			if(_has_current_text_icon_color()){
				_text_icon_color = _get_current_text_icon_color();
			}

			const bool is_clipped = clip_text || overrun_behavior != TextServer::OVERRUN_NO_TRIMMING || autowrap_mode != TextServer::AUTOWRAP_OFF;
			Size2 custom_element_size = drawable_size_remained;

			

			// Draw the icon.
			if (_icon.is_valid()) {
				Size2 icon_size;

				{ // Calculate the drawing size of the icon.
					icon_size = _icon->get_size();

					if (expand_icon) {
						const Size2 text_buf_size = text_buf->get_size();
						Size2 _size = custom_element_size;
						if (!is_clipped && icon_align_rtl_checked != HORIZONTAL_ALIGNMENT_CENTER && text_buf_size.width > 0.0f) {
							// If there is not enough space for icon and h_separation, h_separation will occupy the space first,
							// so the icon's width may be negative. Keep it negative to make it easier to calculate the space
							// reserved for text later.
							_size.width -= text_buf_size.width + h_separation;
						}
						if (vertical_icon_alignment != VERTICAL_ALIGNMENT_CENTER) {
							_size.height -= text_buf_size.height;
						}

						float icon_width = icon_size.width * _size.height / icon_size.height;
						float icon_height = _size.height;

						if (icon_width > _size.width) {
							icon_width = _size.width;
							icon_height = icon_size.height * icon_width / icon_size.width;
						}

						icon_size = Size2(icon_width, icon_height);
					}
					icon_size = _fit_icon_size(icon_size);
				}

				if (icon_size.width > 0.0f) {
					// Calculate the drawing position of the icon.
					Point2 icon_ofs;

					switch (icon_align_rtl_checked) {
						case HORIZONTAL_ALIGNMENT_CENTER: {
							icon_ofs.x = (custom_element_size.width - icon_size.width) / 2.0f;
						}
							[[fallthrough]];
						case HORIZONTAL_ALIGNMENT_FILL:
						case HORIZONTAL_ALIGNMENT_LEFT: {
							icon_ofs.x += style_margin_left;
							icon_ofs.x += left_internal_margin_with_h_separation;
						} break;

						case HORIZONTAL_ALIGNMENT_RIGHT: {
							icon_ofs.x = size.x - style_margin_right;
							icon_ofs.x -= right_internal_margin_with_h_separation;
							icon_ofs.x -= icon_size.width;
						} break;
					}

					switch (vertical_icon_alignment) {
						case VERTICAL_ALIGNMENT_CENTER: {
							icon_ofs.y = (custom_element_size.height - icon_size.height) / 2.0f;
						}
							[[fallthrough]];
						case VERTICAL_ALIGNMENT_FILL:
						case VERTICAL_ALIGNMENT_TOP: {
							icon_ofs.y += style_margin_top;
						} break;

						case VERTICAL_ALIGNMENT_BOTTOM: {
							icon_ofs.y = size.y - style_margin_bottom - icon_size.height;
						} break;
					}

					Rect2 icon_region = Rect2(icon_ofs, icon_size);
					draw_texture_rect(_icon, icon_region, false, _icon_color);
				}

				if (!xl_text.is_empty()) {
					// Update the size after the icon is stripped. Stripping only when the icon alignments are not center.
					if (icon_align_rtl_checked != HORIZONTAL_ALIGNMENT_CENTER) {
						// Subtract the space's width occupied by icon and h_separation together.
						drawable_size_remained.width -= icon_size.width + h_separation;
					}

					if (vertical_icon_alignment != VERTICAL_ALIGNMENT_CENTER) {
						drawable_size_remained.height -= icon_size.height;
					}
				}
			} else {
				// Draw the text icon.
				if (!_text_icon.is_empty()) {
					Size2 text_icon_size;
					{
						text_icon_buf->set_alignment(icon_align_rtl_checked);
						if (expand_icon) {
							const Size2 text_buf_size = text_buf->get_size();
							Size2 _size = custom_element_size;
							if (!is_clipped && icon_align_rtl_checked != HORIZONTAL_ALIGNMENT_CENTER && text_buf_size.width > 0.0f) {
								// If there is not enough space for icon and h_separation, h_separation will occupy the space first,
								// so the icon's width may be negative. Keep it negative to make it easier to calculate the space
								// reserved for text later.
								_size.width -= text_buf_size.width + h_separation;
							}
							if (vertical_icon_alignment != VERTICAL_ALIGNMENT_CENTER) {
								_size.height -= text_buf_size.height;
							}
							float icon_width = _size.height;
							float icon_height = _size.height;

							if (icon_width > _size.width) {
								icon_width = _size.width;
								icon_height = icon_width;
							}
							text_icon_size = Size2(icon_width, icon_height);

							text_icon_size = _fit_icon_size(text_icon_size);
							if (text_icon_size.width < text_icon_size.height) {
								text_icon_size = Size2(text_icon_size.width, text_icon_size.width);
							} else {
								text_icon_size = Size2(text_icon_size.height, text_icon_size.height);
							}
						} else {
							int cur_font_size = theme_cache.text_icon_font_size;
							float font_height = text_icon_font->get_height(cur_font_size);

							int icon_max_width = theme_cache.icon_max_width;
							int max_size = font_height;
							if (icon_max_width > 0) {
								if (max_size > icon_max_width) {
									max_size = icon_max_width;
								}
							}
							text_icon_size = Size2(max_size, max_size);
						}
						float text_buf_width = MAX(1.0f, drawable_size_remained.width); // The space's width filled by the text_buf.
						text_icon_buf->set_width(text_icon_size.width);
					}

					Point2 text_ofs;
					switch (icon_align_rtl_checked) {
						case HORIZONTAL_ALIGNMENT_CENTER: {
							text_ofs.x = (custom_element_size.width - text_icon_size.width) / 2.0f;
						}
							[[fallthrough]];
						case HORIZONTAL_ALIGNMENT_FILL:
						case HORIZONTAL_ALIGNMENT_LEFT: {
							text_ofs.x += style_margin_left;
							text_ofs.x += left_internal_margin_with_h_separation;
						} break;
						case HORIZONTAL_ALIGNMENT_RIGHT: {
							text_ofs.x = size.x - style_margin_right;
							text_ofs.x -= right_internal_margin_with_h_separation;
							text_ofs.x -= text_icon_size.width;
						} break;
					}

					switch (vertical_icon_alignment) {
						case VERTICAL_ALIGNMENT_CENTER: {
							text_ofs.y = (custom_element_size.height - text_icon_size.height) / 2.0f;
						}
							[[fallthrough]];
						case VERTICAL_ALIGNMENT_FILL:
						case VERTICAL_ALIGNMENT_TOP: {
							text_ofs.y += style_margin_top;
						} break;

						case VERTICAL_ALIGNMENT_BOTTOM: {
							text_ofs.y = size.y - style_margin_bottom - text_icon_size.height;
						} break;
					}

					_icon_shape(Ref<TextParagraph>(), _text_icon, text_icon_size.width);
					text_icon_buf->draw(ci, text_ofs, _text_icon_color);

					if (!xl_text.is_empty()) {
						// Update the size after the icon is stripped. Stripping only when the icon alignments are not center.
						if (icon_align_rtl_checked != HORIZONTAL_ALIGNMENT_CENTER) {
							// Subtract the space's width occupied by icon and h_separation together.
							drawable_size_remained.width -= text_icon_size.width + h_separation;
						}

						if (vertical_icon_alignment != VERTICAL_ALIGNMENT_CENTER) {
							drawable_size_remained.height -= text_icon_size.height;
						}
					}
				}
			}

			// Draw the text.
			if (!xl_text.is_empty()) {
				text_buf->set_alignment(align_rtl_checked);

				float text_buf_width = MAX(1.0f, drawable_size_remained.width); // The space's width filled by the text_buf.
				text_buf->set_width(text_buf_width);

				Point2 text_ofs;

				switch (align_rtl_checked) {
					case HORIZONTAL_ALIGNMENT_CENTER: {
						text_ofs.x = (drawable_size_remained.width - text_buf_width) / 2.0f;
					}
						[[fallthrough]];
					case HORIZONTAL_ALIGNMENT_FILL:
					case HORIZONTAL_ALIGNMENT_LEFT:
					case HORIZONTAL_ALIGNMENT_RIGHT: {
						text_ofs.x += style_margin_left;
						text_ofs.x += left_internal_margin_with_h_separation;
						if (icon_align_rtl_checked == HORIZONTAL_ALIGNMENT_LEFT) {
							// Offset by the space's width that occupied by icon and h_separation together.
							text_ofs.x += custom_element_size.width - drawable_size_remained.width;
						}
					} break;
				}

				text_ofs.y = (drawable_size_remained.height - text_buf->get_size().height) / 2.0f + style_margin_top;
				if (vertical_icon_alignment == VERTICAL_ALIGNMENT_TOP) {
					text_ofs.y += custom_element_size.height - drawable_size_remained.height; // Offset by the icon's height.
				}

				Color font_outline_color = _get_current_font_outline_color();
				int font_outline_size = theme_cache.font_outline_size;

				if (font_outline_size > 0 && font_outline_color.a > 0.0f) {
					text_buf->draw_outline(ci, text_ofs, font_outline_size, font_outline_color);
				}
				text_buf->draw(ci, text_ofs, _font_color);
			}
		}
	}
}

Size2 Button::_fit_icon_size(const Size2 &p_size) const {
	int max_width = theme_cache.icon_max_width;
	Size2 icon_size = p_size;

	if (max_width > 0 && icon_size.width > max_width) {
		icon_size.height = icon_size.height * max_width / icon_size.width;
		icon_size.width = max_width;
	}

	return icon_size;
}

Size2 Button::get_minimum_size_for_text_and_icon(const String &p_text, Ref<Texture2D> p_icon, const String &p_text_icon) const {
	Ref<TextParagraph> paragraph;
	if (p_text.is_empty()) {
		paragraph = text_buf;
	} else {
		paragraph.instantiate();
		const_cast<Button *>(this)->_shape(paragraph, p_text);
	}

	Size2 minsize = paragraph->get_size();
	if (clip_text || overrun_behavior != TextServer::OVERRUN_NO_TRIMMING || autowrap_mode != TextServer::AUTOWRAP_OFF) {
		minsize.width = 0;
	}

	if (!expand_icon && p_icon.is_valid()) {
		Size2 icon_size = _fit_icon_size(p_icon->get_size());
		if (vertical_icon_alignment == VERTICAL_ALIGNMENT_CENTER) {
			minsize.height = MAX(minsize.height, icon_size.height);
		} else {
			minsize.height += icon_size.height;
		}

		if (horizontal_icon_alignment != HORIZONTAL_ALIGNMENT_CENTER) {
			minsize.width += icon_size.width;
			if (!xl_text.is_empty() || !p_text.is_empty()) {
				minsize.width += MAX(0, theme_cache.h_separation);
			}
		} else {
			minsize.width = MAX(minsize.width, icon_size.width);
		}
	} else if (!expand_icon && !p_text_icon.is_empty()) {
		Ref<Font> font = theme_cache.text_icon_font;
		float font_height = font->get_height(theme_cache.text_icon_font_size);
		int icon_max_width = theme_cache.icon_max_width;
		int max_size = font_height;
		if (icon_max_width > 0) {
			if (max_size > icon_max_width) {
				max_size = icon_max_width;
			}
		}

		Size2 icon_size = Size2(max_size, max_size);
		if (vertical_icon_alignment == VERTICAL_ALIGNMENT_CENTER) {
			minsize.height = MAX(minsize.height, icon_size.height);
		} else {
			minsize.height += icon_size.height;
		}

		if (horizontal_icon_alignment != HORIZONTAL_ALIGNMENT_CENTER) {
			minsize.width += icon_size.width;
			if (!p_text_icon.is_empty()) {
				minsize.width += MAX(0, theme_cache.h_separation);
			}
		} else {
			minsize.width = MAX(minsize.width, icon_size.width);
		}
	}

	if (!xl_text.is_empty() || !p_text.is_empty()) {
		Ref<Font> font = theme_cache.font;
		float font_height = font->get_height(theme_cache.font_size);
		if (vertical_icon_alignment == VERTICAL_ALIGNMENT_CENTER) {
			minsize.height = MAX(font_height, minsize.height);
		} else {
			minsize.height += font_height;
		}
	}

	return _get_current_default_stylebox()->get_minimum_size() + minsize;
}

void Button::_shape(Ref<TextParagraph> p_paragraph, String p_text) {
	if (p_paragraph.is_null()) {
		p_paragraph = text_buf;
	}

	if (p_text.is_empty()) {
		p_text = xl_text;
	}

	p_paragraph->clear();

	Ref<Font> font = theme_cache.font;
	int font_size = theme_cache.font_size;
	if (font.is_null() || font_size == 0) {
		// Can't shape without a valid font and a non-zero size.
		return;
	}

	BitField<TextServer::LineBreakFlag> autowrap_flags = TextServer::BREAK_MANDATORY;
	switch (autowrap_mode) {
		case TextServer::AUTOWRAP_WORD_SMART:
			autowrap_flags = TextServer::BREAK_WORD_BOUND | TextServer::BREAK_ADAPTIVE | TextServer::BREAK_MANDATORY;
			break;
		case TextServer::AUTOWRAP_WORD:
			autowrap_flags = TextServer::BREAK_WORD_BOUND | TextServer::BREAK_MANDATORY;
			break;
		case TextServer::AUTOWRAP_ARBITRARY:
			autowrap_flags = TextServer::BREAK_GRAPHEME_BOUND | TextServer::BREAK_MANDATORY;
			break;
		case TextServer::AUTOWRAP_OFF:
			break;
	}
	autowrap_flags = autowrap_flags | TextServer::BREAK_TRIM_EDGE_SPACES;
	p_paragraph->set_break_flags(autowrap_flags);

	if (text_direction == Control::TEXT_DIRECTION_INHERITED) {
		p_paragraph->set_direction(is_layout_rtl() ? TextServer::DIRECTION_RTL : TextServer::DIRECTION_LTR);
	} else {
		p_paragraph->set_direction((TextServer::Direction)text_direction);
	}
	p_paragraph->add_string(p_text, font, font_size, language);
	p_paragraph->set_text_overrun_behavior(overrun_behavior);
}

void Button::_icon_shape(Ref<TextParagraph> p_paragraph, String p_text_icon, int expand_icon_size) {
	if (p_paragraph.is_null()) {
		p_paragraph = text_icon_buf;
	}

	Ref<Font> font = theme_cache.text_icon_font;
	int font_size = theme_cache.text_icon_font_size;

	if (p_text_icon.is_empty()) {
		if(_has_current_text_icon()){
			p_text_icon = _get_current_text_icon();
			String local_name = font->get_path().get_file().get_basename();
			Ref<Translation> trans = TranslationServer::get_singleton()->get_translation_object(local_name);
			if (trans.is_valid()) {
				p_text_icon = trans->get_message(p_text_icon);
				if (!p_text_icon.is_empty()) {
					p_text_icon = String::chr(("0x" + p_text_icon.to_lower()).hex_to_int());
				}
			}
		}
	}

	p_paragraph->clear();

	if (font.is_null() || font_size == 0) {
		// Can't shape without a valid font and a non-zero size.
		return;
	}

	BitField<TextServer::LineBreakFlag> autowrap_flags = TextServer::BREAK_MANDATORY;
	switch (autowrap_mode) {
		case TextServer::AUTOWRAP_WORD_SMART:
			autowrap_flags = TextServer::BREAK_WORD_BOUND | TextServer::BREAK_ADAPTIVE | TextServer::BREAK_MANDATORY;
			break;
		case TextServer::AUTOWRAP_WORD:
			autowrap_flags = TextServer::BREAK_WORD_BOUND | TextServer::BREAK_MANDATORY;
			break;
		case TextServer::AUTOWRAP_ARBITRARY:
			autowrap_flags = TextServer::BREAK_GRAPHEME_BOUND | TextServer::BREAK_MANDATORY;
			break;
		case TextServer::AUTOWRAP_OFF:
			break;
	}
	autowrap_flags = autowrap_flags | TextServer::BREAK_TRIM_EDGE_SPACES;
	p_paragraph->set_break_flags(autowrap_flags);

	if (text_direction == Control::TEXT_DIRECTION_INHERITED) {
		p_paragraph->set_direction(is_layout_rtl() ? TextServer::DIRECTION_RTL : TextServer::DIRECTION_LTR);
	} else {
		p_paragraph->set_direction((TextServer::Direction)text_direction);
	}

	float font_height = font->get_height(font_size);

	int icon_max_width = theme_cache.icon_max_width;
	int max_size = font_height;
	if (icon_max_width > 0) {
		if (max_size > icon_max_width) {
			max_size = icon_max_width;
		}
	}

	if (expand_icon_size != 0) {
		max_size = expand_icon_size;
	}

	p_paragraph->add_string(p_text_icon, font, max_size, language);
	p_paragraph->set_text_overrun_behavior(overrun_behavior);
}

void Button::set_text_overrun_behavior(TextServer::OverrunBehavior p_behavior) {
	if (overrun_behavior != p_behavior) {
		bool need_update_cache = overrun_behavior == TextServer::OVERRUN_NO_TRIMMING || p_behavior == TextServer::OVERRUN_NO_TRIMMING;
		overrun_behavior = p_behavior;
		_shape();
		_icon_shape();

		if (need_update_cache) {
			_queue_update_size_cache();
		}
		queue_redraw();
		update_minimum_size();
	}
}

TextServer::OverrunBehavior Button::get_text_overrun_behavior() const {
	return overrun_behavior;
}

void Button::set_text(const String &p_text) {
	if (text != p_text) {
		text = p_text;
		xl_text = atr(text);
		_shape();

		queue_redraw();
		update_minimum_size();
	}
}

String Button::get_text() const {
	return text;
}

void Button::set_autowrap_mode(TextServer::AutowrapMode p_mode) {
	if (autowrap_mode != p_mode) {
		autowrap_mode = p_mode;
		_shape();
		_icon_shape();
		queue_redraw();
		update_minimum_size();
	}
}

TextServer::AutowrapMode Button::get_autowrap_mode() const {
	return autowrap_mode;
}

void Button::set_text_direction(Control::TextDirection p_text_direction) {
	ERR_FAIL_COND((int)p_text_direction < -1 || (int)p_text_direction > 3);
	if (text_direction != p_text_direction) {
		text_direction = p_text_direction;
		_shape();
		_icon_shape();
		queue_redraw();
	}
}

Control::TextDirection Button::get_text_direction() const {
	return text_direction;
}

void Button::set_language(const String &p_language) {
	if (language != p_language) {
		language = p_language;
		_shape();
		_icon_shape();
		queue_redraw();
	}
}

String Button::get_language() const {
	return language;
}


void Button::set_icon(const Ref<Texture2D> &p_icon) {
	if (icon == p_icon) {
		return;
	}

	if (icon.is_valid()) {
		icon->disconnect_changed(callable_mp(this, &Button::_texture_changed));
	}

	icon = p_icon;

	if (icon.is_valid()) {
		icon->connect_changed(callable_mp(this, &Button::_texture_changed));
	}

	queue_redraw();
	update_minimum_size();
}

void Button::_texture_changed() {
	queue_redraw();
	update_minimum_size();
}

Ref<Texture2D> Button::get_icon() const {
	return icon;
}



void Button::set_expand_icon(bool p_enabled) {
	if (expand_icon != p_enabled) {
		expand_icon = p_enabled;
		_shape();
		_icon_shape();
		_queue_update_size_cache();
		queue_redraw();
		update_minimum_size();
	}
}

bool Button::is_expand_icon() const {
	return expand_icon;
}

void Button::set_flat(bool p_enabled) {
	if (flat != p_enabled) {
		flat = p_enabled;
		queue_redraw();
	}
}

bool Button::is_flat() const {
	return flat;
}

void Button::set_clip_text(bool p_enabled) {
	if (clip_text != p_enabled) {
		clip_text = p_enabled;

		_queue_update_size_cache();
		queue_redraw();
		update_minimum_size();
	}
}

bool Button::get_clip_text() const {
	return clip_text;
}

void Button::set_text_alignment(HorizontalAlignment p_alignment) {
	if (alignment != p_alignment) {
		alignment = p_alignment;
		queue_redraw();
	}
}

HorizontalAlignment Button::get_text_alignment() const {
	return alignment;
}

void Button::set_icon_alignment(HorizontalAlignment p_alignment) {
	if (horizontal_icon_alignment == p_alignment) {
		return;
	}

	horizontal_icon_alignment = p_alignment;
	update_minimum_size();
	queue_redraw();
}

void Button::set_vertical_icon_alignment(VerticalAlignment p_alignment) {
	if (vertical_icon_alignment == p_alignment) {
		return;
	}
	bool need_update_cache = vertical_icon_alignment == VERTICAL_ALIGNMENT_CENTER || p_alignment == VERTICAL_ALIGNMENT_CENTER;
	vertical_icon_alignment = p_alignment;

	if (need_update_cache) {
		_queue_update_size_cache();
	}
	update_minimum_size();
	queue_redraw();
}

HorizontalAlignment Button::get_icon_alignment() const {
	return horizontal_icon_alignment;
}

VerticalAlignment Button::get_vertical_icon_alignment() const {
	return vertical_icon_alignment;
}

void Button::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_text", "text"), &Button::set_text);
	ClassDB::bind_method(D_METHOD("get_text"), &Button::get_text);
	ClassDB::bind_method(D_METHOD("set_text_overrun_behavior", "overrun_behavior"), &Button::set_text_overrun_behavior);
	ClassDB::bind_method(D_METHOD("get_text_overrun_behavior"), &Button::get_text_overrun_behavior);
	ClassDB::bind_method(D_METHOD("set_autowrap_mode", "autowrap_mode"), &Button::set_autowrap_mode);
	ClassDB::bind_method(D_METHOD("get_autowrap_mode"), &Button::get_autowrap_mode);
	ClassDB::bind_method(D_METHOD("set_text_direction", "direction"), &Button::set_text_direction);
	ClassDB::bind_method(D_METHOD("get_text_direction"), &Button::get_text_direction);
	ClassDB::bind_method(D_METHOD("set_language", "language"), &Button::set_language);
	ClassDB::bind_method(D_METHOD("get_language"), &Button::get_language);
	ClassDB::bind_method(D_METHOD("set_button_icon", "texture"), &Button::set_icon);
	ClassDB::bind_method(D_METHOD("get_button_icon"), &Button::get_icon);
	ClassDB::bind_method(D_METHOD("set_flat", "enabled"), &Button::set_flat);
	ClassDB::bind_method(D_METHOD("is_flat"), &Button::is_flat);
	ClassDB::bind_method(D_METHOD("set_clip_text", "enabled"), &Button::set_clip_text);
	ClassDB::bind_method(D_METHOD("get_clip_text"), &Button::get_clip_text);
	ClassDB::bind_method(D_METHOD("set_text_alignment", "alignment"), &Button::set_text_alignment);
	ClassDB::bind_method(D_METHOD("get_text_alignment"), &Button::get_text_alignment);
	ClassDB::bind_method(D_METHOD("set_icon_alignment", "icon_alignment"), &Button::set_icon_alignment);
	ClassDB::bind_method(D_METHOD("get_icon_alignment"), &Button::get_icon_alignment);
	ClassDB::bind_method(D_METHOD("set_vertical_icon_alignment", "vertical_icon_alignment"), &Button::set_vertical_icon_alignment);
	ClassDB::bind_method(D_METHOD("get_vertical_icon_alignment"), &Button::get_vertical_icon_alignment);
	ClassDB::bind_method(D_METHOD("set_expand_icon", "enabled"), &Button::set_expand_icon);
	ClassDB::bind_method(D_METHOD("is_expand_icon"), &Button::is_expand_icon);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "text", PROPERTY_HINT_MULTILINE_TEXT), "set_text", "get_text");
	// ADD_PROPERTY(PropertyInfo(Variant::STRING, "text_icon", PROPERTY_HINT_MULTILINE_TEXT), "set_text_icon", "get_text_icon");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "icon", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_button_icon", "get_button_icon");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flat"), "set_flat", "is_flat");

	ADD_GROUP("Text Behavior", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "alignment", PROPERTY_HINT_ENUM, "Left,Center,Right"), "set_text_alignment", "get_text_alignment");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "text_overrun_behavior", PROPERTY_HINT_ENUM, "Trim Nothing,Trim Characters,Trim Words,Ellipsis,Word Ellipsis"), "set_text_overrun_behavior", "get_text_overrun_behavior");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "autowrap_mode", PROPERTY_HINT_ENUM, "Off,Arbitrary,Word,Word (Smart)"), "set_autowrap_mode", "get_autowrap_mode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "clip_text"), "set_clip_text", "get_clip_text");

	ADD_GROUP("Icon Behavior", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "icon_alignment", PROPERTY_HINT_ENUM, "Left,Center,Right"), "set_icon_alignment", "get_icon_alignment");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "vertical_icon_alignment", PROPERTY_HINT_ENUM, "Top,Center,Bottom"), "set_vertical_icon_alignment", "get_vertical_icon_alignment");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "expand_icon"), "set_expand_icon", "is_expand_icon");

	ADD_GROUP("BiDi", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "text_direction", PROPERTY_HINT_ENUM, "Auto,Left-to-Right,Right-to-Left,Inherited"), "set_text_direction", "get_text_direction");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "language", PROPERTY_HINT_LOCALE_ID, ""), "set_language", "get_language");

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, Button, default_color_scheme);

	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_STYLEBOX, Button, default_stylebox);
	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_STYLEBOX, Button, state_layer_stylebox);

	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_COLOR, Button, font_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, Button, font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, Button, font_size);

	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_COLOR, Button, font_outline_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, Button, font_outline_size);

	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, Button, text_icon_font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, Button, text_icon_font_size);

	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_ICON, Button, icon);
	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_COLOR, Button, icon_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, Button, icon_max_width);

	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_STR, Button, text_icon);
	BIND_THEME_ITEM_MULTI(Theme::DATA_TYPE_COLOR, Button, text_icon_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, Button, h_separation);	
}

Button::Button(const String &p_text) {
	text_buf.instantiate();
	text_buf->set_break_flags(TextServer::BREAK_MANDATORY | TextServer::BREAK_TRIM_EDGE_SPACES);

	text_icon_buf.instantiate();
	text_icon_buf->set_break_flags(TextServer::BREAK_MANDATORY | TextServer::BREAK_TRIM_EDGE_SPACES);

	set_mouse_filter(MOUSE_FILTER_STOP);

	set_text(p_text);
	// set_text_icon("");
}

Button::~Button() {
}

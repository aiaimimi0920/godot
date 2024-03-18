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

#include "core/string/translation.h"
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
		tex_size = Size2(on_tex->get_width(), on_tex->get_height());
	}
	if (!off_tex.is_null()) {
		tex_size = Size2(MAX(tex_size.width, off_tex->get_width()), MAX(tex_size.height, off_tex->get_height()));
	}

	if (tex_size == Size2(0, 0)) {
		tex_size = Size2(theme_cache.text_icon_font_size, theme_cache.text_icon_font_size);

		if (theme_cache.icon_max_width > 0) {
			tex_size = Size2(MIN(tex_size.width, theme_cache.icon_max_width), MIN(tex_size.width, theme_cache.icon_max_width));
		}
	}

	return tex_size;
}

Size2 CheckButton::get_minimum_size() const {
	Size2 minsize = Button::get_minimum_size();
	const Size2 tex_size = get_icon_size();
	if (tex_size.width > 0 || tex_size.height > 0) {
		const Size2 padding = _get_current_default_stylebox()->get_minimum_size();
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

Ref<StyleBox> CheckButton::_get_focus_state_layer_stylebox() const {
	Ref<StyleBox> style;
	if (is_pressed()) {
		style = theme_cache.selected_check_focus_state_layer;
	} else {
		style = theme_cache.unselected_check_focus_state_layer;
	}
	return style;
}

Ref<StyleBox> CheckButton::_get_current_state_layer_stylebox() const {
	Ref<StyleBox> stylebox = theme_cache.selected_check_hover_pressed_state_layer;
	const bool rtl = is_layout_rtl();

	switch (get_draw_mode()) {
		case DRAW_HOVER_PRESSED: {
			if (is_pressed()) {
				if (has_theme_stylebox("selected_check_hover_pressed_state_layer")) {
					if (rtl && has_theme_stylebox(SNAME("selected_check_hover_pressed_state_layer_mirrored"))) {
						stylebox = theme_cache.selected_check_hover_pressed_state_layer_mirrored;
					} else {
						stylebox = theme_cache.selected_check_hover_pressed_state_layer;
					}
					break;
				}
			} else {
				if (has_theme_stylebox("hover_pressed_state_layer")) {
					if (rtl && has_theme_stylebox(SNAME("unselected_check_hover_pressed_state_layer_mirrored"))) {
						stylebox = theme_cache.unselected_check_hover_pressed_state_layer_mirrored;
					} else {
						stylebox = theme_cache.unselected_check_hover_pressed_state_layer;
					}
					break;
				}
			}
		}
			[[fallthrough]];
		case DRAW_PRESSED: {
			if (is_pressed()) {
				if (rtl && has_theme_stylebox(SNAME("selected_check_pressed_state_layer_mirrored"))) {
					stylebox = theme_cache.selected_check_pressed_state_layer_mirrored;
				} else {
					stylebox = theme_cache.selected_check_pressed_state_layer;
				}
			} else {
				if (rtl && has_theme_stylebox(SNAME("unselected_check_pressed_state_layer_mirrored"))) {
					stylebox = theme_cache.unselected_check_pressed_state_layer_mirrored;
				} else {
					stylebox = theme_cache.unselected_check_pressed_state_layer;
				}
			}
		} break;

		case DRAW_HOVER: {
			if (is_pressed()) {
				if (rtl && has_theme_stylebox(SNAME("selected_check_hover_state_layer_mirrored"))) {
					stylebox = theme_cache.selected_check_hover_state_layer_mirrored;
				} else {
					stylebox = theme_cache.selected_check_hover_state_layer;
				}
			} else {
				if (rtl && has_theme_stylebox(SNAME("unselected_check_hover_state_layer_mirrored"))) {
					stylebox = theme_cache.unselected_check_hover_state_layer_mirrored;
				} else {
					stylebox = theme_cache.unselected_check_hover_state_layer;
				}
			}

		} break;
	}
	return stylebox;
}

void CheckButton::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_THEME_CHANGED: {
			update_minimum_size();
			queue_redraw();
		} break;
		case NOTIFICATION_LAYOUT_DIRECTION_CHANGED:
		case NOTIFICATION_TRANSLATION_CHANGED: {
			if (is_layout_rtl()) {
				_set_internal_margin(SIDE_LEFT, get_icon_size().width);
				_set_internal_margin(SIDE_RIGHT, 0.f);
			} else {
				_set_internal_margin(SIDE_LEFT, 0.f);
				_set_internal_margin(SIDE_RIGHT, get_icon_size().width);
			}
			update_minimum_size();
			queue_redraw();
		} break;

		case NOTIFICATION_DRAW: {
			RID ci = get_canvas_item();
			bool rtl = is_layout_rtl();

			Ref<Texture2D> on_tex;
			Ref<Texture2D> off_tex;

			if (rtl) {
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

			Vector2 ofs;
			Size2 tex_size = get_icon_size();

			if (rtl) {
				ofs.x = theme_cache.normal_style->get_margin(SIDE_LEFT);
			} else {
				ofs.x = get_size().width - (tex_size.width + theme_cache.normal_style->get_margin(SIDE_RIGHT));
			}
			ofs.y = (get_size().height - tex_size.height) / 2 + theme_cache.check_v_offset;

			String cur_text_icon;
			String cur_code_text_icon;

			String cur_bg_text_icon;
			String cur_code_bg_text_icon;

			switch (get_draw_mode()) {
				case DRAW_NORMAL: {
					if (is_layout_rtl()) {
						if (is_pressed()) {
							if (!selected_check_text_icon_normal_mirrored.is_empty()) {
								cur_text_icon = selected_check_text_icon_normal_mirrored;
								if (code_selected_check_text_icon_normal_mirrored.is_empty()) {
									code_selected_check_text_icon_normal_mirrored = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_selected_check_text_icon_normal_mirrored;
							}

							if (!selected_bg_check_text_icon_normal_mirrored.is_empty()) {
								cur_bg_text_icon = selected_bg_check_text_icon_normal_mirrored;
								if (code_selected_bg_check_text_icon_normal_mirrored.is_empty()) {
									code_selected_bg_check_text_icon_normal_mirrored = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_selected_bg_check_text_icon_normal_mirrored;
							}

						} else {
							if (!unselected_check_text_icon_normal_mirrored.is_empty()) {
								cur_text_icon = unselected_check_text_icon_normal_mirrored;
								if (code_unselected_check_text_icon_normal_mirrored.is_empty()) {
									code_unselected_check_text_icon_normal_mirrored = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_unselected_check_text_icon_normal_mirrored;
							}

							if (!unselected_bg_check_text_icon_normal_mirrored.is_empty()) {
								cur_bg_text_icon = unselected_bg_check_text_icon_normal_mirrored;
								if (code_unselected_bg_check_text_icon_normal_mirrored.is_empty()) {
									code_unselected_bg_check_text_icon_normal_mirrored = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_unselected_bg_check_text_icon_normal_mirrored;
							}
						}
					} else {
						if (is_pressed()) {
							if (!selected_check_text_icon_normal.is_empty()) {
								cur_text_icon = selected_check_text_icon_normal;
								if (code_selected_check_text_icon_normal.is_empty()) {
									code_selected_check_text_icon_normal = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_selected_check_text_icon_normal;
							}
							if (!selected_bg_check_text_icon_normal.is_empty()) {
								cur_bg_text_icon = selected_bg_check_text_icon_normal;
								if (code_selected_bg_check_text_icon_normal.is_empty()) {
									code_selected_bg_check_text_icon_normal = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_selected_bg_check_text_icon_normal;
							}

						} else {
							if (!unselected_check_text_icon_normal.is_empty()) {
								cur_text_icon = unselected_check_text_icon_normal;
								if (code_unselected_check_text_icon_normal.is_empty()) {
									code_unselected_check_text_icon_normal = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_unselected_check_text_icon_normal;
							}
							if (!unselected_bg_check_text_icon_normal.is_empty()) {
								cur_bg_text_icon = unselected_bg_check_text_icon_normal;
								if (code_unselected_bg_check_text_icon_normal.is_empty()) {
									code_unselected_bg_check_text_icon_normal = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_unselected_bg_check_text_icon_normal;
							}
						}
					}

				} break;
				case DRAW_HOVER_PRESSED:
				case DRAW_PRESSED: {
					if (is_layout_rtl()) {
						if (is_pressed()) {
							if (selected_check_text_icon_pressed_mirrored.is_empty()) {
								if (selected_check_text_icon_hover_mirrored.is_empty()) {
									if (selected_check_text_icon_normal_mirrored.is_empty()) {
										cur_text_icon = selected_check_text_icon_normal_mirrored;
										if (code_selected_check_text_icon_normal_mirrored.is_empty()) {
											code_selected_check_text_icon_normal_mirrored = _get_trans_text(cur_text_icon);
										}
										cur_code_text_icon = code_selected_check_text_icon_normal_mirrored;
									}
								} else {
									cur_text_icon = selected_check_text_icon_hover_mirrored;
									if (code_selected_check_text_icon_hover_mirrored.is_empty()) {
										code_selected_check_text_icon_hover_mirrored = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_selected_check_text_icon_hover_mirrored;
								}
							} else {
								cur_text_icon = selected_check_text_icon_pressed_mirrored;
								if (code_selected_check_text_icon_pressed_mirrored.is_empty()) {
									code_selected_check_text_icon_pressed_mirrored = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_selected_check_text_icon_pressed_mirrored;
							}

							if (selected_bg_check_text_icon_pressed_mirrored.is_empty()) {
								if (selected_bg_check_text_icon_hover_mirrored.is_empty()) {
									if (selected_bg_check_text_icon_normal_mirrored.is_empty()) {
										cur_bg_text_icon = selected_bg_check_text_icon_normal_mirrored;
										if (code_selected_bg_check_text_icon_normal_mirrored.is_empty()) {
											code_selected_bg_check_text_icon_normal_mirrored = _get_trans_text(cur_bg_text_icon);
										}
										cur_code_bg_text_icon = code_selected_bg_check_text_icon_normal_mirrored;
									}
								} else {
									cur_bg_text_icon = selected_bg_check_text_icon_hover_mirrored;
									if (code_selected_bg_check_text_icon_hover_mirrored.is_empty()) {
										code_selected_bg_check_text_icon_hover_mirrored = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_selected_bg_check_text_icon_hover_mirrored;
								}
							} else {
								cur_bg_text_icon = selected_bg_check_text_icon_pressed_mirrored;
								if (code_selected_bg_check_text_icon_pressed_mirrored.is_empty()) {
									code_selected_bg_check_text_icon_pressed_mirrored = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_selected_bg_check_text_icon_pressed_mirrored;
							}

						} else {
							if (unselected_check_text_icon_pressed_mirrored.is_empty()) {
								if (unselected_check_text_icon_hover_mirrored.is_empty()) {
									if (unselected_check_text_icon_normal_mirrored.is_empty()) {
										cur_text_icon = unselected_check_text_icon_normal_mirrored;
										if (code_unselected_check_text_icon_normal_mirrored.is_empty()) {
											code_unselected_check_text_icon_normal_mirrored = _get_trans_text(cur_text_icon);
										}
										cur_code_text_icon = code_unselected_check_text_icon_normal_mirrored;
									}
								} else {
									cur_text_icon = unselected_check_text_icon_hover_mirrored;
									if (code_unselected_check_text_icon_hover_mirrored.is_empty()) {
										code_unselected_check_text_icon_hover_mirrored = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_unselected_check_text_icon_hover_mirrored;
								}
							} else {
								cur_text_icon = unselected_check_text_icon_pressed_mirrored;
								if (code_unselected_check_text_icon_pressed_mirrored.is_empty()) {
									code_unselected_check_text_icon_pressed_mirrored = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_unselected_check_text_icon_pressed_mirrored;
							}

							if (unselected_bg_check_text_icon_pressed_mirrored.is_empty()) {
								if (unselected_bg_check_text_icon_hover_mirrored.is_empty()) {
									if (unselected_bg_check_text_icon_normal_mirrored.is_empty()) {
										cur_bg_text_icon = unselected_bg_check_text_icon_normal_mirrored;
										if (code_unselected_bg_check_text_icon_normal_mirrored.is_empty()) {
											code_unselected_bg_check_text_icon_normal_mirrored = _get_trans_text(cur_bg_text_icon);
										}
										cur_code_bg_text_icon = code_unselected_bg_check_text_icon_normal_mirrored;
									}
								} else {
									cur_bg_text_icon = unselected_bg_check_text_icon_hover_mirrored;
									if (code_unselected_bg_check_text_icon_hover_mirrored.is_empty()) {
										code_unselected_bg_check_text_icon_hover_mirrored = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_unselected_bg_check_text_icon_hover_mirrored;
								}
							} else {
								cur_bg_text_icon = unselected_bg_check_text_icon_pressed_mirrored;
								if (code_unselected_bg_check_text_icon_pressed_mirrored.is_empty()) {
									code_unselected_bg_check_text_icon_pressed_mirrored = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_unselected_bg_check_text_icon_pressed_mirrored;
							}
						}
					} else {
						if (is_pressed()) {
							if (selected_check_text_icon_pressed.is_empty()) {
								if (selected_check_text_icon_hover.is_empty()) {
									if (selected_check_text_icon_normal.is_empty()) {
										cur_text_icon = selected_check_text_icon_normal;
										if (code_selected_check_text_icon_normal.is_empty()) {
											code_selected_check_text_icon_normal = _get_trans_text(cur_text_icon);
										}
										cur_code_text_icon = code_selected_check_text_icon_normal;
									}
								} else {
									cur_text_icon = selected_check_text_icon_hover;
									if (code_selected_check_text_icon_hover.is_empty()) {
										code_selected_check_text_icon_hover = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_selected_check_text_icon_hover;
								}
							} else {
								cur_text_icon = selected_check_text_icon_pressed;
								if (code_selected_check_text_icon_pressed.is_empty()) {
									code_selected_check_text_icon_pressed = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_selected_check_text_icon_pressed;
							}

							if (selected_bg_check_text_icon_pressed.is_empty()) {
								if (selected_bg_check_text_icon_hover.is_empty()) {
									if (selected_bg_check_text_icon_normal.is_empty()) {
										cur_bg_text_icon = selected_bg_check_text_icon_normal;
										if (code_selected_bg_check_text_icon_normal.is_empty()) {
											code_selected_bg_check_text_icon_normal = _get_trans_text(cur_bg_text_icon);
										}
										cur_code_bg_text_icon = code_selected_bg_check_text_icon_normal;
									}
								} else {
									cur_bg_text_icon = selected_bg_check_text_icon_hover;
									if (code_selected_bg_check_text_icon_hover.is_empty()) {
										code_selected_bg_check_text_icon_hover = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_selected_bg_check_text_icon_hover;
								}
							} else {
								cur_bg_text_icon = selected_bg_check_text_icon_pressed;
								if (code_selected_bg_check_text_icon_pressed.is_empty()) {
									code_selected_bg_check_text_icon_pressed = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_selected_bg_check_text_icon_pressed;
							}

						} else {
							if (unselected_check_text_icon_pressed.is_empty()) {
								if (unselected_check_text_icon_hover.is_empty()) {
									if (unselected_check_text_icon_normal.is_empty()) {
										cur_text_icon = unselected_check_text_icon_normal;
										if (code_unselected_check_text_icon_normal.is_empty()) {
											code_unselected_check_text_icon_normal = _get_trans_text(cur_text_icon);
										}
										cur_code_text_icon = code_unselected_check_text_icon_normal;
									}
								} else {
									cur_text_icon = unselected_check_text_icon_hover;
									if (code_unselected_check_text_icon_hover.is_empty()) {
										code_unselected_check_text_icon_hover = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_unselected_check_text_icon_hover;
								}
							} else {
								cur_text_icon = unselected_check_text_icon_pressed;
								if (code_unselected_check_text_icon_pressed.is_empty()) {
									code_unselected_check_text_icon_pressed = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_unselected_check_text_icon_pressed;
							}

							if (unselected_bg_check_text_icon_pressed.is_empty()) {
								if (unselected_bg_check_text_icon_hover.is_empty()) {
									if (unselected_bg_check_text_icon_normal.is_empty()) {
										cur_bg_text_icon = unselected_bg_check_text_icon_normal;
										if (code_unselected_bg_check_text_icon_normal.is_empty()) {
											code_unselected_bg_check_text_icon_normal = _get_trans_text(cur_bg_text_icon);
										}
										cur_code_bg_text_icon = code_unselected_bg_check_text_icon_normal;
									}
								} else {
									cur_bg_text_icon = unselected_bg_check_text_icon_hover;
									if (code_unselected_bg_check_text_icon_hover.is_empty()) {
										code_unselected_bg_check_text_icon_hover = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_unselected_bg_check_text_icon_hover;
								}
							} else {
								cur_bg_text_icon = unselected_bg_check_text_icon_pressed;
								if (code_unselected_bg_check_text_icon_pressed.is_empty()) {
									code_unselected_bg_check_text_icon_pressed = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_unselected_bg_check_text_icon_pressed;
							}
						}
					}

				} break;
				case DRAW_HOVER: {
					if (is_layout_rtl()) {
						if (is_pressed()) {
							if (selected_check_text_icon_hover_mirrored.is_empty()) {
								if (!selected_check_text_icon_pressed_mirrored.is_empty() && is_pressed()) {
									cur_text_icon = selected_check_text_icon_pressed_mirrored;
									if (code_selected_check_text_icon_pressed_mirrored.is_empty()) {
										code_selected_check_text_icon_pressed_mirrored = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_selected_check_text_icon_pressed_mirrored;
								} else if (!selected_check_text_icon_normal_mirrored.is_empty()) {
									cur_text_icon = selected_check_text_icon_normal_mirrored;
									if (code_selected_check_text_icon_normal_mirrored.is_empty()) {
										code_selected_check_text_icon_normal_mirrored = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_selected_check_text_icon_normal_mirrored;
								}
							} else {
								cur_text_icon = selected_check_text_icon_hover_mirrored;
								if (code_selected_check_text_icon_hover_mirrored.is_empty()) {
									code_selected_check_text_icon_hover_mirrored = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_selected_check_text_icon_hover_mirrored;
							}
							if (selected_bg_check_text_icon_hover_mirrored.is_empty()) {
								if (!selected_bg_check_text_icon_pressed_mirrored.is_empty() && is_pressed()) {
									cur_bg_text_icon = selected_bg_check_text_icon_pressed_mirrored;
									if (code_selected_bg_check_text_icon_pressed_mirrored.is_empty()) {
										code_selected_bg_check_text_icon_pressed_mirrored = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_selected_bg_check_text_icon_pressed_mirrored;
								} else if (!selected_bg_check_text_icon_normal_mirrored.is_empty()) {
									cur_bg_text_icon = selected_bg_check_text_icon_normal_mirrored;
									if (code_selected_bg_check_text_icon_normal_mirrored.is_empty()) {
										code_selected_bg_check_text_icon_normal_mirrored = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_selected_bg_check_text_icon_normal_mirrored;
								}
							} else {
								cur_bg_text_icon = selected_bg_check_text_icon_hover_mirrored;
								if (code_selected_bg_check_text_icon_hover_mirrored.is_empty()) {
									code_selected_bg_check_text_icon_hover_mirrored = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_selected_bg_check_text_icon_hover_mirrored;
							}
						} else {
							if (unselected_check_text_icon_hover_mirrored.is_empty()) {
								if (!unselected_check_text_icon_pressed_mirrored.is_empty() && is_pressed()) {
									cur_text_icon = unselected_check_text_icon_pressed_mirrored;
									if (code_unselected_check_text_icon_pressed_mirrored.is_empty()) {
										code_unselected_check_text_icon_pressed_mirrored = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_unselected_check_text_icon_pressed_mirrored;
								} else if (!unselected_check_text_icon_normal_mirrored.is_empty()) {
									cur_text_icon = unselected_check_text_icon_normal_mirrored;
									if (code_unselected_check_text_icon_normal_mirrored.is_empty()) {
										code_unselected_check_text_icon_normal_mirrored = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_unselected_check_text_icon_normal_mirrored;
								}
							} else {
								cur_text_icon = unselected_check_text_icon_hover_mirrored;
								if (code_unselected_check_text_icon_hover_mirrored.is_empty()) {
									code_unselected_check_text_icon_hover_mirrored = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_unselected_check_text_icon_hover_mirrored;
							}

							if (unselected_bg_check_text_icon_hover_mirrored.is_empty()) {
								if (!unselected_bg_check_text_icon_pressed_mirrored.is_empty() && is_pressed()) {
									cur_bg_text_icon = unselected_bg_check_text_icon_pressed_mirrored;
									if (code_unselected_bg_check_text_icon_pressed_mirrored.is_empty()) {
										code_unselected_bg_check_text_icon_pressed_mirrored = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_unselected_bg_check_text_icon_pressed_mirrored;
								} else if (!unselected_bg_check_text_icon_normal_mirrored.is_empty()) {
									cur_bg_text_icon = unselected_bg_check_text_icon_normal_mirrored;
									if (code_unselected_bg_check_text_icon_normal_mirrored.is_empty()) {
										code_unselected_bg_check_text_icon_normal_mirrored = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_unselected_bg_check_text_icon_normal_mirrored;
								}
							} else {
								cur_bg_text_icon = unselected_bg_check_text_icon_hover_mirrored;
								if (code_unselected_bg_check_text_icon_hover_mirrored.is_empty()) {
									code_unselected_bg_check_text_icon_hover_mirrored = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_unselected_bg_check_text_icon_hover_mirrored;
							}
						}
					} else {
						if (is_pressed()) {
							if (selected_check_text_icon_hover.is_empty()) {
								if (!selected_check_text_icon_pressed.is_empty() && is_pressed()) {
									cur_text_icon = selected_check_text_icon_pressed;
									if (code_selected_check_text_icon_pressed.is_empty()) {
										code_selected_check_text_icon_pressed = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_selected_check_text_icon_pressed;
								} else if (!selected_check_text_icon_normal.is_empty()) {
									cur_text_icon = selected_check_text_icon_normal;
									if (code_selected_check_text_icon_normal.is_empty()) {
										code_selected_check_text_icon_normal = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_selected_check_text_icon_normal;
								}
							} else {
								cur_text_icon = selected_check_text_icon_hover;
								if (code_selected_check_text_icon_hover.is_empty()) {
									code_selected_check_text_icon_hover = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_selected_check_text_icon_hover;
							}

							if (selected_bg_check_text_icon_hover.is_empty()) {
								if (!selected_bg_check_text_icon_pressed.is_empty() && is_pressed()) {
									cur_bg_text_icon = selected_bg_check_text_icon_pressed;
									if (code_selected_bg_check_text_icon_pressed.is_empty()) {
										code_selected_bg_check_text_icon_pressed = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_selected_bg_check_text_icon_pressed;
								} else if (!selected_bg_check_text_icon_normal.is_empty()) {
									cur_bg_text_icon = selected_bg_check_text_icon_normal;
									if (code_selected_bg_check_text_icon_normal.is_empty()) {
										code_selected_bg_check_text_icon_normal = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_selected_bg_check_text_icon_normal;
								}
							} else {
								cur_bg_text_icon = selected_bg_check_text_icon_hover;
								if (code_selected_bg_check_text_icon_hover.is_empty()) {
									code_selected_bg_check_text_icon_hover = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_selected_bg_check_text_icon_hover;
							}

						} else {
							if (unselected_check_text_icon_hover.is_empty()) {
								if (!unselected_check_text_icon_pressed.is_empty() && is_pressed()) {
									cur_text_icon = unselected_check_text_icon_pressed;
									if (code_unselected_check_text_icon_pressed.is_empty()) {
										code_unselected_check_text_icon_pressed = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_unselected_check_text_icon_pressed;
								} else if (!unselected_check_text_icon_normal.is_empty()) {
									cur_text_icon = unselected_check_text_icon_normal;
									if (code_unselected_check_text_icon_normal.is_empty()) {
										code_unselected_check_text_icon_normal = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_unselected_check_text_icon_normal;
								}
							} else {
								cur_text_icon = unselected_check_text_icon_hover;
								if (code_unselected_check_text_icon_hover.is_empty()) {
									code_unselected_check_text_icon_hover = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_unselected_check_text_icon_hover;
							}

							if (unselected_bg_check_text_icon_hover.is_empty()) {
								if (!unselected_bg_check_text_icon_pressed.is_empty() && is_pressed()) {
									cur_bg_text_icon = unselected_bg_check_text_icon_pressed;
									if (code_unselected_bg_check_text_icon_pressed.is_empty()) {
										code_unselected_bg_check_text_icon_pressed = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_unselected_bg_check_text_icon_pressed;
								} else if (!unselected_bg_check_text_icon_normal.is_empty()) {
									cur_bg_text_icon = unselected_bg_check_text_icon_normal;
									if (code_unselected_bg_check_text_icon_normal.is_empty()) {
										code_unselected_bg_check_text_icon_normal = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_unselected_bg_check_text_icon_normal;
								}
							} else {
								cur_bg_text_icon = unselected_bg_check_text_icon_hover;
								if (code_unselected_bg_check_text_icon_hover.is_empty()) {
									code_unselected_bg_check_text_icon_hover = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_unselected_bg_check_text_icon_hover;
							}
						}
					}

				} break;
				case DRAW_DISABLED: {
					if (is_layout_rtl()) {
						if (is_pressed()) {
							if (selected_check_text_icon_disabled_mirrored.is_empty()) {
								if (!selected_check_text_icon_normal_mirrored.is_empty()) {
									cur_text_icon = selected_check_text_icon_normal_mirrored;
									if (code_selected_check_text_icon_normal_mirrored.is_empty()) {
										code_selected_check_text_icon_normal_mirrored = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_selected_check_text_icon_normal_mirrored;
								}
							} else {
								cur_text_icon = selected_check_text_icon_disabled_mirrored;
								if (code_selected_check_text_icon_disabled_mirrored.is_empty()) {
									code_selected_check_text_icon_disabled_mirrored = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_selected_check_text_icon_disabled_mirrored;
							}

							if (selected_bg_check_text_icon_disabled_mirrored.is_empty()) {
								if (!selected_bg_check_text_icon_normal_mirrored.is_empty()) {
									cur_bg_text_icon = selected_bg_check_text_icon_normal_mirrored;
									if (code_selected_bg_check_text_icon_normal_mirrored.is_empty()) {
										code_selected_bg_check_text_icon_normal_mirrored = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_selected_bg_check_text_icon_normal_mirrored;
								}
							} else {
								cur_bg_text_icon = selected_bg_check_text_icon_disabled_mirrored;
								if (code_selected_bg_check_text_icon_disabled_mirrored.is_empty()) {
									code_selected_bg_check_text_icon_disabled_mirrored = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_selected_bg_check_text_icon_disabled_mirrored;
							}

						} else {
							if (unselected_check_text_icon_disabled_mirrored.is_empty()) {
								if (!unselected_check_text_icon_normal_mirrored.is_empty()) {
									cur_text_icon = unselected_check_text_icon_normal_mirrored;
									if (code_unselected_check_text_icon_normal_mirrored.is_empty()) {
										code_unselected_check_text_icon_normal_mirrored = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_unselected_check_text_icon_normal_mirrored;
								}
							} else {
								cur_text_icon = unselected_check_text_icon_disabled_mirrored;
								if (code_unselected_check_text_icon_disabled_mirrored.is_empty()) {
									code_unselected_check_text_icon_disabled_mirrored = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_unselected_check_text_icon_disabled_mirrored;
							}

							if (unselected_bg_check_text_icon_disabled_mirrored.is_empty()) {
								if (!unselected_bg_check_text_icon_normal_mirrored.is_empty()) {
									cur_bg_text_icon = unselected_bg_check_text_icon_normal_mirrored;
									if (code_unselected_bg_check_text_icon_normal_mirrored.is_empty()) {
										code_unselected_bg_check_text_icon_normal_mirrored = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_unselected_bg_check_text_icon_normal_mirrored;
								}
							} else {
								cur_bg_text_icon = unselected_bg_check_text_icon_disabled_mirrored;
								if (code_unselected_bg_check_text_icon_disabled_mirrored.is_empty()) {
									code_unselected_bg_check_text_icon_disabled_mirrored = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_unselected_bg_check_text_icon_disabled_mirrored;
							}
						}
					} else {
						if (is_pressed()) {
							if (selected_check_text_icon_disabled.is_empty()) {
								if (!selected_check_text_icon_normal.is_empty()) {
									cur_text_icon = selected_check_text_icon_normal;
									if (code_selected_check_text_icon_normal.is_empty()) {
										code_selected_check_text_icon_normal = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_selected_check_text_icon_normal;
								}
							} else {
								cur_text_icon = selected_check_text_icon_disabled;
								if (code_selected_check_text_icon_disabled.is_empty()) {
									code_selected_check_text_icon_disabled = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_selected_check_text_icon_disabled;
							}

							if (selected_bg_check_text_icon_disabled.is_empty()) {
								if (!selected_bg_check_text_icon_normal.is_empty()) {
									cur_bg_text_icon = selected_bg_check_text_icon_normal;
									if (code_selected_bg_check_text_icon_normal.is_empty()) {
										code_selected_bg_check_text_icon_normal = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_selected_bg_check_text_icon_normal;
								}
							} else {
								cur_bg_text_icon = selected_bg_check_text_icon_disabled;
								if (code_selected_bg_check_text_icon_disabled.is_empty()) {
									code_selected_bg_check_text_icon_disabled = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_selected_bg_check_text_icon_disabled;
							}

						} else {
							if (unselected_check_text_icon_disabled.is_empty()) {
								if (!unselected_check_text_icon_normal.is_empty()) {
									cur_text_icon = unselected_check_text_icon_normal;
									if (code_unselected_check_text_icon_normal.is_empty()) {
										code_unselected_check_text_icon_normal = _get_trans_text(cur_text_icon);
									}
									cur_code_text_icon = code_unselected_check_text_icon_normal;
								}
							} else {
								cur_text_icon = unselected_check_text_icon_disabled;
								if (code_unselected_check_text_icon_disabled.is_empty()) {
									code_unselected_check_text_icon_disabled = _get_trans_text(cur_text_icon);
								}
								cur_code_text_icon = code_unselected_check_text_icon_disabled;
							}

							if (unselected_bg_check_text_icon_disabled.is_empty()) {
								if (!unselected_bg_check_text_icon_normal.is_empty()) {
									cur_bg_text_icon = unselected_bg_check_text_icon_normal;
									if (code_unselected_bg_check_text_icon_normal.is_empty()) {
										code_unselected_bg_check_text_icon_normal = _get_trans_text(cur_bg_text_icon);
									}
									cur_code_bg_text_icon = code_unselected_bg_check_text_icon_normal;
								}
							} else {
								cur_bg_text_icon = unselected_bg_check_text_icon_disabled;
								if (code_unselected_bg_check_text_icon_disabled.is_empty()) {
									code_unselected_bg_check_text_icon_disabled = _get_trans_text(cur_bg_text_icon);
								}
								cur_code_bg_text_icon = code_unselected_bg_check_text_icon_disabled;
							}
						}
					}

				} break;
			}

			Ref<Font> font = theme_cache.text_icon_font;

			if (cur_text_icon != "" && cur_bg_text_icon != "" && !font->get_path().get_file().get_basename().is_empty()) {
				Color text_icon_font_color;
				Color bg_text_icon_font_color;
				// Get the font color and icon modulate color in the current state.
				switch (get_draw_mode()) {
					case DRAW_NORMAL: {
						// Focus colors only take precedence over normal state.
						if (has_focus()) {
							if (is_pressed()) {
								if (has_theme_color(SNAME("selected_check_text_icon_focus_color"))) {
									text_icon_font_color = theme_cache.selected_check_text_icon_focus_color;
								}
							} else {
								if (has_theme_color(SNAME("unselected_check_text_icon_focus_color"))) {
									text_icon_font_color = theme_cache.unselected_check_text_icon_focus_color;
								}
							}
						} else {
							if (is_pressed()) {
								if (has_theme_color(SNAME("selected_check_text_icon_normal_color"))) {
									text_icon_font_color = theme_cache.selected_check_text_icon_normal_color;
								}
							} else {
								if (has_theme_color(SNAME("unselected_check_text_icon_normal_color"))) {
									text_icon_font_color = theme_cache.unselected_check_text_icon_normal_color;
								}
							}
						}
					} break;
					case DRAW_HOVER_PRESSED: {
						// Edge case for CheckButton and CheckBox.
						if (is_pressed()) {
							if (has_theme_stylebox("selected_check_hover_pressed")) {
								if (has_theme_color(SNAME("selected_check_text_icon_hover_pressed_color"))) {
									text_icon_font_color = theme_cache.selected_check_text_icon_hover_pressed_color;
								}
								break;
							}
						} else {
							if (has_theme_stylebox("unselected_check_hover_pressed")) {
								if (has_theme_color(SNAME("unselected_check_text_icon_hover_pressed_color"))) {
									text_icon_font_color = theme_cache.unselected_check_text_icon_hover_pressed_color;
								}
								break;
							}
						}
					}
						[[fallthrough]];
					case DRAW_PRESSED: {
						if (is_pressed()) {
							if (has_theme_color(SNAME("selected_check_text_icon_pressed_color"))) {
								text_icon_font_color = theme_cache.selected_check_text_icon_pressed_color;
							}
						} else {
							if (has_theme_color(SNAME("unselected_check_text_icon_pressed_color"))) {
								text_icon_font_color = theme_cache.unselected_check_text_icon_pressed_color;
							}
						}
					} break;
					case DRAW_HOVER: {
						if (is_pressed()) {
							if (has_theme_color(SNAME("selected_check_text_icon_hover_color"))) {
								text_icon_font_color = theme_cache.selected_check_text_icon_hover_color;
							}
						} else {
							if (has_theme_color(SNAME("unselected_check_text_icon_hover_color"))) {
								text_icon_font_color = theme_cache.unselected_check_text_icon_hover_color;
							}
						}
					} break;
					case DRAW_DISABLED: {
						if (is_pressed()) {
							if (has_theme_color(SNAME("selected_check_text_icon_disabled_color"))) {
								text_icon_font_color = theme_cache.selected_check_text_icon_disabled_color;
							}
						} else {
							if (has_theme_color(SNAME("unselected_check_text_icon_disabled_color"))) {
								text_icon_font_color = theme_cache.unselected_check_text_icon_disabled_color;
							}
						}
					} break;
				}

				switch (get_draw_mode()) {
					case DRAW_NORMAL: {
						// Focus colors only take precedence over normal state.
						if (has_focus()) {
							if (is_pressed()) {
								if (has_theme_color(SNAME("selected_bg_check_text_icon_focus_color"))) {
									bg_text_icon_font_color = theme_cache.selected_bg_check_text_icon_focus_color;
								}
							} else {
								if (has_theme_color(SNAME("unselected_bg_check_text_icon_focus_color"))) {
									bg_text_icon_font_color = theme_cache.unselected_bg_check_text_icon_focus_color;
								}
							}
						} else {
							if (is_pressed()) {
								if (has_theme_color(SNAME("selected_bg_check_text_icon_normal_color"))) {
									bg_text_icon_font_color = theme_cache.selected_bg_check_text_icon_normal_color;
								}
							} else {
								if (has_theme_color(SNAME("unselected_bg_check_text_icon_normal_color"))) {
									bg_text_icon_font_color = theme_cache.unselected_bg_check_text_icon_normal_color;
								}
							}
						}
					} break;
					case DRAW_HOVER_PRESSED: {
						// Edge case for CheckButton and CheckBox.
						if (is_pressed()) {
							if (has_theme_stylebox("selected_bg_check_hover_pressed")) {
								if (has_theme_color(SNAME("selected_bg_check_text_icon_hover_pressed_color"))) {
									bg_text_icon_font_color = theme_cache.selected_bg_check_text_icon_hover_pressed_color;
								}
								break;
							}
						} else {
							if (has_theme_stylebox("unselected_bg_check_hover_pressed")) {
								if (has_theme_color(SNAME("unselected_bg_check_text_icon_hover_pressed_color"))) {
									bg_text_icon_font_color = theme_cache.unselected_bg_check_text_icon_hover_pressed_color;
								}
								break;
							}
						}
					}
						[[fallthrough]];
					case DRAW_PRESSED: {
						if (is_pressed()) {
							if (has_theme_color(SNAME("selected_bg_check_text_icon_pressed_color"))) {
								bg_text_icon_font_color = theme_cache.selected_bg_check_text_icon_pressed_color;
							}
						} else {
							if (has_theme_color(SNAME("unselected_bg_check_text_icon_pressed_color"))) {
								bg_text_icon_font_color = theme_cache.unselected_bg_check_text_icon_pressed_color;
							}
						}
					} break;
					case DRAW_HOVER: {
						if (is_pressed()) {
							if (has_theme_color(SNAME("selected_bg_check_text_icon_hover_color"))) {
								bg_text_icon_font_color = theme_cache.selected_bg_check_text_icon_hover_color;
							}
						} else {
							if (has_theme_color(SNAME("unselected_bg_check_text_icon_hover_color"))) {
								bg_text_icon_font_color = theme_cache.unselected_bg_check_text_icon_hover_color;
							}
						}
					} break;
					case DRAW_DISABLED: {
						if (is_pressed()) {
							if (has_theme_color(SNAME("selected_bg_check_text_icon_disabled_color"))) {
								bg_text_icon_font_color = theme_cache.selected_bg_check_text_icon_disabled_color;
							}
						} else {
							if (has_theme_color(SNAME("unselected_bg_check_text_icon_disabled_color"))) {
								bg_text_icon_font_color = theme_cache.unselected_bg_check_text_icon_disabled_color;
							}
						}
					} break;
				}

				Size2 cur_size = get_icon_size();
				if (!cur_text_icon.is_empty()) {

					bg_text_icon_buf->clear();
					bg_text_icon_buf->set_width(cur_size.width);
					if (cur_code_bg_text_icon.is_empty()) {
						cur_code_bg_text_icon = _get_trans_text(cur_bg_text_icon);
					}

					bg_text_icon_buf->add_string(cur_code_bg_text_icon, font, cur_size.width, "");
					bg_text_icon_buf->draw(ci, ofs, bg_text_icon_font_color);


					text_icon_buf->clear();
					text_icon_buf->set_width(cur_size.width);
					if (cur_code_text_icon.is_empty()) {
						cur_code_text_icon = _get_trans_text(cur_text_icon);
					}

					text_icon_buf->add_string(cur_code_text_icon, font, cur_size.width, "");
					text_icon_buf->draw(ci, ofs, text_icon_font_color);


				}
			} else {
				if (is_pressed()) {
					on_tex->draw(ci, ofs);
				} else {
					off_tex->draw(ci, ofs);
				}
			}
		} break;
	}
}

String CheckButton::_get_trans_text(const String &p_text_icon) {
	Ref<Font> text_icon_font = theme_cache.text_icon_font;

	String local_name = text_icon_font->get_path().get_file().get_basename();
	if (local_name.is_empty()) {
		local_name = text_icon_font->get_name();
	}

	Ref<Translation> trans = TranslationServer::get_singleton()->get_translation_object(local_name);
	String result_text = "";
	if (trans.is_valid()) {
		result_text = trans->get_message(p_text_icon);
		if (!result_text.is_empty()) {
			result_text = String::chr(("0x" + result_text.to_lower()).hex_to_int());
		}
	} else {
		result_text = "";
	}
	return result_text;
}

void CheckButton::set_selected_check_text_icon_normal(const String &p_selected_check_text_icon_normal) {
	if (selected_check_text_icon_normal != p_selected_check_text_icon_normal) {
		selected_check_text_icon_normal = p_selected_check_text_icon_normal;
		code_selected_check_text_icon_normal = _get_trans_text(selected_check_text_icon_normal);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_check_text_icon_normal() const {
	return selected_check_text_icon_normal;
}

void CheckButton::set_selected_check_text_icon_pressed(const String &p_selected_check_text_icon_pressed) {
	if (selected_check_text_icon_pressed != p_selected_check_text_icon_pressed) {
		selected_check_text_icon_pressed = p_selected_check_text_icon_pressed;
		code_selected_check_text_icon_pressed = _get_trans_text(selected_check_text_icon_pressed);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_check_text_icon_pressed() const {
	return selected_check_text_icon_pressed;
}

void CheckButton::set_selected_check_text_icon_hover(const String &p_selected_check_text_icon_hover) {
	if (selected_check_text_icon_hover != p_selected_check_text_icon_hover) {
		selected_check_text_icon_hover = p_selected_check_text_icon_hover;
		code_selected_check_text_icon_hover = _get_trans_text(selected_check_text_icon_hover);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_check_text_icon_hover() const {
	return selected_check_text_icon_hover;
}

void CheckButton::set_selected_check_text_icon_disabled(const String &p_selected_check_text_icon_disabled) {
	if (selected_check_text_icon_disabled != p_selected_check_text_icon_disabled) {
		selected_check_text_icon_disabled = p_selected_check_text_icon_disabled;
		code_selected_check_text_icon_disabled = _get_trans_text(selected_check_text_icon_disabled);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_check_text_icon_disabled() const {
	return selected_check_text_icon_disabled;
}

void CheckButton::set_unselected_check_text_icon_normal(const String &p_unselected_check_text_icon_normal) {
	if (unselected_check_text_icon_normal != p_unselected_check_text_icon_normal) {
		unselected_check_text_icon_normal = p_unselected_check_text_icon_normal;
		code_unselected_check_text_icon_normal = _get_trans_text(unselected_check_text_icon_normal);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_check_text_icon_normal() const {
	return unselected_check_text_icon_normal;
}

void CheckButton::set_unselected_check_text_icon_pressed(const String &p_unselected_check_text_icon_pressed) {
	if (unselected_check_text_icon_pressed != p_unselected_check_text_icon_pressed) {
		unselected_check_text_icon_pressed = p_unselected_check_text_icon_pressed;
		code_unselected_check_text_icon_pressed = _get_trans_text(unselected_check_text_icon_pressed);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_check_text_icon_pressed() const {
	return unselected_check_text_icon_pressed;
}

void CheckButton::set_unselected_check_text_icon_hover(const String &p_unselected_check_text_icon_hover) {
	if (unselected_check_text_icon_hover != p_unselected_check_text_icon_hover) {
		unselected_check_text_icon_hover = p_unselected_check_text_icon_hover;
		code_unselected_check_text_icon_hover = _get_trans_text(unselected_check_text_icon_hover);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_check_text_icon_hover() const {
	return unselected_check_text_icon_hover;
}

void CheckButton::set_unselected_check_text_icon_disabled(const String &p_unselected_check_text_icon_disabled) {
	if (unselected_check_text_icon_disabled != p_unselected_check_text_icon_disabled) {
		unselected_check_text_icon_disabled = p_unselected_check_text_icon_disabled;
		code_unselected_check_text_icon_disabled = _get_trans_text(unselected_check_text_icon_disabled);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_check_text_icon_disabled() const {
	return unselected_check_text_icon_disabled;
}

void CheckButton::set_selected_bg_check_text_icon_normal(const String &p_selected_bg_check_text_icon_normal) {
	if (selected_bg_check_text_icon_normal != p_selected_bg_check_text_icon_normal) {
		selected_bg_check_text_icon_normal = p_selected_bg_check_text_icon_normal;
		code_selected_bg_check_text_icon_normal = _get_trans_text(selected_bg_check_text_icon_normal);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_bg_check_text_icon_normal() const {
	return selected_bg_check_text_icon_normal;
}

void CheckButton::set_selected_bg_check_text_icon_pressed(const String &p_selected_bg_check_text_icon_pressed) {
	if (selected_bg_check_text_icon_pressed != p_selected_bg_check_text_icon_pressed) {
		selected_bg_check_text_icon_pressed = p_selected_bg_check_text_icon_pressed;
		code_selected_bg_check_text_icon_pressed = _get_trans_text(selected_bg_check_text_icon_pressed);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_bg_check_text_icon_pressed() const {
	return selected_bg_check_text_icon_pressed;
}

void CheckButton::set_selected_bg_check_text_icon_hover(const String &p_selected_bg_check_text_icon_hover) {
	if (selected_bg_check_text_icon_hover != p_selected_bg_check_text_icon_hover) {
		selected_bg_check_text_icon_hover = p_selected_bg_check_text_icon_hover;
		code_selected_bg_check_text_icon_hover = _get_trans_text(selected_bg_check_text_icon_hover);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_bg_check_text_icon_hover() const {
	return selected_bg_check_text_icon_hover;
}

void CheckButton::set_selected_bg_check_text_icon_disabled(const String &p_selected_bg_check_text_icon_disabled) {
	if (selected_bg_check_text_icon_disabled != p_selected_bg_check_text_icon_disabled) {
		selected_bg_check_text_icon_disabled = p_selected_bg_check_text_icon_disabled;
		code_selected_bg_check_text_icon_disabled = _get_trans_text(selected_bg_check_text_icon_disabled);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_bg_check_text_icon_disabled() const {
	return selected_bg_check_text_icon_disabled;
}

void CheckButton::set_unselected_bg_check_text_icon_normal(const String &p_unselected_bg_check_text_icon_normal) {
	if (unselected_bg_check_text_icon_normal != p_unselected_bg_check_text_icon_normal) {
		unselected_bg_check_text_icon_normal = p_unselected_bg_check_text_icon_normal;
		code_unselected_bg_check_text_icon_normal = _get_trans_text(unselected_bg_check_text_icon_normal);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_bg_check_text_icon_normal() const {
	return unselected_bg_check_text_icon_normal;
}

void CheckButton::set_unselected_bg_check_text_icon_pressed(const String &p_unselected_bg_check_text_icon_pressed) {
	if (unselected_bg_check_text_icon_pressed != p_unselected_bg_check_text_icon_pressed) {
		unselected_bg_check_text_icon_pressed = p_unselected_bg_check_text_icon_pressed;
		code_unselected_bg_check_text_icon_pressed = _get_trans_text(unselected_bg_check_text_icon_pressed);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_bg_check_text_icon_pressed() const {
	return unselected_bg_check_text_icon_pressed;
}

void CheckButton::set_unselected_bg_check_text_icon_hover(const String &p_unselected_bg_check_text_icon_hover) {
	if (unselected_bg_check_text_icon_hover != p_unselected_bg_check_text_icon_hover) {
		unselected_bg_check_text_icon_hover = p_unselected_bg_check_text_icon_hover;
		code_unselected_bg_check_text_icon_hover = _get_trans_text(unselected_bg_check_text_icon_hover);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_bg_check_text_icon_hover() const {
	return unselected_bg_check_text_icon_hover;
}

void CheckButton::set_unselected_bg_check_text_icon_disabled(const String &p_unselected_bg_check_text_icon_disabled) {
	if (unselected_bg_check_text_icon_disabled != p_unselected_bg_check_text_icon_disabled) {
		unselected_bg_check_text_icon_disabled = p_unselected_bg_check_text_icon_disabled;
		code_unselected_bg_check_text_icon_disabled = _get_trans_text(unselected_bg_check_text_icon_disabled);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_bg_check_text_icon_disabled() const {
	return unselected_bg_check_text_icon_disabled;
}

void CheckButton::set_selected_check_text_icon_normal_mirrored(const String &p_selected_check_text_icon_normal_mirrored) {
	if (selected_check_text_icon_normal_mirrored != p_selected_check_text_icon_normal_mirrored) {
		selected_check_text_icon_normal_mirrored = p_selected_check_text_icon_normal_mirrored;
		code_selected_check_text_icon_normal_mirrored = _get_trans_text(selected_check_text_icon_normal_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_check_text_icon_normal_mirrored() const {
	return selected_check_text_icon_normal_mirrored;
}

void CheckButton::set_selected_check_text_icon_pressed_mirrored(const String &p_selected_check_text_icon_pressed_mirrored) {
	if (selected_check_text_icon_pressed_mirrored != p_selected_check_text_icon_pressed_mirrored) {
		selected_check_text_icon_pressed_mirrored = p_selected_check_text_icon_pressed_mirrored;
		code_selected_check_text_icon_pressed_mirrored = _get_trans_text(selected_check_text_icon_pressed_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_check_text_icon_pressed_mirrored() const {
	return selected_check_text_icon_pressed_mirrored;
}

void CheckButton::set_selected_check_text_icon_hover_mirrored(const String &p_selected_check_text_icon_hover_mirrored) {
	if (selected_check_text_icon_hover_mirrored != p_selected_check_text_icon_hover_mirrored) {
		selected_check_text_icon_hover_mirrored = p_selected_check_text_icon_hover_mirrored;
		code_selected_check_text_icon_hover_mirrored = _get_trans_text(selected_check_text_icon_hover_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_check_text_icon_hover_mirrored() const {
	return selected_check_text_icon_hover_mirrored;
}

void CheckButton::set_selected_check_text_icon_disabled_mirrored(const String &p_selected_check_text_icon_disabled_mirrored) {
	if (selected_check_text_icon_disabled_mirrored != p_selected_check_text_icon_disabled_mirrored) {
		selected_check_text_icon_disabled_mirrored = p_selected_check_text_icon_disabled_mirrored;
		code_selected_check_text_icon_disabled_mirrored = _get_trans_text(selected_check_text_icon_disabled_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_check_text_icon_disabled_mirrored() const {
	return selected_check_text_icon_disabled_mirrored;
}

void CheckButton::set_unselected_check_text_icon_normal_mirrored(const String &p_unselected_check_text_icon_normal_mirrored) {
	if (unselected_check_text_icon_normal_mirrored != p_unselected_check_text_icon_normal_mirrored) {
		unselected_check_text_icon_normal_mirrored = p_unselected_check_text_icon_normal_mirrored;
		code_unselected_check_text_icon_normal_mirrored = _get_trans_text(unselected_check_text_icon_normal_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_check_text_icon_normal_mirrored() const {
	return unselected_check_text_icon_normal_mirrored;
}

void CheckButton::set_unselected_check_text_icon_pressed_mirrored(const String &p_unselected_check_text_icon_pressed_mirrored) {
	if (unselected_check_text_icon_pressed_mirrored != p_unselected_check_text_icon_pressed_mirrored) {
		unselected_check_text_icon_pressed_mirrored = p_unselected_check_text_icon_pressed_mirrored;
		code_unselected_check_text_icon_pressed = _get_trans_text(unselected_check_text_icon_pressed_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_check_text_icon_pressed_mirrored() const {
	return unselected_check_text_icon_pressed_mirrored;
}

void CheckButton::set_unselected_check_text_icon_hover_mirrored(const String &p_unselected_check_text_icon_hover_mirrored) {
	if (unselected_check_text_icon_hover_mirrored != p_unselected_check_text_icon_hover_mirrored) {
		unselected_check_text_icon_hover_mirrored = p_unselected_check_text_icon_hover_mirrored;
		code_unselected_check_text_icon_hover_mirrored = _get_trans_text(unselected_check_text_icon_hover_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_check_text_icon_hover_mirrored() const {
	return unselected_check_text_icon_hover_mirrored;
}

void CheckButton::set_unselected_check_text_icon_disabled_mirrored(const String &p_unselected_check_text_icon_disabled_mirrored) {
	if (unselected_check_text_icon_disabled_mirrored != p_unselected_check_text_icon_disabled_mirrored) {
		unselected_check_text_icon_disabled_mirrored = p_unselected_check_text_icon_disabled_mirrored;
		code_unselected_check_text_icon_disabled_mirrored = _get_trans_text(unselected_check_text_icon_disabled_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_check_text_icon_disabled_mirrored() const {
	return unselected_check_text_icon_disabled_mirrored;
}

void CheckButton::set_selected_bg_check_text_icon_normal_mirrored(const String &p_selected_bg_check_text_icon_normal_mirrored) {
	if (selected_bg_check_text_icon_normal_mirrored != p_selected_bg_check_text_icon_normal_mirrored) {
		selected_bg_check_text_icon_normal_mirrored = p_selected_bg_check_text_icon_normal_mirrored;
		code_selected_bg_check_text_icon_normal_mirrored = _get_trans_text(selected_bg_check_text_icon_normal_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_bg_check_text_icon_normal_mirrored() const {
	return selected_bg_check_text_icon_normal_mirrored;
}

void CheckButton::set_selected_bg_check_text_icon_pressed_mirrored(const String &p_selected_bg_check_text_icon_pressed_mirrored) {
	if (selected_bg_check_text_icon_pressed_mirrored != p_selected_bg_check_text_icon_pressed_mirrored) {
		selected_bg_check_text_icon_pressed_mirrored = p_selected_bg_check_text_icon_pressed_mirrored;
		code_selected_bg_check_text_icon_pressed_mirrored = _get_trans_text(selected_bg_check_text_icon_pressed_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_bg_check_text_icon_pressed_mirrored() const {
	return selected_bg_check_text_icon_pressed_mirrored;
}

void CheckButton::set_selected_bg_check_text_icon_hover_mirrored(const String &p_selected_bg_check_text_icon_hover_mirrored) {
	if (selected_bg_check_text_icon_hover_mirrored != p_selected_bg_check_text_icon_hover_mirrored) {
		selected_bg_check_text_icon_hover_mirrored = p_selected_bg_check_text_icon_hover_mirrored;
		code_selected_bg_check_text_icon_hover_mirrored = _get_trans_text(selected_bg_check_text_icon_hover_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_bg_check_text_icon_hover_mirrored() const {
	return selected_bg_check_text_icon_hover_mirrored;
}

void CheckButton::set_selected_bg_check_text_icon_disabled_mirrored(const String &p_selected_bg_check_text_icon_disabled_mirrored) {
	if (selected_bg_check_text_icon_disabled_mirrored != p_selected_bg_check_text_icon_disabled_mirrored) {
		selected_bg_check_text_icon_disabled_mirrored = p_selected_bg_check_text_icon_disabled_mirrored;
		code_selected_bg_check_text_icon_disabled_mirrored = _get_trans_text(selected_bg_check_text_icon_disabled_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_selected_bg_check_text_icon_disabled_mirrored() const {
	return selected_bg_check_text_icon_disabled_mirrored;
}

void CheckButton::set_unselected_bg_check_text_icon_normal_mirrored(const String &p_unselected_bg_check_text_icon_normal_mirrored) {
	if (unselected_bg_check_text_icon_normal_mirrored != p_unselected_bg_check_text_icon_normal_mirrored) {
		unselected_bg_check_text_icon_normal_mirrored = p_unselected_bg_check_text_icon_normal_mirrored;
		code_unselected_bg_check_text_icon_normal_mirrored = _get_trans_text(unselected_bg_check_text_icon_normal_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_bg_check_text_icon_normal_mirrored() const {
	return unselected_bg_check_text_icon_normal_mirrored;
}

void CheckButton::set_unselected_bg_check_text_icon_pressed_mirrored(const String &p_unselected_bg_check_text_icon_pressed_mirrored) {
	if (unselected_bg_check_text_icon_pressed_mirrored != p_unselected_bg_check_text_icon_pressed_mirrored) {
		unselected_bg_check_text_icon_pressed_mirrored = p_unselected_bg_check_text_icon_pressed_mirrored;
		code_unselected_bg_check_text_icon_pressed = _get_trans_text(unselected_bg_check_text_icon_pressed_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_bg_check_text_icon_pressed_mirrored() const {
	return unselected_bg_check_text_icon_pressed_mirrored;
}

void CheckButton::set_unselected_bg_check_text_icon_hover_mirrored(const String &p_unselected_bg_check_text_icon_hover_mirrored) {
	if (unselected_bg_check_text_icon_hover_mirrored != p_unselected_bg_check_text_icon_hover_mirrored) {
		unselected_bg_check_text_icon_hover_mirrored = p_unselected_bg_check_text_icon_hover_mirrored;
		code_unselected_bg_check_text_icon_hover_mirrored = _get_trans_text(unselected_bg_check_text_icon_hover_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_bg_check_text_icon_hover_mirrored() const {
	return unselected_bg_check_text_icon_hover_mirrored;
}

void CheckButton::set_unselected_bg_check_text_icon_disabled_mirrored(const String &p_unselected_bg_check_text_icon_disabled_mirrored) {
	if (unselected_bg_check_text_icon_disabled_mirrored != p_unselected_bg_check_text_icon_disabled_mirrored) {
		unselected_bg_check_text_icon_disabled_mirrored = p_unselected_bg_check_text_icon_disabled_mirrored;
		code_unselected_bg_check_text_icon_disabled_mirrored = _get_trans_text(unselected_bg_check_text_icon_disabled_mirrored);
		queue_redraw();
		update_minimum_size();
	}
}

String CheckButton::get_unselected_bg_check_text_icon_disabled_mirrored() const {
	return unselected_bg_check_text_icon_disabled_mirrored;
}

void CheckButton::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_selected_check_text_icon_normal", "text_icon"), &CheckButton::set_selected_check_text_icon_normal);
	ClassDB::bind_method(D_METHOD("set_selected_check_text_icon_pressed", "text_icon"), &CheckButton::set_selected_check_text_icon_pressed);
	ClassDB::bind_method(D_METHOD("set_selected_check_text_icon_hover", "text_icon"), &CheckButton::set_selected_check_text_icon_hover);
	ClassDB::bind_method(D_METHOD("set_selected_check_text_icon_disabled", "text_icon"), &CheckButton::set_selected_check_text_icon_disabled);

	ClassDB::bind_method(D_METHOD("get_selected_check_text_icon_normal"), &CheckButton::get_selected_check_text_icon_normal);
	ClassDB::bind_method(D_METHOD("get_selected_check_text_icon_pressed"), &CheckButton::get_selected_check_text_icon_pressed);
	ClassDB::bind_method(D_METHOD("get_selected_check_text_icon_hover"), &CheckButton::get_selected_check_text_icon_hover);
	ClassDB::bind_method(D_METHOD("get_selected_check_text_icon_disabled"), &CheckButton::get_selected_check_text_icon_disabled);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "selected_check_text_icon_normal"), "set_selected_check_text_icon_normal", "get_selected_check_text_icon_normal");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "selected_check_text_icon_pressed"), "set_selected_check_text_icon_pressed", "get_selected_check_text_icon_pressed");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "selected_check_text_icon_hover"), "set_selected_check_text_icon_hover", "get_selected_check_text_icon_hover");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "selected_check_text_icon_disabled"), "set_selected_check_text_icon_disabled", "get_selected_check_text_icon_disabled");

	ClassDB::bind_method(D_METHOD("set_unselected_check_text_icon_normal", "text_icon"), &CheckButton::set_unselected_check_text_icon_normal);
	ClassDB::bind_method(D_METHOD("set_unselected_check_text_icon_pressed", "text_icon"), &CheckButton::set_unselected_check_text_icon_pressed);
	ClassDB::bind_method(D_METHOD("set_unselected_check_text_icon_hover", "text_icon"), &CheckButton::set_unselected_check_text_icon_hover);
	ClassDB::bind_method(D_METHOD("set_unselected_check_text_icon_disabled", "text_icon"), &CheckButton::set_unselected_check_text_icon_disabled);

	ClassDB::bind_method(D_METHOD("get_unselected_check_text_icon_normal"), &CheckButton::get_unselected_check_text_icon_normal);
	ClassDB::bind_method(D_METHOD("get_unselected_check_text_icon_pressed"), &CheckButton::get_unselected_check_text_icon_pressed);
	ClassDB::bind_method(D_METHOD("get_unselected_check_text_icon_hover"), &CheckButton::get_unselected_check_text_icon_hover);
	ClassDB::bind_method(D_METHOD("get_unselected_check_text_icon_disabled"), &CheckButton::get_unselected_check_text_icon_disabled);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "unselected_check_text_icon_normal"), "set_unselected_check_text_icon_normal", "get_unselected_check_text_icon_normal");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "unselected_check_text_icon_pressed"), "set_unselected_check_text_icon_pressed", "get_unselected_check_text_icon_pressed");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "unselected_check_text_icon_hover"), "set_unselected_check_text_icon_hover", "get_unselected_check_text_icon_hover");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "unselected_check_text_icon_disabled"), "set_unselected_check_text_icon_disabled", "get_unselected_check_text_icon_disabled");

	ClassDB::bind_method(D_METHOD("set_selected_check_text_icon_normal_mirrored", "text_icon"), &CheckButton::set_selected_check_text_icon_normal_mirrored);
	ClassDB::bind_method(D_METHOD("set_selected_check_text_icon_pressed_mirrored", "text_icon"), &CheckButton::set_selected_check_text_icon_pressed_mirrored);
	ClassDB::bind_method(D_METHOD("set_selected_check_text_icon_hover_mirrored", "text_icon"), &CheckButton::set_selected_check_text_icon_hover_mirrored);
	ClassDB::bind_method(D_METHOD("set_selected_check_text_icon_disabled_mirrored", "text_icon"), &CheckButton::set_selected_check_text_icon_disabled_mirrored);

	ClassDB::bind_method(D_METHOD("get_selected_check_text_icon_normal_mirrored"), &CheckButton::get_selected_check_text_icon_normal_mirrored);
	ClassDB::bind_method(D_METHOD("get_selected_check_text_icon_pressed_mirrored"), &CheckButton::get_selected_check_text_icon_pressed_mirrored);
	ClassDB::bind_method(D_METHOD("get_selected_check_text_icon_hover_mirrored"), &CheckButton::get_selected_check_text_icon_hover_mirrored);
	ClassDB::bind_method(D_METHOD("get_selected_check_text_icon_disabled_mirrored"), &CheckButton::get_selected_check_text_icon_disabled_mirrored);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "selected_check_text_icon_normal_mirrored"), "set_selected_check_text_icon_normal_mirrored", "get_selected_check_text_icon_normal_mirrored");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "selected_check_text_icon_pressed_mirrored"), "set_selected_check_text_icon_pressed_mirrored", "get_selected_check_text_icon_pressed_mirrored");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "selected_check_text_icon_hover_mirrored"), "set_selected_check_text_icon_hover_mirrored", "get_selected_check_text_icon_hover_mirrored");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "selected_check_text_icon_disabled_mirrored"), "set_selected_check_text_icon_disabled_mirrored", "get_selected_check_text_icon_disabled_mirrored");

	ClassDB::bind_method(D_METHOD("set_unselected_check_text_icon_normal_mirrored", "text_icon"), &CheckButton::set_unselected_check_text_icon_normal_mirrored);
	ClassDB::bind_method(D_METHOD("set_unselected_check_text_icon_pressed_mirrored", "text_icon"), &CheckButton::set_unselected_check_text_icon_pressed_mirrored);
	ClassDB::bind_method(D_METHOD("set_unselected_check_text_icon_hover_mirrored", "text_icon"), &CheckButton::set_unselected_check_text_icon_hover_mirrored);
	ClassDB::bind_method(D_METHOD("set_unselected_check_text_icon_disabled_mirrored", "text_icon"), &CheckButton::set_unselected_check_text_icon_disabled_mirrored);

	ClassDB::bind_method(D_METHOD("get_unselected_check_text_icon_normal_mirrored"), &CheckButton::get_unselected_check_text_icon_normal_mirrored);
	ClassDB::bind_method(D_METHOD("get_unselected_check_text_icon_pressed_mirrored"), &CheckButton::get_unselected_check_text_icon_pressed_mirrored);
	ClassDB::bind_method(D_METHOD("get_unselected_check_text_icon_hover_mirrored"), &CheckButton::get_unselected_check_text_icon_hover_mirrored);
	ClassDB::bind_method(D_METHOD("get_unselected_check_text_icon_disabled_mirrored"), &CheckButton::get_unselected_check_text_icon_disabled_mirrored);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "unselected_check_text_icon_normal_mirrored"), "set_unselected_check_text_icon_normal_mirrored", "get_unselected_check_text_icon_normal_mirrored");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "unselected_check_text_icon_pressed_mirrored"), "set_unselected_check_text_icon_pressed_mirrored", "get_unselected_check_text_icon_pressed_mirrored");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "unselected_check_text_icon_hover_mirrored"), "set_unselected_check_text_icon_hover_mirrored", "get_unselected_check_text_icon_hover_mirrored");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "unselected_check_text_icon_disabled_mirrored"), "set_unselected_check_text_icon_disabled_mirrored", "get_unselected_check_text_icon_disabled_mirrored");

	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, CheckButton, text_icon_font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, CheckButton, text_icon_font_size);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, CheckButton, icon_max_width);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, CheckButton, h_separation);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, CheckButton, check_v_offset);
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, CheckButton, normal_style, "normal");

	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, checked);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, unchecked);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, checked_disabled);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, unchecked_disabled);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, checked_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, unchecked_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, checked_disabled_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_ICON, CheckButton, unchecked_disabled_mirrored);

	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, selected_check_hover_state_layer);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, selected_check_hover_state_layer_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, selected_check_pressed_state_layer);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, selected_check_pressed_state_layer_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, selected_check_hover_pressed_state_layer);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, selected_check_hover_pressed_state_layer_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, selected_check_focus_state_layer);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_normal_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_check_text_icon_normal_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_check_text_icon_normal_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_normal_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_pressed_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_check_text_icon_pressed_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_check_text_icon_pressed_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_pressed_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_hover_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_check_text_icon_hover_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_check_text_icon_hover_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_hover_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_focus_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_check_text_icon_focus_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_check_text_icon_focus_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_focus_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_hover_pressed_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_check_text_icon_hover_pressed_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_check_text_icon_hover_pressed_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_hover_pressed_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_disabled_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_check_text_icon_disabled_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_check_text_icon_disabled_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_check_text_icon_disabled_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, unselected_check_hover_state_layer);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, unselected_check_hover_state_layer_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, unselected_check_pressed_state_layer);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, unselected_check_pressed_state_layer_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, unselected_check_hover_pressed_state_layer);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, unselected_check_hover_pressed_state_layer_mirrored);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, CheckButton, unselected_check_focus_state_layer);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_normal_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_check_text_icon_normal_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_check_text_icon_normal_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_normal_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_pressed_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_check_text_icon_pressed_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_check_text_icon_pressed_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_pressed_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_hover_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_check_text_icon_hover_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_check_text_icon_hover_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_hover_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_focus_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_check_text_icon_focus_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_check_text_icon_focus_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_focus_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_hover_pressed_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_check_text_icon_hover_pressed_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_check_text_icon_hover_pressed_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_hover_pressed_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_disabled_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_check_text_icon_disabled_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_check_text_icon_disabled_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_check_text_icon_disabled_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_normal_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_bg_check_text_icon_normal_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_bg_check_text_icon_normal_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_normal_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_pressed_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_bg_check_text_icon_pressed_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_bg_check_text_icon_pressed_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_pressed_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_hover_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_bg_check_text_icon_hover_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_bg_check_text_icon_hover_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_hover_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_focus_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_bg_check_text_icon_focus_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_bg_check_text_icon_focus_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_focus_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_hover_pressed_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_bg_check_text_icon_hover_pressed_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_bg_check_text_icon_hover_pressed_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_hover_pressed_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_disabled_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, selected_bg_check_text_icon_disabled_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, selected_bg_check_text_icon_disabled_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, selected_bg_check_text_icon_disabled_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_normal_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_bg_check_text_icon_normal_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_bg_check_text_icon_normal_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_normal_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_pressed_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_bg_check_text_icon_pressed_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_bg_check_text_icon_pressed_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_pressed_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_hover_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_bg_check_text_icon_hover_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_bg_check_text_icon_hover_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_hover_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_focus_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_bg_check_text_icon_focus_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_bg_check_text_icon_focus_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_focus_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_hover_pressed_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_bg_check_text_icon_hover_pressed_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_bg_check_text_icon_hover_pressed_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_hover_pressed_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_disabled_color_scale);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_SCHEME, CheckButton, unselected_bg_check_text_icon_disabled_color_scheme);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR_ROLE, CheckButton, unselected_bg_check_text_icon_disabled_color_role);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, CheckButton, unselected_bg_check_text_icon_disabled_color);
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

	text_icon_buf.instantiate();
	text_icon_buf->set_break_flags(TextServer::BREAK_MANDATORY | TextServer::BREAK_TRIM_EDGE_SPACES);

	bg_text_icon_buf.instantiate();
	bg_text_icon_buf->set_break_flags(TextServer::BREAK_MANDATORY | TextServer::BREAK_TRIM_EDGE_SPACES);
}

CheckButton::~CheckButton() {
}

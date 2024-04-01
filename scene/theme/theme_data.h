/**************************************************************************/
/*  theme_db.h                                                            */
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

#ifndef THEME_DATA_H
#define THEME_DATA_H

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "scene/resources/theme.h"

// #include <functional>

// class Font;
// class Node;
// class StyleBox;
// class Texture2D;
// class ColorScheme;

enum State {
	NormalNoneLTR,
	PressedNoneLTR,
	HoverNoneLTR,
	FocusNoneLTR,
	HoverPressedNoneLTR,
	DisabledNoneLTR,

	NormalCheckedLTR,
	PressedCheckedLTR,
	HoverCheckedLTR,
	FocusCheckedLTR,
	HoverPressedCheckedLTR,
	DisabledCheckedLTR,

	NormalUncheckedLTR,
	PressedUncheckedLTR,
	HoverUncheckedLTR,
	FocusUncheckedLTR,
	HoverPressedUncheckedLTR,
	DisabledUncheckedLTR,

	NormalNoneRTL,
	PressedNoneRTL,
	HoverNoneRTL,
	FocusNoneRTL,
	HoverPressedNoneRTL,
	DisabledNoneRTL,

	NormalCheckedRTL,
	PressedCheckedRTL,
	HoverCheckedRTL,
	FocusCheckedRTL,
	HoverPressedCheckedRTL,
	DisabledCheckedRTL,

	NormalUncheckedRTL,
	PressedUncheckedRTL,
	HoverUncheckedRTL,
	FocusUncheckedRTL,
	HoverPressedUncheckedRTL,
	DisabledUncheckedRTL,

	STATE_MAX
};

static const Vector<State> search_order[STATE_MAX] = {
	{
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::PressedNoneLTR,
			State::PressedNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::HoverNoneLTR,
			State::HoverNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::FocusNoneLTR,
			State::FocusNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::HoverPressedNoneLTR,
			State::HoverPressedNoneRTL,
			State::PressedNoneLTR,
			State::PressedNoneRTL,
			State::HoverNoneLTR,
			State::HoverNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::DisabledNoneLTR,
			State::DisabledNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::NormalCheckedLTR,
			State::NormalCheckedRTL,

			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::PressedCheckedLTR,
			State::PressedCheckedRTL,
			State::NormalCheckedLTR,
			State::NormalCheckedRTL,

			State::PressedNoneLTR,
			State::PressedNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::HoverCheckedLTR,
			State::HoverCheckedRTL,
			State::NormalCheckedLTR,
			State::NormalCheckedRTL,

			State::HoverNoneLTR,
			State::HoverNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::FocusCheckedLTR,
			State::FocusCheckedRTL,
			State::NormalCheckedLTR,
			State::NormalCheckedRTL,

			State::FocusNoneLTR,
			State::FocusNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::HoverPressedCheckedLTR,
			State::HoverPressedCheckedRTL,
			State::PressedCheckedLTR,
			State::PressedCheckedRTL,
			State::HoverCheckedLTR,
			State::HoverCheckedRTL,
			State::NormalCheckedLTR,
			State::NormalCheckedRTL,

			State::HoverPressedNoneLTR,
			State::HoverPressedNoneRTL,
			State::PressedNoneLTR,
			State::PressedNoneRTL,
			State::HoverNoneLTR,
			State::HoverNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::DisabledCheckedLTR,
			State::DisabledCheckedRTL,
			State::NormalCheckedLTR,
			State::NormalCheckedRTL,

			State::DisabledNoneLTR,
			State::DisabledNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},

	{
			State::NormalUncheckedLTR,
			State::NormalUncheckedRTL,

			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::PressedUncheckedLTR,
			State::PressedUncheckedRTL,
			State::NormalUncheckedLTR,
			State::NormalUncheckedRTL,

			State::PressedNoneLTR,
			State::PressedNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::HoverUncheckedLTR,
			State::HoverUncheckedRTL,
			State::NormalUncheckedLTR,
			State::NormalUncheckedRTL,

			State::HoverNoneLTR,
			State::HoverNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::FocusUncheckedLTR,
			State::FocusUncheckedRTL,
			State::NormalUncheckedLTR,
			State::NormalUncheckedRTL,

			State::FocusNoneLTR,
			State::FocusNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::HoverPressedUncheckedLTR,
			State::HoverPressedUncheckedRTL,
			State::PressedUncheckedLTR,
			State::PressedUncheckedRTL,
			State::HoverUncheckedLTR,
			State::HoverUncheckedRTL,
			State::NormalUncheckedLTR,
			State::NormalUncheckedRTL,

			State::HoverPressedNoneLTR,
			State::HoverPressedNoneRTL,
			State::PressedNoneLTR,
			State::PressedNoneRTL,
			State::HoverNoneLTR,
			State::HoverNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},
	{
			State::DisabledUncheckedLTR,
			State::DisabledUncheckedRTL,
			State::NormalUncheckedLTR,
			State::NormalUncheckedRTL,

			State::DisabledNoneLTR,
			State::DisabledNoneRTL,
			State::NormalNoneLTR,
			State::NormalNoneRTL,
	},

	{
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::PressedNoneRTL,
			State::PressedNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::HoverNoneRTL,
			State::HoverNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::FocusNoneRTL,
			State::FocusNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,

	},
	{
			State::HoverPressedNoneRTL,
			State::HoverPressedNoneLTR,
			State::PressedNoneRTL,
			State::PressedNoneLTR,
			State::HoverNoneRTL,
			State::HoverNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::DisabledNoneRTL,
			State::DisabledNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},

	{
			State::NormalCheckedRTL,
			State::NormalCheckedLTR,

			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::PressedCheckedRTL,
			State::PressedCheckedLTR,
			State::NormalCheckedRTL,
			State::NormalCheckedLTR,

			State::PressedNoneRTL,
			State::PressedNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::HoverCheckedRTL,
			State::HoverCheckedLTR,
			State::NormalCheckedRTL,
			State::NormalCheckedLTR,

			State::HoverNoneRTL,
			State::HoverNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::FocusCheckedRTL,
			State::FocusCheckedLTR,
			State::NormalCheckedRTL,
			State::NormalCheckedLTR,

			State::FocusNoneRTL,
			State::FocusNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::HoverPressedCheckedRTL,
			State::HoverPressedCheckedLTR,
			State::PressedCheckedRTL,
			State::PressedCheckedLTR,
			State::HoverCheckedRTL,
			State::HoverCheckedLTR,
			State::NormalCheckedRTL,
			State::NormalCheckedLTR,

			State::HoverPressedNoneRTL,
			State::HoverPressedNoneLTR,
			State::PressedNoneRTL,
			State::PressedNoneLTR,
			State::HoverNoneRTL,
			State::HoverNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::DisabledCheckedRTL,
			State::DisabledCheckedLTR,
			State::NormalCheckedRTL,
			State::NormalCheckedLTR,

			State::DisabledNoneRTL,
			State::DisabledNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},

	{
			State::NormalUncheckedRTL,
			State::NormalUncheckedLTR,

			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::PressedUncheckedRTL,
			State::PressedUncheckedLTR,
			State::NormalUncheckedRTL,
			State::NormalUncheckedLTR,

			State::PressedNoneRTL,
			State::PressedNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::HoverUncheckedRTL,
			State::HoverUncheckedLTR,
			State::NormalUncheckedRTL,
			State::NormalUncheckedLTR,

			State::HoverNoneRTL,
			State::HoverNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::FocusUncheckedRTL,
			State::FocusUncheckedLTR,
			State::NormalUncheckedRTL,
			State::NormalUncheckedLTR,

			State::FocusNoneRTL,
			State::FocusNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::HoverPressedUncheckedRTL,
			State::HoverPressedUncheckedLTR,
			State::PressedUncheckedRTL,
			State::PressedUncheckedLTR,
			State::HoverUncheckedRTL,
			State::HoverUncheckedLTR,
			State::NormalUncheckedRTL,
			State::NormalUncheckedLTR,

			State::HoverPressedNoneRTL,
			State::HoverPressedNoneLTR,
			State::PressedNoneRTL,
			State::PressedNoneLTR,
			State::HoverNoneRTL,
			State::HoverNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	},
	{
			State::DisabledUncheckedRTL,
			State::DisabledUncheckedLTR,
			State::NormalUncheckedRTL,
			State::NormalUncheckedLTR,

			State::DisabledNoneRTL,
			State::DisabledNoneLTR,
			State::NormalNoneRTL,
			State::NormalNoneLTR,
	}
};

static const char *state_prefix_name[STATE_MAX] = {
	"normal_",
	"pressed_",
	"hover_",
	"focus_",
	"hover_pressed_",
	"disabled_",

	"normal_checked_",
	"pressed_checked_",
	"hover_checked_",
	"focus_checked_",
	"hover_pressed_checked_",
	"disabled_checked_",

	"normal_unchecked_",
	"pressed_unchecked_",
	"hover_unchecked_",
	"focus_unchecked_",
	"hover_pressed_unchecked_",
	"disabled_unchecked_",

	"normal_mirrored_",
	"pressed_mirrored_",
	"hover_mirrored_",
	"focus_mirrored_",
	"hover_pressed_mirrored_",
	"disabled_mirrored_",

	"normal_checked_mirrored_",
	"pressed_checked_mirrored_",
	"hover_checked_mirrored_",
	"focus_checked_mirrored_",
	"hover_pressed_checked_mirrored_",
	"disabled_checked_mirrored_",

	"normal_unchecked_mirrored_",
	"pressed_unchecked_mirrored_",
	"hover_unchecked_mirrored_",
	"focus_unchecked_mirrored_",
	"hover_pressed_unchecked_mirrored_",
	"disabled_unchecked_mirrored_"
};

template <typename T>
class ThemeData : public Object {
	GDCLASS(ThemeData, Object);

	struct DataCache {
		T normal;
		T pressed;
		T hover;
		T focus;
		T hover_pressed;
		T disabled;

		T normal_checked;
		T pressed_checked;
		T hover_checked;
		T focus_checked;
		T hover_pressed_checked;
		T disabled_checked;

		T normal_unchecked;
		T pressed_unchecked;
		T hover_unchecked;
		T focus_unchecked;
		T hover_pressed_unchecked;
		T disabled_unchecked;

		T normal_rtl;
		T pressed_rtl;
		T hover_rtl;
		T focus_rtl;
		T hover_pressed_rtl;
		T disabled_rtl;

		T normal_checked_rtl;
		T pressed_checked_rtl;
		T hover_checked_rtl;
		T focus_checked_rtl;
		T hover_pressed_checked_rtl;
		T disabled_checked_rtl;

		T normal_unchecked_rtl;
		T pressed_unchecked_rtl;
		T hover_unchecked_rtl;
		T focus_unchecked_rtl;
		T hover_pressed_unchecked_rtl;
		T disabled_unchecked_rtl;

	} data_cache;

	HashMap<State, bool> has_data_map;
	String data_name;

public:
	void set_data_name(const String &p_data_name);

	String get_state_data_name(State p_state = State::NormalNoneLTR) const;

	T get_data(State p_state = State::NormalNoneLTR) const;
	void set_data(T p_data, State p_state = State::NormalNoneLTR);

	Vector<State> get_search_order(State p_state) const;

	ThemeData<T>(const String &p_data_name):data_name(p_data_name){};
	ThemeData<T>(){};
	~ThemeData<T>(){};
};

template <typename T>
void ThemeData<T>::set_data_name(const String &p_data_name) {
	data_name = p_data_name;
}

template <typename T>
String ThemeData<T>::get_state_data_name(State p_state) const {
	return state_prefix_name[p_state] + data_name;
}

template <typename T>
T ThemeData<T>::get_data(State p_state) const {
	switch (p_state) {
		case State::NormalNoneLTR: {
			return data_cache.normal;
		} break;
		case State::PressedNoneLTR: {
			return data_cache.pressed;
		} break;
		case State::HoverNoneLTR: {
			return data_cache.hover;
		} break;
		case State::FocusNoneLTR: {
			return data_cache.focus;
		} break;
		case State::HoverPressedNoneLTR: {
			return data_cache.hover_pressed;
		} break;
		case State::DisabledNoneLTR: {
			return data_cache.disabled;
		} break;
		case State::NormalCheckedLTR: {
			return data_cache.normal_checked;
		} break;
		case State::PressedCheckedLTR: {
			return data_cache.pressed_checked;
		} break;
		case State::HoverCheckedLTR: {
			return data_cache.hover_checked;
		} break;
		case State::FocusCheckedLTR: {
			return data_cache.focus_checked;
		} break;
		case State::HoverPressedCheckedLTR: {
			return data_cache.hover_pressed_checked;
		} break;
		case State::DisabledCheckedLTR: {
			return data_cache.disabled_checked;
		} break;
		case State::NormalUncheckedLTR: {
			return data_cache.normal_unchecked;
		} break;
		case State::PressedUncheckedLTR: {
			return data_cache.pressed_unchecked;
		} break;
		case State::HoverUncheckedLTR: {
			return data_cache.hover_unchecked;
		} break;
		case State::FocusUncheckedLTR: {
			return data_cache.focus_unchecked;
		} break;
		case State::HoverPressedUncheckedLTR: {
			return data_cache.hover_pressed_unchecked;
		} break;
		case State::DisabledUncheckedLTR: {
			return data_cache.disabled_unchecked;
		} break;
		case State::NormalNoneRTL: {
			return data_cache.normal_rtl;
		} break;
		case State::PressedNoneRTL: {
			return data_cache.pressed_rtl;
		} break;
		case State::HoverNoneRTL: {
			return data_cache.hover_rtl;
		} break;
		case State::FocusNoneRTL: {
			return data_cache.focus_rtl;
		} break;
		case State::HoverPressedNoneRTL: {
			return data_cache.hover_pressed_rtl;
		} break;
		case State::DisabledNoneRTL: {
			return data_cache.disabled_rtl;
		} break;
		case State::NormalCheckedRTL: {
			return data_cache.normal_checked_rtl;
		} break;
		case State::PressedCheckedRTL: {
			return data_cache.pressed_checked_rtl;
		} break;
		case State::HoverCheckedRTL: {
			return data_cache.hover_checked_rtl;
		} break;
		case State::FocusCheckedRTL: {
			return data_cache.focus_checked_rtl;
		} break;
		case State::HoverPressedCheckedRTL: {
			return data_cache.hover_pressed_checked_rtl;
		} break;
		case State::DisabledCheckedRTL: {
			return data_cache.disabled_checked_rtl;
		} break;
		case State::NormalUncheckedRTL: {
			return data_cache.normal_unchecked_rtl;
		} break;
		case State::PressedUncheckedRTL: {
			return data_cache.pressed_unchecked_rtl;
		} break;
		case State::HoverUncheckedRTL: {
			return data_cache.hover_unchecked_rtl;
		} break;
		case State::FocusUncheckedRTL: {
			return data_cache.focus_unchecked_rtl;
		} break;
		case State::HoverPressedUncheckedRTL: {
			return data_cache.hover_pressed_unchecked_rtl;
		} break;
		case State::DisabledUncheckedRTL: {
			return data_cache.disabled_unchecked_rtl;
		} break;
		default: {
			return data_cache.normal;
		} break;
	}
}

template <typename T>
void ThemeData<T>::set_data(T p_data, State p_state) {
	switch (p_state) {
		case State::NormalNoneLTR: {
			data_cache.normal = p_data;
		} break;
		case State::PressedNoneLTR: {
			data_cache.pressed = p_data;
		} break;
		case State::HoverNoneLTR: {
			data_cache.hover = p_data;
		} break;
		case State::FocusNoneLTR: {
			data_cache.focus = p_data;
		} break;
		case State::HoverPressedNoneLTR: {
			data_cache.hover_pressed = p_data;
		} break;
		case State::DisabledNoneLTR: {
			data_cache.disabled = p_data;
		} break;
		case State::NormalCheckedLTR: {
			data_cache.normal_checked = p_data;
		} break;
		case State::PressedCheckedLTR: {
			data_cache.pressed_checked = p_data;
		} break;
		case State::HoverCheckedLTR: {
			data_cache.hover_checked = p_data;
		} break;
		case State::FocusCheckedLTR: {
			data_cache.focus_checked = p_data;
		} break;
		case State::HoverPressedCheckedLTR: {
			data_cache.hover_pressed_checked = p_data;
		} break;
		case State::DisabledCheckedLTR: {
			data_cache.disabled_checked = p_data;
		} break;
		case State::NormalUncheckedLTR: {
			data_cache.normal_unchecked = p_data;
		} break;
		case State::PressedUncheckedLTR: {
			data_cache.pressed_unchecked = p_data;
		} break;
		case State::HoverUncheckedLTR: {
			data_cache.hover_unchecked = p_data;
		} break;
		case State::FocusUncheckedLTR: {
			data_cache.focus_unchecked = p_data;
		} break;
		case State::HoverPressedUncheckedLTR: {
			data_cache.hover_pressed_unchecked = p_data;
		} break;
		case State::DisabledUncheckedLTR: {
			data_cache.disabled_unchecked = p_data;
		} break;
		case State::NormalNoneRTL: {
			data_cache.normal_rtl = p_data;
		} break;
		case State::PressedNoneRTL: {
			data_cache.pressed_rtl = p_data;
		} break;
		case State::HoverNoneRTL: {
			data_cache.hover_rtl = p_data;
		} break;
		case State::FocusNoneRTL: {
			data_cache.focus_rtl = p_data;
		} break;
		case State::HoverPressedNoneRTL: {
			data_cache.hover_pressed_rtl = p_data;
		} break;
		case State::DisabledNoneRTL: {
			data_cache.disabled_rtl = p_data;
		} break;
		case State::NormalCheckedRTL: {
			data_cache.normal_checked_rtl = p_data;
		} break;
		case State::PressedCheckedRTL: {
			data_cache.pressed_checked_rtl = p_data;
		} break;
		case State::HoverCheckedRTL: {
			data_cache.hover_checked_rtl = p_data;
		} break;
		case State::FocusCheckedRTL: {
			data_cache.focus_checked_rtl = p_data;
		} break;
		case State::HoverPressedCheckedRTL: {
			data_cache.hover_pressed_checked_rtl = p_data;
		} break;
		case State::DisabledCheckedRTL: {
			data_cache.disabled_checked_rtl = p_data;
		} break;
		case State::NormalUncheckedRTL: {
			data_cache.normal_unchecked_rtl = p_data;
		} break;
		case State::PressedUncheckedRTL: {
			data_cache.pressed_unchecked_rtl = p_data;
		} break;
		case State::HoverUncheckedRTL: {
			data_cache.hover_unchecked_rtl = p_data;
		} break;
		case State::FocusUncheckedRTL: {
			data_cache.focus_unchecked_rtl = p_data;
		} break;
		case State::HoverPressedUncheckedRTL: {
			data_cache.hover_pressed_unchecked_rtl = p_data;
		} break;
		case State::DisabledUncheckedRTL: {
			data_cache.disabled_unchecked_rtl = p_data;
		} break;
		default: {
			data_cache.normal = p_data;
		} break;
	}
}

template <typename T>
Vector<State> ThemeData<T>::get_search_order(State p_state) const {
	return search_order[p_state];
}

typedef ThemeData<Color> ThemeColorData;
typedef ThemeData<Ref<ColorRole>> ThemeColorRoleData;
typedef ThemeData<Ref<ColorScheme>> ThemeColorSchemeData;
typedef ThemeData<Ref<Font>> ThemeFontData;
typedef ThemeData<Ref<Texture2D>> ThemeIconData;
typedef ThemeData<Ref<StyleBox>> ThemeStyleboxData;
typedef ThemeData<String> ThemeStrData;
typedef ThemeData<int> ThemeIntData;

#endif // THEME_DATA_H

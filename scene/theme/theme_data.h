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

	void set_data_name(const String &p_data_name);


	String get_state_data_name(State p_state = State::NormalNoneLTR);

	T get_loop_data(State p_state = State::NormalNoneLTR);

	T get_data(State p_state = State::NormalNoneLTR);
	void set_data(T p_data, State p_state = State::NormalNoneLTR);

	bool has_data(State p_state);

	bool has_loop_data(State p_state);

	ThemeData<T>() {

	}
	~ThemeData<T>() {}

};



template <typename T>
void ThemeData<T>::set_data_name(const String &p_data_name) {
	data_name = p_data_name;
}

template <typename T>
String ThemeData<T>::get_state_data_name(State p_state){
	return data_name + state_suffix_name[p_state]
}

template <typename T>
T ThemeData<T>::get_loop_data(State p_state) {
	for (const State &E : search_order[p_state]) {
		if (has_data(E)) {
			return get_data(E);
		}
	}
	return <T>();
}

template <typename T>
T ThemeData<T>::get_data(State p_state) {
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
	}
}

template <typename T>
void ThemeData<T>::set_data(T p_data, State p_state) {
	data_map[p_state] = true;
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
	}
}


template <typename T>
bool ThemeData<T>::has_data(State p_state) {
	return has_data_map[p_state];
}

template <typename T>
bool ThemeData<T>::has_loop_data(State p_state) {
	for (const State &E : search_order[p_state]) {
		if (has_data(E)) {
			return true;
		}
	}
	return false;
}

typedef ThemeData<Color> ThemeColorData;
typedef ThemeData<ColorRole> ThemeColorRoleData;
typedef ThemeData<Ref<ColorScheme>> ThemeColorSchemeData;
typedef ThemeData<Ref<Font>> ThemeFontData;
typedef ThemeData<Ref<Texture2D>> ThemeIconData;
typedef ThemeData<Ref<StyleBox>> ThemeStyleboxData;
typedef ThemeData<String> ThemeTextData;

#endif // THEME_DATA_H

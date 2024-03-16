/**************************************************************************/
/*  theme_db.cpp                                                          */
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

#include "theme_data.h"

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

static const char *state_suffix_name[STATE_MAX] = {
	"_normal",
	"_pressed",
	"_hover",
	"_focus",
	"_hover_pressed",
	"_disabled",

	"_normal_checked",
	"_pressed_checked",
	"_hover_checked",
	"_focus_checked",
	"_hover_pressed_checked",
	"_disabled_checked",

	"_normal_unchecked",
	"_pressed_unchecked",
	"_hover_unchecked",
	"_focus_unchecked",
	"_hover_pressed_unchecked",
	"_disabled_unchecked",

	"_normal_mirrored",
	"_pressed_mirrored",
	"_hover_mirrored",
	"_focus_mirrored",
	"_hover_pressed_mirrored",
	"_disabled_mirrored",

	"_normal_checked_mirrored",
	"_pressed_checked_mirrored",
	"_hover_checked_mirrored",
	"_focus_checked_mirrored",
	"_hover_pressed_checked_mirrored",
	"_disabled_checked_mirrored",

	"_normal_unchecked_mirrored",
	"_pressed_unchecked_mirrored",
	"_hover_unchecked_mirrored",
	"_focus_unchecked_mirrored",
	"_hover_pressed_unchecked_mirrored",
	"_disabled_unchecked_mirrored"
};



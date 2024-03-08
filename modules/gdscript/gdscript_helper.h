/*************************************************************************/
/*  gdscript_helper.h                                             */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2023 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2023 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef GDSCRIPT_HELPER_H
#define GDSCRIPT_HELPER_H

#include "core/object/ref_counted.h"
#include "gdscript.h"

class GDScriptHelper : public RefCounted {
	GDCLASS(GDScriptHelper, RefCounted);
	List<String> r_functions;
	List<ScriptLanguage::ScriptError> r_errors;
	List<ScriptLanguage::CodeCompletionOption> r_options;
	String code_hint;
	bool complete_forced = false;

protected:
	static void _bind_methods();

public:
	enum LookupResultType {
		LOOKUP_RESULT_SCRIPT_LOCATION,
		LOOKUP_RESULT_CLASS,
		LOOKUP_RESULT_CLASS_CONSTANT,
		LOOKUP_RESULT_CLASS_PROPERTY,
		LOOKUP_RESULT_CLASS_METHOD,
		LOOKUP_RESULT_CLASS_SIGNAL,
		LOOKUP_RESULT_CLASS_ENUM,
		LOOKUP_RESULT_CLASS_TBD_GLOBALSCOPE,
		LOOKUP_RESULT_CLASS_ANNOTATION,
		LOOKUP_RESULT_MAX
	};

	bool set_validate_code(const String &p_code, const String &p_path = "");
	bool has_errors() const;
	TypedArray<Dictionary> get_errors() const;
	bool has_functions() const;
	Dictionary get_functions() const;

	Error set_completion_code(const String &p_code, const String &p_path = "", Object *p_owner = nullptr);
	bool has_completion_options() const;
	TypedArray<Dictionary> get_completion_options() const;
	bool has_completion_hint() const;
	String get_completion_hint() const;
	bool is_completion_forced() const;

	Dictionary lookup_code(const String &p_code, const String &p_symbol, const String &p_path = "", Object *p_owner = nullptr);

	GDScriptHelper();
	~GDScriptHelper();
};

const int KIND_COUNT = 10;
// The order in which to sort code completion options.
const ScriptLanguage::CodeCompletionKind KIND_SORT_ORDER[KIND_COUNT] = {
	ScriptLanguage::CODE_COMPLETION_KIND_VARIABLE,
	ScriptLanguage::CODE_COMPLETION_KIND_MEMBER,
	ScriptLanguage::CODE_COMPLETION_KIND_FUNCTION,
	ScriptLanguage::CODE_COMPLETION_KIND_ENUM,
	ScriptLanguage::CODE_COMPLETION_KIND_SIGNAL,
	ScriptLanguage::CODE_COMPLETION_KIND_CONSTANT,
	ScriptLanguage::CODE_COMPLETION_KIND_CLASS,
	ScriptLanguage::CODE_COMPLETION_KIND_NODE_PATH,
	ScriptLanguage::CODE_COMPLETION_KIND_FILE_PATH,
	ScriptLanguage::CODE_COMPLETION_KIND_PLAIN_TEXT,
};

struct CustomCodeCompletionOptionCompare {
	_FORCE_INLINE_ bool operator()(const ScriptLanguage::CodeCompletionOption &l, const ScriptLanguage::CodeCompletionOption &r) const {
		if (l.location == r.location) {
			// If locations are same, sort on kind
			if (l.kind == r.kind) {
				// If kinds are same, sort alphanumeric
				return l.display < r.display;
			}

			// Sort kinds based on the const sorting array defined above. Lower index = higher priority.
			int l_index = -1;
			int r_index = -1;
			for (int i = 0; i < KIND_COUNT; i++) {
				const ScriptLanguage::CodeCompletionKind kind = KIND_SORT_ORDER[i];
				l_index = kind == l.kind ? i : l_index;
				r_index = kind == r.kind ? i : r_index;

				if (l_index != -1 && r_index != -1) {
					return l_index < r_index;
				}
			}

			// This return should never be hit unless something goes wrong.
			// l and r should always have a Kind which is in the sort order array.
			return l.display < r.display;
		}

		return l.location < r.location;
	}
};

VARIANT_ENUM_CAST(GDScriptHelper::LookupResultType);

#endif // GDSCRIPT_HELPER_H
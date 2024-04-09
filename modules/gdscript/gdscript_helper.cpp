/*************************************************************************/
/*  gdscript_helper.cpp                                           */
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

#include "gdscript_helper.h"
#include "scene\gui\code_edit.h"

void GDScriptHelper::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_validate_code", "code", "script_path"), &GDScriptHelper::set_validate_code, DEFVAL(String()));
	ClassDB::bind_method("has_errors", &GDScriptHelper::has_errors);
	ClassDB::bind_method("get_errors", &GDScriptHelper::get_errors);
	ClassDB::bind_method("has_functions", &GDScriptHelper::has_functions);
	ClassDB::bind_method("get_functions", &GDScriptHelper::get_functions);

	ClassDB::bind_method(D_METHOD("set_completion_code", "code", "script_path", "script_owner"), &GDScriptHelper::set_completion_code, DEFVAL(String()), DEFVAL(Variant()));
	ClassDB::bind_method("has_completion_options", &GDScriptHelper::has_completion_options);
	ClassDB::bind_method("get_completion_options", &GDScriptHelper::get_completion_options);
	ClassDB::bind_method("has_completion_hint", &GDScriptHelper::has_completion_hint);
	ClassDB::bind_method("get_completion_hint", &GDScriptHelper::get_completion_hint);
	ClassDB::bind_method("is_completion_forced", &GDScriptHelper::is_completion_forced);

	ClassDB::bind_method(D_METHOD("lookup_code", "code", "symbol", "script_path", "script_owner"), &GDScriptHelper::lookup_code, DEFVAL(String()), DEFVAL(Variant()));

	BIND_ENUM_CONSTANT(LOOKUP_RESULT_SCRIPT_LOCATION);
	BIND_ENUM_CONSTANT(LOOKUP_RESULT_CLASS);
	BIND_ENUM_CONSTANT(LOOKUP_RESULT_CLASS_CONSTANT);
	BIND_ENUM_CONSTANT(LOOKUP_RESULT_CLASS_PROPERTY);
	BIND_ENUM_CONSTANT(LOOKUP_RESULT_CLASS_METHOD);
	BIND_ENUM_CONSTANT(LOOKUP_RESULT_CLASS_SIGNAL);
	BIND_ENUM_CONSTANT(LOOKUP_RESULT_CLASS_ENUM);
	BIND_ENUM_CONSTANT(LOOKUP_RESULT_CLASS_TBD_GLOBALSCOPE);
	BIND_ENUM_CONSTANT(LOOKUP_RESULT_CLASS_ANNOTATION);
	BIND_ENUM_CONSTANT(LOOKUP_RESULT_MAX);
}

bool GDScriptHelper::set_validate_code(const String &p_code, const String &p_path) {
	r_functions.clear();
	r_errors.clear();
	GDScriptLanguage *lang = GDScriptLanguage::get_singleton();
	return lang->validate(p_code, p_path, &r_functions, &r_errors);
}

bool GDScriptHelper::has_errors() const {
	return !r_errors.is_empty();
}

TypedArray<Dictionary> GDScriptHelper::get_errors() const {
	TypedArray<Dictionary> list;
	for (const ScriptLanguage::ScriptError &e : r_errors) {
		Dictionary error;
		error["line"] = e.line-1;
		error["column"] = e.column-1;
		error["message"] = e.message;
		list.append(error);
	}
	return list;
}

bool GDScriptHelper::has_functions() const {
	return !r_functions.is_empty();
}

Dictionary GDScriptHelper::get_functions() const {
	Dictionary list;
	for (const String &e : r_functions) {
		list[e.get_slice(":", 0)] = e.get_slice(":", 1).to_int()-1;
	}
	return list;
}

Error GDScriptHelper::set_completion_code(const String &p_code, const String &p_path, Object *p_owner) {
	r_options.clear();
	complete_forced = false;
	code_hint = "";
	GDScriptLanguage *lang = GDScriptLanguage::get_singleton();
	Error err = lang->complete_code(p_code, p_path, p_owner, &r_options, complete_forced, code_hint);
	r_options.sort_custom_inplace<CustomCodeCompletionOptionCompare>();
	return err;
}

bool GDScriptHelper::has_completion_options() const {
	return !r_options.is_empty();
}

TypedArray<Dictionary> GDScriptHelper::get_completion_options() const {
	TypedArray<Dictionary> list;
	for (const ScriptLanguage::CodeCompletionOption &e : r_options) {
		Dictionary option;
		option["type"] = (CodeEdit::CodeCompletionKind)e.kind;
		option["display_text"] = e.display;
		option["insert_text"] = e.insert_text;
		option["default_value"] = e.default_value;
		list.append(option);
	}
	return list;
}

bool GDScriptHelper::has_completion_hint() const {
	return !code_hint.is_empty();
}

String GDScriptHelper::get_completion_hint() const {
	return code_hint;
}

bool GDScriptHelper::is_completion_forced() const {
	return complete_forced;
}

Dictionary GDScriptHelper::lookup_code(const String &p_code, const String &p_symbol, const String &p_path, Object *p_owner) {
	Dictionary result_dict;
	ScriptLanguage::LookupResult lookup_result;
	GDScriptLanguage *lang = GDScriptLanguage::get_singleton();
	if (lang->lookup_code(p_code, p_symbol, p_path, p_owner, lookup_result) == OK) {
		result_dict["type"] = (LookupResultType)lookup_result.type;
		result_dict["script"] = lookup_result.script;
		result_dict["class_name"] = lookup_result.class_name;
		result_dict["class_member"] = lookup_result.class_member;
		result_dict["class_path"] = lookup_result.class_path;
		result_dict["location"] = lookup_result.location;
	}
	return result_dict;
}

GDScriptHelper::GDScriptHelper() = default;

GDScriptHelper::~GDScriptHelper() = default;
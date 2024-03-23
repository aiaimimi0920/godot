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

#ifndef THEME_DB_H
#define THEME_DB_H

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "scene/resources/theme.h"

#include <functional>

class Font;
class Node;
class StyleBox;
class Texture2D;
class ThemeContext;
class ColorScheme;

// Macros for binding theme items of this class. This information is used for the documentation, theme
// overrides, etc. This is also the basis for theme cache.

#define BIND_THEME_ITEM(m_data_type, m_class, m_prop)                                                                   \
	ThemeDB::get_singleton()->bind_class_item(m_data_type, get_class_static(), #m_prop, #m_prop, [](Node *p_instance) { \
		m_class *p_cast = Object::cast_to<m_class>(p_instance);                                                         \
		p_cast->theme_cache.m_prop = p_cast->get_theme_item(m_data_type, _scs_create(#m_prop));                         \
	})

#define BIND_THEME_ITEM_MULTI(m_data_type, m_class, m_prop)                                                                                                                                             \
	ThemeIntData cur_theme_data_##m_prop;                                                                                                                                                               \
	cur_theme_data_##m_prop.set_data_name(#m_prop);                                                                                                                                                     \
	for (int i = 0; i < STATE_MAX; i++) {                                                                                                                                                               \
		String cur_theme_data_state_##m_prop = cur_theme_data_##m_prop.get_state_data_name(static_cast<State>(i));                                                                                      \
		ThemeDB::get_singleton()->bind_class_item(m_data_type, get_class_static(), cur_theme_data_state_##m_prop, cur_theme_data_state_##m_prop, [i, cur_theme_data_state_##m_prop](Node *p_instance) { \
			m_class *p_cast = Object::cast_to<m_class>(p_instance);                                                                                                                                     \
			p_cast->theme_cache.m_prop.set_data(p_cast->get_theme_item(m_data_type, cur_theme_data_state_##m_prop), static_cast<State>(i));                                                             \
		});                                                                                                                                                                                             \
	}

#define BIND_THEME_ITEM_CUSTOM(m_data_type, m_class, m_prop, m_item_name)                                                   \
	ThemeDB::get_singleton()->bind_class_item(m_data_type, get_class_static(), #m_prop, m_item_name, [](Node *p_instance) { \
		m_class *p_cast = Object::cast_to<m_class>(p_instance);                                                             \
		p_cast->theme_cache.m_prop = p_cast->get_theme_item(m_data_type, _scs_create(m_item_name));                         \
	})

#define BIND_THEME_ITEM_CUSTOM_MULTI(m_data_type, m_class, m_prop, m_item_name)                                                                                                                                   \
	ThemeIntData cur_theme_data_##m_item_name;                                                                                                                                                                    \
	cur_theme_data_##m_item_name.set_data_name(#m_item_name);                                                                                                                                                     \
	ThemeIntData cur_theme_data_##m_prop;                                                                                                                                                                         \
	cur_theme_data_##m_prop.set_data_name(#m_prop);                                                                                                                                                               \
	for (int i = 0; i < STATE_MAX; i++) {                                                                                                                                                                         \
		String cur_theme_data_state_##m_item_name = cur_theme_data_##m_item_name.get_state_data_name(static_cast<State>(i));                                                                                      \
		String cur_theme_data_state_##m_prop = cur_theme_data_##m_prop.get_state_data_name(static_cast<State>(i));                                                                                                \
		ThemeDB::get_singleton()->bind_class_item(m_data_type, get_class_static(), cur_theme_data_state_##m_prop, cur_theme_data_state_##m_item_name, [i, cur_theme_data_state_##m_item_name](Node *p_instance) { \
			m_class *p_cast = Object::cast_to<m_class>(p_instance);                                                                                                                                               \
			p_cast->theme_cache.m_prop.set_data(p_cast->get_theme_item(m_data_type, cur_theme_data_state_##m_item_name), static_cast<State>(i));                                                                  \
		});                                                                                                                                                                                                       \
	}

// Macro for binding theme items used by this class, but defined/binded by other classes. This is primarily used for
// the theme cache. Can also be used to list such items in documentation.

#define BIND_THEME_ITEM_EXT(m_data_type, m_class, m_prop, m_item_name, m_type_name)                                                               \
	ThemeDB::get_singleton()->bind_class_external_item(m_data_type, get_class_static(), #m_prop, m_item_name, m_type_name, [](Node *p_instance) { \
		m_class *p_cast = Object::cast_to<m_class>(p_instance);                                                                                   \
		p_cast->theme_cache.m_prop = p_cast->get_theme_item(m_data_type, _scs_create(m_item_name), _scs_create(m_type_name));                     \
	})

#define BIND_THEME_ITEM_EXT_MULTI(m_data_type, m_class, m_prop, m_item_name, m_type_name)                                                                                                                                      \
	ThemeIntData cur_theme_data_##m_item_name;                                                                                                                                                                                 \
	cur_theme_data_##m_item_name.set_data_name(#m_item_name);                                                                                                                                                                  \
	ThemeIntData cur_theme_data_##m_prop;                                                                                                                                                                                      \
	cur_theme_data_##m_prop.set_data_name(#m_prop);                                                                                                                                                                            \
	for (int i = 0; i < STATE_MAX; i++) {                                                                                                                                                                                      \
		String cur_theme_data_state_##m_item_name = cur_theme_data_##m_item_name.get_state_data_name(static_cast<State>(i));                                                                                                   \
		String cur_theme_data_state_##m_prop = cur_theme_data_##m_prop.get_state_data_name(static_cast<State>(i));                                                                                                             \
		ThemeDB::get_singleton()->bind_class_external_item(m_data_type, get_class_static(), cur_theme_data_state_##m_prop, cur_theme_data_state_##m_item_name, m_type_name, [i, cur_theme_data_state_##m_item_name](Node *p_instance) { \
			m_class *p_cast = Object::cast_to<m_class>(p_instance);                                                                                                                                                            \
			p_cast->theme_cache.m_prop.set_data(p_cast->get_theme_item(m_data_type, cur_theme_data_state_##m_item_name, _scs_create(m_type_name)), static_cast<State>(i));                                                     \
		});                                                                                                                                                                                                                    \
	}

class ThemeDB : public Object {
	GDCLASS(ThemeDB, Object);

	static ThemeDB *singleton;

	// Global Theme resources used by the default theme context.

	Ref<Theme> default_theme;
	Ref<Theme> project_theme;

	// Universal default values, final fallback for every theme.

	float fallback_base_scale = 1.0;
	Ref<Font> fallback_font;
	Ref<Font> fallback_icon_font;

	int fallback_font_size = 16;
	Ref<Texture2D> fallback_icon;
	Ref<StyleBox> fallback_stylebox;
	Ref<ColorScheme> fallback_color_scheme;

	// Global theme contexts used to scope global Theme resources.

	ThemeContext *default_theme_context = nullptr;
	HashMap<Node *, ThemeContext *> theme_contexts;

	void _propagate_theme_context(Node *p_from_node, ThemeContext *p_context);
	void _init_default_theme_context();
	void _finalize_theme_contexts();

	// Binding of theme items to Node classes.

public:
	typedef std::function<void(Node *)> ThemeItemSetter;

	struct ThemeItemBind {
		Theme::DataType data_type;
		StringName class_name;
		StringName item_name;
		StringName type_name;
		bool external = false;

		ThemeItemSetter setter;

		struct SortByType {
			_FORCE_INLINE_ bool operator()(const ThemeItemBind &l, const ThemeItemBind &r) const {
				return l.data_type < r.data_type;
			}
		};
	};

private:
	HashMap<StringName, HashMap<StringName, ThemeItemBind>> theme_item_binds;
	HashMap<StringName, List<ThemeItemBind>> theme_item_binds_list; // Used for listing purposes.

	void _sort_theme_items();

protected:
	static void _bind_methods();

public:
	void initialize_theme();
	void initialize_theme_noproject();
	void finalize_theme();

	// Global Theme resources.

	void set_default_theme(const Ref<Theme> &p_default);
	Ref<Theme> get_default_theme();

	void set_project_theme(const Ref<Theme> &p_project_default);
	Ref<Theme> get_project_theme();

	// Universal fallback values.

	void set_fallback_base_scale(float p_base_scale);
	float get_fallback_base_scale();

	void set_fallback_font(const Ref<Font> &p_font);
	Ref<Font> get_fallback_font();

	void set_fallback_icon_font(const Ref<Font> &p_icon_font);
	Ref<Font> get_fallback_icon_font();

	void set_fallback_font_size(int p_font_size);
	int get_fallback_font_size();

	void set_fallback_color_scheme(const Ref<ColorScheme> p_color_scheme);
	Ref<ColorScheme> get_fallback_color_scheme();

	void set_fallback_icon(const Ref<Texture2D> &p_icon);
	Ref<Texture2D> get_fallback_icon();

	void set_fallback_stylebox(const Ref<StyleBox> &p_stylebox);
	Ref<StyleBox> get_fallback_stylebox();

	void get_native_type_dependencies(const StringName &p_base_type, List<StringName> *p_list);

	// Global theme contexts.

	ThemeContext *create_theme_context(Node *p_node, List<Ref<Theme>> &p_themes);
	void destroy_theme_context(Node *p_node);

	ThemeContext *get_theme_context(Node *p_node) const;
	ThemeContext *get_default_theme_context() const;
	ThemeContext *get_nearest_theme_context(Node *p_for_node) const;

	// Theme item binding.

	void bind_class_item(Theme::DataType p_data_type, const StringName &p_class_name, const StringName &p_prop_name, const StringName &p_item_name, ThemeItemSetter p_setter);
	void bind_class_external_item(Theme::DataType p_data_type, const StringName &p_class_name, const StringName &p_prop_name, const StringName &p_item_name, const StringName &p_type_name, ThemeItemSetter p_setter);
	void update_class_instance_items(Node *p_instance);

	void get_class_items(const StringName &p_class_name, List<ThemeItemBind> *r_list, bool p_include_inherited = false, Theme::DataType p_filter_type = Theme::DATA_TYPE_MAX);

	// Memory management, reference, and initialization.

	static ThemeDB *get_singleton();
	ThemeDB();
	~ThemeDB();
};

class ThemeContext : public Object {
	GDCLASS(ThemeContext, Object);

	friend class ThemeDB;

	Node *node = nullptr;
	ThemeContext *parent = nullptr;

	// Themes are stacked in the order of relevance, for easy iteration.
	// This means that the first theme is the one you should check first,
	// and the last theme is the fallback theme where every lookup ends.
	List<Ref<Theme>> themes;

	void _emit_changed();

protected:
	static void _bind_methods();

public:
	void set_themes(List<Ref<Theme>> &p_themes);
	List<Ref<Theme>> get_themes() const;
	Ref<Theme> get_fallback_theme() const;
};

#endif // THEME_DB_H

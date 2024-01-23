/**
 * blackboard_source.cpp
 * =============================================================================
 * Copyright 2021-2024 Serhii Snitsaruk
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 * =============================================================================
 */

#include "blackboard_source.h"

bool BlackboardSource::_set(const StringName &p_name, const Variant &p_value) {
	String prop_name = p_name;

	// * Editor
	if (data.has(prop_name)) {
		data[prop_name].set_value(p_value);
		return true;
	}

	// * Storage
	if (prop_name.begins_with("var/")) {
		String var_name = prop_name.get_slicec('/', 1);
		String what = prop_name.get_slicec('/', 2);
		if (!data.has(var_name) && what == "name") {
			data.insert(var_name, BBVariable());
		}
		if (what == "name") {
			// We don't store variable name with the variable.
		} else if (what == "type") {
			data[var_name].set_type((Variant::Type)(int)p_value);
		} else if (what == "value") {
			data[var_name].set_value(p_value);
		} else if (what == "hint") {
			data[var_name].set_hint((PropertyHint)(int)p_value);
		} else if (what == "hint_string") {
			data[var_name].set_hint_string(p_value);
		} else {
			return false;
		}
		return true;
	}

	return false;
}

bool BlackboardSource::_get(const StringName &p_name, Variant &r_ret) const {
	String prop_name = p_name;

	// * Editor
	if (data.has(prop_name)) {
		r_ret = data[prop_name].get_value();
		return true;
	}

	// * Storage
	if (!prop_name.begins_with("var/")) {
		return false;
	}

	String var_name = prop_name.get_slicec('/', 1);
	String what = prop_name.get_slicec('/', 2);
	ERR_FAIL_COND_V(!data.has(var_name), false);
	if (what == "type") {
		r_ret = data[var_name].get_type();
	} else if (what == "value") {
		r_ret = data[var_name].get_value();
	} else if (what == "hint") {
		r_ret = data[var_name].get_hint();
	} else if (what == "hint_string") {
		r_ret = data[var_name].get_hint_string();
	}
	return true;
}

void BlackboardSource::_get_property_list(List<PropertyInfo> *p_list) const {
	for (const KeyValue<String, BBVariable> &kv : data) {
		String var_name = kv.key;
		BBVariable var = kv.value;

		// * Editor
		p_list->push_back(PropertyInfo(var.get_type(), var_name, var.get_hint(), var.get_hint_string(), PROPERTY_USAGE_EDITOR));

		// * Storage
		p_list->push_back(PropertyInfo(Variant::STRING, "var/" + var_name + "/name", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL));
		p_list->push_back(PropertyInfo(Variant::INT, "var/" + var_name + "/type", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL));
		p_list->push_back(PropertyInfo(var.get_type(), "var/" + var_name + "/value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL));
		p_list->push_back(PropertyInfo(Variant::INT, "var/" + var_name + "/hint", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL));
		p_list->push_back(PropertyInfo(Variant::STRING, "var/" + var_name + "/hint_string", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL));
	}
}

bool BlackboardSource::_property_can_revert(const StringName &p_name) const {
	return base.is_valid() && base->data.has(p_name);
}

bool BlackboardSource::_property_get_revert(const StringName &p_name, Variant &r_property) const {
	if (base->data.has(p_name)) {
		r_property = base->data[p_name].get_value();
		return true;
	}
	return false;
}

void BlackboardSource::set_base_source(const Ref<BlackboardSource> &p_base) {
	base = p_base;
	sync_with_base_source();
	emit_changed();
}

void BlackboardSource::set_value(const String &p_name, const Variant &p_value) {
	ERR_FAIL_COND(!data.has(p_name));
	data.get(p_name).set_value(p_value);
}

Variant BlackboardSource::get_value(const String &p_name) const {
	ERR_FAIL_COND_V(!data.has(p_name), Variant());
	return data.get(p_name).get_value();
}

void BlackboardSource::add_var(const String &p_name, const BBVariable &p_var) {
	ERR_FAIL_COND(data.has(p_name));
	ERR_FAIL_COND(base.is_valid());
	data.insert(p_name, p_var);
}

void BlackboardSource::remove_var(const String &p_name) {
	ERR_FAIL_COND(!data.has(p_name));
	ERR_FAIL_COND(base.is_valid());
	data.erase(p_name);
}

BBVariable BlackboardSource::get_var(const String &p_name) {
	ERR_FAIL_COND_V(!data.has(p_name), BBVariable());
	return data.get(p_name);
}

PackedStringArray BlackboardSource::list_vars() const {
	PackedStringArray ret;
	for (const KeyValue<String, BBVariable> &kv : data) {
		ret.append(kv.key);
	}
	return ret;
}

void BlackboardSource::sync_with_base_source() {
	if (base.is_null()) {
		return;
	}
	for (const KeyValue<String, BBVariable> &kv : base->data) {
		if (!data.has(kv.key)) {
			data.insert(kv.key, kv.value.duplicate());
			continue;
		}

		BBVariable var = data.get(kv.key);
		if (!var.is_same_prop_info(kv.value)) {
			var.copy_prop_info(kv.value);
		}
		if (var.get_value().get_type() != kv.value.get_type()) {
			var.set_value(kv.value.get_value());
		}
	}
}

Ref<Blackboard> BlackboardSource::create_blackboard() {
	Ref<Blackboard> bb = memnew(Blackboard);
	for (const KeyValue<String, BBVariable> &kv : data) {
		bb->add_var(kv.key, kv.value.duplicate());
	}
	return bb;
}

void BlackboardSource::populate_blackboard(const Ref<Blackboard> &p_blackboard, bool overwrite) {
	for (const KeyValue<String, BBVariable> &kv : data) {
		if (p_blackboard->has_var(kv.key)) {
			if (overwrite) {
				p_blackboard->erase_var(kv.key);
			} else {
				continue;
			}
		}
		p_blackboard->add_var(kv.key, kv.value.duplicate());
	}
}

BlackboardSource::BlackboardSource() {
	// TODO: REMOVE ALL BELOW
	data.insert("speed", BBVariable(Variant::Type::FLOAT, PropertyHint::PROPERTY_HINT_NONE, ""));
	data.insert("limit_speed", BBVariable(Variant::Type::BOOL, PropertyHint::PROPERTY_HINT_NONE, ""));
	data.insert("about", BBVariable(Variant::Type::STRING, PropertyHint::PROPERTY_HINT_MULTILINE_TEXT, ""));
}

/**************************************************************************/
/*  refactor_unique_name_dialog.cpp                                       */
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

#include "refactor_unique_name_dialog.h"

#include "core/object/callable_mp.h"
#include "core/object/script_language.h"
#include "editor/editor_node.h"
#include "editor/editor_string_names.h"
#include "scene/main/scene_tree.h"

#include <editor/themes/editor_scale.h>
#include <modules/gdscript/gdscript.h>

RefactorUniqueNameDialog::RefactorUniqueNameDialog() {
	set_ok_button_text(TTRC("Rename in scripts"));

	VBoxContainer *vbox = memnew(VBoxContainer);
	add_child(vbox);

	label = memnew(Label);
	label->set_focus_mode(Control::FOCUS_ACCESSIBILITY);
	vbox->add_child(label);

	scene_tree_selector = memnew(SceneTreeSelector);
	scene_tree_selector->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	scene_tree_selector->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	vbox->add_child(scene_tree_selector);
}

void RefactorUniqueNameDialog::add_refactor(const StringName &p_old_name, const StringName &p_new_name) {
	Vector<ObjectID> nodes_to_consider = _get_nodes_to_refactor(p_old_name);
	refactor_queue.push_back(RefactorData{ nodes_to_consider, p_old_name, p_new_name });
	if (!is_visible()) {
		_next();
	}
}

void RefactorUniqueNameDialog::_next() {
	if (refactor_queue.is_empty()) {
		return;
	}
	const RefactorData &refactor_data = refactor_queue[0];
	label->set_text(vformat(TTR("Unique node renamed: %s -> %s"), refactor_data.old_name, refactor_data.new_name));
	popup_refactor();
}

void RefactorUniqueNameDialog::ok_pressed() {
	if (!refactor_queue.is_empty()) {
		const RefactorData &refactor_data = refactor_queue[0];
		_refactor_unique_name(refactor_data);
		refactor_queue.remove_at(0);
	}
	callable_mp(this, &RefactorUniqueNameDialog::_next).call_deferred();
}

void RefactorUniqueNameDialog::popup_refactor() {
	popup_centered_clamped(Size2(350, 700) * EDSCALE);
}

void RefactorUniqueNameDialog::cancel_pressed() {
	if (!refactor_queue.is_empty()) {
		refactor_queue.remove_at(0);
	}
	callable_mp(this, &RefactorUniqueNameDialog::_next).call_deferred();
}

Node *RefactorUniqueNameDialog::get_scene_root() const {
	ERR_FAIL_COND_V(!is_inside_tree(), nullptr);

	return get_tree()->get_edited_scene_root();
}

void RefactorUniqueNameDialog::_refactor_unique_name(RefactorData refactor_data) {
	for (int i = 0; i < refactor_data.nodes_to_consider.size(); i++) {
		Node *n = Object::cast_to<Node>(ObjectDB::get_instance(refactor_data.nodes_to_consider[i]));
		if (!n) {
			continue;
		}

		if (!n->get_script().is_null()) {
			//TODO
		}
	}
}

void RefactorUniqueNameDialog::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (!is_visible()) {
				return;
			}
			const RefactorData &refactor_data = refactor_queue[0];
			scene_tree_selector->create(get_scene_root(), refactor_data.nodes_to_consider);
		} break;
	}
}

Vector<ObjectID> RefactorUniqueNameDialog::_get_nodes_to_refactor(const StringName &p_old_name) {
	ERR_FAIL_NULL_V(get_scene_root(), Vector<ObjectID>());
	Vector<ObjectID> nodes_to_refactor;
	LocalVector<Node *> stack;

	stack.push_back(get_scene_root());
	while (!stack.is_empty()) {
		Node *current = stack[stack.size() - 1];
		stack.remove_at(stack.size() - 1);

		Ref<Script> script = current->get_script();
		if (!script.is_null()) {
			ScriptLanguage *gdscript_language = script.ptr()->get_language();
			if (gdscript_language->get_name() == "GDScript" && script.ptr()->get_source_code().contains("%" + p_old_name)) {
				nodes_to_refactor.push_back(current->get_instance_id());
			}
		}

		for (int i = 0; i < current->get_child_count(); i++) {
			stack.push_back(current->get_child(i));
		}
	}

	return nodes_to_refactor;
}

void SceneTreeSelector::_add_item(Node *p_parent, Node *p_node, int p_index) {
	TreeItem *parent_item = nullptr;
	if (p_parent && node_item_map.has(p_parent->get_instance_id())) {
		parent_item = Object::cast_to<TreeItem>(ObjectDB::get_instance(node_item_map.get(p_parent->get_instance_id())));
	}
	TreeItem *item = scene_tree->create_item(parent_item, p_index);
	node_item_map.insert(p_node->get_instance_id(), item->get_instance_id());
	item_node_map.insert(item->get_instance_id(), p_node->get_instance_id());

	if (selectable_nodes.has(p_node->get_instance_id())) {
		item->set_cell_mode(0, TreeItem::CELL_MODE_CHECK);
		item->set_selectable(0, true);
		item->set_editable(0, true);
	} else {
		item->set_cell_mode(0, TreeItem::CELL_MODE_STRING);
		item->set_selectable(0, false);
		item->set_editable(0, false);
	}
	item->set_text(0, p_node->get_name());
	item->set_icon(0, EditorNode::get_singleton()->get_object_icon(p_node));
}

void SceneTreeSelector::_on_item_edited() {
	TreeItem *edited_item = scene_tree->get_edited();
	int edited_column = scene_tree->get_edited_column();

	if (!edited_item) {
		return;
	}

	if (edited_item->get_cell_mode(edited_column) == TreeItem::CELL_MODE_CHECK) {
		bool is_checked = edited_item->is_checked(edited_column);
		ObjectID id = item_node_map.get(edited_item->get_instance_id());
		if (is_checked) {
			selected_nodes.push_back(id);
		} else {
			selected_nodes.erase(id);
		}
	}
}

void SceneTreeSelector::_reset() {
	clear();
	_add_item(nullptr, scene_root, 0);
	_update_subtree(scene_root);
}

void SceneTreeSelector::_update_subtree(Node *p_node) {
	for (int i = 0; i < p_node->get_child_count(); i++) {
		Node *child = p_node->get_child(i);
		_add_item(p_node, child, i);
		_update_subtree(child);
	}
}

void SceneTreeSelector::clear() {
	scene_tree->clear();
	item_node_map.clear();
	node_item_map.clear();
	selected_nodes.clear();
}

void SceneTreeSelector::create(Node *p_root, Vector<ObjectID> p_selectable_nodes) {
	ERR_FAIL_NULL(p_root);
	scene_root = p_root;
	selectable_nodes = p_selectable_nodes;
	_reset();
}

Vector<Node *> SceneTreeSelector::get_selected_nodes() const {
	Vector<Node *> nodes;
	for (int i = 0; i < selected_nodes.size(); i++) {
		Object *obj = ObjectDB::get_instance(selected_nodes[i]);
		Node *node = Object::cast_to<Node>(obj);
		if (node) {
			nodes.push_back(node);
		}
	}
	return nodes;
}

SceneTreeSelector::SceneTreeSelector() {
	scene_tree = memnew(Tree);
	scene_tree->set_auto_translate_mode(AUTO_TRANSLATE_MODE_DISABLED);
	scene_tree->set_anchor(SIDE_RIGHT, ANCHOR_END);
	scene_tree->set_anchor(SIDE_BOTTOM, ANCHOR_END);
	scene_tree->set_allow_reselect(true);
	scene_tree->add_theme_constant_override("button_margin", 0);
	scene_tree->add_theme_constant_override("icon_max_width", get_theme_constant(SNAME("class_icon_size"), EditorStringName(Editor)));
	add_child(scene_tree);

	scene_tree->connect("item_edited", callable_mp(this, &SceneTreeSelector::_on_item_edited));
}

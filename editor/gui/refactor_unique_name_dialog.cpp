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

void RefactorUniqueNameDialog::_bind_methods() {
}

RefactorUniqueNameDialog::RefactorUniqueNameDialog(Node *p_root) {
	scene_root = p_root;

	set_ok_button_text(TTRC("Rename in scripts"));

	label = memnew(Label);
	label->set_focus_mode(Control::FOCUS_ACCESSIBILITY);
	add_child(label);
}

void RefactorUniqueNameDialog::add_refactor(const StringName &p_old_name, const StringName &p_new_name) {
	refactor_queue.push_back({ p_old_name, p_new_name });
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
	popup_centered();
}

void RefactorUniqueNameDialog::ok_pressed() {
	if (!refactor_queue.is_empty()) {
		const RefactorData &refactor_data = refactor_queue[0];
		//_refactor_unique_name(refactor_data, scene_tree_editor->get_scene_tree());
		refactor_queue.remove_at(0);
	}
	callable_mp(this, &RefactorUniqueNameDialog::_next).call_deferred();
}

void RefactorUniqueNameDialog::cancel_pressed() {
	if (!refactor_queue.is_empty()) {
		refactor_queue.remove_at(0);
	}
	callable_mp(this, &RefactorUniqueNameDialog::_next).call_deferred();
}

void RefactorUniqueNameDialog::_refactor_unique_name(RefactorData refactor_data, const TypedArray<NodePath> &p_paths) {
	for (int i = 0; i < p_paths.size(); i++) {
		Node *n = scene_root->get_node(p_paths[i]);
		if (!n) {
			continue;
		}

		if (!n->get_script().is_null()) {
			//TODO
		}
	}
}

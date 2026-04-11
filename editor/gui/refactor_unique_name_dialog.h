/**************************************************************************/
/*  refactor_unique_name_dialog.h                                         */
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

#pragma once

#include "editor/scene/scene_tree_editor.h"

class SceneTreeSelector : public Control {
	GDCLASS(SceneTreeSelector, Control);

	Tree *scene_tree = nullptr;
	Node *scene_root = nullptr;
	AHashMap<ObjectID, ObjectID> item_node_map;
	AHashMap<ObjectID, ObjectID> node_item_map;

	Vector<ObjectID> selected_nodes;
	Vector<ObjectID> selectable_nodes;

	void _add_item(Node *p_parent, Node *p_node, int p_index);

	void _on_item_edited();

	void _reset();
	void _update_subtree(Node *p_node);

public:
	void clear();
	void create(Node *p_root, Vector<ObjectID> p_selectable_nodes);
	Vector<Node *> get_selected_nodes() const;
	SceneTreeSelector();
};

class RefactorUniqueNameDialog : public ConfirmationDialog {
	GDCLASS(RefactorUniqueNameDialog, ConfirmationDialog);

	struct RefactorData {
		Vector<ObjectID> nodes_to_consider;
		StringName old_name;
		StringName new_name;
	};

	Vector<RefactorData> refactor_queue;
	Label *label = nullptr;

	SceneTreeSelector *scene_tree_selector = nullptr;

	Node *get_scene_root() const;

	void _next();
	void _refactor_unique_name(RefactorData p_refactor_data);
	void _notification(int p_what);
	Vector<ObjectID> _get_nodes_to_refactor(const StringName &p_old_name);

public:
	void popup_refactor();

	virtual void cancel_pressed() override;
	virtual void ok_pressed() override;

	void add_refactor(const StringName &p_old_name, const StringName &p_new_name);

	RefactorUniqueNameDialog();
};

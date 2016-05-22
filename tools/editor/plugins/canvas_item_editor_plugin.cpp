/*************************************************************************/
/*  canvas_item_editor_plugin.cpp                                        */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
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
#include "canvas_item_editor_plugin.h"
#include "print_string.h"
#include "tools/editor/editor_node.h"
#include "os/keyboard.h"
#include "scene/main/viewport.h"
#include "scene/main/canvas_layer.h"
#include "scene/2d/node_2d.h"
#include "globals.h"
#include "os/input.h"
#include "tools/editor/editor_settings.h"
#include "scene/gui/grid_container.h"
#include "tools/editor/animation_editor.h"
#include "tools/editor/plugins/animation_player_editor_plugin.h"
#include "scene/resources/packed_scene.h"

class SnapDialog : public ConfirmationDialog {

	OBJ_TYPE(SnapDialog,ConfirmationDialog);

friend class CanvasItemEditor;

	SpinBox *grid_offset_x;
	SpinBox *grid_offset_y;
	SpinBox *grid_step_x;
	SpinBox *grid_step_y;
	SpinBox *rotation_offset;
	SpinBox *rotation_step;

public:
	SnapDialog() : ConfirmationDialog() {
		const int SPIN_BOX_GRID_RANGE = 256;
		const int SPIN_BOX_ROTATION_RANGE = 360;
		Label *label;
		VBoxContainer *container;
		GridContainer *child_container;

		set_title(TTR("Configure Snap"));
		get_ok()->set_text(TTR("Close"));

		container = memnew( VBoxContainer );
		add_child(container);
		set_child_rect(container);

		child_container = memnew( GridContainer );
		child_container->set_columns(3);
		container->add_child(child_container);

		label = memnew( Label );
		label->set_text(TTR("Grid Offset:"));
		child_container->add_child(label);
		label->set_h_size_flags(SIZE_EXPAND_FILL);

		grid_offset_x = memnew( SpinBox );
		grid_offset_x->set_min(-SPIN_BOX_GRID_RANGE);
		grid_offset_x->set_max(SPIN_BOX_GRID_RANGE);
		grid_offset_x->set_suffix("px");
		child_container->add_child(grid_offset_x);

		grid_offset_y = memnew( SpinBox );
		grid_offset_y->set_min(-SPIN_BOX_GRID_RANGE);
		grid_offset_y->set_max(SPIN_BOX_GRID_RANGE);
		grid_offset_y->set_suffix("px");
		child_container->add_child(grid_offset_y);

		label = memnew( Label );
		label->set_text(TTR("Grid Step:"));
		child_container->add_child(label);
		label->set_h_size_flags(SIZE_EXPAND_FILL);

		grid_step_x = memnew( SpinBox );
		grid_step_x->set_min(-SPIN_BOX_GRID_RANGE);
		grid_step_x->set_max(SPIN_BOX_GRID_RANGE);
		grid_step_x->set_suffix("px");
		child_container->add_child(grid_step_x);

		grid_step_y = memnew( SpinBox );
		grid_step_y->set_min(-SPIN_BOX_GRID_RANGE);
		grid_step_y->set_max(SPIN_BOX_GRID_RANGE);
		grid_step_y->set_suffix("px");
		child_container->add_child(grid_step_y);

		container->add_child( memnew( HSeparator ) );

		child_container = memnew( GridContainer );
		child_container->set_columns(2);
		container->add_child(child_container);

		label = memnew( Label );
		label->set_text(TTR("Rotation Offset:"));
		child_container->add_child(label);
		label->set_h_size_flags(SIZE_EXPAND_FILL);

		rotation_offset = memnew( SpinBox );
		rotation_offset->set_min(-SPIN_BOX_ROTATION_RANGE);
		rotation_offset->set_max(SPIN_BOX_ROTATION_RANGE);
		rotation_offset->set_suffix("deg");
		child_container->add_child(rotation_offset);

		label = memnew( Label );
		label->set_text(TTR("Rotation Step:"));
		child_container->add_child(label);
		label->set_h_size_flags(SIZE_EXPAND_FILL);

		rotation_step = memnew( SpinBox );
		rotation_step->set_min(-SPIN_BOX_ROTATION_RANGE);
		rotation_step->set_max(SPIN_BOX_ROTATION_RANGE);
		rotation_step->set_suffix("deg");
		child_container->add_child(rotation_step);
	}

	void set_fields(const Point2 p_grid_offset, const Size2 p_grid_step, const float p_rotation_offset, const float p_rotation_step) {
		grid_offset_x->set_val(p_grid_offset.x);
		grid_offset_y->set_val(p_grid_offset.y);
		grid_step_x->set_val(p_grid_step.x);
		grid_step_y->set_val(p_grid_step.y);
		rotation_offset->set_val(p_rotation_offset * (180 / Math_PI));
		rotation_step->set_val(p_rotation_step * (180 / Math_PI));
	}

	void get_fields(Point2 &p_grid_offset, Size2 &p_grid_step, float &p_rotation_offset, float &p_rotation_step) {
		p_grid_offset.x = grid_offset_x->get_val();
		p_grid_offset.y = grid_offset_y->get_val();
		p_grid_step.x = grid_step_x->get_val();
		p_grid_step.y = grid_step_y->get_val();
		p_rotation_offset = rotation_offset->get_val() / (180 / Math_PI);
		p_rotation_step = rotation_step->get_val() / (180 / Math_PI);
	}
};

void CanvasItemEditor::_edit_set_pivot(const Vector2& mouse_pos) {
	List<Node*> &selection = editor_selection->get_selected_node_list();

	undo_redo->create_action(TTR("Move Pivot"));

	for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

		Node2D *n2d = E->get()->cast_to<Node2D>();

		if (n2d && n2d->edit_has_pivot()) {

			Vector2 offset = n2d->edit_get_pivot();
			Vector2 gpos = n2d->get_global_pos();

			Vector2 local_mouse_pos = n2d->get_canvas_transform().affine_inverse().xform(mouse_pos);

			Vector2 motion_ofs = gpos-local_mouse_pos;

			undo_redo->add_do_method(n2d,"set_global_pos",local_mouse_pos);
			undo_redo->add_do_method(n2d,"edit_set_pivot",offset+n2d->get_global_transform().affine_inverse().basis_xform(motion_ofs));
			undo_redo->add_undo_method(n2d,"set_global_pos",gpos);
			undo_redo->add_undo_method(n2d,"edit_set_pivot",offset);
			for(int i=0;i<n2d->get_child_count();i++) {
				Node2D *n2dc = n2d->get_child(i)->cast_to<Node2D>();
				if (!n2dc)
					continue;

				undo_redo->add_do_method(n2dc,"set_global_pos",n2dc->get_global_pos());
				undo_redo->add_undo_method(n2dc,"set_global_pos",n2dc->get_global_pos());

			}

		}

	}

	undo_redo->commit_action();

}

void CanvasItemEditor::_unhandled_key_input(const InputEvent& p_ev) {

	if (!is_visible() || get_viewport()->gui_has_modal_stack())
		return;
	if (p_ev.key.mod.control)
		// prevent to change tool mode when control key is pressed
		return;
	if (p_ev.key.pressed && !p_ev.key.echo && p_ev.key.scancode==KEY_Q)
		_tool_select(TOOL_SELECT);
	if (p_ev.key.pressed && !p_ev.key.echo && p_ev.key.scancode==KEY_W)
		_tool_select(TOOL_MOVE);
	if (p_ev.key.pressed && !p_ev.key.echo && p_ev.key.scancode==KEY_E)
		_tool_select(TOOL_ROTATE);
	if (p_ev.key.pressed && !p_ev.key.echo && p_ev.key.scancode==KEY_V && drag==DRAG_NONE && can_move_pivot) {
		if (p_ev.key.mod.shift) {
			//move drag pivot
			drag=DRAG_PIVOT;
		} else if (!Input::get_singleton()->is_mouse_button_pressed(0)) {

			List<Node*> &selection = editor_selection->get_selected_node_list();

			Vector2 mouse_pos = viewport->get_local_mouse_pos();
			if (selection.size() && viewport->get_rect().has_point(mouse_pos)) {
				//just in case, make it work if over viewport
				mouse_pos=transform.affine_inverse().xform(mouse_pos);
				mouse_pos=snap_point(mouse_pos);

				_edit_set_pivot(mouse_pos);
			}

		}
	}

}

void CanvasItemEditor::_tool_select(int p_index) {


	ToolButton *tb[TOOL_MAX]={select_button,list_select_button,move_button,rotate_button,pivot_button,pan_button};
	for(int i=0;i<TOOL_MAX;i++) {

		tb[i]->set_pressed(i==p_index);
	}


	viewport->update();
	tool=(Tool)p_index;

}

Object *CanvasItemEditor::_get_editor_data(Object *p_what) {

	CanvasItem *ci = p_what->cast_to<CanvasItem>();
	if (!ci)
		return NULL;

	return memnew( CanvasItemEditorSelectedItem );
}

inline float _snap_scalar(float p_offset, float p_step, bool p_snap_relative, float p_target, float p_start) {
	float offset = p_snap_relative ? p_start : p_offset;
	return p_step != 0 ? Math::stepify(p_target - offset, p_step) + offset : p_target;
}

Vector2 CanvasItemEditor::snap_point(Vector2 p_target, Vector2 p_start) const {
	if (snap_grid) {
		p_target.x = _snap_scalar(snap_offset.x, snap_step.x, snap_relative, p_target.x, p_start.x);
		p_target.y = _snap_scalar(snap_offset.y, snap_step.y, snap_relative, p_target.y, p_start.y);
	}
	if (snap_pixel)
		p_target = p_target.snapped(Size2(1, 1));

	return p_target;
}

float CanvasItemEditor::snap_angle(float p_target, float p_start) const {
	return snap_rotation ? _snap_scalar(snap_rotation_offset, snap_rotation_step, snap_relative, p_target, p_start) : p_target;
}

Dictionary CanvasItemEditor::get_state() const {

	Dictionary state;
	state["zoom"]=zoom;
	state["ofs"]=Point2(h_scroll->get_val(),v_scroll->get_val());
//	state["ofs"]=-transform.get_origin();
	state["snap_offset"]=snap_offset;
	state["snap_step"]=snap_step;
	state["snap_rotation_offset"]=snap_rotation_offset;
	state["snap_rotation_step"]=snap_rotation_step;
	state["snap_grid"]=snap_grid;
	state["snap_show_grid"]=snap_show_grid;
	state["snap_rotation"]=snap_rotation;
	state["snap_relative"]=snap_relative;
	state["snap_pixel"]=snap_pixel;
	return state;
}
void CanvasItemEditor::set_state(const Dictionary& p_state){

	Dictionary state=p_state;
	if (state.has("zoom")) {
		zoom=p_state["zoom"];
	}

	if (state.has("ofs")) {
		_update_scrollbars(); // i wonder how safe is calling this here..
		Point2 ofs=p_state["ofs"];
		h_scroll->set_val(ofs.x);
		v_scroll->set_val(ofs.y);
	}

	if (state.has("snap_step")) {
		snap_step=state["snap_step"];
	}

	if (state.has("snap_offset")) {
		snap_offset=state["snap_offset"];
	}

	if (state.has("snap_rotation_step")) {
		snap_rotation_step=state["snap_rotation_step"];
	}

	if (state.has("snap_rotation_offset")) {
		snap_rotation_offset=state["snap_rotation_offset"];
	}

	if (state.has("snap_grid")) {
		snap_grid=state["snap_grid"];
		int idx = edit_menu->get_popup()->get_item_index(SNAP_USE);
		edit_menu->get_popup()->set_item_checked(idx,snap_grid);
	}

	if (state.has("snap_show_grid")) {
		snap_show_grid=state["snap_show_grid"];
		int idx = edit_menu->get_popup()->get_item_index(SNAP_SHOW_GRID);
		edit_menu->get_popup()->set_item_checked(idx,snap_show_grid);
	}

	if (state.has("snap_rotation")) {
		snap_rotation=state["snap_rotation"];
		int idx = edit_menu->get_popup()->get_item_index(SNAP_USE_ROTATION);
		edit_menu->get_popup()->set_item_checked(idx,snap_rotation);
	}

	if (state.has("snap_relative")) {
		snap_relative=state["snap_relative"];
		int idx = edit_menu->get_popup()->get_item_index(SNAP_RELATIVE);
		edit_menu->get_popup()->set_item_checked(idx,snap_relative);
	}

	if (state.has("snap_pixel")) {
		snap_pixel=state["snap_pixel"];
		int idx = edit_menu->get_popup()->get_item_index(SNAP_USE_PIXEL);
		edit_menu->get_popup()->set_item_checked(idx,snap_pixel);
	}
}


void CanvasItemEditor::_add_canvas_item(CanvasItem *p_canvas_item) {

	editor_selection->add_node(p_canvas_item);
#if 0
	if (canvas_items.has(p_canvas_item))
		return;

	canvas_items.insert(p_canvas_item,p_info);
	p_canvas_item->connect("hide",this,"_visibility_changed",varray(p_canvas_item->get_instance_ID()),CONNECT_ONESHOT);
#endif
}

void CanvasItemEditor::_remove_canvas_item(CanvasItem *p_canvas_item) {

	editor_selection->remove_node(p_canvas_item);
#if 0
	p_canvas_item->disconnect("hide",this,"_visibility_changed");
	canvas_items.erase(p_canvas_item);
#endif

}
void CanvasItemEditor::_clear_canvas_items() {

	editor_selection->clear();;
#if 0
	while(canvas_items.size())
		_remove_canvas_item(canvas_items.front()->key());
#endif
}

void CanvasItemEditor::_visibility_changed(ObjectID p_canvas_item) {
#if 0
	Object *c = ObjectDB::get_instance(p_canvas_item);
	if (!c)
		return;
	CanvasItem *ct = c->cast_to<CanvasItem>();
	if (!ct)
		return;
	canvas_items.erase(ct);
	//_remove_canvas_item(ct);
	update();
#endif
}


void CanvasItemEditor::_node_removed(Node *p_node) {
#if 0
	CanvasItem *canvas_item = (CanvasItem*)p_node; //not a good cast, but safe
	if (canvas_items.has(canvas_item))
		_remove_canvas_item(canvas_item);

	update();
#endif
}

void CanvasItemEditor::_keying_changed() {

	if (AnimationPlayerEditor::singleton->get_key_editor()->is_visible())
		animation_hb->show();
	else
		animation_hb->hide();
}

bool CanvasItemEditor::_is_part_of_subscene(CanvasItem *p_item) {

	Node* scene_node = get_tree()->get_edited_scene_root();
	Node* item_owner = p_item->get_owner();

	return item_owner && item_owner!=scene_node && p_item!=scene_node && item_owner->get_filename()!="";
}

// slow but modern computers should have no problem
CanvasItem* CanvasItemEditor::_select_canvas_item_at_pos(const Point2& p_pos,Node* p_node,const Matrix32& p_parent_xform,const Matrix32& p_canvas_xform) {

	if (!p_node)
		return NULL;
	if (p_node->cast_to<Viewport>())
		return NULL;

	CanvasItem *c=p_node->cast_to<CanvasItem>();


	for (int i=p_node->get_child_count()-1;i>=0;i--) {

		CanvasItem *r=NULL;

		if (c && !c->is_set_as_toplevel())
			r=_select_canvas_item_at_pos(p_pos,p_node->get_child(i),p_parent_xform * c->get_transform(),p_canvas_xform);
		else {
			CanvasLayer *cl = p_node->cast_to<CanvasLayer>();
			r=_select_canvas_item_at_pos(p_pos,p_node->get_child(i),transform ,cl ? cl->get_transform() : p_canvas_xform); //use base transform
		}

		if (r)
			return r;
	}

	if (c && c->is_visible() && !c->has_meta("_edit_lock_") && !_is_part_of_subscene(c) && !c->cast_to<CanvasLayer>()) {

		Rect2 rect = c->get_item_rect();
		Point2 local_pos = (p_parent_xform * p_canvas_xform * c->get_transform()).affine_inverse().xform(p_pos);


		if (rect.has_point(local_pos))
			return c;

	}

	return NULL;
}

void CanvasItemEditor::_find_canvas_items_at_pos(const Point2 &p_pos,Node* p_node,const Matrix32& p_parent_xform,const Matrix32& p_canvas_xform, Vector<_SelectResult> &r_items) {
	if (!p_node)
		return;
	if (p_node->cast_to<Viewport>())
		return;

	CanvasItem *c=p_node->cast_to<CanvasItem>();

	for (int i=p_node->get_child_count()-1;i>=0;i--) {

		if (c && !c->is_set_as_toplevel())
			_find_canvas_items_at_pos(p_pos,p_node->get_child(i),p_parent_xform * c->get_transform(),p_canvas_xform, r_items);
		else {
			CanvasLayer *cl = p_node->cast_to<CanvasLayer>();
			_find_canvas_items_at_pos(p_pos,p_node->get_child(i),transform ,cl ? cl->get_transform() : p_canvas_xform, r_items); //use base transform
		}
	}


	if (c && c->is_visible() && !c->has_meta("_edit_lock_") && !c->cast_to<CanvasLayer>()) {

		Rect2 rect = c->get_item_rect();
		Point2 local_pos = (p_parent_xform * p_canvas_xform * c->get_transform()).affine_inverse().xform(p_pos);


		if (rect.has_point(local_pos)) {
			Node2D *node=c->cast_to<Node2D>();

			_SelectResult res;
			res.item=c;
			res.z=node?node->get_z():0;
			res.has_z=node;
			r_items.push_back(res);
		}

	}

	return;
}

void CanvasItemEditor::_find_canvas_items_at_rect(const Rect2& p_rect,Node* p_node,const Matrix32& p_parent_xform,const Matrix32& p_canvas_xform,List<CanvasItem*> *r_items) {

	if (!p_node)
		return;
	if (p_node->cast_to<Viewport>())
		return;

	CanvasItem *c=p_node->cast_to<CanvasItem>();


	for (int i=p_node->get_child_count()-1;i>=0;i--) {

		if (c && !c->is_set_as_toplevel())
			_find_canvas_items_at_rect(p_rect,p_node->get_child(i),p_parent_xform * c->get_transform(),p_canvas_xform,r_items);
		else {
			CanvasLayer *cl = p_node->cast_to<CanvasLayer>();
			_find_canvas_items_at_rect(p_rect,p_node->get_child(i),transform,cl?cl->get_transform():p_canvas_xform,r_items);
		}
	}


	if (c && c->is_visible() && !c->has_meta("_edit_lock_") && !c->cast_to<CanvasLayer>()) {

		Rect2 rect = c->get_item_rect();
		Matrix32 xform = p_parent_xform * p_canvas_xform * c->get_transform();

		if ( p_rect.has_point( xform.xform( rect.pos ) ) &&
		     p_rect.has_point( xform.xform( rect.pos+Vector2(rect.size.x,0) ) ) &&
		     p_rect.has_point( xform.xform( rect.pos+Vector2(rect.size.x,rect.size.y) ) ) &&
		     p_rect.has_point( xform.xform( rect.pos+Vector2(0,rect.size.y) ) ) ) {

			r_items->push_back(c);

		}
	}


}

bool CanvasItemEditor::_select(CanvasItem *item, Point2 p_click_pos, bool p_append, bool p_drag) {

	if (p_append) {
		//additive selection

		if (!item) {

			if (p_drag) {
				drag_from=transform.affine_inverse().xform(p_click_pos);

				box_selecting=true;
				box_selecting_to=drag_from;
			}

			return false; //nothing to add
		}

		if (editor_selection->is_selected(item)) {
			//already in here, erase it
			editor_selection->remove_node(item);
			//_remove_canvas_item(c);

			viewport->update();
			return false;

		}
		_append_canvas_item(item);
		viewport->update();

		return true;

	} else {
		//regular selection

		if (!item) {
			//clear because nothing clicked
			editor_selection->clear();;

			if (p_drag) {
				drag_from=transform.affine_inverse().xform(p_click_pos);

				box_selecting=true;
				box_selecting_to=drag_from;
			}

			viewport->update();
			return false;
		}

		if (!editor_selection->is_selected(item)) {
			//select a new one and clear previous selection
			editor_selection->clear();
			editor_selection->add_node(item);
			//reselect
			if (get_tree()->is_editor_hint()) {
				editor->call("edit_node",item);
			}

		}

		if (p_drag) {
			//prepare to move!

			List<Node*> &selection = editor_selection->get_selected_node_list();

			for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;
				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;

				CanvasItemEditorSelectedItem *se=editor_selection->get_node_editor_data<CanvasItemEditorSelectedItem>(canvas_item);
				if (!se)
					continue;

				se->undo_state=canvas_item->edit_get_state();
				if (canvas_item->cast_to<Node2D>())
					se->undo_pivot=canvas_item->cast_to<Node2D>()->edit_get_pivot();

			}

			drag=DRAG_ALL;
			drag_from=transform.affine_inverse().xform(p_click_pos);
			drag_point_from=_find_topleftmost_point();
		}

		viewport->update();

		return true;

	}
}

void CanvasItemEditor::_key_move(const Vector2& p_dir, bool p_snap, KeyMoveMODE p_move_mode) {


	if (drag!=DRAG_NONE)
		return;

	if (editor_selection->get_selected_node_list().empty())
		return;

	undo_redo->create_action(TTR("Move Action"),true);

	List<Node*> &selection = editor_selection->get_selected_node_list();

	for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

		CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
		if (!canvas_item || !canvas_item->is_visible())
			continue;
		if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
			continue;

		CanvasItemEditorSelectedItem *se=editor_selection->get_node_editor_data<CanvasItemEditorSelectedItem>(canvas_item);
		if (!se)
			continue;

		if (canvas_item->has_meta("_edit_lock_"))
			continue;


		Vector2 drag = p_dir;
		if (p_snap)
			drag*=snap_step;

		undo_redo->add_undo_method(canvas_item,"edit_set_state",canvas_item->edit_get_state());

		if (p_move_mode  == MOVE_VIEW_BASE) {

			// drag =  transform.affine_inverse().basis_xform(p_dir); // zoom sensitive
			drag = canvas_item->get_global_transform_with_canvas().affine_inverse().basis_xform(drag);
			Rect2 local_rect = canvas_item->get_item_rect();
			local_rect.pos+=drag;
			undo_redo->add_do_method(canvas_item,"edit_set_rect",local_rect);

		} else { // p_move_mode==MOVE_LOCAL_BASE || p_move_mode==MOVE_LOCAL_WITH_ROT

			if (Node2D *node_2d = canvas_item->cast_to<Node2D>()) {

				if (p_move_mode == MOVE_LOCAL_WITH_ROT) {
					Matrix32 m;
					m.rotate( node_2d->get_rot() );
					drag = m.xform(drag);
				}
				node_2d->set_pos(node_2d->get_pos() + drag);

			} else if (Control *control = canvas_item->cast_to<Control>()) {

				control->set_pos(control->get_pos()+drag);
			}
		}
	}

	undo_redo->commit_action();
}

Point2 CanvasItemEditor::_find_topleftmost_point() {



	Vector2 tl=Point2(1e10,1e10);
	Rect2 r2;
	r2.pos=tl;


	List<Node*> &selection = editor_selection->get_selected_node_list();

	for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

		CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
		if (!canvas_item || !canvas_item->is_visible())
			continue;
		if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
			continue;




		Rect2 rect=canvas_item->get_item_rect();
		Matrix32 xform=canvas_item->get_global_transform_with_canvas();

		r2.expand_to(xform.xform(rect.pos));
		r2.expand_to(xform.xform(rect.pos+Vector2(rect.size.x,0)));
		r2.expand_to(xform.xform(rect.pos+rect.size));
		r2.expand_to(xform.xform(rect.pos+Vector2(0,rect.size.y)));

	}

	return r2.pos;
}



int CanvasItemEditor::get_item_count() {

	List<Node*> &selection = editor_selection->get_selected_node_list();

	int ic=0;
	for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

		CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
		if (!canvas_item || !canvas_item->is_visible())
			continue;

		if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
			continue;

		ic++;
	};

	return ic;
}

CanvasItem *CanvasItemEditor::get_single_item() {


	Map<Node*,Object*> &selection = editor_selection->get_selection();

	CanvasItem *single_item=NULL;

	for(Map<Node*,Object*>::Element *E=selection.front();E;E=E->next()) {

		CanvasItem *canvas_item = E->key()->cast_to<CanvasItem>();
		if (!canvas_item || !canvas_item->is_visible())
			continue;
		if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
			continue;

		if (single_item)
			return NULL; //morethan one

		single_item=canvas_item;
	};

	return single_item;
}

CanvasItemEditor::DragType CanvasItemEditor::_find_drag_type(const Matrix32& p_xform, const Rect2& p_local_rect, const Point2& p_click, Vector2& r_point) {

	CanvasItem *canvas_item = get_single_item();

	ERR_FAIL_COND_V(!canvas_item,DRAG_NONE);

	Rect2 rect=canvas_item->get_item_rect();
	Matrix32 xforml=canvas_item->get_global_transform_with_canvas();
	Matrix32 xform=transform * xforml;

	Vector2 endpoints[4]={

		xform.xform(rect.pos),
		xform.xform(rect.pos+Vector2(rect.size.x,0)),
		xform.xform(rect.pos+rect.size),
		xform.xform(rect.pos+Vector2(0,rect.size.y))
	};

	Vector2 endpointsl[4]={

		xforml.xform(rect.pos),
		xforml.xform(rect.pos+Vector2(rect.size.x,0)),
		xforml.xform(rect.pos+rect.size),
		xforml.xform(rect.pos+Vector2(0,rect.size.y))
	};

	DragType dragger[]={
		DRAG_TOP_LEFT,
		DRAG_TOP,
		DRAG_TOP_RIGHT,
		DRAG_RIGHT,
		DRAG_BOTTOM_RIGHT,
		DRAG_BOTTOM,
		DRAG_BOTTOM_LEFT,
		DRAG_LEFT
	};

	float radius = (select_handle->get_size().width/2)*1.5;

	//try draggers

	for(int i=0;i<4;i++) {

		int prev = (i+3)%4;
		int next = (i+1)%4;

		r_point=endpointsl[i];

		Vector2 ofs = ((endpoints[i] - endpoints[prev]).normalized() + ((endpoints[i] - endpoints[next]).normalized())).normalized();
		ofs*=1.4144*(select_handle->get_size().width/2);

		ofs+=endpoints[i];

		if (ofs.distance_to(p_click)<radius)
			return dragger[i*2];

		ofs = (endpoints[i]+endpoints[next])/2;
		ofs += (endpoints[next]-endpoints[i]).tangent().normalized()*(select_handle->get_size().width/2);

		r_point=(endpointsl[i]+endpointsl[next])/2;


		if (ofs.distance_to(p_click)<radius)
			return dragger[i*2+1];

	}

	/*
	if (rect.has_point(xform.affine_inverse().xform(p_click))) {
		r_point=_find_topleftmost_point();
		return DRAG_ALL;
	}*/

	//try draggers

	return DRAG_NONE;
}

void CanvasItemEditor::incbeg(float& beg,float &end, float inc, float minsize,bool p_symmetric) {

	if (minsize<0) {

		beg+=inc;
		if (p_symmetric)
			end-=inc;
	} else {

		if (p_symmetric) {
			beg+=inc;
			end-=inc;
			if (end-beg < minsize) {
				float center = (beg+end)/2.0;
				beg=center-minsize/2.0;
				end=center+minsize/2.0;
			}

		} else {
			if (end-(beg+inc) < minsize)
				beg=end-minsize;
			else
				beg+=inc;
		}

	}
}

void CanvasItemEditor::incend(float &beg,float& end, float inc, float minsize,bool p_symmetric) {

	if (minsize<0) {

		end+=inc;
		if (p_symmetric)
			beg-=inc;
	} else {

		if (p_symmetric) {

			end+=inc;
			beg-=inc;
			if (end-beg < minsize) {
				float center = (beg+end)/2.0;
				beg=center-minsize/2.0;
				end=center+minsize/2.0;
			}

		} else {
			if ((end+inc)-beg < minsize)
				end=beg+minsize;
			else
				end+=inc;
		}

	}
}

void CanvasItemEditor::_append_canvas_item(CanvasItem *c) {

	editor_selection->add_node(c);

}


void CanvasItemEditor::_snap_changed() {
	((SnapDialog *)snap_dialog)->get_fields(snap_offset, snap_step, snap_rotation_offset, snap_rotation_step);
	viewport->update();
}

void CanvasItemEditor::_dialog_value_changed(double) {

	if (updating_value_dialog)
		return;

	switch(last_option) {

		case ZOOM_SET: {

			zoom=dialog_val->get_val()/100.0;
			_update_scroll(0);
			viewport->update();

		} break;
		default:{}
	}
}

void CanvasItemEditor::_selection_result_pressed(int p_result) {

	if (selection_results.size() <= p_result)
		return;

	CanvasItem *item=selection_results[p_result].item;

	if (item)
		_select(item, Point2(), additive_selection, false);
}

void CanvasItemEditor::_selection_menu_hide() {

	selection_results.clear();
	selection_menu->clear();
	selection_menu->set_size(Vector2(0, 0));
}

bool CanvasItemEditor::get_remove_list(List<Node*> *p_list) {


	return false;//!p_list->empty();
}


void CanvasItemEditor::_list_select(const InputEventMouseButton& b) {

	Point2 click=Point2(b.x,b.y);

	Node* scene = editor->get_edited_scene();
	if (!scene)
		return;

	_find_canvas_items_at_pos(click, scene,transform,Matrix32(), selection_results);

	for(int i=0;i<selection_results.size();i++) {
		CanvasItem *item=selection_results[i].item;
		if (item!=scene && item->get_owner()!=scene && !scene->is_editable_instance(item->get_owner())) {
			//invalid result
			selection_results.remove(i);
			i--;
		}

	}

	if (selection_results.size() == 1) {

		CanvasItem *item = selection_results[0].item;
		selection_results.clear();

		additive_selection=b.mod.shift;
		if (!_select(item, click, additive_selection, false))
			return;

	} else if (!selection_results.empty()) {

		selection_results.sort();

		NodePath root_path = get_tree()->get_edited_scene_root()->get_path();
		StringName root_name = root_path.get_name(root_path.get_name_count()-1);

		for (int i = 0; i < selection_results.size(); i++) {

			CanvasItem *item=selection_results[i].item;


			Ref<Texture> icon;
			if (item->has_meta("_editor_icon"))
				icon=item->get_meta("_editor_icon");
			else
				icon=get_icon( has_icon(item->get_type(),"EditorIcons")?item->get_type():String("Object"),"EditorIcons");

			String node_path="/"+root_name+"/"+root_path.rel_path_to(item->get_path());

			selection_menu->add_item(item->get_name());
			selection_menu->set_item_icon(i, icon );
			selection_menu->set_item_metadata(i, node_path);
			selection_menu->set_item_tooltip(i,String(item->get_name())+
					"\nType: "+item->get_type()+"\nPath: "+node_path);
		}

		additive_selection=b.mod.shift;

		selection_menu->set_global_pos(Vector2( b.global_x, b.global_y ));
		selection_menu->popup();
		selection_menu->call_deferred("grab_click_focus");
		selection_menu->set_invalidate_click_until_motion();


		return;
	}

}

void CanvasItemEditor::_viewport_input_event(const InputEvent& p_event) {

	 {

		EditorNode *en = editor;
		EditorPluginList *over_plugin_list = en->get_editor_plugins_over();

		if (!over_plugin_list->empty()) {
			bool discard = over_plugin_list->forward_input_event(p_event);
			if (discard) {
				accept_event();
				return;
			}
		}
	}


	if (p_event.type==InputEvent::MOUSE_BUTTON) {

		const InputEventMouseButton &b=p_event.mouse_button;


		if (b.button_index==BUTTON_WHEEL_DOWN) {

			float prev_zoom=zoom;
			zoom=zoom*0.95;
			{
				Point2 ofs(b.x,b.y);
				ofs = ofs/prev_zoom - ofs/zoom;
				h_scroll->set_val( h_scroll->get_val() + ofs.x );
				v_scroll->set_val( v_scroll->get_val() + ofs.y );
			}
			_update_scroll(0);
			viewport->update();
			return;
		}

		if (b.button_index==BUTTON_WHEEL_UP) {

			float prev_zoom=zoom;
			zoom=zoom*(1.0/0.95);
			{
				Point2 ofs(b.x,b.y);
				ofs = ofs/prev_zoom - ofs/zoom;
				h_scroll->set_val( h_scroll->get_val() + ofs.x );
				v_scroll->set_val( v_scroll->get_val() + ofs.y );
			}

			_update_scroll(0);
			viewport->update();
			return;
		}

		if (b.button_index==BUTTON_RIGHT) {


			if (b.pressed && (tool==TOOL_SELECT && b.mod.alt)) {

				_list_select(b);
				return;
			}

			if (get_item_count() > 0 && drag!=DRAG_NONE) {
				//cancel drag

				if (bone_ik_list.size()) {

					for(List<BoneIK>::Element *E=bone_ik_list.back();E;E=E->prev()) {

						E->get().node->edit_set_state(E->get().orig_state);
					}

					bone_ik_list.clear();

				} else {


					List<Node*> &selection = editor_selection->get_selected_node_list();

					for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

						CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
						if (!canvas_item || !canvas_item->is_visible())
							continue;
						if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
							continue;


						CanvasItemEditorSelectedItem *se=editor_selection->get_node_editor_data<CanvasItemEditorSelectedItem>(canvas_item);
						if (!se)
							continue;

						canvas_item->edit_set_state(se->undo_state);
						if (canvas_item->cast_to<Node2D>())
							canvas_item->cast_to<Node2D>()->edit_set_pivot(se->undo_pivot);

					}
				}

				drag=DRAG_NONE;
				viewport->update();
				can_move_pivot=false;

			} else if (box_selecting) {
				box_selecting=false;
				viewport->update();
			} else if (b.pressed) {
#if 0
				ref_item = NULL;
				Node* scene = get_scene()->get_root_node()->cast_to<EditorNode>()->get_edited_scene();
				if ( scene ) ref_item =_select_canvas_item_at_pos( Point2( b.x, b.y ), scene, transform );
#endif
				//popup->set_pos(Point2(b.x,b.y));
				//popup->popup();
			}
			return;
		}
		//if (!canvas_items.size())
		//	return;

		if (b.button_index==BUTTON_LEFT && tool==TOOL_LIST_SELECT) {
			if (b.pressed)
				_list_select(b);
			return;
		}


		if (b.button_index==BUTTON_LEFT && tool==TOOL_EDIT_PIVOT) {
			if (b.pressed) {

				Point2 mouse_pos(b.x,b.y);
				mouse_pos=transform.affine_inverse().xform(mouse_pos);
				mouse_pos=snap_point(mouse_pos);
				_edit_set_pivot(mouse_pos);
			}
			return;
		}



		if (tool==TOOL_PAN || b.button_index!=BUTTON_LEFT || Input::get_singleton()->is_key_pressed(KEY_SPACE))
			return;

		if (!b.pressed) {

			if (drag!=DRAG_NONE) {

				if (undo_redo) {


					if (bone_ik_list.size()) {


						undo_redo->create_action(TTR("Edit IK Chain"));

						for(List<BoneIK>::Element *E=bone_ik_list.back();E;E=E->prev()) {

							undo_redo->add_do_method(E->get().node,"edit_set_state",E->get().node->edit_get_state());
							undo_redo->add_undo_method(E->get().node,"edit_set_state",E->get().orig_state);
						}

						undo_redo->add_do_method(viewport,"update");
						undo_redo->add_undo_method(viewport,"update");

						bone_ik_list.clear();

						undo_redo->commit_action();
					} else {

						undo_redo->create_action(TTR("Edit CanvasItem"));


						List<Node*> &selection = editor_selection->get_selected_node_list();

						for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

							CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
							if (!canvas_item || !canvas_item->is_visible())
								continue;
							if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
								continue;

							CanvasItemEditorSelectedItem *se=editor_selection->get_node_editor_data<CanvasItemEditorSelectedItem>(canvas_item);
							if (!se)
								continue;

							Variant state=canvas_item->edit_get_state();
							undo_redo->add_do_method(canvas_item,"edit_set_state",state);
							undo_redo->add_undo_method(canvas_item,"edit_set_state",se->undo_state);
							if (canvas_item->cast_to<Node2D>()) {
								Node2D *pvt = canvas_item->cast_to<Node2D>();
								if (pvt->edit_has_pivot()) {
									undo_redo->add_do_method(canvas_item,"edit_set_pivot",pvt->edit_get_pivot());
									undo_redo->add_undo_method(canvas_item,"edit_set_pivot",se->undo_pivot);
								}
							}
						}
						undo_redo->commit_action();
					}
				}

				drag=DRAG_NONE;
				viewport->update();
				can_move_pivot=false;

			}

			if (box_selecting) {
#if 0
				if ( ! b.mod.shift ) _clear_canvas_items();
				if ( box_selection_end() ) return;
#endif

				Node* scene = editor->get_edited_scene();
				if (scene) {

					List<CanvasItem*> selitems;

					Point2 bsfrom = transform.xform(drag_from);
					Point2 bsto= transform.xform(box_selecting_to);
					if (bsfrom.x>bsto.x)
						SWAP(bsfrom.x,bsto.x);
					if (bsfrom.y>bsto.y)
						SWAP(bsfrom.y,bsto.y);

					_find_canvas_items_at_rect(Rect2(bsfrom,bsto-bsfrom),scene,transform,Matrix32(),&selitems);

					for(List<CanvasItem*>::Element *E=selitems.front();E;E=E->next()) {

						_append_canvas_item(E->get());
					}

				}

				box_selecting=false;
				viewport->update();

			}
			return;
		}


		Map<ObjectID,BoneList>::Element *Cbone=NULL; //closest

		{
			bone_ik_list.clear();
			float closest_dist=1e20;
			int bone_width = EditorSettings::get_singleton()->get("2d_editor/bone_width");
			for(Map<ObjectID,BoneList>::Element *E=bone_list.front();E;E=E->next()) {

				if (E->get().from == E->get().to)
					continue;
				Vector2 s[2]={
					E->get().from,
					E->get().to
				};

				Vector2 p = Geometry::get_closest_point_to_segment_2d(Vector2(b.x,b.y),s);
				float d = p.distance_to(Vector2(b.x,b.y));
				if (d<bone_width && d<closest_dist) {
					Cbone=E;
					closest_dist=d;
				}
			}

			if (Cbone) {
				Node2D *b=NULL;
				Object* obj=ObjectDB::get_instance(Cbone->get().bone);
				if (obj)
					b=obj->cast_to<Node2D>();

				if (b) {


					bool ik_found=false;
					bool first=true;



					while(b) {

						CanvasItem *pi=b->get_parent_item();
						if (!pi)
							break;

						float len=pi->get_global_transform().get_origin().distance_to(b->get_global_pos());
						b=pi->cast_to<Node2D>();
						if (!b)
							break;

						if (first) {

							bone_orig_xform=b->get_global_transform();
							first=false;
						}

						BoneIK bik;
						bik.node=b;
						bik.len=len;
						bik.orig_state=b->edit_get_state();

						bone_ik_list.push_back(bik);

						if (b->has_meta("_edit_ik_")) {

							ik_found=bone_ik_list.size()>1;
							break;
						}

						if (!pi->has_meta("_edit_bone_"))
							break;

					}

					if (!ik_found)
						bone_ik_list.clear();

				}
			}
		}

		CanvasItem *single_item = get_single_item();

		if (single_item) {
			//try single canvas_item edit

			CanvasItem *canvas_item = single_item;
			CanvasItemEditorSelectedItem *se=editor_selection->get_node_editor_data<CanvasItemEditorSelectedItem>(canvas_item);
			ERR_FAIL_COND(!se);


			Point2 click(b.x,b.y);

			if ((b.mod.control && tool==TOOL_SELECT) || tool==TOOL_ROTATE) {

				drag=DRAG_ROTATE;
				drag_from=transform.affine_inverse().xform(click);
				se->undo_state=canvas_item->edit_get_state();
				if (canvas_item->cast_to<Node2D>())
					se->undo_pivot=canvas_item->cast_to<Node2D>()->edit_get_pivot();
				return;
			}

			Matrix32 xform = transform * canvas_item->get_global_transform_with_canvas();
			Rect2 rect=canvas_item->get_item_rect();
		//	float handle_radius = handle_len * 1.4144; //magic number, guess what it means!

			if (tool==TOOL_SELECT) {
				drag = _find_drag_type(xform,rect,click,drag_point_from);

				if (b.doubleclick) {

					if (canvas_item->get_filename()!="" && canvas_item!=editor->get_edited_scene()) {

						editor->open_request(canvas_item->get_filename());
						return;
					}
				}

				if (drag!=DRAG_NONE && (!Cbone || drag!=DRAG_ALL)) {
					drag_from=transform.affine_inverse().xform(click);
					se->undo_state=canvas_item->edit_get_state();
					if (canvas_item->cast_to<Node2D>())
						se->undo_pivot=canvas_item->cast_to<Node2D>()->edit_get_pivot();

					return;
				}
			} else {

				drag=DRAG_NONE;
			}
		}

		//multi canvas_item edit


		Point2 click=Point2(b.x,b.y);

		if ((b.mod.alt || tool==TOOL_MOVE) && get_item_count()) {


			List<Node*> &selection = editor_selection->get_selected_node_list();

			for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;
				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;

				CanvasItemEditorSelectedItem *se=editor_selection->get_node_editor_data<CanvasItemEditorSelectedItem>(canvas_item);
				if (!se)
					continue;

				se->undo_state=canvas_item->edit_get_state();
				if (canvas_item->cast_to<Node2D>())
					se->undo_pivot=canvas_item->cast_to<Node2D>()->edit_get_pivot();

			}


			drag=DRAG_ALL;
			drag_from=transform.affine_inverse().xform(click);
			drag_point_from=_find_topleftmost_point();
			viewport->update();
			return;

		}

		Node* scene = editor->get_edited_scene();
		if (!scene)
			return;

		/*
		if (current_window) {
			//no window.... ?
			click-=current_window->get_scroll();
		}*/
		CanvasItem *c=NULL;

		if (Cbone) {

			Object* obj=ObjectDB::get_instance(Cbone->get().bone);
			if (obj)
				c=obj->cast_to<CanvasItem>();
			if (c)
				c=c->get_parent_item();


		}
		if (!c) {
			c =_select_canvas_item_at_pos(click, scene,transform,Matrix32());


			CanvasItem* cn = c;

			while(cn) {
				if (cn->has_meta("_edit_group_")) {
					c=cn;
				}
				cn=cn->get_parent_item();
			}
		}

		Node* n = c;

		while ((n && n != scene && n->get_owner() != scene) || (n && !n->is_type("CanvasItem"))) {
			n = n->get_parent();
		};
		c = n->cast_to<CanvasItem>();
#if 0
		if ( b.pressed ) box_selection_start( click );
#endif

		additive_selection=b.mod.shift;
		if (!_select(c, click, additive_selection))
			return;

	}

	if (p_event.type==InputEvent::MOUSE_MOTION) {

        if (!viewport->has_focus() && (!get_focus_owner() || !get_focus_owner()->is_text_field()))
			viewport->call_deferred("grab_focus");

		const InputEventMouseMotion &m=p_event.mouse_motion;

		if (box_selecting) {

			box_selecting_to=transform.affine_inverse().xform(Point2(m.x,m.y));
			viewport->update();
			return;

		}


		if (drag==DRAG_NONE) {


			if ( (m.button_mask&BUTTON_MASK_LEFT && tool == TOOL_PAN) || m.button_mask&BUTTON_MASK_MIDDLE || (m.button_mask&BUTTON_MASK_LEFT && Input::get_singleton()->is_key_pressed(KEY_SPACE))) {

				h_scroll->set_val( h_scroll->get_val() - m.relative_x/zoom);
				v_scroll->set_val( v_scroll->get_val() - m.relative_y/zoom);
			}

			return;

		}

		List<Node*> &selection = editor_selection->get_selected_node_list();


		for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

			CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
			if (!canvas_item || !canvas_item->is_visible())
				continue;
			if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
				continue;

			CanvasItemEditorSelectedItem *se=editor_selection->get_node_editor_data<CanvasItemEditorSelectedItem>(canvas_item);
			if (!se)
				continue;

			bool dragging_bone = drag==DRAG_ALL && selection.size()==1 && bone_ik_list.size();


			if (!dragging_bone) {
				canvas_item->edit_set_state(se->undo_state); //reset state and reapply
				if (canvas_item->cast_to<Node2D>())
					canvas_item->cast_to<Node2D>()->edit_set_pivot(se->undo_pivot);
			}


			Vector2 dfrom = drag_from;
			Vector2 dto = transform.affine_inverse().xform(Point2(m.x,m.y));
			if (canvas_item->has_meta("_edit_lock_"))
				continue;


			if (drag==DRAG_ROTATE) {

				Vector2 center = canvas_item->get_global_transform_with_canvas().get_origin();
				if (Node2D *node = canvas_item->cast_to<Node2D>()) {
					Matrix32 rot;
					rot.elements[1] = (dfrom - center).normalized();
					rot.elements[0] = rot.elements[1].tangent();
					node->set_rot(snap_angle(rot.xform_inv(dto-center).angle() + node->get_rot(), node->get_rot()));
					display_rotate_to = dto;
					display_rotate_from = center;
					viewport->update();
				}

				continue;
			}


			bool uniform = m.mod.shift;
			bool symmetric=m.mod.alt;

			dto = dto - (drag == DRAG_ALL ? drag_from - drag_point_from : Vector2(0, 0));

			if(uniform && drag == DRAG_ALL) {
				if(ABS(dto.x - drag_point_from.x) > ABS(dto.y - drag_point_from.y)) {
					dto.y = drag_point_from.y;
				} else {
					dto.x = drag_point_from.x;
				}
			}

			dfrom = drag_point_from;
			dto = snap_point(dto, drag_point_from);

			Vector2 drag_vector =
					canvas_item->get_global_transform_with_canvas().affine_inverse().xform(dto) -
					canvas_item->get_global_transform_with_canvas().affine_inverse().xform(dfrom);

			// Handling dragging vector rotation
			if (Control *control = canvas_item->cast_to<Control>()) {

				drag_vector *= control->get_scale();

				if (control->get_rotation() > 0.0 || control->get_rotation() < 0.0)
				{
					float cos_val = Math::cos(control->get_rotation());
					float sin_val = Math::sin(control->get_rotation());

					float x = cos_val * drag_vector.x - sin_val * drag_vector.y;
					float y = sin_val * drag_vector.x + cos_val * drag_vector.y;

					drag_vector = Vector2(x, y);
				}
			}

			Rect2 local_rect = canvas_item->get_item_rect();
			Vector2 begin=local_rect.pos;
			Vector2 end=local_rect.pos+local_rect.size;
			Vector2 minsize = canvas_item->edit_get_minimum_size();

			if (uniform) {
				float aspect = local_rect.size.get_aspect();
				switch(drag) {
					case DRAG_BOTTOM_LEFT:
					case DRAG_TOP_RIGHT: {
						if (aspect > 1.0) { // width > height, take x as reference
							drag_vector.y = -drag_vector.x/aspect;
						} else { // height > width, take y as reference
							drag_vector.x = -drag_vector.y*aspect;
						}
					} break;
					case DRAG_BOTTOM_RIGHT:
					case DRAG_TOP_LEFT: {
						if (aspect > 1.0) { // width > height, take x as reference
							drag_vector.y = drag_vector.x/aspect;
						} else { // height > width, take y as reference
							drag_vector.x = drag_vector.y*aspect;
						}
					} break;
					default: {}
				}
			}

			switch(drag) {
				case DRAG_ALL: {
					begin+=drag_vector;
					end+=drag_vector;
				} break;
				case DRAG_RIGHT: {

					incend(begin.x,end.x,drag_vector.x,minsize.x,symmetric);

				} break;
				case DRAG_BOTTOM: {

					incend(begin.y,end.y,drag_vector.y,minsize.y,symmetric);

				} break;
				case DRAG_BOTTOM_RIGHT: {

					incend(begin.x,end.x,drag_vector.x,minsize.x,symmetric);
					incend(begin.y,end.y,drag_vector.y,minsize.y,symmetric);
				} break;				
				case DRAG_TOP_LEFT: {

					incbeg(begin.x,end.x,drag_vector.x,minsize.x,symmetric);
					incbeg(begin.y,end.y,drag_vector.y,minsize.y,symmetric);
				} break;
				case DRAG_TOP: {

					incbeg(begin.y,end.y,drag_vector.y,minsize.y,symmetric);

				} break;
				case DRAG_LEFT: {

					incbeg(begin.x,end.x,drag_vector.x,minsize.x,symmetric);

				} break;
				case DRAG_TOP_RIGHT: {

					incbeg(begin.y,end.y,drag_vector.y,minsize.y,symmetric);
					incend(begin.x,end.x,drag_vector.x,minsize.x,symmetric);

				} break;
				case DRAG_BOTTOM_LEFT: {

					incbeg(begin.x,end.x,drag_vector.x,minsize.x,symmetric);
					incend(begin.y,end.y,drag_vector.y,minsize.y,symmetric);
				} break;
				case DRAG_PIVOT: {

					if (canvas_item->cast_to<Node2D>()) {
						Node2D *n2d =canvas_item->cast_to<Node2D>();
						n2d->edit_set_pivot(se->undo_pivot+drag_vector);

					}
					continue;
				} break;

				default:{}
			}



			if (!dragging_bone) {

				local_rect.pos=begin;
				local_rect.size=end-begin;
				canvas_item->edit_set_rect(local_rect);

			} else {
				//ok, all that had to be done was done, now solve IK




				Node2D *n2d = canvas_item->cast_to<Node2D>();
				Matrix32 final_xform = bone_orig_xform;



				if (n2d) {

					float total_len = 0;
					for (List<BoneIK>::Element *E=bone_ik_list.front();E;E=E->next()) {
						if (E->prev())
							total_len+=E->get().len;
						E->get().pos = E->get().node->get_global_transform().get_origin();
					}

					{

						final_xform.elements[2]+=dto-dfrom;//final_xform.affine_inverse().basis_xform_inv(drag_vector);
						//n2d->set_global_transform(final_xform);

					}


					CanvasItem *last = bone_ik_list.back()->get().node;
					if (!last)
						break;

					Vector2 root_pos = last->get_global_transform().get_origin();
					Vector2 leaf_pos = final_xform.get_origin();

					if ((leaf_pos.distance_to(root_pos)) > total_len) {
						//oops dude you went too far
						//print_line("TOO FAR!");
						Vector2 rel = leaf_pos - root_pos;
						rel = rel.normalized() * total_len;
						leaf_pos=root_pos+rel;

					}

					bone_ik_list.front()->get().pos=leaf_pos;

					//print_line("BONE IK LIST "+itos(bone_ik_list.size()));


					if (bone_ik_list.size()>2) {
						int solver_iterations=64;
						float solver_k=0.3;

						for(int i=0;i<solver_iterations;i++) {

							for (List<BoneIK>::Element *E=bone_ik_list.front();E;E=E->next()) {



								if (E==bone_ik_list.back()) {

									break;
								}

								float len = E->next()->get().len;

								if (E->next()==bone_ik_list.back()) {

									//print_line("back");

									Vector2 rel = E->get().pos - E->next()->get().pos;
									//print_line("PREV "+E->get().pos);
									Vector2 desired = E->next()->get().pos+rel.normalized()*len;
									//print_line("DESIRED "+desired);
									E->get().pos=E->get().pos.linear_interpolate(desired,solver_k);
									//print_line("POST "+E->get().pos);


								} else if (E==bone_ik_list.front()) {
									//only adjust parent
									//print_line("front");
									Vector2 rel = E->next()->get().pos - E->get().pos;
									//print_line("PREV "+E->next()->get().pos);
									Vector2 desired = E->get().pos+rel.normalized()*len;
									//print_line("DESIRED "+desired);
									E->next()->get().pos=E->next()->get().pos.linear_interpolate(desired,solver_k);
									//print_line("POST "+E->next()->get().pos);
								} else {

									Vector2 rel = E->next()->get().pos - E->get().pos;
									Vector2 cen = (E->next()->get().pos + E->get().pos)*0.5;
									rel=rel.linear_interpolate(rel.normalized()*len,solver_k);
									rel*=0.5;
									E->next()->get().pos=cen+rel;
									E->get().pos=cen-rel;
									//print_line("mid");

								}
							}
						}
					}
				}

				for (List<BoneIK>::Element *E=bone_ik_list.back();E;E=E->prev()) {

					Node2D *n = E->get().node;

					if (!E->prev()) {
						//last goes to what it was
						final_xform.set_origin(n->get_global_pos());
						n->set_global_transform(final_xform);

					} else {
						Vector2 rel = (E->prev()->get().node->get_global_pos() - n->get_global_pos()).normalized();
						Vector2 rel2 = (E->prev()->get().pos - E->get().pos).normalized();
						float rot = rel.angle_to(rel2);
						if (n->get_global_transform().basis_determinant()<0) {
							//mirrored, rotate the other way
							rot=-rot;
						}

						n->rotate(rot);
					}

				}



				break;
			}
		}
	}

	if (p_event.type==InputEvent::KEY) {

		const InputEventKey &k=p_event.key;

		if (k.pressed && drag==DRAG_NONE) {

			KeyMoveMODE move_mode = MOVE_VIEW_BASE;
			if (k.mod.alt) move_mode = MOVE_LOCAL_BASE;
			if (k.mod.control || k.mod.meta) move_mode = MOVE_LOCAL_WITH_ROT;

			if (k.scancode==KEY_UP)
				_key_move( Vector2(0,-1), k.mod.shift, move_mode );
			else if (k.scancode==KEY_DOWN)
				_key_move( Vector2(0,1), k.mod.shift, move_mode );
			else if (k.scancode==KEY_LEFT)
				_key_move( Vector2(-1,0), k.mod.shift, move_mode );
			else if (k.scancode==KEY_RIGHT)
				_key_move( Vector2(1,0), k.mod.shift, move_mode );
			else if (k.scancode==KEY_ESCAPE) {
				editor_selection->clear();
				viewport->update();
			}
			else
				return;

			accept_event();
		}

	}




}

void CanvasItemEditor::_viewport_draw() {

	// TODO fetch the viewport?

	Ref<Texture> pivot = get_icon("EditorPivot","EditorIcons");
	_update_scrollbars();
	RID ci=viewport->get_canvas_item();

	if (snap_show_grid) {
		Size2 s = viewport->get_size();
		int last_cell;
		Matrix32 xform = transform.affine_inverse();

		if (snap_step.x!=0) {
			for(int i=0;i<s.width;i++) {
				int cell = Math::fast_ftoi(Math::floor((xform.xform(Vector2(i,0)).x-snap_offset.x)/snap_step.x));
				if (i==0)
					last_cell=cell;
				if (last_cell!=cell)
					viewport->draw_line(Point2(i,0),Point2(i,s.height),Color(0.3,0.7,1,0.3));
				last_cell=cell;
			}
		}

		if (snap_step.y!=0) {
			for(int i=0;i<s.height;i++) {
				int cell = Math::fast_ftoi(Math::floor((xform.xform(Vector2(0,i)).y-snap_offset.y)/snap_step.y));
				if (i==0)
					last_cell=cell;
				if (last_cell!=cell)
					viewport->draw_line(Point2(0,i),Point2(s.width,i),Color(0.3,0.7,1,0.3));
				last_cell=cell;
			}
		}
	}

	if (viewport->has_focus()) {
		Size2 size = viewport->get_size();
		if (v_scroll->is_visible())
			size.width-=v_scroll->get_size().width;
		if (h_scroll->is_visible())
			size.height-=h_scroll->get_size().height;

		get_stylebox("EditorFocus","EditorStyles")->draw(ci,Rect2(Point2(),size));
	}

	Ref<Texture> lock = get_icon("Lock","EditorIcons");
	Ref<Texture> group = get_icon("Group","EditorIcons");

	VisualServer::get_singleton()->canvas_item_set_clip(ci,true);

	bool single = get_single_item()!=NULL;

	Map<Node*,Object*> &selection = editor_selection->get_selection();

	CanvasItem *single_item=NULL;

	bool pivot_found=false;

	for(Map<Node*,Object*>::Element *E=selection.front();E;E=E->next()) {


		CanvasItem *canvas_item = E->key()->cast_to<CanvasItem>();
		if (!canvas_item || !canvas_item->is_visible())
			continue;
		if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
			continue;
		CanvasItemEditorSelectedItem *se=editor_selection->get_node_editor_data<CanvasItemEditorSelectedItem>(canvas_item);
		if (!se)
			continue;


		Rect2 rect=canvas_item->get_item_rect();

		Matrix32 xform=transform * canvas_item->get_global_transform_with_canvas();
		VisualServer::get_singleton()->canvas_item_add_set_transform(ci,xform);
		Point2 ofs=Point2();//get_global_pos();
		Rect2 draw_rect=rect;
		Color light_edit_color=Color(1.0,0.8,0.8);
		Color dark_edit_color=Color(0.4,0.1,0.1);
		Size2 handle_size=Size2(handle_len,handle_len);

		//select_sb->draw(ci,draw_rect.grow(2));
		//DRAW_EMPTY_RECT( draw_rect.grow(2), light_edit_color );
		//DRAW_EMPTY_RECT( draw_rect.grow(1), dark_edit_color );

		Vector2 endpoints[4]={

			xform.xform(rect.pos),
			xform.xform(rect.pos+Vector2(rect.size.x,0)),
			xform.xform(rect.pos+rect.size),
			xform.xform(rect.pos+Vector2(0,rect.size.y))
		};

		Color c = Color(1,0.6,0.4,0.7);

		VisualServer::get_singleton()->canvas_item_add_set_transform(ci,Matrix32());

		for(int i=0;i<4;i++) {
			viewport->draw_line(endpoints[i],endpoints[(i+1)%4],c,2);
		}

		if (single && (tool==TOOL_SELECT || tool == TOOL_MOVE || tool == TOOL_ROTATE || tool==TOOL_EDIT_PIVOT)) { //kind of sucks

			if (canvas_item->cast_to<Node2D>()) {


				if (canvas_item->cast_to<Node2D>()->edit_has_pivot()) {
					viewport->draw_texture(pivot,xform.get_origin()+(-pivot->get_size()/2).floor());
					can_move_pivot=true;
					pivot_found=true;
				}

			}


			if (tool==TOOL_SELECT) {


				for(int i=0;i<4;i++) {

					int prev = (i+3)%4;
					int next = (i+1)%4;

					Vector2 ofs = ((endpoints[i] - endpoints[prev]).normalized() + ((endpoints[i] - endpoints[next]).normalized())).normalized();
					ofs*=1.4144*(select_handle->get_size().width/2);

					select_handle->draw(ci,(endpoints[i]+ofs-(select_handle->get_size()/2)).floor());

					ofs = (endpoints[i]+endpoints[next])/2;
					ofs += (endpoints[next]-endpoints[i]).tangent().normalized()*(select_handle->get_size().width/2);

					select_handle->draw(ci,(ofs-(select_handle->get_size()/2)).floor());

				}

			}
		}



		//DRAW_EMPTY_RECT( Rect2( current_window->get_scroll()-Point2(1,1), get_size()+Size2(2,2)), Color(0.8,0.8,1.0,0.8) );
		//E->get().last_rect = rect;
	}

	pivot_button->set_disabled(!pivot_found);
	VisualServer::get_singleton()->canvas_item_add_set_transform(ci,Matrix32());



	Color x_axis_color(1.0,0.4,0.4,0.6);
	Color y_axis_color(0.4,1.0,0.4,0.6);
	Color area_axis_color(0.4,0.4,1.0,0.4);
	Color rotate_color(0.4,0.7,1.0,0.8);

	VisualServer::get_singleton()->canvas_item_add_line(ci,Point2(h_scroll->get_min(),0)+transform.get_origin(),Point2(h_scroll->get_max(),0)+transform.get_origin(),x_axis_color);
	VisualServer::get_singleton()->canvas_item_add_line(ci,Point2(0,v_scroll->get_min())+transform.get_origin(),Point2(0,v_scroll->get_max())+transform.get_origin(),y_axis_color);


	if (box_selecting) {

		Point2 bsfrom = transform.xform(drag_from);
		Point2 bsto= transform.xform(box_selecting_to);


		VisualServer::get_singleton()->canvas_item_add_rect(ci,Rect2(bsfrom,bsto-bsfrom),Color(0.7,0.7,1.0,0.3));
	}

	if (drag==DRAG_ROTATE) {
		VisualServer::get_singleton()->canvas_item_add_line(ci,transform.xform(display_rotate_from), transform.xform(display_rotate_to),rotate_color);
	}

	Size2 screen_size = Size2( Globals::get_singleton()->get("display/width"), Globals::get_singleton()->get("display/height") );

	Vector2 screen_endpoints[4]= {
		transform.xform(Vector2(0,0)),
		transform.xform(Vector2(screen_size.width,0)),
		transform.xform(Vector2(screen_size.width,screen_size.height)),
		transform.xform(Vector2(0,screen_size.height))
	};

	for(int i=0;i<4;i++) {

		VisualServer::get_singleton()->canvas_item_add_line(ci,screen_endpoints[i], screen_endpoints[(i+1)%4],area_axis_color);

	}

	for(List<LockList>::Element*E=lock_list.front();E;E=E->next()) {

		Vector2 ofs = transform.xform(E->get().pos);
		if (E->get().lock) {

			lock->draw(ci,ofs);
			ofs.x+=lock->get_width();
		}
		if (E->get().group) {

			group->draw(ci,ofs);
		}

	}

	int bone_width = EditorSettings::get_singleton()->get("2d_editor/bone_width");
	Color bone_color1 = EditorSettings::get_singleton()->get("2d_editor/bone_color1");
	Color bone_color2 = EditorSettings::get_singleton()->get("2d_editor/bone_color2");
	Color bone_ik_color = EditorSettings::get_singleton()->get("2d_editor/bone_ik_color");
	Color bone_selected_color = EditorSettings::get_singleton()->get("2d_editor/bone_selected_color");

	for(Map<ObjectID,BoneList>::Element*E=bone_list.front();E;E=E->next()) {

		E->get().from=Vector2();
		E->get().to=Vector2();

		Object *obj = ObjectDB::get_instance(E->get().bone);
		if (!obj)
			continue;

		Node2D* n2d = obj->cast_to<Node2D>();
		if (!n2d)
			continue;

		if (!n2d->get_parent())
			continue;

		CanvasItem *pi = n2d->get_parent_item();


		Node2D* pn2d=n2d->get_parent()->cast_to<Node2D>();

		if (!pn2d)
			continue;

		Vector2 from = transform.xform(pn2d->get_global_pos());
		Vector2 to = transform.xform(n2d->get_global_pos());

		E->get().from=from;
		E->get().to=to;

		Vector2 rel = to-from;
		Vector2 relt = rel.tangent().normalized()*bone_width;



		Vector<Vector2> bone_shape;
		bone_shape.push_back(from);
		bone_shape.push_back(from+rel*0.2+relt);
		bone_shape.push_back(to);
		bone_shape.push_back(from+rel*0.2-relt);
		Vector<Color> colors;
		if (pi->has_meta("_edit_ik_")) {

			colors.push_back(bone_ik_color);
			colors.push_back(bone_ik_color);
			colors.push_back(bone_ik_color);
			colors.push_back(bone_ik_color);
		} else {
			colors.push_back(bone_color1);
			colors.push_back(bone_color2);
			colors.push_back(bone_color1);
			colors.push_back(bone_color2);
		}


		VisualServer::get_singleton()->canvas_item_add_primitive(ci,bone_shape,colors,Vector<Vector2>(),RID());

		if (editor_selection->is_selected(pi)) {
			for(int i=0;i<bone_shape.size();i++) {

				VisualServer::get_singleton()->canvas_item_add_line(ci,bone_shape[i],bone_shape[(i+1)%bone_shape.size()],bone_selected_color,2);
			}
		}

	}
}

void CanvasItemEditor::_notification(int p_what) {

	if (p_what==NOTIFICATION_FIXED_PROCESS) {

		List<Node*> &selection = editor_selection->get_selected_node_list();

		bool all_control=true;
		bool has_control=false;

		for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

			CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
			if (!canvas_item || !canvas_item->is_visible())
				continue;

			if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
				continue;

			if (canvas_item->cast_to<Control>())
				has_control=true;
			else
				all_control=false;

			CanvasItemEditorSelectedItem *se=editor_selection->get_node_editor_data<CanvasItemEditorSelectedItem>(canvas_item);
			if (!se)
				continue;

			Rect2 r=canvas_item->get_item_rect();

			Matrix32 xform = canvas_item->get_transform();

			if (r != se->prev_rect || xform!=se->prev_xform) {
				viewport->update();
				se->prev_rect=r;
				se->prev_xform=xform;
			}

		}

		bool show_anchor = all_control && has_control;
		if (show_anchor != !anchor_menu->is_hidden()) {
			if (show_anchor)
				anchor_menu->show();
			else
				anchor_menu->hide();
		}

		for(Map<ObjectID,BoneList>::Element *E=bone_list.front();E;E=E->next()) {

			Object *b = ObjectDB::get_instance(E->get().bone);
			if (!b) {

				viewport->update();
				break;
			}

			Node2D *b2 = b->cast_to<Node2D>();
			if (!b2) {
				continue;
			}

			if (b2->get_global_transform()!=E->get().xform) {

				E->get().xform=b2->get_global_transform();
				viewport->update();
			}
		}
	}

	if (p_what==NOTIFICATION_ENTER_TREE) {

		select_sb->set_texture( get_icon("EditorRect2D","EditorIcons") );
		for(int i=0;i<4;i++) {
			select_sb->set_margin_size(Margin(i),4);
			select_sb->set_default_margin(Margin(i),4);
		}

		select_button->set_icon( get_icon("ToolSelect","EditorIcons"));
		list_select_button->set_icon( get_icon("ListSelect","EditorIcons"));
		move_button->set_icon( get_icon("ToolMove","EditorIcons"));
		rotate_button->set_icon( get_icon("ToolRotate","EditorIcons"));
		pan_button->set_icon( get_icon("ToolPan", "EditorIcons"));
		pivot_button->set_icon( get_icon("EditPivot", "EditorIcons"));
		select_handle=get_icon("EditorHandle","EditorIcons");
		lock_button->set_icon(get_icon("Lock","EditorIcons"));
		unlock_button->set_icon(get_icon("Unlock","EditorIcons"));
		group_button->set_icon(get_icon("Group","EditorIcons"));
		ungroup_button->set_icon(get_icon("Ungroup","EditorIcons"));
		key_insert_button->set_icon(get_icon("Key","EditorIcons"));


		//anchor_menu->add_icon_override("Align Top Left");
		anchor_menu->set_icon(get_icon("Anchor","EditorIcons"));
		PopupMenu *p=anchor_menu->get_popup();

		p->add_icon_item(get_icon("ControlAlignTopLeft","EditorIcons"),"Top Left",ANCHOR_ALIGN_TOP_LEFT);
		p->add_icon_item(get_icon("ControlAlignTopRight","EditorIcons"),"Top Right",ANCHOR_ALIGN_TOP_RIGHT);
		p->add_icon_item(get_icon("ControlAlignBottomRight","EditorIcons"),"Bottom Right",ANCHOR_ALIGN_BOTTOM_RIGHT);
		p->add_icon_item(get_icon("ControlAlignBottomLeft","EditorIcons"),"Bottom Left",ANCHOR_ALIGN_BOTTOM_LEFT);
		p->add_separator();
		p->add_icon_item(get_icon("ControlAlignLeftCenter","EditorIcons"),"Center Left",ANCHOR_ALIGN_CENTER_LEFT);
		p->add_icon_item(get_icon("ControlAlignTopCenter","EditorIcons"),"Center Top",ANCHOR_ALIGN_CENTER_TOP);
		p->add_icon_item(get_icon("ControlAlignRightCenter","EditorIcons"),"Center Right",ANCHOR_ALIGN_CENTER_RIGHT);
		p->add_icon_item(get_icon("ControlAlignBottomCenter","EditorIcons"),"Center Bottom",ANCHOR_ALIGN_CENTER_BOTTOM);
		p->add_icon_item(get_icon("ControlAlignCenter","EditorIcons"),"Center",ANCHOR_ALIGN_CENTER);
		p->add_separator();
		p->add_icon_item(get_icon("ControlAlignLeftWide","EditorIcons"),"Left Wide",ANCHOR_ALIGN_LEFT_WIDE);
		p->add_icon_item(get_icon("ControlAlignTopWide","EditorIcons"),"Top Wide",ANCHOR_ALIGN_TOP_WIDE);
		p->add_icon_item(get_icon("ControlAlignRightWide","EditorIcons"),"Right Wide",ANCHOR_ALIGN_RIGHT_WIDE);
		p->add_icon_item(get_icon("ControlAlignBottomWide","EditorIcons"),"Bottom Wide",ANCHOR_ALIGN_BOTTOM_WIDE);
		p->add_icon_item(get_icon("ControlVcenterWide","EditorIcons"),"VCenter Wide ",ANCHOR_ALIGN_VCENTER_WIDE);
		p->add_icon_item(get_icon("ControlHcenterWide","EditorIcons"),"HCenter Wide ",ANCHOR_ALIGN_HCENTER_WIDE);
		p->add_separator();
		p->add_icon_item(get_icon("ControlAlignWide","EditorIcons"),"Full Rect",ANCHOR_ALIGN_WIDE);


		AnimationPlayerEditor::singleton->get_key_editor()->connect("visibility_changed",this,"_keying_changed");
		_keying_changed();
	}

	if (p_what==NOTIFICATION_READY) {

		get_tree()->connect("node_removed",this,"_node_removed");
	}

	if (p_what==NOTIFICATION_DRAW) {



	}
}

void CanvasItemEditor::edit(CanvasItem *p_canvas_item) {

	drag=DRAG_NONE;

	editor_selection->clear();//_clear_canvas_items();
	editor_selection->add_node(p_canvas_item);
	//_add_canvas_item(p_canvas_item);
	viewport->update();

}


void CanvasItemEditor::_find_canvas_items_span(Node *p_node, Rect2& r_rect, const Matrix32& p_xform) {



	if (!p_node)
		return;

	CanvasItem *c=p_node->cast_to<CanvasItem>();


	for (int i=p_node->get_child_count()-1;i>=0;i--) {

//		CanvasItem *r=NULL;

		if (c && !c->is_set_as_toplevel())
			_find_canvas_items_span(p_node->get_child(i),r_rect,p_xform * c->get_transform());
		else
			_find_canvas_items_span(p_node->get_child(i),r_rect,Matrix32());
	}



	if (c) {

		Rect2 rect = c->get_item_rect();
		Matrix32 xform = p_xform * c->get_transform();


		LockList lock;
		lock.lock=c->has_meta("_edit_lock_");
		lock.group=c->has_meta("_edit_group_");

		if (lock.group || lock.lock) {
			lock.pos=xform.xform(rect.pos);
			lock_list.push_back(lock);
		}

		if (c->has_meta("_edit_bone_")) {

			ObjectID id = c->get_instance_ID();
			if (!bone_list.has(id)) {
				BoneList bone;
				bone.bone=id;
				bone_list[id]=bone;
			}

			bone_list[id].last_pass=bone_last_frame;
		}

		r_rect.expand_to( xform.xform(rect.pos) );
		r_rect.expand_to( xform.xform(rect.pos+Point2(rect.size.x,0)) );
		r_rect.expand_to( xform.xform(rect.pos+Point2(0,rect.size.y)) );
		r_rect.expand_to( xform.xform(rect.pos+rect.size) );

	}

}

void CanvasItemEditor::_update_scrollbars() {


	updating_scroll=true;

	Size2 size = viewport->get_size();
	Size2 hmin = h_scroll->get_minimum_size();
	Size2 vmin = v_scroll->get_minimum_size();

	v_scroll->set_begin( Point2(size.width - vmin.width, 0) );
	v_scroll->set_end( Point2(size.width, size.height) );

	h_scroll->set_begin( Point2( 0, size.height - hmin.height) );
	h_scroll->set_end( Point2(size.width-vmin.width, size.height) );


	Size2 screen_rect = Size2( Globals::get_singleton()->get("display/width"), Globals::get_singleton()->get("display/height") );

	Rect2 local_rect = Rect2(Point2(),viewport->get_size()-Size2(vmin.width,hmin.height));

	Rect2 canvas_item_rect=Rect2(Point2(),screen_rect);

	lock_list.clear();;
	bone_last_frame++;



	if (editor->get_edited_scene())
		_find_canvas_items_span(editor->get_edited_scene(),canvas_item_rect,Matrix32());

	List<Map<ObjectID,BoneList>::Element*> bone_to_erase;

	for(Map<ObjectID,BoneList>::Element*E=bone_list.front();E;E=E->next()) {

		if (E->get().last_pass!=bone_last_frame) {
			bone_to_erase.push_back(E);
		}
	}

	while(bone_to_erase.size()) {
		bone_list.erase(bone_to_erase.front()->get());
		bone_to_erase.pop_front();
	}

	//expand area so it's easier to do animations and stuff at 0,0
	canvas_item_rect.size+=screen_rect*2;
	canvas_item_rect.pos-=screen_rect;

	Point2 ofs;


	if (canvas_item_rect.size.height <= (local_rect.size.y/zoom)) {

		v_scroll->hide();
		ofs.y=canvas_item_rect.pos.y;
	} else {

		v_scroll->show();
		v_scroll->set_min(canvas_item_rect.pos.y);
		v_scroll->set_max(canvas_item_rect.pos.y+canvas_item_rect.size.y);
		v_scroll->set_page(local_rect.size.y/zoom);
		if (first_update) {
			//so 0,0 is visible
			v_scroll->set_val(-10);
			h_scroll->set_val(-10);
			first_update=false;

		}

		ofs.y=v_scroll->get_val();
	}

	if (canvas_item_rect.size.width <= (local_rect.size.x/zoom)) {

		h_scroll->hide();
		ofs.x=canvas_item_rect.pos.x;
	} else {

		h_scroll->show();
		h_scroll->set_min(canvas_item_rect.pos.x);
		h_scroll->set_max(canvas_item_rect.pos.x+canvas_item_rect.size.x);
		h_scroll->set_page(local_rect.size.x/zoom);
		ofs.x=h_scroll->get_val();
	}

//	transform=Matrix32();
	transform.elements[2]=-ofs*zoom;
	editor->get_scene_root()->set_global_canvas_transform(transform);


	updating_scroll=false;

//	transform.scale_basis(Vector2(zoom,zoom));


}

void CanvasItemEditor::_update_scroll(float) {


	if (updating_scroll)
		return;

	Point2 ofs;
	ofs.x=h_scroll->get_val();
	ofs.y=v_scroll->get_val();

//	current_window->set_scroll(-ofs);

	transform=Matrix32();

	transform.scale_basis(Size2(zoom,zoom));
	transform.elements[2]=-ofs;

	editor->get_scene_root()->set_global_canvas_transform(transform);


	viewport->update();

}

void CanvasItemEditor::_set_anchor(Control::AnchorType p_left,Control::AnchorType p_top,Control::AnchorType p_right,Control::AnchorType p_bottom) {
	List<Node*> &selection = editor_selection->get_selected_node_list();

	undo_redo->create_action(TTR("Change Anchors"));
	for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

		Control *c = E->get()->cast_to<Control>();

		undo_redo->add_do_method(c,"set_anchor",MARGIN_LEFT,p_left);
		undo_redo->add_do_method(c,"set_anchor",MARGIN_TOP,p_top);
		undo_redo->add_do_method(c,"set_anchor",MARGIN_RIGHT,p_right);
		undo_redo->add_do_method(c,"set_anchor",MARGIN_BOTTOM,p_bottom);
		undo_redo->add_undo_method(c,"set_anchor",MARGIN_LEFT,c->get_anchor(MARGIN_LEFT));
		undo_redo->add_undo_method(c,"set_anchor",MARGIN_TOP,c->get_anchor(MARGIN_TOP));
		undo_redo->add_undo_method(c,"set_anchor",MARGIN_RIGHT,c->get_anchor(MARGIN_RIGHT));
		undo_redo->add_undo_method(c,"set_anchor",MARGIN_BOTTOM,c->get_anchor(MARGIN_BOTTOM));
	}

	undo_redo->commit_action();

}

void CanvasItemEditor::_popup_callback(int p_op) {

	last_option=MenuOption(p_op);
	switch(p_op) {

		case SNAP_USE: {
			snap_grid = !snap_grid;
			int idx = edit_menu->get_popup()->get_item_index(SNAP_USE);
			edit_menu->get_popup()->set_item_checked(idx,snap_grid);
		} break;
		case SNAP_SHOW_GRID: {
			snap_show_grid = !snap_show_grid;
			int idx = edit_menu->get_popup()->get_item_index(SNAP_SHOW_GRID);
			edit_menu->get_popup()->set_item_checked(idx,snap_show_grid);
			viewport->update();
		} break;
		case SNAP_USE_ROTATION: {
			snap_rotation = !snap_rotation;
			int idx = edit_menu->get_popup()->get_item_index(SNAP_USE_ROTATION);
			edit_menu->get_popup()->set_item_checked(idx,snap_rotation);
		} break;
		case SNAP_RELATIVE: {
			snap_relative = !snap_relative;
			int idx = edit_menu->get_popup()->get_item_index(SNAP_RELATIVE);
			edit_menu->get_popup()->set_item_checked(idx,snap_relative);
		} break;
		case SNAP_USE_PIXEL: {
			snap_pixel = !snap_pixel;
			int idx = edit_menu->get_popup()->get_item_index(SNAP_USE_PIXEL);
			edit_menu->get_popup()->set_item_checked(idx,snap_pixel);
		} break;
		case SNAP_CONFIGURE: {
			((SnapDialog *)snap_dialog)->set_fields(snap_offset, snap_step, snap_rotation_offset, snap_rotation_step);
			snap_dialog->popup_centered(Size2(220,160));
		} break;
		case ZOOM_IN: {
			zoom=zoom*(1.0/0.5);
			_update_scroll(0);
			viewport->update();
			return;
		} break;
		case ZOOM_OUT: {
			zoom=zoom*0.5;
			_update_scroll(0);
			viewport->update();
			return;

		} break;
		case ZOOM_RESET: {

			zoom=1;
			_update_scroll(0);
			viewport->update();
			return;

		} break;
		case ZOOM_SET: {

			updating_value_dialog=true;

			dialog_label->set_text(TTR("Zoom (%):"));
			dialog_val->set_min(0.1);
			dialog_val->set_step(0.1);
			dialog_val->set_max(800);
			dialog_val->set_val(zoom*100);
			value_dialog->popup_centered(Size2(200,85));
			updating_value_dialog=false;


		} break;
		case LOCK_SELECTED: {

			List<Node*> &selection = editor_selection->get_selected_node_list();

			for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;

				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;

				canvas_item->set_meta("_edit_lock_",true);
				emit_signal("item_lock_status_changed");
			}
			viewport->update();
		} break;
		case UNLOCK_SELECTED: {

			List<Node*> &selection = editor_selection->get_selected_node_list();

			for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;

				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;


				canvas_item->set_meta("_edit_lock_",Variant());
				emit_signal("item_lock_status_changed");
			}

			viewport->update();

		} break;
		case GROUP_SELECTED: {

			List<Node*> &selection = editor_selection->get_selected_node_list();

			for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;

				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;

				canvas_item->set_meta("_edit_group_",true);
				emit_signal("item_group_status_changed");
			}
			viewport->update();
		} break;
		case UNGROUP_SELECTED: {

			List<Node*> &selection = editor_selection->get_selected_node_list();

			for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;

				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;

				canvas_item->set_meta("_edit_group_",Variant());
				emit_signal("item_group_status_changed");
			}

			viewport->update();

		} break;

		case EXPAND_TO_PARENT: {

			List<Node*> &selection = editor_selection->get_selected_node_list();

			for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;

				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;


				Control *c = canvas_item->cast_to<Control>();
				if (!c)
					continue;
				c->set_area_as_parent_rect();

			}

			viewport->update();

		} break;

		case ALIGN_VERTICAL: {
#if 0
			if ( ref_item && canvas_items.size() > 1 ) {
				Vector2 ref_pos = ref_item->get_global_transform().elements[2];
				Rect2 ref_r = ref_item->get_item_rect();
				for ( CanvasItemMap::Element *E = canvas_items.front(); E; E = E->next() ) {
					CanvasItem *it_curr = E->key();
					if ( it_curr == ref_item ) continue;
					Vector2 v = it_curr->get_global_transform().elements[2];
					Rect2 r = it_curr->get_item_rect();
					r.pos.x = ( ref_pos.x + ref_r.size.x / 2 ) - ( v.x + r.size.x / 2 );
					it_curr->edit_set_rect( r );
				}
				viewport->update();
			}
#endif
		} break;

		case ALIGN_HORIZONTAL: {
#if 0
			if ( ref_item && canvas_items.size() > 1 ) {
				Vector2 ref_pos = ref_item->get_global_transform().elements[2];
				Rect2 ref_r = ref_item->get_item_rect();
				for ( CanvasItemMap::Element *E = canvas_items.front(); E; E = E->next() ) {
					CanvasItem *it_curr = E->key();
					if ( it_curr == ref_item ) continue;
					Vector2 v = it_curr->get_global_transform().elements[2];
					Rect2 r = it_curr->get_item_rect();
					r.pos.y = ( ref_pos.y + ref_r.size.y / 2 ) - ( v.y + r.size.y / 2 );
					it_curr->edit_set_rect( r );
				}
				viewport->update();
			}
#endif
		} break;

		case SPACE_HORIZONTAL: {
			//space_selected_items< proj_vector2_x, compare_items_x >();
		} break;

		case SPACE_VERTICAL: {
			//space_selected_items< proj_vector2_y, compare_items_y >();
		} break;
		case ANCHOR_ALIGN_TOP_LEFT: {
			_set_anchor(ANCHOR_BEGIN,ANCHOR_BEGIN,ANCHOR_BEGIN,ANCHOR_BEGIN);
		} break;
		case ANCHOR_ALIGN_TOP_RIGHT: {
			_set_anchor(ANCHOR_END,ANCHOR_BEGIN,ANCHOR_END,ANCHOR_BEGIN);
		} break;
		case ANCHOR_ALIGN_BOTTOM_LEFT: {
			_set_anchor(ANCHOR_BEGIN,ANCHOR_END,ANCHOR_BEGIN,ANCHOR_END);
		} break;
		case ANCHOR_ALIGN_BOTTOM_RIGHT: {
			_set_anchor(ANCHOR_END,ANCHOR_END,ANCHOR_END,ANCHOR_END);
		} break;
		case ANCHOR_ALIGN_CENTER_LEFT: {
			_set_anchor(ANCHOR_BEGIN,ANCHOR_CENTER,ANCHOR_BEGIN,ANCHOR_CENTER);
		} break;
		case ANCHOR_ALIGN_CENTER_RIGHT: {

			_set_anchor(ANCHOR_END,ANCHOR_CENTER,ANCHOR_END,ANCHOR_CENTER);
		} break;
		case ANCHOR_ALIGN_CENTER_TOP: {
			_set_anchor(ANCHOR_CENTER,ANCHOR_BEGIN,ANCHOR_CENTER,ANCHOR_BEGIN);
		} break;
		case ANCHOR_ALIGN_CENTER_BOTTOM: {
			_set_anchor(ANCHOR_CENTER,ANCHOR_END,ANCHOR_CENTER,ANCHOR_END);
		} break;
		case ANCHOR_ALIGN_CENTER: {
			_set_anchor(ANCHOR_CENTER,ANCHOR_CENTER,ANCHOR_CENTER,ANCHOR_CENTER);
		} break;
		case ANCHOR_ALIGN_TOP_WIDE: {
			_set_anchor(ANCHOR_BEGIN,ANCHOR_BEGIN,ANCHOR_END,ANCHOR_BEGIN);
		} break;
		case ANCHOR_ALIGN_LEFT_WIDE: {
			_set_anchor(ANCHOR_BEGIN,ANCHOR_BEGIN,ANCHOR_BEGIN,ANCHOR_END);
		} break;
		case ANCHOR_ALIGN_RIGHT_WIDE: {
			_set_anchor(ANCHOR_END,ANCHOR_BEGIN,ANCHOR_END,ANCHOR_END);
		} break;
		case ANCHOR_ALIGN_BOTTOM_WIDE: {
			_set_anchor(ANCHOR_BEGIN,ANCHOR_END,ANCHOR_END,ANCHOR_END);
		} break;
		case ANCHOR_ALIGN_VCENTER_WIDE: {
			_set_anchor(ANCHOR_CENTER,ANCHOR_BEGIN,ANCHOR_CENTER,ANCHOR_END);
		} break;
		case ANCHOR_ALIGN_HCENTER_WIDE: {
			_set_anchor(ANCHOR_BEGIN,ANCHOR_CENTER,ANCHOR_END,ANCHOR_CENTER);
		} break;
		case ANCHOR_ALIGN_WIDE: {
			_set_anchor(ANCHOR_BEGIN,ANCHOR_BEGIN,ANCHOR_END,ANCHOR_END);
		} break;

		case ANIM_INSERT_KEY:
		case ANIM_INSERT_KEY_EXISTING: {

			bool existing = p_op==ANIM_INSERT_KEY_EXISTING;

			Map<Node*,Object*> &selection = editor_selection->get_selection();

			for(Map<Node*,Object*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->key()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;

				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;

				if (canvas_item->cast_to<Node2D>()) {
					Node2D *n2d = canvas_item->cast_to<Node2D>();

					if (key_pos)
						AnimationPlayerEditor::singleton->get_key_editor()->insert_node_value_key(n2d,"transform/pos",n2d->get_pos(),existing);
					if (key_rot)
						AnimationPlayerEditor::singleton->get_key_editor()->insert_node_value_key(n2d,"transform/rot",Math::rad2deg(n2d->get_rot()),existing);
					if (key_scale)
						AnimationPlayerEditor::singleton->get_key_editor()->insert_node_value_key(n2d,"transform/scale",n2d->get_scale(),existing);


					if (n2d->has_meta("_edit_bone_") && n2d->get_parent_item()) {
						//look for an IK chain
						List<Node2D*> ik_chain;

						Node2D *n = n2d->get_parent_item()->cast_to<Node2D>();
						bool has_chain=false;

						while(n) {

							ik_chain.push_back(n);
							if (n->has_meta("_edit_ik_")) {
								has_chain=true;
								break;
							}

							if (!n->get_parent_item())
								break;
							n=n->get_parent_item()->cast_to<Node2D>();
						}

						if (has_chain && ik_chain.size()) {

							for(List<Node2D*>::Element *F=ik_chain.front();F;F=F->next()) {

								if (key_pos)
									AnimationPlayerEditor::singleton->get_key_editor()->insert_node_value_key(F->get(),"transform/pos",F->get()->get_pos(),existing);
								if (key_rot)
									AnimationPlayerEditor::singleton->get_key_editor()->insert_node_value_key(F->get(),"transform/rot",Math::rad2deg(F->get()->get_rot()),existing);
								if (key_scale)
									AnimationPlayerEditor::singleton->get_key_editor()->insert_node_value_key(F->get(),"transform/scale",F->get()->get_scale(),existing);


							}
						}
					}

				} else if (canvas_item->cast_to<Control>()) {

					Control *ctrl = canvas_item->cast_to<Control>();

					if (key_pos)
						AnimationPlayerEditor::singleton->get_key_editor()->insert_node_value_key(ctrl,"rect/pos",ctrl->get_pos(),existing);
					if (key_scale)
						AnimationPlayerEditor::singleton->get_key_editor()->insert_node_value_key(ctrl,"rect/size",ctrl->get_size(),existing);
				}

			}

		} break;
		case ANIM_INSERT_POS: {

			key_pos = key_loc_button->is_pressed();
		} break;
		case ANIM_INSERT_ROT: {

			key_rot = key_rot_button->is_pressed();
		} break;
		case ANIM_INSERT_SCALE: {

			key_scale = key_scale_button->is_pressed();
		} break;
			/*
		case ANIM_INSERT_POS_ROT
		case ANIM_INSERT_POS_SCALE:
		case ANIM_INSERT_ROT_SCALE:
		case ANIM_INSERT_POS_ROT_SCALE: {

			static const bool key_toggles[7][3]={
				{true,false,false},
				{false,true,false},
				{false,false,true},
				{true,true,false},
				{true,false,true},
				{false,true,true},
				{true,true,true}
			};
			key_pos=key_toggles[p_op-ANIM_INSERT_POS][0];
			key_rot=key_toggles[p_op-ANIM_INSERT_POS][1];
			key_scale=key_toggles[p_op-ANIM_INSERT_POS][2];

			for(int i=ANIM_INSERT_POS;i<=ANIM_INSERT_POS_ROT_SCALE;i++) {
				int idx = animation_menu->get_popup()->get_item_index(i);
				animation_menu->get_popup()->set_item_checked(idx,i==p_op);
			}

		} break;*/
		case ANIM_COPY_POSE: {

			pose_clipboard.clear();;


			Map<Node*,Object*> &selection = editor_selection->get_selection();

			for(Map<Node*,Object*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->key()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;

				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;


				if (canvas_item->cast_to<Node2D>()) {

					Node2D *n2d = canvas_item->cast_to<Node2D>();
					PoseClipboard pc;
					pc.pos=n2d->get_pos();
					pc.rot=n2d->get_rot();
					pc.scale=n2d->get_scale();
					pc.id=n2d->get_instance_ID();
					pose_clipboard.push_back(pc);
				}
			}


		} break;
		case ANIM_PASTE_POSE: {

			if (!pose_clipboard.size())
				break;

			undo_redo->create_action(TTR("Paste Pose"));
			for (List<PoseClipboard>::Element *E=pose_clipboard.front();E;E=E->next()) {

				Object *o = ObjectDB::get_instance(E->get().id);
				if (!o)
					continue;
				Node2D *n2d = o->cast_to<Node2D>();
				if (!n2d)
					continue;
				undo_redo->add_do_method(n2d,"set_pos",E->get().pos);
				undo_redo->add_do_method(n2d,"set_rot",E->get().rot);
				undo_redo->add_do_method(n2d,"set_scale",E->get().scale);
				undo_redo->add_undo_method(n2d,"set_pos",n2d->get_pos());
				undo_redo->add_undo_method(n2d,"set_rot",n2d->get_rot());
				undo_redo->add_undo_method(n2d,"set_scale",n2d->get_scale());
			}
			undo_redo->commit_action();

		} break;
		case ANIM_CLEAR_POSE: {

			Map<Node*,Object*> &selection = editor_selection->get_selection();

			for(Map<Node*,Object*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->key()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;

				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;

				if (canvas_item->cast_to<Node2D>()) {
					Node2D *n2d = canvas_item->cast_to<Node2D>();

					if (key_pos)
						n2d->set_pos(Vector2());
					if (key_rot)
						n2d->set_rot(0);
					if (key_scale)
						n2d->set_scale(Vector2(1,1));
				} else if (canvas_item->cast_to<Control>()) {

					Control *ctrl = canvas_item->cast_to<Control>();

					if (key_pos)
						ctrl->set_pos(Point2());
					//if (key_scale)
					//	AnimationPlayerEditor::singleton->get_key_editor()->insert_node_value_key(ctrl,"rect/size",ctrl->get_size());
				}

			}


		} break;
		case VIEW_CENTER_TO_SELECTION:
		case VIEW_FRAME_TO_SELECTION: {

			Vector2 center(0.f, 0.f);
			Rect2 rect;
			int count = 0;

			Map<Node*,Object*> &selection = editor_selection->get_selection();
			for(Map<Node*,Object*>::Element *E=selection.front();E;E=E->next()) {
				CanvasItem *canvas_item = E->key()->cast_to<CanvasItem>();
				if (!canvas_item) continue;
				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;


				// counting invisible items, for now
				//if (!canvas_item->is_visible()) continue;
				++count;

				Rect2 item_rect = canvas_item->get_item_rect();

				Vector2 pos = canvas_item->get_global_transform().get_origin();
				Vector2 scale = canvas_item->get_global_transform().get_scale();
				real_t angle = canvas_item->get_global_transform().get_rotation();

				Matrix32 t(angle, Vector2(0.f,0.f));
				item_rect = t.xform(item_rect);
				Rect2 canvas_item_rect(pos + scale*item_rect.pos, scale*item_rect.size);
				if (count == 1) {
					rect = canvas_item_rect;
				} else {
					rect = rect.merge(canvas_item_rect);
				}
			};
			if (count==0) break;

			if (p_op == VIEW_CENTER_TO_SELECTION) {

				center = rect.pos + rect.size/2;
				Vector2 offset = viewport->get_size()/2 - editor->get_scene_root()->get_global_canvas_transform().xform(center);
				h_scroll->set_val(h_scroll->get_val() - offset.x/zoom);
				v_scroll->set_val(v_scroll->get_val() - offset.y/zoom);

			} else { // VIEW_FRAME_TO_SELECTION

				if (rect.size.x > CMP_EPSILON && rect.size.y > CMP_EPSILON) {
					float scale_x = viewport->get_size().x/rect.size.x;
					float scale_y = viewport->get_size().y/rect.size.y;
					zoom = scale_x < scale_y? scale_x:scale_y;
					zoom *= 0.90;
					_update_scroll(0);
					call_deferred("_popup_callback", VIEW_CENTER_TO_SELECTION);
				}
			}

		} break;
		case SKELETON_MAKE_BONES: {



			Map<Node*,Object*> &selection = editor_selection->get_selection();

			for(Map<Node*,Object*>::Element *E=selection.front();E;E=E->next()) {

				Node2D *n2d = E->key()->cast_to<Node2D>();
				if (!n2d)
					continue;
				if (!n2d->is_visible())
					continue;
				if (!n2d->get_parent_item())
					continue;

				n2d->set_meta("_edit_bone_",true);

			}
			viewport->update();

		} break;
		case SKELETON_CLEAR_BONES: {

			Map<Node*,Object*> &selection = editor_selection->get_selection();

			for(Map<Node*,Object*>::Element *E=selection.front();E;E=E->next()) {

				Node2D *n2d = E->key()->cast_to<Node2D>();
				if (!n2d)
					continue;
				if (!n2d->is_visible())
					continue;

				n2d->set_meta("_edit_bone_",Variant());

			}
			viewport->update();

		} break;
		case SKELETON_SET_IK_CHAIN: {

			List<Node*> &selection = editor_selection->get_selected_node_list();

			for(List<Node*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *canvas_item = E->get()->cast_to<CanvasItem>();
				if (!canvas_item || !canvas_item->is_visible())
					continue;

				if (canvas_item->get_viewport()!=EditorNode::get_singleton()->get_scene_root())
					continue;

				canvas_item->set_meta("_edit_ik_",true);

			}

			viewport->update();

		} break;
		case SKELETON_CLEAR_IK_CHAIN: {

			Map<Node*,Object*> &selection = editor_selection->get_selection();

			for(Map<Node*,Object*>::Element *E=selection.front();E;E=E->next()) {

				CanvasItem *n2d = E->key()->cast_to<CanvasItem>();
				if (!n2d)
					continue;
				if (!n2d->is_visible())
					continue;

				n2d->set_meta("_edit_ik_",Variant());

			}
			viewport->update();

		} break;

	}
}
#if 0
template< class P, class C > void CanvasItemEditor::space_selected_items() {
	P p;
	if ( canvas_items.size() > 2 ) {
		Vector< CanvasItem * > items;
		for ( CanvasItemMap::Element *E = canvas_items.front(); E; E = E->next() ) {
			CanvasItem *it_curr = E->key();
			items.push_back( it_curr );
		}
		items.sort_custom< C >();

		float width_s = p.get( items[0]->get_item_rect().size );
		float width_e = p.get( items[ items.size() - 1 ]->get_item_rect().size );
		float start_x = p.get( items[0]->get_global_transform().elements[2] ) + ( width_s / 2 );
		float end_x = p.get( items[ items.size() - 1 ]->get_global_transform().elements[2] ) + ( width_e / 2 );
		float sp = ( end_x - start_x ) / ( items.size() - 1 );

		for ( int i = 0; i < items.size(); i++ ) {
			CanvasItem *it_curr = items[i];
			Vector2 v = it_curr->get_global_transform().elements[2];
			Rect2 r = it_curr->get_item_rect();
			p.set( r.pos, ( start_x + sp * i ) - ( p.get( v ) + p.get( r.size ) / 2 ) );
			it_curr->edit_set_rect( r );
		}
		viewport->update();
	}
}
#endif

void CanvasItemEditor::_bind_methods() {

	ObjectTypeDB::bind_method("_node_removed",&CanvasItemEditor::_node_removed);
	ObjectTypeDB::bind_method("_update_scroll",&CanvasItemEditor::_update_scroll);
	ObjectTypeDB::bind_method("_popup_callback",&CanvasItemEditor::_popup_callback);
	ObjectTypeDB::bind_method("_visibility_changed",&CanvasItemEditor::_visibility_changed);
	ObjectTypeDB::bind_method("_dialog_value_changed",&CanvasItemEditor::_dialog_value_changed);
	ObjectTypeDB::bind_method("_get_editor_data",&CanvasItemEditor::_get_editor_data);
	ObjectTypeDB::bind_method("_tool_select",&CanvasItemEditor::_tool_select);
	ObjectTypeDB::bind_method("_keying_changed",&CanvasItemEditor::_keying_changed);
	ObjectTypeDB::bind_method("_unhandled_key_input",&CanvasItemEditor::_unhandled_key_input);
	ObjectTypeDB::bind_method("_viewport_draw",&CanvasItemEditor::_viewport_draw);
	ObjectTypeDB::bind_method("_viewport_input_event",&CanvasItemEditor::_viewport_input_event);
	ObjectTypeDB::bind_method("_snap_changed",&CanvasItemEditor::_snap_changed);
	ObjectTypeDB::bind_method(_MD("_selection_result_pressed"),&CanvasItemEditor::_selection_result_pressed);
	ObjectTypeDB::bind_method(_MD("_selection_menu_hide"),&CanvasItemEditor::_selection_menu_hide);

	ADD_SIGNAL( MethodInfo("item_lock_status_changed") );
	ADD_SIGNAL( MethodInfo("item_group_status_changed") );

}

#if 0
void CanvasItemEditor::end_drag() {
	print_line( "end drag" );

	if (undo_redo) {

		undo_redo->create_action("Edit CanvasItem");
		for(CanvasItemMap::Element *E=canvas_items.front();E;E=E->next()) {
			CanvasItem *canvas_item = E->key();
			Variant state=canvas_item->edit_get_state();
			undo_redo->add_do_method(canvas_item,"edit_set_state",state);
			undo_redo->add_undo_method(canvas_item,"edit_set_state",E->get().undo_state);
		}
		undo_redo->commit_action();
	}

	drag=DRAG_NONE;
	viewport->update();
}

void CanvasItemEditor::box_selection_start( Point2 &click ) {
	print_line( "box selection start" );

	drag_from=transform.affine_inverse().xform(click);

	box_selecting=true;
	box_selecting_to=drag_from;
	viewport->update();
}

bool CanvasItemEditor::box_selection_end() {
	print_line( "box selection end" );

	Node* scene = get_scene()->get_root_node()->cast_to<EditorNode>()->get_edited_scene();
	if (scene) {

		List<CanvasItem*> selitems;

		Point2 bsfrom = transform.xform(drag_from);
		Point2 bsto= transform.xform(box_selecting_to);
		if (bsfrom.x>bsto.x)
			SWAP(bsfrom.x,bsto.x);
		if (bsfrom.y>bsto.y)
			SWAP(bsfrom.y,bsto.y);

		if ( bsfrom.distance_to( bsto ) < 3 ) {
			print_line( "box selection too small" );
			box_selecting=false;
			viewport->update();
			return false;
		}

		_find_canvas_items_at_rect(Rect2(bsfrom,bsto-bsfrom),scene,transform,&selitems);

		for(List<CanvasItem*>::Element *E=selitems.front();E;E=E->next()) {

			_append_canvas_item(E->get());
		}

	}

	box_selecting=false;
	viewport->update();

	return true;
}
#endif

void CanvasItemEditor::add_control_to_menu_panel(Control *p_control) {

	hb->add_child(p_control);
}

HSplitContainer *CanvasItemEditor::get_palette_split() {

	return palette_split;
}

VSplitContainer *CanvasItemEditor::get_bottom_split() {

	return bottom_split;
}

CanvasItemEditor::CanvasItemEditor(EditorNode *p_editor) {

	tool = TOOL_SELECT;
	undo_redo=p_editor->get_undo_redo();
	editor=p_editor;
	editor_selection=p_editor->get_editor_selection();
	editor_selection->add_editor_plugin(this);
	editor_selection->connect("selection_changed",this,"update");


	hb = memnew( HBoxContainer );
	add_child( hb );
	hb->set_area_as_parent_rect();

	bottom_split = memnew( VSplitContainer );
	bottom_split->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(bottom_split);

	palette_split = memnew( HSplitContainer);
	palette_split->set_v_size_flags(SIZE_EXPAND_FILL);
	bottom_split->add_child(palette_split);

	Control *vp_base = memnew (Control);
	vp_base->set_v_size_flags(SIZE_EXPAND_FILL);
	palette_split->add_child(vp_base);

	Control *vp = memnew (Control);
	vp_base->add_child(vp);
	vp->set_area_as_parent_rect();
	vp->add_child(p_editor->get_scene_root());


	viewport = memnew( Control );
	vp_base->add_child(viewport);
	viewport->set_area_as_parent_rect();

	h_scroll = memnew( HScrollBar );
	v_scroll = memnew( VScrollBar );

	viewport->add_child(h_scroll);
	viewport->add_child(v_scroll);
	viewport->connect("draw",this,"_viewport_draw");
	viewport->connect("input_event",this,"_viewport_input_event");


	h_scroll->connect("value_changed", this,"_update_scroll",Vector<Variant>(),true);
	v_scroll->connect("value_changed", this,"_update_scroll",Vector<Variant>(),true);

	h_scroll->hide();
	v_scroll->hide();
	updating_scroll=false;
	viewport->set_focus_mode(FOCUS_ALL);
	handle_len=10;
	first_update=true;


	select_button = memnew( ToolButton );
	select_button->set_toggle_mode(true);
	hb->add_child(select_button);
	select_button->connect("pressed",this,"_tool_select",make_binds(TOOL_SELECT));
	select_button->set_pressed(true);
	select_button->set_tooltip(TTR("Select Mode (Q)")+"\n"+keycode_get_string(KEY_MASK_CMD)+TTR("Drag: Rotate")+"\n"+TTR("Alt+Drag: Move")+"\n"+TTR("Press 'v' to Change Pivot, 'Shift+v' to Drag Pivot (while moving).")+"\n"+TTR("Alt+RMB: Depth list selection"));


	move_button = memnew( ToolButton );
	move_button->set_toggle_mode(true);
	hb->add_child(move_button);
	move_button->connect("pressed",this,"_tool_select",make_binds(TOOL_MOVE));
	move_button->set_tooltip(TTR("Move Mode (W)"));

	rotate_button = memnew( ToolButton );
	rotate_button->set_toggle_mode(true);
	hb->add_child(rotate_button);
	rotate_button->connect("pressed",this,"_tool_select",make_binds(TOOL_ROTATE));
	rotate_button->set_tooltip(TTR("Rotate Mode (E)"));

	hb->add_child(memnew(VSeparator));

	list_select_button = memnew( ToolButton );
	list_select_button->set_toggle_mode(true);
	hb->add_child(list_select_button);
	list_select_button->connect("pressed",this,"_tool_select",make_binds(TOOL_LIST_SELECT));
	list_select_button->set_tooltip(TTR("Show a list of all objects at the position clicked\n(same as Alt+RMB in select mode)."));

	pivot_button = memnew( ToolButton );
	pivot_button->set_toggle_mode(true);
	hb->add_child(pivot_button);
	pivot_button->connect("pressed",this,"_tool_select",make_binds(TOOL_EDIT_PIVOT));
	pivot_button->set_tooltip(TTR("Click to change object's rotation pivot."));

	pan_button = memnew( ToolButton );
	pan_button->set_toggle_mode(true);
	hb->add_child(pan_button);
	pan_button->connect("pressed",this,"_tool_select",make_binds(TOOL_PAN));
	pan_button->set_tooltip(TTR("Pan Mode"));

	hb->add_child(memnew(VSeparator));

	lock_button = memnew( ToolButton );
	hb->add_child(lock_button);

	lock_button->connect("pressed",this,"_popup_callback",varray(LOCK_SELECTED));
	lock_button->set_tooltip(TTR("Lock the selected object in place (can't be moved)."));

	unlock_button = memnew( ToolButton );
	hb->add_child(unlock_button);
	unlock_button->connect("pressed",this,"_popup_callback",varray(UNLOCK_SELECTED));
	unlock_button->set_tooltip(TTR("Unlock the selected object (can be moved)."));

	group_button = memnew( ToolButton );
	hb->add_child(group_button);
	group_button->connect("pressed",this,"_popup_callback",varray(GROUP_SELECTED));
	group_button->set_tooltip(TTR("Makes sure the object's children are not selectable."));

	ungroup_button = memnew( ToolButton );
	hb->add_child(ungroup_button);
	ungroup_button->connect("pressed",this,"_popup_callback",varray(UNGROUP_SELECTED));
	ungroup_button->set_tooltip(TTR("Restores the object's children's ability to be selected."));

	hb->add_child(memnew(VSeparator));

	edit_menu = memnew( MenuButton );
	edit_menu->set_text(TTR("Edit"));
	hb->add_child(edit_menu);
	edit_menu->get_popup()->connect("item_pressed", this,"_popup_callback");

	PopupMenu *p;
	p = edit_menu->get_popup();
	p->add_check_item(TTR("Use Snap"),SNAP_USE);
	p->add_check_item(TTR("Show Grid"),SNAP_SHOW_GRID);
	p->add_check_item(TTR("Use Rotation Snap"),SNAP_USE_ROTATION);
	p->add_check_item(TTR("Snap Relative"),SNAP_RELATIVE);
	p->add_item(TTR("Configure Snap.."),SNAP_CONFIGURE);
	p->add_separator();
	p->add_check_item(TTR("Use Pixel Snap"),SNAP_USE_PIXEL);
	p->add_separator();
	p->add_item(TTR("Expand to Parent"),EXPAND_TO_PARENT,KEY_MASK_CMD|KEY_P);
	p->add_separator();
	p->add_submenu_item(TTR("Skeleton.."),"skeleton");
	PopupMenu *p2 = memnew(PopupMenu);
	p->add_child(p2);
	p2->set_name("skeleton");
	p2->add_item(TTR("Make Bones"),SKELETON_MAKE_BONES,KEY_MASK_CMD|KEY_MASK_SHIFT|KEY_B);
	p2->add_item(TTR("Clear Bones"),SKELETON_CLEAR_BONES);
	p2->add_separator();
	p2->add_item(TTR("Make IK Chain"),SKELETON_SET_IK_CHAIN);
	p2->add_item(TTR("Clear IK Chain"),SKELETON_CLEAR_IK_CHAIN);
	p2->connect("item_pressed", this,"_popup_callback");


	/*
	p->add_item("Align Horizontal",ALIGN_HORIZONTAL);
	p->add_item("Align Vertical",ALIGN_VERTICAL);
	p->add_item("Space Horizontal",SPACE_HORIZONTAL);
	p->add_item("Space Vertical",SPACE_VERTICAL);*/

	view_menu = memnew( MenuButton );
	view_menu->set_text(TTR("View"));
	hb->add_child(view_menu);
	view_menu->get_popup()->connect("item_pressed", this,"_popup_callback");

	p = view_menu->get_popup();

	p->add_item(TTR("Zoom In"),ZOOM_IN);
	p->add_item(TTR("Zoom Out"),ZOOM_OUT);
	p->add_item(TTR("Zoom Reset"),ZOOM_RESET);
	p->add_item(TTR("Zoom Set.."),ZOOM_SET);
	p->add_separator();
	p->add_item(TTR("Center Selection"), VIEW_CENTER_TO_SELECTION, KEY_F);
	p->add_item(TTR("Frame Selection"), VIEW_FRAME_TO_SELECTION, KEY_MASK_CMD|KEY_F);

	anchor_menu = memnew( MenuButton );
	anchor_menu->set_text(TTR("Anchor"));
	hb->add_child(anchor_menu);
	anchor_menu->get_popup()->connect("item_pressed", this,"_popup_callback");
	anchor_menu->hide();

	//p = anchor_menu->get_popup();



	animation_hb = memnew( HBoxContainer );
	hb->add_child(animation_hb);
	animation_hb->add_child( memnew( VSeparator ));
	animation_hb->hide();

	key_loc_button = memnew( Button("loc"));
	key_loc_button->set_toggle_mode(true);
	key_loc_button->set_pressed(true);
	key_loc_button->set_focus_mode(FOCUS_NONE);
	key_loc_button->add_color_override("font_color",Color(1,0.6,0.6));
	key_loc_button->add_color_override("font_color_pressed",Color(0.6,1,0.6));
	key_loc_button->connect("pressed",this,"_popup_callback",varray(ANIM_INSERT_POS));
	animation_hb->add_child(key_loc_button);
	key_rot_button = memnew( Button("rot"));
	key_rot_button->set_toggle_mode(true);
	key_rot_button->set_pressed(true);
	key_rot_button->set_focus_mode(FOCUS_NONE);
	key_rot_button->add_color_override("font_color",Color(1,0.6,0.6));
	key_rot_button->add_color_override("font_color_pressed",Color(0.6,1,0.6));
	key_rot_button->connect("pressed",this,"_popup_callback",varray(ANIM_INSERT_ROT));
	animation_hb->add_child(key_rot_button);
	key_scale_button = memnew( Button("scl"));
	key_scale_button->set_toggle_mode(true);
	key_scale_button->set_focus_mode(FOCUS_NONE);
	key_scale_button->add_color_override("font_color",Color(1,0.6,0.6));
	key_scale_button->add_color_override("font_color_pressed",Color(0.6,1,0.6));
	key_scale_button->connect("pressed",this,"_popup_callback",varray(ANIM_INSERT_SCALE));
	animation_hb->add_child(key_scale_button);
	key_insert_button = memnew( Button );
	key_insert_button->set_focus_mode(FOCUS_NONE);
	key_insert_button->connect("pressed",this,"_popup_callback",varray(ANIM_INSERT_KEY));
	key_insert_button->set_tooltip(TTR("Insert Keys (Ins)"));

	animation_hb->add_child(key_insert_button);

	animation_menu = memnew( MenuButton );
	animation_menu->set_text("Animation");
	animation_hb->add_child(animation_menu);
	animation_menu->get_popup()->connect("item_pressed", this,"_popup_callback");

	p = animation_menu->get_popup();

	p->add_item(TTR("Insert Key"),ANIM_INSERT_KEY,KEY_INSERT);
	p->add_item(TTR("Insert Key (Existing Tracks)"),ANIM_INSERT_KEY_EXISTING,KEY_MASK_CMD+KEY_INSERT);
	p->add_separator();
	p->add_item(TTR("Copy Pose"),ANIM_COPY_POSE);
	p->add_item(TTR("Paste Pose"),ANIM_PASTE_POSE);
	p->add_item(TTR("Clear Pose"),ANIM_CLEAR_POSE,KEY_MASK_SHIFT|KEY_K);

	snap_dialog = memnew( SnapDialog );
	snap_dialog->connect("confirmed",this,"_snap_changed");
	add_child(snap_dialog);

	value_dialog = memnew( AcceptDialog );
	value_dialog->set_title(TTR("Set a Value"));
	value_dialog->get_ok()->set_text(TTR("Close"));
	add_child(value_dialog);

	Label *l = memnew(Label);
	l->set_text(TTR("Snap (Pixels):"));
	l->set_pos(Point2(5,5));
	value_dialog->add_child(l);
	dialog_label=l;

	dialog_val=memnew(SpinBox);
	dialog_val->set_anchor(MARGIN_RIGHT,ANCHOR_END);
	dialog_val->set_begin(Point2(15,25));
	dialog_val->set_end(Point2(10,25));
	value_dialog->add_child(dialog_val);
	dialog_val->connect("value_changed",this,"_dialog_value_changed");
	select_sb = Ref<StyleBoxTexture>( memnew( StyleBoxTexture) );

	selection_menu = memnew( PopupMenu );
	add_child(selection_menu);
	selection_menu->set_custom_minimum_size(Vector2(100, 0));
	selection_menu->connect("item_pressed", this, "_selection_result_pressed");
	selection_menu->connect("popup_hide", this, "_selection_menu_hide");

	key_pos=true;
	key_rot=true;
	key_scale=false;

	zoom=1;
	snap_offset=Vector2(0, 0);
	snap_step=Vector2(10, 10);
	snap_rotation_offset=0;
	snap_rotation_step=15 / (180 / Math_PI);
	snap_grid=false;
	snap_show_grid=false;
	snap_rotation=false;
	snap_pixel=false;
	updating_value_dialog=false;
	box_selecting=false;
	//zoom=0.5;
	singleton=this;

	set_process_unhandled_key_input(true);
	can_move_pivot=false;
	drag=DRAG_NONE;
	bone_last_frame=0;
	additive_selection=false;
}

CanvasItemEditor *CanvasItemEditor::singleton=NULL;

void CanvasItemEditorPlugin::edit(Object *p_object) {

	canvas_item_editor->set_undo_redo(&get_undo_redo());
	canvas_item_editor->edit(p_object->cast_to<CanvasItem>());
}

bool CanvasItemEditorPlugin::handles(Object *p_object) const {

	return p_object->is_type("CanvasItem");
}

void CanvasItemEditorPlugin::make_visible(bool p_visible) {

	if (p_visible) {
		canvas_item_editor->show();
		canvas_item_editor->set_fixed_process(true);
		VisualServer::get_singleton()->viewport_set_hide_canvas(editor->get_scene_root()->get_viewport(),false);
		canvas_item_editor->viewport->grab_focus();

	} else {

		canvas_item_editor->hide();
		canvas_item_editor->set_fixed_process(false);
		VisualServer::get_singleton()->viewport_set_hide_canvas(editor->get_scene_root()->get_viewport(),true);
	}

}

Dictionary CanvasItemEditorPlugin::get_state() const {

	return canvas_item_editor->get_state();
}
void CanvasItemEditorPlugin::set_state(const Dictionary& p_state) {

	canvas_item_editor->set_state(p_state);
}

CanvasItemEditorPlugin::CanvasItemEditorPlugin(EditorNode *p_node) {

	editor=p_node;
	canvas_item_editor = memnew( CanvasItemEditor(editor) );
	canvas_item_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	editor->get_viewport()->add_child(canvas_item_editor);
	canvas_item_editor->set_area_as_parent_rect();
	canvas_item_editor->hide();

}


CanvasItemEditorPlugin::~CanvasItemEditorPlugin()
{
}



#include "navigation_mesh_editor_plugin.h"
#include "scene/gui/box_container.h"
#include "scene/3d/mesh_instance.h"
#include "io/marshalls.h"
#include "io/resource_saver.h"

#ifdef RECAST_ENABLED

void NavigationMeshEditor::_node_removed(Node *p_node) {

	if(p_node==node) {
		node=NULL;

		hide();
	}
}

void NavigationMeshEditor::_notification(int p_option) {


	if (p_option==NOTIFICATION_ENTER_TREE) {

		button_bake->set_icon(get_icon("Bake","EditorIcons"));
		button_reset->set_icon(get_icon("Reload","EditorIcons"));
	}
}

void NavigationMeshEditor::_menu_option(int p_option) {
	switch(p_option) {
		case MENU_OPTION_BAKE: {
			ERR_FAIL_COND(!node);
			ERR_FAIL_COND(node->get_navigation_mesh().is_null());
			baker->bake(node->get_navigation_mesh(),node);
		} break;
		case MENU_OPTION_CLEAR: {
		} break;
	}
}

void NavigationMeshEditor::_bake_pressed() {

	ERR_FAIL_COND(!node);
	const String conf_warning = node->get_configuration_warning();
	if (!conf_warning.empty()) {
		err_dialog->set_text(conf_warning);
		err_dialog->popup_centered_minsize();
		button_bake->set_pressed(false);
		return;
	}

	baker->clear(node->get_navigation_mesh());
	baker->bake(node->get_navigation_mesh(),node);

	if (node) {
		node->update_gizmo();
	}
}

void NavigationMeshEditor::_clear_pressed(){

	if(node)
		baker->clear(node->get_navigation_mesh());

	button_bake->set_pressed(false);
	bake_info->set_text("");

	if (node) {
		node->update_gizmo();
	}
}

void NavigationMeshEditor::edit(NavigationMeshInstance *p_nav_mesh_instance) {

	if (p_nav_mesh_instance==NULL || node==p_nav_mesh_instance) {
		return;
	}

	node=p_nav_mesh_instance;
}

void NavigationMeshEditor::_bind_methods() {

	ObjectTypeDB::bind_method("_menu_option",&NavigationMeshEditor::_menu_option);
	ObjectTypeDB::bind_method("_bake_pressed",&NavigationMeshEditor::_bake_pressed);
	ObjectTypeDB::bind_method("_clear_pressed",&NavigationMeshEditor::_clear_pressed);
}

NavigationMeshEditor::NavigationMeshEditor() {

	bake_hbox = memnew( HBoxContainer );
	button_bake = memnew( ToolButton );
	button_bake->set_text(TTR("Bake!"));
	button_bake->set_toggle_mode(true);
	button_reset = memnew( Button );
	button_bake->set_tooltip(TTR("Bake the navigation mesh.\n"));


	bake_info = memnew( Label );
	bake_hbox->add_child( button_bake );
	bake_hbox->add_child( button_reset );
	bake_hbox->add_child( bake_info );

	err_dialog = memnew( AcceptDialog );
	add_child(err_dialog);
	node=NULL;
	baker = memnew( NavigationMeshGenerator );

	button_bake->connect("pressed",this,"_bake_pressed");
	button_reset->connect("pressed",this,"_clear_pressed");
	button_reset->set_tooltip(TTR("Clear the navigation mesh."));
}

NavigationMeshEditor::~NavigationMeshEditor() {

	memdelete(baker);
}

void NavigationMeshEditorPlugin::edit(Object *p_object) {

	navigation_mesh_editor->edit(p_object->cast_to<NavigationMeshInstance>());
}

bool NavigationMeshEditorPlugin::handles(Object *p_object) const {

	return p_object->is_type("NavigationMeshInstance");
}

void NavigationMeshEditorPlugin::make_visible(bool p_visible) {

	if (p_visible) {
		navigation_mesh_editor->show();
		navigation_mesh_editor->bake_hbox->show();
	} else {

		navigation_mesh_editor->hide();
		navigation_mesh_editor->bake_hbox->hide();
		navigation_mesh_editor->edit(NULL);
	}

}

NavigationMeshEditorPlugin::NavigationMeshEditorPlugin(EditorNode *p_node) {

	editor=p_node;
	navigation_mesh_editor = memnew( NavigationMeshEditor );
	editor->get_viewport()->add_child(navigation_mesh_editor);
	add_control_to_container(CONTAINER_SPATIAL_EDITOR_MENU, navigation_mesh_editor->bake_hbox);
	navigation_mesh_editor->hide();
	navigation_mesh_editor->bake_hbox->hide();
}


NavigationMeshEditorPlugin::~NavigationMeshEditorPlugin()
{
}

#endif // RECAST_ENABLED

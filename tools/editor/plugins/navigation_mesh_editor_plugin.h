#ifndef NAVIGATION_MESH_GENERATOR_PLUGIN_H
#define NAVIGATION_MESH_GENERATOR_PLUGIN_H

#ifdef RECAST_ENABLED

#include "tools/editor/editor_plugin.h"
#include "tools/editor/editor_node.h"
#include "tools/editor/plugins/navigation_mesh_generator.h"
#include "scene/gui/spin_box.h"


class MeshInstance;

class NavigationMeshEditor : public Control {

	OBJ_TYPE(NavigationMeshEditor, Control );

	NavigationMeshGenerator *baker;
	AcceptDialog *err_dialog;

	HBoxContainer *bake_hbox;
	Button *button_bake;
	Button *button_reset;
	Label *bake_info;

	NavigationMeshInstance *node;

	enum Menu {

		MENU_OPTION_BAKE,
		MENU_OPTION_CLEAR
	};

	void _bake_pressed();
	void _clear_pressed();

	void _menu_option(int);

friend class NavigationMeshEditorPlugin;
protected:
	void _node_removed(Node *p_node);
	static void _bind_methods();
	void _notification(int p_what);
public:
	void edit(NavigationMeshInstance *p_nav_mesh_instance);
	NavigationMeshEditor();
	~NavigationMeshEditor();
};

class NavigationMeshEditorPlugin : public EditorPlugin {

	OBJ_TYPE(NavigationMeshEditorPlugin, EditorPlugin );

	NavigationMeshEditor *navigation_mesh_editor;
	EditorNode *editor;

public:

	virtual String get_name() const { return "NavigationMesh"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_node);
	virtual bool handles(Object *p_node) const;
	virtual void make_visible(bool p_visible);

	NavigationMeshEditorPlugin(EditorNode *p_node);
	~NavigationMeshEditorPlugin();

};

#endif // RECAST_ENABLED
#endif // NAVIGATION_MESH_GENERATOR_PLUGIN_H



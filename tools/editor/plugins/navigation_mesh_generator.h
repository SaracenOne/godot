#ifndef NAVIGATION_MESH_GENERATOR_H
#define NAVIGATION_MESH_GENERATOR_H

#ifdef RECAST_ENABLED

#include "tools/editor/editor_settings.h"
#include "tools/editor/editor_node.h"

#include "scene/3d/mesh_instance.h"
#include "scene/3d/test_cube.h"

#include "scene/3d/navigation_mesh.h"
#include "scene/3d/body_shape.h"
#include "scene/3d/physics_body.h"

#include "scene/resources/box_shape.h"
#include "scene/resources/capsule_shape.h"
#include "scene/resources/concave_polygon_shape.h"
#include "scene/resources/convex_polygon_shape.h"
#include "scene/resources/sphere_shape.h"

#include "scene/resources/shape.h"
#include "os/thread.h"

#include <Recast.h>
//#include "RecastAlloc.h"
//#include "RecastAssert.h"

class NavigationMeshGenerator {
protected:
	Ref<NavigationMesh> navigation_mesh;

	static void _add_vertex(const Vector3 &p_vec3, Vector<float> &p_verticies);
	static void _add_test_cube(const TestCube *p_test_cube, const Transform& p_xform, Vector<float> &p_verticies, Vector<int> &p_indices);
	static void _add_mesh(const Ref<Mesh>& p_mesh, const Transform& p_xform, Vector<float> &p_verticies, Vector<int> &p_indices);
	static void _parse_geometry(const Transform &p_base_inverse, Node* p_node, Vector<float> &p_verticies, Vector<int> &p_indices);

	static void _convert_detail_mesh_to_native_navigation_mesh(const rcPolyMeshDetail *p_detail_mesh, Ref<NavigationMesh> p_nav_mesh);
	static void _build_recast_navigation_mesh(Ref<NavigationMesh> p_nav_mesh, EditorProgress *ep,
		rcHeightfield * hf, rcCompactHeightfield * chf, rcContourSet * cset, rcPolyMesh * poly_mesh,
		rcPolyMeshDetail * detail_mesh, Vector<float> &verticies, Vector<int> &indices);
public:
	void bake(Ref<NavigationMesh> p_nav_mesh, Node*p_base);
	void clear(Ref<NavigationMesh> p_nav_mesh);

	Ref<NavigationMesh> get_navigation_mesh() { return navigation_mesh; }

	NavigationMeshGenerator();
	~NavigationMeshGenerator();
};

#endif // RECAST_ENABLED
#endif // NAVIGATION_MESH_GENERATOR_H

/*************************************************************************/
/*  spatial_canvas.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2018 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2018 Godot Engine contributors (cf. AUTHORS.md)    */
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
#include "spatial_canvas.h"
#include "../main/viewport.h"

Size2 SpatialCanvas::get_size() const {
	return size;
}

void SpatialCanvas::set_size(const Size2 &p_size) {
	size = p_size;
	if (is_inside_tree()) {
		VS::get_singleton()->spatial_canvas_set_size(spatial_canvas, p_size);
		VS::get_singleton()->spatial_canvas_set_aabb(spatial_canvas, AABB(Vector3(0.0, -get_size().y, 0.0), Vector3(get_size().x, get_size().y, 0.0)));
	}

#if TOOLS_ENABLED
	update_gizmo();
#endif
}

void SpatialCanvas::reset_sort_index() {
	sort_index = 0;
}

int SpatialCanvas::get_sort_index() {
	return sort_index++;
}

Ref<World2D> SpatialCanvas::get_world_2d() const {
	return world_2d;
}

void SpatialCanvas::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_size"), &SpatialCanvas::get_size);
	ClassDB::bind_method(D_METHOD("set_size"), &SpatialCanvas::set_size);

	ADD_GROUP("Rect", "rect_");
	ADD_PROPERTYNZ(PropertyInfo(Variant::VECTOR2, "rect_size", PROPERTY_HINT_NONE, ""), "set_size", "get_size");
}

void SpatialCanvas::_notification(int p_what) {

	switch (p_what) {

		case NOTIFICATION_ENTER_TREE: {
			VS::get_singleton()->spatial_canvas_set_canvas(spatial_canvas, current_canvas);
			VS::get_singleton()->spatial_canvas_set_size(spatial_canvas, Size2());
			VS::get_singleton()->spatial_canvas_set_aabb(spatial_canvas, AABB(Vector3(0.0, -get_size().y, 0.0), Vector3(get_size().x, get_size().y, 0.0)));
		} break;
		case NOTIFICATION_EXIT_TREE: {
			VS::get_singleton()->spatial_canvas_set_canvas(spatial_canvas, RID());
		} break;
	}
}

AABB SpatialCanvas::get_aabb() const {
	return AABB(Vector3(0.0, -get_size().y, 0.0), Vector3(get_size().x, get_size().y, 0.0));
}

PoolVector<Face3> SpatialCanvas::get_faces(uint32_t p_usage_flags) const {
	return PoolVector<Face3>();
}

SpatialCanvas::SpatialCanvas() {

	world_2d = Ref<World2D>(memnew(World2D));
	current_canvas = get_world_2d()->get_canvas();

	spatial_canvas = VS::get_singleton()->spatial_canvas_create();
	set_base(spatial_canvas);
}

SpatialCanvas::~SpatialCanvas() {
	VS::get_singleton()->free(spatial_canvas);
}
/*************************************************************************/
/*  spatial_canvas.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2017 Godot Engine contributors (cf. AUTHORS.md)    */
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

Size2 SpatialCanvas::get_size() const {
	return size;
}

void SpatialCanvas::set_size(const Size2 &p_size) {
	size = p_size;

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
	return canvas;
}

void SpatialCanvas::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_size"), &SpatialCanvas::get_size);
	ClassDB::bind_method(D_METHOD("set_size"), &SpatialCanvas::set_size);

	ADD_GROUP("Rect", "rect_");
	ADD_PROPERTYNZ(PropertyInfo(Variant::VECTOR2, "rect_size", PROPERTY_HINT_NONE, ""), "set_size", "get_size");
}

AABB SpatialCanvas::get_aabb() const {
	return AABB(Vector3(0.0, -get_size().y, 0.0), Vector3(get_size().x, get_size().y, 0.0));
}

PoolVector<Face3> SpatialCanvas::get_faces(uint32_t p_usage_flags) const {
	return PoolVector<Face3>();
}

SpatialCanvas::SpatialCanvas() {

	vp = NULL;
	size = Size2(Vector2());
	sort_index = 0;

	canvas = Ref<World2D>(memnew(World2D));
}

SpatialCanvas::~SpatialCanvas() {
}
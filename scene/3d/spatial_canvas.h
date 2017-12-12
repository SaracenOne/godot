/*************************************************************************/
/*  spatial_canvas.h                                                     */
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
#ifndef SPATIAL_CANVAS_H
#define SPATIAL_CANVAS_H

#include "scene/3d/spatial.h"
#include "scene/3d/visual_instance.h"

class SpatialCanvas : public VisualInstance {

	GDCLASS(SpatialCanvas, VisualInstance);

	RID viewport;
	Viewport *vp;

	int sort_index;
	Size2 size;

	Ref<World2D> canvas;

protected:
	static void _bind_methods();

public:
	virtual AABB get_aabb() const;
	virtual PoolVector<Face3> get_faces(uint32_t p_usage_flags) const;

	Size2 get_size() const;
	void set_size(const Size2 &p_size);

	void reset_sort_index();
	int get_sort_index();

	Ref<World2D> get_world_2d() const;

	SpatialCanvas();
	~SpatialCanvas();
};

#endif // SPATIAL_CANVAS_H

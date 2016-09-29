/*************************************************************************/
/*  morph_data.h                                                         */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2015 Juan Linietsky, Ariel Manzur.                 */
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
#ifndef MORPH_DATA_H
#define MOPRH_DATA_H

#include "scene/resources/mesh.h"

class MorphDataBuffer;

class MorphData : public Resource  {

	OBJ_TYPE(MorphData,Resource);
	RES_BASE_EXTENSION("mor");
	OBJ_SAVE_TYPE(MoprhData);

public:
	enum MorphType
	{
		MORPH_MODE_RELATIVE,
		MORPH_MODE_NORMALIZED
	};
	
	enum MorphArrayType {

		MORPH_ARRAY_VERTEX=VisualServer::MORPH_ARRAY_VERTEX,
		MORPH_ARRAY_INDEX=VisualServer::MORPH_ARRAY_INDEX,
		MORPH_ARRAY_MAX=VisualServer::MORPH_ARRAY_MAX

	};

public:
	Vector<Ref<MorphDataBuffer> > surface_morphs;
protected:

	static void _bind_methods();
public:
	bool _set(const StringName& p_name, const Variant& p_value);
	bool _get(const StringName& p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

	void clear();
	void resize(int p_surface, int p_size);

	void add_surface();

	void set_morph_type(int p_surface, MorphType p_morphType);
	MorphType get_morph_type(int p_surface) const;

	int get_vertex_count(int p_surface) const;
	int get_index_count(int p_surface) const;

	Vector3 get_vertex(int p_surface,int p_idx) const;
	void set_vertex(int p_surface,int p_idx, const Vector3& p_vertex);

	Vector3 get_vertex_normal(int p_surface,int p_idx) const;
	void set_vertex_normal(int p_surface,int p_idx, const Vector3& p_normal);

	int get_index(int p_surface,int p_idx) const;
	void set_index(int p_surface,int p_idx, const int& p_index);

	void set_surface_count(const int p_surface_count);
	int get_surface_count() const;

	MorphData();
	~MorphData();
};

class MorphDataBuffer : public Resource {
	OBJ_TYPE(MorphDataBuffer, Resource);
	RES_BASE_EXTENSION("mdb");
	OBJ_SAVE_TYPE(MorphDataBuffer);

public:
	Vector<Vector3> vertices;
	Vector<Vector3> normals;

	Vector<int> indices;
	MorphData::MorphType morph_type;

	bool _set(const StringName& p_name, const Variant& p_value);
	bool _get(const StringName& p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
};


#endif // MOPRH_DATA_H

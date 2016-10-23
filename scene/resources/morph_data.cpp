/*************************************************************************/
/*  morph_data.cpp                                                       */
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
#include "morph_data.h"

bool MorphData::_set(const StringName& p_name, const Variant& p_value) {

	String sname = p_name;

	if (sname.begins_with("surface_")) {
		int sl = sname.find("/");
		if (sl == -1)
			return false;
		int idx = sname.substr(8, sl - 8).to_int() - 1;
		String what = sname.get_slicec('/', 1);
		if (what == "morph_data_buffer") {
			surface_morphs[idx] = p_value;
			return true;
		}
		else {
			return false;
		}
	}

	return false;
}

bool MorphData::_get(const StringName& p_name, Variant &r_ret) const {

	String sname = p_name;

	if (sname.begins_with("surface_")) {
		int sl = sname.find("/");
		if (sl == -1)
			return false;
		int idx = sname.substr(8, sl - 8).to_int() - 1;
		String what = sname.get_slicec('/', 1);
		if (what == "morph_data_buffer") {
			r_ret = surface_morphs[idx];
			return true;
		}
		else {
			return false;
		}
	}

	return false;
}

void MorphData::_get_property_list(List<PropertyInfo> *p_list) const {

	for (int i = 0; i < surface_morphs.size(); i++) {
		List<PropertyInfo> param_list;

		p_list->push_back(PropertyInfo(Variant::OBJECT, "surface_" + itos(i+1) + "/morph_data_buffer", PROPERTY_HINT_RESOURCE_TYPE, "MorphDataBuffer"));
	}
}

void MorphData::clear() {

	surface_morphs.clear();
}

void MorphData::resize(int p_surface, int p_size) {

	ERR_FAIL_INDEX(p_surface, surface_morphs.size());
	surface_morphs[p_surface]->vertices.resize(p_size);
	surface_morphs[p_surface]->normals.resize(p_size);
	surface_morphs[p_surface]->indices.resize(p_size);
}

void MorphData::add_surface() {

	set_surface_count(get_surface_count() + 1);
}

void MorphData::set_morph_type(int p_surface, MorphData::MorphType p_morphType) {

	ERR_FAIL_INDEX(p_surface, surface_morphs.size());
	surface_morphs[p_surface]->morph_type = p_morphType;
}

MorphData::MorphType MorphData::get_morph_type(int p_surface) const {

	ERR_FAIL_INDEX_V(p_surface, surface_morphs.size(), MORPH_MODE_RELATIVE);
	return surface_morphs[p_surface]->morph_type;
}

int MorphData::get_vertex_count(int p_surface) const {

	ERR_FAIL_INDEX_V(p_surface, surface_morphs.size(), -1);
	return surface_morphs[p_surface]->vertices.size();
}

int MorphData::get_index_count(int p_surface) const {

	ERR_FAIL_INDEX_V(p_surface, surface_morphs.size(), -1);
	return surface_morphs[p_surface]->indices.size();
}

Vector3 MorphData::get_vertex(int p_surface, int p_idx) const {

	ERR_FAIL_INDEX_V(p_surface, surface_morphs.size(), Vector3());
	ERR_FAIL_INDEX_V(p_idx, surface_morphs[p_surface]->vertices.size(), Vector3());
	return surface_morphs[p_surface]->vertices[p_idx];
}

void MorphData::set_vertex(int p_surface, int p_idx, const Vector3& p_vertex){

	ERR_FAIL_INDEX(p_surface, surface_morphs.size());
	ERR_FAIL_INDEX(p_idx, surface_morphs[p_surface]->vertices.size());
	surface_morphs[p_surface]->vertices[p_idx] = p_vertex;
}

Vector3 MorphData::get_vertex_normal(int p_surface, int p_idx) const{

	ERR_FAIL_INDEX_V(p_surface, surface_morphs.size(), Vector3());
	ERR_FAIL_INDEX_V(p_idx, surface_morphs[p_surface]->normals.size(), Vector3());
	return surface_morphs[p_surface]->normals[p_idx];

}
void MorphData::set_vertex_normal(int p_surface, int p_idx, const Vector3& p_normal){

	ERR_FAIL_INDEX(p_surface, surface_morphs.size());
	ERR_FAIL_INDEX(p_idx, surface_morphs[p_surface]->normals.size());
	surface_morphs[p_surface]->normals[p_idx] = p_normal;
}

void MorphData::set_index(int p_surface, int p_idx, const int& p_index){

	ERR_FAIL_INDEX(p_surface, surface_morphs.size());
	ERR_FAIL_INDEX(p_idx, surface_morphs[p_surface]->indices.size());
	surface_morphs[p_surface]->indices[p_idx] = p_index;
}

int MorphData::get_index(int p_surface, int p_idx) const {

	ERR_FAIL_INDEX_V(p_surface, surface_morphs.size(), -1);
	ERR_FAIL_INDEX_V(p_idx, surface_morphs[p_surface]->indices.size(), -1);
	return surface_morphs[p_surface]->indices[p_idx];
}

void MorphData::set_surface_count(const int p_surface_count) {

	surface_morphs.resize(p_surface_count);

	for (int i=0;i<surface_morphs.size();i++) {
		if(surface_morphs[i].is_null()) {
			surface_morphs[i] = Ref<MorphDataBuffer>(memnew(MorphDataBuffer));
		}
	}

	_change_notify();
	emit_changed();
}

int MorphData::get_surface_count() const {

	return surface_morphs.size();
}

void MorphData::_bind_methods() {

	ObjectTypeDB::bind_method(_MD("add_surface"), &MorphData::add_surface);

	ObjectTypeDB::bind_method(_MD("set_surface_count"), &MorphData::set_surface_count);
	ObjectTypeDB::bind_method(_MD("get_surface_count"), &MorphData::get_surface_count);

	ObjectTypeDB::bind_method(_MD("clear"), &MorphData::clear);
	ObjectTypeDB::bind_method(_MD("resize","surface","size"), &MorphData::resize);

	ObjectTypeDB::bind_method(_MD("get_vertex_count"),&MorphData::get_vertex_count);
	ObjectTypeDB::bind_method(_MD("get_index_count"),&MorphData::get_index_count);

	ObjectTypeDB::bind_method(_MD("set_vertex","surface","idx","vertex"),&MorphData::set_vertex);
	ObjectTypeDB::bind_method(_MD("get_vertex", "surface", "idx"), &MorphData::get_vertex);

	ObjectTypeDB::bind_method(_MD("set_vertex_normal","surface", "idx","normal"),&MorphData::set_vertex_normal);
	ObjectTypeDB::bind_method(_MD("get_vertex_normal","surface", "idx"),&MorphData::get_vertex_normal);

	ObjectTypeDB::bind_method(_MD("set_index","surface","idx","index"), &MorphData::set_index);
	ObjectTypeDB::bind_method(_MD("get_index","surface","idx"), &MorphData::get_index);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "surface_count"), _SCS("set_surface_count"), _SCS("get_surface_count"));

	BIND_CONSTANT(MORPH_ARRAY_VERTEX);
	BIND_CONSTANT(MORPH_ARRAY_INDEX);
	BIND_CONSTANT(MORPH_ARRAY_MAX);
}

MorphData::MorphData(){

	clear();
}

MorphData::~MorphData(){

	clear();
}

bool MorphDataBuffer::_set(const StringName& p_name, const Variant& p_value) {
	String sname = p_name;

	if (sname == "vertices") {
		vertices = p_value;
		return true;
	}
	else if (sname == "normals") {
		normals = p_value;
		return true;
	}
	else if (sname == "indices") {
		indices = p_value;
		return true;
	}

	return false;
}

bool MorphDataBuffer::_get(const StringName& p_name, Variant &r_ret) const {
	String sname = p_name;

	if (sname == "vertices") {
		r_ret = vertices;
		return true;
	}
	else if (sname == "normals") {
		r_ret = normals;
		return true;
	}
	else if (sname == "indices") {
		r_ret = indices;
		return true;
	}

	return false;
}

void MorphDataBuffer::_get_property_list(List<PropertyInfo> *p_list) const {

	p_list->push_back(PropertyInfo(Variant::VECTOR3_ARRAY, "vertices"));
	p_list->push_back(PropertyInfo(Variant::VECTOR3_ARRAY, "normals"));
	p_list->push_back(PropertyInfo(Variant::INT_ARRAY, "indices"));
}
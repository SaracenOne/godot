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


void MorphData::clear() {

	for (int i = 0; i < surfaceMorphs.size(); i++)
	{
		surfaceMorphs.get(i)->vertices.clear();
		surfaceMorphs.get(i)->normals.clear();
		surfaceMorphs.get(i)->indexes.clear();
	}

	surfaceMorphs.clear();
}

void MorphData::resize(int p_surface, int p_size) {

	ERR_FAIL_INDEX(p_surface, surfaceMorphs.size());
	surfaceMorphs[p_surface]->vertices.resize(p_size);
	surfaceMorphs[p_surface]->normals.resize(p_size);
	surfaceMorphs[p_surface]->indexes.resize(p_size);
}

void MorphData::add_surface() {

	surfaceMorphs.resize(surfaceMorphs.size() + 1);
	Ref<MorphDataSurface> surface = memnew(MorphDataSurface);
	surfaceMorphs[surfaceMorphs.size() - 1] = surface;
}

void MorphData::set_morph_type(int p_surface, MorphData::MorphType p_morphType) {

	ERR_FAIL_INDEX(p_surface, surfaceMorphs.size());
	surfaceMorphs[p_surface]->morphType = p_morphType;
}

MorphData::MorphType MorphData::get_morph_type(int p_surface) const {

	ERR_FAIL_INDEX_V(p_surface, surfaceMorphs.size(), MORPH_MODE_RELATIVE);
	return surfaceMorphs[p_surface]->morphType;
}

int MorphData::get_vertex_count(int p_surface) const {

	ERR_FAIL_INDEX_V(p_surface, surfaceMorphs.size(), -1);
	return surfaceMorphs[p_surface]->vertices.size();
}

int MorphData::get_index_count(int p_surface) const {

	ERR_FAIL_INDEX_V(p_surface, surfaceMorphs.size(), -1);
	return surfaceMorphs[p_surface]->indexes.size();
}

Vector3 MorphData::get_vertex(int p_surface, int p_idx) const {

	ERR_FAIL_INDEX_V(p_surface, surfaceMorphs.size(), Vector3());
	ERR_FAIL_INDEX_V(p_idx, surfaceMorphs[p_surface]->vertices.size(), Vector3());
	return surfaceMorphs[p_surface]->vertices[p_idx];
}

void MorphData::set_vertex(int p_surface, int p_idx, const Vector3& p_vertex){

	ERR_FAIL_INDEX(p_surface, surfaceMorphs.size());
	ERR_FAIL_INDEX(p_idx, surfaceMorphs[p_surface]->vertices.size());
	surfaceMorphs[p_surface]->vertices[p_idx] = p_vertex;
}

Vector3 MorphData::get_vertex_normal(int p_surface, int p_idx) const{

	ERR_FAIL_INDEX_V(p_surface, surfaceMorphs.size(), Vector3());
	ERR_FAIL_INDEX_V(p_idx, surfaceMorphs[p_surface]->normals.size(), Vector3());
	return surfaceMorphs[p_surface]->normals[p_idx];

}
void MorphData::set_vertex_normal(int p_surface, int p_idx, const Vector3& p_normal){

	ERR_FAIL_INDEX(p_surface, surfaceMorphs.size());
	ERR_FAIL_INDEX(p_idx, surfaceMorphs[p_surface]->normals.size());
	surfaceMorphs[p_surface]->normals[p_idx] = p_normal;
}

void MorphData::set_index(int p_surface, int p_idx, const int& p_index){

	ERR_FAIL_INDEX(p_surface, surfaceMorphs.size());
	ERR_FAIL_INDEX(p_idx, surfaceMorphs[p_surface]->indexes.size());
	surfaceMorphs[p_surface]->indexes[p_idx] = p_index;
}

int MorphData::get_index(int p_surface, int p_idx) const {

	ERR_FAIL_INDEX_V(p_surface, surfaceMorphs.size(), -1);
	ERR_FAIL_INDEX_V(p_idx, surfaceMorphs[p_surface]->indexes.size(), -1);
	return surfaceMorphs[p_surface]->indexes[p_idx];
}

int MorphData::get_surface_count() const {

	return surfaceMorphs.size();
}

void MorphData::_bind_methods() {

	ObjectTypeDB::bind_method(_MD("clear"), &MorphData::clear);
	ObjectTypeDB::bind_method(_MD("resize","size"), &MorphData::resize);

	ObjectTypeDB::bind_method(_MD("get_vertex_count"),&MorphData::get_vertex_count);
	ObjectTypeDB::bind_method(_MD("get_index_count"),&MorphData::get_index_count);

	ObjectTypeDB::bind_method(_MD("set_vertex","surface","idx","vertex"),&MorphData::set_vertex);
	ObjectTypeDB::bind_method(_MD("get_vertex", "surface", "idx"), &MorphData::get_vertex);

	ObjectTypeDB::bind_method(_MD("set_vertex_normal","surface", "idx","normal"),&MorphData::set_vertex_normal);
	ObjectTypeDB::bind_method(_MD("get_vertex_normal","surface", "idx"),&MorphData::get_vertex_normal);

	ObjectTypeDB::bind_method(_MD("set_index","surface","idx","index"), &MorphData::set_index);
	ObjectTypeDB::bind_method(_MD("get_index","surface","idx"), &MorphData::get_index);

	ObjectTypeDB::bind_method(_MD("get_surface_count"), &Mesh::get_surface_count);
}

MorphData::MorphData(){

	clear();
}

MorphData::~MorphData(){

	clear();
}

MorphDataSurface::MorphDataSurface(){

}

MorphDataSurface::~MorphDataSurface(){

}
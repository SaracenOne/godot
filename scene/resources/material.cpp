/*************************************************************************/
/*  material.cpp                                                         */
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
#include "material.h"
#include "scene/scene_string_names.h"


static const char*_flag_names[Material::FLAG_MAX]={
	"visible",
	"double_sided",
	"invert_faces",
	"unshaded",
	"on_top",
	"lightmap_on_uv2",
	"colarray_is_srgb",
	"skip_shadow_casting"
};


static const Material::Flag _flag_indices[Material::FLAG_MAX]={
	Material::FLAG_VISIBLE,
	Material::FLAG_DOUBLE_SIDED,
	Material::FLAG_INVERT_FACES,
	Material::FLAG_UNSHADED,
	Material::FLAG_ONTOP,
	Material::FLAG_LIGHTMAP_ON_UV2,
	Material::FLAG_COLOR_ARRAY_SRGB,
	Material::FLAG_SKIP_SHADOW_CASTING,

};

static const char*_color_mask_bit_names[Material::COLOR_MASK_BIT_COUNT] = {
	"r",
	"g",
	"b",
	"a",
};


static const Material::ColorMaskBit _color_mask_bit_indices[Material::COLOR_MASK_BIT_COUNT] = {
	Material::COLOR_MASK_BIT_R,
	Material::COLOR_MASK_BIT_G,
	Material::COLOR_MASK_BIT_B,
	Material::COLOR_MASK_BIT_A,
};

RID Material::get_rid() const {

	return material;
}

void Material::_bind_methods() {

	ObjectTypeDB::bind_method(_MD("set_flag", "flag", "enable"), &Material::set_flag);
	ObjectTypeDB::bind_method(_MD("get_flag", "flag"), &Material::get_flag);
	ObjectTypeDB::bind_method(_MD("set_blend_mode", "mode"), &Material::set_blend_mode);
	ObjectTypeDB::bind_method(_MD("get_blend_mode"), &Material::get_blend_mode);
	ObjectTypeDB::bind_method(_MD("set_line_width", "width"), &Material::set_line_width);
	ObjectTypeDB::bind_method(_MD("get_line_width"), &Material::get_line_width);
	ObjectTypeDB::bind_method(_MD("set_depth_draw_mode", "mode"), &Material::set_depth_draw_mode);
	ObjectTypeDB::bind_method(_MD("get_depth_draw_mode"), &Material::get_depth_draw_mode);
	ObjectTypeDB::bind_method(_MD("set_depth_test_mode", "mode"), &Material::set_depth_test_mode);
	ObjectTypeDB::bind_method(_MD("get_depth_test_mode"), &Material::get_depth_test_mode);

	BIND_CONSTANT(FLAG_VISIBLE);
	BIND_CONSTANT(FLAG_DOUBLE_SIDED);
	BIND_CONSTANT(FLAG_INVERT_FACES);
	BIND_CONSTANT(FLAG_UNSHADED);
	BIND_CONSTANT(FLAG_ONTOP);
	BIND_CONSTANT(FLAG_LIGHTMAP_ON_UV2);
	BIND_CONSTANT(FLAG_COLOR_ARRAY_SRGB);
	BIND_CONSTANT(FLAG_MAX);

	BIND_CONSTANT(COLOR_MASK_BIT_R);
	BIND_CONSTANT(COLOR_MASK_BIT_G);
	BIND_CONSTANT(COLOR_MASK_BIT_B);
	BIND_CONSTANT(COLOR_MASK_BIT_A);

	BIND_CONSTANT(DEPTH_DRAW_ALWAYS);
	BIND_CONSTANT(DEPTH_DRAW_OPAQUE_ONLY);
	BIND_CONSTANT(DEPTH_DRAW_OPAQUE_PRE_PASS_ALPHA);
	BIND_CONSTANT(DEPTH_DRAW_NEVER);

	BIND_CONSTANT(DEPTH_TEST_MODE_NEVER);
	BIND_CONSTANT(DEPTH_TEST_MODE_LESS);
	BIND_CONSTANT(DEPTH_TEST_MODE_EQUAL);
	BIND_CONSTANT(DEPTH_TEST_MODE_LEQUAL);
	BIND_CONSTANT(DEPTH_TEST_MODE_GREATER);
	BIND_CONSTANT(DEPTH_TEST_MODE_NOTEQUAL);
	BIND_CONSTANT(DEPTH_TEST_MODE_GEQUAL);
	BIND_CONSTANT(DEPTH_TEST_MODE_ALWAYS);

	BIND_CONSTANT(BLEND_MODE_MIX);
	BIND_CONSTANT(BLEND_MODE_ADD);
	BIND_CONSTANT(BLEND_MODE_SUB);
	BIND_CONSTANT(BLEND_MODE_MUL);
	BIND_CONSTANT(BLEND_MODE_PREMULT_ALPHA);

}

Material::Material(const RID& p_material) {

	material = p_material;
}

Material::~Material() {

	VisualServer::get_singleton()->free(material);
}

bool SinglePassMaterial::_set(const StringName& p_name, const Variant& p_value) {
	Vector<String> split_path = String(p_name).split("/");
	if (split_path[0] == "params") {
		if (split_path[1] == "stencil_reference_value") {
			set_stencil_reference_value((int)p_value);
			return true;
		}

		else if (split_path[1] == "stencil_read_mask") {
			set_stencil_read_mask((int)p_value);
			return true;
		}

		else if (split_path[1] == "stencil_write_mask") {
			set_stencil_write_mask((int)p_value);
			return true;
		}

		else if (split_path[1] == "stencil_comparison") {
			set_stencil_comparison((Material::StencilComparison)(int)p_value);
			return true;
		}
		else if (split_path[1] == "stencil_options") {
			if (split_path[2] == "sfail") {
				set_stencil_option(STENCIL_OP_OPTION_SFAIL, (Material::StencilOperation)(int)p_value);
				return true;
			}
			else if (split_path[2] == "dpfail") {
				set_stencil_option(STENCIL_OP_OPTION_DPFAIL, (Material::StencilOperation)(int)p_value);
				return true;
			}
			else if (split_path[2] == "dppass") {
				set_stencil_option(STENCIL_OP_OPTION_DPPASS, (Material::StencilOperation)(int)p_value);
				return true;
			}
		}
	}
	return false;
}

bool SinglePassMaterial::_get(const StringName& p_name, Variant &r_ret) const {
	Vector<String> split_path = String(p_name).split("/");
	if (split_path[0] == "params") {
		if (split_path[1] == "stencil_reference_value") {
			r_ret = get_stencil_reference_value();
			return true;
		}

		else if (split_path[1] == "stencil_read_mask") {
			r_ret = get_stencil_read_mask();
			return true;
		}

		else if (split_path[1] == "stencil_write_mask") {
			r_ret = get_stencil_write_mask();
			return true;
		}

		else if (split_path[1] == "stencil_comparison") {
			r_ret = get_stencil_comparison();
			return true;
		}
		else if (split_path[1] == "stencil_options") {
			if (split_path[2] == "sfail") {
				r_ret = get_stencil_option(STENCIL_OP_OPTION_SFAIL);
				return true;
			}
			else if (split_path[2] == "dpfail") {
				r_ret = get_stencil_option(STENCIL_OP_OPTION_DPFAIL);
				return true;
			}
			else if (split_path[2] == "dppass") {
				r_ret = get_stencil_option(STENCIL_OP_OPTION_DPPASS);
				return true;
			}
		}
	}
	return false;
}

void SinglePassMaterial::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::INT, "params/stencil_reference_value", PROPERTY_HINT_RANGE, "0,255,1"));
	p_list->push_back(PropertyInfo(Variant::INT, "params/stencil_read_mask", PROPERTY_HINT_RANGE, "0,255,1"));
	p_list->push_back(PropertyInfo(Variant::INT, "params/stencil_write_mask", PROPERTY_HINT_RANGE, "0,255,1"));
	p_list->push_back(PropertyInfo(Variant::INT, "params/stencil_comparison", PROPERTY_HINT_ENUM, "Never,Less,Equal,Less-or-Equal,Greater,Not Equal,Greater-or-Equal,Always"));
	p_list->push_back(PropertyInfo(Variant::INT, "params/stencil_options/sfail", PROPERTY_HINT_ENUM, "Keep,Zero,Replace,Increment Saturate,Increment Wrap,Decrement Saturate,Decrement Wrap,Invert"));
	p_list->push_back(PropertyInfo(Variant::INT, "params/stencil_options/dpfail", PROPERTY_HINT_ENUM, "Keep,Zero,Replace,Increment Saturate,Increment Wrap,Decrement Saturate,Decrement Wrap,Invert"));
	p_list->push_back(PropertyInfo(Variant::INT, "params/stencil_options/dppass", PROPERTY_HINT_ENUM, "Keep,Zero,Replace,Increment Saturate,Increment Wrap,Decrement Saturate,Decrement Wrap,Invert"));
}

void SinglePassMaterial::_bind_methods() {
	ObjectTypeDB::bind_method(_MD("set_color_mask_bit", "bit", "enabled"), &SinglePassMaterial::set_color_mask_bit);
	ObjectTypeDB::bind_method(_MD("get_color_mask_bit", "bit"), &SinglePassMaterial::get_color_mask_bit);

	for (int i = 0; i<FLAG_MAX; i++)
		ADD_PROPERTYI(PropertyInfo(Variant::BOOL, String() + "flags/" + _flag_names[i]), _SCS("set_flag"), _SCS("get_flag"), _flag_indices[i]);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "params/blend_mode", PROPERTY_HINT_ENUM, "Mix,Add,Sub,PMAlpha,NoBlend"), _SCS("set_blend_mode"), _SCS("get_blend_mode"));
	ADD_PROPERTY(PropertyInfo(Variant::INT, "params/depth_draw", PROPERTY_HINT_ENUM, "Always,Opaque Only,Pre-Pass Alpha,Never"), _SCS("set_depth_draw_mode"), _SCS("get_depth_draw_mode"));
	ADD_PROPERTY(PropertyInfo(Variant::INT, "params/depth_test", PROPERTY_HINT_ENUM, "Never,Less,Equal,Less-or-Equal,Greater,Not Equal,Greater-or-Equal,Always"), _SCS("set_depth_test_mode"), _SCS("get_depth_test_mode"));
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "params/line_width", PROPERTY_HINT_RANGE, "0.1,32.0,0.1"), _SCS("set_line_width"), _SCS("get_line_width"));

	for (int i=0;i<COLOR_MASK_BIT_COUNT;i++)
		ADD_PROPERTYI(PropertyInfo(Variant::BOOL, String() + "params/color_mask_bit/" + _color_mask_bit_names[i]), _SCS("set_color_mask_bit"), _SCS("get_color_mask_bit"), _color_mask_bit_indices[i]);
}

void SinglePassMaterial::set_flag(Flag p_flag,bool p_enabled) {

	ERR_FAIL_INDEX(p_flag,FLAG_MAX);
	flags[p_flag]=p_enabled;
	VisualServer::get_singleton()->material_set_flag(material,0,(VS::MaterialFlag)p_flag,p_enabled);
	_change_notify();
}


void SinglePassMaterial::set_blend_mode(BlendMode p_blend_mode) {

	ERR_FAIL_INDEX(p_blend_mode,4);
	blend_mode=p_blend_mode;
	VisualServer::get_singleton()->material_set_blend_mode(material,0,(VS::MaterialBlendMode)p_blend_mode);
	_change_notify();
}

Material::BlendMode SinglePassMaterial::get_blend_mode() const {

	return blend_mode;
}


void SinglePassMaterial::set_depth_draw_mode(DepthDrawMode p_depth_draw_mode) {

	depth_draw_mode=p_depth_draw_mode;
	VisualServer::get_singleton()->material_set_depth_draw_mode(material,0,(VS::MaterialDepthDrawMode)p_depth_draw_mode);
}

Material::DepthDrawMode SinglePassMaterial::get_depth_draw_mode() const {

	return depth_draw_mode;
}

void SinglePassMaterial::set_depth_test_mode(DepthTestMode p_depth_test_mode) {

	depth_test_mode = p_depth_test_mode;
	VisualServer::get_singleton()->material_set_depth_test_mode(material, 0, (VS::MaterialDepthTestMode)p_depth_test_mode);
}

Material::DepthTestMode SinglePassMaterial::get_depth_test_mode() const {

	return depth_test_mode;
}

void SinglePassMaterial::set_color_mask_bit(ColorMaskBit p_bit, bool p_enabled) {
	ERR_FAIL_INDEX(p_bit, COLOR_MASK_BIT_COUNT);
	color_mask_bit[p_bit] = p_enabled;
	VisualServer::get_singleton()->material_set_color_mask_bit(material, 0, (VS::MaterialColorMaskBit)p_bit, p_enabled);
}

bool SinglePassMaterial::get_color_mask_bit(ColorMaskBit p_bit) const {
	ERR_FAIL_INDEX_V(p_bit, COLOR_MASK_BIT_COUNT, false);
	return color_mask_bit[p_bit];
}


void SinglePassMaterial::set_stencil_reference_value(uint8_t p_reference_value) {
	stencil_reference_value = p_reference_value;
	VisualServer::get_singleton()->material_set_stencil_reference_value(material, 0, p_reference_value);
}

uint8_t SinglePassMaterial::get_stencil_reference_value() const {
	return stencil_reference_value;
}

void SinglePassMaterial::set_stencil_read_mask(uint8_t p_read_mask) {
	stencil_read_mask = p_read_mask;
	VisualServer::get_singleton()->material_set_stencil_read_mask(material, 0, p_read_mask);
}

uint8_t SinglePassMaterial::get_stencil_read_mask() const {
	return stencil_read_mask;
}

void SinglePassMaterial::set_stencil_write_mask(uint8_t p_write_mask) {
	stencil_write_mask = p_write_mask;
	VisualServer::get_singleton()->material_set_stencil_write_mask(material, 0, p_write_mask);
}

uint8_t SinglePassMaterial::get_stencil_write_mask() const {
	return stencil_write_mask;
}

void SinglePassMaterial::set_stencil_comparison(StencilComparison p_comparison) {
	ERR_FAIL_INDEX(p_comparison, STENCIL_COMPARISON_COUNT);
	stencil_comparison = p_comparison;
	VisualServer::get_singleton()->material_set_stencil_comparison(material, 0, (VS::MaterialStencilComparison)p_comparison);
}

Material::StencilComparison SinglePassMaterial::get_stencil_comparison() const {
	return stencil_comparison;
}

void SinglePassMaterial::set_stencil_option(StencilOption p_option, StencilOperation p_operation) {
	stencil_options[p_option] = p_operation;
	VisualServer::get_singleton()->material_set_stencil_option(material, 0, (VS::MaterialStencilOperationOption)p_option, (VS::MaterialStencilOperation)p_operation);
}

Material::StencilOperation SinglePassMaterial::get_stencil_option(StencilOption p_option) const {
	ERR_FAIL_INDEX_V(p_option, STENCIL_OP_OPTION_COUNT, STENCIL_OPERATION_KEEP);
	return stencil_options[p_option];
}

bool SinglePassMaterial::get_flag(Flag p_flag) const {

	ERR_FAIL_INDEX_V(p_flag,FLAG_MAX,false);
	return flags[p_flag];
}

void SinglePassMaterial::set_line_width(float p_width) {

	line_width=p_width;
	VisualServer::get_singleton()->material_set_line_width(material,0,p_width);
	_change_notify("line_width");
}

float SinglePassMaterial::get_line_width() const {

	return line_width;
}

SinglePassMaterial::SinglePassMaterial(const RID& p_material) : Material(p_material) {

	flags[FLAG_VISIBLE] = true;
	flags[FLAG_DOUBLE_SIDED] = false;
	flags[FLAG_INVERT_FACES] = false;
	flags[FLAG_UNSHADED] = false;
	flags[FLAG_ONTOP] = false;
	flags[FLAG_LIGHTMAP_ON_UV2] = true;
	flags[FLAG_COLOR_ARRAY_SRGB] = false;

	depth_draw_mode = DEPTH_DRAW_OPAQUE_ONLY;
	depth_test_mode = DEPTH_TEST_MODE_LEQUAL;
	blend_mode = BLEND_MODE_MIX;
	line_width = 0.1;

	for (int i=0;i<COLOR_MASK_BIT_COUNT;i++) {
		color_mask_bit[i] = true;
	}

	stencil_reference_value = 0xff;
	stencil_read_mask = 0xff;
	stencil_write_mask = 0xff;
	stencil_comparison = STENCIL_COMPARISON_ALWAYS;
	stencil_options[STENCIL_OP_OPTION_SFAIL] = STENCIL_OPERATION_KEEP;
	stencil_options[STENCIL_OP_OPTION_DPFAIL] = STENCIL_OPERATION_KEEP;
	stencil_options[STENCIL_OP_OPTION_DPPASS] = STENCIL_OPERATION_KEEP;
}

static const char*_param_names[FixedMaterial::PARAM_MAX]={
	"diffuse",
	"detail",
	"specular",
	"emission",
	"specular_exp",
	"glow",
	"normal",
	"shade_param"
};

static const char*_full_param_names[FixedMaterial::PARAM_MAX]={
	"params/diffuse",
	"params/detail",
	"params/specular",
	"params/emission",
	"params/specular_exp",
	"params/glow",
	"params/normal",
	"params/shade_param"
};

static const FixedMaterial::Parameter _param_indices[FixedMaterial::PARAM_MAX]={
	FixedMaterial::PARAM_DIFFUSE,
	FixedMaterial::PARAM_DETAIL,
	FixedMaterial::PARAM_SPECULAR,
	FixedMaterial::PARAM_EMISSION,
	FixedMaterial::PARAM_SPECULAR_EXP,
	FixedMaterial::PARAM_GLOW,
	FixedMaterial::PARAM_NORMAL,
	FixedMaterial::PARAM_SHADE_PARAM,
};

void FixedMaterial::set_parameter(Parameter p_parameter, const Variant& p_value) {

	ERR_FAIL_INDEX(p_parameter,PARAM_MAX);
	if ((p_parameter==PARAM_DIFFUSE || p_parameter==PARAM_SPECULAR || p_parameter==PARAM_EMISSION)) {

		if (p_value.get_type()!=Variant::COLOR) {
			ERR_EXPLAIN(String(_param_names[p_parameter])+" expects Color");
			ERR_FAIL();
		}
	} else {

		if (!p_value.is_num()) {
			ERR_EXPLAIN(String(_param_names[p_parameter])+" expects scalar");
			ERR_FAIL();
		}
	}

	ERR_FAIL_COND( (p_parameter==PARAM_DIFFUSE || p_parameter==PARAM_SPECULAR || p_parameter==PARAM_EMISSION) && p_value.get_type()!=Variant::COLOR );
	ERR_FAIL_COND( p_parameter!=PARAM_SHADE_PARAM && p_parameter!=PARAM_DIFFUSE && p_parameter!=PARAM_DETAIL && p_parameter!=PARAM_SPECULAR && p_parameter!=PARAM_EMISSION && p_value.get_type()!=Variant::REAL && p_value.get_type()!=Variant::INT );

	param[p_parameter]=p_value;

	VisualServer::get_singleton()->fixed_material_set_param(material,(VS::FixedMaterialParam)p_parameter,p_value);

	_change_notify(_full_param_names[p_parameter]);
}

Variant FixedMaterial::get_parameter(Parameter p_parameter) const {
	ERR_FAIL_INDEX_V(p_parameter,PARAM_MAX,Variant());
	return param[p_parameter];
}



void FixedMaterial::set_texture(Parameter p_parameter, Ref<Texture> p_texture) {

	ERR_FAIL_INDEX(p_parameter,PARAM_MAX);

	texture_param[p_parameter]=p_texture;
	VisualServer::get_singleton()->fixed_material_set_texture(material,(VS::FixedMaterialParam)p_parameter,p_texture.is_null()?RID():p_texture->get_rid());

	_change_notify();
}
Ref<Texture> FixedMaterial::get_texture(Parameter p_parameter) const {

	ERR_FAIL_INDEX_V(p_parameter,PARAM_MAX,Ref<Texture>());
	return texture_param[p_parameter];
}

void FixedMaterial::set_texcoord_mode(Parameter p_parameter, TexCoordMode p_mode) {

	ERR_FAIL_INDEX(p_parameter,PARAM_MAX);
	ERR_FAIL_INDEX(p_mode,4);

	if (p_mode==texture_texcoord[p_parameter])
		return;

	texture_texcoord[p_parameter]=p_mode;

	VisualServer::get_singleton()->fixed_material_set_texcoord_mode(material,(VS::FixedMaterialParam)p_parameter,(VS::FixedMaterialTexCoordMode)p_mode);

	_change_notify();
}

FixedMaterial::TexCoordMode FixedMaterial::get_texcoord_mode(Parameter p_parameter) const {

	ERR_FAIL_INDEX_V(p_parameter,PARAM_MAX,TEXCOORD_UV);
	return texture_texcoord[p_parameter];
}

void FixedMaterial::set_light_shader(LightShader p_shader) {

	light_shader=p_shader;
	VS::get_singleton()->fixed_material_set_light_shader(material,VS::FixedMaterialLightShader(p_shader));
}

FixedMaterial::LightShader FixedMaterial::get_light_shader() const {

	return light_shader;
}


void FixedMaterial::set_uv_transform(const Transform& p_transform) {

	uv_transform=p_transform;
	VisualServer::get_singleton()->fixed_material_set_uv_transform(material, p_transform );
	_change_notify();
}

Transform FixedMaterial::get_uv_transform() const {

	return uv_transform;
}





void FixedMaterial::set_fixed_flag(FixedFlag p_flag, bool p_value) {
	ERR_FAIL_INDEX(p_flag,5);
	fixed_flags[p_flag]=p_value;
	VisualServer::get_singleton()->fixed_material_set_flag(material,(VS::FixedMaterialFlags)p_flag,p_value);

}

bool FixedMaterial::get_fixed_flag(FixedFlag p_flag) const {
	ERR_FAIL_INDEX_V(p_flag,5,false);
	return fixed_flags[p_flag];
}

void FixedMaterial::set_point_size(float p_size) {

	ERR_FAIL_COND(p_size<0);
	point_size=p_size;
	VisualServer::get_singleton()->fixed_material_set_point_size(material,p_size);
}

float FixedMaterial::get_point_size() const{


	return point_size;
}


void FixedMaterial::_bind_methods() {
	ObjectTypeDB::bind_method(_MD("set_parameter","param","value"),&FixedMaterial::set_parameter);
	ObjectTypeDB::bind_method(_MD("get_parameter","param"),&FixedMaterial::get_parameter);

	ObjectTypeDB::bind_method(_MD("set_texture","param","texture:Texture"),&FixedMaterial::set_texture);
	ObjectTypeDB::bind_method(_MD("get_texture:Texture","param"),&FixedMaterial::get_texture);


	ObjectTypeDB::bind_method(_MD("set_texcoord_mode","param","mode"),&FixedMaterial::set_texcoord_mode);
	ObjectTypeDB::bind_method(_MD("get_texcoord_mode","param"),&FixedMaterial::get_texcoord_mode);

	ObjectTypeDB::bind_method(_MD("set_fixed_flag","flag","value"),&FixedMaterial::set_fixed_flag);
	ObjectTypeDB::bind_method(_MD("get_fixed_flag","flag"),&FixedMaterial::get_fixed_flag);

	ObjectTypeDB::bind_method(_MD("set_uv_transform","transform"),&FixedMaterial::set_uv_transform);
	ObjectTypeDB::bind_method(_MD("get_uv_transform"),&FixedMaterial::get_uv_transform);

	ObjectTypeDB::bind_method(_MD("set_light_shader","shader"),&FixedMaterial::set_light_shader);
	ObjectTypeDB::bind_method(_MD("get_light_shader"),&FixedMaterial::get_light_shader);

	ObjectTypeDB::bind_method(_MD("set_point_size","size"),&FixedMaterial::set_point_size);
	ObjectTypeDB::bind_method(_MD("get_point_size"),&FixedMaterial::get_point_size);


	ADD_PROPERTYI( PropertyInfo( Variant::BOOL, "fixed_flags/use_alpha" ), _SCS("set_fixed_flag"), _SCS("get_fixed_flag"), FLAG_USE_ALPHA);
	ADD_PROPERTYI( PropertyInfo( Variant::BOOL, "fixed_flags/use_color_array" ), _SCS("set_fixed_flag"), _SCS("get_fixed_flag"), FLAG_USE_COLOR_ARRAY);
	ADD_PROPERTYI( PropertyInfo( Variant::BOOL, "fixed_flags/use_point_size" ), _SCS("set_fixed_flag"), _SCS("get_fixed_flag"), FLAG_USE_POINT_SIZE);
	ADD_PROPERTYI( PropertyInfo( Variant::BOOL, "fixed_flags/discard_alpha" ), _SCS("set_fixed_flag"), _SCS("get_fixed_flag"), FLAG_DISCARD_ALPHA);
	ADD_PROPERTYI( PropertyInfo( Variant::BOOL, "fixed_flags/use_xy_normalmap" ), _SCS("set_fixed_flag"), _SCS("get_fixed_flag"), FLAG_USE_XY_NORMALMAP);
	ADD_PROPERTYI( PropertyInfo( Variant::COLOR, "params/diffuse" ), _SCS("set_parameter"), _SCS("get_parameter"), PARAM_DIFFUSE);
	ADD_PROPERTYI( PropertyInfo( Variant::COLOR, "params/specular", PROPERTY_HINT_COLOR_NO_ALPHA ), _SCS("set_parameter"), _SCS("get_parameter"), PARAM_SPECULAR );
	ADD_PROPERTYI( PropertyInfo( Variant::COLOR, "params/emission", PROPERTY_HINT_COLOR_NO_ALPHA ), _SCS("set_parameter"), _SCS("get_parameter"), PARAM_EMISSION );
	ADD_PROPERTYI( PropertyInfo( Variant::REAL, "params/specular_exp", PROPERTY_HINT_RANGE,"1,64,0.01" ), _SCS("set_parameter"), _SCS("get_parameter"), PARAM_SPECULAR_EXP );
	ADD_PROPERTYI( PropertyInfo( Variant::REAL, "params/detail_mix", PROPERTY_HINT_RANGE,"0,1,0.01" ), _SCS("set_parameter"), _SCS("get_parameter"), PARAM_DETAIL );
	ADD_PROPERTYI( PropertyInfo( Variant::REAL, "params/normal_depth", PROPERTY_HINT_RANGE,"-4,4,0.01" ), _SCS("set_parameter"), _SCS("get_parameter"), PARAM_NORMAL );
	ADD_PROPERTY( PropertyInfo( Variant::INT, "params/shader", PROPERTY_HINT_ENUM,"Lambert,Wrap,Velvet,Toon" ), _SCS("set_light_shader"), _SCS("get_light_shader") );
	ADD_PROPERTYI( PropertyInfo( Variant::REAL, "params/shader_param", PROPERTY_HINT_RANGE,"0,1,0.01" ), _SCS("set_parameter"), _SCS("get_parameter"), PARAM_SHADE_PARAM );
	ADD_PROPERTYI( PropertyInfo( Variant::REAL, "params/glow", PROPERTY_HINT_RANGE,"0,8,0.01" ), _SCS("set_parameter"), _SCS("get_parameter"), PARAM_GLOW );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "params/point_size", PROPERTY_HINT_RANGE,"0,1024,1" ), _SCS("set_point_size"), _SCS("get_point_size"));
	ADD_PROPERTY( PropertyInfo( Variant::TRANSFORM, "uv_xform"), _SCS("set_uv_transform"), _SCS("get_uv_transform") );

	for(int i=0;i<PARAM_MAX;i++) {
		ADD_PROPERTYI( PropertyInfo( Variant::OBJECT, String()+"textures/"+_param_names[i],PROPERTY_HINT_RESOURCE_TYPE,"Texture" ), _SCS("set_texture"), _SCS("get_texture"), _param_indices[i]);
		ADD_PROPERTYI( PropertyInfo( Variant::INT, String()+"textures/"+_param_names[i]+"_tc",PROPERTY_HINT_ENUM,"UV,UV Xform,UV2,Sphere" ), _SCS("set_texcoord_mode"), _SCS("get_texcoord_mode"), _param_indices[i] );
	}


	BIND_CONSTANT( PARAM_DIFFUSE );
	BIND_CONSTANT( PARAM_DETAIL );
	BIND_CONSTANT( PARAM_SPECULAR );
	BIND_CONSTANT( PARAM_EMISSION );
	BIND_CONSTANT( PARAM_SPECULAR_EXP );
	BIND_CONSTANT( PARAM_GLOW );
	BIND_CONSTANT( PARAM_NORMAL );
	BIND_CONSTANT( PARAM_SHADE_PARAM );
	BIND_CONSTANT( PARAM_MAX );

	BIND_CONSTANT( TEXCOORD_UV );
	BIND_CONSTANT( TEXCOORD_UV_TRANSFORM );
	BIND_CONSTANT( TEXCOORD_UV2 );
	BIND_CONSTANT( TEXCOORD_SPHERE );

	BIND_CONSTANT( FLAG_USE_ALPHA );
	BIND_CONSTANT( FLAG_USE_COLOR_ARRAY );
	BIND_CONSTANT( FLAG_USE_POINT_SIZE );
	BIND_CONSTANT( FLAG_DISCARD_ALPHA );

	BIND_CONSTANT( LIGHT_SHADER_LAMBERT );
	BIND_CONSTANT( LIGHT_SHADER_WRAP );
	BIND_CONSTANT( LIGHT_SHADER_VELVET );
	BIND_CONSTANT( LIGHT_SHADER_TOON );

}


FixedMaterial::FixedMaterial() : SinglePassMaterial(VS::get_singleton()->fixed_material_create()) {



	param[PARAM_DIFFUSE]=Color(1,1,1);
	param[PARAM_SPECULAR]=Color(0.0,0.0,0.0);
	param[PARAM_EMISSION]=Color(0.0,0.0,0.0);
	param[PARAM_SPECULAR_EXP]=40;
	param[PARAM_GLOW]=0;
	param[PARAM_NORMAL]=1;
	param[PARAM_SHADE_PARAM]=0.5;
	param[PARAM_DETAIL]=1.0;

	set_flag(FLAG_COLOR_ARRAY_SRGB,true);

	fixed_flags[FLAG_USE_ALPHA]=false;
	fixed_flags[FLAG_USE_COLOR_ARRAY]=false;
	fixed_flags[FLAG_USE_POINT_SIZE]=false;
	fixed_flags[FLAG_USE_XY_NORMALMAP]=false;
	fixed_flags[FLAG_DISCARD_ALPHA]=false;


	for(int i=0;i<PARAM_MAX;i++) {

		texture_texcoord[i]=TEXCOORD_UV;
	}

	light_shader=LIGHT_SHADER_LAMBERT;

	point_size=1.0;
}


FixedMaterial::~FixedMaterial() {

}




bool ShaderMaterial::_set(const StringName& p_name, const Variant& p_value) {

	if (p_name==SceneStringNames::get_singleton()->shader_shader) {
		set_shader(p_value);
		return true;
	} else {

		if (shader.is_valid()) {


			StringName pr = shader->remap_param(p_name);
			if (!pr) {
				String n = p_name;
				if (n.find("param/")==0) { //backwards compatibility
					pr = n.substr(6,n.length());
				}
			}
			if (pr) {
				VisualServer::get_singleton()->material_set_param(material,0,pr,p_value);
				return true;
			}
		}
	}

	return false;
}

bool ShaderMaterial::_get(const StringName& p_name,Variant &r_ret) const {


	if (p_name==SceneStringNames::get_singleton()->shader_shader) {

		r_ret=get_shader();
		return true;
	} else {

		if (shader.is_valid()) {

			StringName pr = shader->remap_param(p_name);
			if (pr) {
				r_ret=VisualServer::get_singleton()->material_get_param(material,0,pr);
				return true;
			}
		}

	}


	return false;
}


void ShaderMaterial::_get_property_list( List<PropertyInfo> *p_list) const {

	p_list->push_back( PropertyInfo( Variant::OBJECT, "shader/shader", PROPERTY_HINT_RESOURCE_TYPE,"MaterialShader,MaterialShaderGraph" ) );

	if (!shader.is_null()) {

		shader->get_param_list(p_list);
	}

}


void ShaderMaterial::_shader_changed() {

	_change_notify(); //also all may have changed then
}

void ShaderMaterial::set_shader(const Ref<Shader>& p_shader) {

	ERR_FAIL_COND(p_shader.is_valid() && p_shader->get_mode()!=Shader::MODE_MATERIAL);

	if (shader.is_valid())
		shader->disconnect(SceneStringNames::get_singleton()->changed,this,SceneStringNames::get_singleton()->_shader_changed);
	shader=p_shader;
	VS::get_singleton()->material_set_shader(material,0,shader.is_valid()?shader->get_rid():RID());
	if (shader.is_valid()) {
		shader->connect(SceneStringNames::get_singleton()->changed,this,SceneStringNames::get_singleton()->_shader_changed);
	}
	_change_notify();

}

Ref<Shader> ShaderMaterial::get_shader() const {

	return shader;
}


void ShaderMaterial::set_shader_param(const StringName& p_param,const Variant& p_value) {

	VisualServer::get_singleton()->material_set_param(material,0,p_param,p_value);

}

Variant ShaderMaterial::get_shader_param(const StringName& p_param) const{

	return VisualServer::get_singleton()->material_get_param(material,0,p_param);
}



void ShaderMaterial::_bind_methods() {
	ObjectTypeDB::bind_method(_MD("set_shader","shader:Shader"), &ShaderMaterial::set_shader );
	ObjectTypeDB::bind_method(_MD("get_shader:Shader"), &ShaderMaterial::get_shader );

	ObjectTypeDB::bind_method(_MD("set_shader_param","param","value:Variant"), &ShaderMaterial::set_shader_param);
	ObjectTypeDB::bind_method(_MD("get_shader_param:Variant","param"), &ShaderMaterial::get_shader_param);

	ObjectTypeDB::bind_method(_MD("_shader_changed"), &ShaderMaterial::_shader_changed );
}


void ShaderMaterial::get_argument_options(const StringName& p_function,int p_idx,List<String>*r_options) const {

	String f = p_function.operator String();
	if ((f=="get_shader_param" || f=="set_shader_param") && p_idx==0) {

		if (shader.is_valid()) {
			List<PropertyInfo> pl;
			shader->get_param_list(&pl);
			for (List<PropertyInfo>::Element *E=pl.front();E;E=E->next()) {
				r_options->push_back("\""+E->get().name.replace_first("shader_param/","")+"\"");
			}
		}
	}
	Material::get_argument_options(p_function,p_idx,r_options);
}

ShaderMaterial::ShaderMaterial() :SinglePassMaterial(VisualServer::get_singleton()->material_create(1)){


}

bool MultiPassMaterial::_set(const StringName& p_name, const Variant& p_value) {
	Vector<String> split_path = String(p_name).split("/");

	int pass_index = split_path.get(1).to_int();
	if (pass_index < passes.size()) {
		if (p_name == ("passes/" + itos(pass_index) + "/shader/shader")) {
			set_shader(pass_index, p_value);
			return true;
		}
		else if (split_path[0] == "passes") {
			if (split_path[2] == "flags") {
				for (int i = 0; i < FLAG_MAX; i++) {
					if (split_path[3] == _flag_names[i]) {
						set_pass_flag(pass_index, _flag_indices[i], p_value);
						return true;
					}
				}
			}
			else if (split_path[2] == "params") {
				if (split_path[3] == "blend_mode") {
					set_pass_blend_mode(pass_index, (Material::BlendMode)(int)p_value);
					return true;
				}
				else if (split_path[3] == "depth_draw") {
					set_pass_depth_draw_mode(pass_index, (Material::DepthDrawMode)(int)p_value);
					return true;
				}
				else if (split_path[3] == "depth_test") {
					set_pass_depth_test_mode(pass_index, (Material::DepthTestMode)(int)p_value);
					return true;
				}
				else if (split_path[3] == "line_width") {
					set_pass_line_width(pass_index, p_value);
					return true;
				}
				else if (split_path[3] == "color_mask_bit") {
					for (int i=0;i<COLOR_MASK_BIT_COUNT;i++) {
						if (split_path[4] == _color_mask_bit_names[i]) {
							set_pass_color_mask_bit(pass_index, _color_mask_bit_indices[i], p_value);
							return true;
						}
					}
				}

				else if (split_path[3] == "stencil_reference_value") {
					set_pass_stencil_reference_value(pass_index,(int)p_value);
					return true;
				}

				else if (split_path[3] == "stencil_read_mask") {
					set_pass_stencil_read_mask(pass_index,(int)p_value);
					return true;
				}

				else if (split_path[3] == "stencil_write_mask") {
					set_pass_stencil_write_mask(pass_index,(int)p_value);
					return true;
				}

				else if (split_path[3] == "stencil_comparison") {
					set_pass_stencil_comparison(pass_index,(Material::StencilComparison)(int)p_value);
					return true;
				}
				else if (split_path[3] == "stencil_options") {
					if (split_path[4] == "sfail") {
						set_pass_stencil_option(pass_index, STENCIL_OP_OPTION_SFAIL,(Material::StencilOperation)(int)p_value);
						return true;
					}
					else if (split_path[4] == "dpfail") {
						set_pass_stencil_option(pass_index, STENCIL_OP_OPTION_DPFAIL,(Material::StencilOperation)(int)p_value);
						return true;
					}
					else if (split_path[4] == "dppass") {
						set_pass_stencil_option(pass_index, STENCIL_OP_OPTION_DPPASS, (Material::StencilOperation)(int)p_value);
						return true;
					}
				}
			}
			else {
				Ref<Shader> shader = get_shader(pass_index);
				if (shader.is_valid()) {
					StringName param_name = split_path[2] + "/" + split_path[3];

					StringName pr = shader->remap_param(param_name);
					if (!pr) {
						String n = param_name;
						if (n.find("param/") == 0) { //backwards compatibility
							pr = n.substr(6, n.length());
						}
					}
					if (pr) {
						VisualServer::get_singleton()->material_set_param(material, pass_index, pr, p_value);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool MultiPassMaterial::_get(const StringName& p_name, Variant &r_ret) const {
	Vector<String> split_path = String(p_name).split("/");
	if (split_path.size() == 4)
	{
		int pass_index = split_path.get(1).to_int();
		if (pass_index < passes.size()) {
			if (p_name == ("passes/" + itos(pass_index) + "/shader/shader")) {
				r_ret = get_shader(pass_index);
				return true;
			}
			else if (split_path[0] == "passes") {
				if (split_path[2] == "flags") {
					for (int i = 0; i < FLAG_MAX; i++) {
						if (split_path[3] == _flag_names[i]) {
							r_ret = get_pass_flag(pass_index, _flag_indices[i]);
							return true;
						}
					}
				}
				else if (split_path[2] == "params") {
					if (split_path[3] == "blend_mode") {
						r_ret = get_pass_blend_mode(pass_index);
						return true;
					}
					else if (split_path[3] == "depth_draw") {
						r_ret = get_pass_depth_draw_mode(pass_index);
						return true;
					}
					else if (split_path[3] == "depth_test") {
						r_ret = get_pass_depth_test_mode(pass_index);
						return true;
					}
					else if (split_path[3] == "line_width") {
						r_ret = get_pass_line_width(pass_index);
						return true;
					}
					else if (split_path[3] == "color_mask_bit") {
						for (int i=0;i<COLOR_MASK_BIT_COUNT; i++) {
							if (split_path[4] == _color_mask_bit_names[i]) {
								r_ret = get_pass_color_mask_bit(pass_index, _color_mask_bit_indices[i]);
								return true;
							}
						}
					}

					else if (split_path[3] == "stencil_reference_value") {
						r_ret = get_pass_stencil_reference_value(pass_index);
						return true;
					}

					else if (split_path[3] == "stencil_read_mask") {
						r_ret = get_pass_stencil_read_mask(pass_index);
						return true;
					}

					else if (split_path[3] == "stencil_write_mask") {
						r_ret = get_pass_stencil_write_mask(pass_index);
						return true;
					}

					else if (split_path[3] == "stencil_comparison") {
						r_ret = get_pass_stencil_comparison(pass_index);
						return true;
					}
					else if (split_path[3] == "stencil_options") {
						if (split_path[4] == "sfail") {
							r_ret = get_pass_stencil_option(pass_index, STENCIL_OP_OPTION_SFAIL);
							return true;
						}
						else if (split_path[4] == "dpfail") {
							r_ret = get_pass_stencil_option(pass_index, STENCIL_OP_OPTION_DPFAIL);
							return true;
						}
						else if (split_path[4] == "dppass") {
							r_ret = get_pass_stencil_option(pass_index, STENCIL_OP_OPTION_DPPASS);
							return true;
						}
					}
				}
				else {
					Ref<Shader> shader = get_shader(pass_index);
					if (shader.is_valid()) {
						StringName param_name = split_path[2] + "/" + split_path[3];

						StringName pr = shader->remap_param(param_name);
						if (!pr) {
							String n = param_name;
							if (n.find("param/") == 0) { //backwards compatibility
								pr = n.substr(6, n.length());
							}
						}
						if (pr) {
							r_ret = VisualServer::get_singleton()->material_get_param(material, pass_index, pr);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}


void MultiPassMaterial::_get_property_list(List<PropertyInfo> *p_list) const {
	
	for (int i = 0; i < get_pass_count(); i++) {
		List<PropertyInfo> param_list;

		p_list->push_back(PropertyInfo(Variant::OBJECT, "passes/" + itos(i) + "/shader/shader", PROPERTY_HINT_RESOURCE_TYPE, "MaterialShader,MaterialShaderGraph"));
		Ref<Shader> shader = get_shader(i);

		if (!shader.is_null()) {
			shader->get_param_list(&param_list);
		}

		for (int j = 0; j < param_list.size(); j++) {
			String corrected_name = "passes/" + itos(i) + "/" + param_list[j].name;
			param_list[j].name = corrected_name;
			p_list->push_back(param_list[j]);
		}

		for (int j = 0; j<FLAG_MAX; j++)
			p_list->push_back(PropertyInfo(Variant::BOOL, "passes/" + itos(i) + "/flags/" + _flag_names[j]));

		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/blend_mode", PROPERTY_HINT_ENUM, "Mix,Add,Sub,PMAlpha"));
		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/depth_draw", PROPERTY_HINT_ENUM, "Always,Opaque Only,Pre-Pass Alpha,Never"));
		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/depth_test", PROPERTY_HINT_ENUM, "Never,Less,Equal,Less-or-Equal,Greater,Not Equal,Greater-or-Equal,Always"));
		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/depth_test", PROPERTY_HINT_ENUM, "Never,Less,Equal,Less-or-Equal,Greater,Not Equal,Greater-or-Equal,Always"));
		p_list->push_back(PropertyInfo(Variant::REAL, "passes/" + itos(i) + "/params/line_width", PROPERTY_HINT_RANGE, "0.1,32.0,0.1"));

		for (int j=0;j<COLOR_MASK_BIT_COUNT;j++)
			p_list->push_back(PropertyInfo(Variant::BOOL, "passes/" + itos(i) + "/color_mask_bit/" + _color_mask_bit_names[j]));

		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/stencil_reference_value", PROPERTY_HINT_RANGE, "0,255,1"));
		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/stencil_read_mask", PROPERTY_HINT_RANGE, "0,255,1"));
		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/stencil_write_mask", PROPERTY_HINT_RANGE, "0,255,1"));
		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/stencil_comparison", PROPERTY_HINT_ENUM, "Never,Less,Equal,Less-or-Equal,Greater,Not Equal,Greater-or-Equal,Always"));
		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/stencil_options/sfail", PROPERTY_HINT_ENUM, "Keep,Zero,Replace,Increment Saturate,Increment Wrap,Decrement Saturate,Decrement Wrap,Invert"));
		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/stencil_options/dpfail", PROPERTY_HINT_ENUM, "Keep,Zero,Replace,Increment Saturate,Increment Wrap,Decrement Saturate,Decrement Wrap,Invert"));
		p_list->push_back(PropertyInfo(Variant::INT, "passes/" + itos(i) + "/params/stencil_options/dppass", PROPERTY_HINT_ENUM, "Keep,Zero,Replace,Increment Saturate,Increment Wrap,Decrement Saturate,Decrement Wrap,Invert"));
	}
}


void MultiPassMaterial::_shader_changed() {

	_change_notify(); //also all may have changed then
}

void MultiPassMaterial::set_pass_count(const int p_pass_count) {
	int original_pass_count = passes.size();
	if (p_pass_count > 0 && p_pass_count != original_pass_count) {

		passes.resize(p_pass_count);
		VisualServer::get_singleton()->material_set_pass_count(material, p_pass_count);

		if (p_pass_count > original_pass_count) {
			for (int i = original_pass_count; i < p_pass_count; i++) {
				set_pass_flag(i, FLAG_VISIBLE, true);
				set_pass_flag(i, FLAG_DOUBLE_SIDED, false);
				set_pass_flag(i, FLAG_INVERT_FACES, false);
				set_pass_flag(i, FLAG_UNSHADED, false);
				set_pass_flag(i, FLAG_ONTOP, false);
				set_pass_flag(i, FLAG_LIGHTMAP_ON_UV2, true);
				set_pass_flag(i, FLAG_COLOR_ARRAY_SRGB, false);
				
				set_pass_depth_draw_mode(i, DEPTH_DRAW_OPAQUE_ONLY);
				set_pass_depth_test_mode(i, DEPTH_TEST_MODE_LEQUAL);
				set_pass_blend_mode(i, BLEND_MODE_MIX);
				set_pass_line_width(i, 0.1f);

				set_pass_color_mask_bit(i, COLOR_MASK_BIT_R, true);
				set_pass_color_mask_bit(i, COLOR_MASK_BIT_G, true);
				set_pass_color_mask_bit(i, COLOR_MASK_BIT_B, true);
				set_pass_color_mask_bit(i, COLOR_MASK_BIT_A, true);

				set_pass_stencil_reference_value(i, 0xff);
				set_pass_stencil_read_mask(i, 0xff);
				set_pass_stencil_write_mask(i, 0xff);
				set_pass_stencil_comparison(i, STENCIL_COMPARISON_ALWAYS);
				set_pass_stencil_option(i, STENCIL_OP_OPTION_SFAIL, STENCIL_OPERATION_KEEP);
				set_pass_stencil_option(i, STENCIL_OP_OPTION_DPFAIL, STENCIL_OPERATION_KEEP);
				set_pass_stencil_option(i, STENCIL_OP_OPTION_DPPASS, STENCIL_OPERATION_KEEP);
			}
		}
		_change_notify();
	}
}

int MultiPassMaterial::get_pass_count() const {
	return passes.size();
}

void MultiPassMaterial::set_flag(Flag p_flag, bool p_enabled) {
	set_pass_flag(0, p_flag, p_enabled);
}

bool MultiPassMaterial::get_flag(Flag p_flag) const {
	return get_pass_flag(0, p_flag);
}

void MultiPassMaterial::set_blend_mode(BlendMode p_blend_mode) {
	set_pass_blend_mode(0, p_blend_mode);
}

Material::BlendMode MultiPassMaterial::get_blend_mode() const {
	return get_pass_blend_mode(0);
}

void MultiPassMaterial::set_depth_draw_mode(DepthDrawMode p_depth_draw_mode) {
	set_pass_depth_draw_mode(0, p_depth_draw_mode);
}

Material::DepthDrawMode MultiPassMaterial::get_depth_draw_mode() const {
	return get_pass_depth_draw_mode(0);
}

void MultiPassMaterial::set_depth_test_mode(DepthTestMode p_depth_test_mode) {
	set_pass_depth_test_mode(0, p_depth_test_mode);
}

Material::DepthTestMode MultiPassMaterial::get_depth_test_mode() const {
	return get_pass_depth_test_mode(0);
}

void MultiPassMaterial::set_pass_flag(const int p_pass_index, Flag p_flag, bool p_enabled) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	ERR_FAIL_INDEX(p_flag, FLAG_MAX);
	passes[p_pass_index].flags[p_flag] = p_enabled;
	VisualServer::get_singleton()->material_set_flag(material, p_pass_index, (VS::MaterialFlag)p_flag, p_enabled);
	_change_notify();
}

bool MultiPassMaterial::get_pass_flag(const int p_pass_index, Flag p_flag) const {
	ERR_FAIL_INDEX_V(p_pass_index, passes.size(), false);
	ERR_FAIL_INDEX_V(p_flag, FLAG_MAX, false);
	return passes[p_pass_index].flags[p_flag];
}

void MultiPassMaterial::set_pass_blend_mode(const int p_pass_index, BlendMode p_blend_mode) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	passes[p_pass_index].blend_mode = p_blend_mode;
	VisualServer::get_singleton()->material_set_blend_mode(material, p_pass_index, (VS::MaterialBlendMode)p_blend_mode);
	_change_notify();
}

Material::BlendMode MultiPassMaterial::get_pass_blend_mode(const int p_pass_index) const {
	ERR_FAIL_INDEX_V(p_pass_index, passes.size(), BLEND_MODE_MIX);
	return passes[p_pass_index].blend_mode;
}

void MultiPassMaterial::set_pass_depth_draw_mode(const int p_pass_index, DepthDrawMode p_depth_draw_mode) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	passes[p_pass_index].depth_draw_mode = p_depth_draw_mode;
	VisualServer::get_singleton()->material_set_depth_draw_mode(material, p_pass_index, (VS::MaterialDepthDrawMode)p_depth_draw_mode);
}

Material::DepthDrawMode MultiPassMaterial::get_pass_depth_draw_mode(const int p_pass_index) const {
	ERR_FAIL_INDEX_V(p_pass_index, passes.size(), DEPTH_DRAW_ALWAYS);
	return passes[p_pass_index].depth_draw_mode;
}

void MultiPassMaterial::set_pass_depth_test_mode(const int p_pass_index, DepthTestMode p_depth_test_mode) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	passes[p_pass_index].depth_test_mode = p_depth_test_mode;
	VisualServer::get_singleton()->material_set_depth_test_mode(material, p_pass_index, (VS::MaterialDepthTestMode)p_depth_test_mode);
}

Material::DepthTestMode MultiPassMaterial::get_pass_depth_test_mode(const int p_pass_index) const {
	ERR_FAIL_INDEX_V(p_pass_index, passes.size(), DEPTH_TEST_MODE_LEQUAL);
	return passes[p_pass_index].depth_test_mode;
}

void MultiPassMaterial::set_pass_color_mask_bit(const int p_pass_index, ColorMaskBit p_bit, bool p_enabled) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	ERR_FAIL_INDEX(p_bit, COLOR_MASK_BIT_COUNT);
	passes[p_pass_index].color_mask_bit[p_bit] = p_enabled;
	VisualServer::get_singleton()->material_set_color_mask_bit(material, p_pass_index, (VS::MaterialColorMaskBit)p_bit, p_enabled);
}

bool MultiPassMaterial::get_pass_color_mask_bit(const int p_pass_index, ColorMaskBit p_bit) const {
	ERR_FAIL_INDEX_V(p_pass_index, passes.size(), false);
	ERR_FAIL_INDEX_V(p_bit, COLOR_MASK_BIT_COUNT, false);
	return passes[p_pass_index].color_mask_bit[p_bit];
}

void MultiPassMaterial::set_pass_stencil_reference_value(const int p_pass_index, const uint8_t p_reference_value) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	passes[p_pass_index].stencil_reference_value = p_reference_value;
	VisualServer::get_singleton()->material_set_stencil_reference_value(material, p_pass_index, p_reference_value);
}

uint8_t MultiPassMaterial::get_pass_stencil_reference_value(const int p_pass_index) const {
	ERR_FAIL_INDEX_V(p_pass_index, passes.size(), 0xff);
	return passes[p_pass_index].stencil_reference_value;
}

void MultiPassMaterial::set_pass_stencil_read_mask(const int p_pass_index, const uint8_t p_read_mask) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	passes[p_pass_index].stencil_read_mask = p_read_mask;
	VisualServer::get_singleton()->material_set_stencil_read_mask(material, p_pass_index, p_read_mask);
}

uint8_t MultiPassMaterial::get_pass_stencil_read_mask(const int p_pass_index) const {
	ERR_FAIL_INDEX_V(p_pass_index, passes.size(), 0xff);
	return passes[p_pass_index].stencil_read_mask;
}

void MultiPassMaterial::set_pass_stencil_write_mask(const int p_pass_index, const uint8_t p_write_mask) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	passes[p_pass_index].stencil_write_mask = p_write_mask;
	VisualServer::get_singleton()->material_set_stencil_write_mask(material, p_pass_index, p_write_mask);
}

uint8_t MultiPassMaterial::get_pass_stencil_write_mask(const int p_pass_index) const {
	ERR_FAIL_INDEX_V(p_pass_index, passes.size(), 0xff);
	return passes[p_pass_index].stencil_read_mask;
}

void MultiPassMaterial::set_pass_stencil_comparison(const int p_pass_index, StencilComparison p_comparison) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	ERR_FAIL_INDEX(p_comparison, STENCIL_COMPARISON_COUNT);
	passes[p_pass_index].stencil_comparison = p_comparison;
	VisualServer::get_singleton()->material_set_stencil_comparison(material, p_pass_index, (VS::MaterialStencilComparison)p_comparison);
}

Material::StencilComparison MultiPassMaterial::get_pass_stencil_comparison(const int p_pass_index) const {
	ERR_FAIL_INDEX_V(p_pass_index, passes.size(), STENCIL_COMPARISON_NEVER);
	return passes[p_pass_index].stencil_comparison;
}

void MultiPassMaterial::set_pass_stencil_option(const int p_pass_index, StencilOption p_option, StencilOperation p_operation) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	ERR_FAIL_INDEX(p_option, STENCIL_OP_OPTION_COUNT);
	ERR_FAIL_INDEX(p_operation, STENCIL_OPERATION_COUNT);
	passes[p_pass_index].stencil_options[p_option] = p_operation;
	VisualServer::get_singleton()->material_set_stencil_option(material, p_pass_index, (VS::MaterialStencilOperationOption)p_option, (VS::MaterialStencilOperation)p_operation);
}

Material::StencilOperation MultiPassMaterial::get_pass_stencil_option(const int p_pass_index, StencilOption p_option) const {
	ERR_FAIL_INDEX_V(p_pass_index, passes.size(), STENCIL_OPERATION_KEEP);
	ERR_FAIL_INDEX_V(p_option, STENCIL_OP_OPTION_COUNT, STENCIL_OPERATION_KEEP);
	return passes[p_pass_index].stencil_options[p_option];
}

void MultiPassMaterial::set_pass_line_width(const int p_pass_index, float p_width) {
	ERR_FAIL_INDEX(p_pass_index, passes.size());
	passes[p_pass_index].line_width = p_width;
	VisualServer::get_singleton()->material_set_line_width(material, p_pass_index, p_width);
}

void MultiPassMaterial::set_line_width(float p_width) {
	set_pass_line_width(0, p_width);
}

float MultiPassMaterial::get_pass_line_width(const int p_pass_index) const {
	return passes[p_pass_index].line_width;
}

float MultiPassMaterial::get_line_width() const {
	return get_pass_line_width(0);
}

void MultiPassMaterial::set_shader(const int p_pass_index, const Ref<Shader>& p_shader) {

	ERR_FAIL_COND(p_shader.is_valid() && p_shader->get_mode() != Shader::MODE_MATERIAL);

	if (passes[p_pass_index].shader.is_valid())
		passes[p_pass_index].shader->disconnect(SceneStringNames::get_singleton()->changed, this, SceneStringNames::get_singleton()->_shader_changed);
	passes[p_pass_index].shader = p_shader;
	VS::get_singleton()->material_set_shader(material, p_pass_index, passes[p_pass_index].shader.is_valid() ? passes[p_pass_index].shader->get_rid() : RID());
	if (passes[p_pass_index].shader.is_valid()) {
		passes[p_pass_index].shader->connect(SceneStringNames::get_singleton()->changed, this, SceneStringNames::get_singleton()->_shader_changed);
	}
	_change_notify();

}

Ref<Shader> MultiPassMaterial::get_shader(const int p_pass_index) const {

	if (p_pass_index < passes.size()) {
		return passes[p_pass_index].shader;
	}
	else {
		return NULL;
	}
}


void MultiPassMaterial::set_shader_param(const int p_pass_index, const StringName& p_param, const Variant& p_value) {

	VisualServer::get_singleton()->material_set_param(material, p_pass_index, p_param, p_value);

}

Variant MultiPassMaterial::get_shader_param(const int p_pass_index, const StringName& p_param) const{

	return VisualServer::get_singleton()->material_get_param(material, p_pass_index, p_param);
}



void MultiPassMaterial::_bind_methods() {
	
	ObjectTypeDB::bind_method(_MD("set_pass_flag", "pass_index", "flag", "enable"), &MultiPassMaterial::set_pass_flag);
	ObjectTypeDB::bind_method(_MD("get_pass_flag", "pass_index", "flag"), &MultiPassMaterial::get_pass_flag);
	ObjectTypeDB::bind_method(_MD("set_pass_blend_mode", "pass_index", "mode"), &MultiPassMaterial::set_pass_blend_mode);
	ObjectTypeDB::bind_method(_MD("get_pass_blend_mode", "pass_index"), &MultiPassMaterial::get_pass_blend_mode);
	ObjectTypeDB::bind_method(_MD("set_pass_line_width", "pass_index", "width"), &MultiPassMaterial::set_pass_line_width);
	ObjectTypeDB::bind_method(_MD("get_pass_line_width", "pass_index"), &MultiPassMaterial::get_pass_line_width);
	ObjectTypeDB::bind_method(_MD("set_pass_depth_draw_mode", "pass_index", "mode"), &MultiPassMaterial::set_pass_depth_draw_mode);
	ObjectTypeDB::bind_method(_MD("get_pass_depth_draw_mode", "pass_index"), &MultiPassMaterial::get_pass_depth_draw_mode);
	ObjectTypeDB::bind_method(_MD("set_pass_depth_test_mode", "pass_index", "mode"), &MultiPassMaterial::set_pass_depth_test_mode);
	ObjectTypeDB::bind_method(_MD("get_pass_depth_test_mode", "pass_index"), &MultiPassMaterial::get_pass_depth_test_mode);

	ObjectTypeDB::bind_method(_MD("set_pass_color_mask_bit", "pass_index", "color_mask_bit", "enable"), &MultiPassMaterial::set_pass_color_mask_bit);
	ObjectTypeDB::bind_method(_MD("get_pass_color_mask_bit", "pass_index", "color_mask_bit"), &MultiPassMaterial::get_pass_color_mask_bit);

	ObjectTypeDB::bind_method(_MD("set_pass_count", "pass_count"), &MultiPassMaterial::set_pass_count);
	ObjectTypeDB::bind_method(_MD("get_pass_count"), &MultiPassMaterial::get_pass_count);

	ObjectTypeDB::bind_method(_MD("set_shader", "pass_index", "shader:Shader"), &MultiPassMaterial::set_shader);
	ObjectTypeDB::bind_method(_MD("get_shader:Shader", "pass_index"), &MultiPassMaterial::get_shader);

	ObjectTypeDB::bind_method(_MD("set_shader_param", "pass_index", "param", "value:Variant"), &MultiPassMaterial::set_shader_param);
	ObjectTypeDB::bind_method(_MD("get_shader_param:Variant", "pass_index", "param"), &MultiPassMaterial::get_shader_param);

	ObjectTypeDB::bind_method(_MD("_shader_changed"), &MultiPassMaterial::_shader_changed);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "passes/pass_count", PROPERTY_HINT_RANGE, "1,255,1"), _SCS("set_pass_count"), _SCS("get_pass_count"));
}


void MultiPassMaterial::get_argument_options(const StringName& p_function, int p_idx, List<String>*r_options) const {

	String f = p_function.operator String();
	if ((f == "get_shader_param" || f == "set_shader_param") && p_idx == 0) {
		for (int i = 0; i < passes.size(); i++) {
			if (passes[i].shader.is_valid()) {
				List<PropertyInfo> pl;
				passes[i].shader->get_param_list(&pl);
				for (List<PropertyInfo>::Element *E = pl.front(); E; E = E->next()) {
					r_options->push_back("\"" + E->get().name.replace_first("passes/ " + itos(i) + "/shader_param/", "") + "\"");
				}
			}
		}
	}
	Material::get_argument_options(p_function, p_idx, r_options);
}

MultiPassMaterial::MultiPassMaterial() :Material(VisualServer::get_singleton()->material_create(1)){
	set_pass_count(1);
}


/////////////////////////////////

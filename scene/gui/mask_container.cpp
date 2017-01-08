#include "mask_container.h"

bool MaskContainer::clips_input() const {

	return true;
}

void MaskContainer::_notification(int p_what) {
	if (p_what==NOTIFICATION_DRAW) {

		if (texture.is_null())
			return;

		switch(stretch_mode) {
			case TextureFrame::STRETCH_SCALE_ON_EXPAND: {
				Size2 s=expand?get_size():texture->get_size();
				draw_texture_rect(texture,Rect2(Point2(),s),false,modulate);
			} break;
			case TextureFrame::STRETCH_SCALE: {
				draw_texture_rect(texture,Rect2(Point2(),get_size()),false,modulate);
			} break;
			case TextureFrame::STRETCH_TILE: {
				draw_texture_rect(texture,Rect2(Point2(),get_size()),true,modulate);
			} break;
			case TextureFrame::STRETCH_KEEP: {
				draw_texture_rect(texture,Rect2(Point2(),texture->get_size()),false,modulate);

			} break;
			case TextureFrame::STRETCH_KEEP_CENTERED: {

				Vector2 ofs = (get_size() - texture->get_size())/2;
				draw_texture_rect(texture,Rect2(ofs,texture->get_size()),false,modulate);
			} break;
			case TextureFrame::STRETCH_KEEP_ASPECT_CENTERED:
			case TextureFrame::STRETCH_KEEP_ASPECT: {

				Size2 size=get_size();
				int tex_width = texture->get_width() * size.height / texture ->get_height();
				int tex_height = size.height;

				if (tex_width>size.width) {
					tex_width=size.width;
					tex_height=texture->get_height() * tex_width / texture->get_width();
				}

				int ofs_x = 0;
				int ofs_y = 0;

				if (stretch_mode == TextureFrame::STRETCH_KEEP_ASPECT_CENTERED) {
					ofs_x+=(size.width - tex_width)/2;
					ofs_y+=(size.height - tex_height)/2;
				}
				draw_texture_rect(texture,Rect2(ofs_x,ofs_y,tex_width,tex_height));
			} break;

		}

		VisualServer::get_singleton()->canvas_item_set_clip(get_canvas_item(), true);
		VisualServer::get_singleton()->canvas_item_set_mask(get_canvas_item(), true);
	}
};

void MaskContainer::_bind_methods() {
	ObjectTypeDB::bind_method(_MD("set_texture", "texture"), &MaskContainer::set_texture);
	ObjectTypeDB::bind_method(_MD("get_texture"), &MaskContainer::get_texture);
	ObjectTypeDB::bind_method(_MD("set_modulate", "modulate"), &MaskContainer::set_modulate);
	ObjectTypeDB::bind_method(_MD("get_modulate"), &MaskContainer::get_modulate);
	ObjectTypeDB::bind_method(_MD("set_expand", "enable"), &MaskContainer::set_expand);
	ObjectTypeDB::bind_method(_MD("has_expand"), &MaskContainer::has_expand);
	ObjectTypeDB::bind_method(_MD("set_stretch_mode", "stretch_mode"), &MaskContainer::set_stretch_mode);
	ObjectTypeDB::bind_method(_MD("get_stretch_mode"), &MaskContainer::get_stretch_mode);

	ADD_PROPERTYNZ(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), _SCS("set_texture"), _SCS("get_texture"));
	ADD_PROPERTYNZ(PropertyInfo(Variant::BOOL, "expand"), _SCS("set_expand"), _SCS("has_expand"));
	ADD_PROPERTYNO(PropertyInfo(Variant::INT, "stretch_mode", PROPERTY_HINT_ENUM, "Scale On Expand (Compat),Scale,Tile,Keep,Keep Centered,Keep Aspect,Keep Aspect Centered"), _SCS("set_stretch_mode"), _SCS("get_stretch_mode"));
};

void MaskContainer::set_texture(const Ref<Texture>& p_tex) {

	texture = p_tex;
	update();
	minimum_size_changed();
}

Ref<Texture> MaskContainer::get_texture() const {

	return texture;
}

void MaskContainer::set_modulate(const Color& p_tex) {

	modulate = p_tex;
	update();
	minimum_size_changed();
}

Color MaskContainer::get_modulate() const{

	return modulate;
}

void MaskContainer::set_expand(bool p_expand) {

	expand=p_expand;
	update();
	minimum_size_changed();
}

bool MaskContainer::has_expand() const {

	return expand;
}

void MaskContainer::set_stretch_mode(TextureFrame::StretchMode p_mode) {

	stretch_mode = p_mode;
	update();
}

TextureFrame::StretchMode MaskContainer::get_stretch_mode() const {

	return stretch_mode;
}

MaskContainer::MaskContainer() {


	expand = false;
	modulate = Color(1, 1, 1, 1);
	set_ignore_mouse(true);
	stretch_mode = TextureFrame::STRETCH_SCALE_ON_EXPAND;
}

MaskContainer::~MaskContainer()
{
}
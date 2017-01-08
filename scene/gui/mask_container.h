#ifndef MASK_CONTAINER_H
#define MASK_CONTAINER_H

#include "container.h"
#include "texture_frame.h"

class MaskContainer : public Container {

	OBJ_TYPE(MaskContainer, Container);
protected:
	void _input_event(const InputEvent& p_input_event);
	void _notification(int p_what);

	static void _bind_methods();
private:
	bool expand;
	Color modulate;
	Ref<Texture> texture;
	TextureFrame::StretchMode stretch_mode;
public:
	virtual bool clips_input() const;

	void set_texture(const Ref<Texture>& p_tex);
	Ref<Texture> get_texture() const;

	void set_modulate(const Color& p_tex);
	Color get_modulate() const;

	void set_expand(bool p_expand);
	bool has_expand() const;

	void set_stretch_mode(TextureFrame::StretchMode p_mode);
	TextureFrame::StretchMode get_stretch_mode() const;

	MaskContainer();
	~MaskContainer();
};

#endif

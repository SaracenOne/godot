#include "scene_format_text.h"

#include "globals.h"
#include "version.h"
#include "os/dir_access.h"

#define FORMAT_VERSION 1

#include "version.h"
#include "os/dir_access.h"

#define _printerr() ERR_PRINT(String(res_path+":"+itos(lines)+" - Parse Error: "+error_text).utf8().get_data());


Error ResourceInteractiveLoaderText::parse_property(Variant& r_v, String &r_name)  {

	return OK;
}




///

void ResourceInteractiveLoaderText::set_local_path(const String& p_local_path) {

	res_path=p_local_path;
}

Ref<Resource> ResourceInteractiveLoaderText::get_resource() {

	return resource;
}

Error ResourceInteractiveLoaderText::_parse_sub_resource(VariantParser::Stream* p_stream,Ref<Resource>& r_res,int &line,String &r_err_str) {

	VariantParser::Token token;
	VariantParser::get_token(p_stream,token,line,r_err_str);
	if (token.type!=VariantParser::TK_NUMBER) {
		r_err_str="Expected number (sub-resource index)";
		return ERR_PARSE_ERROR;
	}

	int index = token.value;

	String path = local_path+"::"+itos(index);

	if (!ResourceCache::has(path)) {
		r_err_str="Can't load cached sub-resource: "+path;
		return ERR_PARSE_ERROR;
	}

	r_res=RES(ResourceCache::get(path));

	VariantParser::get_token(p_stream,token,line,r_err_str);
	if (token.type!=VariantParser::TK_PARENTHESIS_CLOSE) {
		r_err_str="Expected ')'";
		return ERR_PARSE_ERROR;
	}


	return OK;
}

Error ResourceInteractiveLoaderText::_parse_ext_resource(VariantParser::Stream* p_stream,Ref<Resource>& r_res,int &line,String &r_err_str){

	VariantParser::Token token;
	VariantParser::get_token(p_stream,token,line,r_err_str);
	if (token.type!=VariantParser::TK_NUMBER) {
		r_err_str="Expected number (sub-resource index)";
		return ERR_PARSE_ERROR;
	}

	int id = token.value;


	if (!ext_resources.has(id)) {
		r_err_str="Can't load cached ext-resource #"+itos(id);
		return ERR_PARSE_ERROR;
	}

	String path = ext_resources[id].path;
	String type = ext_resources[id].type;

	if (path.find("://")==-1 && path.is_rel_path()) {
		// path is relative to file being loaded, so convert to a resource path
		path=Globals::get_singleton()->localize_path(res_path.get_base_dir().plus_file(path));

	}

	r_res=ResourceLoader::load(path,type);

	if (r_res.is_null()) {
		r_err_str="Couldn't load external resource: "+path;
		return ERR_PARSE_ERROR;
	}

	VariantParser::get_token(p_stream,token,line,r_err_str);
	if (token.type!=VariantParser::TK_PARENTHESIS_CLOSE) {
		r_err_str="Expected ')'";
		return ERR_PARSE_ERROR;
	}


	return OK;
}


Error ResourceInteractiveLoaderText::poll() {

	if (error!=OK)
		return error;

	if (next_tag.name=="ext_resource") {


		if (!next_tag.fields.has("path")) {
			error=ERR_FILE_CORRUPT;
			error_text="Missing 'path' in external resource tag";
			_printerr();
			return error;
		}

		if (!next_tag.fields.has("type")) {
			error=ERR_FILE_CORRUPT;
			error_text="Missing 'type' in external resource tag";
			_printerr();
			return error;
		}

		if (!next_tag.fields.has("id")) {
			error=ERR_FILE_CORRUPT;
			error_text="Missing 'id' in external resource tag";
			_printerr();
			return error;
		}

		String path=next_tag.fields["path"];
		String type=next_tag.fields["type"];
		int index=next_tag.fields["id"];


		if (path.find("://")==-1 && path.is_rel_path()) {
			// path is relative to file being loaded, so convert to a resource path
			path=Globals::get_singleton()->localize_path(local_path.get_base_dir().plus_file(path));
		}

		if (remaps.has(path)) {
			path=remaps[path];
		}

		RES res = ResourceLoader::load(path,type);

		if (res.is_null()) {

			if (ResourceLoader::get_abort_on_missing_resources()) {
				error=ERR_FILE_CORRUPT;
				error_text="[ext_resource] referenced nonexistent resource at: "+path;
				_printerr();
				return error;
			} else {
				ResourceLoader::notify_dependency_error(local_path,path,type);
			}
		} else {

			resource_cache.push_back(res);
		}

		ExtResource er;
		er.path=path;
		er.type=type;
		ext_resources[index]=er;

		error = VariantParser::parse_tag(&stream,lines,error_text,next_tag,&rp);

		if (error) {
			_printerr();
		}

		return error;


	} else if (next_tag.name=="sub_resource") {


		if (!next_tag.fields.has("type")) {
			error=ERR_FILE_CORRUPT;
			error_text="Missing 'type' in external resource tag";
			_printerr();
			return error;
		}

		if (!next_tag.fields.has("id")) {
			error=ERR_FILE_CORRUPT;
			error_text="Missing 'index' in external resource tag";
			_printerr();
			return error;
		}

		String type=next_tag.fields["type"];
		int id=next_tag.fields["id"];

		String path = local_path+"::"+itos(id);


		//bool exists=ResourceCache::has(path);

		Ref<Resource> res;

		if ( !ResourceCache::has(path)) { //only if it doesn't exist

			Object *obj = ObjectTypeDB::instance(type);
			if (!obj) {

				error_text+="Can't create sub resource of type: "+type;
				_printerr();
				error=ERR_FILE_CORRUPT;
				return error;
			}


			Resource *r = obj->cast_to<Resource>();
			if (!r) {

				error_text+="Can't create sub resource of type, because not a resource: "+type;
				_printerr();
				error=ERR_FILE_CORRUPT;
				return error;
			}

			res=Ref<Resource>(r);
			resource_cache.push_back(res);
			res->set_path(path);

		}

		while(true) {

			String assign;
			Variant value;

			error = VariantParser::parse_tag_assign_eof(&stream,lines,error_text,next_tag,assign,value,&rp);

			if (error) {
				_printerr();
				return error;
			}

			if (assign!=String()) {
				if (res.is_valid()) {
					res->set(assign,value);
				}
				//it's assignment
			} else if (next_tag.name!=String()) {

				error=OK;
				break;
			} else {
				error=ERR_FILE_CORRUPT;
				error_text="Premature end of file while parsing [sub_resource]";
				_printerr();
				return error;
			}


		}

		return OK;

	} else if (next_tag.name=="resource") {

		if (is_scene) {

			error_text+="found the 'resource' tag on a scene file!";
			_printerr();
			error=ERR_FILE_CORRUPT;
			return error;
		}

		Object *obj = ObjectTypeDB::instance(res_type);
		if (!obj) {

			error_text+="Can't create sub resource of type: "+res_type;
			_printerr();
			error=ERR_FILE_CORRUPT;
			return error;
		}


		Resource *r = obj->cast_to<Resource>();
		if (!r) {

			error_text+="Can't create sub resource of type, because not a resource: "+res_type;
			_printerr();
			error=ERR_FILE_CORRUPT;
			return error;
		}

		resource=Ref<Resource>(r);

		while(true) {

			String assign;
			Variant value;

			error = VariantParser::parse_tag_assign_eof(&stream,lines,error_text,next_tag,assign,value,&rp);

			if (error) {
				if (error!=ERR_FILE_EOF) {
					_printerr();
				}
				return error;
			}

			if (assign!=String()) {
				resource->set(assign,value);
				//it's assignment
			} else if (next_tag.name!=String()) {

				error=ERR_FILE_CORRUPT;
				error_text="Extra tag found when parsing main resource file";
				_printerr();
				return error;
			} else {
				error=ERR_FILE_EOF;
				return error;
			}

		}

		return OK;

	} else if (next_tag.name=="node") {

		if (!is_scene) {

			error_text+="found the 'node' tag on a resource file!";
			_printerr();
			error=ERR_FILE_CORRUPT;
			return error;
		}

		/*
		int add_name(const StringName& p_name);
		int add_value(const Variant& p_value);
		int add_node_path(const NodePath& p_path);
		int add_node(int p_parent,int p_owner,int p_type,int p_name, int p_instance);
		void add_node_property(int p_node,int p_name,int p_value);
		void add_node_group(int p_node,int p_group);
		void set_base_scene(int p_idx);
		void add_connection(int p_from,int p_to, int p_signal, int p_method, int p_flags,const Vector<int>& p_binds);
		void add_editable_instance(const NodePath& p_path);

		*/

		int parent=-1;
		int owner=-1;
		int type=-1;
		int name=-1;
		int instance=-1;
		int base_scene=-1;

		if (next_tag.fields.has("name")) {
			name=packed_scene->get_state()->add_name(next_tag.fields["name"]);
		}

		if (next_tag.fields.has("parent")) {
			parent=packed_scene->get_state()->add_node_path(next_tag.fields["parent"]);
		}

		if (next_tag.fields.has("owner")) {
			owner=packed_scene->get_state()->add_node_path(next_tag.fields["owner"]);
		} else {
			if (parent!=-1)
				owner=0; //if no owner, owner is root
		}


		if (next_tag.fields.has("type")) {
			type=packed_scene->get_state()->add_name(next_tag.fields["type"]);
		}

		if (next_tag.fields.has("instance")) {

			instance=packed_scene->get_state()->add_value(next_tag.fields["instance"]);

			if (packed_scene->get_state()->get_node_count()==0 && parent==-1) {
				packed_scene->get_state()->set_base_scene(instance);
				instance=-1;
			}
		}

		int node_id = packed_scene->get_state()->add_node(parent,owner,type,name,instance);


		while(true) {

			String assign;
			Variant value;

			error = VariantParser::parse_tag_assign_eof(&stream,lines,error_text,next_tag,assign,value,&rp);

			if (error) {
				if (error!=ERR_FILE_EOF) {
					_printerr();
				} else {
					resource=packed_scene;
				}
				return error;
			}

			if (assign!=String()) {
				int nameidx = packed_scene->get_state()->add_name(assign);
				int valueidx = packed_scene->get_state()->add_value(value);
				packed_scene->get_state()->add_node_property(node_id,nameidx,valueidx);
				//it's assignment
			} else if (next_tag.name!=String()) {

				error=OK;
				return error;
			} else {

				resource=packed_scene;
				error=ERR_FILE_EOF;
				return error;
			}

		}

		return OK;

	} else if (next_tag.name=="connection") {

		if (!is_scene) {

			error_text+="found the 'connection' tag on a resource file!";
			_printerr();
			error=ERR_FILE_CORRUPT;
			return error;
		}

		if (!next_tag.fields.has("from")) {
			error=ERR_FILE_CORRUPT;
			error_text="missing 'from' field fron connection tag";
			return error;
		}

		if (!next_tag.fields.has("to")) {
			error=ERR_FILE_CORRUPT;
			error_text="missing 'to' field fron connection tag";
			return error;
		}

		if (!next_tag.fields.has("signal")) {
			error=ERR_FILE_CORRUPT;
			error_text="missing 'signal' field fron connection tag";
			return error;
		}

		if (!next_tag.fields.has("method")) {
			error=ERR_FILE_CORRUPT;
			error_text="missing 'method' field fron connection tag";
			return error;
		}

		NodePath from = next_tag.fields["from"];
		NodePath to = next_tag.fields["to"];
		StringName method = next_tag.fields["method"];
		StringName signal = next_tag.fields["signal"];
		int flags=CONNECT_PERSIST;
		Array binds;

		if (next_tag.fields.has("flags")) {
			flags=next_tag.fields["flags"];
		}

		if (next_tag.fields.has("binds")) {
			binds=next_tag.fields["binds"];
		}

		Vector<int> bind_ints;
		for(int i=9;i<binds.size();i++) {
			bind_ints.push_back( packed_scene->get_state()->add_value( bind_ints[i] ) );
		}

		packed_scene->get_state()->add_connection(
					packed_scene->get_state()->add_node_path(from.simplified()),
					packed_scene->get_state()->add_node_path(to.simplified()),
					packed_scene->get_state()->add_name(signal),
					packed_scene->get_state()->add_name(method),
					flags,
					bind_ints
					);

		error = VariantParser::parse_tag(&stream,lines,error_text,next_tag,&rp);

		if (error) {
			if (error!=ERR_FILE_EOF) {
				_printerr();
			} else {
				resource=packed_scene;
			}
		}

		return error;
	} else if (next_tag.name=="editable") {

		if (!is_scene) {

			error_text+="found the 'editable' tag on a resource file!";
			_printerr();
			error=ERR_FILE_CORRUPT;
			return error;
		}

		if (!next_tag.fields.has("path")) {
			error=ERR_FILE_CORRUPT;
			error_text="missing 'path' field fron connection tag";
			_printerr();
			return error;
		}

		NodePath path = next_tag.fields["path"];

		packed_scene->get_state()->add_editable_instance(path.simplified());

		error = VariantParser::parse_tag(&stream,lines,error_text,next_tag,&rp);

		if (error) {
			if (error!=ERR_FILE_EOF) {
				_printerr();
			} else {
				resource=packed_scene;
			}
		}

		return error;

	} else {

		error_text+="Unknown tag in file: "+next_tag.name;
		_printerr();
		error=ERR_FILE_CORRUPT;
		return error;
	}

	return OK;
}

int ResourceInteractiveLoaderText::get_stage() const {

	return resource_current;
}
int ResourceInteractiveLoaderText::get_stage_count() const {

	return resources_total;//+ext_resources;
}

ResourceInteractiveLoaderText::~ResourceInteractiveLoaderText() {

	memdelete(f);
}

void ResourceInteractiveLoaderText::get_dependencies(FileAccess *f,List<String> *p_dependencies,bool p_add_types) {


	open(f);
	ERR_FAIL_COND(error!=OK);

	while(next_tag.name=="ext_resource") {

		if (!next_tag.fields.has("type")) {
			error=ERR_FILE_CORRUPT;
			error_text="Missing 'type' in external resource tag";
			_printerr();
			return;
		}

		if (!next_tag.fields.has("id")) {
			error=ERR_FILE_CORRUPT;
			error_text="Missing 'index' in external resource tag";
			_printerr();
			return;
		}

		String path=next_tag.fields["path"];
		String type=next_tag.fields["type"];


		if (path.find("://")==-1 && path.is_rel_path()) {
			// path is relative to file being loaded, so convert to a resource path
			path=Globals::get_singleton()->localize_path(local_path.get_base_dir().plus_file(path));
		}


		if (p_add_types) {
			path+="::"+type;
		}

		p_dependencies->push_back(path);

		Error err = VariantParser::parse_tag(&stream,lines,error_text,next_tag,&rp);

		if (err) {
			error_text="Unexpected end of file";
			_printerr();
			error=ERR_FILE_CORRUPT;
		}


	}
}

Error ResourceInteractiveLoaderText::rename_dependencies(FileAccess *p_f, const String &p_path,const Map<String,String>& p_map) {




#if 0
	open(p_f);
	ERR_FAIL_COND_V(error!=OK,error);

	//FileAccess

	bool old_format=false;

	FileAccess *fw = NULL;

	String base_path=local_path.get_base_dir();

	while(true) {
		bool exit;
		List<String> order;

		Tag *tag = parse_tag(&exit,true,&order);

		bool done=false;

		if (!tag) {
			if (fw) {
				memdelete(fw);
			}
			error=ERR_FILE_CORRUPT;
			ERR_FAIL_COND_V(!exit,error);
			error=ERR_FILE_EOF;

			return error;
		}

		if (tag->name=="ext_resource") {

			if (!tag->args.has("index") || !tag->args.has("path") || !tag->args.has("type")) {
				old_format=true;
				break;
			}

			if (!fw) {

				fw=FileAccess::open(p_path+".depren",FileAccess::WRITE);
				fw->store_line("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"); //no escape
				fw->store_line("<resource_file type=\""+resource_type+"\" subresource_count=\""+itos(resources_total)+"\" version=\""+itos(VERSION_MAJOR)+"."+itos(VERSION_MINOR)+"\" version_name=\""+VERSION_FULL_NAME+"\">");

			}

			String path = tag->args["path"];
			String index = tag->args["index"];
			String type = tag->args["type"];


			bool relative=false;
			if (!path.begins_with("res://")) {
				path=base_path.plus_file(path).simplify_path();
				relative=true;
			}


			if (p_map.has(path)) {
				String np=p_map[path];
				path=np;
			}

			if (relative) {
				//restore relative
				path=base_path.path_to_file(path);
			}

			tag->args["path"]=path;
			tag->args["index"]=index;
			tag->args["type"]=type;

		} else {

			done=true;
		}

		String tagt="\t<";
		if (exit)
			tagt+="/";
		tagt+=tag->name;

		for(List<String>::Element *E=order.front();E;E=E->next()) {
			tagt+=" "+E->get()+"=\""+tag->args[E->get()]+"\"";
		}
		tagt+=">";
		fw->store_line(tagt);
		if (done)
			break;
		close_tag("ext_resource");
		fw->store_line("\t</ext_resource>");

	}


	if (old_format) {
		if (fw)
			memdelete(fw);

		DirAccess *da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
		da->remove(p_path+".depren");
		memdelete(da);
		//fuck it, use the old approach;

		WARN_PRINT(("This file is old, so it can't refactor dependencies, opening and resaving: "+p_path).utf8().get_data());

		Error err;
		FileAccess *f2 = FileAccess::open(p_path,FileAccess::READ,&err);
		if (err!=OK) {
			ERR_FAIL_COND_V(err!=OK,ERR_FILE_CANT_OPEN);
		}

		Ref<ResourceInteractiveLoaderText> ria = memnew( ResourceInteractiveLoaderText );
		ria->local_path=Globals::get_singleton()->localize_path(p_path);
		ria->res_path=ria->local_path;
		ria->remaps=p_map;
	//	ria->set_local_path( Globals::get_singleton()->localize_path(p_path) );
		ria->open(f2);

		err = ria->poll();

		while(err==OK) {
			err=ria->poll();
		}

		ERR_FAIL_COND_V(err!=ERR_FILE_EOF,ERR_FILE_CORRUPT);
		RES res = ria->get_resource();
		ERR_FAIL_COND_V(!res.is_valid(),ERR_FILE_CORRUPT);

		return ResourceFormatSaverText::singleton->save(p_path,res);
	}

	if (!fw) {

		return OK; //nothing to rename, do nothing
	}

	uint8_t c=f->get_8();
	while(!f->eof_reached()) {
		fw->store_8(c);
		c=f->get_8();
	}

	bool all_ok = fw->get_error()==OK;

	memdelete(fw);

	if (!all_ok) {
		return ERR_CANT_CREATE;
	}

	DirAccess *da = DirAccess::create(DirAccess::ACCESS_RESOURCES);
	da->remove(p_path);
	da->rename(p_path+".depren",p_path);
	memdelete(da);
#endif
	return OK;

}


void ResourceInteractiveLoaderText::open(FileAccess *p_f) {

	error=OK;

	lines=1;
	f=p_f;


	stream.f=f;
	is_scene=false;


	VariantParser::Tag tag;
	Error err = VariantParser::parse_tag(&stream,lines,error_text,tag);

	if (err) {

		error=err;
		_printerr();
		return;
	}

	if (tag.fields.has("format")) {
		int fmt = tag.fields["format"];
		if (fmt>FORMAT_VERSION) {
			error_text="Saved with newer format version";
			_printerr();
			error=ERR_PARSE_ERROR;
			return;
		}
	}


	if (tag.name=="gd_scene") {
		is_scene=true;
		packed_scene.instance();

	} else if (tag.name=="gd_resource") {
		if (!tag.fields.has("type")) {
			error_text="Missing 'type' field in 'gd_resource' tag";
			_printerr();
			error=ERR_PARSE_ERROR;
			return;
		}

		res_type=tag.fields["type"];

	} else {
		error_text="Unrecognized file type: "+tag.name;
		_printerr();
		error=ERR_PARSE_ERROR;
		return;

	}



	if (tag.fields.has("load_steps")) {
		resources_total=tag.fields["load_steps"];
	} else {
		resources_total=0;
	}


	err = VariantParser::parse_tag(&stream,lines,error_text,next_tag,&rp);

	if (err) {
		error_text="Unexpected end of file";
		_printerr();
		error=ERR_FILE_CORRUPT;
	}

	rp.ext_func=_parse_ext_resources;
	rp.sub_func=_parse_sub_resources;
	rp.func=NULL;
	rp.userdata=this;

}




String ResourceInteractiveLoaderText::recognize(FileAccess *p_f) {

	error=OK;

	lines=1;
	f=p_f;

	stream.f=f;


	VariantParser::Tag tag;
	Error err = VariantParser::parse_tag(&stream,lines,error_text,tag);

	if (err) {
		_printerr();
		return "";
	}

	if (tag.fields.has("format")) {
		int fmt = tag.fields["format"];
		if (fmt>FORMAT_VERSION) {
			error_text="Saved with newer format version";
			_printerr();
			return "";
		}
	}

	if (tag.name=="gd_scene")
		return "PackedScene";

	if (tag.name!="gd_resource")
		return "";



	if (!tag.fields.has("type")) {
		error_text="Missing 'type' field in 'gd_resource' tag";
		_printerr();
		return "";
	}

	return tag.fields["type"];


}

/////////////////////

Ref<ResourceInteractiveLoader> ResourceFormatLoaderText::load_interactive(const String &p_path, Error *r_error) {

	if (r_error)
		*r_error=ERR_CANT_OPEN;

	Error err;
	FileAccess *f = FileAccess::open(p_path,FileAccess::READ,&err);


	if (err!=OK) {

		ERR_FAIL_COND_V(err!=OK,Ref<ResourceInteractiveLoader>());
	}

	Ref<ResourceInteractiveLoaderText> ria = memnew( ResourceInteractiveLoaderText );
	ria->local_path=Globals::get_singleton()->localize_path(p_path);
	ria->res_path=ria->local_path;
//	ria->set_local_path( Globals::get_singleton()->localize_path(p_path) );
	ria->open(f);

	return ria;
}

void ResourceFormatLoaderText::get_recognized_extensions_for_type(const String& p_type,List<String> *p_extensions) const {


	if (p_type=="PackedScene")
		p_extensions->push_back("tscn");
	else
		p_extensions->push_back("tres");

}

void ResourceFormatLoaderText::get_recognized_extensions(List<String> *p_extensions) const{

	p_extensions->push_back("tscn");
	p_extensions->push_back("tres");
}

bool ResourceFormatLoaderText::handles_type(const String& p_type) const{

	return true;
}
String ResourceFormatLoaderText::get_resource_type(const String &p_path) const{



	String ext=p_path.extension().to_lower();
	if (ext=="tscn")
		return "PackedScene";
	else if (ext!="tres")
		return String();

	//for anyhting else must test..

	FileAccess *f = FileAccess::open(p_path,FileAccess::READ);
	if (!f) {

		return ""; //could not rwead
	}

	Ref<ResourceInteractiveLoaderText> ria = memnew( ResourceInteractiveLoaderText );
	ria->local_path=Globals::get_singleton()->localize_path(p_path);
	ria->res_path=ria->local_path;
//	ria->set_local_path( Globals::get_singleton()->localize_path(p_path) );
	String r = ria->recognize(f);
	return r;
}


void ResourceFormatLoaderText::get_dependencies(const String& p_path,List<String> *p_dependencies,bool p_add_types) {

	FileAccess *f = FileAccess::open(p_path,FileAccess::READ);
	if (!f) {

		ERR_FAIL();
	}

	Ref<ResourceInteractiveLoaderText> ria = memnew( ResourceInteractiveLoaderText );
	ria->local_path=Globals::get_singleton()->localize_path(p_path);
	ria->res_path=ria->local_path;
//	ria->set_local_path( Globals::get_singleton()->localize_path(p_path) );
	ria->get_dependencies(f,p_dependencies,p_add_types);


}

Error ResourceFormatLoaderText::rename_dependencies(const String &p_path,const Map<String,String>& p_map) {

	FileAccess *f = FileAccess::open(p_path,FileAccess::READ);
	if (!f) {

		ERR_FAIL_V(ERR_CANT_OPEN);
	}

	Ref<ResourceInteractiveLoaderText> ria = memnew( ResourceInteractiveLoaderText );
	ria->local_path=Globals::get_singleton()->localize_path(p_path);
	ria->res_path=ria->local_path;
//	ria->set_local_path( Globals::get_singleton()->localize_path(p_path) );
	return ria->rename_dependencies(f,p_path,p_map);
}


/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/


void ResourceFormatSaverTextInstance::write_property(const String& p_name,const Variant& p_property,bool *r_ok) {

	if (r_ok)
		*r_ok=false;

	if (p_name!=String()) {
		f->store_string(p_name+" = ");
	}

	switch( p_property.get_type() ) {

		case Variant::NIL: {
			f->store_string("null");
		} break;
		case Variant::BOOL: {

			f->store_string(p_property.operator bool() ? "true":"false" );
		} break;
		case Variant::INT: {

			f->store_string( itos(p_property.operator int()) );
		} break;
		case Variant::REAL: {

			f->store_string( rtoss(p_property.operator real_t()) );
		} break;
		case Variant::STRING: {

			String str=p_property;

			str="\""+str.c_escape()+"\"";
			f->store_string( str );
		} break;
		case Variant::VECTOR2: {

			Vector2 v = p_property;
			f->store_string("Vector2( "+rtoss(v.x) +", "+rtoss(v.y)+" )" );
		} break;
		case Variant::RECT2: {

			Rect2 aabb = p_property;
			f->store_string("Rect2( "+rtoss(aabb.pos.x) +", "+rtoss(aabb.pos.y) +", "+rtoss(aabb.size.x) +", "+rtoss(aabb.size.y)+" )" );

		} break;
		case Variant::VECTOR3: {

			Vector3 v = p_property;
			f->store_string("Vector3( "+rtoss(v.x) +", "+rtoss(v.y)+", "+rtoss(v.z)+" )");
		} break;
		case Variant::PLANE: {

			Plane p = p_property;
			f->store_string("Plane( "+rtoss(p.normal.x) +", "+rtoss(p.normal.y)+", "+rtoss(p.normal.z)+", "+rtoss(p.d)+" )" );

		} break;
		case Variant::_AABB: {

			AABB aabb = p_property;
			f->store_string("AABB( "+rtoss(aabb.pos.x) +", "+rtoss(aabb.pos.y) +", "+rtoss(aabb.pos.z) +", "+rtoss(aabb.size.x) +", "+rtoss(aabb.size.y) +", "+rtoss(aabb.size.z)+" )"  );

		} break;
		case Variant::QUAT: {

			Quat quat = p_property;
			f->store_string("Quat( "+rtoss(quat.x)+", "+rtoss(quat.y)+", "+rtoss(quat.z)+", "+rtoss(quat.w)+" )");

		} break;
		case Variant::MATRIX32: {

			String s="Matrix32( ";
			Matrix32 m3 = p_property;
			for (int i=0;i<3;i++) {
				for (int j=0;j<2;j++) {

					if (i!=0 || j!=0)
						s+=", ";
					s+=rtoss( m3.elements[i][j] );
				}
			}

			f->store_string(s+" )");

		} break;
		case Variant::MATRIX3: {

			String s="Matrix3( ";
			Matrix3 m3 = p_property;
			for (int i=0;i<3;i++) {
				for (int j=0;j<3;j++) {

					if (i!=0 || j!=0)
						s+=", ";
					s+=rtoss( m3.elements[i][j] );
				}
			}

			f->store_string(s+" )");

		} break;
		case Variant::TRANSFORM: {

			String s="Transform( ";
			Transform t = p_property;
			Matrix3 &m3 = t.basis;
			for (int i=0;i<3;i++) {
				for (int j=0;j<3;j++) {

					if (i!=0 || j!=0)
						s+=", ";
					s+=rtoss( m3.elements[i][j] );
				}
			}

			s=s+", "+rtoss(t.origin.x) +", "+rtoss(t.origin.y)+", "+rtoss(t.origin.z);

			f->store_string(s+" )");
		} break;

			// misc types
		case Variant::COLOR: {

			Color c = p_property;
			f->store_string("Color( "+rtoss(c.r) +", "+rtoss(c.g)+", "+rtoss(c.b)+", "+rtoss(c.a)+" )");

		} break;
		case Variant::IMAGE: {


			Image img=p_property;

			if (img.empty()) {
				f->store_string("Image()");
				break;
			}

			String imgstr="Image( ";
			imgstr+=itos(img.get_width());
			imgstr+=", "+itos(img.get_height());
			imgstr+=", "+itos(img.get_mipmaps());
			imgstr+=", ";

			switch(img.get_format()) {

				case Image::FORMAT_GRAYSCALE: imgstr+="GRAYSCALE"; break;
				case Image::FORMAT_INTENSITY: imgstr+="INTENSITY"; break;
				case Image::FORMAT_GRAYSCALE_ALPHA: imgstr+="GRAYSCALE_ALPHA"; break;
				case Image::FORMAT_RGB: imgstr+="RGB"; break;
				case Image::FORMAT_RGBA: imgstr+="RGBA"; break;
				case Image::FORMAT_INDEXED : imgstr+="INDEXED"; break;
				case Image::FORMAT_INDEXED_ALPHA: imgstr+="INDEXED_ALPHA"; break;
				case Image::FORMAT_BC1: imgstr+="BC1"; break;
				case Image::FORMAT_BC2: imgstr+="BC2"; break;
				case Image::FORMAT_BC3: imgstr+="BC3"; break;
				case Image::FORMAT_BC4: imgstr+="BC4"; break;
				case Image::FORMAT_BC5: imgstr+="BC5"; break;
				case Image::FORMAT_PVRTC2: imgstr+="PVRTC2"; break;
				case Image::FORMAT_PVRTC2_ALPHA: imgstr+="PVRTC2_ALPHA"; break;
				case Image::FORMAT_PVRTC4: imgstr+="PVRTC4"; break;
				case Image::FORMAT_PVRTC4_ALPHA: imgstr+="PVRTC4_ALPHA"; break;
				case Image::FORMAT_ETC: imgstr+="ETC"; break;
				case Image::FORMAT_ATC: imgstr+="ATC"; break;
				case Image::FORMAT_ATC_ALPHA_EXPLICIT: imgstr+="ATC_ALPHA_EXPLICIT"; break;
				case Image::FORMAT_ATC_ALPHA_INTERPOLATED: imgstr+="ATC_ALPHA_INTERPOLATED"; break;
				case Image::FORMAT_CUSTOM: imgstr+="CUSTOM"; break;
				default: {}
			}


			String s;

			DVector<uint8_t> data = img.get_data();
			int len = data.size();
			DVector<uint8_t>::Read r = data.read();
			const uint8_t *ptr=r.ptr();;
			for (int i=0;i<len;i++) {

				if (i>0)
					s+=", ";
				s+=itos(ptr[i]);
			}

			imgstr+=", ";
			f->store_string(imgstr);
			f->store_string(s);
			f->store_string(" )");
		} break;
		case Variant::NODE_PATH: {

			String str=p_property;

			str="NodePath(\""+str.c_escape()+"\")";
			f->store_string(str);

		} break;

		case Variant::OBJECT: {

			RES res = p_property;
			if (res.is_null()) {
				f->store_string("null");
				if (r_ok)
					*r_ok=true;

				break; // don't save it
			}

			if (external_resources.has(res)) {

				f->store_string("ExtResource( "+itos(external_resources[res]+1)+" )");
			} else {

				if (internal_resources.has(res)) {
					f->store_string("SubResource( "+itos(internal_resources[res])+" )");
				} else 	if (res->get_path().length() && res->get_path().find("::")==-1) {

					//external resource
					String path=relative_paths?local_path.path_to_file(res->get_path()):res->get_path();
					f->store_string("Resource( \""+path+"\" )");
				} else {
					f->store_string("null");
					ERR_EXPLAIN("Resource was not pre cached for the resource section, bug?");
					ERR_BREAK(true);
					//internal resource
				}
			}

		} break;
		case Variant::INPUT_EVENT: {

			f->store_string("InputEvent()"); //will be added later
		} break;
		case Variant::DICTIONARY: {

			Dictionary dict = p_property;

			List<Variant> keys;
			dict.get_key_list(&keys);
			keys.sort();

			f->store_string("{ ");
			for(List<Variant>::Element *E=keys.front();E;E=E->next()) {

				//if (!_check_type(dict[E->get()]))
				//	continue;
				bool ok;
				write_property("",E->get(),&ok);
				ERR_CONTINUE(!ok);

				f->store_string(":");
				write_property("",dict[E->get()],&ok);
				if (!ok)
					write_property("",Variant()); //at least make the file consistent..
				if (E->next())
					f->store_string(", ");
			}


			f->store_string(" }");


		} break;
		case Variant::ARRAY: {

			f->store_string("[ ");
			Array array = p_property;
			int len=array.size();
			for (int i=0;i<len;i++) {

				if (i>0)
					f->store_string(", ");
				write_property("",array[i]);


			}
			f->store_string(" ]");

		} break;

		case Variant::RAW_ARRAY: {

			f->store_string("ByteArray( ");
			String s;
			DVector<uint8_t> data = p_property;
			int len = data.size();
			DVector<uint8_t>::Read r = data.read();
			const uint8_t *ptr=r.ptr();;
			for (int i=0;i<len;i++) {

				if (i>0)
					f->store_string(", ");

				f->store_string(itos(ptr[i]));

			}

			f->store_string(" )");

		} break;
		case Variant::INT_ARRAY: {

			f->store_string("IntArray( ");
			DVector<int> data = p_property;
			int len = data.size();
			DVector<int>::Read r = data.read();
			const int *ptr=r.ptr();;

			for (int i=0;i<len;i++) {

				if (i>0)
					f->store_string(", ");

				f->store_string(itos(ptr[i]));
			}


			f->store_string(" )");

		} break;
		case Variant::REAL_ARRAY: {

			f->store_string("FloatArray( ");
			DVector<real_t> data = p_property;
			int len = data.size();
			DVector<real_t>::Read r = data.read();
			const real_t *ptr=r.ptr();;

			for (int i=0;i<len;i++) {

				if (i>0)
					f->store_string(", ");
				f->store_string(rtoss(ptr[i]));
			}

			f->store_string(" )");

		} break;
		case Variant::STRING_ARRAY: {

			f->store_string("StringArray( ");
			DVector<String> data = p_property;
			int len = data.size();
			DVector<String>::Read r = data.read();
			const String *ptr=r.ptr();;
			String s;
			//write_string("\n");



			for (int i=0;i<len;i++) {

				if (i>0)
					f->store_string(", ");
				String str=ptr[i];
				f->store_string(""+str.c_escape()+"\"");
			}

			f->store_string(" )");

		} break;
		case Variant::VECTOR2_ARRAY: {

			f->store_string("Vector2Array( ");
			DVector<Vector2> data = p_property;
			int len = data.size();
			DVector<Vector2>::Read r = data.read();
			const Vector2 *ptr=r.ptr();;

			for (int i=0;i<len;i++) {

				if (i>0)
					f->store_string(", ");
				f->store_string(rtoss(ptr[i].x)+", "+rtoss(ptr[i].y) );
			}

			f->store_string(" )");

		} break;
		case Variant::VECTOR3_ARRAY: {

			f->store_string("Vector3Array( ");
			DVector<Vector3> data = p_property;
			int len = data.size();
			DVector<Vector3>::Read r = data.read();
			const Vector3 *ptr=r.ptr();;

			for (int i=0;i<len;i++) {

				if (i>0)
					f->store_string(", ");
				f->store_string(rtoss(ptr[i].x)+", "+rtoss(ptr[i].y)+", "+rtoss(ptr[i].z) );
			}

			f->store_string(" )");

		} break;
		case Variant::COLOR_ARRAY: {

			f->store_string("ColorArray( ");

			DVector<Color> data = p_property;
			int len = data.size();
			DVector<Color>::Read r = data.read();
			const Color *ptr=r.ptr();;

			for (int i=0;i<len;i++) {

				if (i>0)
					f->store_string(", ");

				f->store_string(rtoss(ptr[i].r)+", "+rtoss(ptr[i].g)+", "+rtoss(ptr[i].b)+", "+rtoss(ptr[i].a) );

			}
			f->store_string(" )");

		} break;
		default: {}

	}

	if (r_ok)
		*r_ok=true;

}


void ResourceFormatSaverTextInstance::_find_resources(const Variant& p_variant,bool p_main) {


	switch(p_variant.get_type()) {
		case Variant::OBJECT: {


			RES res = p_variant.operator RefPtr();

			if (res.is_null() || external_resources.has(res))
				return;

			if (!p_main && (!bundle_resources ) && res->get_path().length() && res->get_path().find("::") == -1 ) {
				int index = external_resources.size();
				external_resources[res]=index;
				return;
			}

			if (resource_set.has(res))
				return;

			List<PropertyInfo> property_list;

			res->get_property_list( &property_list );
			property_list.sort();

			List<PropertyInfo>::Element *I=property_list.front();

			while(I) {

				PropertyInfo pi=I->get();

				if (pi.usage&PROPERTY_USAGE_STORAGE || (bundle_resources && pi.usage&PROPERTY_USAGE_BUNDLE)) {

					Variant v=res->get(I->get().name);
					_find_resources(v);
				}

				I=I->next();
			}

			resource_set.insert( res ); //saved after, so the childs it needs are available when loaded
			saved_resources.push_back(res);

		} break;
		case Variant::ARRAY: {

			Array varray=p_variant;
			int len=varray.size();
			for(int i=0;i<len;i++) {

				Variant v=varray.get(i);
				_find_resources(v);
			}

		} break;
		case Variant::DICTIONARY: {

			Dictionary d=p_variant;
			List<Variant> keys;
			d.get_key_list(&keys);
			for(List<Variant>::Element *E=keys.front();E;E=E->next()) {

				Variant v = d[E->get()];
				_find_resources(v);
			}
		} break;
		default: {}
	}

}



Error ResourceFormatSaverTextInstance::save(const String &p_path,const RES& p_resource,uint32_t p_flags) {

	if (p_path.ends_with(".tscn")) {
		packed_scene=p_resource;
	}

	Error err;
	f = FileAccess::open(p_path, FileAccess::WRITE,&err);
	ERR_FAIL_COND_V( err, ERR_CANT_OPEN );
	FileAccessRef _fref(f);

	local_path = Globals::get_singleton()->localize_path(p_path);

	relative_paths=p_flags&ResourceSaver::FLAG_RELATIVE_PATHS;
	skip_editor=p_flags&ResourceSaver::FLAG_OMIT_EDITOR_PROPERTIES;
	bundle_resources=p_flags&ResourceSaver::FLAG_BUNDLE_RESOURCES;
	takeover_paths=p_flags&ResourceSaver::FLAG_REPLACE_SUBRESOURCE_PATHS;
	if (!p_path.begins_with("res://")) {
		takeover_paths=false;
	}

	// save resources
	_find_resources(p_resource,true);

	if (packed_scene.is_valid()) {
		//add instances to external resources if saving a packed scene
		for(int i=0;i<packed_scene->get_state()->get_node_count();i++) {
			Ref<PackedScene> instance=packed_scene->get_state()->get_node_instance(i);
			if (instance.is_valid() && !external_resources.has(instance)) {
				int index = external_resources.size();
				external_resources[instance]=index;
			}
		}
	}


	ERR_FAIL_COND_V(err!=OK,err);

	{
		String title=packed_scene.is_valid()?"[gd_scene ":"[gd_resource ";
		if (packed_scene.is_null())
			title+="type=\""+p_resource->get_type()+"\" ";
		int load_steps=saved_resources.size()+external_resources.size();
		//if (packed_scene.is_valid()) {
		//	load_steps+=packed_scene->get_node_count();
		//}
		//no, better to not use load steps from nodes, no point to that

		if (load_steps>1) {
			title+="load_steps="+itos(load_steps)+" ";
		}
		title+="format="+itos(FORMAT_VERSION)+"";
		//title+="engine_version=\""+itos(VERSION_MAJOR)+"."+itos(VERSION_MINOR)+"\"";

		f->store_string(title);
		f->store_line("]\n"); //one empty line
	}


	for(Map<RES,int>::Element *E=external_resources.front();E;E=E->next()) {

		String p = E->key()->get_path();

		f->store_string("[ext_resource path=\""+p+"\" type=\""+E->key()->get_save_type()+"\" id="+itos(E->get()+1)+"]\n"); //bundled
	}

	if (external_resources.size())
		f->store_line(String()); //separate

	Set<int> used_indices;

	for(List<RES>::Element *E=saved_resources.front();E;E=E->next()) {

		RES res = E->get();
		if (E->next() && (res->get_path()=="" || res->get_path().find("::") != -1 )) {

			if (res->get_subindex()!=0) {
				if (used_indices.has(res->get_subindex())) {
					res->set_subindex(0); //repeated
				} else {
					used_indices.insert(res->get_subindex());
				}
			}
		}
	}

	for(List<RES>::Element *E=saved_resources.front();E;E=E->next()) {

		RES res = E->get();
		ERR_CONTINUE(!resource_set.has(res));
		bool main = (E->next()==NULL);

		if (main && packed_scene.is_valid())
			break; //save as a scene

		if (main) {
			f->store_line("[resource]\n");
		} else {
			String line="[sub_resource ";
			if (res->get_subindex()==0) {
				int new_subindex=1;
				if (used_indices.size()) {
					new_subindex=used_indices.back()->get()+1;
				}

				res->set_subindex(new_subindex);
				used_indices.insert(new_subindex);
			}

			int idx = res->get_subindex();
			line+="type=\""+res->get_type()+"\" id="+itos(idx);
			f->store_line(line+"]\n");
			if (takeover_paths) {
				res->set_path(p_path+"::"+itos(idx),true);
			}

			internal_resources[res]=idx;

		}


		List<PropertyInfo> property_list;
		res->get_property_list(&property_list);
//		property_list.sort();
		for(List<PropertyInfo>::Element *PE = property_list.front();PE;PE=PE->next()) {


			if (skip_editor && PE->get().name.begins_with("__editor"))
				continue;

			if (PE->get().usage&PROPERTY_USAGE_STORAGE || (bundle_resources && PE->get().usage&PROPERTY_USAGE_BUNDLE)) {

				String name = PE->get().name;
				Variant value = res->get(name);


				if ((PE->get().usage&PROPERTY_USAGE_STORE_IF_NONZERO && value.is_zero())||(PE->get().usage&PROPERTY_USAGE_STORE_IF_NONONE && value.is_one()) )
					continue;

				if (PE->get().type==Variant::OBJECT && value.is_zero())
					continue;

				write_property(name,value);
				f->store_string("\n");
			}


		}

		f->store_string("\n");

	}

	if (packed_scene.is_valid()) {
		//if this is a scene, save nodes and connections!
		Ref<SceneState> state = packed_scene->get_state();
		for(int i=0;i<state->get_node_count();i++) {

			StringName type = state->get_node_type(i);
			StringName name = state->get_node_name(i);
			NodePath path = state->get_node_path(i,true);
			NodePath owner = state->get_node_owner_path(i);
			Ref<PackedScene> instance = state->get_node_instance(i);
			Vector<StringName> groups = state->get_node_groups(i);

			if (instance.is_valid())
				print_line("for path "+String(path)+" instance "+instance->get_path());

			String header="[node";
			header+=" name=\""+String(name)+"\"";
			if (type!=StringName()) {
				header+=" type=\""+String(type)+"\"";
			}
			if (path!=NodePath()) {
				header+=" parent=\""+String(path.simplified())+"\"";
			}
			if (owner!=NodePath() && owner!=NodePath(".")) {
				header+=" owner=\""+String(owner.simplified())+"\"";
			}

			if (groups.size()) {
				String sgroups=" groups=[ ";
				for(int j=0;j<groups.size();j++) {
					if (j>0)
						sgroups+=", ";
					sgroups+="\""+groups[j].operator String().c_escape()+"\"";
				}
				sgroups+=" ]";
				header+=sgroups;
			}

			f->store_string(header);

			if (instance.is_valid()) {
				f->store_string(" instance=");
				write_property("",instance);
			}

			f->store_line("]\n");

			for(int j=0;j<state->get_node_property_count(i);j++) {

				write_property(state->get_node_property_name(i,j),state->get_node_property_value(i,j));
				f->store_line(String());

			}

			if (state->get_node_property_count(i)) {
				//add space
				f->store_line(String());
			}

		}

		for(int i=0;i<state->get_connection_count();i++) {

			String connstr="[connection";
			connstr+=" signal=\""+String(state->get_connection_signal(i))+"\"";
			connstr+=" from=\""+String(state->get_connection_source(i).simplified())+"\"";
			connstr+=" to=\""+String(state->get_connection_target(i).simplified())+"\"";
			connstr+=" method=\""+String(state->get_connection_method(i))+"\"";
			int flags = state->get_connection_flags(i);
			if (flags!=Object::CONNECT_PERSIST) {
				connstr+=" flags="+itos(flags);
			}

			Array binds=state->get_connection_binds(i);
			f->store_string(connstr);
			if (binds.size()) {
				f->store_string(" binds=");
				write_property("",binds);
			}

			f->store_line("]\n");
		}

		f->store_line(String());

		Vector<NodePath> editable_instances = state->get_editable_instances();
		for(int i=0;i<editable_instances.size();i++) {
			f->store_line("[editable path=\""+editable_instances[i].operator String()+"\"]");
		}
	}

	if (f->get_error()!=OK && f->get_error()!=ERR_FILE_EOF) {
		f->close();
		return ERR_CANT_CREATE;
	}

	f->close();
	//memdelete(f);

	return OK;
}



Error ResourceFormatSaverText::save(const String &p_path,const RES& p_resource,uint32_t p_flags) {

	if (p_path.ends_with(".sct") && p_resource->get_type()!="PackedScene") {
		return ERR_FILE_UNRECOGNIZED;
	}

	ResourceFormatSaverTextInstance saver;
	return saver.save(p_path,p_resource,p_flags);

}

bool ResourceFormatSaverText::recognize(const RES& p_resource) const {


	return true; // all recognized!
}
void ResourceFormatSaverText::get_recognized_extensions(const RES& p_resource,List<String> *p_extensions) const {

	p_extensions->push_back("tres"); //text resource
	if (p_resource->get_type()=="PackedScene")
		p_extensions->push_back("tscn"); //text scene

}

ResourceFormatSaverText* ResourceFormatSaverText::singleton=NULL;
ResourceFormatSaverText::ResourceFormatSaverText() {
	singleton=this;
}

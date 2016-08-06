#ifndef VSCRIPT_H
#define VSCRIPT_H

#include "script_language.h"


class VisualScriptInstance;
class VisualScriptNodeInstance;
class VisualScript;

class VisualScriptNode : public Resource {
	OBJ_TYPE(VisualScriptNode,Resource)

friend class VisualScript;

	Set<VisualScript*> scripts_used;

	Array default_input_values;

	void _set_default_input_values(Array p_values);
	Array _get_default_input_values() const;
protected:

	void _notification(int p_what);
	void ports_changed_notify();
	static void _bind_methods();
public:

	Ref<VisualScript> get_visual_script() const;

	virtual int get_output_sequence_port_count() const=0;
	virtual bool has_input_sequence_port() const=0;

	virtual String get_output_sequence_port_text(int p_port) const=0;

	virtual int get_input_value_port_count() const=0;
	virtual int get_output_value_port_count() const=0;


	virtual PropertyInfo get_input_value_port_info(int p_idx) const=0;
	virtual PropertyInfo get_output_value_port_info(int p_idx) const=0;

	void set_default_input_value(int p_port,const Variant& p_value);
	Variant get_default_input_value(int p_port) const;

	virtual String get_caption() const=0;
	virtual String get_text() const=0;
	virtual String get_category() const=0;

	virtual VisualScriptNodeInstance* instance(VisualScriptInstance* p_instance)=0;

};


class VisualScriptNodeInstance {
friend class VisualScriptInstance;



	enum { //input argument addressing
		INPUT_SHIFT=1<<24,
		INPUT_MASK=INPUT_SHIFT-1,
		INPUT_DEFAULT_VALUE_BIT=INPUT_SHIFT, // from unassigned input port, using default value (edited by user)
		INPUT_UNSEQUENCED_READ_BIT=INPUT_SHIFT<<1, //from unsequenced read (requires calling a function, used for constants, variales, etc).
	};


	int id;
	int sequence_index;
	VisualScriptNodeInstance **sequence_outputs;
	int sequence_output_count;
	int *input_ports;
	int input_port_count;
	int *output_ports;
	int output_port_count;
	int working_mem_idx;

	VisualScriptNode *base;

public:

	enum {
		STEP_SHIFT=1<<24,
		STEP_MASK=STEP_SHIFT-1,
		STEP_FLAG_PUSH_STACK_BIT=STEP_SHIFT, //push bit to stack
		STEP_FLAG_GO_BACK_BIT=STEP_SHIFT<<1, //go back to previous node
		STEP_EXIT_FUNCTION_BIT=STEP_SHIFT<<2, //return from function

		FLOW_STACK_PUSHED_BIT=1<<30, //in flow stack, means bit was pushed (must go back here if end of sequence)
		FLOW_STACK_MASK=FLOW_STACK_PUSHED_BIT-1

	};

	_FORCE_INLINE_ int  get_input_port_count() const { return input_port_count; }
	_FORCE_INLINE_ int  get_output_port_count() const { return output_port_count; }
	_FORCE_INLINE_ int  get_sequence_output_count() const { return sequence_output_count; }

	_FORCE_INLINE_ int  get_id() const { return id; }

	virtual int get_working_memory_size() const { return 0; }

	//unsequenced ports are those that can return a value even if no sequence happened through them, used for constants, variables, etc.
	virtual bool is_output_port_unsequenced(int p_idx) const { return false; }
	virtual bool get_output_port_unsequenced(int p_idx,Variant* r_value,Variant* p_working_mem,String &r_error) const { return true; }

	virtual int step(const Variant** p_inputs,Variant** p_outputs,bool p_start_sequence,Variant* p_working_mem,Variant::CallError& r_error,String& r_error_str)=0; //do a step, return which sequence port to go out

	Ref<VisualScriptNode> get_base_node() { return Ref<VisualScriptNode>( base ); }

	VisualScriptNodeInstance();
	virtual ~VisualScriptNodeInstance();
};


class VisualScript : public Script {

	OBJ_TYPE( VisualScript, Script )

	RES_BASE_EXTENSION("vs");

public:

	struct SequenceConnection {

		union {

			struct {
				uint64_t from_node : 24;
				uint64_t from_output : 16;
				uint64_t to_node : 24;
			};
			uint64_t id;
		};

		bool operator<(const SequenceConnection& p_connection) const {

			return id<p_connection.id;
		}
	};

	struct DataConnection {

		union {

			struct {
				uint64_t from_node : 24;
				uint64_t from_port : 8;
				uint64_t to_node : 24;
				uint64_t to_port : 8;
			};
			uint64_t id;
		};

		bool operator<(const DataConnection& p_connection) const {

			return id<p_connection.id;
		}
	};


private:
friend class VisualScriptInstance;

	StringName base_type;
	struct Argument {
		String name;
		Variant::Type type;
	};

	struct Function {
		struct NodeData {
			Point2 pos;
			Ref<VisualScriptNode> node;
		};

		Map<int,NodeData> nodes;

		Set<SequenceConnection> sequence_connections;

		Set<DataConnection> data_connections;

		int function_id;


		Function() { function_id=-1; }

	};

	struct Variable {
		PropertyInfo info;
		Variant default_value;
	};



	Map<StringName,Function> functions;
	Map<StringName,Variable> variables;
	Map<StringName,StringName> script_variable_remap;
	Map<StringName,Vector<Argument> > custom_signals;

	Map<Object*,VisualScriptInstance*> instances;

#ifdef TOOLS_ENABLED
	Set<PlaceHolderScriptInstance*> placeholders;
	//void _update_placeholder(PlaceHolderScriptInstance *p_placeholder);
	virtual void _placeholder_erased(PlaceHolderScriptInstance *p_placeholder);
	void _update_placeholders();
#endif

	void _set_variable_info(const StringName& p_name,const Dictionary& p_info);
	Dictionary _get_variable_info(const StringName& p_name) const;


	void _set_data(const Dictionary& p_data);
	Dictionary _get_data() const;

protected:

	void _node_ports_changed(int p_id);
	static void _bind_methods();
public:


	void add_function(const StringName& p_name);
	bool has_function(const StringName& p_name) const;
	void remove_function(const StringName& p_name);
	void rename_function(const StringName& p_name,const StringName& p_new_name);
	void get_function_list(List<StringName> *r_functions) const;
	int get_function_node_id(const StringName& p_name) const;


	void add_node(const StringName& p_func,int p_id,const Ref<VisualScriptNode>& p_node,const Point2& p_pos=Point2());
	void remove_node(const StringName& p_func,int p_id);
	Ref<VisualScriptNode> get_node(const StringName& p_func,int p_id) const;
	void set_node_pos(const StringName& p_func,int p_id,const Point2& p_pos);
	Point2 get_node_pos(const StringName& p_func,int p_id) const;
	void get_node_list(const StringName& p_func,List<int> *r_nodes) const;

	void sequence_connect(const StringName& p_func,int p_from_node,int p_from_output,int p_to_node);
	void sequence_disconnect(const StringName& p_func,int p_from_node,int p_from_output,int p_to_node);
	bool has_sequence_connection(const StringName& p_func,int p_from_node,int p_from_output,int p_to_node) const;
	void get_sequence_connection_list(const StringName& p_func,List<SequenceConnection> *r_connection) const;

	void data_connect(const StringName& p_func,int p_from_node,int p_from_port,int p_to_node,int p_to_port);
	void data_disconnect(const StringName& p_func,int p_from_node,int p_from_port,int p_to_node,int p_to_port);
	bool has_data_connection(const StringName& p_func,int p_from_node,int p_from_port,int p_to_node,int p_to_port) const;
	void get_data_connection_list(const StringName& p_func,List<DataConnection> *r_connection) const;
	bool is_input_value_port_connected(const StringName& p_name,int p_node,int p_port) const;

	void add_variable(const StringName& p_name,const Variant& p_default_value=Variant());
	bool has_variable(const StringName& p_name) const;
	void remove_variable(const StringName& p_name);
	void set_variable_default_value(const StringName& p_name,const Variant& p_value);
	Variant get_variable_default_value(const StringName& p_name) const;
	void set_variable_info(const StringName& p_name,const PropertyInfo& p_info);
	PropertyInfo get_variable_info(const StringName& p_name) const;
	void get_variable_list(List<StringName> *r_variables);
	void rename_variable(const StringName& p_name,const StringName& p_new_name);


	void add_custom_signal(const StringName& p_name);
	bool has_custom_signal(const StringName& p_name) const;
	void custom_signal_add_argument(const StringName& p_func,Variant::Type p_type,const String& p_name,int p_index=-1);
	void custom_signal_set_argument_type(const StringName& p_func,int p_argidx,Variant::Type p_type);
	Variant::Type custom_signal_get_argument_type(const StringName& p_func,int p_argidx) const;
	void custom_signal_set_argument_name(const StringName& p_func,int p_argidx,const String& p_name);
	String custom_signal_get_argument_name(const StringName& p_func,int p_argidx) const;
	void custom_signal_remove_argument(const StringName& p_func,int p_argidx);
	int custom_signal_get_argument_count(const StringName& p_func) const;
	void custom_signal_swap_argument(const StringName& p_func,int p_argidx,int p_with_argidx);
	void remove_custom_signal(const StringName& p_name);
	void rename_custom_signal(const StringName& p_name,const StringName& p_new_name);

	void get_custom_signal_list(List<StringName> *r_custom_signals) const;

	int get_available_id() const;

	void set_instance_base_type(const StringName& p_type);

	virtual bool can_instance() const;

	virtual StringName get_instance_base_type() const;
	virtual ScriptInstance* instance_create(Object *p_this);
	virtual bool instance_has(const Object *p_this) const;


	virtual bool has_source_code() const;
	virtual String get_source_code() const;
	virtual void set_source_code(const String& p_code);
	virtual Error reload(bool p_keep_state=false);

	virtual bool is_tool() const;

	virtual String get_node_type() const;

	virtual ScriptLanguage *get_language() const;

	virtual bool has_script_signal(const StringName& p_signal) const;
	virtual void get_script_signal_list(List<MethodInfo> *r_signals) const;

	virtual bool get_property_default_value(const StringName& p_property,Variant& r_value) const;
	virtual void get_method_list(List<MethodInfo> *p_list) const;


	VisualScript();
	~VisualScript();
};


class VisualScriptInstance : public ScriptInstance {

	Object *owner;
	Ref<VisualScript> script;

	Map<StringName,Variant> variables; //using variable path, not script
	Map<int,VisualScriptNodeInstance*> instances;

	struct Function {
		int node;
		int max_stack;
		int trash_pos;
		int return_pos;
		int flow_stack_size;
		int node_count;
		int argument_count;
		bool valid;

		struct UnsequencedGet {
			VisualScriptNodeInstance* from;
			int from_port;
			int to_stack;
		};

		Vector<UnsequencedGet> unsequenced_gets;

	};

	Map<StringName,Function> functions;

	Vector<Variant> default_values;
	int max_input_args,max_output_args;


	//Map<StringName,Function> functions;

public:
	virtual bool set(const StringName& p_name, const Variant& p_value);
	virtual bool get(const StringName& p_name, Variant &r_ret) const;
	virtual void get_property_list(List<PropertyInfo> *p_properties) const;
	virtual Variant::Type get_property_type(const StringName& p_name,bool *r_is_valid=NULL) const;

	virtual void get_method_list(List<MethodInfo> *p_list) const;
	virtual bool has_method(const StringName& p_method) const;
	virtual Variant call(const StringName& p_method,const Variant** p_args,int p_argcount,Variant::CallError &r_error);
	virtual void notification(int p_notification);

	bool set_variable(const StringName& p_variable,const Variant& p_value) {

		Map<StringName,Variant>::Element *E=variables.find(p_variable);
		if (!E)
			return false;

		E->get()=p_value;
		return true;
	}

	bool get_variable(const StringName& p_variable,Variant* r_variable) const {

		const Map<StringName,Variant>::Element *E=variables.find(p_variable);
		if (!E)
			return false;

		*r_variable=E->get();
		return true;

	}

	virtual Ref<Script> get_script() const;

	_FORCE_INLINE_ VisualScript *get_script_ptr() { return script.ptr(); }
	_FORCE_INLINE_ Object *get_owner_ptr() { return owner; }

	void create(const Ref<VisualScript>& p_script,Object *p_owner);

	virtual ScriptLanguage *get_language();

	VisualScriptInstance();
	~VisualScriptInstance();
};



typedef Ref<VisualScriptNode> (*VisualScriptNodeRegisterFunc)(const String& p_type);

class VisualScriptLanguage : public ScriptLanguage {

	Map<String,VisualScriptNodeRegisterFunc> register_funcs;


public:
	StringName notification;

	static VisualScriptLanguage* singleton;

	Mutex *lock;

	virtual String get_name() const;

	/* LANGUAGE FUNCTIONS */
	virtual void init();
	virtual String get_type() const;
	virtual String get_extension() const;
	virtual Error execute_file(const String& p_path) ;
	virtual void finish();

	/* EDITOR FUNCTIONS */
	virtual void get_reserved_words(List<String> *p_words) const;
	virtual void get_comment_delimiters(List<String> *p_delimiters) const;
	virtual void get_string_delimiters(List<String> *p_delimiters) const;
	virtual Ref<Script> get_template(const String& p_class_name, const String& p_base_class_name) const;
	virtual bool validate(const String& p_script, int &r_line_error,int &r_col_error,String& r_test_error, const String& p_path="",List<String> *r_functions=NULL) const;
	virtual Script *create_script() const;
	virtual bool has_named_classes() const;
	virtual int find_function(const String& p_function,const String& p_code) const;
	virtual String make_function(const String& p_class,const String& p_name,const StringArray& p_args) const;
	virtual void auto_indent_code(String& p_code,int p_from_line,int p_to_line) const;
	virtual void add_global_constant(const StringName& p_variable,const Variant& p_value);


	/* DEBUGGER FUNCTIONS */

	virtual String debug_get_error() const;
	virtual int debug_get_stack_level_count() const;
	virtual int debug_get_stack_level_line(int p_level) const;
	virtual String debug_get_stack_level_function(int p_level) const;
	virtual String debug_get_stack_level_source(int p_level) const;
	virtual void debug_get_stack_level_locals(int p_level,List<String> *p_locals, List<Variant> *p_values, int p_max_subitems=-1,int p_max_depth=-1);
	virtual void debug_get_stack_level_members(int p_level,List<String> *p_members, List<Variant> *p_values, int p_max_subitems=-1,int p_max_depth=-1);
	virtual void debug_get_globals(List<String> *p_locals, List<Variant> *p_values, int p_max_subitems=-1,int p_max_depth=-1);
	virtual String debug_parse_stack_level_expression(int p_level,const String& p_expression,int p_max_subitems=-1,int p_max_depth=-1);


	virtual void reload_all_scripts();
	virtual void reload_tool_script(const Ref<Script>& p_script,bool p_soft_reload);
	/* LOADER FUNCTIONS */

	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual void get_public_functions(List<MethodInfo> *p_functions) const;
	virtual void get_public_constants(List<Pair<String,Variant> > *p_constants) const;

	virtual void profiling_start();
	virtual void profiling_stop();

	virtual int profiling_get_accumulated_data(ProfilingInfo *p_info_arr,int p_info_max);
	virtual int profiling_get_frame_data(ProfilingInfo *p_info_arr,int p_info_max);

	void add_register_func(const String& p_name,VisualScriptNodeRegisterFunc p_func);
	Ref<VisualScriptNode> create_node_from_name(const String& p_name);
	void get_registered_node_names(List<String> *r_names);


	VisualScriptLanguage();
	~VisualScriptLanguage();

};

//aid for registering
template<class T>
static Ref<VisualScriptNode> create_node_generic(const String& p_name) {

	Ref<T> node;
	node.instance();
	return node;
}



#endif // VSCRIPT_H

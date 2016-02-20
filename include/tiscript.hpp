#ifndef __tis_hpp__
#define __tis_hpp__

#include <assert.h>
#include "tiscript.h"

#if defined(__cplusplus)

#include "tiscript-streams.hpp"

namespace tiscript
{
  inline tiscript_native_interface* ni(tiscript_native_interface* ni = nullptr)
  {
    static tiscript_native_interface* _ni = nullptr;
    if( ni ) _ni = ni;
    return _ni;
  }

  //typedef std::ustring wstring;
  //std::ustring

  typedef tiscript_value  value;
  typedef tiscript_VM     VM;

  inline HVM  create_vm(unsigned features = 0xffffffff, unsigned heap_size = 1*1024*1024, unsigned stack_size = 64*1024 )
  {
     return ni()->create_vm(features,heap_size, stack_size);
  }
  inline void destroy_vm(HVM vm) { ni()->destroy_vm(vm); }

  // set stdin, stdout and stderr for this VM
  inline void  set_std_streams(HVM vm, stream* input, stream* output, stream* error) {  ni()->set_std_streams(vm, input, output, error); }

  inline HVM   get_current_vm()         { return ni()->get_current_vm(); }
  inline value get_global_ns(HVM vm)    { return ni()->get_global_ns(vm); }
  inline value get_current_ns(HVM vm)   { return ni()->get_current_ns(vm); }
  inline void  invoke_gc(HVM vm)        { ni()->invoke_gc(vm); }

  struct runtime
  {
    HVM vm;
    runtime(unsigned features = 0xffffffff, unsigned heap_size = 1*1024*1024, unsigned stack_size = 64*1024)
    {
      vm = create_vm(features, heap_size,stack_size);
      assert(vm);
    }
    ~runtime() { destroy_vm(vm); }
    operator HVM() { return vm; } // so runtime reference can be used in places where HVM is required

    value global_ns()          { assert(vm); return get_global_ns(vm); }
    value current_ns()         { assert(vm); return get_current_ns(vm); }
    void  run_gc()             { assert(vm); invoke_gc(vm); }
  };

  inline bool  is_int(value v)          { return ni()->is_int(v); }
  inline bool  is_float(value v)        { return ni()->is_float(v); }
  inline bool  is_symbol(value v)       { return ni()->is_symbol(v); }
  inline bool  is_string(value v)       { return ni()->is_string(v); }
  inline bool  is_array(value v)        { return ni()->is_array(v); }
  inline bool  is_object(value v)       { return ni()->is_object(v); }
  inline bool  is_native_object(value v){ return ni()->is_native_object(v); }
  inline bool  is_function(value v)     { return ni()->is_function(v); }
  inline bool  is_native_function(value v)  { return ni()->is_native_function(v); }
  inline bool  is_instance_of(value v, value cls) { return ni()->is_instance_of(v,cls); }
  inline bool  is_undefined(value v)    { return ni()->is_undefined(v); }
  inline bool  is_nothing(value v)      { return ni()->is_nothing(v); }
  inline bool  is_null(value v)         { return ni()->is_null(v); }
  inline bool  is_true(value v)         { return ni()->is_true(v); }
  inline bool  is_false(value v)        { return ni()->is_false(v); }
  inline bool  is_bool(value v)         { return is_true(v) || is_false(v); }
  inline bool  is_class(HVM vm,value v) { return ni()->is_class(vm,v); }
  inline bool  is_error(value v)        { return ni()->is_error(v); }
  inline bool  is_bytes(value v)        { return ni()->is_bytes(v); }

  // to C/C++ type from the value
  inline int          c_int(value v)     { int dv = 0;         ni()->get_int_value(v,&dv); return dv; }
  inline double       c_float(value v)   { double dv = 0;      ni()->get_float_value(v,&dv); return dv; }
  inline bool         c_bool(value v)    { bool dv = false;    ni()->get_bool_value(v,&dv); return dv; }
  inline std::ustring c_symbol(value v)  { const WCHAR* dv = WSTR("");  ni()->get_symbol_value(v,&dv); return dv; }
  inline std::ustring c_string(value v)  { const WCHAR* dv = WSTR(""); unsigned len = 0; ni()->get_string_value(v,&dv,&len); return std::ustring(dv,len); }
  inline bool         c_bytes(value v, unsigned char* &data, unsigned &datalen) { return ni()->get_bytes(v,&data,&datalen); }

  // to script value from C/C++ type
  inline value        v_nothing()             { static value _v = ni()->nothing_value(); return _v; }   // designates ultimate "does not exist" situation.
  inline value        v_undefined()           { static value _v = ni()->undefined_value(); return _v; } // non-initialized or non-existent value
  inline value        v_null()                { static value _v = ni()->null_value(); return _v; }      // explicit "no object" value
  inline value        v_bool(bool v)          { return ni()->bool_value(v); }
  inline value        v_int(int v)            { return ni()->int_value(v); }
  inline value        v_float(double v)       { return ni()->float_value(v); }
  inline value        v_symbol(const char* v) { return ni()->symbol_value(v); }   // symbol is an int - perfect hash value of the string
                                                                                  // used as property/function names (keys) of objects
                                                                                  // symbol !== string, but convertable to it.
                                                                                  // symbol is not GCable, once created can be stored anywhere and
                                                                                  // yet shared between different VMs.

  inline value        v_string(HVM vm, const WCHAR* str, unsigned len = 0) { return ni()->string_value(vm,str,len); }
                                                                                  // the string of course.
  inline value        v_string(const WCHAR* str, unsigned len = 0) { HVM vm = get_current_vm(); assert(vm); return v_string(vm,str,len); }
                                                                                  // the string of course.

  inline value        v_bytes(HVM vm, const unsigned char* data, unsigned datalen) { return ni()->bytes_value(vm,data,datalen); }
                                                                                  // make instance of Bytes object in script -
                                                                                  // sequence of bytes.
                                                                                  // Be notified: Bytes is a citizen of GCable heap. Use pinned thing to hold it
  inline value        v_bytes(const unsigned char* data, unsigned datalen) { HVM vm = get_current_vm(); assert(vm);
                                                                             return v_bytes(vm,data,datalen); }

  // convert value to string represenatation.
  inline std::ustring to_string(HVM vm,value v) { return c_string(ni()->to_string(vm,v)); }
  inline std::ustring to_string(value v) { HVM vm = get_current_vm(); assert(vm);
                                           return to_string(vm,v); }

  // path here is a global "path" of the object, something like: "one", "one.two", etc.
  inline value        value_by_path(HVM vm, const char* path) { value r = v_undefined(); ni()->get_value_by_path(vm, &r, path); return r; }
  inline value        value_by_path(const char* path) { HVM vm = get_current_vm(); assert(vm); value r = v_undefined(); ni()->get_value_by_path(vm, &r, path); return r; }

//@region Object

  // object creation, of_class == 0 - "Object"
  inline value  create_object(HVM vm, value of_class = 0) { return ni()->create_object(vm, of_class); }
  inline value  create_object(HVM vm, const char* class_path )
  {
      value cls = value_by_path(vm, class_path); assert( is_class(vm,cls) );
      return ni()->create_object(vm, cls);
  }

  // object propery access.
  inline bool  set_prop(HVM vm, value obj, value key, value value) { return ni()->set_prop(vm,obj,key,value); }
  inline value get_prop(HVM vm, value obj, value key) { return ni()->get_prop(vm,obj,key); }

  inline bool  set_prop(HVM vm, value obj, const char* key, value value) { return set_prop(vm,obj,v_symbol(key),value); }
  inline value get_prop(HVM vm, value obj, const char* key)              { return get_prop(vm,obj,v_symbol(key)); }

  // get/set users data associated with instance of native object
  inline void* get_native_data( value obj ) { assert(is_native_object(obj)); return ni()->get_instance_data(obj); }
  inline void  set_native_data( value obj, void* data ) { assert(is_native_object(obj)); ni()->set_instance_data(obj,data); }

//@region Array

  inline value     create_array(HVM vm, unsigned of_size) { return ni()->create_array(vm,of_size); }
  inline bool      set_elem(HVM vm, value arr, unsigned idx, value val) { return ni()->set_elem(vm,arr,idx,val); }
  inline value     get_elem(HVM vm, value arr, unsigned idx) { return ni()->get_elem(vm,arr,idx); }
  inline unsigned  get_length(HVM vm, value arr) { return ni()->get_array_size(vm,arr); }
  // reallocates the array and returns reallocated (if needed) array
  inline value     set_length(HVM vm, value arr, unsigned of_size) { return ni()->set_array_size(vm,arr,of_size); }

  // informs VM that native method got an error condition. Native method should return from the function after the call.
  inline void      throw_error( HVM vm, const WCHAR* error_text) { ni()->throw_error( vm, error_text ); }
  inline void      throw_error( HVM vm, const std::basic_string<WCHAR> error_text) { ni()->throw_error( vm, error_text.c_str() ); }

  inline value     eval(HVM vm, value ns, stream* input, bool template_mode = false)
  {
    value rv = 0;
    if(ni()->eval(vm, ns, input, template_mode, &rv))
      return rv;
    else
      return v_undefined();
  }
  inline value     eval(HVM vm, stream& input, bool template_mode = false) { return eval( vm, get_current_ns(vm), &input, template_mode); }
  inline value     eval(HVM vm, value ns, const WCHAR* text)
  {
    value rv = 0;
    if(ni()->eval_string(vm, ns, text, (unsigned int)aux::wcslen(text), &rv))
      return rv;
    else
      return v_undefined();
  }
  inline value     eval(HVM vm, const WCHAR* text) { return eval( vm, get_current_ns(vm), text); }

  // call method
  inline value     call(HVM vm, value This, value function, const value* argv = 0, unsigned argn = 0)
  {
    value rv = 0;
    if( ni()->call(vm, This, function, argv, argn,&rv) )
      return rv;
    else
      return v_undefined();
  }
  inline value     call(HVM vm, value obj, const char* funcname, const value* argv = 0, unsigned argn = 0)
  {
    value function = get_prop(vm, obj, funcname);
    if( is_function(function) || is_native_function(function))
      return call(vm, obj, function, argv, argn);
    else
      return v_undefined();
  }

  // call global function

  inline value     call(HVM vm, value function, const value* argv = 0, unsigned argn = 0) { return call(vm, get_current_ns(vm), function, argv, argn); }
  inline value     call(HVM vm, const char* funcpath, const value* argv = 0, unsigned argn = 0)
  {
    value function  = value_by_path(vm,funcpath);
    if( is_function(function) || is_native_function(function))
      return call(vm, get_current_ns(vm), function, argv, argn);
    else
      return v_undefined();
  }

  // compile bytecodes
  inline bool     compile( HVM vm, stream& input, stream& output_bytecodes, bool template_mode = false )
    { return ni()->compile( vm, &input, &output_bytecodes, template_mode); }
  // load bytecodes
  inline bool     loadbc( HVM vm, stream* input_bytecodes )
    { return ni()->loadbc(vm,input_bytecodes); }

  // Schedule execution of the pfunc(prm) in the thread owning this VM.
  // Used when you need to call scripting methods from threads other than main (GUI) thread
  // It is safe to call tiscript functions inside the pfunc.
  // returns 'true' if scheduling of the call was accepted, 'false' when failure (VM has no dispatcher attached).
  inline bool      post( HVM pvm, tiscript_callback* pfunc, void* prm)
    { return ni()->post(pvm,pfunc,prm); }

  // pinned value, a.k.a. gc root variable.
  // use pinned values when you need to store the value for long time
  class pinned: public tiscript_pvalue
  {
    friend class args;
  private:
    pinned(const pinned& p);
    pinned operator = (const pinned& p);
  public:
    pinned()                { val = 0, vm = 0, d1 = d2 = 0; }
    pinned(HVM c)           { val = 0, vm = 0, d1 = d2 = 0;  ni()->pin(c,this); }
    pinned(HVM c, value v)  { val = 0, vm = 0, d1 = d2 = 0;  ni()->pin(c,this); val = v; }
    virtual ~pinned()       { detach(); }

    void attach(HVM c){ detach(); ni()->pin(c,this); }
    void detach()     { if(vm) ni()->unpin(this); }
    HVM  get_vm()     { return vm; }

    operator value()  { return val; }
    pinned& operator = (value v) { val = v; assert(vm); return *this; }
    bool is_set() const { return val != 0; }
  };

  // object reference wrapper + object related accessor functions

  class object_ref: pinned
  {
  public:
    object_ref():pinned()                 { }
    object_ref(HVM c):pinned(c)           { }
    object_ref(HVM c,value obj):pinned(c) { assign(obj); }

    // create new object [of class]
    void create(value of_class = 0) { val = ni()->create_object(get_vm(), of_class); }
    void create(const char* class_path )
    {
        value cls = value_by_path(get_vm(), class_path); assert( is_class(vm,cls) );
        val = ni()->create_object(get_vm(), cls);
    }

    operator value()  { return val; }
    object_ref& operator = (value v) {  assert(get_vm()); assign(v); return *this; }

    // accessors

    int length()  { return (val && vm) ? ni()->get_length(vm,val) : 0; }

    // var v = obj[key]; obj[key] = v;
    value get(value key)              { assert(is_set()); return ni()->get_prop(vm,val,key); }
    bool  set(value key, value value) { assert(is_set()); return ni()->set_prop(vm,val,key,value); }

    // var v = obj.key; obj.key = v;
    bool  set(const char* key, value value) { assert(is_set()); return ni()->set_prop(vm,val,v_symbol(key),value); }
    value get(const char* key)              { assert(is_set()); return ni()->get_prop(vm,val,v_symbol(key)); }

    // var v = obj["key"]; obj["key"] = v;
    bool  set(const WCHAR* key, value v)  { assert(is_set()); pinned pv(get_vm(),v); value k = v_string(get_vm(),key); return ni()->set_prop(vm,val,k,pv); }
    value get(const WCHAR* key)           { assert(is_set()); value k = v_string(get_vm(),key); return ni()->get_prop(vm,val,k); }

    //void* native_data()        { assert(is_set()); return get_native_data(val); }
    //bool  native_data(void* p) { assert(is_set()); return set_native_data(val,p); }

    // native data accessors.
    // use them as:
    //   custom_type *ptr = obj.data<custom_type*>();
    //   obj.data(ptr);

    template <typename NDT>
      NDT  data()        { assert(is_set()); return static_cast<NDT>(get_native_data(val)); }
    template <typename NDT>
      void data(NDT ptr) { assert(is_set()); return set_native_data(val,static_cast<void*>(ptr)); }

    private:
      friend class args;
      object_ref(const object_ref& p);
      object_ref operator = (const object_ref& p);
      void assign( value v ) { val = v; assert( is_object(v) || is_native_object(v) ); }

      bool  set(value key, int value);       // to prevent upcasting from int to tiscript_value
      bool  set(const char* key, int value);
      bool  set(const WCHAR* key, int v);
  };

  // array reference wrapper + array related accessor functions
  class array_ref: pinned
  {
  public:
    array_ref():pinned(get_current_vm())     { }
    array_ref(HVM c):pinned(c)     { }
    array_ref(HVM c,value obj):pinned(c) { assign(obj); }
    operator value()  { return val; }
    array_ref& operator = (value v) {  assert(get_vm()); assign(v); return *this; }

    // create new array
    void create(unsigned num_elements = 0) { val = ni()->create_array(get_vm(), num_elements); }

    // accessors
    unsigned length()  { return (val && vm)? (unsigned)ni()->get_length(vm,val) : 0; }

    // var v = arr[n]; arr[n] = v;
    value get(unsigned i)              { assert(is_set()); return ni()->get_elem(vm,val,i); }
    bool  set(unsigned i, value value) { assert(is_set()); return ni()->set_elem(vm,val,i, value); }
    // arr.push(v);
    bool  push(value value)
    {
      pinned pv(get_vm(),value);
      assert(is_set());
      unsigned l = length();
      val = ni()->set_array_size(vm,val,l + 1);
      return ni()->set_elem(vm,val,l,pv);
    }

  private:
    friend class args;
    array_ref(const array_ref& p);
    array_ref operator = (const array_ref& p);
    void assign( value v ) { val = v; assert( is_array(v)); }
    bool  set(unsigned i, int v); // to prevent upcasting from int to tiscript_value
    bool  push(int value);

  };

  // enumerator, allows to enumerate key/value pairs in object or elements of array.
  // Itended to be used as:
  // pinned collection = ...
  // enumerator each(pinned);
  // for(value key,val; each(key,val);) { ... }
  //
  class enumerator
  {
    value   col;//lection
    value   pos;
    HVM     vm;
  public:
    enumerator(pinned& collection):col(collection), vm(collection.get_vm()), pos(0) {}
    enumerator(HVM c, value collection):col(collection), vm(c), pos(0) {}
    bool operator()(value& key, value& val) { return ni()->get_next_key_value(vm,&col,&pos,&key,&val); }
    bool operator()(value& val)             { return ni()->get_next(vm,&col,&pos,&val); }
  };

  // arguments access inside native function imeplentations:
  class args
  {
  public:
    class error // argument fetching error
    {
      char buffer[512];
    public:
      error( int param_n, const char* expecting_type )
        {
          snprintf(buffer, sizeof(buffer), "parameter %d, expecting %s", param_n-2, expecting_type);
        }
      const char* msg() { return buffer; }
    };

    // Each function call has at least two parameters:
    //    arg[0] -> 'this' - object or namespace object for 'static' functions.
    //    arg[1] -> 'super' - usually you will just args::skip it.
    //    arg[2..argc] -> params defined in script

    args(HVM c):vm(c),n(0),opt(false) { argc = ni()->get_arg_count(vm); }

    int   length() const { return argc; }
    value get(int pn) const { return ni()->get_arg_n(vm,pn); }
    value operator[](int i) const { return get(i); }

    args& operator >> (bool& v)   { if( opt && (n >= argc) ) return *this;  if(!ni()->get_bool_value(get(n),&v)) throw error(n,"boolean"); n++;  return *this; }
    args& operator >> (int& v)    { if( opt && (n >= argc) ) return *this;  if(!ni()->get_int_value(get(n),&v)) throw error(n,"integer"); n++; return *this; }
    args& operator >> (double& v) { if( opt && (n >= argc) ) return *this;  if(!ni()->get_float_value(get(n),&v)) throw error(n,"float"); n++; return *this; }
    args& operator >> (std::ustring& v) { if( opt && (n >= argc) ) return *this;
                                    const WCHAR* p = 0; unsigned l = 0;
                                    if(!ni()->get_string_value(get(n),&p,&l)) throw error(n,"string");
                                    n++; v = std::ustring(p,l); return *this; }
    // use pinned values for movable things: object, array, string, etc.
    args& operator >> (pinned& v) { if( opt && (n >= argc) ) return *this;
                                    ni()->pin(vm,&v); v.val = get(n++); return *this; }
    args& operator >> (object_ref& v) { if( opt && (n >= argc) ) return *this;
                                        ni()->pin(vm,&v); v.val = get(n++);
                                        if( !is_object(v.val) && !is_native_object(v.val) ) throw error(n,"object");
                                        return *this; }
    args& operator >> (array_ref& v) { if( opt && (n >= argc) ) return *this;
                                        ni()->pin(vm,&v); v.val = get(n++);
                                        if( !is_array(v.val) ) throw error(n,"array");
                                        return *this; }
    // use non-pinned values only as a storage for non-movable things: symbol, int, float.
    args& operator >> (value& v)  { if( opt && (n >= argc) ) return *this;   v = get(n++); return *this; }

    enum optional_e { optional };
    enum skip_e { skip };

    // arg "stream" modifier, rest parameters after it are optional
    args& operator >> (optional_e m) { opt = true; return *this; }
    // arg "stream" modifier, skip the parameter.
    args& operator >> (skip_e m)     { ++n; return *this; }

  private:
    HVM  vm;
    int  n;
    int  argc;
    bool opt;
  };

  // native class definition ctl
  typedef tiscript_class_def  class_def;
  // native method implementation
  typedef tiscript_method     method_impl;
  // [] accessors implementation
  typedef tiscript_get_item   get_item_impl;
  typedef tiscript_set_item   set_item_impl;
  // getter/setter implementation
  typedef tiscript_get_prop   getter_impl;
  typedef tiscript_set_prop   setter_impl;
  // native object finalizer
  typedef tiscript_finalizer finalizer_impl;
  typedef tiscript_iterator  iterator_impl;

  struct method_def: public tiscript_method_def
  {
    method_def() { dispatch = 0; name = 0; handler = 0; tag = 0; payload = 0;}
    method_def(const char *n, method_impl* h) { dispatch = 0, name = n; handler = h; tag = 0; payload = 0;}
  };
  struct prop_def: public tiscript_prop_def
  {
    prop_def() { dispatch = 0; name = 0; getter = 0; setter = 0; tag = 0; }
    prop_def(const char *n, getter_impl* gh, setter_impl* sh) { dispatch = 0; name = n; getter = gh; setter = sh; tag = 0; }
  };
  struct const_def: public tiscript_const_def
  {
    const_def() { name = 0; val.i = 0; }
    const_def(const char *n, int v) { name = n; val.i = v; type = TISCRIPT_CONST_INT; }
    const_def(const char *n, double v) { name = n; val.f = v; type = TISCRIPT_CONST_FLOAT; }
    const_def(const char *n, const WCHAR* v) { name = n; val.str = v; type = TISCRIPT_CONST_STRING; }
  };

  // define native class
  inline value  define_class( HVM vm, class_def* cd, value zns = 0) // in this namespace object (or 0 if global)
  {
    return ni()->define_class(vm,cd,zns);
  }

  inline void  set_remote_std_streams(HVM vm, pinned& input, pinned& output, pinned& error) {  ni()->set_remote_std_streams(vm, &input, &output, &error); }

  // defines native function that can be accessed globally
  inline void define_global_function( HVM vm, method_def* md, value zns = 0) // in this namespace object (or 0 if global)
  {
    if( !zns ) zns = get_global_ns(vm);
    set_prop(vm,zns,md->name, ni()->native_function_value(vm,md));
  }

  // defines native controlled property (variable if you wish) that can be accessed globally
  inline void define_global_property( HVM vm, prop_def* pd, value zns = 0) // in this namespace object (or 0 if global)
  {
    if( !zns ) zns = get_global_ns(vm);
    set_prop(vm,zns,pd->name, ni()->native_property_value(vm,pd));
  }

/* OBSOLETE stuff: */

  // use enumerator instead!
  struct object_enum
  {
    VM *vm;
    virtual bool operator()(value key, value val) = 0; // true - continue enumeartion
    inline static bool TISAPI _enum(VM *vm, value key, value val, void* tag)
    {
      object_enum* oe = reinterpret_cast<object_enum*>(tag);
      oe->vm = vm; return oe->operator()(key,val);
    }
  };
  inline bool for_each_prop(HVM vm, value obj, object_enum& cb) { return ni()->for_each_prop(vm, obj, object_enum::_enum, &cb); }


}

// multi return macros. Used to return multiple values from native functions.

#define TISCRIPT_RETURN_2(c,rv1,rv2) \
  { tiscript::ni()->set_nth_retval(vm,1,(rv1)); \
    return (rv2); }

#define TISCRIPT_RETURN_3(c,rv1,rv2,rv3) \
  { tiscript::ni()->set_nth_retval(vm,1,(rv2)); \
    tiscript::ni()->set_nth_retval(vm,2,(rv1)); \
    return (rv3); }

#define TISCRIPT_RETURN_4(c,rv1,rv2,rv3,rv4) \
  { tiscript::ni()->set_nth_retval(vm,1,(rv3)); \
    tiscript::ni()->set_nth_retval(vm,2,(rv2)); \
    tiscript::ni()->set_nth_retval(vm,3,(rv1)); \
    return (rv4); }

#endif

#endif

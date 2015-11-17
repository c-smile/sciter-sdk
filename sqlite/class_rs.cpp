// Recordset class, TBD.

#include "tiscriptsqlite.h"
#include "aux-cvt.h"

using namespace tiscript;

const char *CLASSNAME = "Recordset";

// used by DB.exec() to create an RS object
value RS_create(VM* vm, sqlite3_stmt *pst)
{
  value cls = get_prop(vm,get_current_ns(vm),CLASSNAME);
  value rs_obj = create_object(vm,cls);
  set_native_data(rs_obj,pst);
  return rs_obj;
}

inline sqlite3_stmt * self(value obj)
{
  return (sqlite3_stmt*) get_native_data(obj);
}

static void finalize(VM *vm,value rs_obj)
{
  sqlite3_stmt *pst = self(rs_obj);
  if(pst)
  {
    sqlite3_finalize(pst);  
    set_native_data(rs_obj,0);
  }
}

// function Recordset.this() 
//     - ctor, non creatable here
static value RS_this_na(VM* vm)
{
  throw_error(vm,L"Recordset is creatable only by DB.exec()"); 
  return v_null();
}

// function rs.next() : true | false 
//     - advances recordset to the next record.
// returns 'true' if operation is successfull and so record buffer is valid.
// If end-of-set reached than RS is getting finalized (closed).

static value RS_next(VM* vm)
{
  pinned       rs_obj;
  args(vm) >>  rs_obj;     // 'this' is the RS here, instance method. 
  sqlite3_stmt *pst = self(rs_obj);
  if(!pst)
  {
    throw_error(vm,L"Recordset is already closed"); 
    return v_bool(false);
  }
  int r = sqlite3_step( pst );
  if( r == SQLITE_ROW )
    return v_bool(true);
  else
  {
    finalize(vm,rs_obj);
    return v_bool(false);
  }
}

// function rs.close() 
//     - finalizes this thing.
static value RS_close(VM* vm)
{
  pinned       rs_obj;
  args(vm) >>  rs_obj;     // 'this' is the RS here, instance method. 
  sqlite3_stmt *pst = self(rs_obj);
  if(!pst)
    return v_bool(false);
  finalize(vm,rs_obj);
  return v_bool(true);
}

// function rs.name( colno:int [,which: #logical | #field | #table | #database ] ):string 
//     - returns requested name of the field.
static value RS_name(VM* vm)
{
  pinned       rs_obj;
  int          colno = 0;
  value        which = 0;
  try
  {
    args(vm) >> rs_obj     // 'this' is the RS here, instance method. 
             >> args::skip // skip 'super'
             >> colno
             >> args::optional >> which;  
  } 
  catch (args::error &e) { throw_error(vm, aux::a2w(e.msg())); return v_undefined(); }

  sqlite3_stmt *pst = self(rs_obj);
  if(!pst)
  {
    throw_error(vm,L"Recordset is already closed"); 
    return v_bool(false);
  }
  const wchar_t* name = 0;
  if( which == 0 || which == v_symbol("logical") )
    name = (const wchar_t*) sqlite3_column_name16(pst, colno);
  else if( which == v_symbol("field") )
    name = (const wchar_t*) sqlite3_column_origin_name16(pst, colno);
  else if( which == v_symbol("table") )
    name = (const wchar_t*) sqlite3_column_table_name16(pst, colno);
  else if( which == v_symbol("database") )
    name = (const wchar_t*) sqlite3_column_database_name16(pst, colno);
  else
  {
    throw_error(vm,L"Unknown type of name"); 
    return v_bool(false);
  }  
  if( name )
    return v_string(vm,name);
  return v_undefined();
}


// props:

// property rs.length : int
//    - reports number of columns in the result set
static value RS_get_length(VM *vm, value rs_obj)
{
  sqlite3_stmt *pst = self(rs_obj);
  if(!pst)
    return v_undefined();
  return v_int( sqlite3_column_count(pst) );
}

// property rs.valid : true | false
//    - reports true if the buffer contains valid row.
static value RS_get_valid(VM *vm, value rs_obj)
{
  sqlite3_stmt *pst = self(rs_obj);
  if(!pst)
    return v_bool(false);
  return v_bool(true);
}


value field_to_value(VM* vm, sqlite3_stmt *pst, int n )
{
  int total = sqlite3_column_count(pst);
  if( n >= 0 && n < total )
    switch( sqlite3_column_type(pst, n))
    {
      case SQLITE_INTEGER: return v_int( sqlite3_column_int(pst, n) );
      case SQLITE_FLOAT:   return v_float( sqlite3_column_double(pst, n) );
      case SQLITE_TEXT:    return v_string( vm, (const wchar_t*)sqlite3_column_text16(pst, n), sqlite3_column_bytes16(pst,n) / sizeof(wchar_t) );
      case SQLITE_BLOB:    return v_bytes( vm, (const unsigned char*) sqlite3_column_blob(pst,n), sqlite3_column_bytes(pst,n));
      case SQLITE_NULL:    return v_null();
      default:
          throw_error(vm,L"Unknown type of field"); 
    }
  return v_nothing(); // exactly nothing
}

// [] accessor implementation
//     - supported cases:
//       var cv = rs["columnname"]; - by string
//       var cv = rs#columnname; - by symbol
//       var cv = rs[no]; - by ordinal number

static value get_item(VM *vm,value rs_obj,value key)
{
  sqlite3_stmt *pst = self(rs_obj);
  if(!pst)
  {
    throw_error(vm,L"Recordset is already closed"); 
    return v_undefined();
  }
  int n = 0;
  int total = sqlite3_column_count(pst);
  if(is_symbol(key)) // var cv = rs#columnname;
  {
    std::wstring name = c_symbol(key);
    for(n = 0;n < total; ++n)
    {
      const wchar_t* cn = (const wchar_t*)sqlite3_column_name16(pst, n);
      if(!cn) 
        break;
      if( name == cn )
        return field_to_value(vm,pst,n);
    }
  }
  else if(is_string(key)) // var cv = rs["columnname"];
  {
    std::wstring name = c_string(key);
    for(n = 0;n < total; ++n)
    {
      const wchar_t* cn = (const wchar_t*)sqlite3_column_name16(pst, n);
      if(!cn) 
        break;
      if( name == cn )
        return field_to_value(vm,pst,n);
    }
  }
  else if( is_int(key) ) // var cv = rs[0];
  {
    n = c_int(key);
    return field_to_value(vm,pst,n);
  }

  throw_error(vm,L"Field with the index is not found"); 
  return v_undefined();
}

// iterator, handler of for( var v in rs ) calls
static value iterator(VM *vm,value* index, value rs_obj)
{
  sqlite3_stmt *pst = self(rs_obj);
  if(!pst)
    return v_nothing();

  int n = 0;
  if( !is_int(*index) ) 
  { 
    // initial invocation of the iterator
    *index = v_int(n); 
  }
  else 
  {
    n = c_int(*index); ++n;
    *index = v_int(n);
  }
  return field_to_value(vm,pst,n);
}

void init_rs_class( VM *vm )
{
  static method_def methods[] = 
  {
    method_def("this",     RS_this_na),
    method_def("next",     RS_next),
    method_def("close",    RS_close),
    method_def("name",     RS_name),
    method_def()
  };
  
  static prop_def properties[] = 
  {
    prop_def("length", RS_get_length, 0),
    prop_def("valid",  RS_get_valid, 0),
    prop_def()
  };
  
  static const_def consts[] =
  {
    const_def()
  };

  static class_def rs_class = 
  {
    CLASSNAME,
    methods,
    properties,
    consts,
    get_item,
    0,
    finalize,
    iterator
  };
  tiscript::define_class(vm,&rs_class, get_current_ns(vm));
}
#include "tiscriptsqlite.h"

#include "aux-cvt.h"

using namespace tiscript;

// function DB.open(path:string): returns new DB object
value DB_open(VM* vm)
{
  std::wstring db_path;
  pinned       cls;
  try
  {
    args(vm) >> cls        // this is class method (a.k.a. static) so 'this' is the class here. 
             >> args::skip // skip 'super'
             >> db_path;
  } 
  catch (args::error &e) { throw_error(vm, aux::a2w(e.msg())); return v_undefined(); }

  sqlite3 *pDb = 0;
  if( SQLITE_OK != sqlite3_open16( db_path.c_str(), &pDb )) 
    return v_null();

  object_ref obj(vm);
  obj.create(cls);
  obj.data(pDb);
  return obj;
}

inline sqlite3 * self(value obj)
{
  return (sqlite3*) get_native_data(obj);
}

// function DB.close() - closes the DB
value DB_close(VM* vm)
{
  object_ref  obj;
  args(vm) >> obj;        // 'this' is the DB here, instance method. 

  sqlite3 *pdb = obj.data<sqlite3*>();
  int result = 0;
  if( pdb )
  {
    result = sqlite3_close( pdb );
    obj.data<sqlite3*>(0);
  }
  return v_int(result);
}

value DB_last(VM *vm)
{
  object_ref  obj;
  args(vm) >> obj;         // 'this' is the DB here, instance method. 

  sqlite3 *pdb = obj.data<sqlite3*>();
  int result = 0;
  if( pdb )
  {
    result = (int)sqlite3_last_insert_rowid( pdb );
    return v_int(result);
  } else
  {
    throw_error(vm, L"DB is already closed");
    return v_undefined();
  }
}

bool bind_value(VM* vm, sqlite3_stmt *pst, int &n, value arg)
{
    if( is_array(arg) )
      for(int i = 0; i < (int)get_length(vm, arg); ++i)
      {
        if( !bind_value(vm, pst, n, get_elem(vm, arg, i) ) )
          return false;
      }
    else
    if( is_int(arg) )
      sqlite3_bind_int(pst, n, c_int(arg));
    else if( is_float(arg) )
      sqlite3_bind_double(pst, n, c_float(arg));
    else if( is_string(arg) )
    {
      std::ustring s = c_string(arg);
      sqlite3_bind_text16(pst, n, s.c_str(), s.length() * sizeof(wchar_t), SQLITE_TRANSIENT);
    }
    else if( is_null(arg) )
      sqlite3_bind_null(pst, n);
    else if( is_bool(arg) )
      sqlite3_bind_int(pst, n, c_bool(arg));
    else if( is_bytes(arg) )
    {
      unsigned char* data = 0; unsigned len = 0;
      c_bytes(arg,data,len);
      sqlite3_bind_blob(pst, n, data, len, SQLITE_STATIC);
    }
    else return false;

    ++n;
    return true;
}

bool bind_params(VM* vm, sqlite3_stmt *pst, int argn = 3) // 0-this,1-super,2-sql,3-...
{
  args argv(vm);
  int n = 1;
  for(; argn < argv.length(); ++argn)
  {
    value arg = argv[argn];
    if( !bind_value(vm, pst, n, arg) )
    {
      throw_error(vm,L"wrong type of SQL parameter value");
      return false;
    }
  }
  return true;
}

extern value RS_create(VM* vm, sqlite3_stmt *pst);


// function DB.exec(sql [,param1,param2,...]): int | recordset - executes SQL statement on the DB. 
// supports multiple sql statements!
value DB_exec(VM* vm)
{
  std::wstring src; //statement, string with optional '?'s 
  pinned       db_obj;
  try
  {
    args(vm) >>  db_obj     // 'this' is the DB here, instance method. 
             >>  args::skip
             >>  src;
  } 
  catch (args::error &e) { throw_error(vm, aux::a2w(e.msg())); return v_undefined(); }

  sqlite3 *pdb = self(db_obj);
  if( !pdb )
  {
    throw_error(vm, L"DB is already closed");
    return v_undefined();
  }

  sqlite3_stmt *pst = 0;
  const wchar_t* tail = src.c_str();
  int length = sizeof(wchar_t)*src.length();

  int r;
  bool first_statement = true;
  do {
    // skip delimiters, it can cause SQLITE_MISUSE
    while(*tail && *tail <= ' ')
      ++tail;

    if( !(*tail) )
      break;

    // clear last request
    if( pst )
      sqlite3_finalize(pst);

    /* according to sqlite documentation if length includes zero terminator then it operates a bit faster */
    r = sqlite3_prepare16( pdb, tail, length + 2,&pst, (const void**)&tail);
    if( r != SQLITE_OK )
    {
      throw_error(vm, (const wchar_t*)sqlite3_errmsg16(pdb));
      return v_null();
    }

#if defined(DEBUG) || defined(_DEBUG)
    std::wstring out = L"'";
    out += (LPCWSTR)tail;
    out += L"'\n";

    OutputDebugStringW(out.c_str());
#endif

    // will bind only the first statement, so batch_exec could be similar to exec
    if(first_statement && !bind_params(vm,pst))
    {
      sqlite3_finalize(pst);
      return v_int(SQLITE_ERROR);
    }
    first_statement = false;
    
    
    r = sqlite3_step( pst );
    if( r && r != SQLITE_ROW && r != SQLITE_DONE && r != SQLITE_MISUSE )
    {
      // SQLITE_MISUSE may mean that something is wrong with the sql, but we should continue anyway
      throw_error(vm, (const wchar_t*)sqlite3_errmsg16(pdb));
      sqlite3_finalize(pst);
      return v_int(r);
    }

    if( tail )
      length = wcslen((wchar_t*) tail) * sizeof(wchar_t);
    else length = 0;

  } while(tail && length > 1);
  
  // last statement could be select
  if( r == SQLITE_ROW )
    return RS_create(vm, pst);

  sqlite3_finalize(pst);
 
  return v_int(r);
}

// GC detected that the db object is not used by anyone so ... 
static void finalize(VM *vm,value db_obj)
{
  sqlite3 *pdb = self(db_obj);
  if(pdb)
  {
    sqlite3_close(pdb);  
    set_native_data(db_obj,0);
  }
}


void init_db_class( VM *vm )
{
  static method_def methods[] = 
  {
    method_def("open",  DB_open),
    method_def("close", DB_close),
    method_def("exec",  DB_exec),
    method_def("lastInsertedID",  DB_last),
    method_def()
  };

  static const_def consts[] =
  {
    const_def("BUSY",   SQLITE_BUSY),
    const_def("DONE",   SQLITE_DONE),
    const_def("ROW",    SQLITE_ROW),
    const_def("SCHEMA", SQLITE_SCHEMA),
    const_def("ERROR",  SQLITE_ERROR),
    const_def()
  };

  static class_def db_class = 
  {
    "DB",
    methods,
    0,
    consts,
    0, // get []
    0, // set []
    finalize
  };
  tiscript::define_class(vm,&db_class, get_current_ns(vm));
}

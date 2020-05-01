#include "sciter-sqlite.h"
#include "aux-cvt.h"

namespace sqlite {

  DB* DB::open(sciter::string path) {
    sqlite3 *pDb;
    if (SQLITE_OK != sqlite3_open16(path.c_str(), &pDb))
      return nullptr; // undefined
    return new DB(pDb);
  }

  // function DB.close() - closes the DB
  int DB::close()
  {
    if (pDb)
    {
      int result = sqlite3_close(pDb);
      pDb = nullptr;
    }
    return 0;
  }

  int DB::lastRowId()
  {
    if (pDb)
      return (int)sqlite3_last_insert_rowid(pDb);
    else
    {
      throw sciter::om::exception("DB is already closed");
    }
  }

  bool bind_value(sqlite3_stmt *pst, int n, sciter::value arg)
  {
    if (arg.is_array())
      for (int i = 0; i < arg.length(); ++i)
      {
        if (!bind_value(pst, n, arg.get_item(i)))
          return false;
      }
    else if (arg.is_int())
      sqlite3_bind_int(pst, n, arg.get<int>());
    else if (arg.is_float())
      sqlite3_bind_double(pst, n, arg.get<double>());
    else if (arg.is_string())
    {
      sciter::string s = arg.get<sciter::string>();
      sqlite3_bind_text16(pst, n, s.c_str(), s.length() * sizeof(sciter::string::value_type), SQLITE_TRANSIENT);
    }
    else if (arg.is_null())
      sqlite3_bind_null(pst, n);
    else if (arg.is_bool())
      sqlite3_bind_int(pst, n, arg.get<bool>());
    else if (arg.is_bytes())
    {
      auto bytes = arg.get_bytes();
      sqlite3_bind_blob(pst, n, bytes.start, bytes.length, SQLITE_STATIC);
    }
    else 
      return false;

    return true;
  }

  bool bind_params(sqlite3_stmt *pst, aux::slice<sciter::value> params)
  {
    for (int n = 1; n <= int(params.length); ++n)
    {
      if (!bind_value(pst, n, params[n - 1]))
        return false;
    }
    return true;
  }


  // function DB.exec(sql [,param1,param2,...]): [int,int] | recordset - executes SQL statement on the DB. 

  sciter::value DB::exec(sciter::string sql, std::vector<sciter::value> params)
  {
    if (!pDb)
    {
      throw sciter::om::exception("DB is already closed");
      return sciter::value();
    }

    sqlite3_stmt *pst = 0;

    auto statement = aux::chars_of(sql);
    WCHAR *end = 0;

    int r = sqlite3_prepare16(pDb, statement.start, statement.length * sizeof(statement[0]), &pst, (const void**)&end);
    if (r != SQLITE_OK)
      throw sciter::om::exception(sqlite3_errmsg(pDb));
   
    if (!bind_params(pst, aux::elements_of(params)))
    {
      sqlite3_finalize(pst);
      throw sciter::om::exception("wrong type of SQL parameter value");
    }

    r = sqlite3_step(pst);
    if (r && r != SQLITE_ROW && r != SQLITE_DONE && r != SQLITE_MISUSE)
    {
      // SQLITE_MISUSE may mean that something is wrong with the sql, but we should continue anyway
      sqlite3_finalize(pst);
      throw sciter::om::exception(sqlite3_errmsg(pDb));
    }

    // last statement could be select
    if (r == SQLITE_ROW)
      return sciter::value::wrap_asset(new Recordset(pst));

    sqlite3_finalize(pst);

    int numrows_affected = sqlite3_changes(pDb);
    return sciter::value::make_array({ r,numrows_affected });
  }

  DB::~DB() {
    if (pDb)
      sqlite3_close(pDb);
  }

}
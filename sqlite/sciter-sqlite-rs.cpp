// Recordset class, TBD.

#include "sciter-sqlite.h"
#include "aux-cvt.h"

namespace sqlite {

  // used by DB.exec() to create an RS object

  Recordset::Recordset(sqlite3_stmt *pstatement) : pst(pstatement) {
  }

  Recordset::~Recordset() {
    if (pst)
      sqlite3_finalize(pst);
  }

  int Recordset::get_length() const {
    if (!pst)
      throw sciter::om::exception("Recordset is already closed");
    return sqlite3_column_count(pst);
  }

  // function rs.next() : true | false 
  //     - advances recordset to the next record.
  // returns 'true' if operation is successfull and so record buffer is valid.
  // If end-of-set reached than RS is getting finalized (closed).

  bool Recordset::next()
  {
    if (!pst)
      throw sciter::om::exception("Recordset is already closed");
    int r = sqlite3_step(pst);
    
    if (r == SQLITE_ROW)
      return true;
    else
    {
      close();
      return false;
    }
  }

  // function rs.close() 
  //     - finalizes this thing.
  bool Recordset::close()
  {
    if (!pst)
      return false;
    
    sqlite3_finalize(pst);
    pst = nullptr;
    return true;
  }

  // function rs.name( colno:int [,which: #logical | #field | #table | #database ] ):string 
  //     - returns requested name of the field.
  sciter::string Recordset::name(int colno, sciter::string which)
  {
    if (!pst)
      throw sciter::om::exception("Recordset is already closed");

    auto sym = aux::chars_of(which);

    if (sym == const_wchars("logical") || sym.length == 0)
      return (const WCHAR*)sqlite3_column_name16(pst, colno);
    else if (sym == const_wchars("field"))
      return (const WCHAR*)sqlite3_column_origin_name16(pst, colno);
    else if (sym == const_wchars("table"))
      return (const WCHAR*)sqlite3_column_table_name16(pst, colno);
    else if (sym == const_wchars("database"))
      return (const WCHAR*)sqlite3_column_database_name16(pst, colno);
    else
      throw sciter::om::exception("Unknown type of name");
    return sciter::string();
  }


  //    - reports true if the buffer contains valid row.
  bool Recordset::isValid() const
  {
    return pst != nullptr;
  }

  sciter::value Recordset::field_to_value(int n)
  {
    int total = sqlite3_column_count(pst);
    if (n >= 0 && n < total)
      switch (sqlite3_column_type(pst, n))
      {
        case SQLITE_INTEGER: return sciter::value(sqlite3_column_int(pst, n));
        case SQLITE_FLOAT:   return sciter::value(sqlite3_column_double(pst, n));
        case SQLITE_TEXT:    return sciter::value::make_string((const WCHAR*)sqlite3_column_text16(pst, n), sqlite3_column_bytes16(pst, n) / sizeof(WCHAR));
        case SQLITE_BLOB:    return sciter::value::make_bytes((const unsigned char*)sqlite3_column_blob(pst, n), sqlite3_column_bytes(pst, n));
        case SQLITE_NULL:    return sciter::value::null();
        default:
          throw sciter::om::exception("Unknown type of field");
      }
    return sciter::value::nothing(); // exactly nothing
  }

  // [] accessor implementation
  //     - supported cases:
  //       var cv = rs["columnname"]; - by string
  //       var cv = rs#columnname; - by symbol
  //       var cv = rs[no]; - by ordinal number

  bool Recordset::get_item(sciter::value key_or_index, sciter::value& val )
  {
    int n = 0;
    int total = sqlite3_column_count(pst);
    
    if (key_or_index.is_string()) // var cv = rs#columnname;
    {
      sciter::string name = key_or_index.get<sciter::string>();
      for (n = 0; n < total; ++n)
      {
        aux::wchars cn = aux::chars_of((const WCHAR*)sqlite3_column_name16(pst, n));
        if (aux::chars_of(name) == cn) {
          val = field_to_value(n);
          return true;
        }
      }
    }
    else if (key_or_index.is_int()) // var cv = rs[0];
    {
      n = key_or_index.get<int>();
      val = field_to_value(n);
      return true;
    }
    return false;
  }

  // iterator, handler of for( var v in rs ) calls
  bool Recordset::get_next(sciter::value& index, sciter::value& val)
  {
    int n = 0;
    if (!index.is_int())
    {
      // initial invocation of the iterator
      index = n;
    }
    else {
      n = index.get<int>() + 1;
      index = n;
    }
     
    if (n < get_length()) {
      index = n;
      val = field_to_value(n);
      return true;
    } else 
      return false;
  }

  bool Recordset::get_prop(const std::string& field_name, sciter::value& val) {
    int n = 0;
    int total = sqlite3_column_count(pst);

    for (n = 0; n < total; ++n)
    {
      aux::chars cn = aux::chars_of((const CHAR*)sqlite3_column_name(pst, n));
      if (aux::chars_of(field_name) == cn) {
        val = field_to_value(n);
        return true;
      }
    }
    return false;
  }

}
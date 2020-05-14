#ifndef __sciter_sqlite_h__

#include "sqlite-wrap.h"

#include "../include/sciter-x.h"

extern const char sqlite3_version[];

namespace sqlite {
      
  class DB : public sciter::om::asset<DB>
  {
    sqlite3 *pDb = nullptr;

  public:

    DB(sqlite3 *p) : pDb(p) {}
    ~DB();

    static DB* open(sciter::string path);

    int close();
    int lastRowId();
    
    sciter::value exec(sciter::string sql, std::vector<sciter::value> params);

    SOM_PASSPORT_BEGIN(DB)
      SOM_FUNCS(
        SOM_FUNC(exec),
        SOM_FUNC(close), 
        SOM_FUNC(lastRowId) )
    SOM_PASSPORT_END

  };


  // namespace object: 

  class SQLite : public sciter::om::asset<SQLite> {
  public:

    sciter::string version = WSTR(SQLITE_VERSION);

    SQLite() {}

    sciter::value open(sciter::string path) {
      sqlite::DB* pdb = DB::open(path);
      if (pdb)
        return sciter::value::wrap_asset(pdb);
      else
        return sciter::value();
    }
    SOM_PASSPORT_BEGIN(SQLite)
      SOM_PASSPORT_FLAGS(SOM_EXTENDABLE_OBJECT) // allow SQLite to be extended as a namespace
      SOM_FUNCS(SOM_FUNC(open))
      SOM_PROPS(SOM_RO_PROP(version))
    SOM_PASSPORT_END

  };

  class Recordset : public sciter::om::asset<Recordset> {

    sqlite3_stmt *pst;

  public:

    Recordset(sqlite3_stmt *pstatement);

    ~Recordset();
    
    // function rs.next() : true | false 
    //     - advances recordset to the next record.
    //     - returns 'true' if operation is successfull and so record buffer is valid.
    //               If end-of-set reached than RS will be auto-finalized (closed).
    bool next();

    // function rs.close() 
    //     - finalizes this thing.
    bool close();

    // function rs.name( colno:int [,which: #logical | #field | #table | #database ] ):string 
    //     - returns requested name of the field.
    sciter::string name(int column_no, sciter::string which);

    // property
    // rs.length : int
    //    - reports number of columns in the result set
    int get_length() const;

    // rs.valid() : true | false
    //    - reports true if the buffer contains valid row.
    bool isValid() const;

    // [] accessor implementation
    //     - supported cases:
    //       var cv = rs["columnname"]; - by string
    //       var cv = rs[no]; - by ordinal number
    bool get_item(sciter::value key_or_index, sciter::value& val);

    // .propName accessor implementation
    //     - supported cases:
    //       var cv = rs.columnname;
    bool get_prop(const std::string& field_name, sciter::value& val);


    // iterator, handler of for( var v in rs ) calls
    bool get_next(sciter::value& index, sciter::value& val);

    SOM_PASSPORT_BEGIN(Recordset)
      SOM_FUNCS(
        SOM_FUNC(next),
        SOM_FUNC(name),
        SOM_FUNC(isValid),
        SOM_FUNC(close)
      )
      SOM_PROPS( 
        SOM_RO_VIRTUAL_PROP(length,get_length) 
      )
      SOM_ITEM_GET(get_item)
      //SOM_PROP_GET(get_prop) -- see https://sciter.com/forums/topic/recordset-props-fails-if-data-has-column-name/
      SOM_ITEM_NEXT(get_next)
    SOM_PASSPORT_END

  private:
    sciter::value field_to_value(int n);
  };

}


#endif
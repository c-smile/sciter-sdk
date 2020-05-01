
SQLite3 engine wrapped as Sciter extension library.

sqlite3.c.inc is an original sqlite3.c, just renamed

These files are supposed to be linked statically with the application - just add all *.c and *.cpp to your project and define something like this function in your window class

  sciter::value openDb(sciter::string path) {
    sqlite::DB* pdb = sqlite::DB::open(path);
    if (pdb)
      return sciter::value::wrap_asset(pdb);
    else
      return sciter::value();
  }

+ add it to the passport:

  SOM_PASSPORT_BEGIN(frame)
    SOM_FUNCS( SOM_FUNC(openDb) )
  SOM_PASSPORT_END

See: /demos/usciter project that does the integration
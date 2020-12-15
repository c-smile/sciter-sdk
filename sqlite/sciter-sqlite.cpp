#include "sciter-sqlite.h"

extern "C" {

#ifndef WINDOWS
__attribute__((visibility("default")))
#endif
SBOOL SCAPI SciterLibraryInit(ISciterAPI* psapi, SCITER_VALUE* plibobject)
{
  _SAPI(psapi); // set reference to Sciter API provided by host application including scapp(quark)
  static sciter::om::hasset<sqlite::SQLite> sqlite_root = new sqlite::SQLite(); // invloked once (C++ static convention)
  *plibobject = sciter::value::wrap_asset(sqlite_root);
  return TRUE;
}

}

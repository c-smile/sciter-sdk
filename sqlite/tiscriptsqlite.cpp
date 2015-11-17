// tiscriptsqlite.cpp : Defines the entry points for the DLL and TIScript extension library.
//

#include "tiscriptsqlite.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

void init_db_class( tiscript::VM *vm );
void init_rs_class( tiscript::VM *vm );

void EXTAPI  TIScriptLibraryInit(tiscript_VM* vm, tiscript_native_interface* piface )
{
  tiscript::ni(piface);

  init_db_class( vm );
  init_rs_class( vm );
}


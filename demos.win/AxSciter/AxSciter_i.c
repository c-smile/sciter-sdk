

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sun Jun 11 16:03:05 2017
 */
/* Compiler settings for AxSciter.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_ISciter,0xCE8DB2CD,0xC37D,0x40D2,0x9E,0x2A,0x91,0x1E,0xD7,0xA3,0x12,0xCD);


MIDL_DEFINE_GUID(IID, LIBID_AxSciterLib,0xB39EBA5E,0x3275,0x4E64,0xB8,0x8A,0x43,0x5C,0xD3,0xFF,0x17,0x9A);


MIDL_DEFINE_GUID(IID, DIID__ISciterEvents,0x09C2F7D8,0xE1BD,0x4568,0x99,0xA0,0x63,0x1D,0x3A,0x3A,0x09,0xA9);


MIDL_DEFINE_GUID(CLSID, CLSID_Sciter,0x7832863F,0xC30A,0x49DF,0xB6,0x5E,0x3B,0x1C,0xE4,0x6C,0x82,0x9E);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif




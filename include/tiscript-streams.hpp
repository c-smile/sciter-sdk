#ifndef __tiscript_streams_hpp__
#define __tiscript_streams_hpp__

#if defined(__cplusplus)

#include "tiscript.h"
#include "aux-cvt.h"
#include <string>

namespace tiscript 
{

  class stream: public tiscript_stream 
  {
    public:
      stream() 
      { 
        static tiscript_stream_vtbl methods = { _stream_input, _stream_output, _stream_name, _stream_close };
        _vtbl = &methods;
      }
      virtual ~stream()       { close(); } 

      // these need to be overrided 
      virtual const WCHAR*    stream_name() { static WCHAR z = 0; return &z; }
      virtual int  get()      { return -1; }
      virtual bool put(int)   { return false; }
      virtual void close()    {} 
      
    protected:
      inline static bool TISAPI _stream_input(tiscript_stream* tag, int* pv)
        { *pv = static_cast<stream*>(tag)->get(); return *pv >= 0; }
      inline static bool TISAPI _stream_output(tiscript_stream* tag, int v)
        { return static_cast<stream*>(tag)->put(v); }
      inline static const WCHAR* TISAPI _stream_name(tiscript_stream* tag)
        { return static_cast<stream*>(tag)->stream_name(); }
      inline static void TISAPI _stream_close(tiscript_stream* tag)
        { static_cast<stream*>(tag)->close(); }
  };    
  
  // various stream implementations
  class string_istream: public stream
  {
    const WCHAR* _pos;  const WCHAR* _end;
  public:
    string_istream(const WCHAR* str, unsigned length = 0): _pos(str),_end(str) { if(length == 0) length = (unsigned int)aux::wcslen(str); _end = _pos + length; }
    virtual int get() { return (*_pos && _pos < _end)? *_pos++ : -1; }
  };
  
  class string_ostream: public stream
  {
    WCHAR *_str, *_p, *_end;
  public:
    string_ostream() { _p = _str = (WCHAR*)malloc( 128 * sizeof(WCHAR) ); _end = _str + 128; }
    virtual bool put(int v) 
    {
      if( _p >= _end )
      {
        size_t sz = _end - _str; size_t nsz = (sz * 2) / 3; if( nsz < 128 ) nsz = 128;
        WCHAR *nstr = (WCHAR*)realloc(_str, nsz * sizeof(WCHAR));
        if(!nstr) return false;
        _str = nstr; _p = _str + sz; _end = _str + nsz;
      }
      *_p++ = WCHAR(v);
      return true; 
    }
    const WCHAR *c_str() { put(0); --_p; return _str; }    
    virtual void close() { if(_str) { free(_str); _str = _p = _end = 0; } }     
  };  
  
  // simple file input stream. 
  class file_istream: public stream
  {
    FILE*        _file;  
    std::basic_string<WCHAR> _name;
  public:
    file_istream(const WCHAR* filename) {
#ifdef WINDOWS
        _wfopen_s(&_file,filename,L"rb");
#else
        _file = fopen(aux::w2a(filename),"rb");
#endif
        _name = filename;
    }
    virtual void close() { if(_file) {fclose(_file);_file = 0;} }

    virtual const WCHAR* stream_name() { return _name.c_str(); }

    virtual int get() 
    { 
      if(!_file || feof(_file)) return -1;
      return fgetc(_file);
    }
    bool is_valid() const { return _file != 0; }
  };

  inline WCHAR oem2wchar(char c)
  {
    WCHAR wc = '?';
#ifdef WINDOWS
    MultiByteToWideChar(CP_OEMCP,0,&c,1,&wc,1);
#else
    wc = c;
#endif
    return wc;
  }
  inline char wchar2oem(WCHAR wc)
  {
    char c = '?';
#ifdef WINDOWS
    WideCharToMultiByte(CP_OEMCP,0,&wc,1,&c,1,0,0);
#else
    c = char(wc);
#endif
    return c;
  }
  class console: public stream 
  {
  public:
    virtual int  get() { int c = getchar(); return c != EOF? oem2wchar(char(c)) : -1; }      
    virtual bool put(int v) 
    { 
      return putchar( wchar2oem(WCHAR(v)) ) != EOF; 
    }
  };
  
  
}

#endif

#endif

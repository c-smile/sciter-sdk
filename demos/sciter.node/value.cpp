#include "sciter-x.h"
#include "node_api.h"
#include "napi-common.h"

napi_value cvt(napi_env env, sciter::value val) {
  val.isolate(); // force it to be converted to primitive close-to-JSON constructs
  napi_value result = nullptr;
  switch (val.t) {
    case T_UNDEFINED:   napi_get_undefined(env, &result); return result;
    case T_NULL:        napi_get_null(env, &result); return result;
    case T_BOOL:        napi_get_boolean(env,val.get<bool>(), &result); return result;
    case T_COLOR:
    case T_INT:         napi_create_int32(env, val.get<int>(), &result); return result;
    case T_DURATION:
    case T_ANGLE:
    case T_FLOAT:       napi_create_double(env, val.get<double>(), &result); return result;
    case T_STRING: {
      sciter::string s = val.to_string();
      napi_create_string_utf16(env, (const char16_t*)s.c_str(), s.size(), &result);
      return result;
    }
    case  T_ARRAY: {
      size_t n = val.length();
      napi_create_array_with_length(env,n,&result);
      for (uint32_t i = 0; i < n; ++i)
        napi_set_element(env, result, i, cvt(env, val.get_item(int(i))));
      return result;
    }
    case  T_MAP: {
      napi_create_object(env, &result);
      val.each_key_value([&](const sciter::value& key, const sciter::value& val) -> bool {
        napi_set_property(env, result, cvt(env, key), cvt(env, val));
        return true;
      });
      return result;
    }
/*
      T_DATE = 6,     // INT64 - contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC), a.k.a. FILETIME on Windows
      T_CURRENCY = 7, // INT64 - 14.4 fixed number. E.g. dollars = int64 / 10000; 
      T_LENGTH = 8,   // length units, value is int or float, units are VALUE_UNIT_TYPE
      T_MAP = 10,
      T_FUNCTION = 11,   // named tuple , like array but with name tag
      T_BYTES = 12,      // sequence of bytes - e.g. image data
      T_OBJECT = 13,     // scripting object proxy (TISCRIPT/SCITER)
      T_DOM_OBJECT = 14,  // DOM object, use get_object_data to get HELEMENT 
                          //T_RESOURCE = 15,  // 15 - other thing derived from tool::resource
                          //T_RANGE = 16,     // 16 - N..M, integer range.
*/

  }
  napi_get_undefined(env, &result);
  return result;

}

sciter::value cvt(napi_env env, napi_value val) {
  napi_valuetype vt = napi_undefined;
  napi_typeof(env,val,&vt);
  switch (vt) 
  {
    case napi_undefined: return sciter::value();
    case napi_null: return sciter::value::null();
    case napi_boolean: { bool r; napi_get_value_bool(env, val, &r); return sciter::value(r); }
    case napi_number: { 
      double r,ri; 
      napi_get_value_double(env, val, &r); 
      if (modf(r, &ri) == 0 && r > -MAXINT32 && r < MAXINT32)
        return sciter::value(int(r));
      return sciter::value(r); 
    }

    case napi_string: {
      size_t length;
      napi_get_value_string_utf16(env, val, nullptr,0,&length);
      std::vector<char16_t> buf;
      buf.resize(length + 1);
      napi_get_value_string_utf16(env, val, buf.data(), buf.size(), &length);
      return sciter::value((const WCHAR*)buf.data(),UINT(length));
    }

      //napi_symbol,
    case napi_object:
    {
      bool is_array = false;
      napi_is_array(env,val,&is_array);
      if (is_array) {
        uint32_t length;
        napi_get_array_length(env,val,&length);
        sciter::value arr = sciter::value::make_array(length);
        for (uint32_t i = 0; i < length; ++i) {
          napi_value item;
          napi_get_element(env,val,i,&item);
          arr.set_item(int(i), cvt(env, item));
        }
        return arr;
      }
      else {
        sciter::value map = sciter::value::make_map();
        napi_value names;
        napi_get_property_names(env,val,&names);
        napi_is_array(env, names, &is_array); assert(is_array);
        uint32_t length;
        napi_get_array_length(env, names, &length);
        for (uint32_t i = 0; i < length; ++i) {
          napi_value key;
          napi_get_element(env, val, i, &key);
          bool has_own_property = false;
          napi_has_own_property(env, val, key, &has_own_property);
          if (!has_own_property) continue;
          napi_value item;
          napi_get_property(env, val, key, &item);
          map.set_item(cvt(env, key), cvt(env, item));
        }
        return map;
      }
    }
    //napi_function,
    //napi_external,
    //napi_bigint,
  }
  return sciter::value();
}

std::vector<sciter::value> cvt(napi_env env, aux::slice<napi_value> args)
{
  std::vector<sciter::value> rv(args.length);
  for (unsigned i = 0; i < args.length; ++ i)
    rv[i] = cvt(env, args[i]);
  return rv;
}

std::vector<napi_value>    cvt(napi_env env, aux::slice<sciter::value> args)
{
  std::vector<napi_value> rv(args.length);
  for (unsigned i = 0; i < args.length; ++i)
    rv[i] = cvt(env, args[i]);
  return rv;
}

sciter::astring cvt_string8(napi_env env, napi_value val) {
  napi_valuetype vt;
  napi_typeof(env, val, &vt);
  NAPI_ASSERT(env, vt == napi_string);
  size_t length = 0;
  napi_get_value_string_utf8(env, val, NULL, 0, &length);
  std::vector<char> r;
  r.resize(length + 1);
  napi_get_value_string_utf8(env, val, r.data(), r.size(), &length);
  return sciter::astring(r.data(), length);
}
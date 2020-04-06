#pragma once

#include <cstring>

namespace sciter {

  namespace om {
    // reference countable, COM::IUnknown alike, thing:
    struct entity 
    {
      // mandatory:
      virtual long  add_ref() = 0;
      virtual long  release() = 0;
      // can be used to as to get interface as to check if the thing supports the feature
      virtual bool  get_interface(const char* name, entity** out = nullptr) {
        if( 0 != strcmp(name,"entity.sciter.com")) return false;
        if(out) { this->add_ref(); *out = this; }
        return true;
      } 
    };
  }

}

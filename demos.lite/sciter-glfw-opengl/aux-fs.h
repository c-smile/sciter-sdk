#ifndef __AUX_FS_H__
#define __AUX_FS_H__

#include "sciter-x.h"

#ifdef _WIN32
#include <windows.h>    //GetModuleFileNameW
#include <shlwapi.h>    //PathRemoveFileSpecA 
#else
#include <limits.h>
#include <unistd.h>     //readlink
#endif

#include <string>

namespace aux {

  inline std::string getexepath()
  {
#ifdef WINDOWS
	char result[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, result, MAX_PATH);
	PathRemoveFileSpecA(result);
#else
    char result[PATH_MAX] = {0};
    readlink("/proc/self/exe", result, PATH_MAX);
	dirname(result);
#endif
    return std::string(result);
  }

  // get exe relative path
  inline std::string getpath(const std::string& relpath)
  {
    std::string ep = getexepath();
    ep += "/";
	ep += relpath;
    return ep;
  }
}

#endif
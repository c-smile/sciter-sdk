
newoption {
   trigger     = "windowsxp",
   description = "Will add Windows XP support"
}


if( _TARGET_OS ~= "macosx") then  -- we are not auto generating XCode solutions for a while
                                  -- structure of typical XCode is not trivial - requires manual inputs.
function osabbr() 
  if( _TARGET_OS == "macosx") then return "osx" 
  elseif( _TARGET_OS == "windows") then return "win"
  elseif( _TARGET_OS == "linux") then return "lnx"
  else return "unk"
  end
end

-- function that setups target dir according to configuration
function settargetdir() 
  basedir = basedir or ""
  targetdir ("bin." .. osabbr() .."/%{cfg.platform}")
  filter "configurations:*Skia" 
    targetdir ("bin." .. osabbr() .."/%{cfg.platform}skia")
  filter {}
end

if _OPTIONS["windowsxp"] then 

  filter {"system:windows", "action:vs2015", "platforms:not arm64" }
    toolset "v140_xp"
  filter {"system:windows", "action:vs2017", "platforms:not arm64" }
    toolset "v141_xp"
  filter {}
end    


workspace "sciter.sdk"
  configurations { "Debug", "Release" }
  platforms { "x32", "x64", "arm32", "arm64" } 

  cppdialect "C++14" 

  staticruntime "On"
  
  -- -- location "build"
  filter "system:windows"
    configurations { "DebugSkia", "ReleaseSkia" }
    location "build.windows"
    links { "shell32", "advapi32", "ole32", "oleaut32", "comdlg32" }
    systemversion "latest"
  filter "system:macosx"
    location "build.macosx"
    filter "system:macosx"
    links { "CoreFoundation.framework", "Cocoa.framework" }
    buildoptions { "-fobjc-arc" }
  filter "system:linux"
    location "build.linux"
    defines { "_GNU_SOURCE" }
    buildoptions {
     "`pkg-config gtk+-3.0 --cflags`",      
     "`pkg-config fontconfig --cflags`",
     "-fPIC",
     "-Wno-unknown-pragmas",
     "-Wno-write-strings",
     "-ldl",
    }
    linkoptions { 
      "-fPIC",
      "-pthread",
    }

  filter {}

  includedirs { "include" }  

  flags { "MultiProcessorCompile" }

  filter "platforms:x32"
    architecture "x86"
  filter "platforms:x64"
    architecture "x86_64"  
  filter "platforms:arm64"
    architecture "ARM64"  
  filter "platforms:arm32"
    architecture "ARM"  


  filter {"platforms:x32", "system:windows"}
    defines { "WIN32" }
  filter {"platforms:x64", "system:windows"}
    defines { "WIN32","WIN64" }      
  filter {"platforms:arm64", "system:windows"}
    defines { "WIN32","WIN64", "ARM64" }      

  filter "configurations:Debug*"
    defines { "DEBUG", "_DEBUG" }
    symbols "On"

  filter "configurations:Release*"
    defines { "NDEBUG"}  
    optimize "Size"
    symbols "Off"
    flags { "LinkTimeOptimization" }

  filter {"system:windows"}
    defines { "_CRT_SECURE_NO_WARNINGS" } 
 
  filter {}

project "usciter"
  kind "WindowedApp"
  language "C++"

  dpiawareness "HighPerMonitor"

  files { "demos/usciter/usciter.cpp",
          "sqlite/*.h",
          "sqlite/*.cpp",
          "sqlite/sqlite-wrap.c" }

  settargetdir()

  filter "system:windows"
    removeplatforms { "x64" }
    removeconfigurations { "*skia" }
    files {"include/sciter-*.h",
           "include/sciter-*.hpp",
           "include/aux-*.*",
           "include/sciter-win-main.cpp",
           "demos/usciter/win-res/usciter.rc",
           "demos/usciter/win-res/dpi-aware.manifest" }
    prebuildcommands { 
      "%{prj.location}..\\bin.win\\packfolder.exe %{prj.location}..\\demos\\usciter\\res %{prj.location}..\\demos\\usciter\\resources.cpp -v \"resources\""
    }

  filter "system:macosx"
    files {"include/sciter-osx-main.mm"}
  filter "system:linux"
    files {"include/sciter-gtk-main.cpp"}
    buildoptions {
       "`pkg-config gtk+-3.0 --cflags`"
    }
    linkoptions { 
       "`pkg-config gtk+-3.0 --libs`",
       "`pkg-config fontconfig --libs`",
       "-fPIC",
       "-pthread",
       "-Wl,--no-undefined",
       "-ldl",
    }

  filter {}

project "uminimal"
  kind "WindowedApp"
  language "C++"

  dpiawareness "HighPerMonitor"

  files { "demos/uminimal/uminimal.cpp" }  

  settargetdir()


  filter "system:windows"
    removeplatforms { "x64","arm64" }
    removeconfigurations { "*skia" }
    files {"include/sciter-win-main.cpp"}
  filter "system:macosx"
    files {"include/sciter-osx-main.mm"}
  filter "system:linux"
    files {"include/sciter-gtk-main.cpp"}
    linkoptions { 
       "`pkg-config gtk+-3.0 --libs`",
       "`pkg-config fontconfig --libs`",
       "-fPIC",
       "-pthread",
       "-Wl,--no-undefined",
       "-ldl",
    }
  filter {}

project "ulayered"
  kind "WindowedApp"
  language "C++"

  dpiawareness "HighPerMonitor"

  files { "demos/ulayered/ulayered.cpp" }

  settargetdir() 
  
  filter "system:windows"
    removeplatforms { "x64","arm64" } 
    removeconfigurations { "*skia" }
    files {"include/sciter-win-main.cpp"}
  filter "system:macosx"
    files {"include/sciter-osx-main.mm"}
  filter "system:linux"
    files {"include/sciter-gtk-main.cpp"}
    linkoptions { 
       "`pkg-config gtk+-3.0 --libs`",
       "`pkg-config fontconfig --libs`",
       "-fPIC",
       "-pthread",
       "-Wl,--no-undefined",
       "-ldl",
    }
  filter {}

project "inspector"
  kind "WindowedApp"
  language "C++"

  dpiawareness "HighPerMonitor"

  configuration "windows"
    prebuildcommands { 
      "%{prj.location}..\\bin.win\\packfolder.exe %{prj.location}..\\demos\\inspector\\res %{prj.location}..\\demos\\inspector\\resources.cpp -v \"resources\""
    }
  configuration {}

  files { "demos/inspector/inspector.cpp" }  

  settargetdir() 

  filter "system:windows"
    removeplatforms { "x64" }
    removeconfigurations { "*skia" }
    files {"include/sciter-win-main.cpp", 
           "demos/inspector/win-res/inspector.rc", 
           "demos/inspector/win-res/dpi-aware.manifest" }
  filter "system:macosx"
    files {"include/sciter-osx-main.mm", "demos/inspector/mac-res/*.m"}
  filter "system:linux"
    files {"include/sciter-gtk-main.cpp"}
    linkoptions { 
       "`pkg-config gtk+-3.0 --libs`",
       "`pkg-config fontconfig --libs`",
       "-fPIC",
       "-pthread",
       "-Wl,--no-undefined",
       "-ldl",
    }
  filter {}

project "notepad"
  kind "WindowedApp"
  language "C++"

  removeconfigurations { "*Skia" }

  dpiawareness "HighPerMonitor"

  targetname "html-notepad"

  configuration "windows"
    prebuildcommands { 
      "%{prj.location}..\\bin.win\\packfolder.exe %{prj.location}..\\notepad\\res %{prj.location}..\\notepad\\resources.cpp -v \"resources\""
    }
  configuration {}  

  files { "notepad/html-notepad.cpp" }

  settargetdir() 

  filter "system:windows"
    systemversion "latest"
    removeplatforms { "x64" }
    removeconfigurations { "*skia","*lite" }
    files {"include/sciter-win-main.cpp", 
           "notepad/windows/dpi-aware.manifest",
           "notepad/windows/notepad.rc" }
    includedirs { "notepad/windows" }

  filter "system:macosx"
    files {"include/sciter-osx-main.mm"}
  filter "system:linux"
    files {"include/sciter-gtk-main.cpp"}
    linkoptions { 
       "`pkg-config gtk+-3.0 --libs`",
       "`pkg-config fontconfig --libs`",
       "-fPIC",
       "-pthread",
       "-Wl,--no-undefined",
       "-ldl",
    }
  filter {}


project "notes"
  kind "WindowedApp"
  language "C++"

  dpiawareness "HighPerMonitor"

  settargetdir() 

  files { "notes/notes.cpp" }

  configuration "windows"
    prebuildcommands { 
      "%{prj.location}..\\bin.win\\packfolder.exe %{prj.location}..\\notes\\res %{prj.location}..\\notes\\resources.cpp -v \"resources\""
    }
  configuration {}  

  filter "system:windows"
    removeplatforms { "x64" }
    removeconfigurations { "*Skia" }
    files {"include/sciter-win-main.cpp", 
           "notes/windows/dpi-aware.manifest",
           "notes/windows/notes.rc" }
    includedirs { "notes/windows" }

  filter "system:macosx"
    files {"include/sciter-osx-main.mm"}
  filter "system:linux"
    files {"include/sciter-gtk-main.cpp"}
    linkoptions { 
       "`pkg-config gtk+-3.0 --libs`",
       "`pkg-config fontconfig --libs`",
       "-fPIC",
       "-pthread",
       "-Wl,--no-undefined",
       "-ldl",
    }
  filter {}

if _TARGET_OS == "windows" then

 project "wsciter"
    kind "WindowedApp"
    language "C++"

    dpiawareness "HighPerMonitor"

    includedirs { "demos/sciter" }  

    removeplatforms { "arm64" } 

    files { 
            "include/sciter-*.h",
            "include/sciter-*.hpp",
            "include/aux-*.*",
            "demos/sciter/stdafx.cpp",
            "demos/sciter/main.cpp",
            "demos/sciter/sciter.cpp",
            "demos/sciter/sciter.rc",
            "include/behaviors/behavior_drawing.cpp",
            "include/behaviors/behavior_drawing-gdi.cpp",
            "include/behaviors/behavior_native_textarea.cpp",
            "include/behaviors/behavior_tabs.cpp",
            "include/behaviors/behavior_video_generator.cpp",
            "include/behaviors/behavior_camera_capture.cpp",
            "include/behaviors/behavior_drawing-opengl.cpp",
            "demos/sciter/res/dpi-aware.manifest" }

    settargetdir() 

  project "ui-framework"
    kind "WindowedApp"
    language "C++"
    -- location "build"

    removeplatforms { "x64","arm64" }
    removeconfigurations { "*Skia" }

    dpiawareness "HighPerMonitor"

    includedirs { "demos/ui-framework" }  

    files { "demos/ui-framework/stdafx.cpp",
            "demos/ui-framework/ui-framework.cpp",
            "demos/ui-framework/worker-thread.cpp",
            "demos/ui-framework/ui-framework.rc" }
    
    settargetdir() 

  project "plain-win"
    kind "WindowedApp"
    language "C++"
    -- location "build"

    removeplatforms { "x64","arm64" }
    removeconfigurations { "*Skia" }

    dpiawareness "HighPerMonitor"

    includedirs { "demos/plain-win" }  

    files { "demos/plain-win/stdafx.cpp",
            "demos/plain-win/plain-win.cpp",
            "demos/plain-win/plain-win.rc" }

    settargetdir() 

--[[
  project "tiscript-sqlite"
    kind "SharedLib"
    language "C++"

    removeconfigurations { "*Skia" }

    files { 
      "sqlite/sqlite-wrap.c",
      "sqlite/*.cpp",
      "sqlite/*.h",
      "sqlite/tiscriptsqlite.def"  }

    settargetdir() 
]]--

end

project "glfw-opengl"
  kind "ConsoleApp"
  language "C++"

  defines "WINDOWLESS"

  cppdialect "C++17" 

  removeconfigurations { "*Skia" }

  targetdir ("bin." .. osabbr() .."/%{cfg.platform}lite")

  dpiawareness "HighPerMonitor"

  configuration "windows"
    prebuildcommands { 
      "%{prj.location}..\\bin.win\\packfolder.exe %{prj.location}..\\demos.lite\\facade %{prj.location}..\\demos.lite\\facade-resources.cpp -v \"resources\""
    }
  configuration {}

  -- ours:
  files { 
    "include/*.h", 
    "include/*.hpp",
    "demos.lite/sciter-glfw-opengl/*.h",
    "demos.lite/sciter-glfw-opengl/basic.cpp",
    "include/behaviors/behavior_drawing.cpp"
  }
  
    -- theirs, GLFW:
  includedirs { 
    "demos.lite/sciter-glfw-opengl",
    "demos.lite/glfw/include",
    "demos.lite/glfw/deps" }  

  files { 
    "demos.lite/glfw/src/context.c",
    "demos.lite/glfw/src/init.c",
    "demos.lite/glfw/src/input.c",
    "demos.lite/glfw/src/monitor.c",
    "demos.lite/glfw/src/vulkan.c",
    "demos.lite/glfw/src/window.c",
    "demos.lite/glfw/deps/glad.c",
  }

  filter "system:windows"
    entrypoint "mainCRTStartup"
    dpiawareness "HighPerMonitor"
    defines "_GLFW_WIN32"
    files {
      "demos.lite/glfw/src/win32_init.c",
      "demos.lite/glfw/src/win32_joystick.c",
      "demos.lite/glfw/src/win32_monitor.c",
      "demos.lite/glfw/src/win32_time.c",
      "demos.lite/glfw/src/win32_thread.c",
      "demos.lite/glfw/src/win32_window.c",
      "demos.lite/glfw/src/wgl_context.c",
      "demos.lite/glfw/src/egl_context.c",
      "demos.lite/glfw/src/osmesa_context.c",
    }
    links "shlwapi"
    removeplatforms { "arm64" }
  filter "system:macosx"
    defines "_GLFW_COCOA"
  filter "system:linux"  
    linkoptions { 
      "-Wall", 
      "-pthread", "-lm", 
      "-lX11","-lXrandr","-lXinerama", "-lXcursor",
      "-lGL", "-lGLU", "-ldl" }
    defines "_GLFW_X11" -- or "_GLFW_WAYLAND" or "_GLFW_MIR"
    files {
      "demos.lite/glfw/src/xkb_unicode.c",
      "demos.lite/glfw/src/glx_context.c",
      "demos.lite/glfw/src/egl_context.c",
      "demos.lite/glfw/src/osmesa_context.c",
      "demos.lite/glfw/src/posix_thread.*",
      "demos.lite/glfw/src/posix_time.*",
      "demos.lite/glfw/src/linux_*.*",
      "demos.lite/glfw/src/x11_*.*",
    }
    
  filter {}

end 

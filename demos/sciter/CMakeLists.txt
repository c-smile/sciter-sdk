message(STATUS "----------------------------------------------------------------------------")
message(STATUS "   Project:        sciter-exe                                               ")

#
# We need only release sciter.exe and sciter64.exe So, exclude DebugXXX from default build
# Though, when building both ReleaseMTU and ReleaseMDU latest wins. Hope nobody builds both.

#
# Headers

set(SCITEREXE_HEADERS_PRECOMP stdafx.h)
set(SCITER_EXE_HEADERS_PRECOMP stdafx.h)

set(SCITEREXE_HEADERS
    targetver.h
	Resource.h
	sciter.h
	../../include/aux-asset.h
	../../include/aux-cvt.h
	../../include/aux-platform.h
	../../include/aux-slice.h
	../../include/sciter-x-api.h
	../../include/sciter-x-behavior.h
	../../include/sciter-x-debug.h
	../../include/sciter-x-def.h
	../../include/sciter-x-dom.h
	../../include/sciter-x-dom.hpp
	../../include/sciter-x-graphics.h
	../../include/sciter-x-graphics.hpp
	../../include/sciter-x-host-callback.h
	../../include/sciter-x-lite.hpp
	../../include/sciter-x-request.h
	../../include/sciter-x-request.hpp
	../../include/sciter-x-threads.h
	../../include/sciter-x-types.h
	../../include/sciter-x-value.h
	../../include/sciter-x-video-api.h
	../../include/sciter-x.h
	../../include/tiscript.h
	../../include/tiscript.hpp
	../../include/value.h
	../../include/value.hpp
)

set(SCITEREXE_SOURCES_PRECOMP stdafx.cpp)

set(SCITEREXE_SOURCES
	sciter.cpp
	main.cpp
	../../include/behaviors/behavior_camera_capture.cpp
	../../include/behaviors/behavior_drawing-gdi.cpp
	../../include/behaviors/behavior_drawing.cpp
	../../include/behaviors/behavior_tabs.cpp
	../../include/behaviors/behavior_video_generator.cpp
	sciter.rc
)

#
# PROCESS FILES ABOVE
include(Utils)

add_definitions(-D_WINDOWS)

include_directories(./)
include_directories(${SC_SOURCE_DIR})
include_directories(${SC_SOURCE_DIR}/sdk/include)

#Delay load
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:\"MF.DLL\" /DELAYLOAD:\"MFPLAT.DLL\" /DELAYLOAD:\"MFREADWRITE.DLL\" /DELAYLOAD:\"shlwapi.dll\" ")

#Tweak for Resource Compiler
if (WIN32)
	set(CMAKE_RC_COMPILER_INIT windres)
	ENABLE_LANGUAGE(RC)
	SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif()

set(SCITEREXE_ALL_SOURCES ${SCITEREXE_HEADERS_PRECOMP} ${SCITEREXE_HEADERS} ${SCITEREXE_SOURCES_PRECOMP} ${SCITEREXE_SOURCES})

use_precompiled_header(SCITEREXE_SOURCES ${SCITEREXE_HEADERS_PRECOMP} ${SCITEREXE_SOURCES_PRECOMP})

add_executable(sciter-exe WIN32 ${SCITEREXE_ALL_SOURCES})
release_only(sciter-exe)

set(PDB_NAME "$(TargetDir)$(TargetName).pdb")

if (SC_ARCH_X64)
	# Don't mess with sciter64.dll
	set(SCITEREXE_NAME sciter_64)
else ()
	set(SCITEREXE_NAME sciter)
endif ()

set_target_properties(sciter-exe PROPERTIES
			OUTPUT_NAME ${SCITEREXE_NAME}
			CLEAN_DIRECT_OUTPUT 1)

IF (MSVC)
	add_custom_command(
		TARGET sciter-exe
		POST_BUILD
		COMMAND "mt.exe" -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\res\\dpi-aware.manifest\" -inputresource:\"$<TARGET_FILE:sciter-exe>\"\;\#1 -outputresource:\"$<TARGET_FILE:sciter-exe>\"\;\#1
		COMMENT "Adding display aware manifest..." 
	)
ENDIF(MSVC)

target_link_libraries(sciter-exe delayimp.lib)

#
# INSTALL BINARIES
include(InstallTarget)

install_target(sciter-exe RUNTIME sdk/bin TRUE)

message(STATUS "----------------------------------------------------------------------------")
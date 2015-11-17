message(STATUS "----------------------------------------------------------------------------")
message(STATUS "   Project:        inspector                                                ")

#
# We need only one inspector.exe So, get rid it from x64 solution and exclude DebugXXX from default build of x86
# Though, when building both ReleaseMTU and ReleaseMDU latest wins. Hope nobody builds both.

if (NOT SC_ARCH_X64)

#
# Headers

set(INSPECTOR_HEADERS_PRECOMP)

set(INSPECTOR_HEADERS
	../../include/sciter-x-window.hpp
)

set(INSPECTOR_SOURCES_PRECOMP)

set(INSPECTOR_SOURCES
    ../../include/sciter-win-main.cpp
    inspector.cpp
	win-res/inspector.rc
)

#
# PROCESS FILES ABOVE
include(Utils)

#add_definitions(-D_WINDOWS)

include_directories(./)
include_directories(${SC_SOURCE_DIR}/sdk/include)

#Tweak for Resource Compiler
if (WIN32)
	set(CMAKE_RC_COMPILER_INIT windres)
	ENABLE_LANGUAGE(RC)
	SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif()

set(INSPECTOR_ALL_SOURCES ${INSPECTOR_HEADERS_PRECOMP} ${INSPECTOR_HEADERS} ${INSPECTOR_SOURCES_PRECOMP} ${INSPECTOR_SOURCES})

#use_precompiled_header(INSPECTOR_SOURCES ${INSPECTOR_HEADERS_PRECOMP} ${INSPECTOR_SOURCES_PRECOMP})

add_executable(inspector WIN32 ${INSPECTOR_ALL_SOURCES})
release_only(inspector)

set(PDB_NAME "$(TargetDir)$(TargetName).pdb")

#target_link_libraries(inspector tool)

IF (MSVC)
	add_custom_command(
		TARGET inspector
		POST_BUILD
		COMMAND "mt.exe" -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\win-res\\dpi-aware.manifest\" -inputresource:\"$<TARGET_FILE:inspector>\"\;\#1 -outputresource:\"$<TARGET_FILE:inspector>\"\;\#1
		COMMENT "Adding display aware manifest..." 
	)
ENDIF(MSVC)

#
# INSTALL BINARIES
include(InstallTarget)

install_target(inspector RUNTIME sdk/bin TRUE)

else()

message(STATUS "                   Skipped for x64 build                                    ")

endif () # if (NOT SC_ARCH_X64)

message(STATUS "----------------------------------------------------------------------------")
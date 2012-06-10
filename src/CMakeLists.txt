
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/version.h"
)

if(WIN32)
    include(Win32PostBuild)
else()
    include(PosixPostBuild)
endif()

include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}/meshlib ${CMAKE_CURRENT_SOURCE_DIR}/trelib ${PROJECT_SOURCE_DIR}/swgOSG
)

## project libraries
add_subdirectory(anh)
add_subdirectory(meshlib)
add_subdirectory(trelib)

## project executables
add_subdirectory(swganh)

## publish 14 core implementation
add_subdirectory(pub14_core)

## game editor - win32 only
if(WIN32)
    add_subdirectory(swged)
endif()

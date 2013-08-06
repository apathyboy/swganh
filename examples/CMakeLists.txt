
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

#add_subdirectory(datatable_reader)
#add_subdirectory(tre_archiver)
#add_subdirectory(tre_reader)
#add_subdirectory(tre_unpacker)

if(WIN32)
#add_subdirectory(terrain_renderer)
endif()

add_subdirectory(iff_dump)
add_subdirectory(terrain_readerwriter)


include(GroupSourcesByFilesystem)

file(GLOB_RECURSE SOURCES *.h *.hpp *.cpp *.cc)

GroupSourcesByFilesystem("${SOURCES}")

include_directories(
    ${DirectX_D3D11_INCLUDE_DIR}
)

add_executable(terrain_renderer WIN32 ${SOURCES})

target_link_libraries(terrain_renderer
    ${DirectX_D3D11_LIBRARY}
)

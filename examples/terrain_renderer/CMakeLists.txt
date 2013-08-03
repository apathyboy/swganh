
include(GroupSourcesByFilesystem)

file(GLOB_RECURSE SOURCES *.h *.hpp *.cpp *.cc)

GroupSourcesByFilesystem("${SOURCES}")

include_directories(
	${GLM_INCLUDE_DIR}
    ${DirectX_D3D11_INCLUDE_DIR}
)

add_executable(terrain_renderer WIN32 ${SOURCES})

target_link_libraries(terrain_renderer
	swganh_lib
    ${DirectX_D3D11_LIBRARY}
)


include(GroupSourcesByFilesystem)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES *.h *.hpp *.cpp *.cc)
file(GLOB_RECURSE INTERFACES *.ui)
file(GLOB_RECURSE RESOURCES *.qrc *.qss)

GroupSourcesByFilesystem("${SOURCES}")
GroupSourcesByFilesystem("${INTERFACES}")

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

qt5_wrap_ui(UI_SOURCES ${INTERFACES})
qt5_add_resources(RESOURCES_RCC ${RESOURCES})

add_executable(swged ${GUI_TYPE} ${SOURCES} ${UI_SOURCES} ${RESOURCES_RCC})

target_link_libraries(swged
    swganh_lib
    ${Boost_LIBRARIES}
    ${Qt5Core_QTMAIN_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

# Use the Widgets module from Qt 5
qt5_use_modules(swged Concurrent)
qt5_use_modules(swged Widgets)
qt5_use_modules(swged OpenGL)
qt5_use_modules(swged OpenGLExtensions)

if(ZLIB_LIBRARY_DEBUG)
    target_link_libraries(swged debug ${ZLIB_LIBRARY_DEBUG})
    get_filename_component(ZLIB_PATH ${ZLIB_LIBRARY_DEBUG} PATH)
    get_filename_component(ZLIB_FILE ${ZLIB_LIBRARY_DEBUG} NAME_WE)
endif()

if(ZLIB_LIBRARY_RELEASE)
    target_link_libraries(swged optimized ${ZLIB_LIBRARY_RELEASE})
    get_filename_component(ZLIB_PATH ${ZLIB_LIBRARY_RELEASE} PATH)
    get_filename_component(ZLIB_FILE ${ZLIB_LIBRARY_RELEASE} NAME_WE)
endif()

if(WIN32)
    add_custom_command(TARGET swged POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${ZLIB_PATH}/${ZLIB_FILE}.dll
            $<TARGET_FILE_DIR:swged>)
endif()

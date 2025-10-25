cmake_minimum_required(VERSION 3.14)

# Automatically find all .cpp files in this directory
file(GLOB PLUGIN_SOURCES CONFIGURE_DEPENDS "*.cpp")

if(NOT PLUGIN_SOURCES)
    message(STATUS "No plugin sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
    return()
endif()

set(PLUGIN_TARGETS "")

foreach(source_file IN LISTS PLUGIN_SOURCES)
    # Get filename without path and extension
    get_filename_component(plugin_name ${source_file} NAME_WE)

    # Create a shared library (DLL) for each plugin
    add_library(${plugin_name} SHARED ${source_file})

    set_target_properties(${plugin_name} PROPERTIES
            PREFIX ""           # No "lib" prefix on Windows
            OUTPUT_NAME "${plugin_name}"  # Explicit name
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/static_plugins  # optional: control where .a goes
    )

    list(APPEND PLUGIN_TARGETS ${plugin_name})
endforeach()

# Expose PLUGIN_TARGETS to parent scope so main CMakeLists can copy them
set(PLUGIN_TARGETS ${PLUGIN_TARGETS} PARENT_SCOPE)

message(STATUS "Found ${CMAKE_CURRENT_SOURCE_DIR} plugins: ${PLUGIN_TARGETS}")
cmake_minimum_required(VERSION 3.10)
project(LogDBProject)

# For analysis with clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to static link Qt5
option(STATIC_LINK_QT "Link Qt libraries statically" OFF)
if(STATIC_LINK_QT)
    add_definitions(-DQT_STATIC)
endif()

# Enable automoc, autouic, and autorcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# QT5
find_package(Qt5 COMPONENTS Widgets Sql REQUIRED)
file(GLOB includes ${CMAKE_SOURCE_DIR}/include)

# Specify the source files for the executable.
file(GLOB SRC_TOP        "${CMAKE_SOURCE_DIR}/src/*.cpp")          # new
file(GLOB_RECURSE SRC_REST CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/**/*.cpp")

set(SRC_FILES  ${SRC_TOP} ${SRC_REST})
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE H_FILES "${CMAKE_SOURCE_DIR}/include/*.h")
set(
    SOURCES
    ${SRC_FILES}
    ${HEADER_FILES}
    ${H_FILES}
)

# Add UI files explicitly for AUTOUIC to pick up. This is to avoid some issues I ran into with the latest ui design not being picked up by autouic.
set(UI_FILES
    ${CMAKE_SOURCE_DIR}/ui/standard_cut_dialog.ui
    ${CMAKE_SOURCE_DIR}/ui/validate_log_length_dialog.ui
    ${CMAKE_SOURCE_DIR}/ui/adjust_log_length_dialog.ui
    ${CMAKE_SOURCE_DIR}/ui/inventory.ui
    ${CMAKE_SOURCE_DIR}/ui/cutlist.ui
    ${CMAKE_SOURCE_DIR}/ui/sales.ui
    ${CMAKE_SOURCE_DIR}/ui/mainwindow.ui
)

# Add them to the source list
list(APPEND SOURCES ${UI_FILES})

# Create the executable target.
add_executable(logdb ${SOURCES})
# New executable for the test harness. Includes everything except main.cpp
add_executable(Woodworks_test src/main_testing.cpp ${SOURCES})
target_link_libraries(Woodworks_test PRIVATE Qt5::Widgets)

# Define the BUILDING_WOODWORKS_TEST macro to select the right main
target_compile_definitions(Woodworks_test PRIVATE BUILDING_WOODWORKS_TEST)

# -----------------------------------------------------------------------------
# Documentation (Doxygen)
# -----------------------------------------------------------------------------

option(BUILD_DOCS "Generate API documentation with Doxygen" OFF)

if (BUILD_DOCS)
    # Attempt to locate Doxygen quietly; documentation is optional.
    find_package(Doxygen QUIET COMPONENTS dot)

    if (DOXYGEN_FOUND)
        # configure_file copies our checked‑in Doxyfile, replaces variables, and
        # writes a build‑dir version called Doxyfile
        set(DOXYGEN_IN  ${CMAKE_SOURCE_DIR}/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} COPYONLY)

        add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/..
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    else()
        message(WARNING "Doxygen not found; documentation target will be skipped.")
    endif()
endif()

# Set the AUTOUIC_SEARCH_PATHS property to the source directory.
set_target_properties(logdb PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/ui;${CMAKE_SOURCE_DIR}/ui/widgets"
)
set_target_properties(Woodworks_test PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/ui;${CMAKE_SOURCE_DIR}/ui/widgets"
)

# Specify the include directories.
target_include_directories(logdb PUBLIC ${includes})
target_include_directories(Woodworks_test PUBLIC ${includes})

target_link_libraries(logdb PRIVATE Qt5::Widgets Qt5::Sql)
target_link_libraries(Woodworks_test PRIVATE Qt5::Widgets Qt5::Sql)

add_compile_options(
    # Standard warnings.
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    # Warning guards against undefined behavior.
    -Wundef
    -Wuninitialized
    #-Wuse-after-free (only in clang)
    -Wstrict-aliasing
    -Wstrict-overflow
    -Warray-bounds
    -Wsequence-point #(only in gcc and deprecated after C++20)
    # Additional warnings that are a good idea.
    -Wsign-conversion
    -Wsign-compare
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
    -Wcast-align
)

# Copy in the database file
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/test.db
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/example.db
            ${CMAKE_BINARY_DIR}/test.db
    DEPENDS ${CMAKE_SOURCE_DIR}/example.db
    COMMENT "Copying example.db to test.db in build directory"
)

add_custom_target(copy_database ALL DEPENDS ${CMAKE_BINARY_DIR}/test.db)

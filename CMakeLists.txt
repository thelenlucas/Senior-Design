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

# Fetchcontent for SQLiteCpp, tag 3.3.2
include(FetchContent)
FetchContent_Declare(
    SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG        3.3.2
)
FetchContent_MakeAvailable(SQLiteCpp)

# Specify the source files for the executable.
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
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

# Set the AUTOUIC_SEARCH_PATHS property to the source directory.
set_target_properties(logdb PROPERTIES
    AUTOMOC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/include;${CMAKE_SOURCE_DIR}/src"
    AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/ui"
)

# Specify the include directories.
target_include_directories(logdb PUBLIC ${includes})

target_link_libraries(logdb PRIVATE SQLiteCpp Qt5::Widgets Qt5::Sql)

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

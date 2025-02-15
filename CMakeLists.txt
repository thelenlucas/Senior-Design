cmake_minimum_required(VERSION 3.10)
project(LogDBProject)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automoc, autouic, and autorcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# QT5
find_package(Qt5 COMPONENTS Widgets REQUIRED)
add_subdirectory(${CMAKE_SOURCE_DIR}/SQLiteCpp)
file(GLOB includes ${CMAKE_SOURCE_DIR}/include)

# Specify the source files for the executable.
set(SOURCES
    src/main.cpp
    src/db.cpp
    src/logs.cpp
    src/mainwindow.cpp
    include/mainwindow.h
)

# Create the executable target.
add_executable(logdb ${SOURCES})

# Set the AUTOUIC_SEARCH_PATHS property to the source directory.
set_target_properties(logdb PROPERTIES AUTOMOC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/include")

# Specify the include directories.
target_include_directories(logdb PUBLIC ${includes})

target_link_libraries(logdb PRIVATE SQLiteCpp Qt5::Widgets)

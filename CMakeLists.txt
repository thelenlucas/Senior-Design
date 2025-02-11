cmake_minimum_required(VERSION 3.10)
project(LogDBProject)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/SQLiteCpp)

# Specify the source files for the executable.
set(SOURCES
    src/main.cpp
    src/db.cpp
    src/logs.cpp
)

# Create the executable target.
add_executable(logdb ${SOURCES})

# Specify the include directories.
target_include_directories(logdb PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(logdb PRIVATE SQLiteCpp)

cmake_minimum_required(VERSION 3.10)
project(LogDBProject)

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

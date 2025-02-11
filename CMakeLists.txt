cmake_minimum_required(VERSION 3.10)
project(LogDBProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the SQLitecpp dir
add_subdirectory(${CMAKE_SOURCE_DIR}/SQLiteCpp)

# Create the executable from source files.
add_executable(logdb src/main.cpp)

# Include directories:
# - Include directory (our code)
# - The sqlite-amalgamation directory
target_include_directories(logdb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite-amalgamation
)

# Link against the sqlite3 library created in the sqlite-amalgamation subdirectory.
target_link_libraries(
    logdb
    SQLiteCpp
    sqlite3
    pthread
    dl
)

cmake_minimum_required(VERSION 3.10)
project(LogDBProject)

# Use a modern C++ standard (adjust as needed).
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the sqlite-amalgamation subdirectory.
add_subdirectory(sqlite-amalgamation)

# Create the executable from your source file(s).
add_executable(logdb src/main.cpp)

# Include directories:
# - Your project's include directory (e.g. for logs.hpp)
# - The sqlite-amalgamation directory (to find sqlite3.h, etc.)
target_include_directories(logdb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite-amalgamation
)

# Link against the sqlite3 library created in the sqlite-amalgamation subdirectory.
target_link_libraries(logdb PRIVATE sqlite3)

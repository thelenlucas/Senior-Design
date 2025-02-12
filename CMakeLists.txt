cmake_minimum_required(VERSION 3.10)
project(LogDBProject)

# We need to make sure we are compiling with GCC and not MSVC or Clang by mistake.
#if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
#    message(FATAL_ERROR "This project must be built with GCC compiler.")
#endif()

#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_CXX_COMPILER g++)

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

# Setup for Qt5 packages for development.
set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql Gui)

# Specify the include directories.
target_include_directories(logdb PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${Qt5_INCLUDE_DIRS}
)

target_link_libraries(logdb PRIVATE Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Gui SQLiteCpp)

# Set installation directory.
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/dist")

# Install target executable.
install(TARGETS logdb
    RUNTIME DESTINATION bin
)

# Install Qt5 dependencies.
install(DIRECTORY "${CMAKE_PREFIX_PATH}/bin/"
    DESTINATION bin
    FILES_MATCHING PATTERN "*.dll" # Windows
)

install(DIRECTORY "${CMAKE_PREFIX_PATH}/lib/"
    DESTINATION lib
    FILES_MATCHING PATTERN "*.so*" # Linux
)

# Install any Qt5 plugins if used here.
install(DIRECTORY "${CMAKE_PREFIX_PATH}/plugins/"
    DESTINATION plugins
)

# Install any Qt5 translations if used here. (probably not but just incase)
install(DIRECTORY "${CMAKE_PREFIX_PATH}/translations/"
    DESTINATION translations
)

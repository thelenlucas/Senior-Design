cmake_minimum_required(VERSION 3.5)

project(Senior_Design)

add_library(sqlite3 STATIC sqlite-amalgamation/sqlite3.c)
target_include_directories(sqlite3 PUBLIC .)

add_executable(Senior_Design src/main.cpp)

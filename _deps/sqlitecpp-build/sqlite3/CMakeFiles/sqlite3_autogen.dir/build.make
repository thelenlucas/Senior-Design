# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/c/repos/Senior-Design

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/repos/Senior-Design

# Utility rule file for sqlite3_autogen.

# Include any custom commands dependencies for this target.
include _deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include _deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/progress.make

_deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen: _deps/sqlitecpp-build/sqlite3/sqlite3_autogen/timestamp

_deps/sqlitecpp-build/sqlite3/sqlite3_autogen/timestamp: /usr/lib/qt5/bin/moc
_deps/sqlitecpp-build/sqlite3/sqlite3_autogen/timestamp: /usr/lib/qt5/bin/uic
_deps/sqlitecpp-build/sqlite3/sqlite3_autogen/timestamp: _deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/mnt/c/repos/Senior-Design/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target sqlite3"
	cd /mnt/c/repos/Senior-Design/_deps/sqlitecpp-build/sqlite3 && /usr/bin/cmake -E cmake_autogen /mnt/c/repos/Senior-Design/_deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/AutogenInfo.json ""
	cd /mnt/c/repos/Senior-Design/_deps/sqlitecpp-build/sqlite3 && /usr/bin/cmake -E touch /mnt/c/repos/Senior-Design/_deps/sqlitecpp-build/sqlite3/sqlite3_autogen/timestamp

sqlite3_autogen: _deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen
sqlite3_autogen: _deps/sqlitecpp-build/sqlite3/sqlite3_autogen/timestamp
sqlite3_autogen: _deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/build.make
.PHONY : sqlite3_autogen

# Rule to build all files generated by this target.
_deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/build: sqlite3_autogen
.PHONY : _deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/build

_deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/clean:
	cd /mnt/c/repos/Senior-Design/_deps/sqlitecpp-build/sqlite3 && $(CMAKE_COMMAND) -P CMakeFiles/sqlite3_autogen.dir/cmake_clean.cmake
.PHONY : _deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/clean

_deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/depend:
	cd /mnt/c/repos/Senior-Design && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/repos/Senior-Design /mnt/c/repos/Senior-Design/_deps/sqlitecpp-src/sqlite3 /mnt/c/repos/Senior-Design /mnt/c/repos/Senior-Design/_deps/sqlitecpp-build/sqlite3 /mnt/c/repos/Senior-Design/_deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : _deps/sqlitecpp-build/sqlite3/CMakeFiles/sqlite3_autogen.dir/depend


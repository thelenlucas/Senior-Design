# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/mnt/c/repos/Senior-Design/_deps/sqlitecpp-src"
  "/mnt/c/repos/Senior-Design/_deps/sqlitecpp-build"
  "/mnt/c/repos/Senior-Design/_deps/sqlitecpp-subbuild/sqlitecpp-populate-prefix"
  "/mnt/c/repos/Senior-Design/_deps/sqlitecpp-subbuild/sqlitecpp-populate-prefix/tmp"
  "/mnt/c/repos/Senior-Design/_deps/sqlitecpp-subbuild/sqlitecpp-populate-prefix/src/sqlitecpp-populate-stamp"
  "/mnt/c/repos/Senior-Design/_deps/sqlitecpp-subbuild/sqlitecpp-populate-prefix/src"
  "/mnt/c/repos/Senior-Design/_deps/sqlitecpp-subbuild/sqlitecpp-populate-prefix/src/sqlitecpp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/mnt/c/repos/Senior-Design/_deps/sqlitecpp-subbuild/sqlitecpp-populate-prefix/src/sqlitecpp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/mnt/c/repos/Senior-Design/_deps/sqlitecpp-subbuild/sqlitecpp-populate-prefix/src/sqlitecpp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Projects/spreadsheet/_deps/googletest-src"
  "C:/Projects/spreadsheet/_deps/googletest-build"
  "C:/Projects/spreadsheet/_deps/googletest-subbuild/googletest-populate-prefix"
  "C:/Projects/spreadsheet/_deps/googletest-subbuild/googletest-populate-prefix/tmp"
  "C:/Projects/spreadsheet/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
  "C:/Projects/spreadsheet/_deps/googletest-subbuild/googletest-populate-prefix/src"
  "C:/Projects/spreadsheet/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Projects/spreadsheet/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Projects/spreadsheet/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/carter/circpp/circuitpp/test/build/_deps/format.cmake-src"
  "/Users/carter/circpp/circuitpp/test/build/_deps/format.cmake-build"
  "/Users/carter/circpp/circuitpp/test/build/_deps/format.cmake-subbuild/format.cmake-populate-prefix"
  "/Users/carter/circpp/circuitpp/test/build/_deps/format.cmake-subbuild/format.cmake-populate-prefix/tmp"
  "/Users/carter/circpp/circuitpp/test/build/_deps/format.cmake-subbuild/format.cmake-populate-prefix/src/format.cmake-populate-stamp"
  "/Users/carter/circpp/circuitpp/test/build/_deps/format.cmake-subbuild/format.cmake-populate-prefix/src"
  "/Users/carter/circpp/circuitpp/test/build/_deps/format.cmake-subbuild/format.cmake-populate-prefix/src/format.cmake-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/carter/circpp/circuitpp/test/build/_deps/format.cmake-subbuild/format.cmake-populate-prefix/src/format.cmake-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/carter/circpp/circuitpp/test/build/_deps/format.cmake-subbuild/format.cmake-populate-prefix/src/format.cmake-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

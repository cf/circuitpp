# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/carter/circpp/circuitpp/test/.."
  "/Users/carter/circpp/circuitpp/test/build/_deps/circuitpp-build"
  "/Users/carter/circpp/circuitpp/test/build/_deps/circuitpp-subbuild/circuitpp-populate-prefix"
  "/Users/carter/circpp/circuitpp/test/build/_deps/circuitpp-subbuild/circuitpp-populate-prefix/tmp"
  "/Users/carter/circpp/circuitpp/test/build/_deps/circuitpp-subbuild/circuitpp-populate-prefix/src/circuitpp-populate-stamp"
  "/Users/carter/circpp/circuitpp/test/build/_deps/circuitpp-subbuild/circuitpp-populate-prefix/src"
  "/Users/carter/circpp/circuitpp/test/build/_deps/circuitpp-subbuild/circuitpp-populate-prefix/src/circuitpp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/carter/circpp/circuitpp/test/build/_deps/circuitpp-subbuild/circuitpp-populate-prefix/src/circuitpp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/carter/circpp/circuitpp/test/build/_deps/circuitpp-subbuild/circuitpp-populate-prefix/src/circuitpp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/carter/circpp/circuitpp/standalone/build/_deps/cxxopts-src"
  "/Users/carter/circpp/circuitpp/standalone/build/_deps/cxxopts-build"
  "/Users/carter/circpp/circuitpp/standalone/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix"
  "/Users/carter/circpp/circuitpp/standalone/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/tmp"
  "/Users/carter/circpp/circuitpp/standalone/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/src/cxxopts-populate-stamp"
  "/Users/carter/circpp/circuitpp/standalone/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/src"
  "/Users/carter/circpp/circuitpp/standalone/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/src/cxxopts-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/carter/circpp/circuitpp/standalone/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/src/cxxopts-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/carter/circpp/circuitpp/standalone/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/src/cxxopts-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

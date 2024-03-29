#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "CircuitPP::CircuitPP" for configuration "Debug"
set_property(TARGET CircuitPP::CircuitPP APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CircuitPP::CircuitPP PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/CircuitPP-1.0/libCircuitPP.a"
  )

list(APPEND _cmake_import_check_targets CircuitPP::CircuitPP )
list(APPEND _cmake_import_check_files_for_CircuitPP::CircuitPP "${_IMPORT_PREFIX}/lib/CircuitPP-1.0/libCircuitPP.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

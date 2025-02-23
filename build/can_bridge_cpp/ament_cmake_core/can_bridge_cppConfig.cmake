# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_can_bridge_cpp_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED can_bridge_cpp_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(can_bridge_cpp_FOUND FALSE)
  elseif(NOT can_bridge_cpp_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(can_bridge_cpp_FOUND FALSE)
  endif()
  return()
endif()
set(_can_bridge_cpp_CONFIG_INCLUDED TRUE)

# output package information
if(NOT can_bridge_cpp_FIND_QUIETLY)
  message(STATUS "Found can_bridge_cpp: 0.0.0 (${can_bridge_cpp_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'can_bridge_cpp' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${can_bridge_cpp_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(can_bridge_cpp_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${can_bridge_cpp_DIR}/${_extra}")
endforeach()

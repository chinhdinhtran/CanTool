# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\CanTool_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CanTool_autogen.dir\\ParseCache.txt"
  "CanTool_autogen"
  )
endif()

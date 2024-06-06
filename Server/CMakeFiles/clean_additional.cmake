# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\ServerDemo_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ServerDemo_autogen.dir\\ParseCache.txt"
  "ServerDemo_autogen"
  )
endif()

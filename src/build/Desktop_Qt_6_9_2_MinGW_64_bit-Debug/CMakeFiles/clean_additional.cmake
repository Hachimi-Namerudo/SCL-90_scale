# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\SCL-90_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SCL-90_autogen.dir\\ParseCache.txt"
  "SCL-90_autogen"
  )
endif()

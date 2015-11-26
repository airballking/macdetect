cmake_minimum_required(VERSION 2.6)
project(macdetect)

include(FindPkgConfig)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(
  macdetect-utils/include
  macdetectd/include
  macdetect-client/include
  pymacdetect/include)

add_subdirectory(macdetect-utils)
add_subdirectory(macdetectd)
add_subdirectory(macdetect-client)
add_subdirectory(pymacdetect)

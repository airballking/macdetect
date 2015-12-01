cmake_minimum_required(VERSION 2.6)
project(macdetect)

include(FindPkgConfig)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

enable_testing()

include_directories(
  utils/include
  macdetectd/include
  client/include
  pymacdetect/include)

add_subdirectory(utils)
add_subdirectory(macdetectd)
add_subdirectory(client)
add_subdirectory(pymacdetect)


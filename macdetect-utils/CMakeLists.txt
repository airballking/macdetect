cmake_minimum_required(VERSION 2.8)
project(macdetect-utils)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories(
  include)

add_library(${PROJECT_NAME}
  src/macdetect-utils/Packet.cpp
  src/macdetect-utils/PacketEntity.cpp)

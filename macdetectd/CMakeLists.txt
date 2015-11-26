cmake_minimum_required(VERSION 2.8)
project(macdetectd)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories(
  include)

add_executable(${PROJECT_NAME}
  src/macdetectd/daemon.cpp
  src/macdetectd/Network.cpp
  src/macdetectd/Device.cpp
  src/macdetectd/Event.cpp
  src/macdetectd/SocketReader.cpp
  src/macdetectd/Wire.cpp
  src/macdetectd/Data.cpp
  src/macdetectd/RARP.cpp
  src/macdetectd/Daemon.cpp
  src/macdetectd/Server.cpp
  src/macdetectd/Served.cpp)

target_link_libraries(${PROJECT_NAME}
  macdetect-utils)

cmake_minimum_required(VERSION 2.8)
project(macdetectd)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories(
  include)

add_executable(${PROJECT_NAME}
  src/daemon.cpp
  src/Network.cpp
  src/Device.cpp
  src/Event.cpp
  src/SocketReader.cpp
  src/Wire.cpp
  src/Data.cpp
  src/RARP.cpp
  src/Daemon.cpp
  src/Server.cpp
  src/Served.cpp)

target_link_libraries(${PROJECT_NAME}
  macdetect-utils)

add_executable(test-readvendordata
  src/Data.cpp
  tests/readvendordata.cpp)

add_test(NAME readvendordata
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test-readvendordata)

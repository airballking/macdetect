cmake_minimum_required(VERSION 2.8)
project(MACDetect)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

set(EXECUTABLE_NAME macdetect)
set(DAEMON_NAME macdetectd)
set(CLI_NAME macdetect-cli)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories(
  include)

add_executable(${EXECUTABLE_NAME}
  src/main.cpp)

add_executable(${DAEMON_NAME}
  src/daemon.cpp
  src/macdetect/Network.cpp
  src/macdetect/Device.cpp
  src/macdetect/Event.cpp
  src/macdetect/SocketReader.cpp
  src/macdetect/Wire.cpp
  src/macdetect/Data.cpp
  src/macdetect/RARP.cpp
  src/macdetect/protocols/IP.cpp
  src/macdetect/Daemon.cpp
  src/macdetect/Server.cpp
  src/macdetect/Served.cpp
  src/macdetect/Packet.cpp
  src/macdetect/PacketEntity.cpp)

add_executable(${CLI_NAME}
  src/cli.cpp
  src/macdetect-client/MDClient.cpp
  src/macdetect/Client.cpp
  src/macdetect/Packet.cpp
  src/macdetect/PacketEntity.cpp)

add_library(${PROJECT_NAME}
  src/macdetect/Network.cpp
  src/macdetect/Device.cpp
  src/macdetect/Event.cpp
  src/macdetect/SocketReader.cpp
  src/macdetect/Wire.cpp
  src/macdetect/Data.cpp
  src/macdetect/RARP.cpp
  src/macdetect/protocols/IP.cpp)

target_link_libraries(${EXECUTABLE_NAME}
  ${PROJECT_NAME})

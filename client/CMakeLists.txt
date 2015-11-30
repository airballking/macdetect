cmake_minimum_required(VERSION 2.8)
project(macdetect-client)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

include_directories(
  include)

add_library(${PROJECT_NAME}
  src/Client.cpp
  src/MDClient.cpp)

add_executable(macdetect-cli
  src/cli.cpp)

target_link_libraries(${PROJECT_NAME}
  macdetect-utils)

target_link_libraries(macdetect-cli
  ${PROJECT_NAME})

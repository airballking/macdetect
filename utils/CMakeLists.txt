cmake_minimum_required(VERSION 2.8)
project(macdetect-utils)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

include_directories(
  include)

add_library(${PROJECT_NAME}
  src/Value.cpp
  src/PacketEntity.cpp
  src/ArgumentParser.cpp)

add_executable(test-argumentparser
  src/ArgumentParser.cpp
  tests/argumentparser.cpp)

add_test(NAME argumentparser
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test-argumentparser)

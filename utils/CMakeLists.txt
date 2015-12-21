cmake_minimum_required(VERSION 2.8)
project(macdetect-utils)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

include_directories(
  include)

add_library(${PROJECT_NAME}
  src/Value.cpp
  src/PacketEntity.cpp
  src/ArgumentParser.cpp
  src/Config.cpp)

add_executable(test-argumentparser
  src/ArgumentParser.cpp
  tests/argumentparser.cpp)

add_executable(test-config
  src/Config.cpp
  tests/config.cpp)

add_test(NAME argumentparser
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test-argumentparser)

add_test(NAME config
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test-config ${CMAKE_SOURCE_DIR}/macdetectd/config/default.config)

## Install
install(TARGETS ${PROJECT_NAME}
  DESTINATION "lib"
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ) # 750

file(GLOB HEADERS "include/macdetect-utils/*.h")

install(FILES ${HEADERS}
  DESTINATION "include/macdetect/macdetect-utils"
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ) # 750

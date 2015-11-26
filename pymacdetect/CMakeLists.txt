cmake_minimum_required(VERSION 2.8)
project(pymacdetect)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories(
  include)

#add_library(${PROJECT_NAME}
#  src/pymacdetect/PyMACDetect.cpp)

#target_link_libraries(${PROJECT_NAME}
#  macdetect-client)

add_custom_command(OUTPUT pymacdetect.so
  COMMAND python "${PROJECT_SOURCE_DIR}/src/pymacdetect/setup.py" build --build-lib "${LIBRARY_OUTPUT_PATH}"
  DEPENDS macdetect-client
  COMMENT "Compiling the PyMACDetect libraries")

add_custom_target(${PROJECT_NAME} ALL DEPENDS pymacdetect.so)

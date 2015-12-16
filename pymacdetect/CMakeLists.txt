cmake_minimum_required(VERSION 2.8)
project(pymacdetect)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

include_directories(
  include)

set_directory_properties(PROPERTIES
  ADDITIONAL_MAKE_CLEAN_FILES "${LIBRARY_OUTPUT_PATH}/pymacdetect.so")

add_custom_command(OUTPUT pymacdetect.so
  COMMAND python "${PROJECT_SOURCE_DIR}/scripts/setup.py" build --build-lib "${LIBRARY_OUTPUT_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}"
  DEPENDS macdetect-client
  COMMENT "Compiling the PyMACDetect libraries")

add_custom_target(${PROJECT_NAME} ALL DEPENDS pymacdetect.so)

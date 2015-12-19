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

add_test(NAME nosetests-${PROJECT_NAME}
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests.sh)

# Just call the default python installer. That handles installing
# everything appropriately.
install(CODE "execute_process(COMMAND python ${PROJECT_SOURCE_DIR}/scripts/setup.py install -f --prefix=${CMAKE_INSTALL_PREFIX} ${PROJECT_SOURCE_DIR})")

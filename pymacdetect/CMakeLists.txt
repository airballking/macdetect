cmake_minimum_required(VERSION 2.8)
project(pymacdetect)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

include_directories(
  include)

set_directory_properties(PROPERTIES
  ADDITIONAL_MAKE_CLEAN_FILES "${LIBRARY_OUTPUT_PATH}/pymacdetect_ext.so")

find_program(PYTHON "python")

if(PYTHON)
  set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/scripts/setup.py.in")
  set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
  set(DEPS        "macdetect-client")
  
  configure_file(${SETUP_PY_IN} ${SETUP_PY})
  
  add_custom_command(
    OUTPUT pymacdetect_ext.so
    COMMAND ${PYTHON}
    ARGS setup.py build --build-lib "${LIBRARY_OUTPUT_PATH}"
    DEPENDS ${DEPS}
    COMMENT "Compiling the PyMACDetect libraries")
  
  add_custom_target(${PROJECT_NAME} ALL DEPENDS pymacdetect_ext.so)
  
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/setup.py"
    COMPONENT client-pymacdetect
    DESTINATION "/usr/share/pyshared/pymacdetect/scripts")
  install(FILES lib/pymd/__init__.py lib/pymd/pymdClient.py lib/pymd/pymdDesktop.py
    COMPONENT client-pymacdetect
    DESTINATION "/usr/share/pyshared/pymacdetect/lib/pymd")
  install(FILES lib/pymd/pymdGui/__init__.py lib/pymd/pymdGui/MDGui.py lib/pymd/pymdGui/ConnectionManager.py 
    COMPONENT client-pymacdetect
    DESTINATION "/usr/share/pyshared/pymacdetect/lib/pymd/pymdGui")
  install(FILES lib/pymd/pymdLib/PyMACDetect.py
    COMPONENT client-pymacdetect
    DESTINATION "/usr/share/pyshared/pymacdetect/lib/pymd/pymdLib")
endif()

add_test(NAME nosetests-${PROJECT_NAME}
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/tests.sh)

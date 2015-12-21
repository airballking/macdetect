cmake_minimum_required(VERSION 2.6)
project(macdetect)

include(FindPkgConfig)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

enable_testing()

include_directories(
  utils/include
  macdetectd/include
  client/include
  pymacdetect/include)

add_subdirectory(utils)
add_subdirectory(macdetectd)
add_subdirectory(client)
add_subdirectory(pymacdetect)

## Install
install(FILES "data/oui.txt"
  DESTINATION "share/macdetect/data")

install(DIRECTORY "docs/_build/html"
  DESTINATION "doc/macdetect")

## CPack
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_NAME})
set(CPACK_PACKAGE_NAME "macdetect")
set(CPACK_PACKAGE_VENDOR "Jan Winkler <jan.winkler.84@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set(CPACK_PACKAGE_EXECUTABLES "macdetectd")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jan Winkler <jan.winkler.84@gmail.com>")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-release")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "python2.7")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/pymacdetect/scripts/postinst;${CMAKE_SOURCE_DIR}/pymacdetect/scripts/prerm;")
set(CPACK_SET_DESTDIR "ON")

include(CPack)

cmake_minimum_required(VERSION 2.6)
project(macdetect)

include(FindPkgConfig)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-undefined")

set(CMAKE_INSTALL_PREFIX "/usr")

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
  DESTINATION "share/macdetect/data"
  COMPONENT daemon
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ) # 750

install(DIRECTORY "docs/_build/html"
  DESTINATION "share/macdetect/doc"
  COMPONENT daemon
  PATTERN ".git" EXCLUDE
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ) # 750

## CPack
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MAC address based host detection framework")
set(CPACK_DEBIAN_PACKAGE_NAME "macdetect")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER  "Jan Winkler <jan.winkler.84@gmail.com>")
set(CPACK_PACKAGE_VENDOR "Jan Winkler <jan.winkler.84@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Host detection framework for monitoring networks
 Monitors any given number of networks for hosts (C++/Python).
 Contains a C++ daemon that monitors networks, and C++ and Python libraries
 plus a Python client program for reading events from the daemon.")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "7")
set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}${CPACK_PACKAGE_REVISION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_PACKAGE_EXECUTABLES "macdetectd")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jan Winkler <jan.winkler.84@gmail.com>")
#set(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}_${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "python2.7, libc6 (>= 2.19-0ubuntu6)")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/scripts/postinst;${CMAKE_SOURCE_DIR}/scripts/prerm;")
set(CPACK_SET_DESTDIR "ON")
set(CPACK_SOURCE_STRIP_FILES "")
set(CPACK_STRIP_FILES "usr/bin/macdetectd;usr/bin/macdetect-client-cli")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}")
set(CPACK_DEB_COMPONENT_INSTALL ON)

# Intercomponent-Dependencies
set(CPACK_COMPONENT_DAEMON_DEPENDS lib)
set(CPACK_COMPONENT_CLIENT-CLI_DEPENDS lib)
set(CPACK_COMPONENT_CLIENT-PYMACDETECT_DEPENDS lib)
set(CPACK_COMPONENT_DEV_DEPENDS lib)

set(CPACK_COMPONENT_DAEMON_REQUIRED lib)
set(CPACK_COMPONENT_CLIENT-CLI_REQUIRED lib)
set(CPACK_COMPONENT_CLIENT-PYMACDETECT_REQUIRED lib)
set(CPACK_COMPONENT_DEV_REQUIRED lib)

set(CPACK_COMPONENT_DAEMON_DISPLAY_NAME "Daemon runtime")
set(CPACK_COMPONENT_CLIENT-CLI_DISPLAY_NAME "Command line interface client")
set(CPACK_COMPONENT_CLIENT-PYMACDETECT_DISPLAY_NAME "Python GUI client")
set(CPACK_COMPONENT_DEV_DISPLAY_NAME "C++ Headers")

set(CPACK_COMPONENT_DAEMON_GROUP "daemon")
set(CPACK_COMPONENT_CLIENT-CLI_GROUP "clients")
set(CPACK_COMPONENT_CLIENT-PYMACDETECT_GROUP "clients")
set(CPACK_COMPONENT_DEV_GROUP "development")
set(CPACK_COMPONENT_LIB_GROUP "libraries")

set(CPACK_COMPONENT_GROUP_DAEMON_DISPLAY_NAME "Daemon")
set(CPACK_COMPONENT_GROUP_CLIENTS_DISPLAY_NAME "Clients")
set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DISPLAY_NAME "Development")
set(CPACK_COMPONENT_GROUP_LIBRARIES_DISPLAY_NAME "Libraries")

set(CPACK_COMPONENT_GROUP_DAEMON_DISPLAY_DESCRIPTION "Daemon that monitors networks")
set(CPACK_COMPONENT_GROUP_CLIENTS_DISPLAY_DESCRIPTION "Clients that connect to a daemon")
set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DISPLAY_DESCRIPTION "Header files for the daemon and clients")
set(CPACK_COMPONENT_GROUP_LIBRARIES_DISPLAY_DESCRIPTION "Libraries that implement functionality used by the daemon and the clients")

set(CPACK_COMPONENT_DAEMON_INSTALL_TYPES daemon full)
set(CPACK_COMPONENT_CLIENT-CLI_INSTALL_TYPES clients full)
set(CPACK_COMPONENT_CLIENT-PYMACDETECT_INSTALL_TYPES clients full)
set(CPACK_COMPONENT_DEV_INSTALL_TYPES development full)
set(CPACK_COMPONENT_LIB_INSTALL_TYPES daemon clients development full)

set(CPACK_INSTALL_TYPE_DAEMON_DISPLAY_NAME "Daemon")
set(CPACK_INSTALL_TYPE_CLIENTS_DISPLAY_NAME "Clients")
set(CPACK_INSTALL_TYPE_DEVELOPMENT_DISPLAY_NAME "Development")
set(CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Full")

configure_file(scripts/repackage_deb.sh.in scripts/repackage_deb.sh @ONLY)
configure_file(scripts/postinst.in scripts/postinst @ONLY)
configure_file(scripts/prerm.in scripts/prerm @ONLY)

include(CPack)

FILE(WRITE "${CMAKE_BINARY_DIR}/copyright"
     "Copyright (C) 2010-2014 ${CPACK_DEBIAN_PACKAGE_MAINTAINER}

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
  
  On Debian systems, the complete text of the GNU General Public
  License can be found in `/usr/share/common-licenses/GPL-2'.")
install(FILES "${CMAKE_BINARY_DIR}/copyright"
        DESTINATION "share/doc/${CPACK_DEBIAN_PACKAGE_NAME}")

message(STATUS "Debian Package: ${CPACK_DEBIAN_PACKAGE_NAME} (${CPACK_DEBIAN_PACKAGE_VERSION}) [${CPACK_PACKAGE_FILE_NAME}.deb]")

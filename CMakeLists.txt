cmake_minimum_required(VERSION 2.6)
project(macdetect)

include(FindPkgConfig)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

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
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ) # 750

install(DIRECTORY "docs/_build/html"
  DESTINATION "share/macdetect/doc"
  PATTERN ".git" EXCLUDE
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ) # 750

## CPack
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MAC address based host detection framework")
set(CPACK_DEBIAN_PACKAGE_NAME "macdetect")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER  "Jan Winkler <jan.winkler.84@gmail.com>")
set(CPACK_PACKAGE_VENDOR "Jan Winkler <jan.winkler.84@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Host detection framework for monitoring networks
 Monitors any given number of networks for hosts (C++/Python).
 Contains a C++ daemon that monitors networks, and C++ and Python libraries
 plus a Python client program for reading events from the daemon.")
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "3")
SET(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}${CPACK_PACKAGE_REVISION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_PACKAGE_EXECUTABLES "macdetectd")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jan Winkler <jan.winkler.84@gmail.com>")
#set(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}_${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-release")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "python2.7, libc6 (>= 2.19-0ubuntu6)")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/pymacdetect/scripts/postinst;${CMAKE_SOURCE_DIR}/pymacdetect/scripts/prerm;")
set(CPACK_SET_DESTDIR "ON")
set(CPACK_SOURCE_STRIP_FILES "")
set(CPACK_STRIP_FILES "usr/bin/macdetectd;usr/bin/macdetect-client-cli")
#set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i686")

include(CPack)

FILE(WRITE "${CMAKE_BINARY_DIR}/copyright"
     "Copyright (C) 2010-2014 ${CPACK_DEBIAN_PACKAGE_MAINTAINER}

   This software may be licensed under the terms of the
   GNU Lesser General Public License Version 3 (the ``LGPL''),
   or (at your option) any later version.

   Software distributed under the License is distributed
   on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
   express or implied. See the LGPL for the specific language
   governing rights and limitations.

   You should have received a copy of the LGPL along with this
   program. If not, go to http://www.gnu.org/licenses/lgpl.html
   or write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

On Debian systems, the complete text of the GNU Lesser General Public
License can be found in `/usr/share/common-licenses/LGPL-3'.")
install(FILES "${CMAKE_BINARY_DIR}/copyright"
        DESTINATION "share/doc/${CPACK_DEBIAN_PACKAGE_NAME}")

message(STATUS "Debian Package: ${CPACK_DEBIAN_PACKAGE_NAME} (${CPACK_DEBIAN_PACKAGE_VERSION}) [${CPACK_PACKAGE_FILE_NAME}.deb]")
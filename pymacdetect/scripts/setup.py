## macdetect, a daemon and clients for detecting MAC addresses
## Copyright (C) 2015 Jan Winkler <jan.winkler.84@gmail.com>
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

## \author Jan Winkler


from distutils.core import setup, Extension


import sys
local_cmake_dir = sys.argv[len(sys.argv) - 1]
sys.argv = sys.argv[:-1]

pymacdetect_module = Extension('pymacdetect',
                               define_macros = [('MAJOR_VERSION', '0'),
                                                ('MINOR_VERSION', '1')],
                               include_dirs = ['/usr/local/include',
                                               '../../pymacdetect/include',
                                               '../../client/include',
                                               '../../utils/include'],
                               libraries = ['macdetect-client', 'macdetect-utils', 'gcov'],
                               library_dirs = ['/usr/local/lib',
                                               '../../lib'],
                               sources = [local_cmake_dir + '/src/PyMACDetect.cpp'],
                               extra_compile_args=['-std=c++11', '--coverage'])

setup(name = 'PyMACDetect',
      version = '1.0',
      description = 'Python language bindings for macdetect-client',
      author = 'Jan Winkler',
      author_email = 'jan.winkler.84@gmail.com',
      url = '',
      download_url = '',
      install_requires = ['nose'],
      packages = ['pymdGui', 'pymdLib'],
      package_dir = {'': 'scripts'},
      scripts = [],
      long_description = 'Language bindings for using the client class functionalities of macdetect-client.',
      ext_modules = [pymacdetect_module])

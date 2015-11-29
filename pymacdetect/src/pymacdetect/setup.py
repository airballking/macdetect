## Copyright 2016 Jan Winkler <jan.winkler.84@gmail.com>
## 
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
## 
## http:##www.apache.org/licenses/LICENSE-2.0
## 
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

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
                                               '../../macdetect-client/include',
                                               '../../macdetect-utils/include'],
                               libraries = ['macdetect-client', 'macdetect-utils'],
                               library_dirs = ['/usr/local/lib',
                                               '../../lib'],
                               sources = [local_cmake_dir + '/src/pymacdetect/PyMACDetect.cpp'],
                               extra_compile_args=['-std=c++11'])

setup(name = 'PyMACDetect',
      version = '1.0',
      description = 'Python language bindings for macdetect-client',
      author = 'Jan Winkler',
      author_email = 'jan.winkler.84@gmail.com',
      url = '',
      long_description = '''
Language bindings for using the client class functionalities of macdetect-client.
      ''',
      ext_modules = [pymacdetect_module])

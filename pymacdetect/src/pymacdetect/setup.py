from distutils.core import setup, Extension

pymacdetect_module = Extension('pymacdetect',
                               define_macros = [('MAJOR_VERSION', '1'),
                                                ('MINOR_VERSION', '0')],
                               include_dirs = ['/usr/local/include', '../../pymacdetect/include'],
                               libraries = [],
                               library_dirs = ['/usr/local/lib'],
                               sources = ['../../pymacdetect/src/pymacdetect/PyMACDetect.cpp'])

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

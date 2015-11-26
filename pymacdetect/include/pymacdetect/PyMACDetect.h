#ifndef __PYMACDETECT_H__
#define __PYMACDETECT_H__


// Python
#include <python2.7/Python.h>


namespace pymacdetect {
  static PyObject* macdetectClient(PyObject* pyoSelf, PyObject* pyoArgs);
  PyMODINIT_FUNC initMACDetect(void);
  
  static PyObject* pyoMACDetectError;
  static PyMethodDef PyMACDetectMethods[] = {
    {"macdetectClient",  macdetectClient, METH_VARARGS,
     "Create a MAC Detect client instance."},
    {NULL, NULL, 0, NULL}
  };
}


#endif /* __PYMACDETECT_H__ */

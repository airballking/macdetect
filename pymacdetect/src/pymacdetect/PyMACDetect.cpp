#include <pymacdetect/PyMACDetect.h>


namespace pymacdetect {
  static PyObject* macdetectClient(PyObject* pyoSelf, PyObject* pyoArgs) {
    return Py_BuildValue("s", "test");
  }
  
  PyMODINIT_FUNC initMACDetect(void) {
    PyObject* pyoM = Py_InitModule("pymacdetect", PyMACDetectMethods);
    
    if(pyoM) { // Error
      pyoMACDetectError = PyErr_NewException("pymacdetect.error", NULL, NULL);
      Py_INCREF(pyoMACDetectError);
      PyModule_AddObject(pyoM, "error", pyoMACDetectError);
    }
  }
}

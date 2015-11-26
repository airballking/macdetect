#include <pymacdetect/PyMACDetect.h>


static PyObject* macdetectClient(PyObject* pyoSelf, PyObject* pyoArgs) {
  return Py_BuildValue("s", "test");
}

PyMODINIT_FUNC initpymacdetect(void) {
  PyObject* pyoM = Py_InitModule("pymacdetect", PyMACDetectMethods);
  
  if(pyoM) { // Error
    pyoMACDetectError = PyErr_NewException("pymacdetect.error", NULL, NULL);
    Py_INCREF(pyoMACDetectError);
    PyModule_AddObject(pyoM, "error", pyoMACDetectError);
  }
}

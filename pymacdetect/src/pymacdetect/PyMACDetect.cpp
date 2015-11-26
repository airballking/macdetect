#include <pymacdetect/PyMACDetect.h>


static PyObject* createMDClient(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  
  int nOK = PyArg_ParseTuple(pyoArgs, "");
  if(nOK == 1) {
    macdetect_client::MDClient* mdcClient = new macdetect_client::MDClient();
    pyoResult = PyCObject_FromVoidPtr(mdcClient, NULL);
  }
  
  return pyoResult;
}

static PyObject* destroyMDClient(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  PyObject* pyoClient = NULL;
  
  int nOK = PyArg_ParseTuple(pyoArgs, "O", &pyoClient);
  
  if(nOK == 1) {
    macdetect_client::MDClient* mdcClient = (macdetect_client::MDClient*)PyCObject_AsVoidPtr(pyoClient);
    
    if(mdcClient) {
      delete mdcClient;
      
      Py_INCREF(Py_None);
      pyoResult = Py_None;
    }
  }
  
  return pyoResult;
}

PyMODINIT_FUNC initpymacdetect(void) {
  PyObject* pyoM = Py_InitModule("pymacdetect", PyMACDetectMethods);
  
  if(pyoM) { // Error
    pyoMACDetectError = PyErr_NewException("pymacdetect.error", NULL, NULL);
    Py_INCREF(pyoMACDetectError);
    PyModule_AddObject(pyoM, "error", pyoMACDetectError);
  }
}

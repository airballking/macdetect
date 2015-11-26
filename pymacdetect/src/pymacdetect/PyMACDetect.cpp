// Copyright 2016 Jan Winkler <jan.winkler.84@gmail.com>
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** \author Jan Winkler */


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

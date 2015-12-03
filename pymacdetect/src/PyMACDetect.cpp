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


static PyObject* valueToPyObject(std::shared_ptr<macdetect::Value> valValue) {
  PyObject* pyoResult = NULL;
  
  int nSubCount = valValue->subValues().size();
  pyoResult = PyDict_New();
  
  PyObject* pyoKey = Py_BuildValue("z#", valValue->content().c_str(), valValue->content().length());
  PyDict_SetItemString(pyoResult, valValue->key().c_str(), pyoKey);
  
  if(nSubCount > 0) {
    //PyObject* pyoChildren = Py_BuildValue("z#", "sub-values", 10);
    //PyDict_SetItemString(pyoChildren, valValue->key().c_str(), pyoKey);
    //PyObject* pyoSubValues = PyDict_New();
  }
  
  for(std::shared_ptr<macdetect::Value> valSubValue : valValue->subValues()) {
    
  }
  
  return pyoResult;
}


static macdetect_client::MDClient* clientFromPyArgs(PyObject* pyoArgs) {
  PyObject* pyoClient = NULL;
  macdetect_client::MDClient* mdcClient = NULL;
  int nOK = PyArg_ParseTuple(pyoArgs, "O", &pyoClient);
  
  if(nOK == 1) {
    mdcClient = (macdetect_client::MDClient*)PyCObject_AsVoidPtr(pyoClient);
  }
  
  return mdcClient;
}


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
  macdetect_client::MDClient* mdcClient = clientFromPyArgs(pyoArgs);
  
  if(mdcClient) {
    delete mdcClient;
    
    Py_INCREF(Py_None);
    pyoResult = Py_None;
  }
  
  return pyoResult;
}

static PyObject* connectMDClient(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  PyObject* pyoClient = NULL;
  char* carrIP = NULL;
  
  macdetect_client::MDClient* mdcClient = NULL;
  int nOK = PyArg_ParseTuple(pyoArgs, "Os", &pyoClient, &carrIP);
  
  if(nOK == 1) {
    mdcClient = (macdetect_client::MDClient*)PyCObject_AsVoidPtr(pyoClient);
    
    if(mdcClient) {
      std::string strIP(carrIP);
      
      if(mdcClient->connect(strIP)) {
	Py_INCREF(Py_True);
	pyoResult = Py_True;
      } else {
	Py_INCREF(Py_False);
	pyoResult = Py_False;
      }
    } else {
      Py_INCREF(Py_None);
      pyoResult = Py_None;
    }
  }
  
  return pyoResult;
}

static PyObject* disconnectMDClient(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  macdetect_client::MDClient* mdcClient = clientFromPyArgs(pyoArgs);
  
  if(mdcClient) {
    // Disconnect here.
    
    Py_INCREF(Py_True);
    pyoResult = Py_True;
  } else {
    Py_INCREF(Py_None);
    pyoResult = Py_None;
  }
  
  return pyoResult;
}

static PyObject* knownMACAddresses(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  macdetect_client::MDClient* mdcClient = clientFromPyArgs(pyoArgs);
  
  if(mdcClient) {
    std::list<std::string> lstAddresses = mdcClient->knownMACAddresses();
    pyoResult = PyList_New(lstAddresses.size());
    
    unsigned int unIndex = 0;
    for(std::list<std::string>::iterator itItem = lstAddresses.begin();
	itItem != lstAddresses.end(); itItem++) {
      std::string strItem = *itItem;
      
      PyObject* pyoItem = Py_BuildValue("z#", strItem.c_str(), strItem.length());
      PyList_SetItem(pyoResult, unIndex, pyoItem);
      
      unIndex++;
    }
  } else {
    Py_INCREF(Py_False);
    pyoResult = Py_False;
  }
  
  return pyoResult;
}

static PyObject* enableStream(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  PyObject* pyoClient = NULL;
  char* carrDeviceName = NULL;
  
  macdetect_client::MDClient* mdcClient = NULL;
  int nOK = PyArg_ParseTuple(pyoArgs, "Os", &pyoClient, &carrDeviceName);
  
  if(nOK == 1) {
    mdcClient = (macdetect_client::MDClient*)PyCObject_AsVoidPtr(pyoClient);
    
    if(mdcClient) {
      bool bSuccess = mdcClient->enableStream(std::string(carrDeviceName));
      
      if(bSuccess) {
	Py_INCREF(Py_True);
	pyoResult = Py_True;
      } else {
	Py_INCREF(Py_False);
	pyoResult = Py_False;
      }
    } else {
      Py_INCREF(Py_False);
      pyoResult = Py_False;
    }
  } else {
    Py_INCREF(Py_False);
    pyoResult = Py_False;
  }
  
  return pyoResult;
}

static PyObject* disableStream(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  PyObject* pyoClient = NULL;
  char* carrDeviceName = NULL;
  
  macdetect_client::MDClient* mdcClient = NULL;
  int nOK = PyArg_ParseTuple(pyoArgs, "Os", &pyoClient, &carrDeviceName);
  
  if(nOK == 1) {
    mdcClient = (macdetect_client::MDClient*)PyCObject_AsVoidPtr(pyoClient);
    
    if(mdcClient) {
      bool bSuccess = mdcClient->disableStream(std::string(carrDeviceName));
      
      if(bSuccess) {
	Py_INCREF(Py_True);
	pyoResult = Py_True;
      } else {
	Py_INCREF(Py_False);
	pyoResult = Py_False;
      }
    } else {
      Py_INCREF(Py_False);
      pyoResult = Py_False;
    }
  } else {
    Py_INCREF(Py_False);
    pyoResult = Py_False;
  }
  
  return pyoResult;
}

static PyObject* info(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  macdetect_client::MDClient* mdcClient = clientFromPyArgs(pyoArgs);
  
  if(mdcClient) {
    std::shared_ptr<macdetect::Value> valInfo = mdcClient->info();
    
    if(valInfo) {
      pyoResult = valueToPyObject(valInfo);
    } else {
      Py_INCREF(Py_None);
      pyoResult = Py_None;
    }
  } else {
    Py_INCREF(Py_None);
    pyoResult = Py_None;
  }
  
  return pyoResult;
}

static PyObject* devicesList(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  macdetect_client::MDClient* mdcClient = clientFromPyArgs(pyoArgs);
  
  if(mdcClient) {
    std::list< std::shared_ptr<macdetect::Value> > lstDevices = mdcClient->devicesList();
    pyoResult = PyList_New(lstDevices.size());
    
    unsigned int unIndex = 0;
    for(std::list< std::shared_ptr<macdetect::Value> >::iterator itItem = lstDevices.begin();
	itItem != lstDevices.end(); itItem++) {
      std::shared_ptr<macdetect::Value> valItem = *itItem;
      
      /*PyObject* pyoItem = Py_BuildValue("z#", strItem.c_str(), strItem.length());
	PyList_SetItem(pyoResult, unIndex, pyoItem);*/
      
      unIndex++;
    }
  } else {
    Py_INCREF(Py_False);
    pyoResult = Py_False;
  }
  
  return pyoResult;
}

PyMODINIT_FUNC initpymacdetect(void) {
  PyObject* pyoM = Py_InitModule("pymacdetect", PyMACDetectMethods);
  
  if(pyoM) { // Error
    char cError[] = "pymacdetect.error";
    pyoMACDetectError = PyErr_NewException(cError, NULL, NULL);
    Py_INCREF(pyoMACDetectError);
    PyModule_AddObject(pyoM, "error", pyoMACDetectError);
  }
}

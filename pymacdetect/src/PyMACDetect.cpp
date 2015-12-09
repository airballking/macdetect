// macdetect, a daemon and clients for detecting MAC addresses
// Copyright (C) 2015 Jan Winkler <jan.winkler.84@gmail.com>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

/** \author Jan Winkler */


#include <pymacdetect/PyMACDetect.h>


static PyObject* valueToPyObject(std::shared_ptr<macdetect::Value> valValue) {
  PyObject* pyoResult = PyDict_New();
  PyObject* pyoContent = PyDict_New();
  
  PyObject* pyoContentString = Py_BuildValue("z#", valValue->content().c_str(), valValue->content().length());
  PyDict_SetItemString(pyoContent, "content", pyoContentString);
  
  int nSubCount = valValue->subValues().size();
  if(nSubCount > 0) {
    PyObject* pyoSubs = PyDict_New();
    
    for(std::shared_ptr<macdetect::Value> valSub : valValue->subValues()) {
      PyObject* pyoSub = valueToPyObject(valSub);
      
      PyDict_SetItemString(pyoSubs, valSub->key().c_str(), PyDict_GetItemString(pyoSub, valSub->key().c_str()));
    }
    
    PyDict_SetItemString(pyoContent, "subs", pyoSubs);
  }
  
  PyDict_SetItemString(pyoResult, valValue->key().c_str(), pyoContent);
  
  return pyoResult;
}


static std::shared_ptr<macdetect::Value> pyObjectToValue(PyObject* pyoObject) {
  std::shared_ptr<macdetect::Value> valResult = std::make_shared<macdetect::Value>();
  PyObject* pyoKeys = PyDict_Keys(pyoObject);
  PyObject* pyoKey = PyList_GetItem(pyoKeys, 0);
  
  std::string strKey = PyString_AsString(pyoKey);
  valResult->setKey(strKey);
  PyObject* pyoContentDict = PyDict_GetItemString(pyoObject, strKey.c_str());
  valResult->setContent(PyString_AsString(PyDict_GetItemString(pyoContentDict, "content")));
  
  PyObject* pyoSubs = PyDict_GetItemString(pyoContentDict, "subs");
  
  if(pyoSubs) {
    PyObject* pyoSubKeys = PyDict_Keys(pyoSubs);
    int nSize = PyList_Size(pyoSubKeys);
    
    for(int nI = 0; nI < nSize; nI++) {
      PyObject* pyoSubKey = PyList_GetItem(pyoSubKeys, 0);
      std::string strSubKey = PyString_AsString(pyoSubKey);
      
      PyObject* pyoSubDict = PyDict_New();
      PyDict_SetItemString(pyoSubDict, strSubKey.c_str(), PyDict_GetItemString(pyoSubs, strSubKey.c_str()));
      
      valResult->add(pyObjectToValue(pyoSubDict));
    }
  }
  
  return valResult;
}


static macdetect_client::MDClient* clientFromPyArgs(PyObject* pyoArgs) {
  PyObject* pyoClient = NULL;
  macdetect_client::MDClient* mdcClient = NULL;
  
  std::string strParameterFormat = "O";
  int nOK = PyArg_ParseTuple(pyoArgs, strParameterFormat.c_str(), &pyoClient);
  
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


static PyObject* send(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  PyObject* pyoClient = NULL;
  PyObject* pyoMessage = NULL;
  
  macdetect_client::MDClient* mdcClient = NULL;
  int nOK = PyArg_ParseTuple(pyoArgs, "OO", &pyoClient, &pyoMessage);
  
  if(nOK == 1) {
    mdcClient = (macdetect_client::MDClient*)PyCObject_AsVoidPtr(pyoClient);
    
    if(mdcClient) {
      std::shared_ptr<macdetect::Value> valSend = pyObjectToValue(pyoMessage);
      
      if(valSend) {
	pyoResult = valueToPyObject(mdcClient->requestResponse(valSend));
      } else {
	Py_INCREF(Py_None);
	pyoResult = Py_None;
      }
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


static PyObject* receive(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  macdetect_client::MDClient* mdcClient = clientFromPyArgs(pyoArgs);
  
  if(mdcClient) {
    std::shared_ptr<macdetect::Value> valReceived = mdcClient->receive();
    
    if(valReceived) {
      pyoResult = valueToPyObject(valReceived);
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


PyMODINIT_FUNC initpymacdetect(void) {
  PyObject* pyoM = Py_InitModule("pymacdetect", PyMACDetectMethods);
  
  if(pyoM) { // Error
    char cError[] = "pymacdetect.error";
    pyoMACDetectError = PyErr_NewException(cError, NULL, NULL);
    Py_INCREF(pyoMACDetectError);
    PyModule_AddObject(pyoM, "error", pyoMACDetectError);
  }
}

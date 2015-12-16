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


extern PyObject* g_pyoModule;
extern std::map<std::string, PyObject*> g_mapErrors;


void addException(std::string strName) {
  g_mapErrors[strName] = PyErr_NewException((char*)("pymacdetect." + strName).c_str(), NULL, NULL);
  PyModule_AddObject(g_pyoModule, strName.c_str(), g_mapErrors[strName]);
}


PyObject* exception(std::string strName) {
  return g_mapErrors[strName];
}


static PyObject* argsInvalidException() {
  PyObject* pyoException = exception("ArgumentsInvalidError");
  Py_INCREF(pyoException);
  
  PyErr_SetString(pyoException, "Arguments invalid");
  
  return pyoException;
}


static PyObject* disconnectedException() {
  PyObject* pyoException = exception("DisconnectedError");
  Py_INCREF(pyoException);
  
  PyErr_SetString(pyoException, "Client disconnected");
  
  return pyoException;
}


static PyObject* valueInvalidException() {
  PyObject* pyoException = exception("ValueInvalidError");
  Py_INCREF(pyoException);
  
  PyErr_SetString(pyoException, "Value invalid");
  
  return pyoException;
}


static PyObject* mdcInvalidException() {
  PyObject* pyoException = exception("ClientInvalidError");
  Py_INCREF(pyoException);
  
  PyErr_SetString(pyoException, "Client instance invalid");
  
  return pyoException;
}


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
  } else {
    pyoResult = NULL;
    g_pyoException = argsInvalidException();
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
  } else {
    pyoResult = NULL;
    g_pyoException = mdcInvalidException();
  }
  
  return pyoResult;
}


static PyObject* enableStream(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  PyObject* pyoClient = NULL;
  char* carrDevice = NULL;
  
  macdetect_client::MDClient* mdcClient = NULL;
  int nOK = PyArg_ParseTuple(pyoArgs, "Os", &pyoClient, &carrDevice);
  
  if(nOK == 1) {
    mdcClient = (macdetect_client::MDClient*)PyCObject_AsVoidPtr(pyoClient);
    
    if(mdcClient) {
      std::string strDevice(carrDevice);
      
      if(mdcClient->requestEnableStream(strDevice)) {
	Py_INCREF(Py_True);
	pyoResult = Py_True;
      } else {
	Py_INCREF(Py_False);
	pyoResult = Py_False;
      }
    } else {
      pyoResult = NULL;
      g_pyoException = mdcInvalidException();
    }
  } else {
    pyoResult = NULL;
    g_pyoException = argsInvalidException();
  }
  
  return pyoResult;
}


static PyObject* disableStream(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  PyObject* pyoClient = NULL;
  char* carrDevice = NULL;
  
  macdetect_client::MDClient* mdcClient = NULL;
  int nOK = PyArg_ParseTuple(pyoArgs, "Os", &pyoClient, &carrDevice);
  
  if(nOK == 1) {
    mdcClient = (macdetect_client::MDClient*)PyCObject_AsVoidPtr(pyoClient);
    
    if(mdcClient) {
      std::string strDevice(carrDevice);
      
      if(mdcClient->requestDisableStream(strDevice)) {
	Py_INCREF(Py_True);
	pyoResult = Py_True;
      } else {
	Py_INCREF(Py_False);
	pyoResult = Py_False;
      }
    } else {
      pyoResult = NULL;
      g_pyoException = mdcInvalidException();
    }
  } else {
    pyoResult = NULL;
    g_pyoException = argsInvalidException();
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
      pyoResult = NULL;
      g_pyoException = mdcInvalidException();
    }
  } else {
    pyoResult = NULL;
    g_pyoException = argsInvalidException();
  }
  
  return pyoResult;
}


static PyObject* mdClientConnected(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  macdetect_client::MDClient* mdcClient = clientFromPyArgs(pyoArgs);
  
  if(mdcClient) {
    if(mdcClient->connected()) {
      Py_INCREF(Py_True);
      pyoResult = Py_True;
    } else {
      Py_INCREF(Py_False);
      pyoResult = Py_False;
    }
  } else {
    pyoResult = NULL;
    g_pyoException = mdcInvalidException();
  }
  
  return pyoResult;
}


static PyObject* disconnectMDClient(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  macdetect_client::MDClient* mdcClient = clientFromPyArgs(pyoArgs);
  
  if(mdcClient) {
    if(mdcClient->disconnect()) {
      Py_INCREF(Py_True);
      pyoResult = Py_True;
    } else {
      Py_INCREF(Py_False);
      pyoResult = Py_False;
    }
  } else {
    pyoResult = NULL;
    g_pyoException = mdcInvalidException();
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
	if(mdcClient->send(valSend)) {
	  pyoResult = Py_True;
	  Py_INCREF(Py_True);
	} else {
	  pyoResult = Py_False;
	  Py_INCREF(Py_False);
	}
      } else {
	pyoResult = valueInvalidException();
      }
    } else {
      pyoResult = NULL;
      g_pyoException = mdcInvalidException();
    }
  } else {
    pyoResult = NULL;
    g_pyoException = argsInvalidException();
  }
  
  return pyoResult;
}


static PyObject* receive(PyObject* pyoSelf, PyObject* pyoArgs) {
  PyObject* pyoResult = NULL;
  macdetect_client::MDClient* mdcClient = clientFromPyArgs(pyoArgs);
  
  if(mdcClient) {
    bool bDisconnected;
    std::shared_ptr<macdetect::Value> valReceived = mdcClient->receive(bDisconnected);
    
    if(bDisconnected) {
      pyoResult = NULL;
      g_pyoException = disconnectedException();
    } else {
      if(valReceived) {
	pyoResult = valueToPyObject(valReceived);
      } else {
	Py_INCREF(Py_None);
	pyoResult = Py_None;
      }
    }
  } else {
    pyoResult = NULL;
    g_pyoException = mdcInvalidException();
  }
  
  return pyoResult;
}


PyMODINIT_FUNC initpymacdetect(void) {
  g_pyoModule = Py_InitModule("pymacdetect", PyMACDetectMethods);
  
  if(g_pyoModule) {
    addException("ArgumentsInvalidError");
    addException("DisconnectedError");
    addException("ValueInvalidError");
    addException("ClientInvalidError");
  }
}

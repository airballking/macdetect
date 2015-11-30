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


#ifndef __PYMACDETECT_H__
#define __PYMACDETECT_H__


// System
#include <list>
#include <string>

// Python
#include <python2.7/Python.h>

// MAC detect
#include <macdetect-client/MDClient.h>


static macdetect_client::MDClient* clientFromPyArgs(PyObject* pyoArgs);

static PyObject* createMDClient(PyObject* pyoSelf, PyObject* pyoArgs);
static PyObject* destroyMDClient(PyObject* pyoSelf, PyObject* pyoArgs);

static PyObject* connectMDClient(PyObject* pyoSelf, PyObject* pyoArgs);
static PyObject* disconnectMDClient(PyObject* pyoSelf, PyObject* pyoArgs);

static PyObject* devicesList(PyObject* pyoSelf, PyObject* pyoArgs);
static PyObject* knownMACAddresses(PyObject* pyoSelf, PyObject* pyoArgs);

PyMODINIT_FUNC initpymacdetect(void);

static PyObject* pyoMACDetectError;

static PyMethodDef PyMACDetectMethods[] = {
  {"createClient", createMDClient, METH_VARARGS,
   "Create a MAC Detect client instance."},
  {"destroyClient", destroyMDClient, METH_VARARGS,
   "Destroy a MAC Detect client instance."},
  {"connectClient", connectMDClient, METH_VARARGS,
   "Connects a MAC Detect client instance to a target IP."},
  {"disconnectClient", disconnectMDClient, METH_VARARGS,
   "Disconnects a MAC Detect client instance from its current server."},
  {"knownMACAddresses", knownMACAddresses, METH_VARARGS,
   "Retrieves all known MAC addresses from a connected server."},
  {"devicesList", devicesList, METH_VARARGS,
   "Retrieves all list of all actively monitored network devices on the server."},
  {NULL, NULL, 0, NULL}
};


#endif /* __PYMACDETECT_H__ */

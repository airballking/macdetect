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

static PyObject* info(PyObject* pyoSelf, PyObject* pyoArgs);

static PyObject* enableStream(PyObject* pyoSelf, PyObject* pyoArgs);
static PyObject* disableStream(PyObject* pyoSelf, PyObject* pyoArgs);

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
  {"info", info, METH_VARARGS,
   "Retrieves all info packages currently streamed from the server."},
  {"enableStream", enableStream, METH_VARARGS,
   "Enables the info event stream for the given device name on the server."},
  {"disableStream", disableStream, METH_VARARGS,
   "Disables the info event stream for the given device name on the server."},
  {NULL, NULL, 0, NULL}
};


#endif /* __PYMACDETECT_H__ */

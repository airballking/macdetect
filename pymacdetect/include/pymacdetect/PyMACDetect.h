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


static PyObject* g_pyoException;
static PyObject* g_pyoModule;


static macdetect_client::MDClient* clientFromPyArgs(PyObject* pyoArgs);

static PyObject* createMDClient(PyObject* pyoSelf, PyObject* pyoArgs);
static PyObject* destroyMDClient(PyObject* pyoSelf, PyObject* pyoArgs);

static PyObject* connectMDClient(PyObject* pyoSelf, PyObject* pyoArgs);
static PyObject* disconnectMDClient(PyObject* pyoSelf, PyObject* pyoArgs);

static PyObject* receive(PyObject* pyoSelf, PyObject* pyoArgs);
static PyObject* send(PyObject* pyoSelf, PyObject* pyoArgs);

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
  {"receive", receive, METH_VARARGS,
   "Retrieves the next newest packet received from the server."},
  {"send", send, METH_VARARGS,
   "Sends a packet to the server and requests an immediate response."},
  {NULL, NULL, 0, NULL}
};


#endif /* __PYMACDETECT_H__ */

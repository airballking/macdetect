#ifndef __PYMACDETECT_H__
#define __PYMACDETECT_H__


// Python
#include <python2.7/Python.h>

// MAC detect
#include <macdetect-client/MDClient.h>


static PyObject* createMDClient(PyObject* pyoSelf, PyObject* pyoArgs);
static PyObject* destroyMDClient(PyObject* pyoSelf, PyObject* pyoArgs);

PyMODINIT_FUNC initpymacdetect(void);

static PyObject* pyoMACDetectError;
static PyMethodDef PyMACDetectMethods[] = {
  {"createClient", createMDClient, METH_VARARGS,
   "Create a MAC Detect client instance."},
  {"destroyClient", destroyMDClient, METH_VARARGS,
   "Destroy a MAC Detect client instance."},
  {NULL, NULL, 0, NULL}
};


#endif /* __PYMACDETECT_H__ */

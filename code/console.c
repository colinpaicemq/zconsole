//
// Program to provide console support to python running as a started task
//
//MIT License
//
//Copyright (c) 2022 Colin Paice
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//
#define _OPEN_THREADS 2
#include <sys/__messag.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "stddef.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "pyport.h"
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "cib.h"
#include "printHex.h"
#define Py23Text_FromString PyUnicode_FromString  // converts C char* to Py3 str

//===========================================================================
//  This program is the main program for providing console support for Python
//===========================================================================

#pragma linkage(CPWTO   , OS)
#pragma linkage(QEDIT   , OS)
#pragma linkage(QEDITW  , OS)
static PyObject *ErrorObj;
#define CEYE "CEYE"
// d printHex(FILE * pFile,void *buff, long length);
static char __version__[] = "0.01.0";  //  Initial

static PyObject *ErrorObj;

static char console_doc[] =
  "z Console interface for Python. put and callback";
#include <consoleacb.h>
#include <consoleput.h>
#include <consoleconsole2.h>
#include <taskinfo.h>
static struct PyMethodDef console_methods[] = {
    {"cancel", (PyCFunction)console_cancel,METH_KEYWORDS | METH_VARARGS, console_cancel_doc},
    {"acb", (PyCFunction)console_acb,METH_KEYWORDS | METH_VARARGS, console_acb_doc},
#ifdef no
    {"kill", (PyCFunction)console_kill,METH_KEYWORDS | METH_VARARGS, console_kill_doc},
    {"acancel", (PyCFunction)console_acancel,METH_KEYWORDS | METH_VARARGS, console_acancel_doc},
    {"init", (PyCFunction)console_init,METH_KEYWORDS | METH_VARARGS, console_init_doc},
    {"get", (PyCFunction)console_get,METH_KEYWORDS | METH_VARARGS, console_get_doc},
#endif
    {"console2", (PyCFunction)console_console2,METH_KEYWORDS | METH_VARARGS, console_put_doc},
    {"taskinfo", (PyCFunction)console_taskinfo,METH_KEYWORDS | METH_VARARGS, "task info, ASCB,TCB etc" },
    {"put", (PyCFunction)console_put,METH_KEYWORDS | METH_VARARGS, console_put_doc},
    {NULL, (PyCFunction)NULL, 0, NULL}        /* sentinel */
    };
  // -----------------------------------------
  // -----------------------------------------
static struct PyModuleDef console_module = {
      PyModuleDef_HEAD_INIT,
      "console",
      console_doc,
      -1,
      console_methods
  };
PyMODINIT_FUNC PyInit_zconsole(void) {
  PyObject *m, *d;

  /* Create the module and add the functions */
  m = PyModule_Create(&console_module);

  /* Add some symbolic constants to the module */
  d = PyModule_GetDict(m);
//ErrorObj = PyErr_NewException("console.error", NULL, NULL);
//PyDict_SetItemString(d, "console.error", ErrorObj);

  PyDict_SetItemString(d, "__doc__", Py23Text_FromString(console_doc));
  PyDict_SetItemString(d,"__version__", Py23Text_FromString(__version__));

return m;
}

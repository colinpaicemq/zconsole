//  console_get
//
// calls assembler program which uses QEDIT interface to retrieve
// the parameters started task was started with, and any through operator
// modify interface ( f pythstc,'parms')
// and the stop interface
//////////////////////////////////////////////////////////////////////////
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
//================================================================

static char console_init_doc[] =
  "Internal  code to get a handle " ;

static PyObject *console_init(PyObject *self, PyObject *args, PyObject *keywds ) {
  PyObject *rv = NULL;  // returned object
  // we need some 31 bit storage for the assembler routines
  char * __ptr32 pMsg;
  pMsg =  (char *) __malloc31(1024);
  printf("Console compiled %s %s.\n", __DATE__, __TIME__);
  // set the eye catcher and the kill ecb
  memcpy(pMsg,CEYE,4);
  CIB * pCIB = (CIB *) pMsg;
  pCIB -> killecb = 0;  // preset this
  int handle;
  handle = (int) pMsg;
  rv = Py_BuildValue("l", handle);
  return rv;
}
/////////////////////////////////////////////////////////////////////////////////
//  KILL
////////////////////////////////////////////////////////////////////////////////
static char console_kill_doc[] =
  "Kill the task waiting for the operator kill()  " ;

static PyObject *console_kill(PyObject *self, PyObject *args, PyObject *keywds ) {
  PyObject *rv = NULL;  // returned object
  char * pMsg;
  CIB * pCIB;
  int handle;
  if (!PyArg_ParseTuple(args,"i",
                                    &handle))// message text
  {
      return NULL;
  }
  if ( handle == 0)
  {
     PyErr_Format(ErrorObj, "Bad handle passed in.  It is zero");
       return NULL;
  }
  pMsg = (char * ) handle;
  pCIB = (CIB *) handle;
  if (memcmp(pMsg,CEYE,4) != 0 ) // match
  {
     PyErr_Format(ErrorObj, "Kill Bad handle passed in no longer valid");
     return NULL;
  }
  char * p;
  // in ECB top is wait bit, second is post, remainder is the code
  asm(
  "  LGF  2,%[p1] Address of the block \n"
  "  POST 4(2),7 \n"
  :
  : [p1] "m"(handle)
  : "r0","r1"
  );
  rv = Py_BuildValue("l", 0     );
  return rv;
}

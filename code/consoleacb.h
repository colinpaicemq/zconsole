//  console_acb
//  console_cancel
//
// sets up async thread to do QEDIT work.
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

static char console_acb_doc[] =
  "Asynchronous acb code invoking a callback to a Python function\n "
  "format h= zconsole.acb(afunc,[p1,p2...]\n"
  "where afunc is a 'def afunc(args,qedit):...'\n"
  ;
 #include <pthread.h>
 // mapping of the data passed to the thread
 struct thread_args {
       char a[8];
       PyObject *method;
       PyObject *a1    ;
       char * __ptr32 pMsg;
       char rest[0]; // The work area and CIB etc
        }   ;
// the async routine
#include <cthread.h>
//
// Asynchronous Call Back
// returns the handle
//
static PyObject *console_acb(PyObject *self, PyObject *args) {
//atic PyObject *console_acb(PyObject *self, PyObject *args, PyObject *keywds ) {
  /*----------------------------------------------------------------*/
  /* Attach the task to respond to modify request                   */
  /*----------------------------------------------------------------*/
  //printf("Console acb compiled %s %s.\n", __DATE__, __TIME__);

  pthread_t thid;
  PyObject * method;
  PyObject * a1 = NULL;
  PyObject * a2 = NULL; // unused
  if (!PyArg_ParseTuple(args,"OO",
                             &method, // function
                             &a1    ))// parms
  {
      PyErr_SetString(PyExc_RuntimeError,"Problems parsing parameters");
      return NULL;
  }
  if (method == NULL)
    {
//     printf("METHOD IS NULL\n");
       PyErr_SetString(PyExc_RuntimeError,"Method is missing or Null");
       return NULL;
    }
  if (a1  == NULL)
    {
//     printf("A1     IS NULL\n");
       PyErr_SetString(PyExc_RuntimeError,"Required parameter is  missing is missing");
       return NULL;
    }
  struct thread_args *zargs = __malloc31(1024);  // for work area etc
  // clear the header
  memset(zargs,0,sizeof(struct thread_args));
  memcpy(&zargs -> a[0],"01234567",8); // used to check parameters are OK
  zargs -> method = method;
  zargs -> a1     = args  ;

  // Point to the CIB etc in the thread block
  char * __ptr32 pMsg;
  pMsg =  (char *)&zargs -> rest[0];
  memcpy(pMsg,CEYE,4);
  CIB * pCIB;     // returned Console Information Block
  CIBX  * pCIBX;  // returned Console Information Block eXtension
  int handle = (int) pMsg;
  if  ( offsetof(CIB, nextCIB)!= 72 )
  {
       PyErr_SetString(PyExc_RuntimeError,"Internal error  CIB offset not 72 ");
       return NULL;
  }

  // pass the address of the storage through to the thread.
  zargs -> pMsg = pMsg;
  //  prevent these from being gc.  The are py_XDECREF in thread
  Py_XINCREF(zargs -> a1);    /* Prevent it    being deallocated. */
  Py_XINCREF(zargs -> method);/* Prevent it    being deallocated. */
  int rc;

  // invoke pThread to create thread and pass the parms through
  rc = pthread_create(&thid, NULL, cthread, zargs);
  if (rc != 0) {
    printf("pthread rc %d \n", rc);
    perror("pthread_create() error");
  }
  PyObject * rv    ;  // return value
  rv = Py_BuildValue("l)",
                      handle);
  return rv;
}
/////////////////////////////////////////////////////////////////////////////////
//  CANCEL()
////////////////////////////////////////////////////////////////////////////////
static char console_cancel_doc[] =
  "Asynchronous cancel the zconsole task running in the background\n "
  ;

// This routine is passed the handle.. the address of the block
static PyObject *console_cancel(PyObject *self, PyObject *args, PyObject *keywds ) {
  PyObject *rv = NULL;  // returned object
  char * pMsg;
//CIB * pCIB;
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
//pCIB = (CIB *) handle;
  if (memcmp(pMsg,CEYE,4) != 0 ) // match
  {
     PyErr_Format(ErrorObj, "Kill Bad handle passed in no longer valid");
     return NULL;
  }
//char * p;
  // Post the ECB
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

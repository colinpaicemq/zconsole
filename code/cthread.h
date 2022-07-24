//  cthread.h
//  Attached thread for processing QEDIT requests
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
////////////////////////////////////////////////////
//  See https://docs.python.org/3/c-api/init.html
//  See call object                  https://docs.python.org/3/c-api/call.html#c.PyObject_CallFunctionObjArgs
//  https://docs.python.org/3/c-api/call.html#c.PyObject_CallFunctionObjArgs
//  https://docs.python.org/3/c-api/init.html
//  https://docs.python.org/3/c-api/exceptions.html
//
// https://pythonextensionpatterns.readthedocs.io/en/latest/exceptions.html
// https://python.readthedocs.io/en/v2.7.2/extending/extending.html
PyGILState_STATE gstate;
//   Have a clean up exit in case thread is cancelled
//   If the GIL lock is held release it
void cleanup(void * arg)
{
  //  do something to say task ended
  //  printf("Thread was cancelled||||\n");
  int s = PyGILState_Check();
  //  release the lock if we have
  if (s)
    PyGILState_Release(gstate);
}
void * cthread(void *_arg) {
  struct thread_args * tA  = (struct thread_args *) _arg ;
  // activate the clean up exit
  pthread_cleanup_push(cleanup,NULL);

  PyObject *rv = NULL;  // returned object
//PyObject *x  = NULL;  // returned object
  char * ret  = 0;
  long  rc;
  int stop = 0;
  rc = 0;
  int ct;
  int thstatus;
  //  This section is not needed because it is the same as the defaults
  #ifdef no
//  //#define PTHREAD_INTR_CONTROLLED    0
//  //#define PTHREAD_INTR_ASYNCHRONOUS  1
//  // default is 0
//  //  The following are not needed because they default
//  int oldstatus;
//  oldstatus =  pthread_setintrtype(PTHREAD_INTR_CONTROLLED ) ;
//  if (oldstatus  == -1 ) {
//    printf("set controlled failed. %s\n", strerror(errno));
//    thstatus = 103;
//  pthread_exit(&thstatus);
//  }
//  printf(" pthread_setintrtype %d \n",oldstatus);
//  oldstatus = pthread_setintr(PTHREAD_INTR_ENABLE);
//  if ( oldstatus == -1 ) {
//    printf("set enable failed. %s\n", strerror(errno));
//    thstatus = 104;
//    pthread_exit(&thstatus);
//  }
  //printf(" pthread_setintr %d \n",oldstatus);
  #endif
  gstate = PyGILState_Ensure();

  int loop;
  for ( loop = 0;        ;loop ++)
  {
    CIB * pCIB;     // returned Console Information Block
    CIBX  * pCIBX;  // returned Console Information Block eXtension
    char * __ptr32 pMsg = tA   -> pMsg;
    pCIB = (CIB *) pMsg;
    int handle = (int) pMsg;
    // preset these every time.
    // go do the work.   This routine waits until the operator issues
    // a command, either Start(S), Modify(F), or Stop(p)

    // let other work run - release the GIL
    Py_BEGIN_ALLOW_THREADS
    rc = QEDIT( pMsg);
    // do not let other work run - get the GIL
    Py_END_ALLOW_THREADS
    // if kill ecb was posted then returned code 8
    if ( rc == 0 )
    {
      //  We woke because of CIB response
      pCIBX =(CIBX * )( (char *)(  pCIB)
                                   +offsetof(CIB, nextCIB)  // get to the CIB
                                   +(pCIB-> cobxoff) //  and then get offset of the CIBX
                      );
      // now the verb type
      char * pVerb = "UNKNOWN";
      switch (pCIB-> verb)
      {
         case 0x04: pVerb = "Start";
            break;
         case 0x44: pVerb = "Modify";
            break;
         case 0x40: pVerb = "Stop";
                    stop = 1;
            break;
      }
      // length of the string passed through
      Py_ssize_t  lData   = pCIB -> lData ;
      Py_ssize_t  l8      = 8;
      // if we have some data, convert it EBCDIC to ASCII
      if ( lData > 0)
        __e2a_l(&(pCIB -> data[0]),lData);

      // always do this
      __e2a_l(  pCIBX ->consolename ,8 );

      rv = Py_BuildValue("{s:i,s:s,s:s#,s:s#,s:y#,s:y#,s:y#}",
                "rc", rc,
                "verb", pVerb,
                "data",&(pCIB -> data[0]),lData,
                "console",&(pCIBX -> consolename),l8,
                "cart",&(pCIBX -> CART),l8,
                "consoleid",&(pCIBX -> consoleid),l8,
                "oconsoleid",&(pCIBX -> consoleid),l8);
       if (rv == NULL )
       {
        PyErr_Print();
        PyErr_SetString(PyExc_RuntimeError,"Py_BuildValue");
        PyGILState_Release(gstate);
        return NULL;
           printf(" Py_BuildValue error \n");
           return NULL;
       }
       Py_XINCREF(rv      );    /* Prevent pLast being deallocated. */
    }
    else  // we had kill
      stop = 1;

    if (rc != 0 ) // killed
       break;

    //////////////////////////////
    // Now call the Python function
    //////////////////////////////
    rv= PyObject_CallFunctionObjArgs( tA -> method,tA -> a1,rv      , NULL);

    if (rv== NULL)
    {
       PyErr_PrintEx(0);
       PyErr_Print();
       PyErr_SetString(PyExc_RuntimeError,"PyObject_CallFunctionObjArgs problems");
       PyGILState_Release(gstate);
       return NULL;
    }
    Py_XDECREF(rv      );    /* Prevent pLast being deallocated. */
    if (stop >0)
       {
          //printf("Stop found - cthread exiting \n");
         break;
       }
    Py_BEGIN_ALLOW_THREADS
    //  If an interupt occurs it will occur here.
    pthread_testintr()  ;
    Py_END_ALLOW_THREADS
  }  // end of long loop
  // clean up
  Py_XDECREF(tA -> a1);    /* Prevent pLast being deallocated. */
  Py_XDECREF(tA -> method);    /* Prevent pLast being deallocated. */

  PyErr_PrintEx(0);
  PyErr_Print();
  PyGILState_Release(gstate);

  pthread_cleanup_pop(0);
  pthread_exit(ret);
  return 0;
}

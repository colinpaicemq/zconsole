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

static char console_get_doc[] =
  "rc = console.get(handle)" ;

static PyObject *console_get(PyObject *self, PyObject *args, PyObject *keywds ) {
  PyObject *rv = NULL;  // returned object
  char * pBuffer;
  Py_ssize_t lMsg = 0;
  long  rc;
  rc = 0;
  // we need some 31 bit storage for the assembler routines
  char * __ptr32 pMsg;
//pMsg =  (char *) __malloc31(1024);
  char * p  = "";
  CIB * pCIB;     // returned Console Information Block
  CIBX  * pCIBX;  // returned Console Information Block eXtension
  int handle;

  // "handle" passed into us - the address of the storage
  if (!PyArg_ParseTuple(args,"i"       ,
                                    &handle)) //
  {
      return NULL;
  }

  pMsg = (char *) handle;
  if (memcmp(pMsg,CEYE,4) != 0 ) // does not match
  {
     PyErr_Format(ErrorObj, "Get:Bad handle passed in bad eye catcher");
     return NULL;
  }
  pCIB = (CIB *) pMsg;
  // preset these every time.
  // go do the work.   This routine waits until the operator issues
  // a command, either Start(S), Modify(F), or Stop(p)
  Py_BEGIN_ALLOW_THREADS
  rc = QEDIT( pMsg);
  Py_END_ALLOW_THREADS
  // if the kill EB was posted
  if ( rc != 0 )
     {
       free ( pMsg);
       rv = Py_BuildValue("{s:i}",
            "rc", rc);
       return rv  ;
     }
  //  We woke because of CIB response
  pCIBX =(CIBX * )( (char *)(  pCIB)  +(pCIB-> cobxoff) +sizeof(pCIB-> workarea) + 8);
  // now the verb type
  char * pVerb = "UNKNOWN";
  switch (pCIB-> verb)
  {
     case 0x04: pVerb = "Start";
        break;
     case 0x44: pVerb = "Modify";
        break;
     case 0x40: pVerb = "Stop";
        break;
  }
  // length of the string passed through
  Py_ssize_t  lData   = pCIB -> lData ;
  // create variable used for the fixed length variables
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
  if (rv == NULL)
  printf("GET 100 NULL\n");
  return rv;
}

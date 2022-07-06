//
//  console_put: do a WTO for a python program
//////////////////////////////////////////////
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

static char console_put_doc[] =
   "console.put('msg' )                     " ;

static PyObject *console_put(PyObject *self, PyObject *args, PyObject *keywds ) {
  PyObject *rv = NULL;
  struct __cons_msg2 cmsg;
  long  rc;
  rc = 0;

  int cmsg_cmd;
  // The wtoparm data is the data produced from an assembler WTO
  // This is used in an SVC 35 to actually do he WTO
  int wtoparm[] ={
                0x00080010,
   /* 04 */     0x00000000, // MESSAGE TEXT ADDRESS
                0x02000068,
                0x00800000,
                0x00000000,
                0x00000000,
                0x00000000,
                0x00000000,
                0x00000000,   0x00000000,
                0x00000000,   0x00000000,
                0x00000000,
                0x40404040,   0x40404040, // jobid
                0x40404040,   0x40404040, // jobname
                0x40404040,   0x40404040,      // ret key
                0x00000000,   // token
                0x00000000,   // consoleid
                0x40404040,   0x40404040, // systemname
                0x40404040,   0x40404040, // consolename
                0x00000000,
                0x00000000 ,
                0x00000000
                };
  char * __ptr32 pMsg;
  char * __ptr32 pMsg2;
  pMsg =  (char *) __malloc31(1024);
  pMsg2 = pMsg + 512 ;

  // Currently only text is supported ( the first and only parm)
  static char *kwlist[] = {"text","routecde","descr", "token", "msgid","domtoken",NULL};
  Py_ssize_t     lMsg = 0;
  char * p  = "";
  int   desc  = 0;
  int   route = 0;
  if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#|$iiiii", kwlist,
                                    &p    , // message text
                                    &lMsg , // message text
                                    &route,            // i  this variable is an array
                                    &desc ,     // i  this variable is an array
                                    &cmsg.__cm2_token, // i
                                    &cmsg.__cm2_msgid, // i
                                    &cmsg.__cm2_dom_token)) // i
  {
      return NULL;
  }
  // move the wto structure to the parameter list
  memcpy(pMsg,&wtoparm[0],sizeof(wtoparm));

  if (lMsg > 126) lMsg = 126; // max supported length

  // move the data to our buffer and convert to ebcdic
  memcpy(pMsg2+2,p,lMsg);
  __a2e_l(pMsg2+2,lMsg) ;

  short lsMsg = lMsg;
  memcpy(pMsg2 ,&lsMsg,2);   // length
  // save address of message text in the wto control block
  memcpy(pMsg+4,&pMsg2,4); // save address of structure
                           // in the wto parmlist
  rc = CPWTO( pMsg );
  rv = Py_BuildValue("(l)", rc);
  free(pMsg);
  return rv;
}

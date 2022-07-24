// Provide write to console, using C __console2 interface
// Not recommended as it is untidy
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
// This code based on the sample from IBM C run time library for __console2
//
// #define Py_IS_TYPE(ob, type)
// (_PyObject_CAST(ob)->ob_type == (type))
// static inline PyTypeObject* Py_TYPE(PyObject *ob) {
//     return ob->ob_type;
static PyObject *console_console2(PyObject *self, PyObject *args, PyObject *keywds ) {
  PyObject *rv = NULL;
  struct __cons_msg2 cmsg;
  long  rc;
  rc = 0;

  int cmsg_cmd; // input value - what the operator did.  Stop or modify
  unsigned int cmsg_rout[2] = {1,0};
  unsigned int cmsg_desc[2] = {12,0};
  /* Fill in the __cons_msg2 struct */
  cmsg.__cm2_format = __CONSOLE_FORMAT_2;
  cmsg.__cm2_msglength = 0; /* preset this */
  cmsg.__cm2_routcde = cmsg_rout;
  cmsg.__cm2_descr = cmsg_desc;
  cmsg.__cm2_token = 0;
  cmsg.__cm2_msgid = NULL;
  cmsg.__cm2_dom_token = 0;
  char * __ptr32 pStorage; // whole buffer
  char * __ptr32 pMsg2;   // used as a buffer converting from ASCII to EBCDIC
  pStorage = (char *) __malloc31(1024);
  pMsg2 = pStorage + 512 ;
  // list of keywords used to pass data to console/wto
  static char *kwlist[] = {"text",NULL};
  Py_ssize_t     lMsg = 0;
  char * p  = "";
  // preset these
  int   desc  = 0;
  int   route = 0;
  int s;
  // parse the passed data
  if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#", kwlist,
                                      &p    , // message text
                                      &lMsg)) // Length of message text
//if (!PyArg_ParseTupleAndKeywords(args, keywds, "O|$ii", kwlist,
//                                  &rv ,
//                                  &route,            // i  this variable is an array
//                                  &desc ))    // i  this variable is an array
  {
      return NULL;
  }
  if (lMsg > 126) lMsg = 126; // max supported length
  cmsg_desc[0]     = desc ;
  cmsg_rout[0]     = route;
  // we need to convert from ASCII to EBCDIC, so move data to our buffer and
  // convert it
  memcpy(pMsg2  ,p,lMsg);
  __a2e_l(pMsg2  ,lMsg) ;
  // point to our message
  cmsg.__cm2_msg = pMsg2  ;
  cmsg.__cm2_msglength   = lMsg;
  //  Now issue the console2 to display the data
  rc = __console2(&cmsg,NULL,&cmsg_cmd);// cmsg_cmd not returned on output
  if(rc == -1) {
    printf("__console2() failed\n");
    printf("Errno %d \n",errno);
    printf("%s\n",strerror(errno));
    rc = errno;
  }
  else {
  }
  rv = Py_BuildValue("(l)", rc);
  free(pStorage);
  return rv;
}
//  end of routine
#ifdef no
//  //  The following parses all of the parameters
//  //  This could be used in future
//  // parse the passed data
//   static char *kwlist[] = {"text","routecde","descr", "token", "msgid","domtoken","xx",NULL};
//  if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#|$iiiii", kwlist,
//                                    &p    , // message text
//                                    &lMsg , // message text
//                                    &route,            // i  this variable is an array
//                                    &desc ,     // i  this variable is an array
//                                    &cmsg.__cm2_token, // i
//                                    &cmsg.__cm2_msgid, // i
//                                    &cmsg.__cm2_dom_token)) // i
//  {
//      return NULL;
//  }
//
//
//  // and do the work
//  rc = __console2(&cmsg,NULL,&cmsg_cmd);
//  if(rc == -1) {
//    printf("__console2() failed\n");
//    printf("Errno %d \n",errno);
//    printf("%s\n",strerror(errno));
//    rc = errno;
//  }
//  else {
//  }
  #endif

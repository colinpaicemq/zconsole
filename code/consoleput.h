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
                0x00088010,
   /* 04 */     0x00000000, // MESSAGE TEXT ADDRESS
                0x02000068, // version,misc,replylen,wpxlen
                0x00800000, // emcs  flag,reserved
                0x00000000, // reply buffer address
                0x00000000, // reply ecb address
                0x00000000, // connect id
                0x00000000, // x1c desc codes,reserved
                0x00000000, 0x00000000, 0x00000000, 0x00000000,//xl16
                            // extended routing codes
                0x00000000, // message type, message priority
                0x40404040,   0x40404040, // 0x54 jobid
                0x40404040,   0x40404040, // jobname
                0x40404040,   0x40404040, // ret key
                0x00000000,   // token
                0x00000000,   // consoleid
                0x40404040,   0x40404040, // systemname
                0x40404040,   0x40404040, // consolename
                0x00000000, // reply console name
                0x00000000, // cart address
                0x00000000  // wsparm
                };
//char * pcWTO = (char * )&wtoparm[0]; // get a char offset into it
  char * __ptr32 pMsg;
  char * __ptr32 pMsg2;
  pMsg =  (char *) __malloc31(1024);
  pMsg2 = pMsg + 512 ;

  // Currently only text is supported ( the first and only parm)
  static char *kwlist[] = {"text","route","desc", "token", "msgid","domtoken",NULL};
  Py_ssize_t     lMsg = 0;
  char * p  = "";
  int  desc  = 0 ;
  int  route = 2 ;
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
  if (route > 0  && route <=20) // valid range
  {
       printf("routecode %d\n",route);
       char * pByte = pMsg;     //
       int i = route/8;        // which byte...
       pByte += 32 + i;         // offset to the byte 0x20
       int pos = route%8;      //gives the corresponding bit position from rhs
       pos = 8 - pos;          // from left
       unsigned char flag = 1;   // flag = 0000.....00001
       flag = flag << pos;      // flag = 0000...010...000   (shifted k positions)
      *pByte = *pByte | flag;   // Set the bit at the k-th position in A[i]
  }
  if (desc  > 0  && desc  < 16) // valid range
  {
       printf("desc %d\n",desc);
       char * pByte = pMsg;     //
       int i = desc /8;        // which byte...
       pByte += 28 + i;         // offset to the byte (0x1c)
       int pos = desc %8;      //gives the corresponding bit position from rhs
       pos = 8 - pos;          // from left
       unsigned char flag = 1;   // flag = 0000.....00001
       flag = flag << pos;      // flag = 0000...010...000   (shifted k positions)
      *pByte |= flag;           // Set the bit at the k-th position in A[i]
      int diff = pByte - pMsg;
      printf("i %d pos %d dif\n",i,pos,diff);
      printHex(stdout,pMsg,128    );
  }
  if (lMsg > 126) lMsg = 126; // max supported length

  // move the data to our buffer and convert to ebcdic
  memcpy(pMsg2+2,p,lMsg);
  __a2e_l(pMsg2+2,lMsg) ;

  short lsMsg = lMsg;
  memcpy(pMsg2 ,&lsMsg,2);   // length
  // save address of message text in the wto control block
  memcpy(pMsg+4,&pMsg2,4); // save address of len||data
//printHex(stdout,pMsg2  ,lMsg+2);
//printHex(stdout,pMsg   ,128);
  // Issue the WTO Inline.
  // It used to all CPWTO
  // Code taken from WTO expansion
  // the data is in 31 bit storage so only the bottom part of regisers
  // will be needed.  Need to clear the high part of registers
  asm(
  "        SGR   1,1       \n" // clear register 1 hi and lo
  "        L     1,%[p1]   \n" // load bottom with data
  "        L     14,1      \n" // get the address of the data
  "        SGR   15,15     \n" // clear R15 high and low
  "        AH    15,0(1,0) \n" // get length of data
  "        AR    14,15     \n" // get past the constant data
  "        SVC   35        \n" // and issue the wto
  "        ST    15,%[p2] \n"  // save the return code
  :
  : [p1] "m"(pMsg),
    [p2] "m"(rc)
  : "r14","r15","r1"
  );
                           // in the wto parmlist
  #ifdef delete
  rc = CPWTO( pMsg );
#endif
  rv = Py_BuildValue("(l)", rc);
  free(pMsg);
  return rv;
}

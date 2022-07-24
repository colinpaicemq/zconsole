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

static char console_taskinfo_doc[] =
  "get the ASCB, TCB and TCBTTIME ";

static PyObject *console_taskinfo(PyObject *self, PyObject *args, PyObject *keywds ) {
  PyObject *rv = NULL;  // returned object
  //
  // get the ASCB
  //
  char cASCB[11]; // 0x012345678n
  long pASCB;
  __asm__(" LLGT 1,548 pASCB \n"
          " STG  1,%[p1] pASCB    \n"
  : [p1] "=m"(pASCB)
  :
  : "r1","r2"
  );
  sprintf(&cASCB[0],"0x%8.8x\0",pASCB);
  //
  // get the TCB
  //
  char cTCB[11]; // 0x012345678n
  long pTCB;
  __asm__(" LLGT 1,540 pTCB  \n"
          " STG  1,%[p1] pTCB     \n"
  : [p1] "=m"(pTCB)
  :
  : "r1","r2"
  );
  sprintf(&cTCB[0],"0x%8.8x\0",pTCB);
  //
  //  JOBNAME
  //
  //   the name is either in offset 172 for jobs or 176 to stc
  char jobName[9];
  char * pJobName;
  __asm__(" LLGT 1,548 pASCB \n"
          " SGR  2,2   0 for started task \n"
          " L    2,172(1) p Jobname \n"
          " LTR  2,2   0 for started task \n"
          " JNZ  GOBACK \n"
          " L    2,176(1) p started task \n"
          "GOBACK  DS 0H                     \n"
          " STG 2,%[pJobName] save p jobname \n"
  : [pJobName] "=m"(pJobName)
  :
  : "r1","r2"
  );
  // move data to our buffer, and convert to ASCII
  memcpy(&jobName[0],pJobName,8);
  jobName[8]=0; // null terminator
  __e2a_l(&(jobName[0]),8);

  //
  //  ttime
  //
  long ttimer;
  __asm__(" LLGT 1,540 pTCB \n"
          " LLGT 2,348(1) ttime\n"
          " STG 2,%[ttime] save p jobname \n"
  : [ttime] "=m"(ttimer)
  :
  : "r1","r2"
  );
  ttimer = ttimer/1024;  //  because the bottom 12 bits are not used
  // do not convert to ASCII as already in ASCII

  rv = Py_BuildValue("{s:s,s:s,s:s,s:l}",
            "jobname",jobName,
            "ascb",  cASCB,
            "tcb",   cTCB,
            "tcbttime", ttimer);
  if (rv == NULL)
   {
    PyErr_Print();
    PyErr_SetString(PyExc_RuntimeError,"Py_BuildValue in taskinfo");
    printf(" Py_BuildValue error in taskinfo\n");
   }
  return rv;
}

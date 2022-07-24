*   issue the assembler WTO
           AGO   .SKIP
* //////////////////////////////////////////////////////////////////////
* MIT License
*
* Copyright (c) 2022 Colin Paice
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated
* documentation files (the "Software"), to deal in the
* Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute,
* sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall
* be included in all copies or substantial portions of the
* Software.
*
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
* KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS
* OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
.SKIP    ANOP
* ================================================================
CPWTO    CSECT
CPWTO    AMODE 64
CPWTO    RMODE 64
         BSM   14,0
         BAKR  R14,0
         USING  CPWTO,R14
         LGR   R3,R1
         XGR   R0,R0
         LG    R1,0(R1)          first and only parm
*        L     R2,4(R1)         A(h(len),data)
         LR    14,1 FIRST BYTE OF PARM LIST
         SR    15,15 CLEAR REGISTER 15
         AH    15,0(1,0) ADD LENGTH OF TEXT + 4
         AR    14,15 FIRST BYTE AFTER TEXT
         SVC  35                Issue WTO
*        SR  15,15
         PR
         USING SSSS,R3
SSSS     DSECT
SWTO     WTO TEXT=MYTEXT,desc=7,routCDE=3
MYTEXT   DC  H'4'
         DC  C'ABCD'
R0       EQU  0
R1       EQU  1
R2       EQU  2
R3       EQU  3
R14      EQU  14
         END

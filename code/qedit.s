*   Assembler qedit code to interface with console routines
* /////////////////////////////////////////////////////////////////////
* MIT License
*
* Copyright (c) 2022 Colin Paice
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated
* documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to
* whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall
* be included in all copies or substantial portions of the
* Software.
*
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
* KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* ================================================================
**********************************************************************
*  Logic:
*  The first time through, if the job was started, there will be
*     a CIB saying CIBSTART.
*     Copy the data back to the requestor
*     Delete the CIB entry
*     Return
*  Successive times through wait for a modify or stop request
*     There will be no CIB
*     Set the allowed number of outstanding CIBs to 1 using QEDIT
*     Set up the ECLIST
*         There is an ECB for the KILL request and one for the CIB
*         With the kill request ECB  we can terminate the wait
*     Wait
*     After wait
*     If Kill
*        return code 8
*     Return the data to caller
*     Delete the CIB
*     Return code 0
**********************************************************************
*     Input
*     Register 1-> 31 bit address of storage
**********************************************************************
**********************************************************************
*Process RENT
QEDIT    CSECT
QEDIT    AMODE 64
QEDIT    RMODE ANY
         PRINT GEN
         BSM   14,0             We need this to return in the proper
         BAKR  R14,0            Save for return
         LGR   R3,R1            Save it for debug
         XGR   R0,R0            QEDIT needs this to be 0
*        XGR   R4,R4
*        LLGTR R12,R4            Base register
         LGR   12,15
         USING *,12              MODULE ADDRESSABILITY
         LG    R2,0(R1)          first and only parm
         USING  DYNAMIC,R2
* Clear the high word of the registers we used  so we do
* not have any residual data in them
         LLGTR R1,R1
         LLGTR R6,R6
         LLGTR R7,R7
         LLGTR R8,R8
         LLGTR R9,R9
         SAM31                    31 bit mode needed
* R2 -> the buffer
         LA    R9,COMADDR         GET COMMUNICATIONS AREA
*                                 ADDRESS AT COMADDR
*------------------------------------------------------------------*
*        OBTAIN ADDRESS OF THE CIB                                 *
*------------------------------------------------------------------*
         LA    1,EXTRACT          The extract output location
         XC    EXTRACT(12),EXTRACT clear for debug
         EXTRACT (R9),FIELDS=COMM,MF=(E,EXTRACT)
*                                 Get the field = comm and store in
*                                 extract location
         L     R9,COMADDR         GET ADDRESS OF THE AREA
         USING COM,R9             USE R9 AS BASE ADDRESS OF COMM AREA
         ICM   R7,15,COMCIBPT     GET CIB ADDRESS FROM COM AREA
         USING CIB,R7             BASE CIB MAPPING
*********************************************************************
*
*  MAIN PROCESSING
*
*********************************************************************
* The first time we will have a CIB if a  started task.
* If not started task, or 2nd + time through then no CIB
         JZ    NOCIB              NO CIB, TASK WAS NOT STARTED
         TM    CIBVERB,CIBSTART
         JO    START1
         J     NOTSTART1
* STARTUP
START1   DS    0H
* If the task is started with S PYT,,,MYPARM
* then MYPARM will be passed through as the parm to this start code
         BRAS  R5,DOCIB          PROCESS THE CIB
         BRAS  R5,DELCIB         Delete the  CIB
         J     GOHOME
**************************************************************
*   Come here if job is not started task, or 2nd time through
**************************************************************
NOCIB       DS  0H
NOTSTART1   DS  0H
         QEDIT ORIGIN=COMCIBPT,CIBCTR=2   SET MODIFY LIMIT TO 2
         ICM   R1,15,COMCIBPT  GET CIB ADDRESS FROM COM AREA
         L     R1,COMECBPT     GET ADDRESS OF THE COMMUNICATION ECB
         ST    R1,MODECB       PUT ADDR OF MODIFY ECB IN LIST
         LA    R1,KILLECB      the passed ECB
         ST    R1,AKILLECB
         OI    AKILLECB,x'80'  set top bit on for wait list
*
WAIT     DS    0H
         WAIT  ECBLIST=ECBS       WAIT FOR A MODIFY/STOP or kill
         L     r1,killECB
         LTR   r1,r1
         J     NOKILL
         LA    R15,8              return code
         J     GOHOME
NOKILL   DS    0h
*
*  When  wait is posted, a modify or stop has been issued
*
         ICM   R7,15,COMCIBPT     GET CIB ADDRESS FROM COM AREA
         USING CIB,R7             BASE CIB MAPPING
         BRAS  R5,DOCIB          Return data to caller
         BRAS  R5,DELCIB          Free the CIB
         SR    R15,15
         J     GOHOME
*
GOHOME   DS  0h
         SAM64      reset back to 64 bit mode.
         PR
*------------------------------------------------------------------ *
*        FREE THE CIB                                               *
*------------------------------------------------------------------ *
DELCIB   DS    0H
         QEDIT ORIGIN=COMCIBPT,BLOCK=(R7)   FREE THE CIB
         BR    R5
*------------------------------------------------------------------ *
*        User processing                                            *
*------------------------------------------------------------------ *
DOCIB    DS    0H
*        72 is offset 72 from start of area offset(NEXTCIB)
         MVC   72(255,R2),0(R7) offsetfrom start
*              to preserve ECB data and work area
         BR     R5
*
*-------------------------------------------------------------------*
*  CONSTANTS                                                        *
*    None                                                           *
*-------------------------------------------------------------------*

DYNAMIC  DSECT
*-------------------------------------------------------------------*
*  FIELDS REQUIRED IN DYNAMIC STORAGE                               *
*-------------------------------------------------------------------*
WORKAREA DS    CL64
         ORG   WORKAREA
EYEC     DS    CL4
KILLECB  DS    A
ECBS     DS    0CL4               ECB LIST FOR WAIT
MODECB   DS    A                  ADDR(MODIFY/STOP ECB)
AKILLECB DS    A
COMADDR  DS    F                  ADDR(COMAREA) FROM EXTRACT
EXTRACT  EXTRACT MF=L             EXTRACT PARAMETER LIST
         ORG   *
SDYNAMIC EQU   *-DYNAMIC          SAVE AREA

*-------------------------------------------------------------------*
*  REQUIRED DSECTs                                                  *
*-------------------------------------------------------------------*
         PRINT  GEN
COM      DSECT
         IEZCOM   ,                COM AREA
CIB      DSECT
         IEZCIB   ,                CIB
         IKJTCB   LIST=YES
         IHAASCB  LIST=YES
*-------------------------------------------------------------------*
R0       EQU  0
R1       EQU  1
R2       EQU  2
R3       EQU  3
R4       EQU  4
R5       EQU  5
R6       EQU  6
R7       EQU  7
R8       EQU  8
R9       EQU  9
R10      EQU  10
R11      EQU  11
R12      EQU  12
R13      EQU  13
R14      EQU  14
R15      EQU  15
         END

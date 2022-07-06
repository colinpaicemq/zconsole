CPWTO    CSECT
CPWTO    AMODE 64
CPWTO    RMODE 64
         BSM   14,0
         BAKR  R14,0
         LGR   R3,R1
         XGR   R0,R0
         LG    R1,0(R1)          first and only parm
         L     R2,4(R1)          h(len),data
         LR    14,1 FIRST BYTE OF PARM LIST
         SR    15,15 CLEAR REGISTER 15
         AH    15,0(1,0) ADD LENGTH OF TEXT + 4
         AR    14,15 FIRST BYTE AFTER TEXT
*        DC    C'DEAD'
         SVC  35                Issue WTO
*        SR  15,15
         PR
R0       EQU  0
R1       EQU  1
R2       EQU  2
R3       EQU  3
R14      EQU  14
         END

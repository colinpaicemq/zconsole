//  C header file for the z/OS CIB control block used by QEDIT
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
  typedef struct tagCIB CIB  ;
  // mapping of the IEZCIB console interface block
  struct tagCIB{
      char workarea[64];
      int  killecb   ;  // request to shutdown
      int  deadecbz  ;  // Unused
  //  now the CIB proper
      char nextCIB[4];  // next CIB
      char verb;
      char cibLength8;  // length of the CIB in DW
      short cobxoff;    // Offset to CIB Extension
      char reserved08[2];
      char reserved0A[2]; // Old ASID/TSOID
      char reserved0C[1]; // CIBCONID
      char version[1];    //
      short lData   ;    // Length of data provided
      char data[1]  ;    // Actual data returned
  };
  //  Mapping of the CIB extension
  //  Located by aaddress(CIB) + cobxoff
  typedef struct tagCIBX CIBX  ;
  struct tagCIBX {
     char reserver00[4]; // pointer to utoken reserved storage
     char authcode[2];
     char authority;     //  eg 4 COMMAND HAS MASTER AUTHORITY
     char reserved07;    //
     char consolename[8];// eg userid
     char CART[8];       // eg hex data
     int  consoleid;
     char reserver24[4];
     int  origconsoleid;
     char reserverd24[12];
  };

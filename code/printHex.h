//  printhex
///////////
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
#ifndef PRINTHEX_INCLUDE
#define PRINTHEX_INCLUDE
#define ASCII_STRING \
                "................" \
                "................" \
                " !\"#½%&'()*+,-./" \
                "0123456789:;<=>?" \
                "@ABCDEFGHIJKLMNO" \
                "PQRSTUVWXYZ[\\][_" \
                "'abcdefghijklmno" \
                "pqrstuvwxyz{ }~." \
                "................" \
                "................" \
                "................" \
                "................" \
                "................" \
                "................"
#define EBCDIC_STRING \
               "................"\
               "................"\
               "................"\
               "................"\
               " .........½.<(+|"\
               "&.........!$*);^"\
               "-/........Ý,%_>?"\
               ".........`:#@'=\""\
               ".abcdefghi......"\
               ".jklmnopqr......"\
               ".~stuvwxyz...í.."\
               ".............ù.."\
               "{ABCDEFGHI......"\
               "}JKLMNOPQR......"\
               "\\.STUVWXYZ......"\
               "0123456789......"\
// const char ascii_tab[256] = ASCII_STRING ;
/////////////////////////////
const char ascii_tab[256] = EBCDIC_STRING ;
//    char other_tab[256];
char * pEBCDIC = 0;
void printHex(FILE * pFile,void *buff, long length)
{
  char sepchars[2] = {' ',' '};
  long i, j;
  unsigned char *pbuff = (unsigned char*)buff;
  long bytesleft;
  char printchar;
  for( i=0; i<length; i+=16, pbuff+=16 )
  {
    if( (bytesleft=length-i) >= 16 )
            bytesleft=16;
    /**********************************************************
     * First print a row of hex chars.
     **********************************************************/
    fprintf(pFile,"%8.8X :",i);
    for( j=0; j<bytesleft; j++ ) {
            if(j % 4 == 0) fprintf(pFile," %2.2X",*(pbuff+j));
            else fprintf(pFile,"%2.2X",*(pbuff+j));
    }
    /**********************************************************
    * This pads the hex display with blanks if this is the
    * last row and it's less than 16 bytes long.
    **********************************************************/
    for( j=bytesleft; j<16; j++ ) {
            if(j % 4 == 0) fprintf(pFile,"   ");
            else fprintf(pFile,"  ");
    }
    /***********************************************************
     * Next print the characters themselves (if they're
     * printable).
     ********************************************************/
  fprintf(pFile," %c", sepchars[0]);
  for( j=0; j<bytesleft; j++ ) {
          printchar = (char) isprint(*(pbuff+j)) ? *(pbuff+j) : '.';
          fprintf(pFile,"%c",printchar);
  }
  /**********************************************************
   * ..and pad the last row with blanks if necessary.
   **********************************************************/
  for( j=bytesleft; j<16; j++ ) fprintf(pFile," ");
  fprintf(pFile,"%c",sepchars[1]);
  for( j=0; j<bytesleft; j++ ) {
          fprintf(pFile,"%c", ascii_tab[*(pbuff+j)]);
  }
  /**********************************************************
   * ..and pad the last row with blanks if necessary.
   **********************************************************/
  for( j=bytesleft; j<16; j++ ) fprintf(pFile," ");
  fprintf(pFile,"%c\n",sepchars[1]);
  }
  fflush(pFile);
  return;
}
#endif

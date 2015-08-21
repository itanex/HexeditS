#include <conio.h>
#include <stdlib.h>
#include "scrnacc.h"

int *stackbuff[64];
int counter = 0;
int cursrow[64], curscol[64];
char screenbuff[25][80][2];

void drawscreen()
{
 ScreenUpdate(screenbuff);
}

void grabscreen()
{
 ScreenRetrieve(screenbuff);
}

void showcursor()
{
 _setcursortype(_NORMALCURSOR);
}

void hidecursor()
{
 _setcursortype(_NOCURSOR);
}

void setcursor(const int row, const int col)
{
 ScreenSetCursor(row - 1, col - 1);
}

void clear(const int backcolor, const int forecolor, const int srow, const int erow)
{
 register char *x = &screenbuff[srow-1][0][0];
 const register char *xmax = &screenbuff[erow-1][79][1];
 const register char color = (backcolor << 4) | forecolor;
 while (x < xmax) {
   *x++ = 0;
   *x++ = color;
   *x++ = 0;
   *x++ = color;
 }
}

void setcolor(const int backcolor, const int forecolor, const int startrow, const int endrow)
{
 register char *x = &screenbuff[startrow-1][0][0];
 const register char *xmax = &screenbuff[endrow-1][79][1];
 const register char color = (backcolor << 4) | forecolor;
 while (x < xmax) {
   x++;
   *x++ = color;
   x++;
   *x++ = color;
 }
}

void placestring(const int row, const int col, const int l, const char * const str)
{
 register char *w=&screenbuff[row-1][col-1][0];
 register char *x=(char *)str;
 register int chars;
 int len=l;
 if (len-1 > (&screenbuff[24][79][0] - &screenbuff[row-1][col-1][0])/2)
   len = 1 + (&screenbuff[24][79][0] - &screenbuff[row-1][col-1][0])/2;
 chars=len;
 while (chars-- > 0) {
   *w++ = *x++;
   w++;
 }
}

void placechars(const int row, const int col, const int l, register const int ch)
{
 register char *w=&screenbuff[row-1][col-1][0];
 register int chars;
 int len=l;
 if (len-1 > (&screenbuff[24][79][0] - &screenbuff[row-1][col-1][0])/2)
   len = 1 + (&screenbuff[24][79][0] - &screenbuff[row-1][col-1][0])/2;
 chars=len;
 while (chars-- > 0) {
   *w++ = ch;
   w++;
 }
}

void colorchunk(const int backcolor, const int forecolor, const int row, const int scol, const int ecol)
{
 register char *x = &screenbuff[row-1][scol-1][0];
 const register char *xmax = &screenbuff[row-1][ecol-1][1];
 const register char color = (backcolor << 4) | forecolor;
 while (x < xmax) {
   x++;
   *x++ = color;
 }
}

int pushscreen()
{
 register int *source=(int *)&screenbuff[0][0][0];
 register int *target;
 register int loopsleft=(80 * 25 * 2) >> 4;
 if (counter > 63) {
   counter = 64;
   return 0;
 }
 if ((stackbuff[counter]=(int *)malloc(80*25*2)) == NULL) return 0;
 ScreenGetCursor(&cursrow[counter], &curscol[counter]);
 target=stackbuff[counter++];
 while (loopsleft-- > 0) {
   *target++ = *source++;
   *target++ = *source++;
   *target++ = *source++;
   *target++ = *source++;
 }
 return 1;
}

int popscreen()
{
 register int *source;
 register int *target=(int *)&screenbuff[0][0][0];
 register int loopsleft=(80 * 25 * 2) / 16;
 --counter;
 if (counter < 0) {
   counter = 0;
   return 0;
 }
 source=stackbuff[counter];
 while (loopsleft-- > 0) {
   *target++ = *source++;
   *target++ = *source++;
   *target++ = *source++;
   *target++ = *source++;
 }
 ScreenSetCursor(cursrow[counter], curscol[counter]);
 free(stackbuff[counter]);
 return 1;
}

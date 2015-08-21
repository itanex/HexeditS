#include <conio.h>
#include <pc.h>
#include <string.h>
#include <ctype.h>
#include "misc.h"
#include "scrnacc.h"

const int vertical=186;  // These are the ascii codes for double lined
const int topright=187;  // boxes.
const int bottomright=188;
const int bottomleft=200;
const int topleft=201;
const int horizontal=205;

/*
 *  This function shows the help screen.
 */
void showhelp()
{
 const char title[]="The Following are Valid Commands for HexEdit";
 const char line1[]="E                                 Seek a Specific Location";
 const char line2[]="Arrow Keys                        Move the Cursor Around";
 const char line3[]="Home and End                      Move to the Top or Bottom";
 const char line4[]="PageUp and PageDown               Move Up or Down One Full Page";
 const char line5[]="Ctrl-Up or Ctrl-Down              Scroll Up or Down One Line";
 const char line6[]="Escape, Q, or X                   Exit";
 const char line7[]="H                                 Edit the Hex Field";
 const char line8[]="A                                 Edit the ASCII Field";
 const char line9[]="I or Insert                       Insert a Character";
 const char lineA[]="D or Delete                       Delete Current Character";
 const char lineB[]="Backspace                         Delete Previous Byte";
 const char lineC[]="S                                 Save Work So Far";
 const char lineD[]="B                                 Give the Current Byte Location";
 const char lineE[]="C                                 Hex <-> Decimal Calculator";
 const char lineF[]="?                                 This Screen";
 const char hitkey[]="Press Any Key to Continue...";
 const int head=(80 - strlen(title))/2;
 const int line=(80 - strlen(lineD))/2;
 const int hit=(80 - strlen(hitkey))/2;
 pushscreen();
 clear(1, 15, 2, 24);
 placestring(4, head, strlen(title), title);
 placestring(6, line, strlen(line1), line1);
 placestring(7, line, strlen(line2), line2);
 placestring(8, line, strlen(line3), line3);
 placestring(9, line, strlen(line4), line4);
 placestring(10, line, strlen(line5), line5);
 placestring(11, line, strlen(line6), line6);
 placestring(12, line, strlen(line7), line7);
 placestring(13, line, strlen(line8), line8);
 placestring(14, line, strlen(line9), line9);
 placestring(15, line, strlen(lineA), lineA);
 placestring(16, line, strlen(lineB), lineB);
 placestring(17, line, strlen(lineC), lineC);
 placestring(18, line, strlen(lineD), lineD);
 placestring(19, line, strlen(lineE), lineE);
 placestring(20, line, strlen(lineF), lineF);
 placestring(22, hit, strlen(hitkey), hitkey);
 drawscreen();
 if (!getch()) getch();
 popscreen();
 drawscreen();
}

void calc()
{
 const char input1[]="   Hex:              ";
 const char input2[]="   Decimal:          ";
 const int len=strlen(input1);
 const int startcol=(80 - len)/2 - 1;
 int row=0, done=0, value;
 unsigned char input[4]={0};
 pushscreen();
 colorchunk(1, 14, 9, startcol, startcol+1+len);
 colorchunk(1, 14, 10, startcol, startcol+1+len);
 colorchunk(1, 14, 11, startcol, startcol+1+len);
 colorchunk(1, 15, 11, startcol+1, startcol+len);
 colorchunk(1, 14, 12, startcol, startcol+1+len);
 colorchunk(1, 14, 13, startcol, startcol+1+len);
 colorchunk(1, 15, 13, startcol+1, startcol+len);
 colorchunk(1, 14, 14, startcol, startcol+1+len);
 colorchunk(1, 14, 15, startcol, startcol+1+len);
 placechars(9, startcol, 1, topleft);
 placechars(9, startcol + 1, len, horizontal);
 placechars(9, startcol + len + 1, 1, topright);
 placechars(10, startcol, 1, vertical);
 placechars(10, startcol + 1, len, ' ');
 placechars(10, startcol + len + 1, 1, vertical);
 placechars(11, startcol, 1, vertical);
 placestring(11, startcol + 1, len, input1);
 placechars(11, startcol + len + 1, 1, vertical);
 placechars(12, startcol, 1, vertical);
 placechars(12, startcol + 1, len, ' ');
 placechars(12, startcol + len + 1, 1, vertical);
 placechars(13, startcol, 1, vertical);
 placestring(13, startcol + 1, len, input2);
 placechars(13, startcol + len + 1, 1, vertical);
 placechars(14, startcol, 1, vertical);
 placechars(14, startcol + 1, len, ' ');
 placechars(14, startcol + len + 1, 1, vertical);
 placechars(15, startcol, 1, bottomleft);
 placechars(15, startcol + 1, len, horizontal);
 placechars(15, startcol + len + 1, 1, bottomright);
 colorchunk(4, 15, 11, 32, 35);
 drawscreen();
 while (!done) {
   switch (getch()) {
     case 0:
          switch(getch()) {
            case 72:
                 colorchunk(4, 15, 11, 32, 35);
                 colorchunk(1, 15, 13, 32, 39);
                 drawscreen();
                 row=0;
                 break;
            case 80:
                 colorchunk(1, 15, 11, 32, 35);
                 colorchunk(4, 15, 13, 32, 39);
                 drawscreen();
                 row=1;
                 break;
            case 77:
                 ungetch(13);
                 break;
            case 45:
            case 16:
                 ungetch(27);
                 break;
          }
          break;
     case 13:
          switch (row) {
            case 0:
                 colorchunk(1, 15, 11, 32, 35);
                 placechars(11, 41, 5, ' ');
                 placechars(13, 41, 5, ' ');
                 drawscreen();
                 showcursor();
                 setcursor(11, 42);
                 if (!(input[0]=toupper(getch()))) input[0]=toupper(getch());
                 if (isxdigit(input[0])) {
                   placechars(11, 42, 1, input[0]);
                   drawscreen();
                   setcursor(11, 43);
                   if (!(input[1]=toupper(getch()))) input[1]=toupper(getch());
                   if (isxdigit(input[1])) placechars(11, 43, 1, input[1]);
                 }
                 hidecursor();
                 value=strtol(input, NULL, 16);
                 input[0]=input[1]=input[2]=input[3]=0;
                 sprintf(input, "%d", value);
                 placestring(13, 42, 3, input);
                 colorchunk(4, 15, 11, 32, 35);
                 drawscreen();
                 input[0]=input[1]=input[2]=input[3]=0;
                 break;
            case 1:
                 colorchunk(1, 15, 13, 32, 39);
                 placechars(11, 41, 5, ' ');
                 placechars(13, 41, 5, ' ');
                 drawscreen();
                 showcursor();
                 setcursor(13, 42);
                 if (!(input[0]=getch())) input[0]=getch();
                 if (isdigit(input[0])) {
                   placechars(13, 42, 1, input[0]);
                   drawscreen();
                   setcursor(13, 43);
                   if (!(input[1]=getch())) input[1]=getch();
                   if (isdigit(input[1])) {
                     placechars(13, 43, 1, input[1]);
                     drawscreen();
                     setcursor(13, 44);
                     if (!(input[2]=getch())) input[2]=getch();
                     if (isdigit(input[2])) placechars(13, 44, 1, input[2]);
                   }
                 }
                 hidecursor();
                 value=strtol(input, NULL, 10);
                 input[0]=input[1]=input[2]=input[3]=0;
                 sprintf(input, "%X", value);
                 placestring(11, 42, 3, input);
                 colorchunk(4, 15, 13, 32, 39);
                 drawscreen();
                 input[0]=input[1]=input[2]=input[3]=0;
                 break;
          }
          break;
     case '?':
          showcalchelp();
          break;
     case 27:
     case 17:
     case 24:
     case 'Q':
     case 'q':
     case 'X':
     case 'x':
          done=1;
          break;
   }
 }
 popscreen();
 drawscreen();
}

void showcalchelp()
{
 const char title[]="The Following are Valid Calculator Commands";
 const char line1[]="Arrow Keys                        Move the Cursor Around";
 const char line2[]="Enter                             Show Data Entry Cursor";
 const char line3[]="Escape, Q, or X                   Exit";
 const char line4[]="?                                 This Screen";
 const char hitkey[]="Press Any Key to Continue...";
 const int head=(80 - strlen(title))/2;
 const int line=(80 - strlen(line2))/2;
 const int hit=(80 - strlen(hitkey))/2;
 pushscreen();
 clear(1, 15, 2, 24);
 placestring(9, head, strlen(title), title);
 placestring(11, line, strlen(line1), line1);
 placestring(12, line, strlen(line2), line2);
 placestring(13, line, strlen(line3), line3);
 placestring(14, line, strlen(line4), line4);
 placestring(16, hit, strlen(hitkey), hitkey);
 drawscreen();
 if (!getch()) getch();
 popscreen();
 drawscreen();
}

void showversion()
{
 const char line1[]="HexEdit, version 1.1";
 const char line2[]="Written by Carl Howells (c_wraith@hotmail.com)";
 const char line3[]="Source code is available upon request.";
 const char line4[]="This product includes software developed by the";
 const char line5[]="University of California, Berkeley and its contributors.";
 const char line6[]="This program was compiled with djgpp version 2.11.";
 const char line7[]="DJGPP can be obtained at http://www.delorie.com/djgpp/";
 const char line8[]="Hit any key to continue...";
 const int cent1=(80 - strlen(line1))/2 + 1;
 const int cent2=(80 - strlen(line2))/2 + 1;
 const int cent3=(80 - strlen(line3))/2 + 1;
 const int cent4=(80 - strlen(line4))/2 + 1;
 const int cent5=(80 - strlen(line5))/2 + 1;
 const int cent6=(80 - strlen(line6))/2 + 1;
 const int cent7=(80 - strlen(line7))/2 + 1;
 const int cent8=(80 - strlen(line8))/2 + 1;
 const int leftedge = cent5 - 4;
 const int rightedge = cent5 + strlen(line5) + 3;
 int x;
 pushscreen();
 colorchunk(1, 14, 6, leftedge, rightedge);
 colorchunk(1, 14, 20, leftedge, rightedge);
 placechars(6, leftedge, 1, topleft);
 placechars(6, rightedge, 1, topright);
 placechars(6, leftedge + 1, rightedge - leftedge - 1, horizontal);
 placechars(20, leftedge, 1, bottomleft);
 placechars(20, rightedge, 1, bottomright);
 placechars(20, leftedge + 1, rightedge - leftedge - 1, horizontal);
 for (x = 7; x < 20; x++) {
    colorchunk(1, 14, x, leftedge, rightedge);
    colorchunk(1, 15, x, leftedge + 1, rightedge - 1);
    placechars(x, rightedge, 1, vertical);
    placechars(x, leftedge, 1, vertical);
    placechars(x, leftedge + 1, rightedge - leftedge - 1, ' ');
 }
 placestring(8, cent1, strlen(line1), line1);
 placestring(9, cent2, strlen(line2), line2);
 placestring(11, cent3, strlen(line3), line3);
 placestring(12, cent4, strlen(line4), line4);
 placestring(13, cent5, strlen(line5), line5);
 placestring(15, cent6, strlen(line6), line6);
 placestring(16, cent7, strlen(line7), line7);
 placestring(18, cent8, strlen(line8), line8);
 drawscreen();
 if (!getch()) getch();
 popscreen();
 drawscreen();
}

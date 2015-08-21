#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>
#include "scrnacc.h"
#include "io.h"

#define BUFFERSIZE (1024 * 1024)  // This is the size of the buffer for copying
FILE *temp;  // This is the global temp file pointer
char tempname[L_tmpnam];  // This is the global name of the temporary file
char filename[L_tmpnam];  // This is the global name of the edited file
int row=1, col=1;  // Global cursor position
int readonly=0;  // Global read-only flag
int currentbyte=1; // This is the global byte position
unsigned char copybuffer[BUFFERSIZE]; // This is the buffer for copying the file
unsigned char showbuffer[23 * 16]; // This is the buffer for displaying the file
int filesize=0;  // The size of the file in bytes
int topleftbyte=1;  // The byte in the top right corner
unsigned char hextable[256][2];  // Lookup table for Hex conversions
int changed=0;  // Whether the file has been modified since the last save
int excess=0;  // tracks the data beyond the end of the file.


/*
 *  This function returns non-zero if the file is read only.  It also
 *  creates the temp file, and initializes the hex table.
 */
int initfile(const char *file)
{
 int hexinit;
 int bytesmoved;
 FILE *source;
 if (!(source=fopen(file, "rb"))) return -1;
 fclose(source);
 if (!(source=fopen(file, "ab"))) {
   readonly=1;
 }
 fclose(source);
 if (!(source=fopen(file, "rb"))) return -1;
 strcpy(filename, file);
 tmpnam(tempname);
 if (!(temp=fopen(tempname, "wb+"))) {
   printf("Error creating temporary file.");
   exit (1);
 }
 do {
   if ((filesize += (bytesmoved=fread(copybuffer, 1, BUFFERSIZE, source))) > INT_MAX - BUFFERSIZE*7) {
    printf("Sorry, that file is too big for this software to handle\n");
    fclose(temp);
    remove(tempname);
    exit(1);
   }
   if (fwrite(copybuffer, 1, bytesmoved, temp) != bytesmoved) {
     printf("Write error during creation of temporary file\n");
     fclose(temp);
     remove(tempname);
     exit(1);
   }
 } while (bytesmoved==BUFFERSIZE);
 fclose(source);
 fflush(temp);
 for (hexinit=0; hexinit < 256; hexinit++) {
    sprintf(hextable[hexinit], "%02X", hexinit);
 }
 return readonly;
}


/*
 *  This function should fill the screen, and should not call drawscreen().
 */
void showpage()
{
 int x1, x2, y;
 int bytesread;
 fseek(temp, topleftbyte-1, SEEK_SET);
 bytesread =1 + filesize - topleftbyte;
 if (bytesread > 23 * 16) bytesread = 23 * 16;
 if (bytesread != fread(showbuffer, 1, bytesread, temp)) {
   emergencyexit("Error reading temporary file\n");
 }
 clear(1, 15, 2, 24);
 for (x1=0; x1 < 23; x1++) {
    if ((x2 = x1 << 4) >= bytesread) break;
    if (bytesread - x2 > 8) placechars(x1 + 2, 29, 1, '-');
    for (y=0; y < 16; y++) {
       if (x2 + y >= bytesread) break;
       placestring(x1 + 2, (y + 1)*3 + 3, 2, hextable[showbuffer[x2 + y]]);
       placechars(x1 + 2, y + 60, 1, showbuffer[x2 + y]);
    }
 }
}


/*
 *  This function should shift the contents of the file by one byte longer,
 *  from the given position.
 */
void insert()
{
 int bytesmoved;
 unsigned int tempbyte = currentbyte;
 if (readonly) return;
 if (currentbyte > filesize) return;
 while ((tempbyte += BUFFERSIZE) < filesize);
 do {
   tempbyte -= BUFFERSIZE;
   fseek(temp, tempbyte-1, SEEK_SET);
   bytesmoved = filesize - (tempbyte - 1);
   if (bytesmoved > BUFFERSIZE) bytesmoved = BUFFERSIZE;
   if (bytesmoved != fread(copybuffer, 1, bytesmoved, temp)) {
     emergencyexit("Error reading from temporary file\n");
   }
   fseek(temp, tempbyte, SEEK_SET);
   if (fwrite(copybuffer, 1, bytesmoved, temp) != bytesmoved) {
     emergencyexit("Could not write to temporary file\n");
   }
 } while (tempbyte > currentbyte);
 if (--excess < 0) excess = 0;
 filesize++;
 showpage();
 showpercent();
 colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 drawscreen();
 changed = 1;
}


/*
 *  These functions should shift the contents of the file by one byte
 *  shorter, from the given position.  I must investigate the best way
 *  to shorten the file at that point.
 */
void removbyte()
{
 int bytesmoved;
 unsigned int tempbyte=currentbyte;
 if (readonly) return;
 if (currentbyte > filesize) return;
 do {
   fseek(temp, tempbyte, SEEK_SET);
   bytesmoved = filesize - tempbyte;
   if (bytesmoved > BUFFERSIZE) bytesmoved = BUFFERSIZE;
   if (bytesmoved != fread(copybuffer, 1, bytesmoved, temp)) {
     emergencyexit("Error reading from temporary file\n");
   }
   fseek(temp, tempbyte - 1, SEEK_SET);
   if (fwrite(copybuffer, 1, bytesmoved, temp) != bytesmoved) {
     emergencyexit("Could not write to temporary file\n");
   }
   tempbyte += BUFFERSIZE;
 } while (bytesmoved == BUFFERSIZE);
 filesize--;
 if (excess++ > BUFFERSIZE) {
   fclose(temp);
   if (truncate(tempname, filesize)) {
     emergencyexit("Could not shorten temporary file\n");
   }
   if (!(temp=fopen(tempname, "rb+"))) {
     emergencyexit("Could not reopen temporary file after truncating\n");
   }
   excess = 0;
 }
 showpage();
 showpercent();
 colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 drawscreen();
 changed = 1;
}

void backspace()
{
 if (readonly) return;
 if (currentbyte == 1) return;
 cursleft();  // Ok, I could just reimplement the whole thing, but why?
 removbyte(); // This works nearly as well.  The difference isn't noticeable.
}


/*
 *  This functions saves the work so far, by copying the temporary file
 *  to the file being edited.
 */
void save()
{
 const char input[]="Save Changes? (Y/N)";
 int in;
 unsigned int bytesmoved;
 FILE *dest;
 if (readonly) return;
 if (!changed) return;
 placestring(25, 4, strlen(input), input);
 drawscreen();
 showcursor();
 setcursor(25, 5 + strlen(input));
 if (!(in=getch())) in=getch();
 hidecursor();
 if (toupper(in) == 'Y') {
   if (!(dest=fopen(filename, "wb"))) {
     emergencyexit("Could not open original file to save\n");
   }
   fseek(temp, 0, SEEK_SET);
   fseek(dest, 0, SEEK_SET);
   do {
     bytesmoved = filesize - ftell(temp);
     if (bytesmoved > BUFFERSIZE) bytesmoved = BUFFERSIZE;
     if (bytesmoved != fread(copybuffer, 1, bytesmoved, temp)) {
       emergencyexit("Error reading from temporary file\n");
     }
     if (fwrite(copybuffer, 1, bytesmoved, dest) != bytesmoved) {
       fclose(dest);
       emergencyexit("Error saving changes\n");
     }
   } while (bytesmoved==BUFFERSIZE);
   fclose(dest);
   fflush(temp);
   changed=0;
 }
 placechars(25, 4, strlen(input), ' ');
 drawscreen();
}


/*
 *  This function closes the temporary file and removes it.
 */
void closefile()
{
 fclose(temp);
 remove(tempname);
}


/*
 *  This function moves the view to the end of the file.
 */
void enddata()
{
 topleftbyte = (((filesize >> 4) - 20) << 4) + 1;
 if (topleftbyte < 1) topleftbyte = 1;
 currentbyte = topleftbyte + ((row - 1) << 4) + (col - 1);
 if (currentbyte > filesize + 1) {
   currentbyte = filesize + 1;
   row = ((currentbyte - topleftbyte) >> 4) + 1;
   col = ((currentbyte - topleftbyte) % 16) + 1;
 }
 showpage();
 showpercent();
 if (!readonly) {
   colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
   colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 }
 drawscreen();
}


/*
 *  This function moves the view to the beginning of the file.
 */
void home()
{
 topleftbyte = 1;
 currentbyte = topleftbyte + ((row - 1) << 4) + (col - 1);
 showpage();
 showpercent();
 if (!readonly) {
   colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
   colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 }
 drawscreen();
}


/*
 *  This function edits the ASCII portion of the current byte.
 */
void editasc()
{
 unsigned char inchar;
 if (readonly) return;
 colorchunk(1, 15, row + 1, 59 + col, 59 + col);
 placestring(row + 1, col + 59, 1, " ");
 showcursor();
 setcursor(row + 1, col + 59);
 drawscreen();
 if (!(inchar=getch())) inchar=getch();
 hidecursor();
 fseek(temp, currentbyte-1, SEEK_SET);
 if (!fwrite(&inchar, 1, 1, temp)) {
   emergencyexit("Could not write to temporary file\n");
 }
 fflush(temp);
 if (currentbyte == filesize + 1) filesize++;
 showpage();
 showpercent();
 colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 drawscreen();
 changed = 1;
}


/*
 *  This function edits the hex portion of the current byte.
 */
void edithex()
{
 unsigned char input[3]={0}, inchar;
 if (readonly) return;
 colorchunk(1, 15, row + 1, col*3 + 3, col*3 + 4);
 placestring(row + 1, col*3 + 3, 2, "  ");
 drawscreen();
 showcursor();
 setcursor(row + 1, col*3 + 3);
 if (!(input[0]=getch())) input[0]=getch();
 if (isxdigit(input[0])) {
   placechars(row + 1, col*3 + 3, 1, toupper(input[0]));
   drawscreen();
   setcursor(row + 1, col*3 + 4);
   if (!(input[1]=getch())) input[1]=getch();
 }
 hidecursor();
 inchar = strtol(input, NULL, 16);
 fseek(temp, currentbyte-1, SEEK_SET);
 if (!fwrite(&inchar, 1, 1, temp)) {
   emergencyexit("Could not write to temporary file\n");
 }
 fflush(temp);
 if (currentbyte == filesize + 1) filesize++;
 showpage();
 showpercent();
 colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 drawscreen();
 changed = 1;
}


/*
 *  Everything else has its own function, why not this?
 */
void pageup()
{
 if (topleftbyte < 369) topleftbyte = 369;
 currentbyte = topleftbyte + ((row - 1) << 4) + (col - 1);
 currentbyte -= 368;
 topleftbyte -= 368;
 showpage();
 showpercent();
 if (!readonly) {
   colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
   colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 }
 drawscreen();
}


/*
 *  Everything else has its own function, why not this?
 */
void pagedown()
{
 topleftbyte += 368;
 if (topleftbyte > filesize - (20 * 16)) {
  while ((topleftbyte -= 16) > filesize - (20 * 16));
 }
 if (topleftbyte < 1) topleftbyte = 1;
 currentbyte = topleftbyte + ((row - 1) << 4) + (col - 1);
 if (currentbyte > filesize + 1) {
   currentbyte = filesize + 1;
   row = ((currentbyte - topleftbyte) >> 4) + 1;
   col = ((currentbyte - topleftbyte) % 16) + 1;
 }
 showpage();
 showpercent();
 if (!readonly) {
   colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
   colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 }
 drawscreen();
}


/*
 *  This function exists primarily to facilitate cursup on a read only file.
 */
void lineup()
{
 if (topleftbyte < 17) return;
 topleftbyte -= 16;
 currentbyte -= 16;
 showpage();
 showpercent();
 if (!readonly) {
   colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
   colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 }
 drawscreen();
}


/*
 *  This function exists primarily to facilitate cursdown on a read only file.
 */
void linedown()
{
 if (topleftbyte + (20 * 16) > filesize) return;
 topleftbyte += 16;
 currentbyte = topleftbyte + ((row - 1) << 4) + (col - 1);
 if (currentbyte > filesize + 1) {
   currentbyte = filesize + 1;
   row = ((currentbyte - topleftbyte) >> 4) + 1;
   col = ((currentbyte - topleftbyte) % 16) + 1;
 }
 showpage();
 showpercent();
 if (!readonly) {
   colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
   colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 }
 drawscreen();
}


/*
 *  This functions shows the percent of the file that the cursor is.
 */
void showpercent()
{
 char line[]="100";
 if (filesize != 0) sprintf(line, "%d\0", (int)((double)100*currentbyte/filesize));
 placestring(1, 3, 3, "   ");
 placestring(1, 7-strlen(line), strlen(line), line);
}



/*
 *  These functions move the cursor.  In the event the cursor is somewhere
 *  that must be dealt with in a special manner, these functions handle it.
 */

void cursup()
{
 if (readonly) {
   lineup();
   return;
 }
 if (currentbyte < 17) return;
 colorchunk(1, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(1, 15, row + 1, 59 + col, 59 + col);
 if (row==1) {
   topleftbyte -= 16;
   showpage();
   row++;
 }
 row--;
 currentbyte -= 16;
 colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 showpercent();
 drawscreen();
}

void cursdown()
{
 if (readonly) {
   linedown();
   return;
 }
 if (currentbyte+15 > filesize) return;
 colorchunk(1, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(1, 15, row + 1, 59 + col, 59 + col);
 if (row==23) {
   topleftbyte += 16;
   showpage();
   row--;
 }
 row++;
 currentbyte += 16;
 colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 showpercent();
 drawscreen();
}

void cursleft()
{
 if (readonly) return;
 if (currentbyte == 1) return;
 colorchunk(1, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(1, 15, row + 1, 59 + col, 59 + col);
 if (row == 1 && col == 1) {
   col = 17;
   topleftbyte -= 16;
   showpage();
 }
 if (col == 1) {
   col = 17;
   row--;
 }
 col--;
 currentbyte--;
 colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 showpercent();
 drawscreen();
}

void cursright()
{
 if (readonly) return;
 if (currentbyte == filesize + 1) return;
 colorchunk(1, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(1, 15, row + 1, 59 + col, 59 + col);
 if (row == 23 && col == 16) {
   col = 0;
   topleftbyte += 16;
   showpage();
 }
 if (col == 16) {
   col = 0;
   row++;
 }
 col++;
 currentbyte++;
 colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 showpercent();
 drawscreen();
}

void initcurs()
{
 showpercent();
 if (readonly) return;
 colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
 colorchunk(4, 15, row + 1, 59 + col, 59 + col);
}

void seek()
{
 const char message[]="What percent of the file?";
 const int len=strlen(message);
 char input[4]={0};
 int target;
 placestring(25, 4, len, message);
 drawscreen();
 showcursor();
 setcursor(25, 6 + len);
 if (!(input[0]=getch())) input[0]=getch();
 if (isdigit(input[0])) {
   placechars(25, 6 + len, 1, input[0]);
   drawscreen();
   setcursor(25, 7 + len);
   if (!(input[1]=getch())) input[1]=getch();
   if (isdigit(input[1])) {
     placechars(25, 7 + len, 1, input[1]);
     drawscreen();
     setcursor(25, 8 + len);
     if (!(input[2]=getch())) input[2]=getch();
   }
 }
 hidecursor();
 target = strtol(input, NULL, 10);
 if (target < 0) target = 0;
 if (target > 109) target /= 10;
 topleftbyte=((((int)((double)target * filesize / 100)) >> 4) - 10 << 4 ) + 1;
 if (topleftbyte < 1 || topleftbyte > filesize) topleftbyte = 1;
 currentbyte = ceil((double)target * filesize / 100);
 if (currentbyte > filesize + 1) currentbyte = 1;
 row = ((currentbyte - topleftbyte) >> 4) + 1;
 col = ((currentbyte - topleftbyte) % 16) + 1;
 if (target == 0) currentbyte = topleftbyte = row = col = 1;
 showpage();
 showpercent();
 placechars(25, 4, len + 4, ' ');
 if (!readonly) {
   colorchunk(4, 15, row + 1, col*3 + 3, col*3 + 4);
   colorchunk(4, 15, row + 1, 59 + col, 59 + col);
 }
 drawscreen();
}

void emergencyexit(const char *message)
{
 fclose(temp);
 remove(tempname);
 popscreen();
 drawscreen();
 printf(message);
 showcursor();
 exit(1);
}


/*
 *  This function should give the current byte in a popup window.
 */
void givecurrentbyte()
{
 const int vertical=186;
 const int topright=187;
 const int bottomright=188;
 const int bottomleft=200;
 const int topleft=201;
 const int horizontal=205;
 const char message[]="     The current byte is:     ";
 const char output1[]="   Hex:                       ";
 const char output2[]="   Decimal:                   ";
 const char len=strlen(message);
 const int startcol=(80 - len)/2 - 1;
 unsigned char decstring[17]={0}, hexstring[17]={0};
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
 colorchunk(1, 15, 15, startcol+1, startcol+len);
 colorchunk(1, 14, 16, startcol, startcol+1+len);
 colorchunk(1, 14, 17, startcol, startcol+1+len);
 placechars(9, startcol, 1, topleft);
 placechars(9, startcol + 1, len, horizontal);
 placechars(9, startcol + len + 1, 1, topright);
 placechars(10, startcol, 1, vertical);
 placechars(10, startcol + 1, len, ' ');
 placechars(10, startcol + len + 1, 1, vertical);
 placechars(11, startcol, 1, vertical);
 placestring(11, startcol + 1, len, message);
 placechars(11, startcol + len + 1, 1, vertical);
 placechars(12, startcol, 1, vertical);
 placechars(12, startcol + 1, len, ' ');
 placechars(12, startcol + len + 1, 1, vertical);
 placechars(13, startcol, 1, vertical);
 placestring(13, startcol + 1, len, output1);
 placechars(13, startcol + len + 1, 1, vertical);
 placechars(14, startcol, 1, vertical);
 placechars(14, startcol + 1, len, ' ');
 placechars(14, startcol + len + 1, 1, vertical);
 placechars(15, startcol, 1, vertical);
 placestring(15, startcol + 1, len, output2);
 placechars(15, startcol + len + 1, 1, vertical);
 placechars(16, startcol, 1, vertical);
 placechars(16, startcol + 1, len, ' ');
 placechars(16, startcol + len + 1, 1, vertical);
 placechars(17, startcol, 1, bottomleft);
 placechars(17, startcol + 1, len, horizontal);
 placechars(17, startcol + len + 1, 1, bottomright);
 sprintf(hexstring, "%16x", currentbyte);
 sprintf(decstring, "%16d", currentbyte);
 placestring(13, 36, 16, hexstring);
 placestring(15, 36, 16, decstring);
 drawscreen();
 if (!getch()) getch();
 popscreen();
 drawscreen();
}

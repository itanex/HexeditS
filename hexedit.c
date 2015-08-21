Attribute VB_Name = "Module1"


int main (int argc, char *argv[])
{
 int initreturn;
 'start program based on dos prompt
 if (argc != 2) {
   printf("The correct syntax is:\n\nhexedit filename\n");
   return 1;
 }
 if ((initreturn=initfile(argv[1])) == -1) {
   printf("File not found\n");
   return 1;
 }
 'end start
 
 'open file into program
 initscreen(argv[1], initreturn);
 'end open file
 
 showversion();
 mainloop();
 closefile();
 closescreen();
 return 0;
}

void mainloop()
{
 int done=0;
 while (!done) {
    switch (getch()) {
      Case 0:
           switch (getch()) {
             Case 72:
                  cursup();
                  break;
             Case 80:
                  cursdown();
                  break;
             Case 75:
                  cursleft();
                  break;
             Case 77:
                  cursright();
                  break;
             Case 82:
                  insert();
                  break;
             Case 83:
                  removbyte();
                  break;
             Case 71:
                  home();
                  break;
             Case 79:
                  enddata();
                  break;
             Case 73:
                  pageup();
                  break;
             Case 81:
                  pagedown();
                  break;
             Case 141:
                  lineup();
                  break;
             Case 145:
                  linedown();
                  break;
             Case 45:
             Case 16:
                  ungetch(27);
                  break;
           }
           break;
      case 'A':
      case 'a':
           editasc();
           break;
      case 'H':
      case 'h':
           edithex();
           break;
      case 'E':
      case 'e':
           seek();
           break;
      case 'V':
      case 'v':
           showversion();
           break;
      case 'B':
      case 'b':
           givecurrentbyte();
           break;
      case 'C':
      case 'c':
           calc();
           break;
      Case 8:
           backspace();
           break;
      case 'D':
      case 'd':
           removbyte();
           break;
      case 'I':
      case 'i':
           insert();
           break;
      case 'S':
      case 's':
           save();
           break;
      case '?':
           showhelp();
           break;
      Case 27:
      Case 17:
      Case 24:
      case 'Q':
      case 'q':
      case 'X':
      case 'x':
           save();
           done = 1;
           break;
    }
 }
}


void initscreen(const char *filename, int readonly)
{
 const char title[]="HexEdit Ver. 1.1.  By Carl Howells";
 const char help[]="Press ? for Help";
 const char read[]="*Read Only*";
 const char *base=basename(filename);
 hidecursor();
 grabscreen();
 pushscreen();
 clear(1, 15, 1, 25);
 setcolor(7, 4, 1, 1);
 setcolor(7, 4, 25, 25);
 placestring(1, 15, strlen(title), title);
 placestring(1, 60, strlen(base), base);
 placestring(25, 61, strlen(help), help);
 if (readonly) placestring(25, 41, strlen(read), read);
 placestring(1, 8, 1, "%");
 showpage();
 initcurs();
 drawscreen();
}

void closescreen()
{
 popscreen();
 drawscreen();
 showcursor();
}

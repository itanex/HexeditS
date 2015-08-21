#if !defined SCREENACCESS_HEADER
#define SCREENACCESS_HEADER

#if defined __cplusplus
extern "C" {
#endif

void drawscreen();
void grabscreen();
void showcursor();
void hidecursor();
void setcursor(const int row, const int col);
void clear(const int backcolor, const int forecolor, const int srow, const int erow);
void setcolor(const int backcolor, const int forecolor, const int startrow, const int endrow);
void placestring(const int row, const int col, const int l, const char * const str);
void placechars(const int row, const int col, const int l, register const int ch);
void colorchunk(const int backcolor, const int forecolor, const int row, const int scol, const int ecol);
int pushscreen();
int popscreen();

#if defined __cplusplus
}
#endif

#endif //SCREENACCESS_HEADER

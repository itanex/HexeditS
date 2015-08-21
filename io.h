#if !defined FILEIO_HEADER
#define FILEIO_HEADER

#if defined __cplusplus
extern "C" {
#endif

int initfile(const char *file);
void showpage();
void insert();
void removbyte();
void backspace();
void save();
void closefile();
void enddata();
void home();
void editasc();
void edithex();
void pageup();
void pagedown();
void lineup();
void linedown();
void showpercent();
void cursup();
void cursdown();
void cursleft();
void cursright();
void initcurs();
void seek();
void givecurrentbyte();
void emergencyexit(const char *message);

#if defined __cplusplus
}
#endif

#endif // !defined FILEIO_HEADER

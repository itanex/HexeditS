#if !defined HEXEDIT_HEADER
#define HEXEDIT_HEADER

#if defined __cplusplus
extern "C" {
#endif

int main (int argc, char *argv[]);
void mainloop();
void initscreen(const char *filename, int readonly);
void closescreen();

#if defined __cplusplus
}
#endif

#endif // !defined HEXEDIT_HEADER

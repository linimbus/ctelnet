#ifndef _CONIO_H_
#define _CONIO_H_

#ifdef __cplusplus
extern "C" {
#endif

struct text_info {
    unsigned char winleft;
    unsigned char wintop;
    unsigned char winright;
    unsigned char winbottom;
    unsigned char attribute;
    unsigned char normattr;
    unsigned char currmode;
    unsigned char screenheight;
    unsigned char screenwidth;
    unsigned char curx;
    unsigned char cury;
};

enum COLORS {
    /*  dark colors     */
    BLACK,          
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    /*  light colors    */
    DARKGRAY, /* "light black" */
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
};
#define BLINK   0x80    /*  blink bit; doesn't work yet  */

#define _NOCURSOR      0
#define _SOLIDCURSOR   1
#define _NORMALCURSOR  2

/* 19 of 31 functions implemented, 62% */

int _conio_kbhit();
void _set_screen_lines(int nlines);
void _setcursortype(int _type);                                                 /* done */
void blinkvideo();
char *cgets(char *_str);                                 /* how does it work? */
void clreol();                                           /* what does it do? */
void clrscr();                                                                  /* done */
#define cprintf printf                                                          /* done */
int cputs(const char *_str);                                                    /* done */
#define cscanf scanf                                                            /* done */
void delline();
#define getch getchar                                                           /* done */
int getche();                                                                   /* done */
int gettext(int _left, int _top, int _right, int _bottom, void *_destin);
void gettextinfo(struct text_info *_r);                                         /* done */
void gotoxy(int x, int y);                                                      /* done */
void gppconio_init();                                                           /* done; does nothing */
void highvideo();                                                               /* maybe */
void insline();                                                                 /* done */
void intensevideo();
void lowvideo();
int movetext(int _left, int _top, int _right, int _bottom,
             int _destleft, int _desttop);
void normvideo();
int putch(int _c);                                                              /* done */
int puttext(int _left, int _top, int _right, int _bottom, void *_source);
void textattr(int _attr);                                                       /* done */
void textbackground(int _color);                                                /* done */
void textcolor(int _color);                                                     /* done */
void textmode(int _mode);
int ungetch(int);
int wherex();                                                                   /* done */
int wherey();                                                                   /* done */
void window(int _left, int _top, int _right, int _bottom);                      /* done */

#ifdef __cplusplus
}
#endif

#endif _CONIO_H_

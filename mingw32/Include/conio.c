#ifndef _CONIO_C_
#define _CONIO_C_

/* Please keep all functions alphabetical! */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include "conio.h"

int __FOREGROUND = LIGHTGRAY;
int __BACKGROUND = BLACK;

void _setcursortype(int _type) {
  CONSOLE_CURSOR_INFO Info;
  Info.bVisible = TRUE;
  if (_type == _NOCURSOR)
     Info.bVisible = FALSE;
  else if (_type == _SOLIDCURSOR)
     Info.dwSize = 100;
  else if (_type == _NORMALCURSOR)
     Info.dwSize = 1;
  SetConsoleCursorInfo (GetStdHandle (STD_OUTPUT_HANDLE), &Info);
}

void clreol() {
  /* What does this function do? */
}

void clrscr() {
  COORD coord;
  DWORD written;
  CONSOLE_SCREEN_BUFFER_INFO info;

  coord.X = 0;
  coord.Y = 0;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
  FillConsoleOutputCharacter (GetStdHandle(STD_OUTPUT_HANDLE), ' ',
    info.dwSize.X * info.dwSize.Y, coord, &written);
  gotoxy (1, 1);
}

int cputs(const char *_str) {
  printf ("%s\n", _str);
  return 0;
}

int getche() {
  int ch;
  ch = getch ();
  printf ("%c\n", ch);
  return ch;
}

void gettextinfo(struct text_info *_r) {
  CONSOLE_SCREEN_BUFFER_INFO Info;
  GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &Info);
  _r->winleft = Info.srWindow.Left;
  _r->winright = Info.srWindow.Right;
  _r->wintop = Info.srWindow.Top;
  _r->winbottom = Info.srWindow.Bottom;
  _r->attribute = Info.wAttributes;
  _r->normattr = LIGHTGRAY | BLACK;
/*  _r->currmode = ; */ /* What is currmode? */
  _r->screenheight = Info.dwSize.Y;
  _r->screenwidth = Info.dwSize.X;
  _r->curx = wherex ();
  _r->cury = wherey ();
}

void gotoxy(int x, int y) {
  COORD c;
  c.X = x - 1;
  c.Y = y - 1;
  SetConsoleCursorPosition (GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void gppconio_init() {
  /* Do nothing */
}

void highvideo() {
  if (__FOREGROUND <= BROWN)
     textcolor (__FOREGROUND + 9);
  if (__BACKGROUND <= BROWN)
     textbackground (__BACKGROUND + 9);
}

void insline() {
  printf ("\n");
}

int putch(int _c) {
  printf ("%c", _c);
  return _c;
}

void textattr(int _attr) {
  SetConsoleTextAttribute (GetStdHandle(STD_OUTPUT_HANDLE), _attr);
//  printf ("%d\n", text_info.screenheight);
}

void textbackground(int _color) {
  if (_color == BLINK)
     _color = WHITE;
  __BACKGROUND = _color;
  textattr (__FOREGROUND | (_color + 29));
}

void textcolor(int _color) {
  if (_color == BLINK)
     _color = WHITE;
  __FOREGROUND = _color;
  textattr(_color | __BACKGROUND);
}

int wherex() {
  CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
  return info.dwCursorPosition.X;
}

int wherey() {
  CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
  return info.dwCursorPosition.Y - 2;
}

void window(int _left, int _top, int _right, int _bottom) {
  SMALL_RECT R;
  R.Left = _left;
  R.Top = _top;
  R.Right = _right;
  R.Bottom = _bottom;
  SetConsoleWindowInfo (GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &R);
  gotoxy (_left, _top);
}

#endif _CONIO_C_

#ifndef DEFS_H
#define DEFS_H

#define u_int   unsigned int
#define u_char  unsigned char
#define bool    char

#define true    1
#define false   0
#define TRUE    1
#define FALSE   0
#define on      1
#define off     0
#define ON      1
#define OFF     0

#ifndef NULL
  #define NULL 0
#endif

#define TRANSP  0
#define BLACK   1
#define GREEN   2
#define LBLUE   5
#define DBLUE   4
#define LGREEN  3
#define DRED    6
#define LRED    9
#define RED     8
#define GREY    14
#define WHITE   15
#define MAGENTA 13
#define SKYBLUE 7
#define DGREEN  12
#define LYELLOW 11
#define DYELLOW 10

#define FORCLR  (*(TINY *)0xf3e9)
#define BAKCLR  (*(TINY *)0xf3ea)
#define BDRCLR  (*(TINY *)0xfe3b)
#define JIFFY   (*(TINY *)0xfc9e)

#define i2f(v)  ((v) << 6)

#define f2i(v)  ((v) >> 6)


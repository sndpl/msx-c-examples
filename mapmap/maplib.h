/*
 * maplib.h
 * on 26 nov 1990 by nao-i
 * (C) 1990 N. Ishikawa
 * free to use and copy without warranty
 */
#ifndef MAPLIB_H           /* avoiding multiple including */
#define MAPLIB_H

struct WORDREGS {
  unsigned int AF;
  unsigned int BC;
  unsigned int DE;
  unsigned int HL;
  unsigned int IX;
  unsigned int IY;
};

struct BYTEREGS {
  char F, A;
  char C, B;
  char E, D;
  char L, H;
  char IXL, IXH;
  char IYL, IYH;
};

union REGS {
  struct WORDREGS x;
  struct BYTEREGS h;
};

#ifdef LSIC80
extern int callz(char *to_call, union REGS *inregs, union REGS *outregs);
#else
extern int callz();
#endif /* LSIC80 */

typedef struct {
  char map_slot;
  char map_total;
  char map_free;
  char map_system;
  char map_user;
  char map_reserved[3];
} mapv_t;

typedef struct {
  char pmap_slot;
  char pmap_total;
  char pmap_free;
  int (*map_jump)(); /* address of jump table */
} maps_t;

#define MAX_MAP_SLOT 9

#ifdef LSIC80
extern mapv_t *mapinit(maps_t *maps);
#else
extern mapv_t *mapinit();
#endif  /* LSIC80 */

#endif /*MAPLIB_H */

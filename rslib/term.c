/*
 * term.c : terminal on MSX-DOS
 * for MSX-C 1.1 or 1.2 with MSX-C library
 * do not exit() before rsrestore()
 *
 * by nao-i on 19. jan 1988 dedicated for CoCo
 * by nao-i on 22. Mar 1990 improved for MSX-SERIAL232
 * (C) Isikawa 1988,1990
 * free to use and copy, but no guaruantee or support
 */
#pragma nonrec
#include <stdio.h>
#include <msxbios.h>
#include "rslib.h"
typedef unsigned uint;
typedef char void;

static rstbl_t deftbl = {
  '8', 'N', '1', 'X', 'N', 'N', 'N', 'N',
  (uint)1200, (uint)1200, (char)0 };

void kbkill()     /* kill keyboard buffer */
{
  while (chsns())
      chget();
}

static void term()
{
  kbkill();
  do {
    if (rsloc()) {
      register int cc;
      if ((cc = rsgetc()) == -1) {
#ifdef PRERR
        printf("\nError %04XH\n", rsstat());
#else
        chput('?');
#endif
      } else {
        chput((char)cc);
      }
    }
    if (chsns()) {
      rsputc((char)chget());
    }
  } while (!breakx());
  puts("\nStopped.\n");
  kbkill();
}

void main(argc, argv)
int argc;
char **argv;
{
#ifdef SBUG
#define rsstack ((char*)0xf55e)
#undef RSSTACKSIZE
#define RSSTACKSIZE (0xf660 - 0xf55e)
#else
  auto char rsstack[RSSTACKSIZE];
#endif /* endifdef SBUG */
  auto char rsfcb[RSFCBSIZE];

  if ((uint)rsfcb < 0x8000 || (uint)rsstack < 0xc100) {
    puts("Out of memory.\n");
    return;
  }
  puts("Running\n");
  printf("rsinit() returns %d\n", (int)rsinit(&deftbl));
  printf("rsopen() returns %d\n", (int)rsopen());
  printf("rsstat() returns %XH\n", rsstat());
  term();
  printf("rsstat() returns %XH\n", rsstat());
  printf("rsclose() returns %d\n", (int)rsclose());
  rsrestore(); /* Do not forget ! */
  return;
}

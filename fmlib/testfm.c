/*
 *	testfm.c
 *  by nao-i on 29. May. 1990
 *  (C) Isikawa 1990
 *  free to use and copy, but no guarantee or support
 */

#include <stdio.h>
#include "fmlib.h"
#pragma nonrec

#define TESTLENGTH 20
#define TESTTIMES 4


static char fmdata[] = {      /* test data */
  14, 0,                      /* 0: offset to rythme */
  33, 0,                      /* 2: offset to ch 1 */
  0,0,0,0,0,0,0,0,0,0,        /* 4: offset to ch 2..6 */

  FM_RVOL + 31, 8,            /* 14: rythem VOL = 8 */
  0x30, TESTLENGTH,           /* 16: B drum */
  0x28, TESTLENGTH,           /* 18: S drum */
  0x28, TESTLENGTH,           /* 20: S drum */
  0x28, TESTLENGTH,           /* 22: S drum */
  0x30, TESTLENGTH,           /* 24: B drum */
  0x28, TESTLENGTH,           /* 26: S drum */
  0x28, TESTLENGTH,           /* 28: S drum */
  0x28, TESTLENGTH,           /* 30: S drum */
  FM_END,                     /* 32: end of rhythm */

  FM_VOL + 8,                 /* 33: ch. 1 Vol 8 */
  FM_INST + 3,                /* 34: Guitar */
  FM_SUSON,
  FM_LEGOFF,
  FM_Q, 6,
  FM_04 + FM_C, TESTLENGTH,
  FM_04 + FM_D, TESTLENGTH,
  FM_04 + FM_E, TESTLENGTH,
  FM_04 + FM_F, TESTLENGTH,
  FM_04 + FM_G, TESTLENGTH,
  FM_04 + FM_A, TESTLENGTH,
  FM_04 + FM_B, TESTLENGTH,
  FM_04 + FM_C, TESTLENGTH,
  FM_END                        /* end of Ch. 1 */
};

VOID main(argc, argv)
int argc;
char **argv;
{
  auto char fmwork[192]; /* address must be >=8000H */
  auto char fmbuf[256]; /* address must be >= 8000H */
  char fmstat;

  if ((fmstat = fmopen(fmwork)) == 1) {
    puts("NO FM-BIOS.");
    exit(1);
  } else if (fmstat == 2) {
    puts("Bad address.");
    exit(1);
  }
  printf("fmopen: address of work area is %04X\n", (unsigned)fmwork);
  memcpy(fmbuf, fmdata, sizeof(fmdata));
  fmstart(fmbuf, (char)TESTTIMES);
  do {
    fputs("Playing. \015", stdout);

  } while(fmtest());
  fputs("\nEnd of play.\n", stdout);
  fmstop();
  fputs("fmstop : complete\n", stdout);
  fmclose();
  fputs("fmclose: complete\n", stdout);
  exit(0);
}

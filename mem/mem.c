#include <stdio.h>
#include <bdosfunc.h>
#include "mem.h"
#pragma nonrec

int main(argc, argv)
int argc;
char **argv;
{
  int i;
  int total_all=0; /* Agregated total, free and reserved memory for all mappers */
  int free_all=0;
  int reserved_all=0;
  int mappers = 0; /* Number of mappers found */
  XREG regs; /* Register set for DOS calls */
  char buffer[3]; /* For the slot printing routine */
  uint vartable; /* Address of the mapper variables table */

  /* Show initial message */

  printf("\n\rMemorytestator (the judgement byte) - MSX-C version\n\r");
  printf("By Konami Man (27-12-2000), Sandy Pleyte (16-07-2016)\n\n\r");

  /* Check for DOS2 */

  regs.bc = (unsigned)_DOSVER;
  callxx(BDOS, &regs);
  if (regs.bc < 0x0220)
  {
     printf("Sorry, this program needs MSX-DOS 2.20 or higher.\n\r");
     exit(1);
  }

  /* Obtain mapper variables table via extended BIOS */

  regs.af = 0x00;
  regs.de = 0x0401;
  callxx(EXTBIO, &regs);
  if (high(regs.af) == 0)
  {
     printf("ERROR: No mapper support routines!\r\n");
     exit(1);
  }
  vartable = regs.hl;

  /* Obtain total, reserved and free memory for all mappers */

  for (i=0; i<16; i++)
  {
     mappers++;
     mapper[i].slot = (*(char*)vartable) & 3;
     mapper[i].subslot = ((*(char*)vartable++) & 12) >> 2;
     mapper[i].mtotal = (*(char*)vartable++);
     mapper[i].mfree = (*(char*)vartable++);
     mapper[i].msystem = (*(char*)vartable++);
     mapper[i].muser = (*(char*)vartable++);

     vartable = vartable+3;
     if ((*(char*)vartable) == 0) break;
  }

  /***** Show information for each mapper *****/

  printf("Mappers found: %d\r\n\n",mappers);
  printf("          Slot            Memory            Reserved            Free\n\r");
  printf("          ----            ------            --------            ----\r\n\n");

  for (i=0; i<mappers; i++)
  {
     /* Is the slot expanded? */
     if ((*(char *)(0xFCC1+mapper[i].slot)) & 128) {
       printf("           %d-%d", mapper[i].slot, mapper[i].subslot);
     } else {
       printf("             %d", mapper[i].slot);
     }
     printf("            %5dK              %5dK          %5dK\n\r",
           mapper[i].mtotal*16,
           (mapper[i].muser + mapper[i].msystem)*16,
           (mapper[i].mfree)*16
           );

     /* Agregated info calculation */
     total_all = total_all+(mapper[i].mtotal)*16;
     free_all = free_all+(mapper[i].mfree)*16;
     reserved_all = reserved_all+(mapper[i].muser + mapper[i].msystem)*16;
  }

  printf("\n\r");     /* Agregated information print */
  printf("Total:                    %5dK              %5dK          %5dK\n\n\r",
       total_all, reserved_all, free_all);

  /* Show information about RAM disk */

  regs.bc = 0xFF68; /* call _RAMD */
  callxx(BDOS, &regs);
  if (high(regs.bc) == 0) {
    printf ("RAM disk not installed\n\r");
  } else {
     printf ("RAM disk size: %dK\n\r", high(regs.bc) * 16);
  }

  /* Show information about MemMan */

  regs.af = 0x2200;
  regs.de = 0x4D1E;
  callxx(EXTBIO, &regs);
  /* check if reg a = 34 + M */
  if (high(regs.af) == 111)  {
    printf ("MemMan version: %d.%d\n\r", high(regs.de), low(regs.de));
  } else {
    printf ("MemMan not installed\n\r");
  }

  /* Show information about TPA size */

  printf ("TPA end address: &H%x - TPA size is %dK\n\r",
        *(uint *)0x006,
        ((*(uint *)0x006)-256)/1024
       );
}
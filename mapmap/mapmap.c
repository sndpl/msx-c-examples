/*
 * mapmap.c : main of mapmap.com
 * on 26 nov. 1990 by  nao-i
 * (C) 1990 N. Ishikawa
 * free to use and copy without warranty
 */
#include <stdio.h>
#include "maplib.h"
#pragma nonrec

int main (argc, argv)
int argc;
char **argv;
{
  maps_t  maps;
  mapv_t *pmapv, *pmapvTmp;
  char cCounter;

  if ((pmapv = mapinit(&maps)) == NULL) {
    fputs("No MSX-DOS2.\n", stderr);
    exit(1);
  }
  printf("Primary mapper slot = %02XH\n", (int)maps.pmap_slot);
  pmapvTmp = pmapv;
  fputs("Slot Total Free System User\n", stdout);
  cCounter = 0;
  do {
    printf(" %02XH%6d%5d%7d%5d\n",
      (int)pmapvTmp->map_slot, (int)pmapvTmp->map_total,
      (int)pmapvTmp->map_free, (int)pmapvTmp->map_system,
      (int)pmapvTmp->map_user);
  } while(++cCounter < MAX_MAP_SLOT && (++pmapvTmp)->map_slot != 0);
  return 0;
}

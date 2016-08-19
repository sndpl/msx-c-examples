#include <stdio.h>
#include "mbdrv.h"
#include "mbload.h"

char main(argc, argv)
int argc;
char *argv[];
{
    char chips = 0;
    puts("LOAD ALESTE 1.MBM\n");
    mbmload("aleste1");
    puts("PLAYING ALESTE 1.MBM\n");
    chips = mbchip();
    switch (chips) {
        case 0:
            puts("IN MSX-AUDIO\n");
            break;
        case 1:
            puts("IN MSX-MUSIC\n");
            break;
        case 2:
            puts("IN STEREO\n");
            break;
    }

    mbplay();
    getchar();
    puts("STOPPED ALESTE1.MBM\n");
    mbstop();

}

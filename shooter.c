#include <stdio.h>
#include <glib.h>
#include <defs.h>

/* Player data */
struct player_struct {
  TINY x;
  TINY y;
  TINY vx, vy;
  bool trig;
};

/* Bullet data */
struct bullet_struct {
  TINY x;
  TINY y;
  char sp;
  bool active;
};

typedef struct player_struct player_t;

typedef struct bullet_struct bullet_t;
typedef struct bullet_struct rocket_t;

#define MAX_ROCKET	1
#define MAX_BULLET	3

VOID move_player(player_t *player)
{

  direction = gtstck((TINY)0);
  switch (direction) {
    case 1:
      player.y--;
      break;
    case 2:
      player.y--;
      player.x++;
      break;
    case 3:
      player.x++;
      break;
    case 4:
      player.y++;
      player.x++;
      break;
    case 5:
      player.y++;
      break;
    case 6:
      player.y++;
      player.x--;
      break;
    case 7:
      player.x--;
      break;
    case 8:
      player.x--;
      player.y--;
      break;
  }
  /* Check bounderies */
  if (player.x < 0) { player.x = 0; }
  if (player.y < 0) { player.y = 0; }
  if (player.x>192) { player.x = 192; }
  if (player.y>212) { player.y = 212; }

  putspr((TINY)0,player.x,player.y,(TINY)WHITE,(TINY)0);
}

VOID main()
{
  player_t player;
  rocket_t rockets[MAX_ROCKET], *rckt;
  bullet_t bullets[MAX_BULLET], *blt;

  char sprit[8], colortb[3];
  int x1;
  int y1;
  int direction;
  int timer;

  colortb[0] = FORCLR;
  colortb[1] = BAKCLR;
  colortb[2] = BDRCLR;

  printf("PRESS ENTER TO START");
  getchar();

  color ((TINY) WHITE, (TINY)BLACK, (TINY)DGREEN);
  ginit();
  screen((TINY)2);
  cls();
  inispr((TINY)1);

  sprit[0]= 0x7e;  sprit[1]= 0x81;  sprit[2]= 0xa5;  sprit[3]= 0x81;
  sprit[4]= 0x81;  sprit[5]= 0xbd;  sprit[6]= 0x81;  sprit[7]= 0x7e;

  sprite((TINY)0, sprit);
  x1=100;
  y1=100;

  player.x = 128;
  player.y = 96;
  player.vx = player.vy = player.trig = 0;

  timer = JIFFY;
  while(gttrig((TINY)0) == false) {

    move_player(&player);

    while (timer == JIFFY) {
      /* Wait for intterupt */
    }
    timer = JIFFY;

  }

  color (colortb[0], colortb[1], colortb[2]);
  screen ((TINY)0);
  cls();
}


      

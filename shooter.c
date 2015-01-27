#include <stdio.h>
#include <glib.h>
#include "defs.h"

/* Player data */
struct player_struct {
  int x;
  int y;
  int vx, vy;
  bool trig;
};

/* Bullet data */
struct bullet_struct {
  int x;
  int y;
  char sp;
  bool active;
};

typedef struct player_struct player_t;
typedef struct bullet_struct bullet_t;

#define MAX_BULLET	3

bullet_t* alloc_bullet(array)
bullet_t *array;
{
  int c;
  for (c = 0; c < MAX_BULLET; c++) {
    if (!array->active) {
      array->active = true;
      array->sp = 10 + c;
      return array;
    }
    ++array;
  }
  return NULL;
}

VOID free_bullet(r)
bullet_t *r;
{
  r->active = false;
  putspr(r->sp,0,192,(TINY)0,(TINY)1);
}

VOID initialize_bullets(array)
bullet_t *array;
{
  int c;
  for (c = 0; c < MAX_BULLET; c++) {
    free_bullet(array++);
  }
}

VOID move_bullets(array)
bullet_t *array;
{
  int c;
  for (c = 0; c < MAX_BULLET; c++) {
    if (array->active) {
      if ((array->y -= 7) < 0) {
        free_bullet(array);
      }
      else {
        putspr(array->sp,array->x,array->y,(TINY)LRED,(TINY)1);
      }
    }
    ++array;
  }
}

VOID move_player(player)
player_t *player;
{
  int direction;

  direction = gtstck((TINY)0);
  switch (direction) {
    case 1:
      player->y--;
      break;
    case 2:
      player->y--;
      player->x++;
      break;
    case 3:
      player->x++;
      break;
    case 4:
      player->y++;
      player->x++;
      break;
    case 5:
      player->y++;
      break;
    case 6:
      player->y++;
      player->x--;
      break;
    case 7:
      player->x--;
      break;
    case 8:
      player->x--;
      player->y--;
      break;
  }

  /* Check bounderies */
  if (player->x < 0) { player->x = 0; }
  if (player->y < 0) { player->y = 0; }
  if (player->x > 192) { player->x = 192; }
  if (player->y > 212) { player->y = 212; }

  putspr((TINY)0,player->x,player->y,(TINY)LYELLOW,(TINY)0);
}


VOID main()
{
  player_t player;
  bullet_t bullets[MAX_BULLET], *blt;

  char play_spr[8], bull_spr[8], colortb[3];
  int timer;

  colortb[0] = FORCLR;
  colortb[1] = BAKCLR;
  colortb[2] = BDRCLR;

/*
  printf("PRESS ENTER TO START");
  getchar();
  */

  color ((TINY) WHITE, (TINY)BLACK, (TINY)DGREEN);
  ginit();
  screen((TINY)2);
  cls();
  inispr((TINY)1);


  bull_spr[0]=0x00;bull_spr[1]=0x00;bull_spr[2]=0x00;bull_spr[3]=0x00;
  bull_spr[4]=0x81;bull_spr[5]=0x81;bull_spr[6]=0x81;bull_spr[7]=0x81;

  play_spr[0]= 0x00; play_spr[1]= 0x00; play_spr[2]= 0x81; play_spr[3]= 0x81;
  play_spr[4]= 0xc3; play_spr[5]= 0xe7; play_spr[6]= 0x7E; play_spr[7]= 0x3c;

  sprite((TINY)0, play_spr);
  sprite((TINY)1, bull_spr);

  player.x = 128;
  player.y = 96;
  player.vx = player.vy = player.trig = 0;

  initialize_bullets(bullets);

  timer = JIFFY;

  for(;;) {

    move_player(&player);
    player.trig |= gttrig((TINY)0);

    /*
    if (player.trig && (blt = alloc_bullet(bullets))) {
      blt->x = player.x + 4;
      blt->y = player.y - 8;
      player.trig = false;
    }
    */

    move_bullets(bullets);


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
                        

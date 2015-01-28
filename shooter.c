#include <stdio.h>
#include <glib.h>
#include "defs.h"

/*
play "S1M5000A"

game over
sound 6,55
sound 7,7
play "S1M200000L2A"
*/

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

struct star_struct {
  int x;
  int y;
  int sp;
};

struct enemy_struct {
  int x;
  int y;
  int sp;
};

typedef struct player_struct player_t;
typedef struct bullet_struct bullet_t;
typedef struct star_struct star_t;
typedef struct enemy_struct enemy_t;

#define MAX_BULLET	1
#define MAX_STARS 15

/* Helper functions */

/**
 * Check if var is between min, max
 */
int range(var, min, max)
int var;
int min;
int max;
{
  if (var < min) {
    var = min;
  }
  else if (var > max) {
      var = max;
  }
  return var;
}

/**
 * Check if spacebar is pressed
 */
char space() {
  char c;
  c=(inp(0xaa)&0xf0)|8;
  outp(0xaa,c);
  c=inp(0xa9)&1;

  return (1-c);
}

/**
 * Locate + Print
 */
VOID g_print(x,y,c,a)
int x;
int y;
char *c;
int a;
{
  int i;
  glocate(x,y);
  for (i=0; i < a; i++) {
    grpprt(c[i], (TINY)0);
  }
}



/* Bullet functions */

/**
 * Allocate bullet
 */
bullet_t *alloc_bullet(array)
bullet_t *array;
{
  int c;
  for (c = 0; c < MAX_BULLET; c++) {
    if (!array->active) {
      array->active = 1;
      array->sp = 10 + c;
      return array;
    }
  }
  ++array;
  return NULL;
}

/**
 * Free bullet, remove from screen
 */
VOID free_bullet(r)
bullet_t *r;
{
  r->active = false;
  putspr(r->sp,0,192,(TINY)0,(TINY)1);
}

/**
 * Initialize bullets
 */
VOID init_bullets(array)
bullet_t *array;
{
  int c;
  for (c = 0; c < MAX_BULLET; c++) {
    free_bullet(array++);
  }
}

/**
 * Move the bullets or clear them
 * when they reach the end of the screen
 */
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

/* Player functions */

/**
 * Initialze player
 */
VOID init_player(player)
player_t *player;
{
  player->x = 128;
  player->y = 96;
  player->vx = player->vy = player->trig = 0;
}

/**
 * Read stick 0 and move player spriter
 */
VOID move_player(player)
player_t *player;
{
  int direction;
  int ax, ay;
  ax = 0;
  ay = 0;

  direction = gtstck((TINY)0);
  switch (direction) {
    case 1:
      ay = -1;
      break;
    case 2:
      ay = -1;
      ax = 1;
      break;
    case 3:
      ax = 1;
      break;
    case 4:
      ay = 1;
      ax = 1;
      break;
    case 5:
      ay = 1;
      break;
    case 6:
      ay = 1;
      ax = -1;
      break;
    case 7:
      ax = -1;
      break;
    case 8:
      ax = -1;
      ay = -1;
      break;
  }

  if (ax != 0 || ay != 0) {
    if (ax != 0) {
      player->vx += ax;
    } else {
      player->vx = 0;
    }

    if (ay != 0) {
      player->vy += ay;
    } else {
      player->vy = 0;
    }

    player->vx = range(player->vx, -3, 3);
    player->vy = range(player->vy, -3, 3);

    player->x += player->vx;
    player->y += player->vy;

    /* Check bounderies */
    if (player->x < 0) { player->x = 0; }
    if (player->y < 0) { player->y = 0; }
    if (player->x > 192) { player->x = 192; }
    if (player->y > 175) { player->y = 175; }

    putspr((TINY)0,player->x,player->y,(TINY)LYELLOW,(TINY)0);
  }
}

/* Star functions */

/**
 * Initialze the stars array with random values
 */
VOID init_stars(array)
star_t *array;
{
  int c;
  for(c = 0; c < MAX_STARS ; c++) {
    array->x = rnd(192);
    array->y = rnd(175);
    array->sp = rnd(2);
    if (array->sp == 0) {
      array->sp = 1;
    }
    ++array;
  }
}

/**
 * Move the stars by first paint the pixel black
 * calculate new value and paint it in white
 */
VOID move_stars(array)
star_t *array;
{
  int c;
  for (c = 0; c < MAX_STARS; c++) {
    pset(array->x, array->y, (TINY)0, (TINY)0);
    array->y += array->sp;
    if (array->y > 175) {
      array->y = 0;
      /* array->x = rnd(192); */
    }
    pset(array->x, array->y, (TINY)15, (TINY)0);
    ++array;
  }
}


/**
 * Show intro screen
 */
VOID show_intro()
{
  char *start = "PRESS SPACE TO START";
  disscr();
  cls();
  g_print(50,50,start,20);
  enascr();
  while(!space()) {}
  cls();
}


VOID main()
{
  player_t player;
  bullet_t bullets[MAX_BULLET], *blt;
  star_t stars[MAX_STARS];
  enemy_t enemies[2];

  char play_spr[8], bull_spr[8], colortb[3];
  int timer;
  int star_timer;
  int score;
  int hiscore;

  score = 0;
  hiscore=0;

  color ((TINY) WHITE, (TINY)BLACK, (TINY)BLACK);
  ginit();
  screen((TINY)2);
  inispr((TINY)1);

  show_intro();

  bull_spr[0]=0x00;bull_spr[1]=0x00;bull_spr[2]=0x00;bull_spr[3]=0x00;
  bull_spr[4]=0x81;bull_spr[5]=0x81;bull_spr[6]=0x81;bull_spr[7]=0x81;

  play_spr[0]= 0x00; play_spr[1]= 0x00; play_spr[2]= 0x81; play_spr[3]= 0x81;
  play_spr[4]= 0xc3; play_spr[5]= 0xe7; play_spr[6]= 0x7E; play_spr[7]= 0x3c;

  sprite((TINY)0, play_spr);
  sprite((TINY)1, bull_spr);

  init_player(&player);
  init_bullets(bullets);
  init_stars(stars);

  timer = JIFFY;
  for(;;) {

    move_stars(stars);
    move_player(&player);
    player.trig |= gttrig((TINY)0);


    if (player.trig && (blt = alloc_bullet(bullets))) {
      blt->x = player.x;
      blt->y = player.y - 8;
      player.trig = false;
      sound((TINY)6, (TINY)55);
      sound((TINY)7, (TINY)7);
    }

    move_bullets(bullets);

    /* Wait for interupt */
    while (timer == JIFFY) {}
    timer = JIFFY;

  }

  color (colortb[0], colortb[1], colortb[2]);
  screen ((TINY)0);
  cls();
}


      
                        

/**
 * First attempt to build a shoot-em up game.
 */
#pragma nonrec
#include <stdio.h>
#include <glib.h>
#include "defs.h"

/*
Shoot
play "S1M5000A"

game over
play "S1M200000L2A"
*/

/* Player data */
struct player_struct {
  int x;
  int y;
  int vx, vy;
  bool trig;
  char sp;
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
  char sp;
};

struct enemy_struct {
  int x;
  int y;
  char sp;
  bool active;
  int age;
  int range;
};

typedef struct player_struct player_t;
typedef struct bullet_struct bullet_t;
typedef struct star_struct star_t;
typedef struct enemy_struct enemy_t;

#define MAX_BULLET	3
#define MAX_STARS 15
#define MAX_ENEMIES 5
#define PI 3.14159265358979323846



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
VOID g_print(x,y,text)
int x;
int y;
char *text;
{
  glocate(x,y);
  while(*text) {
    grpprt(*text, (TINY)0);
    text++;
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
      array->sp = 1 + c;
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

/* Enenmies functions */

VOID init_enemies(array)
enemy_t *array;
{
  int c;
  for(c = 0; c < MAX_ENEMIES; c++) {
    array->x = rnd(172);
    if (array->x < 20) {
      array->x = 20;
    }
    array->y = -rnd(50);
    array->sp = 5 + c;
    array->active = true;
    array->age = rnd(30);
    ++array;
  }
}

VOID draw_enemies(array)
enemy_t *array;
{
  int c;
  for(c = 0; c < MAX_ENEMIES; c++) {
    putspr(array->sp, array->x, array->y, (TINY)DRED,(TINY)0);
    ++array;
  }
}

VOID move_enemies(array)
enemy_t *array;
{
  int c;
  static int sin[30] = {1,1,1,2,2,3,2,2,1,1,1,0,0,0,0,-1,-1,-1,-2,-2,-3,-2,-2,-1,-1,-1,0,0,0,0 };

  for(c = 0; c < MAX_ENEMIES; c++) {
    if (array->active) {
      array->x += sin[array->age];
      array->y++;
      if (array->x > 192) {
        array->x = 192;
      }
      if (array->y > 175) {
        array->y = 0;
        array->x = rnd(172);
      }

      array->age++;
      if (array->age > 29) {
        array->age = 0;
      }
    }
    putspr(array->sp, array->x, array->y, (TINY)DRED,(TINY)0);
    ++array;
  }
}

VOID kill_enemy(enemy)
enemy_t *enemy;
{
  enemy->active = false;
  putspr(enemy->sp, 0, 192, (TINY)0,(TINY)0);
}

int player_collision(player, enemy)
player_t *player;
enemy_t *enemy;
{
  int c, dx, dy;
  for(c = 0; c < MAX_ENEMIES; c++) {
    dx = player->x - enemy->x;
    dy = player->y - enemy->y;
    if (dx < 0) {
      dx = abs(dx);
    }
    if (dy < 0) {
      dy = abs(dy);
    }
    if (dx < 12 &&
        dy < 12 &&
        enemy->active == true) {

      return 1;
    }
    ++enemy;
  }
  return 0;
}

VOID bullet_collision(bullet, enemy)
bullet_t *bullet;
enemy_t *enemy;
{
  int c, d, dx, dy;
  for (c = 0; c < MAX_BULLET ; c++) {
    for(d = 0; d < MAX_ENEMIES; d++) {
      if (enemy->active == true) {
        dx = bullet->x - enemy->x;
        dy = bullet->y - enemy->y;
        if (dx < 0) {
          dx = abs(dx);
        }
        if (dy < 0) {
          dy = abs(dy);
        }
        if (dx < 12 &&
            dy < 12
        ) {
          kill_enemy(enemy);
          free_bullet(bullet);
        }
      }
      ++enemy;
    }
    ++bullet;
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
  player->sp = 0;
}

/**
 * Read stick 0 and move player spriter
 */
VOID move_player(player)
player_t *player;
{
  int direction;
  int ax, ay;
  char *text2;
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
  }
  putspr(player->sp,player->x,player->y,(TINY)LYELLOW,(TINY)0);
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
  static char *text1 = "PRESS SPACE TO START";

  disscr();
  screen((TINY)2);
  cls();
  g_print(50,50,text1);
  enascr();
  while(!space()) {}
  cls();
}

int enemies_left(enemy)
enemy_t *enemy;
{
  int c, left;
  left = 0;
  for(c=0; c < MAX_ENEMIES; c++) {
    if (enemy->active == true) {
      left++;
    }
    enemy++;
  }
  return left;
}


VOID game_over()
{
  static char *text2 = "GAME OVER";
  int i;
  cls();
  g_print(50,50,text2);
}

VOID you_won()
{
  static char *text3 = "YOU WON!!";
  int i;
  cls();
  g_print(50,50,text3);
}

VOID wait(sec)
int sec;
{
  int timer;
  timer = JIFFY + (sec * 10);
  while (timer > JIFFY) {}
}


VOID main()
{
  player_t player;
  bullet_t bullets[MAX_BULLET], *blt;
  star_t stars[MAX_STARS];
  enemy_t enemies[MAX_ENEMIES];

  static char play_spr[8] = { 0x00, 0x00, 0x81, 0x81, 0xc3, 0xe7, 0x7e, 0x3c };
  static char bull_spr[8] = { 0x00, 0x00, 0x00, 0x00, 0x81, 0x81, 0x81, 0x81 };
  static int timer = 0;
  static int star_timer = 0;
  static int score = 0;
  static int hiscore = 0;
  static int go = 0;

  score = 0;
  hiscore=0;

  color ((TINY) WHITE, (TINY)BLACK, (TINY)BLACK);
  ginit();
  gicini();
  screen((TINY)2);
  inispr((TINY)1);

  sprite((TINY)0, play_spr);
  sprite((TINY)1, bull_spr);


  for(;;) {


    show_intro();

    init_player(&player);
    init_bullets(bullets);
    init_stars(stars);
    init_enemies(enemies);
    draw_enemies(enemies);

    timer = JIFFY;
    go = 0;
    while(go == 0) {

      move_stars(stars);
      move_player(&player);
      player.trig |= gttrig((TINY)0);

      if (player.trig && (blt = alloc_bullet(bullets))) {
        blt->x = player.x;
        blt->y = player.y - 8;
        player.trig = false;
      }

      move_bullets(bullets);
      move_enemies(enemies);

      go = player_collision(&player, enemies);
      bullet_collision(bullets, enemies);
      if (enemies_left(enemies) == 0) {
        go = 2;
      }

      /* Wait for interupt */
      while (timer == JIFFY) {}
      timer = JIFFY;
    }
    if (go == 2) {
      you_won();
    } else {
      game_over();
    }
    wait(2);
  }
}


      
                        
                  

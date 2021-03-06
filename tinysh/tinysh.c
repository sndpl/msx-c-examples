#define DOS2 0

#include <stdio.h>
#include <bdosfunc.h>

VOID screen(),wtvdp(),wtvram(),keyoff();
char stick(),strig();

#define DIRMAX 112
#define BLTBL 0x800

int RG;

char *MES0 = "                                             ";
char *MES1 = "(CR):Go or Chdir (A H):Drive Change (ESC):End";
char *CLRSTR = "\x0c";

VOID putstr(pt)
char *pt;
{
  while(*pt != '\0')
    bdos((char)2,*pt++,0);  /* 02h console output */
}

VOID blink(x,y,f)
char x,y,f;
{
  int add;

  add = BLTBL + (int)x / 8 + (int)y * 10;
  wtvram(add,f);
  wtvram(add + 1,f);
}

char keyscan()
{
  static char stbuf = 0, t1buf = 0, t3buf = 0;
  static char cnv[] = {0,30,0,28,0,31,0,29,0};
  char s,sc;

  sc = 0;
  while(bdos((char)0x0b,0,0) == 0 && sc == 0) { /* 0x0b Console status */
    if((s = stick((char)1)) != stbuf) {
      stbuf = s;
      if (s != 0)
        sc = cnv[s];

    } else if((s = strig((char)1)) != t1buf) {
      t1buf = s;
      if (s != 0)
        sc = 27;
    } else if((s = strig((char)3)) != t3buf) {
      t3buf = s;
      if(s != 0)
        sc = 13;
    }
  }
  if(sc == 0)
    sc = bdos((char)8,0,0);  /* Console input without echo */

  return sc;
}

VOID blclr()
{
  int i;
  for(i = 0; i < 260;i++)
    wtvram(BLTBL + i,0);
}

VOID sini()
{
  int i;
  screen((char)0,(char)80);
  wtvdp((char)7,(char)0xf0);
  putstr("\x1bx5");
  wtvdp((char)12,(char)0xf4);
  wtvdp((char)13,(char)0x10);
  blclr();
}

VOID locate(x,y)
char x,y;
{
  /* ESC Y (Positioning Cursor) */
  static char pos [] = {27,'Y',0,0,0};
  pos[2] = y + 32;
  pos[3] = x + 32;
  putstr(pos);
}

int chklog(a)
char a;
{
  return((1 << a) & bdosh((char)0x18,0,0));  /* 0x18h get login vector */
}

char chkcom(p1)
char *p1;
{
  static char *chkstr = "COM";
  char f, *p2;
  p2 = chkstr;
  while((*p1 != '.') && (*p1 != '\0'))
    p1++;
  if(*p1++ == '\0')
    return(0);

  f = 1;
  while(*p2 != '\0') {
    if(*p1 != *p2) {
      f = 0;
      break;
    }
    p1++,p2++;
  }
  return(f);
}

#ifdef DOS1

typedef struct {
  char drive;
  char name[11];
  char dummy1[11];
  int time;
  int date;
  int dummy2;
  int size_l;
  int size_h;
} DIRENT;

typedef struct {
  char drive;
  char name[11];
  char dummy[25];
} FCBENT;

DIRENT *DIRS, *gdp;

int gtdir()
{
  static FCBENT f1;
  static char *sfn = "????????COM";
  char buf[3];
  char i,j;
  int e;
  DIRENT *dp;

  for(i = 0;i < sizeof(FCBENT);i++)
    *((char*)((int)(&f1) + i)) = 0;
  strcpy(f1.name,sfn);

  e = 0;

  dp = DIRS = (DIRENT*)(malloc(sizeof(DIRENT) * DIRMAX));
  bdos(_SETDTA,(int)dp,0);
  if(bdos(_SFIRST,(int)(&f1),0) == 255)
    return(0);

  do {
    for(i = 0; i < 3; i++)
      buf[i] = dp->name[8 + i];
    dp->name[8] = ' ';
    j = 0;
    while(dp->name[j++] != ' ')

    dp->name[j - 1] = '.';
    for(i = 0;i < 3;i++)
      dp->name[j++] = buf[i];

    dp->name[j] = '\0';
    e++, dp++;
    bdos(_SETDTA,(int)dp, 0);
  } while(e != 200 && bdos(_SNEXT,0,0) == 0);
  return(e);
}
#endif
#ifdef DOS2

typedef struct {
  char f1;
  char name[13];
  char type;
  int time;
  int date;
  int dummy;
  int size_l;
  int size_h;
} FIBSUB;

typedef struct {
  FIBSUB fibs;
  char dummy[39];
} FIBENT;

FIBENT fres;
FIBSUB *DIRS,*gdp;

int gtdir()
{
  char ffirst(), fnext();
  char i, *p1, *p2;
  int e;
  FIBSUB *fp;

  e = 0;
  fp = DIRS = (FIBSUB*)(malloc(sizeof(FIBSUB) * (DIRMAX -1) + sizeof(FIBENT)));

  if (fp == NULL) {
    printf("memory error");
    exit();
  }
  if(ffirst("*.*",&fres,0x10) != 0)
    return(0);

  do{
    if((fres.fibs.type & 0x10) == 0x10
      || chkcom(fres.fibs.name) == 1) {
        p1 = (char*)fp,p2 = (char*)(&fres);
        for(i = 0; i< sizeof(FIBSUB);i++)
            *p1++ = *p2++;
        e++;
        fp++;
      }
  } while(e != DIRMAX && fnext() == 0);
  return(e);
}
char chkdir(et)
char et;
{
  if((DIRS[et].type & 0x10) != 0x10)
    return(0);
  return(1);
}
#endif

go(et)
char et;
{
  char *p1,*comline[2];
  locate((char)0,(char)23);
  putstr(MES0);
  locate((char)0,(char)23);
  putstr("Input Command Line:>");

  p1 = (char*)0x0080;
  *p1 = 125;
  bdos((char)0x0a, (int)p1, 0); /* 0x0a Buffered line input */
  *(p1 + *(p1 + 1) +  2) = '\0';

  comline[0] = p1 + 2;
  comline[1] = NULL;
  putstr(CLRSTR);
  blclr();
  execv(DIRS[et].name,comline);
}

VOID mkfnm(buf, pt, type)
char *buf, *pt,type;
{
  char i,k;

  for(i = 0; i< 16;i++)
    buf[i] = ' ';
  buf[i] = '\0';
  i = 2;
  if(*pt == '.') {
    while(*pt != '\0')
      buf[i++] = *pt++;
  } else {
    i = 2;
    while(*pt != '.' && *pt != '\0')
      buf[i++] = *pt++;
    if(*pt != '\0') {
      pt++;
      for(k = 11; k < 14; k++)
        buf[k] = *pt++;
    }
  }
#ifdef DOS2
  if((type & 0x10) == 0x10) {
    buf[1] = '<';
    buf[14] = '>';
  }
#endif
}

ptdir()
{
  static char buf[17];
  char i0,j;

  blclr();
  putstr(CLRSTR);
  gdp = DIRS;
  /* locate((char)0,(char)23); */
  for(i0 = 0;i0 < 23;i0++) {
    for(j = 0; j < 5; j++) {
      if(i0 * 5 + j >= RG)
        goto ESC;

#ifdef DOS1
      mkfnm(buf, gdp->name, 0);
#endif
#ifdef DOS2
      mkfnm(buf, gdp->name, gdp->type);
#endif
      locate(j * 16, i0);
      putstr(buf);
      gdp++;
    }
  }

ESC: locate((char)0,(char)23);
     putstr(MES1);
}

VOID move()
{
  char xpos, ypos,x0pos,y0pos,et,ks;

  xpos = ypos = x0pos = y0pos = 0;
  for(;;) {
    blink(xpos,ypos,(char)255);
    if((xpos != x0pos) || (ypos != y0pos))
      blink(x0pos,y0pos,(char)0);

    x0pos = xpos, y0pos = ypos;
    et = xpos / 16 + ypos * 5;
    switch(ks = toupper(keyscan())) {
      case 28:  xpos += 16;           /* right */
                if(xpos == 80)
                  xpos = 0, ypos ++;
                if(ypos * 5 + (xpos / 16) > RG -1)
                  xpos = ypos = 0;
                break;
      case 29:  xpos -= 16;          /* left */
                if(xpos == 240)
                  xpos = 64,ypos--;
                if(ypos == 255) {
                  ypos = (RG -1) / 5;
                  xpos = (RG -1) % 5 * 16;
                }
                break;
      case 30:  if(--ypos == 255) {
                  ypos = (RG -1) /5;
                  if(xpos / 16 > (RG -1) % 5)
                    --ypos;
                }
                break;
      case 31:  if(++ypos > (RG -1) / 5)
                  ypos = 0;
                else if ((ypos > (RG -1) / 5 -1)
                      &&(xpos / 16 > (RG -1) %5))
                        ypos = 0;
                break;
      case 27:  putstr("\xc");     /* Escape */
                blclr();
                bdos((char)0,0,0);
                break;
#ifdef DOS1
      case 13:  go(et);            /* Enter */
                ptdir();
                xpos = ypos = x0pos = y0pos = 0;
                break;
#endif
#ifdef DOS2
      case 13:  if(chkdir(et) != 0) {   /* Enter */
                  chdir(DIRS[et].name);
                  free((char*)DIRS);
                  RG = gtdir();
                } else {
                  go(et);
                }
                ptdir();
                xpos = ypos = x0pos = y0pos = 0;
                break;
#endif
    }
    if('A' <= ks && ks <= 'H') {
      if(chklog(ks - 'A') != 0) {
        putstr(CLRSTR);
        blclr();
        bdos((char)0x0e,ks - 'A',0);
        free((char*)DIRS);
        RG = gtdir();
        ptdir();
        xpos = ypos = x0pos = y0pos = 0;
      }
    }
  }
}

main()
{
  sini();
  RG = gtdir();
  ptdir();

  move();
}
  

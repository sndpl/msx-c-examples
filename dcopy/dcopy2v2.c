/*
 * dcopy2.c
 * (C) 1991 N. Ishikawa
 * Free to copy and use without warranty
 * on 20. Nov 1991 by nao-i@ascii.co.jp
 * on 23. Dec 1991 by nao-i@ascii.co.jp supporting mapped RAM,VRAM
 */
#define DEBUG 0
#include "dcopy.h"
#pragma nonrec

#ifdef DEBUG
#define DPUTCHAR(c)            {putchar(c); fflush(stdout);}
#define DONE()                 printf(" done. \n")
#define DPRINTF1(p1)           printf(p1)
#define DPRINTF2(p1,p2)        printf(p1,p2)
#define DPRINTF3(p1,p2,p3)     printf(p1,p2,p3)
#define DPRINTF4(p1,p2,p3,p4)  printf(p1,p2,p3,p4)
#else /* DEBUG */
#define DPUTCHAR(c)
#define DONE()
#define DPRINTF1(p1)
#define DPRINTF2(p1,p2)
#define DPRINTF3(p1,p2,p3)
#define DPRINTF4(p1,p2,p3,p4)
#endif /* DEBUG */
#define STATICF static

typedef struct {
  char map_slot;
  char map_total;
  char map_free;
  char map_system;
  char map_user;
  char map_reserved[3];
} mapv_t;

typedef struct {
  char pmap_slot;
  char pmap_total;
  char pmap_free;
  int (*map_jump)(); /* Address of jump table */
} maps_t;
extern STATUS _seek();

#define MAX_MAP_SLOT 9
#define MAX_MAP_SEG 64
#define MAP_SEG_KB 16

/*
 * BIOS, system work area, extended BIOS
 */
#define CALSLT        0x001c
#define BIOS_LDIRMV   0x0059
#define BIOS_LDIRVM   0x005c
#define MODE          0xfafc
#define LINLEN        0xf3b0
#define EXPTBL        0xfcc1
#define EXTBIOS       0xffca
#define ALL_SEG       0
#define FRE_SEG       3
#define RD_SEG        6
#define WR_SEG        9

/*
 * Flags in flags register
 */
#define FLAG_S  0x80
#define FLAG_Z  0x40
#define FLAG_H  0x10
#define FLAG_P  0x04
#define FLAG_N  0x02
#define FLAG_C  0x01

/*
 * variables
 */
static FD     fdWork;
static char   acWorkName[] = "H:DCOPY.$$$";
static TINY   tMapSegs, atMapSeg[64];
static int    (*jump_table)();    /* address of jump table */
static uint   jump_rd_seg, jump_wr_seg, uiIY;
static XREG   regs;
static char   cModeSave;

/**
 * work area map
 *
 * +--------------------+ uiDiskStart = 0
 * | RAM disk           |
 * +--------------------+ uiDiskEnd = uiMapStart
 * | mapped RAM         |
 * +--------------------+ uiMapEnd = uiVRAMStart
 * | VRAM               |
 * +--------------------+ uiVRAMEnd
 */
static uint uiDiskEnd, uiMapEnd, uiVRAMEnd, uiWorkOff, uiVRAMOff;
#define uiDiskStart 0;
#define uiMapStart uiDiskEnd
#define uiVRAMStart uiMapEnd

/**
 * get Kanji mode
 */
#define getKMode()    calla(EXTBIOS, 0, 0, 0, 0x1100)

/**
 * make sure C100H <= SP
 */
STATICF recursive VOID check_stack()
{
  auto char acDummy[1];
#ifdef DEBUG_DONE
  printf("check_stack() : 04X\n", (unsigned)acDummy);
#endif /* DEBUG */
  if ((unsigned)acDummy < 0xc100) {
    fputs("Stack overflow. \n", stderr);
    exit(1);
  }
}

/**
 * Initialize memory mapper
 */
mapv_t *mapinit(pmaps)
maps_t *pmaps;
{
  check_stack();
  memset(&regs, (tiny)0, sizeof(regs));
  regs.de = 0x0402;
  callxx(EXTBIOS, &regs);
  if ((regs.af & 0xff00) == 0) {
    return NULL;
  }
  pmaps->pmap_slot = regs.bc >> 8;
  pmaps->pmap_total = regs.af >> 8;
  pmaps->pmap_free = regs.bc & 0x00ff;
  pmaps->map_jump = (int (*)())regs.hl;
  return (mapv_t*)call(EXTBIOS, 0, 0, 0, 0x0401);
}

/**
 * make temporary file and return size in K bytes
 */
uint dcOpen()
{
  struct  diskfree_t diskspace;
  ushort  usSectorSize;
  maps_t  maps;
  mapv_t  *pmapv;
  uint    uiKB;

  check_stack();
  uiIY = (uint)*((char*)EXPTBL) << 8;
  DPRINTF2("dcOpen() : IY to call BIOS = %04x\n", uiIY);
  /* prepare RAM disk */
  memset(&regs, (tiny)0, sizeof(regs)); /* for fale save */
  regs.bc = 0xff00 | _RAMD;
  callxx((unsigned)5, &regs);
  if ((regs.bc & 0xff00) == 0) {
    fputs("No RAM disk.\n", stderr);
    exit(1);
  } else if ((fdWork = creat(acWorkName)) < 0) {
    fprintf(stderr, "Can not create \"%s\".\n", acWorkName);
    exit(1);
  } else if (close(fdWork) < 0 || (fdWork = open(acWorkName, O_RDWR)) < 0) {
    fprintf(stderr, "Can not open \"%s\".\n", acWorkName);
    exit(1);
  } else if (_dos_getdiskfree((unsigned)('H' - 'A' + 1), &diskspace)) {
    close(fdWork);
    fputs("_dos_getdiskfree() : error.\n", stderr);
    exit(1);
  }
  usSectorSize = diskspace.bytes_per_sector;
  if (usSectorSize < 1024) {
    uiKB = diskspace.avail_clusters * diskspace.sectors_per_cluster
      * (usSectorSize / 128) / 8;
  } else {
    uiKB = diskspace.avail_clusters * diskspace.sectors_per_cluster
      * (usSectorSize / 1024);
  }
  uiDiskEnd = uiKB;
  /* prepare mapped RAM */
  if ((pmapv =  mapinit(&maps)) == NULL) {
    fputs("No DOS2.\n", stderr);
    exit(1);
  }
  jump_table = maps.map_jump;
  jump_rd_seg = (unsigned)jump_table + RD_SEG;
  jump_wr_seg = (unsigned)jump_table + WR_SEG;
  uiKB = 0;
  tMapSegs = 0;
  while (1) {
    memset(&regs, (tiny)0, sizeof(regs));
    callxx((unsigned)jump_table + ALL_SEG, &regs);
    if ((regs.af & FLAG_C) == 0) {
      atMapSeg[tMapSegs] = regs.af >> 8;
      tMapSegs++;
      uiKB += MAP_SEG_KB;
      DPRINTF3("dcOpen() : segment %02XH allocated. %d \n", regs.af >> 8, regs.af & FLAG_C);
    } else {
      break;
    }
  }
  printf("Check LINLEN");
  uiMapEnd = uiDiskEnd + uiKB;
  /* prepare VRAM */
  uiKB = 0;
  if (getKMode() == 0) {
    TINY tWidth;
    if ((tWidth = *(char*)LINLEN) <= 32) {
      uiVRAMOff = 0x4000; /* SCREEN 1 */
      uiKB = 48;
    } else if (tWidth <= 40) {
      uiVRAMOff = 0x1000; /* SCREEN 0 : WIDTH 40 */
    } else {
      uiVRAMOff = 0x1800; /* SCREEN 0 : WIDTH 80 */
      uiKB = 58;
    }
  }
  *(char*)MODE = (cModeSave = *(char*)MODE) | 8;
  uiVRAMEnd = uiMapEnd + uiKB;
  printf("dcOpen() : Disk End = %u, Map End = %u, VRAM End = %u \n",
    (uint)uiDiskEnd, (uint)uiMapEnd, (uint)uiVRAMEnd);
  return uiVRAMEnd;
}

/**
 * erase temporary file
 */
VOID dcClose()
{
  TINY *ptTmp;
  check_stack();
  *(char*)MODE = cModeSave;
  memset(&regs, (tiny)0, sizeof(regs));
  /* DPRINTF2("dcClose() : tMapSegs = %d\n", (int)tMapSegs); */
  for (ptTmp = atMapSeg; tMapSegs; ++ptTmp, --tMapSegs) {
    DPRINTF2("dcClose() : Seg = %d\n", (int)*ptTmp);
    regs.af = (uint)*ptTmp << 8;
    callxx((unsigned)jump_table + FRE_SEG, &regs);
  }
  close(fdWork);
  unlink(acWorkName);
}

/**
 * rewind temporary file
 */
VOID dcRewind()
{
  uiWorkOff = 0;
  _seek(fdWork, (uint) 0, (tiny)0);
}

/**
 * 0 : RAM disk, 1 : mapped RAM, 2 : VRAM
 */
STATICF TINY where_to_use()
{
  if (uiWorkOff < uiDiskEnd) {
    DPUTCHAR('R');
    return 0;
  } else if (uiWorkOff < uiMapEnd) {
    DPUTCHAR('M');
    return 1;
  } else if (uiWorkOff < uiVRAMEnd) {
    DPUTCHAR('V');
    return 2;
  } else {
    DPRINTF2("where_to_use() : uiWorkOff = %u\n", uiWorkOff);
    exit(1);
    return 3; /* avoiding warning message when cc */
  }
}

/**
 * write temporary file
 */
STATICF VOID dcPutMap(pcBuf)
char *pcBuf;
{
  uint uiA, uiHL, uiTmp;

  uiTmp = uiWorkOff - uiMapStart;
  uiA = atMapSeg[uiTmp / MAP_SEG_KB];
  uiHL = uiTmp * 1024;
  do {
    calla(jump_wr_seg, uiA, uiHL, 0, (uint)*pcBuf);
    pcBuf++;
  } while (++uiHL & 1023);
}

STATICF VOID dcPutVRAM(pcBuf)
char *pcBuf;
{
  auto char acBufBIOS[1024]; /* must be in page 2 or 3 */
  memset(&regs, (tiny)0, sizeof(regs));
  memcpy(acBufBIOS, pcBuf, 1024);
  regs.ix = BIOS_LDIRVM;
  regs.iy = uiIY;
  regs.bc = 1024;
  regs.de = (uiWorkOff = uiVRAMStart) * 1024 + uiVRAMOff;
  regs.hl = (uint)acBufBIOS;
  callxx(CALSLT, &regs);
}

VOID dcPut(pcBuf, uiKBytes)
char *pcBuf;
uint uiKBytes;
{
  /*
  DPRINTF4("dcPut(0x04x, %2u) : uiWorkOff = %4u ",
    (uint)pcBuf, uiKBytes, uiWorkOff);
    */
  check_stack();
  for ( ; uiKBytes; pcBuf += 1024, ++uiWorkOff, --uiKBytes) {
    switch (where_to_use()) {
      case 0:
              if (write(fdWork, pcBuf, 1024) != 1024) {
                close(fdWork);
                fputs("Can not write. \n", stderr);
                exit(1);
              }
              break;
      case 1:
              dcPutMap(pcBuf);
              break;
      default:
              dcPutVRAM(pcBuf);
              break;
    }
  }
  DONE();
}

/**
 * read temporary file
 */
STATICF VOID dcGetMap(pcBuf)
char *pcBuf;
{
  uint uiA, uiHL, uiTmp;
  uiTmp = uiWorkOff - uiMapStart;
  uiA = atMapSeg[uiTmp / MAP_SEG_KB];
  uiHL = uiTmp * 1024;
  do {
    *pcBuf = calla(jump_rd_seg, uiA, uiHL, 0, 0);
    pcBuf++;
  } while (++uiHL & 1023);
}

STATICF VOID dcGetVRAM(pcBuf)
char *pcBuf;
{
  auto char acBufBIOS[1024]; /* must be in page 2 or 3 */
  regs.ix = BIOS_LDIRMV;
  regs.iy = uiIY;
  regs.bc = 1024;
  regs.de = (uint)acBufBIOS;
  regs.hl = (uiWorkOff - uiVRAMStart) * 1024 + uiVRAMOff;
  callxx(CALSLT, &regs);
  memcpy(pcBuf,acBufBIOS, 1024);
}

VOID dcGet(pcBuf, uiKBytes)
char *pcBuf;
uint uiKBytes;
{
  /*
  DPRINTF4("dcGet(0x%04x, %2u) : uiWOrkOff = %4u ",
    (uint)pcBuf, uiKBytes, uiWorkOff);
  */
  check_stack();
  memset(&regs, (tiny)0, sizeof(regs));
  for ( ; uiKBytes; pcBuf += 1024, ++uiWorkOff, --uiKBytes) {
    switch (where_to_use()) {
      case 0:
              if (read(fdWork, pcBuf, 1024) != 1024) {
                close(fdWork);
                fputs("Can not write. \n", stderr);
                exit(1);
              }
              break;
      case 1:
              dcGetMap(pcBuf);
              break;
      default:
              dcGetVRAM(pcBuf);
              break;
    }
  }
  DONE();
}

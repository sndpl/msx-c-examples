/**
 * dcopy1.c
 * (C) 1991 N. Ishikawa
 * Free to copy and use without warranty
 * On 20. Nov. 1991 by nao-i@ascii.co.jp
 */
#include "dcopy.h"
#pragma nonrec

#define DEBUG 1
#define BLOCK_BUF_SIZE (512 * 2 * 9)

static char acBuf[BLOCK_BUF_SIZE];

uint uiMin(uiX, uiY)
uint uiX, uiY;
{
  return (uiX < uiY ? uiX : uiY);
}

#ifndef LSI_C
int memcmp(pcX, pcY, uiLen)
char *pcX, *pcY;
uint uiLen;
{
  while(uiLen) {
    int iTmp;
    if ((iTmp = *pcX-*pcY) != 0) {
      return iTmp;
    }
    pcX++;
    pcY++;
    --uiLen;
  }
  return 0;
}
#endif /* LSI_C */

tiny isSameMedia (pdp1, pdp2)
diskparam_t *pdp1, *pdp2;
{
  return memcmp(&pdp1->usSectorLength, &pdp2->usSectorLength, 18) == 0;
}

VOID getDiskParam(pdp, tDrive)
diskparam_t *pdp;
tiny tDrive; /* 0 : current, 1 : A, .... */
{
  if(bdos(_DPARM, pdp, (ushort)tDrive)) {
    fputs("Can not get disk parameter.\n", stderr);
    exit(1);
  }
}

unsigned _dos_getdiskfree(device, diskspace)
unsigned device; /* 0 : current, 1 : A, .... */
struct diskfree_t *diskspace;
{
  XREG regs;
  char cAReg;

  memset(&regs, (tiny)0, sizeof(regs)); /* for face safe */
  regs.bc = _ALLOC;
  regs.de = device;
  callxx((unsigned)5, &regs);
  if ((cAReg = regs.af >> 8) == 0xff) {
    return 1;
  }
  diskspace->total_clusters = regs.de;
  diskspace->avail_clusters = regs.hl;
  diskspace->sectors_per_cluster = cAReg;
  diskspace->bytes_per_sector = regs.bc;
  return 0;
}

VOID dcWait(pcMsg)
char *pcMsg;
{
  fputs(pcMsg, stderr);
  fputs("Type RETURN key when ready.", stderr);
  do {
    ;
  } while (getchar() != '\n');
}



VOID dcRdBlock(uiStartSector, uiSectors, uiSectorSize)
uint uiStartSector, uiSectors, uiSectorSize;
{
  uint uiSectorToRead;

  dcRewind();
  uiSectorToRead = BLOCK_BUF_SIZE / uiSectorSize;
  while (uiSectors) {
    if (uiSectors < uiSectorToRead) {
      uiSectorToRead = uiSectors;
    }
#ifdef DEBUG
    fprintf(stderr, "Reading from sector %4u to %4u.\n",
      uiStartSector, uiStartSector + uiSectorToRead - 1);
#endif /* DEBUG */
    bdos(_SETDTA, acBuf, 0);
    if (bdos(_RDABS, uiStartSector, uiSectorToRead * 256)) {
      fputs("Can not _RDABS.\n", stderr);
      exit(1);
    }
    dcPut(acBuf, uiSectorToRead * uiSectorSize / 1024);
    uiStartSector += uiSectorToRead;
    uiSectors -= uiSectorToRead;
  }
}

VOID dcWrBlock(uiStartSector, uiSectors, uiSectorSize)
uint uiStartSector, uiSectors, uiSectorSize;
{
  uint uiSectorToWrite;

  dcRewind();
  uiSectorToWrite = BLOCK_BUF_SIZE / uiSectorSize;
  while (uiSectors) {
    if (uiSectors < uiSectorToWrite) {
      uiSectorToWrite = uiSectors;
    }
#ifdef DEBUG
    fprintf(stderr, "Writing from sector %4u to %4u.\n",
      uiStartSector, uiStartSector + uiSectorToWrite - 1);
#endif /* DEBUG */
    dcGet(acBuf, uiSectorToWrite * uiSectorSize / 1024);
    bdos(_SETDTA, acBuf, 0);
    if (bdos(_WRABS, uiStartSector, uiSectorToWrite * 256)) {
      fputs("Can not _WRABS.\n", stderr);
      exit(1);
    }
    uiStartSector += uiSectorToWrite;
    uiSectors -= uiSectorToWrite;
  }
}

main()
{
  diskparam_t  diskparamSrc, diskparamDst;
  static char acMsgSrc[] = "Insert source disk.\n";
  static char acMsgDst[] = "Inset destination disk.\n";
  uint uiWorkKBytes, uiWorkSectors, uiCurrentSector;
  uint uiSectorLength, uiRestSectors, uiBlockSectors;
  tiny tIsFirstTime;

  uiWorkKBytes = dcOpen();
  printf("%u K bytes of work area available.\n", uiWorkKBytes);
  dcWait(acMsgSrc);
  getDiskParam(&diskparamSrc, (tiny)1);
  uiRestSectors = diskparamSrc.usLogicalSectors;

  if ((uiSectorLength = diskparamSrc.usSectorLength) < 1024) {
    uiWorkSectors = uiWorkKBytes * (1024 / uiSectorLength);
  } else {
    uiWorkSectors = uiWorkKBytes* (16384 / uiSectorLength) / 16;
  }
  uiCurrentSector = 0;
  tIsFirstTime = 1;
  do {
    uiBlockSectors = uiMin(uiRestSectors, uiWorkSectors);
    if (!tIsFirstTime) {
      dcWait(acMsgSrc);
    }
    dcRdBlock(uiCurrentSector, uiBlockSectors, uiSectorLength);
    dcWait(acMsgDst);
    if (tIsFirstTime) {
      tIsFirstTime = 0;
      getDiskParam(&diskparamDst, (tiny)1);
      if (!isSameMedia(&diskparamSrc, &diskparamDst)) {
        fputs("Incompatible media. \n", stderr);
        exit(1);
      }
    }
    dcWrBlock(uiCurrentSector, uiBlockSectors, uiSectorLength);
    uiCurrentSector += uiBlockSectors;

  } while ((uiRestSectors -= uiBlockSectors) != 0);
  dcClose();
  exit(0);
}

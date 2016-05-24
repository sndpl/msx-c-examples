/**
 * dcopy2.c (v1)
 * (C) 1991 N. Ishikawa
 * Free to copy and use without warranty
 * on 20. Nov. 1991 by nao-i@ascii.co.jp
 */
#include "dcopy.h"
#pragma nonrec

extern STATUS _seek();
static FD fdWork;
static char acWorkName[] = "H:DCOPY.$$$";

/**
 * make temporary file and return size in K bytes
 */
uint dcOpen()
{
  XREG regs;
  struct  diskfree_t diskspace;
  ushort  usSectorSize;

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
    return diskspace.avail_clusters * diskspace.sectors_per_cluster
      * (usSectorSize / 128) / 8;
  } else {
    return diskspace.avail_clusters * diskspace.sectors_per_cluster
      * (usSectorSize / 1024);
  }
}

/**
 * erase temporary file
 */
VOID dcClose()
{
  close(fdWork);
  unlink(acWorkName);
}

/**
 * rewind temporary file
 */
VOID dcRewind()
{
  _seek(fdWork, (uint)0, (tiny)0);
}

VOID dcPut(acBuf, uiKBytes)
char *acBuf;
uint uiKBytes;
{
  size_t sizeToWrite;

  sizeToWrite = uiKBytes * 1024;
  if (write(fdWork, acBuf, sizeToWrite) != sizeToWrite) {
    close(fdWork);
    fputs("Can not write. \n", stderr);
    exit(1);
  }
}

VOID dcGet(acBuf, uiKBytes)
char *acBuf;
uint uiKBytes;
{
  size_t sizeToWrite;
  sizeToWrite = uiKBytes * 1024;
  if (read(fdWork, acBuf, sizeToWrite) != sizeToWrite) {
    close(fdWork);
    fputs("Can not read. \n", stderr);
    exit(1);
  }
}

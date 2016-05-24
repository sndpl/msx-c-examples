/*
 * dcopy.h
 * (C) 1991 N. Ishikawa
 * Free to copy and use without warranty
 * on 19. Nov 1991 by nao-i@ascii.co.jp
 */
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <bdosfunc.h>
#include <memory.h>

typedef char tiny;
typedef unsigned ushort, uint;

typedef struct {
  char    cPhisicalDrive;       /*  0 */
  ushort  usSectorLength;       /*  1 */
  char    cClusterBlocks;       /*  3 */
  ushort  usReservedSectors;    /*  4 */
  char    cFATs;                /*  6 */
  ushort  usDirectoryEntries;   /*  7 */
  ushort  usLogicalSectors;     /*  9 */
  char    cMediaDescripter;     /* 11 */
  char    cFATLength;           /* 12 */
  char    usRootStart;          /* 13 */
  char    usDataStart;          /* 15 */
  char    usMaxCluster;         /* 17 */
  char    cDirtyFlag;           /* 19 */
  char    acVolumeID[4];        /* 20 */
  char    acReserved[8];        /* 24 */
} diskparam_t;

struct diskfree_t {
  unsigned    total_clusters;
  unsigned    avail_clusters;
  unsigned    sectors_per_cluster;
  unsigned    bytes_per_sector;
};

unsigned _dos_getdiskfree();
uint  dcOpen();
VOID  dcClose();
VOID  dcRewind();
VOID  dcPut();
VOID  dcGet();

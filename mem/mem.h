
typedef unsigned uchar, uint;

/* Mapper information structure */
struct{
            uchar slot;              /* Mapper slot */
            uchar subslot;           /* Mapper subslot */
            uchar mtotal;            /* Total memory */
            uchar msystem;           /* System reserved memory */
            uchar muser;             /* User reserved memory */
            uchar mfree;             /* Free memory */
} mapper[16]; /* Up to 16 mappers supported */

#define high(r) ((r & 0xFF00) >> 8)
#define low(r) (r & 0xFF)
#ifndef NULL
#define NULL 0
#endif

#define EXTBIO 0x0FFCA /* Extended Bios */

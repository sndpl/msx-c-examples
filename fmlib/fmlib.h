/*
 * fmlib.h : header file for fmlib
 * by nao-i on 31 May 1990
 * (C) isikawa 1990
 * free to use and copy, but no guarantee or support
 */

extern char     fmopen(); /* please call this first */
extern VOID     fmclose(); /* please call this last */
extern VOID     fmwrite(); /* write to OPLL register */
extern VOID     fmotir(); /* write to OPLL register 0...7 */
extern VOID     fmstart(); /* back ground music */
extern VOID     fmstop(); /* stop back ground music */
extern char     *fmread(); /* read data from ROM */
extern char     fmtest(); /* Now playing ? */
#define FM_WORK  (160+32); /* size of workarea */

#define FM_VOL      0x0060 /* volume 60H .. 6FH */
#define FM_INST     0x0070 /* instument 70H...7FH */
#define FM_SUSOFF   0x0080 /* sustain off */
#define FM_SUSON    0x0081 /* sustain on */
#define FM_EXPINST  0x0082 /* expandet instrument */
#define FM_USRINST  0x0083 /* user-defined instrument */
#define FM_LEGOFF   0x0084 /* legato off */
#define FM_LEGON    0x0085 /* legato on */
#define FM_Q        0x0086 /* Q */
#define FM_END      0x00ff /* end of data */
#define FM_RVOL     0x00a0 /* volume of rhythm */

/* pitch */
#define FM_C    0   /* C */
#define FM_CS   1   /* C# */
#define FM_D    2
#define FM_DS   3
#define FM_E    4
#define FM_F    5
#define FM_FS   6
#define FM_G    7
#define FM_GS   8
#define FM_A    9
#define FM_AS   10
#define FM_B    11
#define FM_DS   3

/* octove */
#define FM_01   0   /* FM_01 + FM_C means C of octove 0 */
#define FM_02   12
#define FM_03   24
#define FM_04   36
#define FM_05   48
#define FM_06   60
#define FM_07   72
#define FM_08   84

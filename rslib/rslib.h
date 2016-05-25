/*
 * rslib.h : header file for rslib.asm
 *
 * by nao-i on 17. feb 1988 dedicated for CoCo
 * by nao-i on 22. Mar 1990 improved for MSX-SERIAL232
 * (C) Isikawa 1988,1990
 * free to use and copy, but no guaruantee or support
 */
typedef struct {    /* for rsinit() */
  char    rs_len;   /* character length '5' - '8'         */
  char    rs_par;   /* parity 'E', '1', '0' or 'N'        */
  char    rs_stp;   /* stop bits '1', '2', or '3'         */
  char    rs_xon;   /* XON/XOFF control 'Y' or 'N'        */
  char    rs_ctr;   /* CTR-RTS handshaking 'Y' or 'N'     */
  char    rs_rlf;   /* auto LF for receiver 'Y' or 'N'    */
  char    rs_slf;   /* auto LF for sender 'Y' or 'N'      */
  char    rs_sio;   /* S1/S0 control 'Y' or N'            */
  unsigned rs_rbp;  /* receiver speed 50 - 19200          */
  unsigned rs_tbp;  /* sender speed 50 - 19200            */
  char    rs_tim;   /* time out counter 0 - 255           */
} rstbl_t;

extern char   rsinit(), rsopen(), rsputc(), rsclose(), rsbreak(), rsdtr();
extern char   rsgetc(), kbgetc();
extern unsigned rsloc(), rslof(), rsstat();
extern char   rsprep();
extern VOID   rsrestore();

#define RSSTACKSIZE 256
#define RSFCBSIZE (517+6)

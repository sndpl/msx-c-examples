;
; rslib.asm : library for MSX-C 1.1 or 1.2
; not supporting TOHSIBA HX-R700
;
; by nao-i on 27. jan 1988 dedicated for CoCo
; by nao-i on 22. Mar 1990 improved for MSX-SERIAL232
; (C) Isikawa 1988,1990
; free to use and copy, but no guaruantee or support
        .Z80
;
CALSLT  EQU    0001CH    ; inter slot call
BREAKV  EQU    0F325H    ; Control + C break vector
RAMAD3  EQU    0F3FFH    ; slot of RAM in page 3
HOKVLD  EQU    0FB20H    ; hook is valid if b0 on
EXTBIOS EQU    0FFCAH    ; entry to extended BIOS
;
        DSEG
RSSLOT: DS    1 ; slot of RS232C
RSFCB:  DS    2 ; address for FCB
RSPAT:  DS    2 ; address for patch
RSTMP@: DS    2 ; address for table
RSSP@:  DS    2 ; SP for EXTBIOS
SPSAV:  DS    2 ; saving SP
BREAKS: DS    2 ; saving break vector
JINIT:  DS    2 ; entry address to init
JOPEN:  DS    2
JSTAT:  DS    2
JGETC:  DS    2
JSNDC:  DS    2
JCLOS:  DS    2
JEOF:   DS    2
JLOC:   DS    2
JLOF:   DS    2
JBACK:  DS    2
JSNDB:  DS    2
JDTR:   DS    2
NUMENT  EQU ($-JINIT)/2 ; # of jump vector
;
        CSEG
;
VVV:    DW    TORET
;
PATCH1:
        PUSH  BC
        CALL  0FFCAH
        POP   BC
TORET:  RET
;
E_NORS:                         ; jumped if no RS-232_C
        LD    A,1
        RET                     ; return (char)1
;
; char rsprep(rsstack, &rsstack[RSSTACKSIZE], rsfcb)
;
RSPREP::
;------------------------------------- allocate work area
        LD   (RSTMP@),HL
        LD   (RSSP@),DE
        LD   (RSPAT),BC
        LD   HL,6
        ADD  HL,BC
        LD   (RSFCB),HL
; ------------------------------------ get address of jump table
        LD   A,(HOKVLD)
        AND  1                 ; hook is valid
        JR   Z,E_NORS          ; no, there is no RS232C
        LD   HL,(RSTMP@)
        LD   A,(RAMAD3)
        LD   B,A
        LD   DE,8*256+0
        PUSH HL
        LD   (SPSAV),SP
        LD   SP,(RSSP@)
        CALL EXTBIOS
        LD   SP,(SPSAV)
        POP  DE                ; DE = (RSTMP@)
        XOR  A
        SBC  HL,DE
        EX   DE,HL             ; HL = top of table
        JR   Z,E_NORS          ; There is no RS232C
        LD   A,(HL)            ; Slot of RS232C
        LD   (RSSLOT),A
        INC  HL
        LD   E,(HL)
        INC  HL
        LD   D,(HL)            ; DE = jump table
        LD   HL,JINIT-1
        LD   B,NUMENT          ; # of jump vector
INIT1:
        INC  DE
        INC  DE
        INC  DE                 ; DE = next vector
        INC  HL
        LD   (HL),E
        INC  HL
        LD   (HL),D
        DJNZ INIT1
; ------------------------------ set break vector
        LD   HL,(BREAKV)
        LD   (BREAKS),HL       ; save break vector
        LD   HL,VVV
        LD   (BREAKV),HL       ; set break vector
;------------------------------- patch for PUSH BC bug
        LD   HL,PATCH1
        LD   DE,(RSPAT)
        PUSH DE
        LD   BC,6
        LDIR
        POP  HL                 ; = (RSPAT)
        LD   (0FFDEH),HL
        XOR  A
        RET                     ; return (char) 0
;
; void rsrestore()
;
RSRESTORE::
        LD   HL,(BREAKS)
        LD   (BREAKV),HL        ; restore break vector
        LD   HL,0FFCAH
        LD   (0FFDEH),HL        ; restore patch
        RET
;=================================
;    functions to call RS-232-C
;=================================
CALLRS1:
        LD   (SPSAV),SP
        LD   SP,(RSSP@)
        LD   IY,(RSSLOT-1)
        CALL CALSLT
        LD   SP,(SPSAV)
        EI
        RET
;
CALLRS  MACRO VECTOR
        LD   IX,(VECTOR)
        CALL CALLRS1
        ENDM
;
JPRS    MACRO VECTOR
        LD   IX,(VECTOR)
        JP   CALLRS1
        ENDM
;
; char rsinit(table)
; rstbl_t *table
;
RSINIT::
        LD   DE,(RSTMP@)
        PUSH DE
        LD   BC,13              ; sizeof(rs_tbl)
        LDIR
        POP  HL                 ; = (RSTMP@)
        LD   A,(RAMAD3)
        LD   B,A
        CALLRS JINIT
        JP   RETCY
;
; char rsopen()
;
RSOPEN::
        LD   HL,(RSFCB)
        LD   C,254              ; buffer length
        LD   E,4                ; RAW I/O mode
        CALLRS JOPEN
        JR   RETCY
;
; uint rsstat()
;
RSSTAT::
        JPRS    JSTAT
;
; int rsgetc()
;
RSGETC::
        CALLRS JGETC
        LD   H,0
        LD   L,A
        RET  P                  ; no Error
        LD   HL,-1
        RET
;
; char rsputc(c)
; char c;
;
RSPUTC::
        CALLRS JSNDC
        LD   A,1
        RET  C                  ; STOP key was pressed
        LD   A,2
        RET  Z                  ; Time Out
        XOR  A
        RET                     ; no problem
;
; char rsclose()
;
RSCLOS::
        CALLRS JCLOS
RETCY:                          ; return 255 if cy set
        LD   A,0
        RET  NC
        DEC  A
        RET
;
; uint rsloc()
;
RSLOC@::
        JPRS   JLOC
;
; uint rslof()
;
RSLOF@::
        JPRS   JLOF
;
; char rsbreak(n)
; uint n;
;
RSBREA::
        EX   DE,HL
        CALLRS JSNDB
        JR   RETCY
;
; void rsdtr(f)
; char f;
;
RSDTR@::
        JPRS  JDTR
;
        END

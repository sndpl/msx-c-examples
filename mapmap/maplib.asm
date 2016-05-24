;
; maplib.z80
; on 26 nov 1990 by nao-i
; (C) 1990 N. Ishikawa
; free to use and copy without warranty
EXTBIOS equ   0ffcah
        .Z80
        CSEG
;
; int callz(char *address, union REGS *inregs, union REGS *outregs)
;
callz@::
        push    bc      ; &outregs
        ld      bc,callz_ret
        push    bc      ; address to return
        push    hl      ; address to call
        ld      hl,12
        add     hl,sp
        ld      sp,hl
        ex      de,hl   ; de = sp, hl = &inregs
        ld      bc,12
        ldir            ; from inregs to stack
        pop     af
        pop     bc
        pop     de
        pop     hl
        pop     ix
        pop     iy
        ret
callz_ret:              ; back to the here
        push    iy
        push    ix
        push    hl
        push    de
        push    bc
        push    af
        ld      hl,12
        add     hl,sp
        ld      e,(hl)
        inc     hl
        ld      d,(hl)  ; de = &outregs
        ld      hl,0
        add     hl,sp   ; hl = sp
        ld      bc,12
        ldir            ; from stack to outregs
        ld      hl,-14
        add     hl,sp
        ld      sp,hl   ; discard stack
        ret
;
; *mapinit(maps_t *maps);
;
mapinit@::
        push    hl
        xor     a
        ld      de,0402H        ; get  mapper jump table
        call    EXTBIOS
        or      a
        jr      z,mapinit_nomap
        ex      de,hl
        pop     hl              ; maps
        ld      (hl),b          ; maps->pmap_slot
        inc     hl
        ld      (hl),a          ; maps->pmap_total
        inc     hl
        ld      (hl),c          ; maps->pmap_free
        inc     hl
        ld      (hl),e
        inc     hl
        ld      (hl), d         ; maps->pmap_jump
;
        xor     a
        ld      de,0401H        ; get mapper variable table
        call    EXTBIOS
        ret                     ; return address of table
;
mapinit_nomap:
        pop     hl
        ld      hl,0
        ret                     ; returns NULL if error
;
        END

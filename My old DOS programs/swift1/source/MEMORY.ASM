
                 .model medium, C

;--------------------------------------------------------------------------
; (c) Michael Finnegan    15 May 1999
;--------------------------------------------------------------------------
;The major routines in this file callable from C are:

;void initmem()
;int getlinebuf(char *textbuf, int *cline);
;int overwriteline(char *textbuf, int *cline);
;int insertline(char *textbuf, int *cline);
;int deletelines(int *clines, int *numlines);
;int appendline(char *textbuf);
;int getlines();
;
;There are 128 bytes reserved for each line and 256k. That means the
;total reserved is for a maximun of 2,048 lines
;Each line stored is prefixed by its length byte.
;The actual text length must not exceed 127 bytes
;--------------------------------------------------------------------------
                 .FARDATA txttab             ;
                 db   65535 dup(0)           ;reserve 256k for text
                 .FARDATA txttab2
                 db   65535 dup(0)
					  .FARDATA txttab3
					  db   65535 dup(0)
					  .FARDATA txttab4
                 db   65535 dup(0)
;----------------------------------------
                 .DATA
_lines           dw   0                      ;r # of text line
userlen          dw   0
                 .CODE
LOCALS

;--------------------------------------------------------------------------
;This routine returns with the r len of the users string in ax
;The length returned excludes the zero byte suffix.
;Entry:
;  ds:di = adr of string
;Exit:
; ax = r len of string exluding the zero suffix byte
getlen           proc
                 push cx
                 push es                     ;In the comparsion instruct
                 push di                     ;ion repnz scasb the es;[di]
                 mov  ax, ds                 ;contains the dest byte which
                 mov  es, ax                 ;is subtracted from al,
                 xor  ax,ax                  ;without changing al, just the
                 cld                         ;flags. So we need to save the
                 mov  cx, -1                 ;original value in es, cause the
                 repnz scasb
                 mov  ax, cx                 ;reg. We are going to restore di
                 not  ax                     ;and al
                 dec  ax
                 pop  di
                 pop  es
                 pop  cx
					  ret
getlen           endp
;--------------------------------------------------------------------------
;initialise the text memory, storing 1 default line consisting
;of a RTN code
                 PUBLIC _initmem
_initmem         PROC C uses ds es di si
                 mov  dx, txttab
                 mov  ds, dx
                 mov  si, 0
                 mov  ax, 0d01h              ;ah = 13, al = 01
                 mov  ds:[si],ax
                 mov  ax, 1
                 mov  ss:_lines,ax
					  ret
_initmem         endp
;--------------------------------------------------------------------------
;Return a line of text to a C++ supplied buffer
;int GetLineBuf(char *linebuf1, int &cline);
;Exit:
; if successful ax = len of text
; else ax =0
                 PUBLIC _getlinebuf
_getlinebuf      PROC C uses ds es bx cx dx si di, \
                 textbuf: ptr word, cline: ptr word
                 push ds
                 pop  es                     ;make sure es point to ds
                 mov  bx, cline
                 mov  cx, [bx]               ;get r line #
                 mov  ax, 0                  ;prepare for error
                 cmp  ss:_lines, cx
                 jb   @@ex                   ;line out of range error
                 cmp  cx, 0
                 je   @@ex                   ;line out of range error
                 mov  ax, cx
                 dec  ax
					  sal  ax, 3                  ;ax *8
                 add  ax, txttab
                 mov  ds, ax
                 mov  si, 0
					  mov  cx, ds:[si]            ;get len of txt line
                 mov  ch, 0
                 mov  bx, cx                 ;keep copy for later
                 inc  si                     ;point to 1st char of text
                 mov  di, textbuf            ;es:[di] = adr of user buf
                 cld
                 rep  movsb
                 mov  al, 0                  ;null terminate user string
                 mov  es:[di], al
                 mov  ax, bx                 ;r len of text
@@ex:            ret
_getlinebuf      ENDP

;--------------------------------------------------------------------------
;Overwrite the current line in far memory with the user's line
;Exit:
; ax = len of line written
; if ax = 0 error
                 PUBLIC _overwriteline
_overwriteline   PROC C uses ds es di si,\
					  textbuf: ptr word, cline: ptr word

                 mov  bx, cline
                 mov  cx,[bx]                ;get r line #
                 mov  ax, 0                  ;prepare for error
                 cmp  ss:_lines, cx
                 jb   @@ex                   ;error: cx > lines
                 cmp  cx, 0
                 je   @@ex                   ;error: cx =0

                 mov  di, textbuf
                 call getlen                 ;rtns w/ ax = len
                 cmp  ax, 0
                 je   @@ex                   ;error: user's len =0
                 cmp  ax, 127
                 ja   @@ex                   ;error user's len too big
					  xchg cx, ax                 ;cx = r len, ax = line
					  mov  si, di                 ;ds:si = user's string
                 dec  ax
                 sal  ax, 3                  ;ax *8
                 add  ax, txttab
                 mov  es, ax
                 mov  di, 0                  ;es:[di] = far adr
                 mov  es:[di], cl            ;new len byte
                 inc  di
                 cld
                 mov  ax, cx                 ;r len of line for later
                 rep  movsb
                 jmp  @@ex1
@@ex:            mov  ax, 0                  ;flg: error
@@ex1:           ret
_overwriteline   ENDP
;--------------------------------------------------------------------------

;insert a line of text in memory in range line 1 to lines
;All the lines from the original line at that numbered position to
;the end line is moved down by 80 bytes, and the new line written into
;the gap thus opened up.
;Exit
;  ax =0 if error, else
;  ax = r len of line written

                 public _insertline
_insertline      proc C uses ds es di si,\
                 textbuf: ptr word, cline: ptr word

                 mov  bx, cline
                 mov  cx,[bx]                ;line # of line to overwrite
                 
                 mov  ax, 0                  ;prepare for error
					  cmp  cx, 0
                 je   @@ex                   ;error: cline =0
                 cmp  ss:_lines, cx
                 jae  @@con
                 jmp  @@ex

@@con:           mov  di, textbuf
                 call getlen                 ;get user's len
                 mov  userlen, ax
                 cmp  ax, 0
                 je   @@ex
                 cmp  ax, 127
                 ja   @@ex                   ;error: len to great

                 mov  dx,ss:_lines
                 mov  bx, dx
                 sub  bx, cx
                 inc  bx                     ;bx = r # of lines to push
                 mov  ax, dx                 ;down
                 dec  ax
					  sal  ax, 3
                 add  ax, txttab
                 mov  ds, ax                 ;ds = seg adr of last line
                 add  ax, 8                  ;go down 128 bytes
                 mov  es, ax
                 cld

@@a1:            mov  si, 0
                 mov  di, 0
                 mov  cx, 64                 ;= 128 bytes
                 rep  movsw
                 mov  ax, ds
                 mov  es, ax
                 sub  ax, 8
                 mov  ds, ax
                 dec  bx
                 cmp  bx, 0
                 jne  @@a1

                 mov  ax, ss
					  mov  ds, ax
                 mov  si, textbuf
                 mov  di, 0
                 mov  ax, ss:userlen
                 mov  cx, ax
                 mov  es:[di],al
                 inc  di
                 rep  movsb
                 inc  ss:_lines
                 jmp  @@ex1                  ;ax = r len of line


@@ex:            mov  ax,0                   ;error exit
@@ex1:           ret
_insertline      endp
;--------------------------------------------------------------------------
;Delete 1 line from memory within range 1 to lines
;if there are lines below the line being deleted push them all up, and
;decrement _lines, else if its the last line then just decrement _lines
; if _lines = 0 call _initmem to place 1 default line in memory
;Exit
; ax = 0 if error, else ax = r # of lines left

                 public _deleteline
_deleteline      proc C uses ds es di si,\
                 cline: ptr word

                 mov  bx, cline
                 mov  cx, [bx]
                 mov  ax, 0                  ;assume error
                 cmp  cx, ss:_lines
                 ja   @@ex                   ;error: cline > lines
                 jb   @@con                  ;cline < lines
;delete the last line.
                 mov  ax, ss:_lines
                 dec  ax
                 cmp  ax, 0
                 jne  @@b1
                 call _initmem
                 jmp  @@ex                   ;ax = 1, lines =1
;delete the last lines. There is a line above
@@b1:            mov  ss:_lines, ax          ;lines = lines-1
                 jmp  @@ex

;delete a middle line. Shift the block below up to the current line
@@con:
                 mov  bx, ss:_lines
                 sub  bx, cx                 ;bx = r # of lines below
                 cld
                 mov  ax, cx
                 sal  ax, 3
                 add  ax, txttab
                 mov  ds, ax
					  sub  ax, 8
                 mov  es, ax

@@c1:            mov  cx, 64
                 mov  si, 0
                 mov  di, 0
                 rep  movsw                  ;ds:[si] to es:[di]
                 mov  ax, ds
                 mov  es, ax
                 add  ax, 8
                 mov  ds, ax
                 dec  bx
                 cmp  bx, 0
                 jne  @@c1

                 dec  ss:_lines
                 mov  ax, ss:_lines
@@ex:            ret
_deleteline      endp
;--------------------------------------------------------------------------
;Append a line into far memory with the user's line
;Exit:
; ax = len of line written
; if ax = 0 error
                 PUBLIC _appendline
_appendline      PROC C uses ds es di si,\
                 textbuf: ptr word

                 mov  di, textbuf
                 call getlen                 ;rtns w/ ax = len
                 cmp  ax, 0
                 je   @@ex
                 cmp  ax, 127
                 ja   @@ex

                 mov  cx, ax                 ;cx = r len
					  mov  si, di                 ;ds:si = user's string
					  mov  ax, ss:_lines
					  sal  ax, 3                  ;ax *8
					  add  ax, txttab
					  mov  es, ax
					  mov  di, 0                  ;es:[di] = far adr
					  mov  es:[di], cl            ;new len byte
					  inc  di
					  cld
					  mov  ax, cx                 ;r len of line for later
					  rep  movsb
					  inc  ss:_lines
					  jmp  @@ex1
@@ex:            mov  ax, 0                  ;flg: error
@@ex1:           ret
_appendline      ENDP

;--------------------------------------------------------------------------

                 public _getlines 
_getlines        proc
                 mov  ax, ss:_lines
                 ret
_getlines        endp
;--------------------------------------------------------------------------
                 END


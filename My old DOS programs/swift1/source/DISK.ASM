;-------------------------------------------------------------------------
;   (C) Michael Finnegan  16 May 1999
;   
;
;   
;-------------------------------------------------------------------------*/

; ((C))1
;-----------------------------------------------------------------------------
;This file contains functions for reading in dir enteries
;and file enteries and sorting them (using Shell/Metzer sort algorithm)
;and send the enteries one at a time back to C++ in a user supplied buffer
;It can cather for Windows 95 long names.
;---------------------------------------------------------------------------

				 .model medium, C

				 .fardata dirbuf
				 db   49152 dup(0)           ;2234*22: space 2234
;short enteries.  or 1500 long enteries averaging around 16 chars
;The normal DOS 6.2 short names directory is structed our way like
;this.
; offset
;   0             db  0                        attrib   (dir, file ect)
;   1            db  0 dup(2)                 hh/mm/ss
;   3            db 0 dup(2)                  dd/mm/yy
;   5            dd 0                          file size
;   9            db 0 dup (13)                name siffixed with zero
;   22           beginning of next entry (attrib byte)

;there is a total of 48k reserved for the directory.
;Our structure of storing enteries with long names is totally different.
;in dirbuf it is:
;                db 0 dup(2)                 addr in seg dirbuf of name
;                db 0 dup(2)                 file attribute
;                db 0 dup(2)                  date
;                db 0 dup(2)                  time
;                db 0 dup(4)                  file size ( 0 - 4,294,967,295)
; this makes up a block of 12 bytes
; there are 24,000 bytes reserved from the beginning of seg dirbuf
; for each enteries 12 byte block.  Tthat is 2,000 blocks
; This means
; the long names are stored beginning at offset 24000. Each name string
; is not null-terminated, but is preceded by a 2 byte len
; example: at offset 24000 it looks like:
;                 dw 24 dup(0)               ;len of long name is 24 bytes
;                 db 'This is a long file name'
;                 dw 8 dup(0)                     ;8 bytes in next name
;                 db 'nextname'
;by using an offset of a multiple of 12 in seg dirbuf we
;can determine the location of the addr of any name
;The the size and addresses of the long names will vary. They begin
;at offset 24000 and 24k is reserved for them. If there are 2,000 enteries
;each name on araverage could be 14 bytes long, + 2 bytes for its len
;
;numdirs is the number of dirs, numfiles the number of files and
;numenteries the number of enteries.
;------------------------------
				 .data

staticdirmatch   db   '*.*',0                ;This is never changed
															;for reading in dirs
buf              db   512 dup(0)             ;buf for reading/ writing sector
currentpath      db   128 dup(0)
dev              db   0
defpath          db   'X:\', 0
pathlen          db   0

dta              db   128 dup(0)             ;setup the data transfer  area
filespec         db   128 dup(0)             ;space for 'c:\windows\*.*'

numfiles         dw   0                      ;r # of files found
numdirs          dw   0                      ;r # of dirs found
numenteries      dw   0
;These are pars needed by the sort & compare functions
gap              dw   0
a                dw   0
b                dw   0
c                dw   0
d                dw   0
n                dw   0
base             dw   0
sortflg          dw   0
fdi              dw   0
fsi              dw   0
fuser_string     dw   0                      ;temp save adr of user string
userslen         db   0                      ;len of user's string
time             dw   0                      ;# of time to read/write sector
;-------------------------------------------------
;This area of memory is reserved for the longfile name
;structure that is sent to INT 21h to be fill for each
;directory entry.
;The long file name is stored at offset 44.                  offset
finddata         db   4 dup(0)           ;attributes           0
				 db   8 dup(0)               ;Creation time        4
				 db   8 dup(0)               ;Last access time    12
				 db   8 dup(0)               ;Modification time   20
				 db   4 dup(0)               ;File size high      28
				 db   4 dup(0)               ;File size lo        32
				 db   8 dup(0)               ;Reserved            36
				 db   260 dup(0)             ;Long file name      44
				 db   14 dup(14)             ;File name[14] reserved 304
;------------------------

				 PUBLIC errorcheck
errorcheck       dw   3333h                  ;error check marker in case of
															;overwrite by DOS 7
;-------------------------
varsb            dw   0                      ;needed by _long_swap_entry
varsd            dw   0
errorvar         dw   0                      ;needed by _longfindfirst ect.
filehandle       dw   0                      ;used by _longfindnext
dosversion       dw   0                      ;dos 0 = short, 1 = long names

;The first 4 bytes of the ModificationTime are as follows
;byte
; 0 --: The time hh:mm:ss
; 1 --:
;
; 2 --: The date yy/mm/dd
; 3 --:
;---------------------------------------------
;There function addresses are needed by the one Shell/Metzner
;sorting algorythm
compare          dw   0                      ;these will hold the addresses
swap             dw   0                      ;of the appropriate version
															;see the main sorting routine
															;for more details.

GetDirNames      dw   0                      ;needed by _sort_all_enteries
GetFileNames     dw   0                      ;to do an indirect call

entryattrib      dw   0                      ;needed by _find_file &
															;_long_find_file

routines         dw   _compare_string        ;for short names
				 dw   _swap_enteries
				 dw   _long_compare_string   		;for long names
				 dw   _long_swap_enteries
;----------------------
;used by _sort_all_enteries at offset 8 from routines
				 dw   _get_dir_names         ;0
				 dw   _get_file_names        ;2
				 dw   _long_get_dir_names    ;4
				 dw   _long_get_file_names   ;6
;----------------------------------------------
;used by _fetch_stored_entry
;must be at routines+16
				 dw   _get_stored_entry		  ;for short names
				 dw   _long_get_stored_entry ;for long names
;--------------------------------------------------------------------
				 PUBLIC currentpath          ;make these strings
				 public
				 PUBLIC filespec
				 .code
LOCALS
;--------------------------------------------------------------------
;return the current disk in dev (ie.,a = 1, b = 2, c= 3)
_get_current_drive    proc C uses di si dx

				 mov  ax, 1900h
				 int  21h
				 inc  al
				 mov  dev, al
				 ret
_get_current_drive    ENDP
;-----------------------------------------------------------------------
;Copy a source string into a destination string
;Entry:
; ds: si = adr of a null term source string
; es: di = adr of destination string
; Exit:
;  si, di advanced to byte past final null
;  ax = r len of source string
_str_copy        PROC
				 push bx
				 mov  bx, di                 ;bx = st adr of destination
@@sc_1:          lodsb                   ;read a byte from ds:[si]
													  ;into al
				 stosb                       ;write al into es:[di]
				 or   al, al                 ;check for final null
				 jnz  @@sc_1
				 mov  ax, di                 ;calculate length copied
				 sub  ax, bx
				 dec  ax

				 pop  bx                     ;ax = r len of source str
				 ret
_str_copy        ENDP
;--------------------------------------------------------------------
;return the r len of a string
;Entry:
; es:di ptr to string
;Exit:
;  ax = len
str_len          PROC
				 push cx
				 push di
				 pushf                       ;save callers flags

				 mov  cx, -1                 ;max string length
				 xor  al, al                 ;clr al
				 cld                         ;ensure correct string direction
				 repne scasb                 ;repeat while es:[di] <>0
				 mov  ax, -2
				 sub  ax, cx                 ;get string len
				 popf                        ;restore callers flags
				 pop  di
				 pop  cx
				 ret
str_len      ENDP
;--------------------------------------------------------------------
;This function stores  the dta entry in far data dirsbuf
;at c entry pos in (ax *22)
; it is only called for short DOS 6.2 type enteries.
;Entry:
;  ax = c # of pos to store entry in (it will be multiplied by 22)

_store_entry PROC C near uses ds es si di
				 mov  cx, 22
				 mul  cx
				 mov  di, ax
				 mov  ax, dirbuf
				 mov  es, ax                 ;es = far data seg dirbuf
													  ;de:di = pos to write into
				 lea  si, dta+21             ;adr of entry details
													  ;cx alread = 22 from above
				 shr  cx, 1                  ;cx =11
				 cld                         ;copy 11 words = 22 bytes
				 rep  movsw                  ;copy from ds:si to es:di

				 ret
_store_entry endp
;--------------------------------------------------------------------
;This routine returns to the caller an enter num (either dir or file)
;from the fardata dirbuf, called from _fetch_stored_entry
;Entry:
; textbuf = the adr of the user's supplied buffer to store the entry in
; num = the c # of the entry to get
;       for example if num = 0 it usually means entry '..' (the
;       spec for DOS to go Back to the previous parent dir
;       if num = 1 if usually points to the first dir (if there are any).
;       The dirs (if any) come first, then the files
;Exit:
; ax = 0 if the entry number was out of range
; else  ax = the r total  # of all enteries in the fardata dirbuf

_get_stored_entry     PROC C near uses ds es di si,\
				 textbuf: ptr word, num: ptr word

				 mov  ax, num
				 cmp  ax, numenteries
				 jc   @@ok                   ;num smaller.
				 jmp  @@ex                   ;num>= numenteries error:
													  ;because num is from 0 upwards
													  ;where as
													  ;numenteries is from  1 upwards

@@ok:        mov  cx, 22
				 mul  cx
				 mov  si, ax
				 cld
				 push ds
				 pop  es                     ;seg user string dest
				 mov  di, textbuf            ;es:di = user string
				 mov  ax, dirbuf
				 mov  ds, ax

													  ;copy from ds:si to es:di
													  ;cx times (cx already =22)
				 shr  cx, 1                  ;cx =11
				 rep  movsw                  ;copy 11 words instead of
													  ;22 bytes
@@ex:        mov  ax, ss:numenteries
				 ret
_get_stored_entry     endp
;--------------------------------------------------------------------
;This routine sets up the DTA
_setupdta        PROC
				 mov  ax, 1a00h
				 lea  dx, dta                ;ds:dx = adr of dta

				 int  21h
				 ret
_setupdta        ENDP
;--------------------------------------------------------------------
;This function returns the drive and the path in a string named
;currentpath
;Entry:
;
;Exit:
; c = 1 if error occurred
;          and ax = error code (0f = invalid drive specification)
; c = 0 and ax = len of current full drive + path +"\"+0
				 PUBLIC _getcurrentpath
_getcurrentpath  PROC
				 push si
				 push di
				 call cs:_GetDosVer
				 pushf                       ;c =0 if small names
				 mov  ax, 1900h              ;19h="get current disk"
				 int  21h                    ;get default drive
				 inc  al                     ;if "a" al = 1, c =3
				 mov  dev, al                ;1 = a, 2 = b 3=c
				 mov  dl, dev                ;drive # 0 = default
				 lea  si, currentpath
				 lea  di, defpath
				 mov  ax, [di]               ;setup 'X:\',+0
				 mov  [si], ax
				 mov  ax, [di+2]
				 mov  [si+2], ax
				 mov  al, 3
				 mov  pathlen, al

				 add  dl, 64
				 mov  [si], dl               ;letter of drive
				 mov  dl, dev                ;drive num
				 mov  ah, 47h                ;47h ="get currentdirectory"
				 popf                        ;long or small names?
				 jnc  @@small
				 mov  ax, 7147h              ;long names "get current dir"
@@small:     int  21h                    ;get current directory
				 jc   @@ex                   ;error occurred

;got path find len of string and store in pathlen
				 lea  di, currentpath
				 push ds
				 pop  es
				 call str_len                ;get len of stringin ax
				 mov  bx, ax
;if there is no path make default ei., 'C:\' for drive C
				 cmp  al, 0
				 jne  @@h4

				 mov  al, dev                ;setup default path as
				 add  al, 64                 ;'C:\' or 'D:\'
				 mov  ah, ':'
				 mov  [di], ax
				 mov  al, '\'
				 mov  ah,0
				 mov  [di+2], ax
				 mov  ax, 3
				 mov  pathlen, al
				 jmp  @@ex1
@@h4:
;if there is a '\' at the end remove it
;because it will be addred on later

													  ;bx = r len
				 mov  al, [di+bx-1]          ;last char
				 cmp  al, '\'
				 jne  @@g1
				 dec  bx
				 mov  [di+bx], ah            ;ah = 0

@@g1:            mov  ax, bx             ;len, either org or new shorter
													  ;len
				 mov  pathlen, al
				 mov  cx, ax
				 mov  al, '\'
				 mov  [di+bx], ax            ;add '\' +0 to path

				 add  bx,1                   ;bx = r len of
													  ;(line with '\' in it)

				 add  cx,2                   ;r # of bytes to shift = len +
													  ;the zero byte

;open line by three bytes to prefix path with 'C:\'
@@h1:        mov  al, byte ptr [di+bx]
				 mov  byte ptr [di+bx+3],al
				 dec  di
				 loop @@h1
				 lea  di, currentpath
				 mov  al, dev
				 add  al, 64
				 mov  ah, ':'
				 mov  word ptr [di], ax
				 mov  al, '\'
				 mov  byte ptr [di+2], al
				 mov  al, pathlen
				 add  al, 4
				 mov  byte ptr pathlen, al
@@ex1:       clc

				 mov  al, pathlen
				 mov  ah,0

@@ex:        pop  di
				 pop  si
				 ret
_getcurrentpath  endp
;--------------------------------------------------------------------
;(called by _get_dir_names
;     also called by _get_file_names)
;
;This routine is called for normal (short) filenames
;Find an entry in the directory either file or dir
;if found it will be in the dta string
;return 1 if found, else return 0
;Entry:
; filespec = the setup file specification search string such as 'c:\*.*'
;            it contains the full drive and path and wild card mask.
; bx       = len of filespec
; if bx = 0 find next, else find first using filespec

; entryattrib = type of entry to tell dos to find

;Exit:
; if ax = 0 not found,
; else ax = 1 and the details of the file that was found is
;             stored at c offset  21 of dta string
;  c = 0 if found, else c =1 not found

_find_file   proc
				 push si
				 push di
				 lea  si, filespec
				 mov  al, 0
				 mov  ds:[si+bx], al         ;null term string
				 cmp  bx, 0                  ;no str so find next
				 jz   @@fn                   ;find next

				 mov  ax, 4e00h              ;find first
				 mov  cx, entryattrib        ;type of enteries to look for
				 mov  dx, si                 ;adr of filespec
				 jmp  @@fn1

@@fn:        mov  ax, 4f00h              ;find next
@@fn1:       int  21h
				 mov  ax, 1                  ;flag for found
				 jnc  @@ex                   ;really did find
				 mov  ax,0                   ;flag: ax = not found

@@ex:			 pop  di
				 pop  si
				 ret
_find_file   endp
;---------------------------------------------------------------------
;(called by _sort_all_enteries)  for DOS 6.2 and less
;mustn't be called from C++
;get the names of all the dirs and store them from dirbuf
;
;Entry:
; fuser_string = adr of user string '*.*'    to get all dirs
;
; exit:
;  ax = r # of dir enteries got
;  the dir enteries will be stored from the beginning of farseg dirbuf
;  each entry is 22 bytes long

_get_dir_names   PROC C near uses ds es si di

				 mov  ax, 0
				 mov  numdirs, ax            ;reset number of dirs + files
				 mov  numfiles, ax           ;to zero.
				 mov  numenteries, ax

				 call _getcurrentpath        ;get drive & path into
				 jnc  @@a1                   ;currentpath
				 jmp  @@ex                   ;error: something invalid!

@@a1:        push ds
				 pop  es
				 lea  si, currentpath
				 lea  di, filespec
				 mov  cx, ax                 ;copy from current path into
				 cld                         ;filespec
				 rep  movsb
													  ;ax = len of currentpath
													  ;di = byte past final null
;append the masbuf ie., ('*.*') to the filespec
				 lea  di, currentpath
				 call str_len
				 lea  di, filespec
				 add  di, ax
													  ;di = adr of z byte in filespec
				 mov  si, fuser_string       ;users supplied mask such as
											 ;'*.*'
				 call _str_copy              ;add mask to file spec
				 add  al, pathlen
				 mov  bx, ax                 ;bx = r len of file spec
				 jmp  @@a2                   ;find first

@@a3:        mov  bx,0                   ;flg find next if bx =0
@@a2:        call _find_file             ;it receives filespec & bx
													  ;and ret w/ entry in dta

				 jc   @@ex                   ;entry not found
;check to see if a valid dir was got
				 mov  al, byte ptr [dta+21]
				 and  al, 16                 ;mask in dir bit only
				 cmp  al, 16                 ;is it a dir?
				 jne  @@a3                   ;no. read in next entry
				 mov  ax, word ptr [dta+30]
				 cmp  ax, 002eh              ;did we get '.' as an entry?
				 je   @@a3                   ;yes. skip it, read in next

				 mov  ax, numdirs
				 call _store_entry
				 inc  numdirs
				 jmp  @@a3

@@ex:        mov  ax, numdirs
				 mov  numenteries, ax        ;return num dirs
				 ret
_get_dir_names   endp
;---------------------------------------------------------------------
;mustn't be called from C++
;_get_dir_names() should be called before calling this function
;
;get the names of all the file enteries and store them from
; dirbuf +(numdirs)*22)
;
;Entry:
; fuser_string = adr of user's string = '*.*'    to get all dirs
;
; exit:
;  ax = r # of file enteries got
;  the file enteries will be stored from farseg dirbuf +(numdirs*22)
;  each entry is 22 bytes long

_get_file_names  PROC C near uses ds es si di


				 call _getcurrentpath        ;get drive & path into
				 jnc  @@a1                   ;currentpath
				 jmp  @@ex                   ;error: something invalid!

@@a1:        push ds
				 pop  es
				 lea  si, currentpath
				 lea  di, filespec
				 mov  cx, ax                 ;copy from currentpath into
				 cld					     		  ;filespec
				 rep  movsb
													  ;ax = len of currentpath
													  ;di = byte past final null
;append the masbuf ie., ('*.*') to the filespec
				 lea  di, currentpath
				 call str_len
				 lea  di, filespec
				 add  di, ax
				 mov  si, fuser_string       ;users supplied mask such as
											 ;'*.*'
				 call _str_copy              ;add mask to filespec
				 add  al, pathlen
				 mov  bx, ax                 ;bx = r len of filespec
				 jmp  @@a2                   ;find first

@@a3:        mov  bx,0                   ;flg find next
@@a2:        call _find_file

				 jc   @@ex                   ;entry not found

;check to see if a valid file entry was got
				 mov  al, byte ptr [dta+21]

				 test al, 16+8+4+2           ;is it  dir, label, sys,hidden?
				 jnz  @@a3                   ;skip: read in next entery
				 mov  ax, word ptr [dta+30]
				 cmp  ax, 002eh              ;did we get '.' as an entry?
				 je   @@a3                   ;yes. skip it, read in next

				 mov  ax, numenteries
				 call _store_entry
				 inc  numenteries
				 jmp  @@a3

@@ex:            mov  ax, numenteries
				 sub  ax, numdirs            ;return numfiles
				 mov  numfiles, ax
				 ret
_get_file_names  endp
;---------------------------------------------------------------------
				 PUBLIC _getnumfiles
_getnumfiles     PROC
					  mov  ax, numfiles
					  ret
_getnumfiles     endp
;----------------------------------------------------------------------
					  PUBLIC _getnumdirs
_getnumdirs      PROC
					  mov  ax, numdirs
					  ret
_getnumdirs      endp
;---------------------------------------------------------------------
;compare string b and string d
;if string b <= d don't bother to sort
;Entry:
; base = the offset of dirbuf
;exit:
; c = 0  and ax =0 no sort
; c = 1  and ax =1 yes sort
_compare_string  PROC C near uses ds es di si

					  mov  ax, b
					  dec  ax
					  mov  cx, 22
					  mul  cx
					  add  ax, base
					  mov  si, ax

					  mov  ax, d
					  dec  ax
					  mul  cx                     ;cx already =22
					  add  ax, base
					  mov  di, ax

						mov  fsi, si                ;save for easy use in swap
						mov  fdi, di

						mov  ax, dirbuf
						mov  es, ax
						mov  ds, ax
						mov  bx, 0
						mov  cx, 12                 ;len of short entry name

						add  si, 9                  ;point to st of name
						add  di, 9

						cld
@@a1:             lodsb                       ;al = ds:[si] inc si
						mov  ah, [di]              ;al = char of string b,
															;ah = char of string d

						cmp  al, 0                 ;end of string b?
						je   @@noswap
						cmp  ah, 0
						je   @@doswap               ;string b> string d
						cmp  al, ah
						jb   @@noswap               ;b < d
						ja   @@doswap               ;b > d
						inc  di
						loop @@a1

@@noswap:         clc
						jnc  @@ex1                  ;qjmp

@@doswap:        stc
@@ex1:           ret
_compare_string  ENDP
;---------------------------------------------------------------------
;This function swaps the short enteries d and b (offset base)
;Entry:
; b = r # of entry to exchange with entry d
; base = the adr offset 0 for dirs or (22* numdirs) for files

_swap_enteries   PROC C near uses es ds si di

					  mov  di, fdi
                 mov  si, fsi
                 mov  ax, dirbuf
                 mov  es, ax
                 mov  ds, ax
                 mov  cx, 11                 ;22 bytes to swap (11 words)
                 xor  bx,bx
                 cld
@@a1:            lodsw                       ;= {ax = ds:[si]  ; add si,2 }
                 xchg ax, [di]
                 mov  [si-2], ax
                 inc  di
                 inc  di
                 loop @@a1

					  ret
_swap_enteries   ENDP
;---------------------------------------------------------------------
; int set_path(char *text);
;This function sets as the new drive & path as the default one.
;entry:
;  text  = the string specifying the full drive and path
;  ie., i = set_path("C:\\WINDOWS");
;Exit:
;  c = 1 & ax = 0 if no string
;  else c = 0 and ax = len of string

                 PUBLIC _set_path
_set_path        PROC C uses ds es di si,\
                 text: ptr word

                 push ds
                 pop  es                     ;make sure es = data seg
                 mov  di, text
                 call str_len                ;get len of user's string
					  mov  userslen, al
                 cmp  al, 0
                 jne  @@a1
@@err:           stc
                 mov  ax, 0
					  jmp  @@ex                   ;error no string

;copy user's string into filespec so that we can alter it if need be
@@a1:            mov  si, di                 ;di = adr of user's str text
                 lea  di, filespec
                 mov  cx, ax                 ;ax = len of user's str
                 cld
                 rep  movsb                  ;same as
                                             ;strcpy(filespec, userstr);

                 lea  di, filespec
                 mov  bx, ax                 ;r len of string
                 mov  [di+bx], ah            ;null term filespec
                 cmp  bx, 4                  ;is len < 4?
                 jb   @@h2                   ;if only something line "C:\"
					  mov  al, [di+bx-1]          ;last char: is it '\'
                 cmp  al, '\'
                 jne  @@h2
                 mov  al, 0
                 mov  [di+bx-1], al          ;delete it
					  dec  byte ptr userslen
@@h2:
					  mov  dx, [di]               ;get drive letter of path
					  cmp  dh, ':'
					  jne  @@err                  ;format of string bad
					  and  dl, 95                 ;make sure its in upper case
					  cmp  dl, 65
					  jb   @@err
					  cmp  dl, 90
					  ja   @@err                  ;drive letter not in
															;range 'A' - 'Z

					  call _get_current_drive     ;di, si & dx is preserved
					  add  al, 64                 ;al = letter of current drive

					  cmp  al, dl
					  je   @@h1                   ;the new directory is on the
															;same drive.
															;No need to change drives
;chage the drive to the one specified in the user's path string
					  sub  dl, 64
					  mov  dev, dl                ;dev 'A' = 1
					  dec  dl
					  mov  ax, 0e00h              ;"change drive"
					  int  21h
;Set the current directory
@@h1:            push di
					  call cs:_GetDosVer             ;small or long names?
					  pop  di                     ; c =0 if small, c =1 if long
					  mov  dx, di                 ;addr of user's path

					  mov  ax, 3b00h              ;"Set Current Directory"
					  jnc  @@small                ;small names
					  mov  ax, 713bh              ;long names
@@small:         int  21h
														  ;if function successful c=0
					  jc   @@err
					  mov  al, userslen
					  mov  ah, 0

@@ex:        	  ret
_set_path        ENDP
;---------------------------------------------------------------------
;This function tests to see if the floppy disk drive A is ready
;Entry:
;  sw = 0 just test read (ignore writeability of drive to save time)
;         else sw <> 0 text both read and writeability
;         This function is not called from any of the other functions
;         in this file. It is called from a function in class Disk
;         in file 'disk2.cpp'
;Exit:
;
;   ax=         0 drive read and writeable
;               1 no floppy drive present
;               2 can't read disk
;               3 write protect on
;
				 PUBLIC _is_drive_ready
_is_drive_ready  PROC C uses es ds di si, sw: ptr word

; first we check if drive a is an available floppy drive
				 mov  ax, 0
				 int  11h                    ;check if floppy drive present
				 test ax, 1
				 jnz  @@cont                 ;floppy drive is present, but
														;see if it is ready.

				 jmp  @@nofloppy             ;error: return ax = 0: no floppy

@@cont:      mov  ax, 3
				 mov  time, ax               ;attempt 3 times to read

@@again:     mov  ah, 0
				 mov  dl, 0
				 int  13h                    ;reset disk system

;see if there is a disk inside by trying to read from it
				 mov  ah, 2                  ;function read sector
				 mov  al, 1                  ;num of sectors to read
				 mov  ch, 1                  ;track 1
				 mov  cl, 1                  ;sector 0
				 mov  dh, 1                  ;head 0
				 mov  dl, 0                  ;drive a
				 push ds
				 pop  es
				 lea  bx, buf                ;es:bx = adr of buf to read sector
				 int  13h
				 jnc  @@fine                 ;cant read
				 dec  time
				 mov  ax, time
				 cmp  ax, 0
				 jne  @@again
				 jmp  @@noread
@@fine:
				 mov  ax, sw
				 cmp  ax, 0
				 je   @@ex                   ;drive readable, don't test
													  ;writability
				 mov  ax, 3                  ;attempt 3 times to write
				 mov  time, ax
;reading ok but is the disk write protected?
;check this by trying to write back the same data we just read in
;and to the same sector.

@@again2:    mov  ah, 0
				 mov  dl, 0
				 int  13h                    ;reset disk system

				 mov  ah, 3                  ;function write sector
				 mov  al, 1                  ;# of sector to write 1
				 mov  ch, 1                  ;track 1
				 mov  cl, 1                  ;sector 0
				 mov  dh, 1                  ;head 0
				 mov  dl, 0                  ;drive a
				 push ds
				 pop  es
				 lea  bx, buf                ;adr of data to send to disk
				 int  13h
				 jnc  @@ok
				 dec  time
				 mov  ax, time
				 cmp  ax, 0
				 jne  @@again2               ;make another attempt to write

				 mov  ax, 3                  ;write protect on
				 jmp  @@ex

@@ok:        mov  ax, 0                  ;drive read (read and writeable)
				 jmp  @@ex

@@noread:    mov  ax, 2                  ;cant read
				 jmp  @@ex

@@nofloppy:  mov  ax, 1                  ;no floppy drive present
													  ;fall through to @@ex
@@ex:        ret
_is_drive_ready  ENDP
;-----------------------------------------------------------------------
; This function tries to read in the current path from the default
; device using the long filenames function. If the attempt fails it means
; long filenames functions are not suppported.

;Exit
;  c = 0 if long names supported
;  C = 1 Dos is in down-level mode

				 PUBLIC _GetLongPath
_GetLongPath PROC
				 mov  ax, 7147h 				;get current path
				 mov dl, 0
				 lea si, buf               ;512 bytes   where path will be if
													;successful
				 stc
				 int 21h
				 ret
												;C = 1 if error: dos in down-level mode
_GetLongPath ENDP
;-------------------------------------------------------------------------
;This function gets the DOS version number
;This function is useful in order to know if DOS supports long file names
;if upon return AL => 7 then Windows95 (or greater) DOS, so long
;names supported.
;
;Exit:
; ax = DOS version number
; al = major version #  (MS DOS 4.01=4 ect) (if MS DOS 1.0 al =0)
; ah = minor version number (MS-DOS 4.01= 01, ect)
;
; BX = 0 if dos 6 or previous
;      1 if dos 7
;
;  C = 1 if version > 6 (ie., if its dos 7 or gerater)
; and BX =1, else c =0 bx =0

				 PUBLIC _GetDosVer
_GetDosVer   PROC  C  uses si di
				 call _GetLongPath

				 jc    @@oldver

				 stc
				 mov ax, 0107h				  ;supports long names
				 mov  bx, 1
				 ret

@@oldver:    clc
				 mov  bx, 0
				 mov ax, 0206h             ;dos version 6.2
				 ret
_GetDosVer   ENDP
;----------------------------------------------------------------------
;(Not callable from C++)
;This is the function that should be called to get get long file names
;DOS 7 (or higher).
;This function is a combination of FindFirst & FindNext
;and FindClose
;upon entry:
; if ax > 0 and <= 255 it calls DOS's FindFirst,
; else if ax = 0 it calls DOS's findnext,
; else ax is >255 so it calls file close, but only if
; a valid file handle was got for a previous call
;
; filespec = something like "C:\WINDOWS\*.*"+0
; Of itself this function will not call fileclose unless the caller
; requests it by flagging ax upon entry with  a number bigger than 255
;
; entryattrib = the attribute to give dos to find an entry
;
; Exit:
;  c=0 if successful
;       and  -- filedata -- contains the entry found. offset 44 is
;       the start of the long name
;       and errorvar = 0
; else
;  c = 1 not found and errorvar contains reason of why not successful
;

_long_find_file  PROC C uses ds es di si

				 push ds
				 pop  es                     ;make sure es = ds

				 lea  dx, filespec           ;example "D:\WINDOWS\*.*"
				 lea  di, finddata
				 mov  cx, entryattrib        ;attribute for findfirst
				 cmp  ax, 0
				 je   @@findnext             ;find next if no prev error
				 cmp  ah, 0
				 je   @@con
				 jmp  @@findclose            ;ax >255
;-------------------------
;calling FindFirst for the first time
;so we must invalidate the filehandle
;Then when we call finfFirst if we are successful we store the
;file handle in filehandle
@@con:       mov  errorvar, 0            ;clr error status

				 mov  ax, 714eh              ;findfirst for long file names
				 mov  si, 1                  ;no nano sec stuff in time field
				 int  21h                    ;"LongFindFirst"

				 jnc  @@noerror
				 mov  errorvar, ax
				 jmp  @@ex                   ;c =1 error & filehandle =33

@@noerror:    cmp  ax, 7100h              ;finished?
				  jne  @@cont                 ;ax = valid filehandle

;No error occured but we did not find a match
					mov  errorvar, ax
					stc                         ;c =1, errorvar = 7100h, and
					jmp  @@ex                   ;filehandle =33

;ax = valid file handle

@@cont:        mov  filehandle, ax         ;handle for findnext

@@co:          clc
					jmp  @@ex                   ;found a valid entry
														 ;its stored in filedata
;----------------------------
;(jumped to here from top ax =0)
;We assume that in a previous call to this routine
;findfirst was called and was successful
;now we call find next giving it the filehandle we got from a
;previous call to _long_find_file
@@findnext:  mov  ax, filehandle

@@cont2:     mov  bx, errorvar
				 cmp  bx, 0
				 je   @@cont3

				 stc                         ;last entry previously got
				 jmp  @@ex                   ;exit
;--------------------------------
;We have a valid file handle from a previous call
;and errorvar = 0 so we can try to look for another entry
@@cont3:
				 mov  ax, 714fh              ;find next
				 mov  si, 1                  ;return normal date/time format

				 mov  bx, filehandle
				 push ds
				 pop  es
				 lea  di, finddata           ;es:di = adr to return data in
				 int  21h
				 jnc  @@noerror1
				 mov  errorvar, ax
				 jmp  @@ex

@@noerror1:      cmp  ax, 7100h              ;finished code?
				 clc
				 jne  @@ex
				 mov  errorvar, ax
				 stc
				 jmp  @@ex
;---------------------
;(jumped to here from top)
@@findclose:
				 mov  bx, filehandle
				 mov  ax, 71a1h
				 int  21h                    ;return the handle # to dos

				 jnc  @@noerror2
				 mov  errorvar,ax
				 jmp  @@ex                   ;c =1 error closing find

@@noerror2:  cmp  ax, 7100h
				 clc
				 jne  @@ex

				 mov  errorvar,ax            ;drup through to @@ex
													  ; c =0
;----------------------
@@ex:        ret
											 ;C=1 if not found, c=0 found
_long_find_file  ENDP
;---------------------------------------------------------------------------
;This function is called for long file names (DOS 7 for Windows 95).
;(called by _long_sort_all_enteries)
;mustn't be called from C++
;get the names of all the dirs and store them from dirbuf
;
;Entry:
; fuser_string = adr of user string '*.*'    to get all dirs
;
; exit:
;  ax = r # of dir enteries got
;  the dir enteries will be stored from the beginning of farseg dirbuf
;  see the long dir structure at the data section of this file

_long_get_dir_names   PROC C near uses ds es di si

				 mov  ax, 0
				 mov  numdirs, ax            ;reset number of dirs + files
				 mov  numfiles, ax           ;to zero.
				 mov  numenteries, ax

				 call _getcurrentpath        ;get drive & path into
				 jnc  @@a1                   ;currentpath
				 jmp  @@ex1                  ;error: something invalid!

@@a1:        push ds
				 pop  es
				 lea  si, currentpath
				 lea  di, filespec
				 call _str_copy              ;copy from currentpath into
													;filespec
													;ax = len of currentpath
													;di = byte past final null
;append the masbuf ie., ('*.*') to the filespec
				 lea  di, currentpath
				 call str_len
				 lea  di, filespec
				 add  di, ax


				 mov  si, fuser_string       ;users supplied mask such as
													  ;'*.*'
				 call _str_copy              ;add mask to filespec
				 add  al, pathlen
													  ;ax = r len of filespec
				 jmp  @@a2                   ;find first

@@a3:        mov  ax,0                   ;flg find next if ax =0
@@a2:        call _long_find_file        ;it receives filespec & ax
				 jc   @@ex                   ;and ret w/ entry in finddata
													  ;if unsuccessful c=1
;check to see if a valid dir was got
				 mov  al, byte ptr [finddata]
				 and  al, 16                 ;mask in dir bit only
				 cmp  al, 16                 ;is it a dir?
				 jne  @@a3                   ;no. read in next entry
				 mov  ax, word ptr [finddata+44]
				 cmp  ax, 002eh              ;did we get '.' as an entry?
				 je   @@a3                   ;yes. skip it, read in next

				 mov  ax, numdirs
				 call cs:_long_store_entry
				 inc  numdirs
				 jmp  @@a3

@@ex:        mov  ax, 256
				 call _long_find_file        ;close the file

@@ex1:       mov  ax, numdirs
				 mov  numenteries, ax        ;return num dirs

				 ret
_long_get_dir_names   endp
;----------------------------------------------------
;This function is called for long file names (DOS 7 for Windows 95).
;(called by _long_sort_all_enteries)
;mustn't be called from C++
;get the names of all the dirs and store them from dirbuf
;
;Entry:
; fuser_string = adr of user string '*.*'    to get all files
;
; exit:
;  ax = r # of file enteries got
;  the file enteries will be stored after the dirs enteries in seg dirbuf
;  see the long dir structure at the data section of this file

_long_get_file_names  PROC C near uses ds es di si

				 mov  ax, 0
				 mov  numfiles, ax           ;reset number of files

				 call _getcurrentpath        ;get drive & path into
				 jnc  @@a1                   ;currentpath
				 jmp  @@ex1                  ;error: something invalid!

@@a1:        push ds
				 pop  es
				 lea  si, currentpath
				 lea  di, filespec
				 call _str_copy              ;copy from currentpath into
													;filespec
													;ax = len of currentpath
													;di = byte past final null
;append the masbuf ie., ('*.*') to the filespec
				 lea  di, currentpath
				 call str_len
				 lea  di, filespec
				 add  di, ax
				 mov  si, fuser_string       ;users supplied mask such as
											 ;'*.*'
				 call _str_copy              ;add mask to filespec
				 add  al, pathlen
													;ax = r len of filespec
				 jmp  @@a2                 ;find first

@@a3:        mov  ax,0                   ;flg find next if bx =0
@@a2:        call _long_find_file        ;it receives filespec & ax
				 jc   @@ex                   ;and ret w/ entry in finddata
													  ;if unsuccessful c=1
;----------
;check to see if a valid file entry was got

				 mov  al, byte ptr [finddata] ;attribute

				 test al, 16+8+4+2           ;is it  dir, label, sys,hidden?
				 jnz  @@a3                   ;skip: read in next entery
				 mov  ax, word ptr [finddata+44]
				 cmp  ax, 002eh              ;did we get '.' as an entry?
				 je   @@a3                   ;yes. skip it, read in next

				 mov  ax, numenteries
				 call cs:_long_store_entry
				 inc  numenteries
				 jmp  @@a3
;----------
@@ex:        mov  ax, 256
				 call _long_find_file        ;close the file

@@ex1:       mov  ax, numenteries
				 sub  ax, numdirs
				 mov  numfiles, ax           ;r # of file enteries stored
				 mov  ax, numenteries
				 ret
_long_get_file_names  ENDP
;----------------------------------------------------
;This function stores  the finddata entry in far data dirsbuf
;at c entry pos in in ax
; it is only called for long DOS 7 type enteries.
; the first 24000 bytes are reserved for 2000 12-byte blocks for entry vars
; block looks like:
;  offset    bytes   meaning
;   0          2       addr of entery's long filename's upper part
;   2          2       file attribute
;   4          2       time hh:mm:ss
;   6          2       date  yy/mm/dd
;   8          4       file size  (upto 4 billion bytes)
;   next 12 byte block
;   12         2        addr of next entery's long filename's upper part
;   ect
;--------
; The upper part of the first entry begins at base offset 12000
;  2400 + offset      bytes       meaning
;            0         2            len of text of long file name
;            2         len          text of file name
;  next entry's upper part is:
;  2400 +   len of text of previous name +2
;
;Entry:
;  ax = c # of pos to store entry

_long_store_entry     PROC C near uses ds es si di
											 ;ax =the c # of the entry
				 lea  bx, finddata
				 mov  cx, 12
				 mul  cx
				 mov  dx, dirbuf
				 mov  es, dx
				 mov  di, ax                 ;es:di = 12-byte block of dir vars

				 mov  ax, ds:[bx]            ;finddata
				 mov  es:[di+2], ax          ;store attrib

				 mov  ax, ds:[bx+20]         ;modification time hh:mm:ss
				 mov  es:[di+4], ax

				 mov  ax, ds:[bx+22]         ;modification date yy/m/dd
				 mov  es:[di+6], ax

				 mov  ax, ds:[bx+32]         ;file size  lo word
				 mov  es:[di+8], ax

				 mov  ax, ds:[bx+ 34]        ;file size hi word
				 mov  es:[di+10], ax

;calculate the new addr for the long name
;if it is the first entry use 24000 as its addr
				 cmp  di, 0
				 jne  @@a1
				 mov  ax, 24000
@@a2:        mov  es:[di], ax            ;= adr of new names len
				 jmp  @@a3                   ;store long name at this
													  ;adr+2 (ie., at 24002 for 1st
													  ;entry)
; find addr of next free space for addr of new long name
@@a1:        mov  si, es:[di-12]         ;addr of previous long name
				 mov  ax, es:[si]            ;len of previous long name
				 add  ax, si                 ;add len to prev addr
				 add  ax, 2                  ;add 2  to skip past prev name
				 jmp  @@a2                   ;ax= addr of new entry


@@a3:        mov  di, ax                 ;new name addr where it len is
				 mov  dx, ax                 ;temp store
				 add  di, 2                  ;adr of new name text
				 lea  si, finddata+44        ;adr of long file name
				 call _str_copy
				 mov  di, dx                 ;adr to store new name's len
				 mov  es:[di], ax            ;store len of name string
				 ret

_long_store_entry     endp
;--------------------------------------------------------------------
;This routine returns to the caller a long entry num (either dir or file)
;from the fardata dirbuf (called from _fetch_stored_entry)
;Entry:
; textbuf = the adr of the user's supplied buffer to store the entry in
; because the entry can be upto 270 bytes the user should make sure
; he has reserved enough memory in his buffer, otherwise the systen
;could crash.
;
; num = the c # of the entry to get
;       for example if num = 0 it usually means entry '..' (the
;       spec for DOS to go Back to the previous parent dir
;       if num = 1 if usually points to the first dir (if there are any).
;       The dirs (if any) come first, then the files
;Exit:
; ax = 0 if the entry number was out of range
; else  ax = the r total  # of all enteries in the fardata dirbuf
;The result will be in the format
; offset   bytes                       meaning
;   0        1                        attribute   dir/file
;   1        2                        time hh:mm:ss
;   3        2                        date dd/mm/yy
;   5        4                        file size upto 4 billion bytes
;   9    unto end of string           text of name null-terminated
;Under dos 7 the size of the name part of the string can be
;upto 260 bytes long
; c =1 if error and ax =0

_long_get_stored_entry PROC C near uses ds es di si,\
				 textbuf: ptr word, num: ptr word
				 push ds
				 pop  es

				 mov  ax, num
				 cmp  ax, numenteries
				 jc   @@ok                   ;num smaller.
				 mov  ax, 0
				 jmp  @@ex                   ;num>= numenteries error:
													  ;because num is from 0 upwards
													  ;where as
													  ;numenteries is from  1 upwards

@@ok:        mov  cx, 12
				 mul  cx
				 mov  si, ax
				 mov  ax, dirbuf
				 mov  ds, ax                 ;ds:si = adr of entry's vars
				 mov  di, textbuf            ;es:di = adr of user's buf

				 mov  al, ds:[si+2]          ;get entery's attribute
				 mov  es:[di], al

				 mov  ax, ds:[si+4]          ;enteriy's time
				 mov  es:[di+1], ax

				 mov  ax, ds:[si+6]          ;entery's date
				 mov  es:[di+3], ax

				 mov  ax, ds:[si+8]          ;entery's file size lo word
				 mov  dx, ds:[si+10]         ;entery's file size hi word
				 mov  es:[di+5], ax
				 mov  es:[di+7], dx

;now we must get the adr of the name and copy the name into the
;user's buf at offset 9
				 mov  si, ds:[si]            ;si = adr of upper part
				 mov  cx, ds:[si]            ;get len  of name
				 add  si, 2                  ;skip past 2 len bytes

				 add  di, 9                  ;offset 9 of user's buf
				 cld
				 rep  movsb                  ;copy from ds:[si] to es:[di]
				 mov  byte ptr es:[di], 0    ;null-term user's string
				 clc                         ;c = 0 ok
@@ex1:       mov  ax, ss:numenteries
@@ex:        ret
_long_get_stored_entry    endp
;------------------------------------------------------------------------
;compare long name string b and string d
;if string b <= d don't bother to sort
;Entry:
; base = the offset of dirbuf
;exit:
; c = 0  no swap
; c = 1  yes swap
_long_compare_string  PROC C near uses ds es di si

				 push ds                     ;save ds
				 push es

				 mov  ax, dirbuf
				 mov  ds, ax
				 mov  es, ax

				 mov  ax, ss:b
				 dec  ax
				 mov  cx, 12
				 mul  cx
				 add  ax, ss:base
				 mov  si, ax                 ;adr of vars 12-byte block for
													  ;entry b
				 mov  ss:varsb, ax           ;save for quick use by swap fnc
				 mov  si, ds:[si]            ;get adr of upper part
				 add  si, 2                  ;ds:si=adr of text of long name

				 mov  ax, ss:d
				 dec  ax
				 mul  cx                     ;cx =12
				 add  ax, ss:base
				 mov  di, ax                 ;adr of vars 12-byte block for
													  ;entry d
				 mov  ss:varsd, ax           ;save for quick use by swap fnc
				 mov  di, ds:[di]            ;get adr of upper part
				 add  di, 2                  ;ds:di=adr of text of long name
				 mov  cx, [si-2]             ;get r len of name b
				 mov  dx, [di-2]             ;get r name of name d
				 mov  bx, 0

@@a1:        mov  al, [si+bx]            ;name of entry b
				 mov  ah, [di+bx]            ;name of entry d
				 cmp  bx, cx
				 je   @@noswap               ;past end of name b
				 cmp  bx, dx
				 je   @@doswap               ;past end of name d
													  ;which mean b is > d
				 and  al, 255 -32            ;convert to uppercase
				 and  ah, 255 -32
				 cmp  al, ah
				 jb   @@noswap               ;no swap
				 ja   @@doswap
				 inc  bx
				 jmp  @@a1

@@doswap:    stc                         ;c =1  swap
				 jmp  @@ex1

@@noswap:    clc

@@ex1:       pop  es
				 pop  ds
				 ret
_long_compare_string  ENDP
;---------------------------------------------------------------------
;This function swaps only the vars of the long enteries d and b
; It does not swap the actual strings. But the effect is just the
;same for the purpose of directory enteries
;Entry:
; varsb = adr of vars to exchange with varsd
_long_swap_enteries   PROC C near uses es ds si di

				 push es
				 push ds                     ;save ds

				 mov  cx, 6                  ;12 bytes = 6 words
				 mov  si, varsb
				 mov  di, varsd

				 mov  ax, dirbuf
				 mov  ds, ax
				 mov  es, ax

@@a1:        lodsw                       ; mov ax, ds:[si] : add si, 2
				 xchg ax, [di]
				 mov  [si-2], ax
				 inc  di
				 inc  di
				 loop @@a1

				 pop  ds
				 pop  es
				 ret
_long_swap_enteries   ENDP
;------------------------------------------------------------------------
; This in an implimentation of the Shell/Metzner algorythem
;In BASIC it would look like this:
;1 gap = 2048
;2 gap = gap /2
;3 if gap >= numenteries goto 2
;4 a = 1
;5 c = numenteries - gap
;6 b = a
;7 d = b + gap
;8 i = compare_string(base, b, d);
;9 if i = 1 goto 13       :rem do swap
;10 a = a+1
;11 if a> c goto 17       :rem do smaller gap (if possible)
;12 goto 6
;13 swap_entry(base, b, d);
;14 x = b - gap: if x >= 1 goto 16
;15 goto 10
;16 b = x : goto 7
;17 if gap = 1 goto 19:     rem exit
;18 gap = gap /2 : goto 4
;19 exit
;
;Entry:
;   ax = 0 sort dir enteries
;   ax = 1 sort file enteries
;   dosversion = 0 sort for small names  (DOS 6.2 and pervious versions)
;   dosversion = 1 sort for long names    (DOS 7 Windows 95 version)
;Exit:
;  The enteries are sorted and in fardata seg dirbuf

_sort_enteries   PROC near

;set up the compare and swap routine version based on the value in bx
				 mov  si, dosversion         ;0 = short names, 1 = long
				 sal  si, 1
				 sal  si, 1
				 lea  bx, routines
				 mov  cx, [si+bx]            ;adr of compare short / long
				 mov  compare, cx
				 mov  cx, [si+bx+2]          ;get adr of swap short/ long
				 mov  swap, cx
				 mov  dx, 22                 ;assume short files
				 cmp  si, 0                  ;really short names?
				 je   @@m1
				 mov  dx, 12                 ;no, its long names
													  ;so dx = its vars multiplier

@@m1:        mov  cx, dx   	          ;multiplier, either 22 or 12
				 mov  bx, ax                 ;0 or 1 (dirs or file enteries)

				 mov  ax, numdirs            ;assume the caller wants us to
													  ;sort dirs, rather than files

				 mul  cx                     ;numdirs * 12, or *22
				 mov  cx, bx                 ;0 or 1 (dirs, or files)
				 mul  cx                     ;ax = 0 if dirs, or ax =
													  ;stadr of files if files are
				 mov  base, ax               ;to be sorted
				 mov  ax, 4096               ;the gap will be half this
				 mov  dx, numdirs            ;the value for n
				 test bx, 1                  ;files ?
				 jz   @@a1                   ;no, dirs
				 mov  dx, numfiles
@@a1:        mov  n, dx                  ; n = r # of elements in list
@@a2:        sar  ax, 1                  ;gap /2
				 cmp  ax, n                  ;if ax >= n  (divide by 2 again)
				 jae  @@a2
				 mov  gap, ax
;a = 1
@@h4:        mov  ax, 1
				 mov  a, ax
;c = n - gap
				 mov  ax, n
				 sub  ax, gap
				 mov  c, ax
;b = a
@@h6:        mov  ax, a
@@h65:       mov  b, ax

;d = b+ gap
@@h7:        mov  ax, gap
				 add  ax, b
				 mov  d, ax

; i = compare_string(base, b, d)
				 call [compare]              ;either the short or long version
				 jc   @@doswap

;a = a+1
@@h10:       inc  a

;if a> c goto h17
				 mov  ax, a
				 cmp  ax, c
				 ja   @@h17
				 jmp  @@h65                  ;goto 6

;swap_entry(base, b, d)
@@doswap:    call [swap]                 ;either the short or long version


;if (b-gap) >=1 goto 16
				 mov  ax, b
				 sub  ax, gap
				 jnc  @@kp                   ; no carry means  b is not smaller


				 jmp  @@h10                  ;no backwards test this time
@@kp:            cmp  ax, 1
				 jae  @@h16

				 jmp  @@h10                  ;no backwards test this time cause

;b = b - gap
@@h16:       mov  b, ax
				 jmp  @@h7                   ;backwards test

;if gap = 1 goto exit
@@h17:       mov  ax, gap
				 cmp  ax, 1
				 je   @@ex
;gap = gap /2
				 sar  ax, 1
				 mov  gap, ax
				 jmp  @@h4

@@ex:			 ret
_sort_enteries   ENDP
;---------------------------------------------------------------------
;This is the primary function in this file
;This function reads in all the dir and file names from the disk into
;fardata memory at seg dirbuf
;It then sorts them all.
;It uses the current drive and path as its file specification.
;
;Entry:
; i = sort_all_enteries(char *spec);
; example
;  i = sort_all_enteries("*.*");
;
;Exit:
; ax = total num of enteries read in

				 PUBLIC _sort_all_enteries
_sort_all_enteries    PROC C uses ds es si di,\
				 maskbuf: ptr word

				 mov  entryattrib, 16        ;looks for dirs

				 call _GetDosVer             ;rtns C =1 if dos 7
													  ;in order to decide which
													  ;routines to call (short or long)

				 mov  dosversion, bx         ;either 0 or 1
				 mov  si, bx
				 sal  si, 1
				 sal  si, 1
				 lea  bx, routines+8
				 mov  cx, [si+bx]
				 mov  GetDirNames, cx
				 mov  cx, [si+bx+2]
				 mov  GetFileNames, cx

				 call _setupdta              ;setup disk transfer area

				 lea  ax, staticdirmatch     ;adr of '*.*'  for dirs
				 mov  fuser_string, ax
				 call ds:[GetDirNames]          ;either short or long version

				 mov  ax, maskbuf
				 mov  fuser_string, ax
				 mov  entryattrib, 0         ;look for file enteries
				 call ds:[GetFileNames]         ;either short or long version

				 mov  ax, numdirs
				 cmp  ax, 1
				 jbe  @@a1
				 mov  ax, 0                  ;flg: sort dirs
				 call _sort_enteries         ;The function will decide
													  ;which version to call.
@@a1:        mov  ax, numfiles
				 cmp  ax, 1
				 jbe  @@ex
				 mov  ax, 1                  ;flg: sort files
				 call _sort_enteries         ;Function will decide which
													  ;version to call
@@ex:        mov  ax, numenteries
				 ret
_sort_all_enteries    ENDP
;---------------------------------------------------------------------
;This function decides which kind of an entry to return to the
;user from seg dirbuf
;if dosversion = 0 it returns a short entry
;else it returns a long name entry
;Entry:
;  textbuf = the user's string to copy the entry into
;             it must be 270 bytes long for long enteries
;             long enteries will be returned to the user in the
;             same format as short enteries, whith the name text
;             starting at offset 9
;             the only difference is that the names will be longer than
;             12 bytes in most cases. But the attrib, file size, tims/date
;             bytes will be in the same positions as for short names.
;
;  num = the c # of the entry to fetch from the store.
; Exit:
;  ax = the total number of enteries got
				 PUBLIC _fetch_stored_entry

_fetch_stored_entry   PROC C uses ds es di si,\
				 textbuf: ptr word, num: ptr word

				 mov  si, dosversion
				 sal  si, 1

				 lea  bx, routines+16        ;either _get_stored_entry
				 mov  cx, [si+bx]            ;or _long_get_stored_entry

;imitate a C++ call by pushing the args that the routine we are about
;to call will need.

				 mov  ax, num                ;the right most para
				 push ax                     ;=the c # of the entry to get
				 mov  ax, textbuf            ;the left most para =
				 push ax                     ;the adr of user's string to
													  ;store the entry in

				 call cx                     ;call either short or long version
				 add  sp, 4                  ;remove paras from stack

				 ret
_fetch_stored_entry   ENDP
;---------------------------------------------------------------------
				 END


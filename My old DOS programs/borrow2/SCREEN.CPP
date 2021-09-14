/*-------------------------------------------------------------------------
   (C) Michael Finnegan      15 May 1999
-------------------------------------------------------------------------*/

#include "screen.h"
#include "labels.h"
//-------------------------------------------------------------------------
Screen::Screen()
{

	//m_boxlevel = 0;
	m_attrib = BLUE*16+ LIGHTGRAY;
	m_x1 = 0;
	m_y1 = 0;
	memset(m_spacestring, ' ', 80);
	memset(m_dotstring, '.', 80);

	m_spacestring[80] = 0;
	m_dotstring[80]   = 0;

	//values for a no border box
	m_tlc[0] = 32;
	m_hline1[0] = 32;
	m_hline2[0] = 32;
	m_trc[0] = 32;
	m_vline1[0] = 32;
	m_vline2[0] = 32;
	m_blc[0] = 32;
	m_brc[0] = 32;

	//values for a thin line border box
	m_tlc[1] = 218;
	m_hline1[1] = 196;
	m_hline2[1] = 196;
	m_trc[1] = 191;
	m_vline1[1] = 179;
	m_vline2[1] = 179;
	m_blc[1] = 192;
	m_brc[1] = 217;

 //values for a double thin line border box
	m_tlc[2] = 201;
	m_hline1[2] = 205;
	m_hline2[2] = 205;
	m_trc[2] = 187;
	m_vline1[2] = 186;
	m_vline2[2] = 186;
	m_blc[2] = 200;
	m_brc[2] = 188;

	//enable bright background colours
	asm{
			 mov ax, 1003h
			 mov bx, 0
			 int 10h
		}

}
//--------------------------------------------------------------------------
//return the m_x1 and m_y1 vars
POINT Screen::Getxy()
{
	POINT point;

	point.x = m_x1;
	point.y = m_y1;

	return point;
}

//--------------------------------------------------------------------------
//set new screen coordinates to be used as default args by
//Screen:WriteLine()
//
// Exit:
//  ax = -1 if error  (default m_x1 m_y1 not changed)
//  else ax = 0; ok

int Screen::Setxy(int x, int y)
{
	if(x < 0 || x> 79) return -1;
	if(y < 0 || y> 24) return -1;

	m_x1 = x;
	m_y1 = y;

	return 0;
}
//--------------------------------------------------------------------------
//Write a line of text directly onto the screen at coordinates c x1, y1
// using either default or users attribute
//Entry:
// if no x1 coord given use default m_x1 instead
// if no y1 coord given use default m_y1 instead

//   size = -1  no size par given   (routine will determine it)
//if size is given an it is less than the actual size of the text string
//only that many chars specified by size will be written. It will appear as
//if the text string is truncated.
//if size if greater than the actual size of the text string
//it means this is the field width to write into
// therefore extra spaces will be written onto the end of the string, but
//the spaces are no added onto the text string, only what is displayed.

//   attrib = -1 no attrib par given (use default)
//  blanks = 0 use normal blanks
//  blanks = 1 use '.' instead (as field filler)
//
//Exit:
//  ax = -1 if error
//  else ax = actual len of string
// after writing a string the m_y1 is incremented to the next line if
// not already on last line.
// m_x1 not altared except if user supplied an x1 upon entry


int Screen::WriteLine(char *text, int x1, int y1, int size,
							 int attrib, int blanks)
{

	int t, userlen, color, actuallen, m= mousehidden;
	char linebuf1[82];

	if(!m) this->HideMouse();     //hide mouse before writing to screen

	//if x1 given use it, else use default m_x1
	if(x1 != -1)
	{
		if(x1 > 79) return -1;
		m_x1 = x1;
	}


	//if y1 given use it, else use default m_y1
	if(y1 != -1)
	{
		if(y1 > 24) return -1;
		m_y1 = y1;
	}


	//if no string size given determine it
	actuallen = strlen(text);
	if( size == -1) userlen = actuallen;
	else
	userlen = size;    //user has told us the size which may be smaller than
							//the actual size of the string

	if( userlen > 80) return -1;

	strncpy(linebuf1, text, userlen);
	//if user specified a field width greater than the actual length of
	//the text string, add the extra spaces to the copy of the text string.
	if(actuallen< userlen)
	{
		t = userlen - actuallen;
		if(blanks ==1)
		strncat(linebuf1, m_dotstring, t);
		else
		strncat(linebuf1, m_spacestring, t);
	}

	// if user supplied a colour use it instead of the previous default one
	if(attrib != -1)
	{
		color = attrib;
		m_attrib = color;
	}
	else color = m_attrib;          //use default colour


	t = (m_y1 * 160) + (m_x1 * 2);
	asm{
		mov dx, t
		push si
		push di
		mov ax, 0b800h
		mov es, ax
		mov di, dx    //es:di =  addr of screen line to write to.
		lea si, linebuf1
		mov ah, byte ptr color
		mov cx, userlen }

a1:asm{ mov al, ds:[si]   //get char from string
		  cmp al, 13
		  jne m1
		  mov al, 32}
m1:asm{ mov es:[di] , ax  //write it with attrib to screen
		  add di, 2         //next screen pos
		  inc si
		  loop a1

		  pop  di
		  pop  si
		}

	// advance m_y1 to the next row if legal, else ignore
	if(m_y1< 24) m_y1++;

	if(!m)this->ShowMouse();         //show mouse after writing to screen
	return actuallen;
}
//------------------------------------------------------------------------
//colour a line without overwriting its text
int Screen::ColourLine(int x1, int y1, int size, int attrib)
{

	int t, userlen, color, m= mousehidden;

	if(!m) this->HideMouse();      //hide mouse before colouring the screen

	if(x1 <0 || x1> 79) return -1;
	m_x1 = x1;

	if(y1 < 0 || y1> 24) return -1;
	m_y1 = y1;

	userlen = size;    //user has told us the size which may be smaller than
							 //the actual size of the string

	if( userlen < 0 || userlen> 80) return -1;

	// if user supplied a colour use it instead of the previous default one
	if(attrib != -1)
	{
		color = attrib;
		m_attrib = color;
	}
	else color = m_attrib;          //use default colour

	t = (m_y1 * 160) + (m_x1 * 2);
	asm{
		 mov dx, t
		 push si
		 push di
		 mov ax, 0b800h
		 mov es, ax
		 mov di, dx    //es:di =  addr of screen line to write to.
		 mov ah, byte ptr color
		 mov cx, userlen }

a1:asm{ mov byte ptr es:[di+1] , ah  //write attrib only to screen
		  add di, 2                    //next screen pos
		  loop a1

		  pop  di
		  pop  si
		}

   // advance m_y1 to the next row if legal, else ignore
   if(m_y1< 24) m_y1++;

   if(!m) this->ShowMouse();        //Show mouse after colouring the screen
   return userlen;
}
//------------------------------------------------------------------------
//draw a box of a particular style
//Exit:
// ax = -1 error

int Screen::DrawBox(int x1, int y1, int x2, int y2, int attrib, int style)
{
	int s, t, outsidecolumns, outsiderows;
	char linebuf1[81];

	s = 0;
	if(style != -1) s= style;
	else s = 1;  //default to a thin border box if no style given

	if( s <0 || s> 2) return -1;

	outsiderows = (y2 - y1)+1; //r num of rows the box takes up
	outsidecolumns = (x2 - x1)+1; //r num of columns the box takes up


	if(outsiderows >25) return -1;
	if(outsidecolumns >80) return -1;


	//prepare to draw top line of box including both corners
	memset(linebuf1, m_hline1[s], outsidecolumns);
	linebuf1[0] = m_tlc[s];
	linebuf1[outsidecolumns-1] = m_trc[s];
	linebuf1[outsidecolumns] = 0;
	Screen::WriteLine(linebuf1, x1, y1, outsidecolumns, attrib);

	//prepare to draw bottom line of box including both corners
	memset(linebuf1, m_hline2[s], outsidecolumns);
	linebuf1[0] = m_blc[s];
	linebuf1[outsidecolumns-1] = m_brc[s];
	Screen::WriteLine(linebuf1, x1, y2, outsidecolumns, attrib);

	//draw the 2 vertical sides
	memset(linebuf1, ' ', outsidecolumns);
	linebuf1[0] = m_vline1[s];
	linebuf1[outsidecolumns-1] = m_vline2[s];
	s = outsiderows -2;

	for(t = 0; t< s; t++){
		Screen::WriteLine(linebuf1, x1, y1+1+t);
	}

	return 0;
}
//------------------------------------------------------------------------
//You should call Screen::CloseBox() before calling this
//function if you want to restore any previously saved screen
//otherwise you will have layers of Menu boxes open
//and evently the routine will fail when m_boxlever =7
//Exit:
// a screen is saved in the buffer. it should normally be restored
//using Screen::CloseBox() before you call this function to open another.
int Screen::OpenShadowBox(int x1, int y1, int x2, int y2, int attrib,
								  int style)
{
	int t, rows, columns;

	rows = (y2 - y1)+1;
	if(rows <0 || rows > 24) return -1;

	columns = (x2 - x1) +1;
	if(columns <0 || columns > 80) return -1;

	//save the save the current screen, before drawing menu box
	if(m_boxlevel ==7) return -1;  //can't save screen: buffer full
	this->Pcopy(0, m_boxlevel+1);
	m_boxlevel++;

	//draw the vertical black shadow on the right side of the box
	for(t = 0; t< rows; t++){
		Screen::ColourLine(x2+1, y1+t+1, 2, BLACK*16+DARKGRAY);
	}
	//draw the horozontal black shadow under the box
	Screen::ColourLine(x1+ 2, y2+1, columns-1, BLACK*16+DARKGRAY);

	//draw the box proper
	Screen::DrawBox(x1, y1, x2, y2, attrib, 0);
	Screen::DrawBox(x1+1, y1, x2-1, y2, attrib, style);
	return 0;
}
//------------------------------------------------------------------------
//This function clears a  portion or all of the screen
//The coordinates represent the area to be cleared
int Screen::ClrScr(int x1, int y1, int x2, int y2, int attrib)
{
	int i, m= mousehidden;
	if(!m) this->HideMouse();
	i= Screen::DrawBox(x1, y1, x2, y2, attrib, 0);

	if( i != -1)
	{
		m_x1 = x1;
		m_y1 = y1;
	}

  if(!m) this->ShowMouse();
  return i;

}

//------------------------------------------------------------------------
//scroll up an area of the screen
//This is faster than the BIOS version
//Exit:
//  ax = -1 if pars out of range
int Screen::ScrollUp(int x1, int y1, int x2, int y2, int attrib)
{
	int rows, columns, dest, src, attr;
	int m = mousehidden;
	if(!m) this->HideMouse();     //hide mouse before scrolling screen

	attr = attrib;
	if(attr == -1)  attr = m_attrib;

	rows = (y2 - y1)+1;
	columns = (x2 - x1)+1;

	if(rows > 24) return -1;    //rows = r # of rows to shift up (max = 24)
	if(columns > 80) return -1;


	dest = (y1 *160) + (x1 *2);
	src = dest + 160;
	if((rows *160 + dest ) > 3998) return -1;

	asm{
		 push si
		 push di
		 push ds

		 mov  di, dest
		 mov  si, src
		 mov  dx, rows

		 mov  bx, columns
		 mov ax, 0b800h
		 mov ds, ax
		 mov es, ax
		 mov ax, di     //temp save
		 cld  }

a1:asm{ mov cx, bx       //r # of columns to copy
		  rep movsw       //copy a line from ds:si to es:di
								//increment to next line
		  add ax, 160     //inc dest
		  mov cx, ax      //temp save
		  mov di, ax

		  add ax, 160
		  mov si, ax
		  mov ax, cx       //the neyly inc'ed dest
		  dec dx
		  cmp dx,0
		  jne a1

		  mov ah, byte ptr attr   //attrib for wiping last line
		  mov al, 32
		  mov cx, bx
		  mov di, si
		  sub di, 160
		  rep stosw       //write to es:di to clear last line

		  pop ax         // = org ds
		  mov ds, ax
		  mov es, ax
		  pop di
		  pop si  }

	if(!m) this->ShowMouse();
	return 0;
}
//------------------------------------------------------------------------
//scroll down an area of the screen
//This is faster than the BIOS version
//Exit:
//  ax = -1 if pars out of range
int Screen::ScrollDown(int x1, int y1, int x2, int y2, int attrib)
{
	int rows, columns, dest, src, attr;
	int m = mousehidden;
	if(!m) this->HideMouse();  //hide mouse before scrolling down the screen
	attr = attrib;
	if(attr == -1)  attr = m_attrib;

	rows = (y2 - y1)+1;
	columns = (x2 - x1)+1;

	if(rows > 25) return -1;    //rows = r # of rows to shift up (max = 24)
	if(columns > 80) return -1;


	dest = (y2 *160) + (x1 *2);
	src = dest - 160;
	if((dest + (columns *2) ) > 3998) return -1;

	asm {
			push si
			push di
			push ds

			mov  di, dest
			mov  si, src
			mov  dx, rows
			sub  dx, 2         //row loop (so that jns will work)
			mov  bx, columns
			mov ax, 0b800h
			mov ds, ax
			mov es, ax
			mov ax, di     //temp save
			cld  }

a1:asm{ mov cx, bx       //r # of columns to copy
		  rep movsw       //copy a line from ds:si to es:di

								//increment to next line
		  sub ax, 160     //dec dest
		  mov cx, ax      //temp save
		  mov di, ax

		  sub ax, 160
		  mov si, ax
		  mov ax, cx       //the neyly dec'ed dest
		  dec dx
		  jns a1

		  mov ah, byte ptr attr   //attrib for wiping last line
		  mov al, 32
		  mov cx, bx

		  rep stosw       //write to es:di to clear last line

		  pop ax         // = org ds
		  mov ds, ax
		  mov es, ax
		  pop di
		  pop si  }

	if(!m) this->ShowMouse();   //show mouse after scrolling down the screen
	return 0;
}
//------------------------------------------------------------------------
void Screen::CursorOn()
{
  asm{
		 push bp
		 mov cx, 0607h  //colour card in text mode
		 mov ah, 1      //cursor type
		 int 10h
		 pop bp
	  }
}
//------------------------------------------------------------------------
void Screen::CursorOff()
{
  asm{
		push bp
		mov cx, 0f00h  //should turn cursor off
		mov ah, 1      //set a new cursor type
		int 10h
		pop bp
	 }
}
//------------------------------------------------------------------------
//This function copies the 4k text screen from  src to dest
//This is similar to the command in qbasic
//Entry:
//scr = screen num 0 - 7
//dest = a screen num 0 -7
//The default screen is 0
//Exit:
// ax = 0 if ok

int Screen::Pcopy(int src, int dest)
{
	int m = mousehidden;
	if(!m) this->HideMouse();  //hide mouse before making a copy of the screen
	if (src < 0 || src >7 ) return 1;
	if (dest < 0 || dest > 7) return 1;
	if (src == dest) return 1;
asm{
	  mov ax, src
	  mov bx, dest
	  push es
	  push ds
	  push di
	  push si
	  xor dx,dx
	  mov cx, 4096
	  mul cx
	  mov si,ax
	  xor dx,dx
	  mov ax,bx
	  mul cx
	  mov di, ax
	  mov ax, 0b800h
	  mov ds, ax
	  mov es,ax
	  shr cx,1           //4096 /2 = 2048
	  cld
	  rep movsw       //copy from ds:[si] to es:[di]
	  pop si
	  pop di
	  pop ds
	  pop es
	  mov ax,0 }     //job ok

	if(!m) this->ShowMouse();   //show mouse after making a copy of the screen
	return 0;
}
//------------------------------------------------------------------
//---------------------------------------------------------------
//opens a box on the screen, but first save the original screen
//see Screen::OpenShadowBox()
//Entry:
// the c pars of the box coordinates
//Exit:
// ax = 0 if job ok
int Screen::OpenBox(int x1, int y1, int x2, int y2, int attrib, int style)
{
   if(m_boxlevel ==7) return -1;  //can't open box
   this->Pcopy(0, m_boxlevel+1);
   m_boxlevel++;
   this->DrawBox(x1, y1, x2, y2, attrib, style);
   return m_boxlevel;
}
//------------------------------------------------------------------
//If a screen was saved using OpenBox() then this function
//restores that screen.
void Screen::CloseBox()
{

   if(m_boxlevel <1 || m_boxlevel > 7) return;
   this->Pcopy(m_boxlevel, 0);
   m_boxlevel--;
}
//----------------------------------------------------------------------
void Screen::ShowMouse()
{
   if(mousehidden ==1)  //only show the mouse if is it already hidden
   {
		asm{
			  mov ax, 0001h
			  int 33h
			}
		mousehidden =0;
	}

}
//------------------------------------------------------------------------
void Screen::HideMouse()
{

	if(mousehidden !=1)   //only hide the mouse if it is not already hidden
	{
		asm{
			  mov ax, 0002h
			  int 33h
			 }
		mousehidden =1;
	}


}
//-------------------------------------------------------------------------
//sets the mouse position on the text screen
//Entry:
// x1 0-79
// y1 0 -24
//Exit:
// returns nothing
void Screen::SetMousePos(int x1, int y1)
{
   int m = mousehidden;
   if(!m) this->HideMouse();         //make sure mouse is hidden before
   asm{                       //we change its position
		 mov cx, x1
		 and cx, 127
		 mov dx, y1
		 and dx, 31
		 shl cx, 3     //cx = cx *8
		 shl dx, 3     //dx = dx *8  char coordinates to pixels
		 mov ax, 0004h
		 int 33h
	  }
	if(!m) this->ShowMouse();          //show mouse at the new position
	//only if it was previously shown
}
//------------------------------------------------------------------------
//Limit the boundaries within which the mouse can move
//the coordinates are in characters. x1 0 - 79, y1 0- 24
//The coordinates to keep the mouse within the screen are
// x1 =0; y1 = 0;
// x2 = 79, y2 =24

void Screen::LimitMouse(int x1, int y1, int x2, int y2)
{
	//define the horizontal limits
	asm{
		 mov ax, 7       //set the horizontal range
		 mov cx, x1
		 and cx, 127
		 shl cx, 3        // cx = cx *8
		 mov dx, x2
		 and dx, 127
		 shl dx, 3       //dx = dx *8
		 int 33h         //sets the horizontal range

		 mov ax, 8       //set vertical range
		 mov cx, y1
			  and cx, 31
		 shl cx, 3        // cx = cx *8
		 mov dx, y2
		 and dx, 31
		 shl dx, 3       //dx = dx *8
		 int 33h         //sets the vertical range
	 }

}
//------------------------------------------------------------------------
//mouse.x, mouse.y are the coordinates of the mouse
//See labels.cpp for the definition of the structure TMouse
//see screen.h for the definition of the structure TMousePos
//This function tells which field the mouse is in
// numfields = the r # of all the field (including the unavailable ones)
//Exit:
// if mouse is in a currently available field ax = the number from
//  0 to 13 or the c # of the max field
//  if the field is not available, or if the mouse is not in a valid
//  field ax =-1

int Screen::WhichFieldIsMouseIn(TMousePos *field, TMouse *mouse,
										  int numfields)
{
	int t, focusfield, x1, y1;

	focusfield = -1;          //assume mouse in no available field

	for(t=0; t< numfields; t++){
		if(!field[t].available) continue; //field not available: skip check
		if(field[t].x1 > mouse->x) continue;  //mouse behind beginning of field
		if(field[t].x2 < mouse->x) continue;    //mouse after end of field

		if(field[t].y1 > mouse->y) continue;   //mouse above top of field
		if(field[t].y2 <mouse->y) continue;    //mouse below bottom of field
		focusfield = t; break;
	}

	return focusfield;
}
//------------------------------------------------------------------------
//mouse.x, mouse.y are the coordinates of the mouse
//See screen.h for the definition of the structure TMouse
//see screen.h for the definition of the structure TMousePos
//This function tells which field the mouse is in
//This differs from the previous function because it tells the
//field the mouse is in (if the mouse is in a valid field) even
//if the field the available flag is off.
// numfields = the r # of all the field (including the unavailable ones)
//Exit:
// if mouse is in a field ax = the number from
//  0 to 13 or the c # of the max field
//  if mouse is in no field ax =-1
//
int Screen::WhichFieldIsMouseIn2(TMousePos *field, TMouse *mouse,
											int numfields)
{
	int t, focusfield, x1, y1;

	focusfield = -1;          //assume mouse in no available field

	for(t=0; t< numfields; t++){
		if(field[t].x1 > mouse->x) continue;  //mouse behind beginning of field
		if(field[t].x2 < mouse->x) continue;  //mouse after end of field

		if(field[t].y1 > mouse->y) continue;  //mouse above top of field
		if(field[t].y2 <mouse->y) continue;   //mouse below bottom of field
		focusfield = t; break;
	}

   return focusfield;
}
//------------------------------------------------------------------------






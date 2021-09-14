/*-------------------------------------------------------------------------
	(C) Michael Finnegan    2 June 1999
-------------------------------------------------------------------------*/

#include "keyboard.h"
#include<ctype.h>

/*------------------------------------
  keyboard.h also inclues screen.h
  the base class Screen is inherited by class Keyboard

  screen.h includes:
		stdio.h
		mem.h
		string.h
		conio.h
*/
//------------------------------------------------------------------------
void Keyboard::ClrMouse()
{
	mouse.index =0;
	mouse.Recording =0;
	mouse.DownTime[0] = 0;
	mouse.DownTime[1] =0;
	mouse.UpTime[0] =0;
	mouse.UpTime[1] =0;
}
//------------------------------------------------------------------------

//Setup keyboard convert table
Keyboard::Keyboard()
{
	m_convert[0][0] = 72;
	m_convert[0][1] = up_char;

	m_convert[1][0] = 80;
	m_convert[1][1] = down_char;

	m_convert[2][0] = 75;
	m_convert[2][1] = left_char;

	m_convert[3][0] = 77;
	m_convert[3][1] = right_char;

	m_convert[4][0] = 71;
	m_convert[4][1] = bol_char;

	m_convert[5][0] = 79;
	m_convert[5][1] = eol_char;

	m_convert[6][0] = 73;
	m_convert[6][1] = pageup_char;

	m_convert[7][0] = 81;
	m_convert[7][1] = pagedown_char;

	m_convert[8][0] = 0x77;
	m_convert[8][1] = bof_char;

	m_convert[9][0] = 0x75;
	m_convert[9][1] = eof_char;

	m_convert[10][0] = 72;
	m_convert[10][1] = ins_char;

	m_convert[11][0] = 83;
	m_convert[11][1] = del_char;

	m_convert[12][0] = 115;
	m_convert[12][1] = prevword_char;

	m_convert[13][0] = 116;
	m_convert[13][1] = nextword_char;

	m_convert[14][0] = 59;
	m_convert[14][1] = f1;

	m_convert[15][0] = 60;
	m_convert[15][1] = f2;

	m_convert[16][0] = 61;
	m_convert[16][1] = f3;

	m_convert[17][0] = 62;
	m_convert[17][1] = f4;

	m_convert[18][0] = 63;
	m_convert[18][1] = f5;

	m_convert[19][0] = 64;
	m_convert[19][1] = f6;

	m_convert[20][0] = 65;
	m_convert[20][1] = f7;

	m_convert[21][0] = 66;
	m_convert[21][1] = f8;

	m_convert[22][0] = 67;
	m_convert[22][1] = f9;

	m_convert[23][0] = 68;
	m_convert[23][1] = f10;

	m_convert[24][0] = 0;
	m_convert[24][1] = 255;
}
//-----------------------------------------------------------------------
//wait a certain amount of time for a mouse or keyboard event.
//Entry:
//  t = the amount of time to wait
//ax =-1 if time out,
//if ax 259 - 266 its a mouse movement as follows
//else ax = 259  up
//          260  down
//          261  left
//          262  right
//				263  up, left
//				264  up, right
//				265  down, left
//				266  down, right
//
//if ax = 257 click
//          or chr
//
int Keyboard::WaitEvent(int t)
{
	int chr, x, y;
	long unsigned int start, stop, tp;

	start = this->GetLongTime();  //start clock
	stop = start +t;
	x = mouse.x;
	y = mouse.y;
top:
	chr = this->GetMouseOrKey();
	if(chr) return chr;
	if(mouse.DblClick) return 258;
	if(mouse.Click ) return 257;
	if(mouse.CoordChange) goto ex;
	tp = this->GetLongTime();
	if(tp < stop) goto top;

	return -1; //timeout

ex:
	if(mouse.y < y)    //if up
	{
		if(mouse.x < x) return 263; //up left
		if(mouse.x > x) return 264; //up right
		return 259;  //just up
	}

	if(mouse.y > y)  //if down
	{
		if(mouse.x < x) return 265; //down left
		if(mouse.x > x) return 266; //down right
		return 260;  //just down
	}
	if(mouse.x < x) return 261; //left
	if(mouse.x > x) return 262; //right

	goto top;
}
//--------------------------------------------------------------------
//-----------------------------------------------------------------------
//This function waits for a key from the keyboard
//It is almost the same as in the PCIO.ASM file.
//Special keys are converted in accordance with the m_convert[][] tbl

// Exit:
//  al = the key pressed
//  if al = 255 the key was illegal
int Keyboard::GetChr()
{
	static char *n_convert;
	n_convert = &m_convert[0][0];

	asm{
		push bp
		mov  ah, 0 //code: wait for a keyboard character
		int  16h
		pop  bp
		//now normal keys are in al, and ah = 0. But if al = 0 it
		//indicates ah contain a special key such as a function key or
		//a Ctrl+key combination.

		cmp  al, 0         //if al >0 its a normal key
		jne not_special

		mov bx, n_convert }
ci_loop:
  asm{ cmp byte ptr [bx], 0      //have we come to the end of the tbl?
		jz  got_it                 //not in tbl: treat as illegal

		cmp ah, byte ptr [bx]
		je  got_it               //its in table: get the converted version

		add bx, 2                //keep searching
		jmp ci_loop }
got_it:
	asm{ inc bx
		  mov al, [bx] }          //if not in tbl al = 255

not_special:
	asm {
			mov ah, 0
		  }
	//return key in ax
}
//---------------------------------------------------------------------
//This function strips away white spaces on the right of text string
//Exit:
// ax = the r length of the resultant string

int Keyboard::RightTrim( char *text)
{
	int len, t;

	len = strlen(text);
	if(!len) return 0;

	t = len-1;
	while (isspace(*(text+t)))
	{
		t--;
		if(t == -1) break;
	}

	text[t+1] =0;

	return t+1;
}
//---------------------------------------------------------------------
//delete a char from a string
//this works line a text edit delete.
// pos = the c posion of the char at the cursor to be shifted back
// overwriting the char to the left. It is the char to the left of
// pos that is deleted not the char at pos. All chars from the char at pos
// to the end of the line are shifted left
//Entry:
//  pos = the c position of the first char  to shift left
//Exit:
//  ax = len of string
//  if ax = -1 error

int Keyboard::DelChar(char *text, int pos)
{
	int len;
	char *pdest, *psrc;

	psrc = text + pos;
	pdest = psrc -1;

	len = strlen(text);
	if(!len) return -1;           //no string: error

	//if there is only 1 char in the string delete it
	if(len ==1)
	{
		*pdest = 0;
		return 0;
	}

	if( pos+1 > len) return -1;   //past last char in string: error


	do{
		 *pdest++ = *psrc++;

		} while( *(pdest-1));


	return len-1;
}
//----------------------------------------------------------------------
//insert a char into a string
//Insert a char at c position pos
//0) if pos < 0 retturn -1 error
//
//1) if the string is empty and pos = 0 enter char at text[0]
//
//2) If the position to insert the char
//   is greater  than at the start add spaces first then the char
//
//3) else the condition is pos is within the string, so shift the chars
//   from pos to the end of the string right, to make room for the char
//   then enter the char at text[pos];
//Exit:
//  ax = the new len of the string


int Keyboard::InsertChar(char *text, int pos, char chr)
{
	int len, t;
	char linebuf1[81];

	char *psrc, *pdest, tempchr;

	len = strlen(text);

	if(pos < 0 ) return -1;

	if(!len)
	{
		//Then string is empty. If the position to insert the char
		//is greater  than at the start add spaces first then the char

		if(pos)
		{
			pdest = text;
			memset(pdest, ' ', pos);
			pdest[pos] = chr;
			pdest[pos+1] = 0;
			return pos+1;
		}

		// the string is empty and the pos is at the start
		text[0] = chr;
		text[1] = 0;
		return 1;
	}

	//the string is not empty
	//If the position to insert the char is within the string, then
	//shift all the chars from the char at pos to the end of the string
	//one position right to make room for the char to be inserted.

	if((pos+1) <=  len )
	{
		t = (len - pos)+1;    //num of bytes to shift right
		psrc = &text[len];
		pdest = psrc+1;
		do{
			 *pdest-- = *psrc--;
			 t--;

		} while(t);

		text[pos] = chr;
		return len+1;
	}

	//the position to enter the char is past the string, so  see if there
	//are and spaces to be appended to the string before adding on the char

	t = (pos) - len;
	if(!t)                 //if t =0  there are no spaces to append first
	{                      //before addeding chr
		text[len] = chr;
		text[len+1] = 0;
		return len +1;
	}

	//pos is past the string by more than 1 column so append spaces
	//then add on chr
	// t = r # of spaces to append first.
	memset(linebuf1, ' ', t);
	strncat(text, linebuf1, t);
	text[pos] = chr;
	text[pos+1] =0;

	return pos+1;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//This function gets a pseudo time for timing mouse clicks only

long unsigned int Keyboard::GetLongTime()
{
	char h,m,s,hs;

	asm{
		 mov ax, 2c00h
		 int 21h             //"Read system clock"
		 mov h, ch
		 mov m, cl
		 mov s, dh
		 mov hs, dl
		}
	m_longtime = (s*100)+(m*6000)+hs;

	return ++m_longtime;        //increment from previous value and return it
}

//---------------------------------------------------------------------
//If a mouse movement, or button press/realese occurred
//or a keyboard key was pressed this function gets it
//but this function doesn't wait for the user. If nothing is available
//to get during an idle period this function returns 0
//else if it was a keyboard key press it returns it in ax,
//else if it was a mouse movement or button it is returned
//in the mouse structure var mouse.
//Entry:
//  if starting off for 1st time
//     just get the current x and y coordinates
//     so that the next time these original coordinates can be compared
//     with the new values to see did any change occure in the mouse cursor
//     position
//Exit:
// ax = 0 no key got
//      else ax = the key code (translated to cater for function keys ect)
//Alse the TMouse structure variable m_mouse is filled in as follows:
//          mouse.DblClick      =1 if a double click occurred, else =0
//          mouse.Click         =1 if the status of the button changed
//    mouse.CoordChange   =0 if no change from last pos
//          mouse.HCoordChange  =0 if no change from last column pos
//          mouse.VCoordChange  =0 if no change from last row pos
//    mouse.ButtonState    0 = up 1 = down

int Keyboard::GetMouseOrKey()
{
	static int initflg;
	long unsigned int tp;
	int x, y, lbutton, rbutton, index, charflg, chr;
top:
	mouse.DblClick =0;
	mouse.Click =0;
	mouse.CoordChange =0;      //assume no change from last pos
	mouse.HCoordChange =0;       //assume no change from last column pos
	mouse.VCoordChange =0;       //assume no change from last row pos

	lbutton = 0;        //assume left button is up
	rbutton = 0;       //assume right button is up
	tp= this->GetLongTime();

	asm{
			mov ax, 3 //get button status and location
			int 33h  //mouse service call
			sar cx, 3  //div 8
			and cx, 255
			mov x, cx
			sar dx, 3     //div 8
			and dx, 255
			mov y, dx
			test bx, 1     //left button pressed?
			jz  a1          //no.
			inc lbutton}     //left button is pressed
a1:asm{  test bx, 2       //right button pressed?
			jz  a2             //no
			inc rbutton}

a2:
	mouse.ButtonState = lbutton;
	//init values for first time
	if(initflg !=33)
	{
		mouse.y =y;
		mouse.x =x;
		mouse.index =0;
		mouse.Recording =0;
		mouse.DownTime[0] = 0;
		mouse.DownTime[1] =0;
		mouse.UpTime[0] =0;
		mouse.UpTime[1] =0;
		initflg =33;
		goto ex1;           //don't get any clicks, just exit
	}

	if(mouse.x != x)
	{
		mouse.CoordChange =1;
		mouse.HCoordChange =1;
	}
	if(mouse.y != y)
	{
		mouse.CoordChange =1;
		mouse.VCoordChange =1;
	}


	if(mouse.Recording==0)   // are we starting recording for 1st time?
	{
		if(lbutton ==0) goto ex1; //button must be down when we start recording
		mouse.Click =1;
		mouse.DownTime[mouse.index] = tp;
		mouse.Recording =1;
		goto ex1;
	}

	//wait for the button to be realeased
	if(lbutton) goto ex1;      //button is still down: exit
	mouse.Click =1;
	//button has been realeased. Record the up time
	mouse.UpTime[mouse.index] = tp;
	mouse.Recording =0;         //stop the recorder
	tp = mouse.UpTime[mouse.index] - mouse.DownTime[mouse.index];
	if(tp <= 38)
	{
		mouse.index++;
		if(mouse.index ==2) //check over-all time for a double click
		{
			tp = mouse.UpTime[1] - mouse.DownTime[0];
			if(tp> 38)   //dbltime out of range
			{
				mouse.UpTime[0] = mouse.UpTime[1];
				mouse.DownTime[0] = mouse.DownTime[1];
				mouse.index =1;
				goto ex1;
			}

			mouse.DblClick =1;
			mouse.index =0;
		}
	}
ex1:
	charflg =0;    //assume no char waiting
	chr =0;
	mouse.x =x; mouse.y =y;
	asm{
			 mov ah, 1
			 int 16h     //is key waiting
			 jz ex2      //no char waiting
			 inc charflg }
ex2:
	if(charflg) chr = this->GetChr();

	return chr;
}
//-----------------------------------------------------------------------
// This function is similiar to Keyboard::GetMouseOrKey()
//except it waits until a keyboard or mouse event occurs
//Exit:
// if a key was pressed            ax = the key presss 1- 255
// if the mouse was moved          ax = 256
// if a button was clicked         ax = 257
// if a button was doubled clicked ax = 258
int Keyboard::WaitGetMouseOrKey()
{
	int chr;
get:
   chr = this->GetMouseOrKey();      //poll the keyboard and mouse
   if(chr) return chr;
	//check the mouse vars to see if a movement, click
	//or double click occurred
	if(mouse.DblClick) return 258;
	if(mouse.Click) return 257;
	if(mouse.CoordChange) return 256;

	goto get;
}

//-----------------------------------------------------------------------
//Entry:
//   the combination of bit swtiches in entrycode determines
//   which keys can be used by the user to exit the input line.
//  if entrycode bit 0 = 1, exit upon tab key          1
//  if entrycode bit 1 = 1, exit upon up key           2
//  if entrycode bit 2 = 1, exit upon down key         4
//  if entrycode bit 3 = 1, exit upon left key         8
//  if entrycode bit 4 = 1, exit upon right key        16
//  if entrycode bit 5 = 1, exit upon page up key      32
//  if entrycode bit 6 = 1, exit upon page down key    64
//  if entrycode bit 7 = 1, exit upon f1 key           128
//  if entrycode bit 8 = 1  exit of mouse is clicked outside input field 256
// if sneek =1 dont calc offset
//Exit:
//  if ax = -1 its an error,
//  if ax = 257 mouse click occurred
//  if ax = 258 mouse Double click occurred
//  else:
//  the answer is in preanswer, but wheather this is valid or not
//  depends of how the user exited

//  ax = 0 normal <return>     //always allowed
//  ax = 1 <esc>               //allways allowed
//  ax = 2 <tab>               //if entrycode bit 0 on
//  ax = 3 <up>                //if entrycode bit 1 on
//  ax = 4 <down>              //if entrycode bit 2 on
//  ax = 5 <left>              //if entrycode bit 3 on
//  ax = 6 <right>             //if entrycode bit 4 on
//  ax = 7 <page up>           //if entrycode bit 5 on
//  ax = 8 <page down>         //if entrycode bit 6 on
//  ax = 9 <f1>                //if entrycode bit 7 on

int Keyboard::Input(TMousePos *MousePos, int numfields,
						  char *preanswer, int x1, int y1,
						  int fieldsize, int maxsize, int *cursorpos,
						  int attrib, int entrycode, int dots, int sneek)
{
	static int chr, t, j, k, ii;
	static int col;
	static int col_offset;
	static int scrollflg;
	static int len;
	static int exitcode;

	if(!sneek) scrollflg =0;

	if(sneek)
	{

		col = sneek-1;
		goto getkey;
	}

	len = strlen(preanswer);
	if(*cursorpos > len) return -1;
	//allow scrolling backwards and forwards if the field size is
	//smaller than the maximum size for the input string
	if(fieldsize< maxsize) scrollflg =1;

	if(fieldsize > maxsize) return -1;   //error: fieldsize cant be greater
																											//then maxsize
	if(((*cursorpos)+1) > maxsize) return -1;  //Error: past maxsize

	if(((*cursorpos)+1) > fieldsize)
	{
		col_offset = 1+(*cursorpos - fieldsize);
		col = fieldsize-1;
	}
	else
	{
		col = *cursorpos;
		col_offset = 0;
	}

getkey2:
	this->WriteLine(preanswer+col_offset, x1, y1, fieldsize, attrib, dots);

getkey1:
	gotoxy(x1+1+col, y1+1);

getkey:
	chr = this->WaitGetMouseOrKey();

gotkey:
	switch( chr)
	{

		case 13:                //always allowed
			exitcode = 0;
			this->RightTrim(preanswer);
rex:  	this->CursorOff();
			*cursorpos = col+ col_offset;    //tell caller the c pos of the
														//cursor upon exit
			return exitcode;


		case 27:             //escape: abort input.  allways allowed
			exitcode = 1;
			goto rex;



		case 9:           //tab
			if(!(entrycode & 1))  goto getkey; //dont exit
			else
			{
				exitcode = 2;
				goto rex;     //<tab>  exit
			}



			case 28:          //right char
				if(!(entrycode & 16)) goto c1;    //no exit: process right key
				else
				{
					exitcode = 6;
					goto rex;
				}


//process right key as normal
c1:         if(!scrollflg)
				{
					if((col+1) <=fieldsize)
					{
						if((col+1) <= len) { col++; goto getkey1;}
					}
					else
					{
						goto getkey;
					}
				}
				//the scrollflg is on, which means the viewing field is smaller
				//than the maximun input string size.
				//allow scrolling if the cursor is at the right end, and if
				//the cursor hasnt come to the end of the string

				if((col+1) <fieldsize)
				{
					if((col+1) <= len) col++;
					goto getkey1;
				}

				//cursor not at end of string
				if((col_offset +col+1) <=len)
				{
					col_offset++;
					goto getkey2;
				}
				else goto getkey;



			case 29:        //cursor left
				if(!(entrycode & 8)) goto c2;   //no exit, process left key
				else
				{
					exitcode = 5;
					goto rex;
				}

//process left key as normal
c2:         if(!scrollflg)
				{
					if(col)
					{
						col--;
						goto getkey1;
					}
					else
					{
						goto getkey;   //ignore because cursor is at beginning
					}
				}
				//the scrollflg is on, which means the viewing field is smaller
				//than the maximun input string size.
				//allow scrolling if the cursor is at the left end, and if
				//the cursor hasnt come to the begining of the string
				if(col_offset +col)
				{
					if(col) {col--; goto getkey1;}
					//We now know the cursor is at the end of the field.
					//but and also we know that col_offset > 0
					col_offset--;
					goto getkey2;
				}
				else goto getkey;



			case 30:           //<up>
				if(!(entrycode & 2)) goto getkey; //no exit using up key
				else
				{
					exitcode = 3;
					goto rex;

				}

			case 31:           //<down>
				if(!(entrycode & 4)) goto getkey;  //no exit using down key
				else
				{
					exitcode = 4;
					goto rex;

				}

			case 210:               //f1
				if(!(entrycode & 128)) goto getkey;    //no exit using f1
				else
				{
					exitcode = 9;
					goto rex;
				}


			case 202:           //page up
				if(!(entrycode & 32)) goto getkey;  //no exit using <page up>
				else
				{
					exitcode = 7;
					goto rex;

				}

			case 203:       //page down
				if(!(entrycode & 64)) goto getkey;   //no exit using <page down>
				else
				{
					exitcode = 8;
					goto rex;
				}


			case 8:             //delete char from string is string > 2
									  //and cursor left if possible
				t = col_offset+ col;
				if(!len) goto getkey;
				if(!t) goto getkey;  //cant delete 1st char

				if(t == len)  //if the cursor is past the last char delete the
				{             //last char
					preanswer[t-1] =0;
					goto h2;
				}


				this->DelChar(preanswer, col_offset+col);
h2:      	this->WriteLine(preanswer+col_offset, x1, y1, fieldsize,
																																								  attrib, dots);
				len = strlen(preanswer);
				chr = 29;      //force a cursor left
				goto gotkey;

			case 256:       //mouse movement (ignore it)
				goto getkey;

			case 257:       //mouse click
				if((entrycode & 256) != 256) goto getkey; //ignore mouse click
				j = mouse.x - x1;
				k = mouse.y - y1;
				if(k) //click occurred outside the input line
				{     //see if it occurred in a valid field, if not
						//ignore it
					ii = this->WhichFieldIsMouseIn2(MousePos, &mouse, numfields);
					if(ii == -1) goto getkey;
					clickfield = ii;
					exitcode = 257;
					goto rex;      //click outside input field
				}

				//if(k) {exitcode = 257; goto rex;} //clicked outside input
															 //field:exit

				//clicked on same line as input field, if in range set the cursor
				//to the mouse position
q1:         k = strlen(preanswer);
				if( k >=fieldsize) k = fieldsize;
				if((j>=0) && (j < k))
				{
					col = j;    //reposition the cursor: its withing txt range
					goto getkey1;
				}
				//mouse on line but past end of txt:
				//pos cursor at end of txt or end of field
				if(k == fieldsize) k--;
				col =k;
				goto getkey1;

//--------
//a mouse double click occurred.
//see if it was on the input line. if so send <return> to caller
			case 258:       //mouse double click
				if((entrycode & 256) != 256) goto getkey; //ignore mouse click
				j = mouse.x - x1;
				k = mouse.y - y1;
				if(k) {exitcode = 258; goto rex;} //clicked outside input
															 //field:exit
				this->PushReturn(); //fool input into thinking user pressed
															//<return>
				goto q1;     							//reposition cursor

//----------
			default:
				if(chr >= 32 && chr <= 127)
				{
					t = col_offset+ col;
					if(len == maxsize)
					{
						if(t == len) goto getkey; //string full cant add on to end

						//truncate last char  cause line full
						preanswer[len-1] =0;
						//insert a char, or add on the new chr at the end
						this->InsertChar(preanswer, t, chr);
						this->WriteLine(preanswer+col_offset, x1, y1, fieldsize,
											 attrib, dots);
						len = strlen(preanswer);
						chr = 28;        //force cursor right
						goto gotkey;
					}

					else    //line not full.
					{
						//insert a char, or add on the new chr at the end
						this->InsertChar(preanswer, t, chr);
						this->WriteLine(preanswer+col_offset, x1, y1, fieldsize,
											 attrib, dots);
						len = strlen(preanswer);
						chr = 28;        //force cursor right
						goto gotkey;
					}

				}

	} //end of switch
	goto getkey;
}
//---------------------------------------------------------------------
//wait for the left mouse button to be released
//Entry:
// if flg =-1 wait until mouse button is released
// else wait intil time up
//Exit:
// ax =0 if button released
// ax = -1 timeout
int Keyboard::WaitMouseButtonRelease(int timeflg)
{
	long unsigned int tp;

get:
	tp = GetLongTime();
	this->GetMouseOrKey();
	if(mouse.ButtonState)
	{
		if( timeflg == -1) goto get;   //left button still pressed down
		if(tp<  (mouse.DownTime[0]+timeflg)) goto get;
		if(tp <(mouse.DownTime[1] +timeflg)) goto get;
		mouse.UpTime[0] =tp-1;
		mouse.UpTime[1] = tp-1;
		mouse.DownTime[0] = tp-1;    //replace org down time
		mouse.DownTime[1] =tp-1;

		 return -1;        //timeout, button still down
	}

  mouse.DownTime[0] = tp-1;    //replace org down time
  mouse.DownTime[1] =tp-1;

  return 0;    //button released
}
//----------------------------------------------------------------------
//Push a tab char into the keyboard buffer
void Keyboard::PushTab()
{
	asm{
			mov ah, 05h      //"push key into buffer"
			mov ch, 15      // scan code
			mov cl, 9        //ascii for tab
			int 16h
		}
}
//------------------------------------------------------------------------
//Push <return> char into the keyboard buffer
void Keyboard::PushReturn()
{
   asm{
       mov ah, 05h      //"push key into buffer"
       mov ch, 28      // scan code
       mov cl, 13        //ascii for <return>
		 int 16h
		}
}
//---------------------------------------------------------------------
//Push <down> char into the keyboard buffer
void Keyboard::PushDown()
{
	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov ch, 80      // scan code
		 mov cl, 0        //ascii for down
		 int 16h
		}
}
//---------------------------------------------------------------------
//Push <up> char into the keyboard buffer
void Keyboard::PushUp()
{
	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov ch, 72      // scan code
		 mov cl, 0        //ascii for up
		 int 16h
		}
}
//---------------------------------------------------------------------
//Push a digit 0 = 9  into the keyboard buffer
//Entry:
//  the digit is 0 -9
void Keyboard::PushDigit(int digit)
{
	int scancode, asciicode;
	static int scanbuf[10] = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	asciicode = digit+48;
	scancode = scanbuf[digit];

	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov ch, byte ptr scancode      // scan code

		 mov cl, byte ptr asciicode       //ascii for up
		 int 16h
		}
}
//----------------------------------------------------------------------
//Push <space> char into the keyboard buffer
void Keyboard::PushSpace()
{
	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov ch, 57      // scan code
		 mov cl, 32       //ascii for up
		 int 16h
		}
}
//------------------------------------------------------------------------
//Push <esc> char into the keyboard buffer
void Keyboard::PushEsc()
{
	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov ch, 1      // scan code
		 mov cl, 27       //ascii for up
		 int 16h
		}
}
//------------------------------------------------------------------------
//Push <help key> char into the keyboard buffer
void Keyboard::PushF1()
{
	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov ch, 59      // scan code
		 mov cl, 0       //ascii for up
		 int 16h
		}
}
//------------------------------------------------------------------------
//Push <F2> char into the keyboard buffer
void Keyboard::PushF2()     //simulates a click pushed into the buffer
{
	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov ch, 60      // scan code
		 mov cl, 0       //ascii for up
		 int 16h
		}
}
//------------------------------------------------------------------------
//Push <F2> char into the keyboard buffer
void Keyboard::PushF3()
{
	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov ch, 61      // scan code
		 mov cl, 0       //ascii for up
		 int 16h
		}
}
//-----------------------------------------------------------------------
//Push <del>  <-- char into the keyboard buffer
void Keyboard::PushDel()
{
	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov ch, 14      // scan code
		 mov cl, 8       //ascii for up
		 int 16h
		}
}
//------------------------------------------------------------------------
//Push <menu key> char into the keyboard buffer
void Keyboard::PushF10()
{
	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov ch, 68      // scan code
		 mov cl, 0       //ascii for up
		 int 16h
		}
}
//------------------------------------------------------------------------
//Push a-z> char into the keyboard buffer
void Keyboard::PushA_Z(int chr)
{
	asm{
		 mov ah, 05h      //"push key into buffer"
		 mov cx, chr      // scan code

		 int 16h
		}
}
//------------------------------------------------------------------------

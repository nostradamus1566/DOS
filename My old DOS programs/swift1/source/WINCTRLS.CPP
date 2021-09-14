/*-------------------------------------------------------------------------
	(C) Michael Finnegan      9 June 1999
-------------------------------------------------------------------------*/

#include "winctrls.h"

#include "disk2.h"

//----------------------------------------------------------------------

//------------------------------------------------------------------------
//this function is needed by WinCtrls::ShowDialogBox() and DoListBox to
//get return the address of an item. It is designed this way
//so that an other function could replace this function if
//we wanted to get file names instead from far data. The functionality in the
//listbox functions would remain the same, only the address of the function
//being called to fetch the address of a line of text would differ.
char *WinCtrls::GetText(char *txtlines[], int item)
{

   return txtlines[item];
}
//------------------------------------------------------------------------
//Called by WinCtrls::DoListBox()
//each time an item is fetched for displaying in a listbox.
//read a file or dir entry from far data
//Entry:
// item = the c # of the file entry to fetch from fardata
// txtlines[] is a dummy for comformity with the listbox in class WinCtrls
//call Disk::GetDirectory() to read in the files and dirs before
//calling this function
//Exit:
// if the item was out of range a null buffer will be returned
//else the addr of the file entry will be returnd. It will
//point to filebuf

char *WinCtrls::GetFileEntry(char *txtlines[], int item)
{
int dirs, files, stpos;

	static Disk b;

	filebuf[0] =0;
	inputbuf[0] =0;

	dirs = _getnumdirs();
	files = _getnumfiles();
	if(files < 1) goto a1;
	_fetch_stored_entry(tempbuf, item +dirs);

	strcpy(&filebuf[1], &tempbuf[9]);
	filebuf[0] =32;
	strcpy(inputbuf, &tempbuf[9]);
a1:

	stpos =0;

	b.WriteLine(&inputbuf[stpos], 29, 3, 32, LIGHTGRAY*16+BLACK, 1);
	return &filebuf[0];
}
//-----------------------------------------------------------------------
//This function is needed in File::OpenBox to get a dir or dev item
//it is called by the DoListBox  function in class WinCtrls
//each time an item is need when displaying a list
//Entry:
//  item = the c # of the item to fetch (from fardata if its a dir)
char *WinCtrls::GetDirOrDevEntry(char *txtlines[], int item)
{
	int dirs, t, len, stpos;
	static Disk c;

	filebuf[0] =0;
	inputbuf[0] =0;

	dirs = _getnumdirs();
	//files = _getnumfiles();

	if(item < dirs)   //caller wants a dir, not a dev
	{
		_fetch_stored_entry(tempbuf, item);
		strcpy(&filebuf[1], &tempbuf[9]);
		filebuf[0] =32;
		strcpy(inputbuf, &tempbuf[9]);
		strcat(inputbuf, "\\");
		if(!m_rw)        //append mask to inputbuf is reading
			strcat(inputbuf, filemask);          //filemask is global
		goto a1;
	}
	//return "[-A-] to the list box
	//and store and display on the input line something like
	//"D:*.*"
	else
	{
		strcpy(filebuf, " [-A-]");
		t = item - dirs;
		filebuf[3] = (c.m_devslist[t] &95); //convert drive letter to upper case
		inputbuf[0] = filebuf[3];       //drive letter
		inputbuf[1] = ':';
		inputbuf[2] =0;
		strcat(inputbuf, filemask);   //to get "A:*.*
	}



a1:
	len = strlen(inputbuf);
	stpos =0;
	if(len> 31) stpos = len -31; //the c pos of the first char to disp
	//displen = len - stpos;        //the r len of the text to disp

	c.WriteLine(&inputbuf[stpos], 29, 3, 32, LIGHTGRAY*16+BLACK, 1);
	return &filebuf[0];
}
//------------------------------------------------------------------------
//-----------------------------------------------------------------------
//This function shows a vertical scrollbar
//Entry:
//  x1, y1       = pos of scrollbar
//                 This is the c position of the top arrow of the scrollbar

//   len         = length of scrollbar
//                 This is the r # of moveable  positions on the
//                 scrollbar.


//   cursorlen   = length of cursor on bar (1 or more character lines)

//   cursorpos   = position of cursor on bar. 0 = top
//                 bottom position = len - cursorlen
//                 so the cursor on the  scrollbar has a range
//                 from 0 to (len - cursorlen)

void WinCtrls::ShowVerticalScrollBar(int x1, int y1, int len,
                                 int cursorlen, int cursorpos,
                                 char attrib1, char attrib2, char attrib3)
{
	int mylen, t, cpos, attrib;
	char linebuf1[5];
	char scrollchars[2][25];

	mylen = len+2;      //r len of scrollbar + 2 arrows
	if(!cursorlen)     //if no scroll bar draw box line
	{
		for(t = 0; t< mylen; t++){
			scrollchars[0][t] = 179;
         scrollchars[1][t] = attrib1;
		}
		goto a1;    //draw it
	}

	scrollchars[0][0] = 24;
   scrollchars[1][0] = attrib2;
	scrollchars[0][mylen-1] = 25;
   scrollchars[1][mylen-1] = attrib2;

	//first set scrollchars[][] to scrollbar, without scrollbar cursor
	for(t =0; t< len; t++){
		scrollchars[0][t+1] = 176;
      scrollchars[1][t+1] = attrib2;
	}

	//do scrollbar cursor
	for(t=0; t< cursorlen; t++){
		scrollchars[0][cursorpos+t+1] = 32;
      scrollchars[1][cursorpos+t+1] = attrib3;
	}

	//draw scroll bar on screen
a1:  for(t=0; t< mylen; t++){
		 attrib = scrollchars[1][t];
		 this->WriteLine(&scrollchars[0][t], x1, y1+t, 1, attrib);
	 }

}

//----------------------------------------------------------------------
//This function shows the buttons, their text and if on sets
//the button cursor to that button

void WinCtrls::ShowButtons()
{
	int t, len;
	char linebuf1[20];

	for(t = 0; t< m_buttonsnum; t++){
		this->WriteLine(&m_buttonstxt[t][0], m_buttonsposx1[t], m_buttonsposy1[t],
		-1, WHITE*16+BLACK);
	}

	//Show the black bit under the button
	for(t = 0; t< m_buttonsnum; t++){
		len = strlen(&m_buttonstxt[t][0]);
		memset(linebuf1, 220, len);
		linebuf1[len] = 0;//222;
		linebuf1[len+1] =0;

		this->WriteLine(linebuf1, m_buttonsposx1[t]+1, m_buttonsposy1[t]+1,
														-1, BLACK*16+LIGHTGRAY);

		linebuf1[0] = 220; linebuf1[1] =0;
		this->WriteLine(linebuf1, m_buttonsposx1[t]+len, m_buttonsposy1[t],
														-1, LIGHTGRAY*16 +BLACK);

	}
	// if a button is pressed clear it shadow
	if(m_buttonpressed)
	{
		t = m_buttonpressed-1;
		len = strlen(&m_buttonstxt[t][0]);
		memset(linebuf1, 32, len);
		linebuf1[len] = 0;

		this->WriteLine(linebuf1, m_buttonsposx1[t]+1, m_buttonsposy1[t]+1,
														-1, LIGHTGRAY*16+BLACK);

		linebuf1[1] =0;
		this->WriteLine(linebuf1, m_buttonsposx1[t]+len, m_buttonsposy1[t],
														-1, LIGHTGRAY*16 +BLACK);

		//show the button shifted 1 position left

		len =strlen(&m_buttonstxt[t][0]);
		this->WriteLine(&m_buttonstxt[t][0], m_buttonsposx1[t]+1,
											 m_buttonsposy1[t], len, WHITE*16+BLACK);

		//clear whit bit on left of button
		this->WriteLine(" ", m_buttonsposx1[t],
											 m_buttonsposy1[t], 1, LIGHTGRAY*16+BLACK);
		goto ex;
	}


	t = m_buttonscursorpos;
	//show the menu button cursor
	strcpy(linebuf1, &m_buttonstxt[t][0]);
	linebuf1[0] = 16;
	len = strlen(linebuf1);
	linebuf1[len-1] = 17;

	this->WriteLine(linebuf1, m_buttonsposx1[t], m_buttonsposy1[t],
			 -1, WHITE*16+BLACK);

ex:
}
//------------------------------------------------------------------------
//----------------------------------------------------------------------
//This function counts the number of buttons based on the ORing of
//buttons
int WinCtrls::CountButtons(int buttonsbits)
{
	int powers[8] = {1,2,4,8,16,32,64,128};
	int numbuttons, t;
	numbuttons =0;

	for (t = 0; t <8; t++){
		if(buttonsbits & powers[t])
		numbuttons++;
  }

	return numbuttons;
}
//----------------------------------------------------------------------
//This function returns the length of the longest line from
//a list of varying line lengths.
//It is used to calculate the dimentions when creating a message box
//Entry:
// m_numitems = number of items in list
// gettext = adr of the function to call to get the adr of a line of text
// the function takes the addr of an array and the c # of the item to fetch
// txtline2[] is the array.
int WinCtrls::FindLongestLen(char *(*gettext)(char *txtlines[],int item),
																		char *txtlines2[])
{
	int maxlen, t, len;
	maxlen =0;
	for(t=0; t<m_numitems; t++){
		len = strlen(gettext(&txtlines2[0], t));
		if(maxlen< len) maxlen = len;
	}
	return maxlen;
}
//----------------------------------------------------------------------
//This function sets the text on the buttons based on the bits set/ clear
// in buttonsbits
//
//  Exit:
//  m_buttonstextlen = len of text of all valid buttons
void WinCtrls::SetTextOnButtons(int numbuttons, int buttonsbits)
{
	int t, powers[8] = {1,2,4,8,16,32,64,128};
	int i;
	char *buttonstxt[] = {"   Ok   ",
								 "  Cancel  ",
								 "  Help  ",
								 "  Yes  ",
								 "  No                                 "};

	if(m_repeatflg == 1)
	{
		 buttonstxt[0] = "  Replace  ";
		 buttonstxt[1] = "  Replace All  ";
		 buttonstxt[2] =	"  Cancel  ";
		 buttonstxt[3] = "  Help  ";
	}
	else if(m_repeatflg ==2)
	{
		buttonstxt[0] = "  Yes  ";
		buttonstxt[1] = "  No  ";
		buttonstxt[2] =	"  Cancel  ";
	}


	else
	{
	  buttonstxt[0] = "   Ok   ";
	  buttonstxt[1] = "  Cancel  ";
	  buttonstxt[2] ="  Help  ";
	  buttonstxt[3]=  "  Yes  ";
	  buttonstxt[4] = "  No  ";

	}


	m_buttonstextlen =0;
	i =0;
	for(t =0; t< 8; t++){
		if((buttonsbits & powers[t]))
		{
			strcpy(&m_buttonstxt[i][0], &buttonstxt[t][0]);
			m_buttonstextlen += strlen(&buttonstxt[t][0]);
			i++;
		}
	}
}
//----------------------------------------------------------------------
//Entry:
// len = the r length of the scroll bar (ie., 10 if maxview = 12)
// line = the c line number  of the text line (ie., 0 - 42)
// Exit:
//  ax = the c top pos of the scrollbar cursor
int WinCtrls::GetScrollbarCursorPos(int len, int line, int cursorsize)
{
	int l, t, r, barstop;

	if(!line) return 0;
	r =-1;
	barstop = len - cursorsize;
	for(t =0; t<barstop; t++){
		if(line < m_ranges[t+1]) {r = t; break;}
	}
	if(r == -1) r = barstop;

	return r;
}
//----------------------------------------------------------------------
//Called by WinCtrls::DoListBox()
//Display a list of text lines and show the one at the
//cursor as either selected, or unselected
//Entry:
// gettext = adr of function to call to get a pointer to a line of text
// m_topitem    = the c # of the first item to get
// m_x1, m_y1  = the coordinates of the first line

// m_relativecurrentline = 0 to height-1
//            this is the line the cursor will be displayed on
//            relative to the coordinates of the first line.
//  m_cursorstyle = 0 small off, black off  (no cursor)
//                1 small on, black off
//                2 small off, black on
//                3 small on, black on

int WinCtrls::DispListBoxLines(char *(*gettext)(char*txtlines[],int item),
																			  char *txtlines2[])
{
	int x, t, h, width, m = mousehidden;
	char *txt;

	if(!m) this->HideMouse();

	m_relativecurrentline = m_currentitem - m_topitem;
	width = (m_x2 - m_x1)-1;
	h = (m_y2 - m_y1)-1;
	t = m_numitems - m_topitem;
	if(t< h) h =t;

	for(t =0; t< h; t++){
		if(t == m_relativecurrentline) continue;  //don't print item at cursor
																//just yet
		txt = gettext(&txtlines2[0], t+m_topitem);
		this->WriteLine(txt, m_x1+1, m_y1+1+t, width, LIGHTGRAY*16+BLACK);
	}

  //if the black select is on show item at cursor in black select colours
  if(m_cursorstyle == 2 || m_cursorstyle ==3)
  {
	  txt = gettext(&txtlines2[0], m_topitem + m_relativecurrentline);
	  this->WriteLine(txt, m_x1+1, m_y1+1+ m_relativecurrentline, width,
																																		  BLACK*16+LIGHTGRAY);
  }
  else //black select is off so print item at cursor as in normal colours
  {
	  txt = gettext(&txtlines2[0], m_topitem + m_relativecurrentline);
	  this->WriteLine(txt, m_x1+1, m_y1+1+ m_relativecurrentline, width,
																																		  LIGHTGRAY*16+BLACK);
  }



  //if the small cursor is on show it
  if(m_cursorstyle ==1 || m_cursorstyle ==3)
  {
	  x =0;
	  if(txt[0] == 32) x =1;   //cursor pos for files or dirs list
	  gotoxy(m_x1+2+x, m_y1 + m_relativecurrentline+2);
	  this->CursorOn();
  }

  if(m_scrollflg)
  {
	  //show the scrollbar and scrollbar cursor pos
	  m_cursorpos = this->GetScrollbarCursorPos(m_maxscrollsize,
															  m_currentitem,m_cursorsize);

		 this->ShowVerticalScrollBar(m_x2, m_y1+1, m_maxscrollsize,
														  m_cursorsize, m_cursorpos);
	}

ex:
	if(!m) this->ShowMouse();
	return 0;
}
//----------------------------------------------------------------------
//move scrollbar cursor down by 1 char
int WinCtrls::MoveScrollbarCursorDown(char *(*gettext)(char *txtlines[],
																					 int item),
																		  char *txtlines2[])
{
	int t, barstop, offset, f_cursorpos, oldcursorpos;

	oldcursorpos = m_cursorpos;
	barstop = m_maxscrollsize - m_cursorsize;
	//cluster = (float) m_numitems / (barstop+1);

	if(m_scrollflg)
	{
		if(m_cursorpos == barstop)  //can't go down
		{
a1:      m_currentitem = m_numitems-1;
			m_topitem = m_currentitem - (m_maxviewlines-1);
			if(m_topitem <0) m_topitem =0;
			m_cursorpos = barstop;
			goto ex;
		}


		if((m_currentitem+1) == m_numitems) goto a1;
		offset = m_currentitem- m_topitem;

		//--------------
		//get start off value for m_currentitem
		f_cursorpos = this->GetScrollbarCursorPos(m_maxscrollsize,
																		m_currentitem,
																		m_cursorsize);
		m_currentitem = m_ranges[f_cursorpos+1];
		goto a33;
		//--------------

a3:   m_currentitem++;
a33:  f_cursorpos = this->GetScrollbarCursorPos(m_maxscrollsize,
																m_currentitem,
																m_cursorsize);
		if(f_cursorpos == oldcursorpos)
		{

			if((m_currentitem+1) >= m_numitems) goto a1;
			goto a3;
		}
		if(f_cursorpos == barstop) goto a1;

		m_topitem = m_currentitem- offset;
		//are there blank lins at bottom?

a2:     if((m_topitem +(m_maxviewlines-1)) > (m_numitems-1))
		  {
			  m_topitem--;
			  goto a2;
		  }

ex:
		  this->DispListBoxLines(gettext, &txtlines2[0]);
	}

	return 0;
}
//----------------------------------------------------------------------
//move scrollbar cursor up by 1 char
int WinCtrls::MoveScrollbarCursorUp(char *(*gettext)(char*txtlines[],int item),
																			char *txtlines2[])
{

	int offset, barstop, oldcursorpos, f_cursorpos, f_offset;

	offset = m_currentitem - m_topitem;
	oldcursorpos =this->GetScrollbarCursorPos(m_maxscrollsize, m_currentitem,
																					m_cursorsize);

	if(!m_currentitem) return 0;    //cant go up
	if(!oldcursorpos) return 0;

	if(m_scrollflg)
	{
a1:   m_currentitem--;
		if(!m_currentitem ) goto ex;
		f_cursorpos = this->GetScrollbarCursorPos(m_maxscrollsize,
																m_currentitem,
																m_cursorsize);

		if(!f_cursorpos)   //if scrollbar cursor at top
		{
			m_topitem =0;
			m_currentitem = offset+m_topitem;
			goto ex;
		}

		if(f_cursorpos == oldcursorpos) goto a1;

ex:
		m_topitem = m_currentitem - offset;
		if(m_topitem <0) m_topitem =0;

		f_cursorpos = this->GetScrollbarCursorPos(m_maxscrollsize,
																m_currentitem,
																m_cursorsize);
		if((oldcursorpos == f_cursorpos) && oldcursorpos ==1)
		{
			m_currentitem =0;
			m_topitem=0;
			offset =0;
		}


		m_topitem = m_currentitem - offset;
		if(m_topitem <0) m_topitem =0;
		this->DispListBoxLines(gettext, &txtlines2[0]);
	}

	return 0;
}
//----------------------------------------------------------------------
//this function is the main listbox function
//it is called by ShowDialogBox
//See ShowDialogBox as an example of how to call this function
//Emtry:
// the following member vars should be setup before calling this function
// m_x1, m_y1, m_x2, m_y2, m_numitems, m_topitem,
// m_currentitem, m_cursorstyle
//This function was called with:
// this->DolistBox(fncadr, &LineAddressesArray[0]);
// fncadr can be the address of any nonclass function as long as the
// function conforms to the following prototype:
//    char *fnc(char *array[], int item);
// the function fnc receives 2 entry pars:
//  par 1 is the array of address of each line in a list
//  par 2 is the c # of the item to fetch
// The function fnc returns the address of the item fetched
// This might seem like a convoluted way of getting the address of a line
// of text from an array, particularly as &txtline2[item] would seem to
// be simpler, but it is designed this way do that the function
// being called is variable. For example a function to get items from
// a file list would be more complicated than just &txtlines2[0]
// It would ignore the address of the array being sent to it, and instead
// get the address of a file item by calling another function to fetch the
// text line from far memory, copy it into a buffer, and return to adr of
// the buffer.
// Making the DoListBox and its supporting functions complex like this
// adds to the versatility of these functions.
// entrycode for  masking in or out exit events
// 0  =   1  exit upon tabkey
// 1  =   2  exit upon upkey
// 2  =   4  exit upon down key
// 3  =          8  exit upon left key
// 4  =         16  exit upon right key
// 5  =         32  exit upon page up
// 6  =         64  exit upon page down
// 7  = 128  exit upon f1
// 8  =        258  exit upon <return>
// 9 =  512  exit upon <esc>
// 10  1024  exit upon click
// 11  2048  exit upon double click

int WinCtrls::DoListBox(char*(*gettext)(char *txtlines[], int item),
																	char *txtlines2[],
																	int entrycode,
																	int boxstyle, int jmpflg)

{
	char *txt, ch2;
	int barstop, chr, t, j, i, width, botitem, nextpagebotitem;
	float cluster, f_numlines, f_cursorsize;
	int init =0, found, row;

	//ititialise the mouse fields for the listbox
	for(t =0; t<4; t++){
		m_mousefields[t].available =0;
	}

	m_maxviewlines = (m_y2- m_y1)-1;
	m_scrollflg =0;             //assume no scrolling allowed
	m_maxscrollsize = m_maxviewlines-2;
	if(m_numitems > m_maxviewlines) m_scrollflg =1;
	//setup the mouse fields
	m_mousefields[0].x1 = m_x1+1;  //the inside area of the list box
	m_mousefields[0].y1 = m_y1+1;
	m_mousefields[0].x2 = m_x2-1;
	m_mousefields[0].y2 = m_y2-1;
	m_mousefields[0].available =1;
  //the up arrow
  m_mousefields[1].x1 = m_x2;
  m_mousefields[1].y1 = m_y1+1;
  m_mousefields[1].x2 = m_x2;
  m_mousefields[1].y2 = m_y1+1;
  m_mousefields[1].available = m_scrollflg;
 //the down arrow
  m_mousefields[2].x1 = m_x2;
  m_mousefields[2].y1 = m_y2-1;
  m_mousefields[2].x2 = m_x2;
  m_mousefields[2].y2 = m_y2-1;
  m_mousefields[2].available = m_scrollflg;
 //the scrollbar field range
  m_mousefields[3].x1 = m_x2;
  m_mousefields[3].y1 = m_y1+2;
  m_mousefields[3].x2 = m_x2;
  m_mousefields[3].y2 = m_y2-2;
  m_mousefields[3].available = m_scrollflg;

  //calculate the size of the cursor in the vertical scrollbar
  f_numlines = (float) m_numitems;
  if(f_numlines <1) f_numlines =1; //prevent diision by zero error
  f_cursorsize = (float) ((m_maxviewlines /f_numlines) *m_maxscrollsize+.4);
  m_cursorsize = f_cursorsize;

g:if(m_cursorsize >= m_maxscrollsize) {m_cursorsize--; goto g;}
  if(m_cursorsize <1) m_cursorsize =1;


  //setup the scroll bar ranges for each char pos on scrollbar
  barstop = m_maxscrollsize - m_cursorsize;  //the last c pos on the bar
  cluster = (float) m_numitems / (barstop+1);
  for(t=0; t<= barstop; t++){
	  m_ranges[t] = (float) t * cluster;
  }
  //m_ranges[barstop] = m_numitems - m_maxviewlines;
  if(m_maxviewlines > m_numitems) m_ranges[barstop] =0;
  //each element of m_ranges[] contains the top c value for that pos


  //draw the box, but don't reserve any previous screen
  this->DrawBox(m_x1, m_y1, m_x2, m_y2, LIGHTGRAY*16+BLACK, boxstyle);

disp:

	DispListBoxLines(gettext, &txtlines2[0]);
	//if jmpflg >0 caller want to go immediately to one of these:
	if(init ==0 && jmpflg ==1) {init =1; goto ua;} //up arrow
	if(init ==0 && jmpflg ==2) {init =1; goto da;} //down arrow
	if(init ==0 && jmpflg ==3) {init =1; goto sb;} //scroll bar arrow

get:

	t = 0;
	found =0;    //not found yet
	chr = this->WaitGetMouseOrKey();

	if(((chr >= 65) && (chr <= 90)) || ((chr >= 97) && (chr <=122)))
	{
		chr = (chr & ((255-32))); //convert to uppercase
		for(t = 0; t< m_numitems; t++){
			txt = gettext(&txtlines2[0], t);
			ch2 = txt[0];
			if(ch2 == 32) ch2 = txt[1];
			if(((ch2 >= 65) && (ch2 <= 90)) || ((ch2 >= 97) && (ch2 <=122)))
				ch2 = (ch2 &( 255-32)); //convert to uppercase

			if(ch2 == chr)
			{
				found = t+1;
				break;
			}
		}
		if(!found)
		{
			txt = gettext(&txtlines2[0], m_currentitem);
			goto get;
		}

		int maxviewlines = (m_y2 - m_y1)-1; //r # of max view lines
		if(m_numitems < maxviewlines ) maxviewlines = m_numitems;
		//is it currently being displayed? If so move cursor down to it, if not
		//display it at top of list
		if((found >= (m_topitem+1)) && (found <= (m_topitem+ maxviewlines)))
		{
			m_currentitem = found -1;
		}
		else
		{
			m_topitem = found-1;
			m_currentitem = found-1;
			if((m_topitem + maxviewlines) > m_numitems) m_topitem = m_numitems - maxviewlines;
		}
		goto disp;
	}

	//its not a-z or A- Z it might be a cursor key a mouse click
	switch( chr)
	{
		  case 29: //left key
					  if((entrycode & 8) == 8) return 29;
				 else goto get;

			 case 28:  //right key
					  if((entrycode & 16) ==16) return 28;
					  else goto get;

			 case 9:  //tab
					  if((entrycode & 1) == 1) return 9;
					  else goto get;

			 case 27:  //<esc>
					  if((entrycode & 512) ==512) return chr;
					  else goto get;

			 case 13:  //<return>
					  if((entrycode & 256) == 256) return chr;
					  else goto get;

			 case 210:
				if((entrycode & 128) ==128) return chr;
				else goto get;



			 case 31:      //key down
				 if((m_currentitem+1) == m_numitems) goto get;
				 m_currentitem++;
				 if(m_currentitem > m_topitem+ (m_maxviewlines-1)) m_topitem++;
				 goto disp;


			case 30:      //key up
				if(!m_currentitem) goto get;
				m_currentitem--;
				if(m_currentitem < m_topitem) m_topitem--;
				goto disp;


			case 203:    //page down
				if(m_currentitem == (m_numitems-1)) goto get;
				t =0;
				botitem = m_topitem+ m_maxviewlines-1;
				if((botitem+1) >= m_numitems) t=1;      //set flg  (pos cur on bot)
				nextpagebotitem = botitem+ m_maxviewlines;
				if(nextpagebotitem+1 >= m_numitems)
				{
					nextpagebotitem = m_numitems-1;
				}
				//remember current relative cursor position
				m_relativecurrentline = m_currentitem - m_topitem;

				m_topitem = nextpagebotitem - (m_maxviewlines-1);
				if(m_topitem <0) m_topitem =0;
				m_currentitem = m_topitem +m_relativecurrentline;
				if((m_currentitem+1) > m_numitems)
				{
					m_currentitem = m_numitems-1;
				}

				if(t)  //if set pos cursor on bottom, because bottom item was
				{      //already displayed
					m_currentitem = botitem;
				}
				m_relativecurrentline = m_currentitem - m_topitem;
				goto disp;


			 case 202:       //page up
				if(!m_currentitem) goto get;    //already on 1st line
				if(!m_topitem)
				{
					m_currentitem = 0;
					m_relativecurrentline = 0;
					goto disp;
				}
				//remember current cursor pos
				m_relativecurrentline = m_currentitem - m_topitem;
				m_topitem = m_topitem - m_maxviewlines;
				if(m_topitem < 0) m_topitem =0;
				m_currentitem = m_topitem + m_relativecurrentline;
				goto disp;




		  case 257: //button clicked
		  {
			  i = this->WhichFieldIsMouseIn(&m_mousefields[0], &mouse, 4);
					  //gotoxy(1,1); printf("%5d   ",i);   //for debugging
			  if(i ==-1)
			  {
				  i= this->WhichFieldIsMouseIn2(&m_mousefields[0], &mouse,4);
				  if(i ==-1)
				  {
					  if((entrycode & 1024) != 1024) goto get;
					  else return -1;  //clicked outside field
				  }                   //caller must check if its in one oo
											//his fields, else reenter listbox

				  else goto get;     //its in one of the listbox fields, but its
											//available flag is off.
			  }

				switch(i)  //the fields the click ocurred in
				{
					case 0:    //clicked in area
						 t = m_topitem +(mouse.y - m_mousefields[0].y1);
						 if((t+1) > m_numitems) t = m_numitems-1;
						 m_currentitem =t;
						 goto disp;

						case 1:      //click occurred in up arrow
						if(!mouse.ButtonState) goto get;

ua:               t = this->WaitMouseButtonRelease(30);
						goto wa35;
wa3:              t = this->WaitMouseButtonRelease(4);
wa35:             if(!m_currentitem) goto get;
						m_currentitem--;
						if(m_topitem) m_topitem--;
						this->DispListBoxLines(gettext, &txtlines2[0]);
						if(!t) goto get;           //button released
						goto wa3;


				  case 2:    // click occurred in down arrow
						if(!mouse.ButtonState) goto get;
da:               t = this->WaitMouseButtonRelease(30);
						goto wa45;
wa4:              t = this->WaitMouseButtonRelease(4);
wa45:             if((m_currentitem+1) == m_numitems) goto get;
						m_currentitem++;
						m_topitem++;
						if((m_topitem +m_maxviewlines) >m_numitems) m_topitem--;
						this->DispListBoxLines(gettext, &txtlines2[0]);
						if(!t) goto get;           //button released
						goto wa4;


						case 3:   //click occurred in scrollbar
sb:               this->LimitMouse(m_mousefields[3].x1, m_mousefields[3].y1,
											 m_mousefields[3].x2, m_mousefields[3].y2);

wa5:              chr = this->GetMouseOrKey();
						if(!mouse.ButtonState)
						{
mget:                 this->LimitMouse(0,0, 79, 24);
							 goto get;
						}

wa55:             t = mouse.y;
wa56:             chr = this->WaitGetMouseOrKey();
						if(chr != 256) //mouse not moved
						{
							if(!mouse.ButtonState) goto mget; //button released
							else goto wa56;
						}
						if(t == mouse.y) goto wa56; //move not big enough

						if(t< mouse.y)
						{

							this->MoveScrollbarCursorDown(gettext, &txtlines2[0]);
							goto wa55;
						}
						else
						{
							this->MoveScrollbarCursorUp(gettext, &txtlines2[0]);
							goto wa55;
						}

						default:
						goto get;
				}  //end of mouse fields click occurred in (switch)
			 } //end of case257



			 case 258:  //double click
				 i = this->WhichFieldIsMouseIn(&m_mousefields[0], &mouse, 4);

				 if(i ==-1)
				 {
					 i= this->WhichFieldIsMouseIn2(&m_mousefields[0], &mouse,4);
					 if(i ==-1) //its outside listbox fields
					 {
						 if((entrycode &2048)  != 2048) goto get;
						 else
						 {

													  //cursor select off:stay in listbox
							 return -1;         //double clicked outside field
													  //caller must check if its in one of
						 }                     //his fields, else reenter listbox
					 }
						 else goto get; //its in one of the listbox fields, but its
											 //available flag is off.
				 }

				 switch(i)  //the fields the double click ocurred in
				 {
					 case 0:    //clicked in lines list area
					 t = m_topitem +(mouse.y - m_mousefields[0].y1);
					 if((t+1) > m_numitems) t = m_numitems-1;
					 m_currentitem =t;
					 DispListBoxLines(gettext, &txtlines2[0]);
					 if((entrycode & 256) !=256) goto get;
					 else
					 {
						 //if(!m_cursorstyle) goto get;  //cursor off: stay in listbox
						 return 13;     //tell caller user chose m_currentitem
					 }
				 }

			 default:
			 goto get;
	}

}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//Entry:
// txtlines[]    = addresses of each lines to display
// The first line at &txtlines2[0] is taken as the title of the dialog box
// Therefore numlines = the total number of lines from &txtlines2[1] to
// to &txtlines2[numlines]
// numlines      = number of lines to display
// the function --- char *GetText(char *txtlines[], numlines); ---
// in the file "trash.cpp" is the function the listbox calls each time
// it wants to get the addr of a line of text to display it.
//trash.cpp is no a class of functions
//cursorstyle = 0 no cursor
//              1 small cursor only
//              2 big cursor  only
//              3 both
//Exit:
//  ax = either 27 or 13
//  if ax = 13 you need to examine m_buttonscursorpos to determine which
//  which button the user pressed <return> on
//if it is the first then m_buttonscursorpos =0,
//if it is the second button then m_buttonscursorpos=1
//If you are just displaying a message you don't need to check the exit value.
//When the listbox receives focus the buttons cursor is on the first button
//so that if the user pressed <return> on a line in the listbox then
//m_buttonscirsorpos =0 upon exit, so you would need to look at
//m_currentitem to see the c # of the item the user pressed <return> on.
int WinCtrls::ShowDialogBox(char *txtlines2[], int numlines,
															  int buttonsbits,
															  int f_maxviewlines,
                                               int cursorstyle,
															  int getout, int repeatflg)
{
	TMousePos MousePos[4];
	int entrycode, x, chr, t, j, i, lenmsg, stpos, x1, y1, lenbuf, boxstyle;
	char linebuf1[81];
	char msgbuf[81], *p;
	entrycode = 1+256+512;               //exit upon tabkey, <return>, <esc>
	m_repeatflg = repeatflg;
	//------------------------
	m_buttonsnum = this->CountButtons(buttonsbits);
	if(m_buttonsnum)
		this->SetTextOnButtons(m_buttonsnum, buttonsbits);

	m_numitems = numlines+1; //scan all the strings including the title
	m_width = 6 + this->FindLongestLen(GetText, &txtlines2[0]);
	m_numitems = numlines;  //restore to original number

	if(m_width <30) m_width =30;
	if(m_buttonsnum > 2 && m_width < 40) m_width =40;
	if(m_width >76) return -1;          //error
	m_height = f_maxviewlines+6;
	m_box_x1 =40 - (m_width /2);
	m_box_x2 = m_box_x1 +(m_width -1);
	m_box_y1 = 12 - (m_height /2);
	m_box_y2 = m_box_y1 +(m_height -1);
	//-----------
	//set up some listbox vars
	entrycode+= 1024+2048;               //exit listbox upon click, dblclick
	//setup vars for DoListBox()
	m_maxviewlines = f_maxviewlines;
	m_numitems = numlines;
	m_topitem =0;
	m_x1 = m_box_x1+1; m_y1 =m_box_y1+1; m_x2 = m_box_x2-1;
	m_y2 = m_box_y2-3;
	m_currentitem = m_topitem;
	m_cursorstyle =cursorstyle;
	//-----------

	this->OpenShadowBox(m_box_x1, m_box_y1, m_box_x2, m_box_y2,
													  LIGHTGRAY*16+BLACK, 0);

	memset(linebuf1, 32, m_width);
	linebuf1[m_width] =0;
	p = GetText(&txtlines2[0], 0);
	strcpy(msgbuf, p);
	lenbuf = strlen(linebuf1);
	lenmsg = strlen(msgbuf);
	stpos = (lenbuf /2) - (lenmsg /2); //pos to entr titl text into linebuf
	for(t=0; t<lenmsg; t++){
		linebuf1[stpos+t] = msgbuf[t];
	}
	this->WriteLine(linebuf1, m_box_x1, m_box_y1, -1, WHITE*16+BLACK);
	//if there are any buttons set up the values to show them
	x1 = m_box_x1+2;
	y1 = m_box_y2-1;
	for(t=0; t< m_buttonsnum; t++){
		m_buttonsposx1[t] = x1;
		m_buttonsposy1[t] = y1;
		x1 += (m_width / m_buttonsnum)* (t+1)-1;
	}

	//if more than one button align last button on right margin
	if( m_buttonsnum >1)
	{
		t = m_buttonsnum-1;           //index to last button
		j = (m_box_x1 + m_width-1);   //right margin
		lenbuf = strlen(&m_buttonstxt[m_buttonsnum-1][0]);
		m_buttonsposx1[t] = j - (lenbuf+2);
	}

	//if only 1 button centre it
	if(m_buttonsnum == 1)
	{
		lenbuf = strlen(&m_buttonstxt[0][0]);
		m_buttonsposx1[0] =  m_box_x1+ (m_width /2) - (lenbuf /2);

	}
	//if three buttons center the middle one
	if(m_buttonsnum == 3)
	{
		lenbuf = strlen(&m_buttonstxt[1][0]);
		m_buttonsposx1[1] =  m_box_x1+ (m_width /2) - (lenbuf /2);

	}
	if(m_buttonsnum == 4)
	{
		lenbuf = strlen(&m_buttonstxt[1][0]);
		m_buttonsposx1[3] =  m_box_x1+ m_width -11;
		m_buttonsposx1[2] +=- 8;
		m_buttonsposx1[1]+=1;

	}
	//setup the mouse fields of the buttons
	for(t = 0; t< m_buttonsnum; t++){
		MousePos[t].x1 = m_buttonsposx1[t];
		MousePos[t].y1 = m_buttonsposy1[t];
		MousePos[t].x2 = MousePos[t].x1 + strlen(&m_buttonstxt[t][0]) -1;
		MousePos[t].y2 = MousePos[t].y1;
		MousePos[t].available =1;
	}
get:
	m_buttonpressed =0;
	m_buttonscursorpos =0;
	this->ShowButtons();

	//------------------------
	//The dialog box is now opened, display the listbox in it
	boxstyle =1;      //visable border in listbox
	if(m_maxviewlines >= m_numitems)
	{
		boxstyle =0; //no visaable border
		entrycode = 1+256+512+1024;//exit upon tabkey, <return>, <esc>,clk
	}


  if((m_maxviewlines >= m_numitems) && m_cursorstyle ==0)
  {
	//if we can display all the lines at once then force a tab key into the
  //buffer so that upon the listbox function receive the tab key and exit
  //immediately after displaying the lines
		this->PushTab();

	}
	chr =this->DoListBox(GetText, &txtlines2[1], entrycode, boxstyle);
	//if user pressed tab goto buttons
	//if user pressed <return> accept as ok
	//if user clicked <check if outside listbox fields
	//if so check if on which button, if any

	switch(chr)
	{

			 case -1:  //clicked outside of field, see if on buttons
kpw:     i = this->WhichFieldIsMouseIn(&MousePos[0], &mouse, m_buttonsnum);
			 if(i ==-1) goto get;  //not on any button field, enter listbox
			 switch(i)
			 {
				 case 0:     //clicked on a button
				 case 1:
				 case 2:
				 case 3:
					  //show button as pressed and wait for it to be released
				 m_buttonpressed = i+1;      //the button that is to be pressed
				 this->ShowButtons();
				 x =0; if (((buttonsbits &1) ==1) && m_buttonpressed ==1) x =1;
				 gotoxy(m_buttonsposx1[i]+3+x, m_buttonsposy1[i]+1);
				 this->WaitMouseButtonRelease(-1);   //see "keybpard.cpp"
				 j = this->WhichFieldIsMouseIn(&MousePos[0], &mouse, m_buttonsnum);
				 //show button as released
				 m_buttonpressed =0;
				 m_buttonscursorpos =i;
				 this->ShowButtons();

				 //if the button when released is not in the same field as when
				 //pressed keep waiting
				 if(j ==-1) goto get;       //enter list box again
				 if(i != j) goto kpw;     //keep waiting in buttons field
				 if(i == 0)  //clicked on 1st button
				 {
					chr = 13; goto ex;
				 }
				 if( i == 1)   //clicked on 2nd button
				 {
					chr = 27;
					goto ex;
				 }
				 if( i == 2)
				 {
					chr  =2;
					goto ex;
				 }

				 default:           //not in buttons fields: enter listbox
					 goto get;

			 }   //end of ckicked in buttons fields switch

		  case 9:       //tabed from listbox: goto to ok button
			  m_buttonscursorpos =0;
			  m_buttonpressed = 0;
bget1:     this->ShowButtons();
			  i = m_buttonscursorpos;
			  x =0; if (((buttonsbits &1) ==1) && m_buttonpressed ==1) x =1;
			  gotoxy(m_buttonsposx1[i]+4+x, m_buttonsposy1[i]+1);

bget:      if(getout) goto ex2;     //leave box open and exit
			  chr = this->WaitGetMouseOrKey();
			  if(chr == 13) goto ex;
			  if(chr >256) goto kpw;   //  mouse clicked
			  if(chr == 28)  //right
			  {
				  if((m_buttonscursorpos+1) == m_buttonsnum) goto bget;
				  m_buttonscursorpos++; goto bget1;
			  }
			  if( chr ==29) //left
			  {
				  if(!m_buttonscursorpos) goto bget;
				  m_buttonscursorpos--; goto bget1;
			  }
			  if(chr == 27) goto ex;
			  if(chr == 30) goto get;  //return focus to listbox
			  goto bget;

			 case 27:
				 goto ex;

			 case 13:
				 goto ex;

			 default:            //default from exit from listbox
				 goto bget;       //
	}

ex:
	this->CloseBox();
ex2:
	return chr;
}
//-------------------------------------------------------------------------

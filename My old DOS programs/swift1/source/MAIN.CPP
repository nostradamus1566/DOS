/*-------------------------------------------------------------------------
	(C) Michael Finnegan     24 June 1999
-------------------------------------------------------------------------*/


#include "main.h"
#include "screen.h"
#include "clip.h"
#include "search.h"
#include "colours.h"
#include "disk.h"
#include <dos.h>
#include "error.h"
#include <stdlib.h>
int (*f[20])(Disk *a, int chr);
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//------------------------------------------------------------------------
//The user either pressed <return> over a menu item or
//pressed <esc> this function runs the corresponding menu item's command.
//Entry:
// i  hi = The c box #, i lo = the c item # in that box
//Exit:
// ax = -1 if <esc>

int RunMenuCmd(int i, Disk *a, File *b, Help *c)
{
		  int ch;

		  a->ShowTopMenu(0);             //show top menu off
		  switch(i)
		  {
			  case 0:
						printf("New\n");
						return 35;  //clear text from memory

			  case 1:
						b->Open(a);         //see "file.cpp"  for this function
						return 100;

			  case 2:
						b->Save(a);
						return 151;

			  case 3:
						b->SaveAs(a);      //see file.cpp
						return 152;

			 case 4:
					  textcolor(LIGHTGRAY);
					  textbackground(BLACK);
					  clrscr();
					  textcolor(YELLOW);
					  cprintf("Type exit to return to swift\n");
					  textcolor(LIGHTGRAY);
					  a->CursorOn();
					  strcpy(linebuf1, "C:\\");
					  _set_path(linebuf1);
					  system("");
					  //printf("close\n");
					  return 101;

			case 5:
					  PrintText(a);
					  return 0;

		  case 6:
			  return SaveSettings(a);  //save internal setting to c:\swift.var
			  //printf("Exit");
			  //return 34; //exit prg code

	case 256:
			 CutToClip(a);
			 return 0;

	case 257:
			 CopyToClip(a);
			 return 0;

	case 258:
		PasteClip(a);
		return 0;


	case 259:
			 ClearCut(a);
			 return 0;


	case 512:
			 FindText(a);
			 return 0;

	case 513:
			 a->PushF3();  //repeat last find
			 return 212;


	case 514:
			 ReplaceText(a);
			 return 0;

	case 768:
			 printf("Settings\n");
			 return 0;

	case 769:
			 ChangeColours(a);
			 return 101;

	case 1024:
			 c->Commands(a);
			 return 0;

	case 1025:
			 c->About(a);
			 return 0;

  default:

			return -1;
  }
}
//------------------------------------------------------------------------
//the user has pressed F10, activate the top menu bar and allow
//the user to select an option
//If the user selects an option run that options function.
//else return to caller
// Entry:
//  sw =0; show top menu and wait
//  sw = 1 show top menu and its submenu box and wait
//exit:
//ax =-1 if user quit menu
int DoMenu( int sw, Disk *a, File *b, Help *c)
{

	int i, mouseflg;
			 if(sw) goto sub;
			 i = a->GetTopMenuItem();    //show top menu as activated
			 if(i == -1 ) goto ex;
			//1=keyboard called this function.
sub:     mouseflg=1; //assume button up
			if(mouse.ButtonState) mouseflg=0;
			i = a->GetSubMenuItem(mouseflg);    //show menu box for that top menu item
												//i = -1 if <esc> else its the
												//c box # *256 + the c option
			 if( i==-1) goto ex;

			 gotoxy(15, 15);
			 i =RunMenuCmd(i, a, b, c);


ex:
		  a->ShowTopMenu(0);             //show top menu off
		  a->WriteLine(" ", 0, 24, 80, LIGHTGRAY*16 +BLACK);
		  return i;                    //user canceled top menu without displaying
}                                //a menu box
//------------------------------------------------------------------------
//the user has pressed F10, activate the top menu bar and allow
//the user to select an option
//If the user selects an option run that options function.
//else return to caller
// Entry:
//  sw =0; show top menu and wait
//  sw = 1 show top menu and its submenu box and wait
//exit:
//ax =-1 if user quit menu
int MouseDoMenu( int sw, Disk *a, File *b, Help *c)
{

	int i;
	Disk e;
	if(sw) goto sub;
	i = a->ShowTopMenu(2);

									//show top menu as activated
sub:  i = a->GetSubMenuItem(0);       //show menu box for that top menu item
		//i = -1 if <esc> else its the
		//c box # *256 + the c option
		if( i==-1) goto ex;   //if escape was pressed in the menu box
										  //skip the running of the commands

		//run the command the user selected from the menu
		gotoxy(15, 15);
		i =RunMenuCmd(i, a, b, c);

ex:
		  a->ShowTopMenu(0);             //show top menu off
		  DispStatusLine(a);
		  SetCursor(&e, 1);
		  //a->WriteLine(" ", 0, 24, 80, LIGHTGRAY*16 +BLACK);
		  return i;                 //user canceled top menu without displaying
}                                //a menu box
//------------------------------------------------------------------------
int tryload(Disk *a)
{
	char buf[80], m_path[128];
	int i, l, flg, t, topline;
	File b;

	_getcurrentpath();
	strcpy(m_path, currentpath);



	nodialog =1;  //don't show an critical err message
	strcpy(linebuf1, f_editortitle);
	lenbuf1 = strlen(linebuf1);
	t = a->RightFindChar(linebuf1, 92, lenbuf1);
	if(!t)
	{
		flg-1;
		goto ex;
	}
	strcpy(linebuf2, linebuf1);
	linebuf1[t] =0;
	i = _set_path(linebuf1);
	if(i <1)
	{
		flg =-1;
		goto ex;
	}
	//------
	strcpy(buf, &linebuf2[t]);
	a->RightTrim(buf);
	i = b.LoadFile(a, buf);
	if(i)
	{
		flg = -1;
		goto ex;
	}
	//if(i) goto   //file not found: create it
			//----------
	lines = a->GetLines();
	//make sure the last line is blabk
	lenbuf1 = a->GetLineBuf(linebuf1, &lines);
	lenbuf1 = a->LeftTrim(linebuf1);
	if(lenbuf1) //append a blank line
	{
		linebuf1[0] =13;
		linebuf1[1] =0;
		a->AppendLine(linebuf1);
		lines++;
	}
	strcpy(editortitle, a->GetPath());
	l = strlen(editortitle);
	if(editortitle[l-1] !='\\')
	{
		editortitle[l] = '\\';
		l++;
		editortitle[l] =0;
	}
	strcat(editortitle, buf);
	col = f_col;
	if(lines >= f_cline) cline = f_cline;
	else cline = lines;
	row = f_row;
	topline = (cline - row)+1;
	if(topline <1 )
	{
		topline = 1;
		row = (cline - topline) +1;
	}

	//-----
ex:
	  if(flg == -1)
	  {   //set the original path
			strcpy(currentpath, m_path);
			_set_path(m_path);
			editortitle[0] =0;
	  }


	return flg;
}
//------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  /*
	lpp =60;                    //lines per page
	m_wordwrap =1;
	mwwo =1;  //match whole word only
	mc =0;    //match case
	maxlen = 68;
	*/
	modified = 0;  //text not yet modified
	nodialog =0;    //normall allow critical fnc to show message
	oldhandler = getvect(INTR);
	setvect(INTR, criticalhandler);
	clrscr();
	mousehidden =1;              //assume mouse is hidden
	m_boxlevel =0;               //global var for screen save level
	tablen = 4;						//4 spcs = 1 tab
	char linebuf1[271];
	char statusline[81];
	TMousePos ed[5];


	int attrib, l, chr, i, t, s, x, len, x1, y1, x2, y2, numitems;
	int entrycode, runtimes;
	char buf[128];

	runtimes =0;

	Disk a, e; Help c;
	helpkey =1; //tell other function to escape when the help key is pressed
//new_mem:
	LoadSettings(&a);
	File b;
	lm = (78- maxlen)/ 2;
	deskjet =1;                  //assume deskjet printing method
	findwhat[0] =0;      //text to find buffer cleared




new_mem:
	attrib = gbc[2] *16 +gfc[2];

	//setup the mouse field coordinates for the mouse functions of the editor
	ed[0].x1 = 1; //top menu bar
	ed[0].y1 = 0;
	ed[0].x2 = 78;
	ed[0].y2 = 0;

	ed[1].x1 = 1; //main text area
	ed[1].y1 = 2;
	ed[1].x2 = 77;
	ed[1].y2 = 23;

	ed[2].x1 = 79; //up arrow on scrollbar
	ed[2].y1 = 2;
	ed[2].x2 = 79;
	ed[2].y2 = 2;

	ed[3].x1 = 79; //down arrow on scrollbar
	ed[3].y1 = 23;
	ed[3].x2 = 79;
	ed[3].y2 = 23;

	ed[4].x1 = 79; //scrollbar  range
	ed[4].y1 = 3;
	ed[4].x2 = 79;
	ed[4].y2 = 22;

	//----------------
	f[0] = EnterChar;
	f[1] = CursorUp;
	f[2] = CursorDown;
	f[3] = CursorLeft;
	f[4] = CursorRight;
	f[5] = PageDown;
	f[6] = PageUp;
	f[7] = CursorHome;
	f[8] = CursorEnd;
	f[9] = EnterTab;
	f[10] = EnterReturn;
	f[11] = DeleteChar;
	f[12] = EscChar;
	f[13] = DeleteLine;
	f[14] = CutLines;
	f[15] = CopyLines;
	f[16] = PasteLines;
	f[17] = ShowFreeMem;
	//----------------

	e.InitMem();

	if(!runtimes) //if there is a file as a command line argument, try to
	{             //load it, if not found creat a new file of that name
		if(argc >1)
		{
			m_rw = 2; //par for openbox to auto load if possible
			strcpy(buf, argv[1]);
			b.getname(&a, buf);
			i = b.LoadFile(&a, buf);
			//if(i) goto   //file not found: create it
			//----------
			lines = a.GetLines();
			//make sure the last line is blabk
			lenbuf1 = a.GetLineBuf(linebuf1, &lines);
			lenbuf1 = a.LeftTrim(linebuf1);
			if(lenbuf1) //append a blank line
			{
				linebuf1[0] =13;
				linebuf1[1] =0;
				a.AppendLine(linebuf1);
				lines++;
			}
			strcpy(editortitle, a.GetPath());
			l = strlen(editortitle);
			if(editortitle[l-1] !='\\')
			{
				editortitle[l] = '\\';
				l++;
				editortitle[l] =0;
			}
			strcat(editortitle, buf);
			modified =0;

		//----------
		}
		else  //try to load in the file whoes path and name is in
		{     //swift.vas
			i = tryload(&a);
			modified =0;
			nodialog =0;    //normal allow critical fnc to show message

		}



	}

vtop:
	attrib = gbc[2] *16 +gfc[2];

	SetCursor(&a, 1);
	a.LimitMouse(0, 0, 79, 24);
	if(!runtimes)
	{
		a.SetMousePos(0,0);           //set mouse at home position
		runtimes++;
	}
	a.ShowMouse();

	a.ClrScr(0, 0, 79, 24, gbc[2]*16+gfc[2]);
	a.DrawBox(0, 1, 79, 24, gbc[0]*16+gfc[0], 1);
	a.ClrScr(1, 2, 78, 23, gbc[2]*16+gfc[2]);
	chr =0;
get1:
	DispTitle(&e);
get3:
	DispLines(&e, (cline-(row-1)), 22, 1, gbc[2]*16+gfc[2]);
get2:

	SetCursor(&e, 1);
	DispStatusLine(&e);


get:
	prevchr = chr;
	chr = a.WaitGetMouseOrKey();
	//b.ShowMouseCoordinates(&a); //for debugging only
	switch(chr)
	{
		case 210: //F1
			c.Commands(&a);
			DispStatusLine(&e);
			goto get;

		case 212:  //f3 "repeat last find"
			 chr = 212;
			 RepeatFindText(&a);
			 chr =212;
			goto get2;

		case 27:  //esc
			goto get1;

		case 219: //F10 pressed do menu
			SetCursor(&a, 0);     //cursor off
			i = DoMenu(0, &a, &b, &c);
			if(i == 34) goto ex; //user chose exit from exit option in menu
			if(i == 35) goto new_mem; //clear text from memory
			else goto get1;

		//case 256: //mouse movement
		  //	if(mouse.ButtonState==1) goto a1; //button was down when moement
														 //occurred
		case 257: //button clicked
			i =a.WhichFieldIsMouseIn2(&ed[0], &mouse, 5);
			if(i ==-1) goto get;
			switch(i)
			{
				case 0:  //click occurred on top menu line
					i = a.CheckTopMenu();  //is it on an item in the top menu?
					if(i==-1) goto get;
					a.m_topMenuCursorPos = i;
					SetCursor(&e, 0);     //cursor off
					i = MouseDoMenu(0, &a, &b, &c);
					if( i >= 151 && i <= 152) goto get1;
					if(i == 34) goto ex;
					if(i == 35) goto new_mem; //clr text from memory
					if(i == 212) chr = 212;
					if(i == 100) goto get1;
					if(i == 101) goto vtop;
					SetCursor(&e, 1);     //cursor on

					goto get;

				case 1:  //click occurred in text area
					SetTextCursor(&a); //attempt to set the text cursor at the
					goto get;          //mouse pos

				case 2:
					ScrollUpArrow(&a);
					goto get;

				case 3:
					ScrollDownArrow(&a);
					goto get;

				case 4:
					ClickInScrollBar(&a);
					goto get;

				default:
					goto get;
			} //end of switch statement for mouse click

		default:   //assume the user pressed a key
			//-------------
			i = TranslateKey(chr);
			if( i == -1) goto get;
			i = (*f[i])(&e, chr);  //call editor function
			goto get;

	} //main end of switch





//----------------------------------------------------------------------

		  //entrycode = 1+8+256+512+1024;
		  //a.DoListBox(GetText, &echar[0], entrycode);

		  //a.ShowDialogBox(&echar[0], 24, 1, 12, 3);

		  //chr = getch();
ex:
		  a.ClrScr(0, 0, 79, 24, BLACK*16 + LIGHTGRAY);
		  gotoxy(1, 25);
		  a.HideMouse();
		  a.CursorOn();
		  setvect(INTR, oldhandler);
		  return 0;
}
//-----------------------------------------------------------------------

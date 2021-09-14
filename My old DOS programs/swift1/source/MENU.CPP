/*-------------------------------------------------------------------------
	(C) Michael Finnegan     29 June 1999
-------------------------------------------------------------------------*/
#include "menu.h"
#include "ctype.h"
//menu.h includes keyboard.h which in turn includes screen.h
//------------------------------------------------------------------------
//a click occurred on the top line, probably indicating that the user
//wants to choose a top menu item.
//see if it is on a top menu item.
//If so return the c # of that item ( 0- 4)
//else return -1
int Menu::CheckTopMenu()
{
	int field, t;
	int fields[5][2];

	field =-1;   //assume out of horozantol range of top item
	for(t=0; t< 4; t++){
		fields[t][0] = this->m_topMenuPos[t];
		fields[t][1] = this->m_topMenuPos[t+1] -4;
  }

  fields[4][0] = this->m_topMenuPos[4];
  fields[4][1] = fields[4][0] +6;

  for(t= 0; t< 5; t++){
	  if(fields[t][0] > mouse.x) continue; //mouse behind beginning of 1st
														//menu item
	  if(fields[t][1] < mouse.x) continue; //mouse behind beginning of 1st
														//menu item
	  field = t; break;
 }

return field;
}

//------------------------------------------------------------------------
//This class is derived from Keyboard class
Menu::Menu()
{
	int yes = 1, no=0;
	//map the screen coordinates of all the menu boxes
	//file box
	m_bigmapper[0][0].x1 = 3;
	m_bigmapper[0][1].x1 = 3;
	m_bigmapper[0][2].x1 = 3;
	m_bigmapper[0][3].x1 = 3;
	m_bigmapper[0][4].x1 = 3;
	m_bigmapper[0][5].x1 = 3;
	m_bigmapper[0][6].x1 = 3;

	m_bigmapper[0][0].y1 = 2;    //new
	m_bigmapper[0][1].y1 = 3;    //open
	m_bigmapper[0][2].y1 = 4;    //save
	m_bigmapper[0][3].y1 = 5;    //save as...
	m_bigmapper[0][4].y1 = 6+1;  //dos shell
	m_bigmapper[0][5].y1 = 8+1;  //print...
	m_bigmapper[0][6].y1 = 10+1; //exit

	m_bigmapper[0][0].x2 = 14;
	m_bigmapper[0][1].x2 = 14;
	m_bigmapper[0][2].x2 = 14;
	m_bigmapper[0][3].x2 = 14;
	m_bigmapper[0][4].x2 = 14;
	m_bigmapper[0][5].x2 = 14;
	m_bigmapper[0][6].x2 = 14;

	m_bigmapper[0][0].y2 = 2;
	m_bigmapper[0][1].y2 = 3;
	m_bigmapper[0][2].y2 = 4;
	m_bigmapper[0][3].y2 = 5;
	m_bigmapper[0][4].y2 = 6+1;
	m_bigmapper[0][5].y2 = 8+1;
	m_bigmapper[0][6].y2 = 10+1;
	//-------------------------
	//edit box
	m_bigmapper[1][0].x1 = 12;
	m_bigmapper[1][1].x1 = 12;
	m_bigmapper[1][2].x1 = 12;
	m_bigmapper[1][3].x1 = 12;

	m_bigmapper[1][0].y1 = 2;
	m_bigmapper[1][1].y1 = 3;
	m_bigmapper[1][2].y1 = 4;
	m_bigmapper[1][3].y1 = 5;

	m_bigmapper[1][0].x2 = 29;
	m_bigmapper[1][1].x2 = 29;
	m_bigmapper[1][2].x2 = 29;
	m_bigmapper[1][3].x2 = 29;

	m_bigmapper[1][0].y2 = 2;
	m_bigmapper[1][1].y2 = 3;
	m_bigmapper[1][2].y2 = 4;
	m_bigmapper[1][3].y2 = 5;
  //---------------------------------
	//search box
	m_bigmapper[2][0].x1 = 21;
	m_bigmapper[2][1].x1 = 21;
	m_bigmapper[2][2].x1 = 21;

	m_bigmapper[2][0].y1 = 2;
	m_bigmapper[2][1].y1 = 3;
	m_bigmapper[2][2].y1 = 4;

	m_bigmapper[2][0].x2 = 42;
	m_bigmapper[2][1].x2 = 42;
	m_bigmapper[2][2].x2 = 42;

	m_bigmapper[2][0].y2 = 2;
	m_bigmapper[2][1].y2 = 3;
	m_bigmapper[2][2].y2 = 4;
  //---------------------------
	//options box
	m_bigmapper[3][0].x1 = 32;
	m_bigmapper[3][1].x1 = 32;

	m_bigmapper[3][0].y1 = 2;
	m_bigmapper[3][1].y1 = 3;

	m_bigmapper[3][0].x2 = 44;
	m_bigmapper[3][1].x2 = 44;

	m_bigmapper[3][0].y2 = 2;
	m_bigmapper[3][1].y2 = 3;
  //--------------------------
		//help box
	m_bigmapper[4][0].x1 = 44;
	m_bigmapper[4][1].x1 = 44;

	m_bigmapper[4][0].y1 = 2;
	m_bigmapper[4][1].y1 = 3;

	m_bigmapper[4][0].x2 = 56;
	m_bigmapper[4][1].x2 = 56;

	m_bigmapper[4][0].y2 = 2;
	m_bigmapper[4][1].y2 = 3;
  //-------------------------
	//set up the data for the main menu on the top of the screen
	m_menu[0][0].txt = "File";
	m_menu[0][0].len = strlen(m_menu[0][0].txt);
	m_menu[0][0].qletter = 'F';
	m_menu[0][0].qletterpos = 0;
	m_menu[0][0].qlettersw = yes;
	m_menu[0][0].barsw = no ;
	m_menu[0][0].exptxt = "Commands for manipulating files";
	m_menu[0][0].availablesw = yes;

	m_menu[1][0].txt = "Edit";
	m_menu[1][0].len = strlen(m_menu[1][0].txt);
	m_menu[1][0].qletter = 'E';
	m_menu[1][0].qletterpos = 0;
	m_menu[1][0].qlettersw = yes;
	m_menu[1][0].barsw = no ;
	m_menu[1][0].exptxt = "Commands for editing text";
	m_menu[1][0].availablesw = yes;

	m_menu[2][0].txt = "Search";
	m_menu[2][0].len = strlen(m_menu[2][0].txt);
	m_menu[2][0].qletter = 'S';
	m_menu[2][0].qletterpos = 0;
	m_menu[2][0].qlettersw = yes;
	m_menu[2][0].barsw = no ;
	m_menu[2][0].exptxt = "Commands for searching and replacing text";
	m_menu[2][0].availablesw = yes;

	m_menu[3][0].txt = "Options";
	m_menu[3][0].len = strlen(m_menu[3][0].txt);
	m_menu[3][0].qletter = 'O';
	m_menu[3][0].qletterpos = 0;
	m_menu[3][0].qlettersw = yes;
	m_menu[3][0].barsw = no ;
	m_menu[3][0].exptxt = "Commands for setting editor options";
	m_menu[3][0].availablesw = yes;

	m_menu[4][0].txt = "Help";
	m_menu[4][0].len = strlen(m_menu[4][0].txt);
	m_menu[4][0].qletter = 'H';
	m_menu[4][0].qletterpos = 0;
	m_menu[4][0].qlettersw = yes;
	m_menu[4][0].barsw = no ;
	m_menu[4][0].exptxt = "Help on Swift";
	m_menu[4][0].availablesw = yes;

	//set the x position of the top menu items
	m_topMenuPos[0] = 2;
	m_topMenuPos[1] = m_topMenuPos[0] + m_menu[0][0].len +5;
	m_topMenuPos[2] = m_topMenuPos[1] + m_menu[1][0].len +5;
	m_topMenuPos[3] = m_topMenuPos[2] + m_menu[2][0].len +5;
	m_topMenuPos[4] = m_topMenuPos[3] + m_menu[3][0].len +5;

	//set up the data for the sub options for 'file'
	m_menu[0][1].txt = "New";
	m_menu[0][1].len = strlen(m_menu[0][1].txt);
	m_menu[0][1].qletter = 'N';
	m_menu[0][1].qletterpos = 0;
	m_menu[0][1].qlettersw = yes;
	m_menu[0][1].barsw = no ;
	m_menu[0][1].exptxt = "Clears the text memory for a new file";
	m_menu[0][1].availablesw = yes;

	m_menu[0][2].txt = "Open...";
	m_menu[0][2].len = strlen(m_menu[0][2].txt);
	m_menu[0][2].qletter = 'O';
	m_menu[0][2].qletterpos = 0;
	m_menu[0][2].qlettersw = yes;
	m_menu[0][2].barsw = no ;
	m_menu[0][2].exptxt = "Loads an existing file from disk";
	m_menu[0][2].availablesw = yes;

	m_menu[0][3].txt = "Save";
	m_menu[0][3].len = strlen(m_menu[0][3].txt);
	m_menu[0][3].qletter = 'S';
	m_menu[0][3].qletterpos = 0;
	m_menu[0][3].qlettersw = yes;
	m_menu[0][3].barsw = no ;
	m_menu[0][3].exptxt = "Saves the current file";
	m_menu[0][3].availablesw = yes;

	m_menu[0][4].txt = "Save as...";
	m_menu[0][4].len = strlen(m_menu[0][4].txt);
	m_menu[0][4].qletter = 'a';
	m_menu[0][4].qletterpos = 5;
	m_menu[0][4].qlettersw = yes;
	m_menu[0][4].barsw = yes ;
	m_menu[0][4].exptxt = "Saves the current file with a new name";
	m_menu[0][4].availablesw = yes;

	m_menu[0][5].txt = "DOS Shell";
	m_menu[0][5].len = strlen(m_menu[0][5].txt);
	m_menu[0][5].qletter = 'D';
	m_menu[0][5].qletterpos = 0;
	m_menu[0][5].qlettersw = yes;
	m_menu[0][5].barsw = yes ;
	m_menu[0][5].exptxt = "Shells out to DOS";
	m_menu[0][5].availablesw = yes;

	m_menu[0][6].txt = "Print...";
	m_menu[0][6].len = strlen(m_menu[0][6].txt);
	m_menu[0][6].qletter = 'P';
	m_menu[0][6].qletterpos = 0;
	m_menu[0][6].qlettersw = yes;
	m_menu[0][6].barsw = yes ;
	m_menu[0][6].exptxt = "Prints currently loaded file to the printer";
	m_menu[0][6].availablesw = yes;

	m_menu[0][7].txt = "Exit";
	m_menu[0][7].len = strlen(m_menu[0][7].txt);
	m_menu[0][7].qletter = 'x';
	m_menu[0][7].qletterpos = 1;
	m_menu[0][7].qlettersw = yes;
	m_menu[0][7].barsw = no ;
	m_menu[0][7].exptxt = "Exits the Swift word processor";
	m_menu[0][7].availablesw = yes;

	m_numOfSubOpts[0] = 7;

	//set up the data for the sub options for 'Edit'
	m_menu[1][1].txt = "Cut       Ctrl+x";
	m_menu[1][1].len = strlen(m_menu[1][1].txt);
	m_menu[1][1].qletter = 't';
	m_menu[1][1].qletterpos = 2;
	m_menu[1][1].qlettersw = yes;
	m_menu[1][1].barsw = no ;
	m_menu[1][1].exptxt = "Deletes selected text and copies it to clipboard";
	m_menu[1][1].availablesw = yes;

	m_menu[1][2].txt = "Copy      Ctrl+c";
	m_menu[1][2].len = strlen(m_menu[1][2].txt);
	m_menu[1][2].qletter = 'C';
	m_menu[1][2].qletterpos = 0;
	m_menu[1][2].qlettersw = yes;
	m_menu[1][2].barsw = no ;
	m_menu[1][2].exptxt = "Copies selected text to clipboard";
	m_menu[1][2].availablesw = yes;

	m_menu[1][3].txt = "Paste     Ctrl+v";
	m_menu[1][3].len = strlen(m_menu[1][3].txt);
	m_menu[1][3].qletter = 'P';
	m_menu[1][3].qletterpos = 0;
	m_menu[1][3].qlettersw = yes;
	m_menu[1][3].barsw = no ;
	m_menu[1][3].exptxt = "Inserts the clipboard contents at cursor location";
	m_menu[1][3].availablesw = yes;

	m_menu[1][4].txt = "Clear     Del";
	m_menu[1][4].len = strlen(m_menu[1][4].txt);
	m_menu[1][4].qletter = 'e';
	m_menu[1][4].qletterpos = 2;
	m_menu[1][4].qlettersw = yes;
	m_menu[1][4].barsw = no ;
	m_menu[1][4].exptxt = "Deletes selected text without copying it to clipboard";
	m_menu[1][4].availablesw = yes;

	m_numOfSubOpts[1] = 4;

	//set up the data for the sub options for 'Search'
	m_menu[2][1].txt = "Find...";
	m_menu[2][1].len = strlen(m_menu[2][1].txt);
	m_menu[2][1].qletter = 'F';
	m_menu[2][1].qletterpos = 0;
	m_menu[2][1].qlettersw = yes;
	m_menu[2][1].barsw = no ;
	m_menu[2][1].exptxt = "Finds specified text";
	m_menu[2][1].availablesw = yes;

	m_menu[2][2].txt = "Repeat Last find  F3";
	m_menu[2][2].len = strlen(m_menu[2][2].txt);
	m_menu[2][2].qletter = 'L';
	m_menu[2][2].qletterpos = 7;
	m_menu[2][2].qlettersw = yes;
	m_menu[2][2].barsw = no ;
	m_menu[2][2].exptxt = "Finds next occurance of text specified in previous search";
	m_menu[2][2].availablesw = yes;

	m_menu[2][3].txt = "Replace...";
	m_menu[2][3].len = strlen(m_menu[2][3].txt);
	m_menu[2][3].qletter = 'R';
	m_menu[2][3].qletterpos = 0;
	m_menu[2][3].qlettersw = yes;
	m_menu[2][3].barsw = no ;
	m_menu[2][3].exptxt = "Finds and changes specified text";
	m_menu[2][3].availablesw = yes;

	m_numOfSubOpts[2] = 3;

	//set up the data for the sub options for 'Options'
	m_menu[3][1].txt = "Settings...";
	m_menu[3][1].len = strlen(m_menu[3][1].txt);
	m_menu[3][1].qletter = 'S';
	m_menu[3][1].qletterpos = 0;
	m_menu[3][1].qlettersw = yes;
	m_menu[3][1].barsw = no ;
	m_menu[3][1].exptxt = "Changes word processer settings";
	m_menu[3][1].availablesw = no;

	m_menu[3][2].txt = "Colours...";
	m_menu[3][2].len = strlen(m_menu[1][2].txt);
	m_menu[3][2].qletter = 'C';
	m_menu[3][2].qletterpos = 0;
	m_menu[3][2].qlettersw = yes;
	m_menu[3][2].barsw = no ;
	m_menu[3][2].exptxt = "Changes word processor screen colours";
	m_menu[3][2].availablesw = yes;

	m_numOfSubOpts[3] = 2;

	//set up the data for the sub options for 'Help'
	m_menu[4][1].txt = "Commands...";
	m_menu[4][1].len = strlen(m_menu[3][1].txt);
	m_menu[4][1].qletter = 'C';
	m_menu[4][1].qletterpos = 0;
	m_menu[4][1].qlettersw = yes;
	m_menu[4][1].barsw = no ;
	m_menu[4][1].exptxt = "Help on Swift commands";
	m_menu[4][1].availablesw = yes;

	m_menu[4][2].txt = "About...";
	m_menu[4][2].len = strlen(m_menu[1][2].txt);
	m_menu[4][2].qletter = 'A';
	m_menu[4][2].qletterpos = 0;
	m_menu[4][2].qlettersw = yes;
	m_menu[4][2].barsw = no ;
	m_menu[4][2].exptxt = "Show version and copyright information";
	m_menu[4][2].availablesw = yes;

	m_numOfSubOpts[4] = 2;

	int t;
	for(t =0; t<5; t++){
		m_subMenuCursorPos[t] =0;  //set the cursor y pos in each box to
	}                            //the first sub item in that box

	m_topMenuCursorPos = 0;      //set cursor to first item in top menu
	m_bc = BLUE;
	m_fc = LIGHTGRAY;


}
//---------------------------------------------------------------------
//Show the top menu activated or off (normal)
//Entry:
// if sw =0 just show top menu as deactivated
// if sw = 1 show menu as activated and quick letters and cursor pos
// if sw = 2 show menu as activated and cursor pos but not quick letters

int Menu::ShowTopMenu( int sw)
{
	static int clrtopline;
	int t, x, len;
	char chr;
	char linebuf1[40];
	char linebuf2[40];

	//clr the top & bottom line if this is the first time this function
	//is called.

		this->WriteLine(" ", 0,0, 80, gbc[1]*16 +gfc[1]);
		clrtopline = 1;
		this->WriteLine(" ", 0, 24, 80, LIGHTGRAY*16 +BLACK);



	//display all the top line items
	for(t = 0; t< 5; t++){
		sprintf(linebuf2, " %s ",  m_menu[t][0].txt);
		this->WriteLine(linebuf2, m_topMenuPos[t], 0, -1, gbc[1]*16+gfc[1]);

	}

  //if top menu is activated show quick letters and current cursor position
  if(sw==1)
  {
		for(t = 0; t< 5; t++)
		{
			x = m_menu[t][0].qletterpos;
			chr = m_menu[t][0].qletter;
			linebuf1[0] = chr;
			linebuf1[1] = 0;
			this->WriteLine(linebuf1, m_topMenuPos[t]+ x+1, 0, -1,
															gbc[1]*16+WHITE);
		}

		//now that we have shown all the quick letters for the top menu
		//lets show the top menu current cursor position
		t = m_topMenuCursorPos;
		len = strlen(m_menu[t][0].txt);
		this->ColourLine(m_topMenuPos[t], 0, len+2, BLACK*16 + LIGHTGRAY);

		//we must once again show the quick letter for this item at the cursor
		//but this time against a black background
		x = m_menu[t][0].qletterpos;
		chr = m_menu[t][0].qletter;
		linebuf1[0] = chr;
		linebuf1[1] = 0;
		this->WriteLine(linebuf1, m_topMenuPos[t]+ x+1, 0, -1,
																	BLACK*16+WHITE);

		//show the explain text on the bottom of the screen for that
		//top menu item
		this->WriteLine(m_menu[t][0].exptxt, 1, 24, 80,
													LIGHTGRAY*16+BLACK);
		return 0;
  } //end if



	//if top menu is activated don't show quick letters,
	//but show the current cursor position on the top menu
  if(sw==2)
  {
		t = m_topMenuCursorPos;
		len = strlen(m_menu[t][0].txt);
		this->ColourLine(m_topMenuPos[t], 0, len+2, BLACK*16 + LIGHTGRAY);


		//show the explain text on the bottom of the screen for that
		//top menu item
		this->WriteLine(m_menu[t][0].exptxt, 1, 24, 80,
												LIGHTGRAY*16+BLACK);
		  return 0;
  } //end if

	return -1;

}
//------------------------------------------------------------------------
//This function turns on the top menu and waits, allowing the user to
//use the cursor keys left right keys to cursor to another item
//or the <esc> key to abort or the return key or down key to select this item
//Exit:
// ax = -1 if user aborted
//         and the top menu is deactivated
// else
//   ax = the c # of the item chosen
//        and the top menu is show as still activated
int Menu::GetTopMenuItem()
{
	int chr, tempchr, t, i;
	char quickletters[2][6];
	int lenquickletters;
	//get all thequick letters of the top menu into a list
	i =0;
	for(t = 0; t<5; t++){
		if(m_menu[t][0].availablesw)
		{
			quickletters[0][i] = m_menu[t][0].qletter;
			quickletters[1][i] = (char)i;
			i++;
		}
	}
	lenquickletters =i;        //r # of quick letters in the list

get1:
	this->ShowTopMenu(1);        //show top menu as activated
get:
	chr = this->WaitGetMouseOrKey();
	switch(chr)
	{
		case 27:                         //abort the selection of top menu item
j_esc:	this->ShowTopMenu(0);        //deactivate top menu
			return -1;

		case 13:             //<return>
		case 31:             //<down>
			return m_topMenuCursorPos;

		case 28:             //cursor right
			if(m_topMenuCursorPos == 4)
			{
				m_topMenuCursorPos = 0;
				goto get1;
			}
			else
			{
				m_topMenuCursorPos++;
				goto get1;
			}

			case 29:             //cursor left
				if(!m_topMenuCursorPos)
				{
					m_topMenuCursorPos = 4;
					goto get1;
				}
				else
				{
					m_topMenuCursorPos--;
					goto get1;
				}

			case 257: //click
				i = this->CheckTopMenu();
				if(i==-1) goto j_esc;
				m_topMenuCursorPos =i;
				return m_topMenuCursorPos;


			default:
				chr = toupper(chr);
				if(!(chr >='A' && chr <= 'Z'))  goto get; //not a quick letter
				if(!lenquickletters) goto get;           //no letters in list

				//see if the letter is in the list
				//if it is move the cursor to that item and exit.
				for(t = 0; t< lenquickletters; t++){
					tempchr = (char) m_menu[t][0].qletter;
					tempchr = toupper(tempchr);
					if(chr == tempchr)
					{
						m_topMenuCursorPos = quickletters[1][t];
						this->ShowTopMenu(1);        //show top menu as activated
						return m_topMenuCursorPos;
					}
				}
				goto get;   //user did not press a valid quick letter
	//----------------------old end of key loop



	}    //end switch

}
//------------------------------------------------------------------------
//This function shows a menu box of menu items and the position of the
//cursor in that box. But it doesn't wait for the user to enter
//from the keyboard
//Entry:
//  if drawboxflg = 0 dont't draw the box, because it was previously
//  from a previous call to this function with drawboxflg set to 0;
//If drawboxflg = 0  this function serves to show the new submenu cursor pos
//subitemsw  is the flag to indicate whether to show the subitem selector
//or not. subitemsw =1 show it (the default value), or =0 don't show it.
int Menu::ShowMenuBox(int drawboxflg, int subitemsw)
{
	int row,i, t, x, y, y2, x2, boxlen, len, avail_attrib, notavail_attrib;
	char linebuf1[81];
	char linebuf2[40];
	char linebuf3[81];

	avail_attrib = LIGHTGRAY*16 +BLACK;
	notavail_attrib = LIGHTGRAY*16+DARKGRAY;

	t = m_topMenuCursorPos;
		  //t =0;
	//based on the submenu items list calculate the following:
	//calc y2 par, and calc the
	//x2 pos of the box based on the longest string item
	y2 = 0;  x2 = 0;
 //if(subitemsw)

	for(i = 1; i<= m_numOfSubOpts[t]; i++){
		if(m_menu[t][i].barsw) y2++;
		boxlen = strlen(m_menu[t][i].txt);
		if(x2< boxlen) x2 = boxlen;

	}
	boxlen = x2+1;
	y2 += m_numOfSubOpts[t];


	x = m_topMenuPos[t];
	if(drawboxflg)
	{
		this->OpenShadowBox(x-1, 1, x+boxlen+3, y2+2, LIGHTGRAY*16+BLACK, 1);
	}
	//draw a line into linebuf1
	memset(linebuf1, 196, boxlen+3);
	linebuf1[0] = 195;
	linebuf1[boxlen+3-1] = 180;
	linebuf1[boxlen+3] = 0;
	//Now that we have displayed an empty box  lets show the list of
	//sub menu item in that box
	row = 0;
	for(i = 1; i<= m_numOfSubOpts[t]; i++){
		sprintf(linebuf2, " %s ", m_menu[t][i].txt);
		if(m_menu[t][i].availablesw)
		{
			this->WriteLine(linebuf2, x+1, row+ i+1,
									boxlen+1, avail_attrib);
		}
		else
		{
			this->WriteLine(linebuf2, x+1, row+ i+1,
								boxlen+1, notavail_attrib);
		}

		//if the the sub menu cursor is at this item show it
		//by displaying the item text in a LIGHTGRAY background and a black
		//foreground.
		if(m_subMenuCursorPos[t] == (i-1))
		{


				if(subitemsw) this->WriteLine(linebuf2, x+1, row+ i+1,
							boxlen+1, BLACK*16 +LIGHTGRAY);

				//if the option is available show the quick letter
				//in white on a black background
				if(m_menu[t][i].availablesw)
				{
					if(subitemsw)
					this->ColourLine(x+2+m_menu[t][i].qletterpos, row+ i+1, 1,
																			BLACK*16+WHITE);
					else //quick letter in light gray cause selector is off
					  this->ColourLine(x+2+m_menu[t][i].qletterpos, row+ i+1, 1,
																			LIGHTGRAY*16+WHITE);


				}

			//show the explain text for this option
			strcpy(linebuf3, m_menu[t][i].exptxt);
						this->WriteLine(linebuf3, 1, 24, 79, LIGHTGRAY*16 +BLACK);
		}
		//sub menu cursor is not at this item so just display its
		//quick letter if the option is available
		else
		{
			if(m_menu[t][i].availablesw)
			{
				this->ColourLine(x+2+m_menu[t][i].qletterpos, row+ i+1, 1,
																	LIGHTGRAY*16+WHITE);
			}
		}

		//show the bar if any, under this option
		if( m_menu[t][i].barsw && drawboxflg)
		{
			this->WriteLine(linebuf1, x, row+ i+2, -1, LIGHTGRAY*16+ BLACK);
			row++;
		}
		// since we are not drawing the box we dont need to draw a bar under
		//an option. But if the option has a bar we need to inc the row so that
		//the next option will appear in its proper place
		else
		{
			if(m_menu[t][i].barsw) row++;
		}


	}  //end for
	goto ex;

//-------------

//-----------
ex:
	return 0;
}
//------------------------------------------------------------------------
//This function waits for the user to select a sub menu item
// it does the following jobs:
// 1) show the top menu as activated
// 2) calls Menu::ShowMenuBox to open up a sub menu box and show
//    the current position of the submenu cursor within that box
// 3) waits for a key
// 4) if its cursor left or right it closes the box and opens the next one
// 5) if its <return> over a valid option it closes the box and exits
//    with the option of the submenu box
// 6) if its <esc> it closes the box, deactivates the top menu and
//     exits with -1
// 7) if its up or down key it allows cursoring within that box

//   keybrd = 1 if f10 called this function
//   keybrd =0 if mouse called this function

//Exit:
//  if the user chose a valid item ax = the c box # and the c item num
//  within that box as  c boxnum * 256 +  c itemnum

int Menu::GetSubMenuItem(int keybrd)
{
	int qletters[2][12], time, time1, keybrd1;
	int chr, t, k, i, quicklen, tt;
	int tempchr, boxon;

	keybrd1 = keybrd;
	boxon=0;     //box's ititial state closed
	time=0;    //first time running this function
	time1=0;
get1:
	this->ShowTopMenu(2); //show top menu as activated even it if was already
	if(boxon) {this->CloseBox(); boxon=0;} //close previous box before
														//opening a new one
	this->ShowMenuBox(1, keybrd1); //open up the current sub menu box
	keybrd1=1;
	boxon =1;  //flg box is opened

	t = m_topMenuCursorPos;

	//collect all of the available quick letters for this box into qletters[]
	quicklen = 0;         //len of qletters[]
	for(i = 0; i< m_numOfSubOpts[t]; i++){
		if(m_menu[t][i+1].availablesw)
		{
			qletters[0][quicklen] = (char) m_menu[t][i+1].qletter;
			qletters[1][quicklen] = t*256 + i;
			quicklen++;
		}
	}



get:

  chr= WaitGetMouseOrKey();
got:
	switch(chr)
	{
		case 31:                //cursor down
			if((m_subMenuCursorPos[t]+ 1) == m_numOfSubOpts[t])
			{
				m_subMenuCursorPos[t] = 0;
			}
			else
			{
				m_subMenuCursorPos[t]++;
			}
			this->ShowMenuBox(0);  //show the cursor within that box at its
										  //new position.
			goto get;


		case 30:                //cursor up
			if((m_subMenuCursorPos[t]) == 0)
			{
				m_subMenuCursorPos[t] = m_numOfSubOpts[t]-1;
			}
			else
			{
				m_subMenuCursorPos[t]--;
			}
			this->ShowMenuBox(0);  //show the cursor within that box at its
										  //new position.
			goto get;

		case 27:
j_esc:
			if(boxon) this->CloseBox();
			this->ShowTopMenu(0);     //deactivate top menu and close box
			return -1;



		case 28:                   //cursor right: close the old box and
			if(boxon) {this->CloseBox();boxon=0;}       //open the next one
			if((t+1) == 5)
			{
				t = 0;
			}
			else
			{
				t++;
			}
			m_topMenuCursorPos = t;
			goto get1;


		case 29:                   //cursor left: close the old box and
			if(boxon){this->CloseBox(); boxon=0;}      //open the next one
			if(!t)
			{
				t = 4;
			}
			else
			{
				t--;
			}
			m_topMenuCursorPos = t;
			goto get1;


rt:		case 13:              //if the option is not available keep waiting
			i = m_subMenuCursorPos[m_topMenuCursorPos];
			t= m_topMenuCursorPos;
			if(m_menu[t][i+1].availablesw)
			{
				if(boxon){this->CloseBox(); boxon=0;}
				return m_topMenuCursorPos*256 +i;
			}
			else
				goto get;

		//----------------------------
		  case 256: //mouse movement
			  if(mouse.ButtonState==0) goto get;
			  chr=this->MouseMovement(boxon);
			  goto got;
		//------------------------------
		 case 257:  //click
			 if(mouse.ButtonState==0)  //a release click
			 {
				 if(mouse.y ==0)  //if mouse on top line
				 {
					 if(boxon)     //if box is showing
					 {  //show the selector on the 1st subitem
						 m_subMenuCursorPos[m_topMenuCursorPos]=0;
						 this->ShowMenuBox(0,1);
					 }

					 goto get;
				 } //end if statement for mouse release click when on top line


				 if(boxon==0) goto j_esc;  //escape cause box closed
				 //see if the click occurred on an item in the box
				 this->CopyCoordinates();
				 i= this->WhichFieldIsMouseIn2(&m_boxmapper[0], &mouse, 16);
				 if(i != -1)
				 {
					 //show menu selector on mouse
					 m_subMenuCursorPos[m_topMenuCursorPos] =i;
					 this->ShowMenuBox(0, 1);
					 chr =13;
					 goto rt;    //attempt to return this item

				 }
				 goto j_esc;  //close box and escape

			 } //end of release click if statement


			 //--------
			 //a down click occurred. case 257, mouse.ButtonState=1
			 //if its on a top menu item open that sub box
			 //if its on a subitem  move the selector to this item
			 //if its out of the box region escape
			 //if its on a bar in the box ignore goto to get
			  if(mouse.y ==0)
			  {
				  i = this->CheckTopMenu();
				  if(i ==-1)  //on top line, but out of top items range
				  {
					  if(boxon) //if the box is open, close it
					  {
						  this->CloseBox();
						  boxon =0;
					  }
						this->ShowTopMenu(0); //show top menu off
						goto get;
				  }
				  //user clicked down on another top menu item
				  if(boxon)
				  {
					  if(i != m_topMenuCursorPos) {this->CloseBox(); boxon=0;}
				  }
				  m_topMenuCursorPos=i; //open new box

				  keybrd1=0;
				  goto get1;

			  }  //end of if mouse on top line

			 //the mouse is not on the top line. a down click occurred somewhere
			 //if it the box is closed escape
			 //else if on a menu item show that sub item as selected
			 if(boxon==0)  goto j_esc;
			 this->CopyCoordinates();
			 i= this->WhichFieldIsMouseIn2(&m_boxmapper[0], &mouse, 16);
			 if(i !=-1)  //mouse was clicked down on a sub menu item
			 {
				 m_subMenuCursorPos[m_topMenuCursorPos] =i;
				 this->ShowMenuBox(0, 1); //show menu selector at mouse
				  goto get;
			 }
			 //although the mouse is not on a sub item
			 //see if the mouse is anywhere in the box
			 //if not escape
			 //else turn the selector off in the box
			 i= this->IsMouseInBox();
			 if(i==-1) goto j_esc; //close box and exit
			 this->ShowMenuBox(0,0); //turn the selector on in the box
			 goto get;


		//------------------------------
		//see if the user chose a quick letter, if so return the
		//c box # and the c pos within that box
		default:
			if(!quicklen)
				goto get;        //no available option: ignore user key
			else
			{
				k = -1;
				chr = toupper(chr);

				for(i = 0; i < quicklen; i++){
					tempchr = qletters[0][i];
					tempchr = toupper(tempchr);
					if(chr == tempchr)
					{
						k= qletters[1][i];
						break;
					}
				}  //end for

				if(k == -1 ) goto get;    //letter user pressed not quick letter

				if(boxon){this->CloseBox(); boxon=0;}
				return k;

			} //end else


	} //end switch

}
//----------------------------------------------------------------------
//Entry:
// chr = the char alreay got from the caller
//upon entry the left mouse button is currently being held down
//while a submenu box is currently opened
//allow 1) opening of another box by moving to that box while holding
//         down the button.
//      2)
int Menu::MouseMovement(int boxon1)
{
	int boxon, chr, i;
	int time;
	time =0;
	boxon = boxon1;

get:

	chr=this->WaitGetMouseOrKey();   //first time

	switch(chr)
	{


		case 256:     //mouse movement

			if(mouse.ButtonState ==0) goto h9; //button is released

			if(mouse.y > 0) goto h7;  //mouse not on top line
			i = this->CheckTopMenu();
			if(i ==-1)    //mouse on top line, but not on any top item
			{
				if(boxon ) {this->CloseBox(); this->ShowTopMenu(0);boxon=0;}
				goto get; //button is still held down
			}

			// the mouse is on a top menu item
			//if its box is displayed and if it is different from
			//the new item then close the box
			if((boxon==1) && (m_topMenuCursorPos != i))
			{
				this->CloseBox();
				boxon=0;
			}

			if(m_topMenuCursorPos !=i)
			{
a1:         if(boxon) this->CloseBox();
				m_topMenuCursorPos =i;
				this->ShowTopMenu(2); //show top menu activated
				this->ShowMenuBox(1, 0);
				boxon=1;
			}
			if((boxon==0) && m_topMenuCursorPos==i) goto a1;

			goto get;


			//--------------------
			case 257: //click occurred
			if(mouse.ButtonState==0)  //a release click
			{
				 if(mouse.y ==0)  //if mouse on top line
				 {
					 if(boxon)     //if box is showing
					 {  //show the selector on the 1st subitem
						 m_subMenuCursorPos[m_topMenuCursorPos]=0;
						 this->ShowMenuBox(0,1);
						 return 0;
					 }

					 return 27;
				 } //end if statement for mouse release click when on top line


				 if(boxon==0) return 27;  //escape cause box closed
				 //see if the click occurred on an item in the box
				 this->CopyCoordinates();
				 i= this->WhichFieldIsMouseIn2(&m_boxmapper[0], &mouse, 16);
				 if(i > -1)
				 {
					 //show menu selector on mouse
					 m_subMenuCursorPos[m_topMenuCursorPos] =i;
					 this->ShowMenuBox(0, 1);
					 return 13;  //attempt to return this item

				 }
					//close box and escape
				 return 27;
			 } //end of release click if statement


	 } //end of switch
	 goto get;

//-----------------
//mouse not on top line
// a) if no box displayed goto get
// b) see if mouse is on a menu item in the box
//    if not on menu item turn all sub menu items off & goto get
// c) if on menu item on . goto get
h7:
	if(boxon==0 ) goto get;
	this->CopyCoordinates();   //copy from bigmapper into m_boxmapper
	i = this->WhichFieldIsMouseIn2(&m_boxmapper[0], &mouse, 16);
	if(i == -1)
	{
		this->ShowMenuBox(0, 0);  //turn off sub menu selector
		goto get;
	}

	m_subMenuCursorPos[m_topMenuCursorPos] = i;

	this->ShowMenuBox(0,1);
	goto get;

//-------------------------

//-----------------

h9:
//if the button is released and no box is shown send a cancel signal to
//the caller
	if(boxon ==0)
	{
		return 27;
	}

	this->CopyCoordinates();   //copy from bigmapper into m_boxmapper
	i = this->WhichFieldIsMouseIn2(&m_boxmapper[0], &mouse, 16);
	if(i==-1)
	{
		if(mouse.y)
		{

			return 27;
		}

		//The mouse it on the top of the screen and release was pressed
		//set the menu selector to the first sub menu item in
		//that box
		m_subMenuCursorPos[m_topMenuCursorPos] = 0;
		this->ShowMenuBox(0,1);  //show menu selector (button was released}
		return 0;

	}

	else
	  m_subMenuCursorPos[m_topMenuCursorPos] = i;
	  return 13;

//------------


}
//------------------------------------------------------------------------
//show the mouse coordinates x1, y1 on the screen (for debugging only)
int Menu::ShowCoordinates()
{
	char linebuf1[81];
	int i;

	sprintf(linebuf1, 	"     %5d     %5d ", mouse.x, mouse.y);
	gotoxy(1,1);
	this->WriteLine(linebuf1, 0, 0, GREEN*16+BLACK);
	return 0;
}
//----------------------------------------------------------------------
int Menu::CopyCoordinates()
{
	int t;
	//clear  old coordinates
	for(t=0; t<16; t++){
	m_boxmapper[t].x1 =0;
	m_boxmapper[t].y1 =0;
	m_boxmapper[t].x2 =0;
	m_boxmapper[t].y2 =0;
	}

	for(t= 0; t< m_numOfSubOpts[m_topMenuCursorPos]; t++){
		m_boxmapper[t].x1 = m_bigmapper[m_topMenuCursorPos][t].x1;
		m_boxmapper[t].y1 = m_bigmapper[m_topMenuCursorPos][t].y1;
		m_boxmapper[t].x2 = m_bigmapper[m_topMenuCursorPos][t].x2;
		m_boxmapper[t].y2 = m_bigmapper[m_topMenuCursorPos][t].y2;

	}

return 0;
}

//----------------------------------------------------------------------
//see if the mouse is in the submenu box
int Menu::IsMouseInBox()
{
	int x1, y1, x2, y2, t;

	t = m_numOfSubOpts[m_topMenuCursorPos]; //get the r # of items in this box
	x1 = m_boxmapper[0].x1-2;
	y1 = m_boxmapper[0].y1-1;
	x2 = m_boxmapper[t-1].x2+2;
	y2 = m_boxmapper[t-1].y2+1;
	if(mouse.x < x1) return -1;
	if(mouse.x > x2) return -1;
	if(mouse.y < y1) return -1;
	if(mouse.y > y2) return -1;

	return 1;  //mouse in legal range
}
//---------------------------------------------------------------------

//-------------------------------------------------------------------
// (c) Michael Finnegan   18 June 1999
//-------------------------------------------------------------------

#include "search.h"
#include "clip.h"
#include "editor.h"

//-------------------------------------------------------------------
//these vars are initialised to 0 in main.cpp
//int mwwo, mc;            //match whole word only, match case
//char findwhat[181];       //text to find
//---------------------------------------------------------------------
//This function doesn't do any inputing of the input line.
//it just displays the contents of the input line in any one
//of 4 different cursor styles
//If the length of the lines is greater then 31
//then the last 31 chars of the line are shown
//the cursor, if it is to be visable is shown to the right of the last
//character.
//Entry:
// cursorstyle = 0 small cursor off, and black unselected
//               1 small cursor on, and black unselected
//               2 small cursor off, and black selected
//               3 small cursor on, and black selected
// ilen = the r # of the inputfield width

void ShowInputLine(Disk *a, char *buf, int cursorstyle, int x, int y, int ilen)
{
	int x1, y1, len, stpos, displen;
	int color[2], selectstyle;
	char dotbuf[2];

	color[1] = BLACK*16+LIGHTGRAY;    //black selected
	color[0] = LIGHTGRAY*16+BLACK;    //black unselected

	selectstyle = 0;                  //assume black unselected
	if(cursorstyle == 2 || cursorstyle == 3) selectstyle =1;


	len = strlen(buf);


	stpos =0;
	if(len> ilen-1) stpos = len-(ilen-1);       //the c pos of the first char to disp

	displen = len - stpos;  //the r len of the text to disp

	x1 = 0; y1 =0;
	if(len)
		a->WriteLine(&buf[stpos], x1+x, y1+y, ilen, color[0], 1);
	else
		a->WriteLine(".", x1+x, y1+y, ilen, color[0], 1);

	if(len)
		a->WriteLine(&buf[stpos], x1+x, y1+y, displen, color[selectstyle], 1);

	//since we didn't write to the last char position on the line
	//it may contain rubbish from a line length of 32 or greater
	//so show the last char as a normal dot. The cursor will
	//be positioned on this dot if the len was 32 or greater
	dotbuf[0] = '.';
	a->WriteLine(dotbuf, x1+x+ilen-1, y1+y, 1, color[0], 1);

	//turn the small cursor on or off (depending on cursorstyle)
	gotoxy(1+x1+x+displen, 1+y1+y);
	if(cursorstyle == 1 || cursorstyle == 3) a->CursorOn();
	else a->CursorOff();

}

//---------------------------------------------------------------------
//bring the black arrows position pointer to the "ok" button
//and show it as released
void BringBack(Disk *a)
{
	a->m_buttonpressed =0;
	a->m_buttonscursorpos =0;
	a->ShowButtons();
}
//---------------------------------------------------------------------
void FindHelp(Disk *a)
{

  static char *pchar[20] = {"Find",
														 " Finds text in a file.",
														 " ",
														 " . The search for a match begins from the current cursor ",
														 "   position until the end of the file is reached.",
														 " . To find text that is an entire word and not part of a longer ",
														 "   word, click Match Whole Word Only.",
														 " . To find text with the same combination of uppercase and",
														 "   lowercase letters as the find text, click Match Case.",
														 " . You can press ESC at any time to cancel the search.",
														 " "
														};
		  pchar[3][1] =4;
		  pchar[5][1] =4;
		  pchar[7][1] =4;
		  pchar[9][1] =4;
		  a->CursorOn();
		  a->ShowDialogBox(&pchar[0], 9, 1, 9, 0);

}
//-----------------------------------------------------------------------
void ReplaceHelp(Disk *a)
{

  static char *pchar[20] = {"Replace",
  "Finds specified text and replaces it with new text.",
  " ",
  " . To confirm each replacement, click the Replace button.",
  " . To replace all occurences of the text at once, click the",
  "   Replace All button",
  " . To find text that is an entire word and not part of a longer",
  "   word, click Match Whole Word Only",
  " . To find text with the same combination of uppercase and",
  "   lowercase letters as the find text, click Match Case.",
  " ",
  " The first occurrence of the text is selected, and you are",
  " prompted to either Replace this match, or to Skip this match",
  " without replacing it, or to cancel the search.",
  " You can also press ESC at any time to cancel the operation.",
  " ",
  " The search begins from the current position in the text."
														};
		  pchar[3][1] =4;
		  pchar[4][1] =4;
		  pchar[6][1] =4;
		  pchar[8][1] =4;

		  a->CursorOn();
		  a->ShowDialogBox(&pchar[0], 16, 1, 16, 0);

}
//-----------------------------------------------------------------------



//---------------------------------------------------------------------
//called by FindText(a);
//exit:
// The search string will be returned in the global var inputbuf
// ax =0 if its ok
// else ax =27 user canceled

int FindText1(Disk *a)
{
	int i, ii, iii, t, ch, lines, cursorfield;
	int cx, sneek;
	Disk b;
	static char *pchar[8] = {"Find",
								 "Find What: [...............................]",
								 " ",
								 "[ ] Match Whole Word Only",
								 "[ ] Match Case",
								 " ",
								 " ",
								 " "};


	FILE *out;
	int i_entrycode, icursorpos;
	i_entrycode = 1+4+128+256;              //exit upon: <tab>, <down key>
														 //or mouse click outside field
														 //F1
	icursorpos =0;
	sneek =0;
	TMousePos MousePos[6];

	MousePos[0].x1 = 29;
	MousePos[0].y1 =  9;
	MousePos[0].x2 = 59;
	MousePos[0].y2 =  9;

	MousePos[1].x1 = 18;
	MousePos[1].y1 = 11;
	MousePos[1].x2 = 18+22;
	MousePos[1].y2 = 11;

	MousePos[2].x1 = 18;
	MousePos[2].y1 = 12;
	MousePos[2].x2 = 18+11;
	MousePos[2].y2 = 12;

	MousePos[3].x1 = 17;
	MousePos[3].y1 = 16;
	MousePos[3].x2 = 24;
	MousePos[3].y2 = 16;

	MousePos[4].x1 = 35;
	MousePos[4].y1 = 16;
	MousePos[4].x2 = 44;
	MousePos[4].y2 = 16;

	MousePos[5].x1 = 54;
	MousePos[5].y1 = 16;
	MousePos[5].x2 = 61;
	MousePos[5].y2 = 16;

	lines = a->GetLines();

	if(strlen(findwhat) != 0)
	{
		strcpy(inputbuf, findwhat);
	}
	else
	{

		inputbuf[0] =0;
		//strcpy(inputbuf, "This is a very long line of text do you not think so?");
	}

	icursorpos =strlen(inputbuf);
	if(icursorpos <0) icursorpos =0;

	if(mwwo)
		pchar[3][1] = 'X';
	else
		pchar[3][1] = ' ';

	if(mc)
		pchar[4][1] = 'X';
	else
		pchar[4][1] = ' ';




	a->CursorOn();
	i = a->ShowDialogBox(&pchar[0], 5, 7, 5, 0, 1);


	cursorfield =0;
	ShowInputLine(a, inputbuf, 1, 29, 9, 31);

reenter:
	//depending on the value of cursorfield, goto that field
	switch(cursorfield)
	{
		 //enter input field
		 case 0:
			BringBack(a);    //"arrows on ok" button
			i = a->Input(&MousePos[0], 6, inputbuf, 29, 9, 31, 128, &icursorpos,
													LIGHTGRAY*16+BLACK, i_entrycode, 1, sneek);
			sneek =0;
			cx = wherex()-30;   //cx =0 if screen crsr at beginning of input
			switch(i)
			{
				case 4:  //[down] from input line
				  cursorfield =1;
				  goto  a1;   //next field

				case 2:    //[tab] from input
					cursorfield =1;
					goto a1;   //tab from input line

				case 0:
					goto ex; //[return] on inputline
				case 1:
					i =27;
					goto ex; //[esc] on inputline

				case 9: //f1
					goto o1;

				case 257: //a mouse click while in the input field
					ii = clickfield;
					//ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 6);
					goto n2;


				default:
					goto reenter;
			}

		 //enter field "match whole word only" field
		 case 1:
a1:      BringBack(a);    //"arrows on ok" button
			gotoxy(19, 12);
			goto get;

		 //enter field "match case"
		 case 2:
			BringBack(a);    //"arrows on ok" button
			gotoxy(19, 13);
			goto get;

		//enter field "ok" button
		case 3:
			BringBack(a);    //"arrows on ok" button
			gotoxy(18+3, 17);
			goto get;

		//enter field "cancel" button
		case 4:
			a->m_buttonpressed =0;
			a->m_buttonscursorpos =1;
			a->ShowButtons();
			gotoxy(35+3, 17);
			goto get;
		//enter field  "help button
		case 5:
			a->m_buttonpressed =0;
			a->m_buttonscursorpos =2;
			a->ShowButtons();
			gotoxy(54+3,17);
			goto get;

	}   //end of switch for entering a cursor field

get:
	a->CursorOn();
	ch = a->WaitGetMouseOrKey();
	if(ch == 210) goto o1;
	i = ch;
	//the user was in any of the 6 fields, he pressed a key or click
	//find out what action to take based on the field he was in and the
	//key he pressed
	switch(cursorfield)
	{
		//we were in input line field
		case 0:
			switch(ch)
			{
				case 13:     //pressed [return] on inputline
					goto ex;

				case 27:   //pressed [esc] on input line
					i = 27;
					goto ex;

				case 9:  //pressed [tab] on input line
					cursorfield =1;
					goto reenter;

				case 31:  //pressed [down] on input line
					cursorfield = 1;
					goto reenter;
				default:
					goto get;
			} //end of inner switch for input field

			//we were on the "Match whole word only" field
			case 1:
				switch(ch)
				{
					case 31: //[down] press on "match whole word only" field
						cursorfield =2;
						goto reenter;

					case 9:   //[tab] pressed on "match whole word only" field
						cursorfield =2;
						goto reenter;

					case 13:  //[return] pressed on "match whole word only" field
						i = 0;
						goto ex;

					case  27:  //[esc] pressed on "match whole word only" field
						goto ex;

					case 30:  //[up] pressed on "match whole word only" field
						cursorfield =0;
						goto reenter;

					case 32: //toggle the "X" in "Match Whole Word Only"
m1:					 if(mwwo)
						 {
							pchar[3][1] = ' ';
							mwwo =0;
						 }
						 else
						 {
							pchar[3][1] = 'X';
							mwwo =1;
						 }
						 a->WriteLine(&pchar[3][0], 17, 11, -1, LIGHTGRAY*16+BLACK);
						 goto reenter;

						case 257: //click occurred
							goto n1;
					default:
						goto reenter;
				}  //end of inner switch for "match whole word only" field


			//we were on the "Match case only" field
			case 2:
				switch(ch)
				{
					case 31: //[down] press on "match case only" field
						cursorfield =3;
						goto reenter;

					case 9:   //[tab] pressed on "match case only" field
						cursorfield =3;
						goto reenter;

					case 13:  //[return] pressed on "match case only" field
						i = 0;
						goto ex;

					case  27:  //[esc] pressed on "match case only" field
						goto ex;

					case 30:  //[up] pressed on "match case only" field
						cursorfield =1;
						goto reenter;


					case 32: //toggle the "X" in "Match Case Only"
m2:					 if(mc)
						 {
							pchar[4][1] = ' ';
							mc =0;
						 }
						 else
						 {
							pchar[4][1] = 'X';
							mc =1;
						 }
						 a->WriteLine(&pchar[4][0], 17, 12, -1, LIGHTGRAY*16+BLACK);
						 goto reenter;

					case 257: //click occurred
						goto n1;

					default:
						goto reenter;
				}  //end of inner switch for "match case only" field

			//we were on the "ok" button
			case 3:
				switch(ch)
				{

					case 9:   //[tab] pressed on "ok" button field
					case 32:  //[spc]
					case 28:  //[right]
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =1;
						a->ShowButtons();
						cursorfield =4;
						goto reenter;

					case 13:  //[return] pressed on "ok" button field
						i = 0;
						goto ex;

					case  27:  //[esc] pressed on "ok" button field
						goto ex;


					case 30:  //[up] pressed on "ok" button field
						cursorfield = 2;
						goto reenter;


				  case 257: //click occurred while on "ok" button
					  goto n1;

					default:
						goto reenter;
				}  //end of inner switch for "ok" button field


			//we were on the "cancel" button
			case 4:
				switch(ch)
				{

					case 9:   //[tab] pressed on "ok" button field
					case 32:  //[spc]
					case 28:  //[right]
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =2;
						a->ShowButtons();
						cursorfield =5;   //goto help button
						goto reenter;

					case 13:  //[return] pressed on "cancel" button field
						i = 27;
						goto ex;

					case  27:  //[esc] pressed on "cancel" button field
						goto ex;


					case 30:  //[up] pressed on "cancel" button field
						cursorfield = 2;  //go up to "match case only" field
						goto reenter;

					case 29: //[left] pressed on cancel button field
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =0;
						a->ShowButtons();
						cursorfield =3;
						goto reenter;

					case 257: //click occurred
						goto n1;

					default:
						goto reenter;
				}  //end of inner switch for "cancel" button field

			//we were on the "help button
			case 5:
				switch(ch)
				{

					case 29:  //[left]
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =1;
						a->ShowButtons();
						cursorfield =4;   //goto "cancel" button
						goto reenter;

					case 13:  //[return] pressed on "help" button field
o1:

						FindHelp(&b);  //open help dialog box for find text
						goto reenter;

					case  27:  //[esc] pressed on "help" button field
						goto ex;


					case 30:  //[up] pressed on "help" button field
						cursorfield = 2;  //go up to "match case only" field
						goto reenter;

					case 257: //click occurred
						goto n1;

					default:
						goto reenter;
				}  //end of inner switch for "help" button field



	case 257: //click
n1:	ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 6);

n2:	switch(ii) //a click occurred in a field
		{

			case 0: //click occurred on input line
				cursorfield =0;
				i = cursorpos;  //old current pos;
				icursorpos -= cx;
				icursorpos += (mouse.x -29);

				t = strlen(inputbuf);
				if(icursorpos >=t)
				{
					i = icursorpos;
					icursorpos = t;  //smaller value
					gotoxy(mouse.x+1 -(i-icursorpos), mouse.y+1);
				}
				else
					gotoxy(mouse.x+1, mouse.y+1);

				a->WaitMouseButtonRelease(-1);
				sneek =1+(wherex()-30);
				goto reenter;




			case 1: //click occurred in "March Whole Word Only" field
				BringBack(a);  //arrows on "ok" button
				if(mouse.ButtonState) //button pressed down
				{
					gotoxy(19,12);
					cursorfield =1;
					a->ColourLine(17, 11, 3, BLACK*16+LIGHTGRAY);
					a->WaitMouseButtonRelease(-1);
					goto m1;

				}
				goto get;

			case 2: //click occurred in "Match Case only" field
				BringBack(a);  //arrows on "ok" button
				if(mouse.ButtonState) //button pressed down
				{
					gotoxy(19, 13);
					cursorfield =2;
					a->ColourLine(17, 12, 3, BLACK*16+LIGHTGRAY);
					a->WaitMouseButtonRelease(-1);
					goto m2;
				 }
				 goto get;

		  case 3: //click occurred on "ok" button
			  a->CursorOn();
			  BringBack(a); //arrows on "ok" button
			  if(mouse.ButtonState) //button pressed down
			  {
				  gotoxy(18+3, 17); //cursor on "ok" button
				  cursorfield =3;
				  a->m_buttonpressed =1;
				  a->m_buttonscursorpos =0;
				  a->ShowButtons();
				  a->WaitMouseButtonRelease(-1);
				  a->m_buttonpressed =0;
				  a->ShowButtons();
				  gotoxy(18+3, 17);
				  iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 6);
				  if(iii !=3) //not relesed on same button
				  {
						//cursorfield =3;
						goto get;
				  }
				  i = 0;
				  goto ex;
			  }



		  case 4: //click occurred on "cancel" button
			  a->CursorOn();
			  //BringBack(a); //arrows on "ok" button
			  if(mouse.ButtonState) //button pressed down
			  {
				  gotoxy(35+3, 17); //cursor on "cancel" button
				  cursorfield =4;
				  a->m_buttonpressed =2;
				  a->m_buttonscursorpos =1;
				  a->ShowButtons();
				  a->WaitMouseButtonRelease(-1);
				  a->m_buttonpressed =0;
				  a->ShowButtons();
				  gotoxy(35+3, 17);
				  iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 6);
				  if(iii !=4) //not relesed on same button
				  {
						//cursorfield =4;
						goto get;
				  }
				  i = 27;
				  goto ex;
			  }


		  case 5: //click occurred on "help" button
			  a->CursorOn();
			  //BringBack(a); //arrows on "ok" button
			  if(mouse.ButtonState) //button pressed down
			  {
				  gotoxy(54+3, 17); //cursor on "help" button
				  cursorfield =5;
				  a->m_buttonpressed =3;
				  a->m_buttonscursorpos =2;
				  a->ShowButtons();
				  a->WaitMouseButtonRelease(-1);
				  a->m_buttonpressed =0;
				  a->ShowButtons();
				  gotoxy(54+3, 17);
				  iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 6);
				  if(iii !=5) //not relesed on same button
				  {
						//cursorfield =4;
						goto get;
				  }

				  goto o1;
			  }



			default:     //click occurred outside any field
				goto get;
		} //end of switch for mouse click

	default:
		goto get;

	}  //end of main switch for geting a key or click



ex:
	a->CloseBox();
ex1:
	return i;
}
//--------------------------------------------------------------------
//Get a char from the text
//Entry:
// *l, *c are the position where to get the char from
// if nxt =1 get the char from the next position
//                       (reloading only linebuf1 when necessary)
// if nxt =0 reload linebuf1 first
// Exit:
//  ax = 0 if no char got (if we are come to the end of the text)
//  else ax = the char got and
// *l, *c = the line and column where it came from
int GetAChar(Disk *a, int *c, int *l, int nxt)
{
	int cline, column, chr;

	column = *c;
	cline = *l;

	if(cline >lines) return 0;

	if(!nxt)
	{
		lenbuf1 = a->GetLineBuf(linebuf1, &cline);
		if(column> lenbuf1) return 0;
		return linebuf1[column-1];
	}

	column++;
	if(column > lenbuf1)
	{
		cline++;
		column =1;
		if(cline >lines)
		{
			return 0;
		}
		lenbuf1 = a->GetLineBuf(linebuf1, &cline);

	}

	*c = column;
	*l = cline;
	return linebuf1[column-1];
}
//---------------------------------------------------------------------
//This function gets a candidate string from the current cursor pos
//for comparsion with the match string.
//The candidate string is stored in buf.
//If the candidate string came from a lower line than the one given
//then
// block.stcol, block.stline points to the location of that line.
//This functions determines what constitutes a suitable candidate string
// based on the length of the string in findwhat.
//Entry:
// block.stcol, block.stline points to the position where the search is
// to begin from.

//Exit:
// ax =0 if no suitable candidate string was found
// else ax =1;
// and block.stcol, block.stline point to the start of the candidate
// and block.endcol, block.endline points to the end of the candidate
//
int GetMatchCandidate(Disk *a, char *buf)
{
	int prevchr, chr, matchlen, len, cline, column, nxt;
	int buflen;

	buflen =0;

	matchlen =strlen(findwhat);
	block.validsw =0;

top2:
	buflen =0;
	cline = block.stline;
	column = block.stcol;
	nxt =0;
	if(cline >lines) return 0;


top1:
	 chr = GetAChar(a, &column, &cline, nxt);
	 nxt++;
	 if(!chr) return 0;  //end of text

	 if(chr == 13)
	 {
		 block.stline = cline+1;
		 block.stcol =1;
		 goto top2;
	 }

	 buf[buflen] = chr;
	 buflen++;
	 if(buflen <matchlen) goto top1;
	 buf[buflen] =0;            //null terminate
	 block.endcol = column;
	 block.endline = cline;
	 block.validsw =2;  //code: find composed the block vars

	 return 1;
}
//--------------------------------------------------------------------
//Attempt to inc the block.stcol, block.stline vars
int IncPosition(Disk *a)
{
	//get current again
	GetAChar(a, &block.stcol, &block.stline, 0);
	return GetAChar(a, &block.stcol, &block.stline, 1); //try to get next
}
//--------------------------------------------------------------------
//This function scans the lines of text looking for a match
//Starting from the current cursor line and column
//it stops at the first match and returns the line and column
//Where it occurred, else it return 0
// Entry:
//  if nxtword =1 it skips the current match and looks for another.
//Exit:
// if ax =0 the returned line and column values are invalid
int ScanLines(Disk *a, int *c, int *l, int nxtwrd)
{
	int ch1, b1, nextword, i, flg, cline, column, len1, len2;
	char *p;
	char buf1[128];

	len2 = strlen(findwhat);
	flg =0;  //assume not found
	cline = *l;
	column = *c;
	lines = a->GetLines();
	if(!cline)  goto ex;
	if(cline > lines) goto ex;
	if(!column) column =1;
	block.stcol = column;
	block.stline = cline;
	nextword =nxtwrd;

top:
	i = GetMatchCandidate(a, linebuf2);
	if(!i) goto ex; //no more candidates to get: end of text reached
	if(!mwwo) goto comp;

	b1 = a->GetLineBuf(buf1, &block.stline);
	if(block.stcol ==1) goto t1; //look at end of match
	ch1 = buf1[block.stcol-2];   //get char before start of match
	if(isalnum(ch1)) goto nogood;

t1:  //lok at char after end of match
	 if(block.stline < block.endline)
	 {
		b1 = a->GetLineBuf(buf1, &block.endline);
	 }
	 ch1 = buf1[block.endcol];
	 if(isalnum(ch1)) goto nogood;

comp:
	flg = a->CompareStrings(linebuf2, findwhat, mwwo, mc);
	if(!flg)
	{
nogood:
		flg=0;
		i = IncPosition(a); //inc block.stcol, stline vars by 1 char if possible
		if(!i) goto ex;     //end of text

		goto top;
	}
	if(!nextword) goto ex;  //current march
	//a match has been found, but skip it and look for another
	nextword =0;
	block.stcol = block.endcol;
	block.stline = block.endline;
	i = IncPosition(a);
	if(i) goto top;

	flg =0; //throw away current match. Can't inc to try to fetch next match
ex:
	return flg;
}
//--------------------------------------------------------------------
//Highlight the word found. Its coordinates in the text
//are stored in block vars

int ShowWord(Disk *a)
{
	static t;
	char *p;
	int j, i, len, topline, botline;


	//printf("%s col=%2d  line=%5d", linebuf2, block.stcol, block.stline);
	//col = block.stcol;
	//cline = block.stline;
	//-----------------------
	//DispLines(a, topline, 22, 1, gbc*16+gfc);
	//SetCursor(a, 1);
	//DispStatusLine(a);

	topline = (cline-row)+1;   //original top line
	botline = topline+21;
	if(botline >lines) botline =lines;
b1:
	//is the 1st line of the text to be highlighted currently being displayed?
	if((block.stline >= topline) && (block.stline <= botline))
	{
		cline = block.stline;
		row = (cline - topline)+1;
		//DODO write code to do the following:
		//the 1st line of the text to be displayed is within range
		//check the second line.
		//if second line not scroll up and adjust topline +botline acordingly
		//then display the two lines of text.
a1:	if(block.endline> botline)
		{
			if(topline< block.stline)
			{

				topline++;
				botline++;
				row--;
				goto a1;
			}
		}
		  //colour the line or lines
		  //a->ColourLine(t+lm, row+1, len, LIGHTGRAY*16+BLACK);
		  //screen may need to be readjusted because topline was
		  //if increased.  In any case it needs to be normalised to
		  //cancle any previous highlighting
		  DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);

		  t = (block.endline - block.stline)+1; //r # of lines in blk
		  if(t > 22) t =22;
		  for(j = 0; j < t; j++){
			  i = block.stline +j;
			  lenbuf1 = a->GetLineBuf(linebuf1, &i);
			  if(i == block.stline)
			  {
				  if(block.stline == block.endline)
					  len = (block.endcol - block.stcol)+1;
					else
						len = (lenbuf1 - block.stcol)+1;
				  a->ColourLine(block.stcol+lm, row+1+j, len, LIGHTGRAY*16+BLACK);
			  }
			  else  //its not the first line. Its either a middle or the last
			  {
				  if(i < block.endline) //its a middle line: highlight it all
				  {
					  a->ColourLine(1+lm, row+1+j, lenbuf1, LIGHTGRAY*16+BLACK);
				  }
				  else //hightlight the last
				  {
					  a->ColourLine(1+lm, row+1+j, block.endcol, LIGHTGRAY*16+BLACK);
				  }
			  }
		  } //end for


		goto ex;
	}
	//The text to be highlighted is not currently displayed
	//move to text to display.
	//try to center the first line of to be highlighted
	cline = block.stline;
	if(cline > lines) cline = lines;
	row = 11;
	topline = (cline - row)+1;
	botline = topline+21;
	if(botline> lines) botline = lines;
	topline = botline -21;
	if(topline <1) topline = 1;
	row = (cline - topline) +1;
	goto b1;

ex:
	col = block.stcol;
	cline = block.stline;
	SetCursor(a, 1);
	DispStatusLine(a);

	return 0;
}
//--------------------------------------------------------------------
//open the find dialogue box to get text to find, then
//close the dialogue box and then scan the from  current cursor pos
//for the first occurance of the string.
//if not found tell user "Swift was unable to find a match."
//else display the fighlighted word found, exit.
int FindText(Disk *a)
{

	  static char *pchar[4] = {"Swift",
										 " ",
										 "  Swift was unable to find a match.  ",
										 " "};


	int j, i, l, c;
	char tempbuf[81];
	findfound =0;
	strcpy(tempbuf, "F1=Help  Enter=Execute  Esc=Cancel  Tab=Next Field");
	a->WriteLine(tempbuf, 1, 24, 78, LIGHTGRAY*16+BLACK);

	lenbuf1 = a->GetLineBuf(linebuf1, &cline);
	if(col >lenbuf1) col = lenbuf1;

	i = FindText1(a); //open dialog box to get fibdwhat string
	if(i == 27) goto ex;
	//find the text the user has entered.
	//it is now in the string findwhat
	strcpy(findwhat, inputbuf);
	l = cline;
	c = col;
	findfound = ScanLines(a, &c, &l, 0);  //accept first match
	if(!findfound)
	{
		a->CursorOn(); //message: "Swift was unable to find a match."
		a->ShowDialogBox(&pchar[0], 3, 1, 3, 0);
		CancelBlock(a);
	}
	else
	{
		ShowWord(a); //highlight the word found
	}
ex:
	return i;
}
//------------------------------------------------------------------------
//open the find dialogue box to get text to find, then
//close the dialogue box and then scan the from  current cursor pos
//for the first occurance of the string.
//if not found tell user "Swift was unable to find a match."
//else display the fighlighted word found, exit.
int RepeatFindText(Disk *a)
{

	static char *pchar[4] = {"Swift",
										 " ",
										 "    No more matches found.  ",
										 " "};


	int nxt, j, i, l, c;
	char tempbuf[81];

	if(strlen(findwhat)==0) return FindText(a);  //open the find dialogue box
	nxt =0;

	lenbuf1 = a->GetLineBuf(linebuf1, &cline);
	if(col >lenbuf1) col = lenbuf1;

	//the text to be found is in the findwhat string
	l = cline;
	c = col;
	if(block.validsw)
	{
		if((col == block.stcol)  && (cline == block.stline)) nxt =1;
	}

	j = ScanLines(a, &c, &l, nxt); //accept match after first match
	if(!j) goto q1;
	if(block.stline >lines) j =0;


q1:
	if(!j)
	{
a1:	a->CursorOn(); //message: "No more matches found."
		a->ShowDialogBox(&pchar[0], 3, 1, 3, 0);
		CancelBlock(a);
	}
	else
	{
		ShowWord(a); //highlight the word found using pars in block vars
	}

ex:
	return j;
}
//------------------------------------------------------------------------
//called by FindText(a);
//exit:
// The search string will be returned in the global var inputbuf
// ax =0 if its replace
// ax =1 if its replace all
// else ax =27 user canceled

int ReplaceText1(Disk *a)
{
	int i, ii, iii, t, ch, lines, cursorfield;
	int cx, cx2, sneek;
	Disk b, c;
	static char *pchar[9] = {"Replace",
								 "Find What:    [....................................]",
								 " ",
								 "Replace With: [....................................]",
								 " ",
								 "[ ] Match Whole Word Only",
								 "[ ] Match Case",
								 " "};


	FILE *out;
	int i_entrycode, i_entrycode2, icursorpos, icursorpos2;
	i_entrycode = 1+4+128+256;              //exit upon: <tab>, <down key>
														 //or mouse click outside field
														 //F1


	i_entrycode2 = 1+2+4+128+256;            //exit upon: <tab>, <up>
														 //<down key>
														 //or mouse click outside field
														 //F1

	icursorpos =0;
	sneek =0;
	TMousePos MousePos[8];

	MousePos[0].x1 = 28;   //find what
	MousePos[0].y1 =  8;
	MousePos[0].x2 = 63;
	MousePos[0].y2 =  8;

	MousePos[1].x1 = 28;  //Replace with
	MousePos[1].y1 = 10;
	MousePos[1].x2 = 63;
	MousePos[1].y2 = 10;

	MousePos[2].x1 = 14;     //Match whole word only
	MousePos[2].y1 = 12;
	MousePos[2].x2 = 37;
	MousePos[2].y2 = 12;

	MousePos[3].x1 = 14;     //Match case
	MousePos[3].y1 = 13;
	MousePos[3].x2 = 26;
	MousePos[3].y2 = 13;

	MousePos[4].x1 = 13;       //Replace
	MousePos[4].y1 = 16;
	MousePos[4].x2 = 23;
	MousePos[4].y2 = 16;

	MousePos[5].x1 = 27;       //Replace all
	MousePos[5].y1 = 16;
	MousePos[5].x2 = 41;
	MousePos[5].y2 = 16;

	MousePos[6].x1 = 45;      //Cancel
	MousePos[6].y1 = 16;
	MousePos[6].x2 = 54;
	MousePos[6].y2 = 16;

	MousePos[7].x1 = 58;      //Help
	MousePos[7].y1 = 16;
	MousePos[7].x2 = 65;
	MousePos[7].y2 = 16;



	lines = a->GetLines();

	if(strlen(findwhat) != 0) strcpy(inputbuf, findwhat);
	else inputbuf[0] =0;

	if(strlen(replacewith) != 0) strcpy(inputbuf2, replacewith);
	else inputbuf2[0] =0;

	icursorpos =strlen(inputbuf);
	if(icursorpos <0) icursorpos =0;

	icursorpos2 =strlen(inputbuf2);
	if(icursorpos2 <0) icursorpos2 =0;


	if(mwwo)
		pchar[5][1] = 'X';
	else
		pchar[5][1] = ' ';

	if(mc)
		pchar[6][1] = 'X';
	else
		pchar[6][1] = ' ';


	a->CursorOn();
	i = a->ShowDialogBox(&pchar[0], 6, 15, 6, 0, 1,1);


	cursorfield =0;
	a->WriteLine(inputbuf2, 28, 10, 36, LIGHTGRAY*16+BLACK, 1);
	ShowInputLine(a, inputbuf, 1, 28, 8, 36);
	cx = 0;
	cx2 = 0;
	sneek =0;
	a->PushUp();
	cursorfield =1;
	gotoxy(29, 11);
	goto a1;

reenter:
	//depending on the value of cursorfield, goto that field

	switch(cursorfield)
	{

		 //enter input field
		 case 0:
			a->CursorOn();
			BringBack(a);    //"arrows on ok" button
			i = a->Input(&MousePos[0], 8, inputbuf, 28, 8, 36, 128, &icursorpos,
													LIGHTGRAY*16+BLACK, i_entrycode, 1, sneek);
			sneek =0;
			cx = wherex()-30;   //cx =0 if screen crsr at beginning of input
			switch(i)
			{
				case 4:  //[down] from input line
				  cursorfield =1;
				  goto  a1;   //next field

				case 2:    //[tab] from input
					cursorfield =1;
					goto a1;   //tab from input line

				case 0:
					goto ex; //[return] on inputline
				case 1:
					i =27;
					goto ex; //[esc] on inputline

				case 9: //f1
				  goto o1;

				case 257: //a mouse click while in the input field
					ii = clickfield;
					//ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 6);
					goto n2;


				default:
					goto reenter;
			}

		 //enter field "replace with"
		 //enter input field
		 case 1:
a1:		BringBack(a);    //"arrows on ok" button
			a->CursorOn();
			i = c.Input(&MousePos[0], 8, inputbuf2, 28, 10, 36, 128, &icursorpos2,
													LIGHTGRAY*16+BLACK, i_entrycode2, 1, sneek);
			sneek =0;
			cx2 = wherex()-30;   //cx =0 if screen crsr at beginning of input
			switch(i)
			{
				case 3:      //up to 1st input line
					cursorfield =0;
					sneek = cx;
					gotoxy(30+cx, 9);
					goto reenter;

				case 4:  //[down] from input line
				  cursorfield =2;
				  goto  a11;   //next field

				case 2:    //[tab] from input
					cursorfield =2;
					goto a11;   //tab from input line

				case 0:
					goto ex; //[return] on inputline

				case 1:
					i =27;
					goto ex; //[esc] on inputline

				case 9: //f1
				  goto o1;

				case 257: //a mouse click while in the input field
					ii = clickfield;
					//ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 6);
					goto n2;


				default:
					goto reenter;
			}



		 case 2:  //Match whole word only
a11:      BringBack(a);    //"arrows on replace" button
			gotoxy(15, 13);
			goto get;

		 //enter field "Match case"
		 case 3:
			BringBack(a);    //"arrows on replace" button
			gotoxy(15, 14);
			goto get;

		//enter field "replace" button
		case 4:
			BringBack(a);    //"arrows on replace" button
			gotoxy(14+2, 17);
			goto get;

		//enter field "replace all" button
		case 5:
			a->m_buttonpressed =0;
			a->m_buttonscursorpos =1;
			a->ShowButtons();
			gotoxy(28+2, 17);
			goto get;
		//enter field  "cancel" button
		case 6:
			a->m_buttonpressed =0;
			a->m_buttonscursorpos =2;
			a->ShowButtons();
			gotoxy(46+2, 17);
			goto get;

		//enter field  "Help" button
		case 7:
			a->m_buttonpressed =0;
			a->m_buttonscursorpos =3;
			a->ShowButtons();
			gotoxy(59+2, 17);
			goto get;

	}   //end of switch for entering a cursor field

//------
get:
	a->CursorOn();
	ch = a->WaitGetMouseOrKey();
	if(ch == 256) goto get;
	if(ch == 210) goto o1;
	i = ch;
	//the user was in any of the 6 fields, he pressed a key or click
	//find out what action to take based on the field he was in and the
	//key he pressed
	switch(cursorfield)
	{
		//we were in input line field
		case 0:
			switch(ch)
			{
				case 13:     //pressed [return] on inputline
					goto ex;

				case 27:   //pressed [esc] on input line
					i = 27;
					goto ex;

				case 9:  //pressed [tab] on input line
					cursorfield =1;
					goto reenter;

				case 31:  //pressed [down] on input line
					cursorfield = 1;
					goto reenter;
				default:
					goto get;
			} //end of inner switch for input field

		case 1:
			switch(ch)
			{
				case 13:     //pressed [return] on 2nd inputline
					goto ex;

				case 27:   //pressed [esc] on 2nd input line
					i = 27;
					goto ex;

				case 9:  //pressed [tab] on 2nd input line
					cursorfield =2;
					goto reenter;

				case 31:  //pressed [down] on 2nd input line
					cursorfield = 2;
					goto reenter;
				default:
					goto get;
			} //end of inner switch for input field



			//we were on the "Match whole word only" field
			case 2:
				switch(ch)
				{
					case 31: //[down] press on "match whole word only" field
						cursorfield =3;
						goto reenter;

					case 9:   //[tab] pressed on "match whole word only" field
						cursorfield =3;
						goto reenter;

					case 13:  //[return] pressed on "match whole word only" field
						i = 0;
						goto ex;

					case  27:  //[esc] pressed on "match whole word only" field
						goto ex;

					case 30:  //[up] pressed on "match whole word only" field
						cursorfield =1;
						goto reenter;

					case 32: //toggle the "X" in "Match Whole Word Only"
m1:					 if(mwwo)
						 {
							pchar[5][1] = ' ';
							mwwo =0;
						 }
						 else
						 {
							pchar[5][1] = 'X';
							mwwo =1;
						 }
						 a->WriteLine(&pchar[5][0], 13, 12, -1, LIGHTGRAY*16+BLACK);
						 goto reenter;

						case 257: //click occurred
							goto n1;
					default:
						goto reenter;
				}  //end of inner switch for "match whole word only" field


			//we were on the "Match case only" field
			case 3:
				switch(ch)
				{
					case 31: //[down] press on "match case only" field
						cursorfield =4;
						goto reenter;

					case 9:   //[tab] pressed on "match case only" field
						cursorfield =4;
						goto reenter;

					case 13:  //[return] pressed on "match case only" field
						i = 0;
						goto ex;

					case  27:  //[esc] pressed on "match case only" field
						goto ex;

					case 30:  //[up] pressed on "match case only" field
						cursorfield =2;
						goto reenter;


					case 32: //toggle the "X" in "Match Case Only"
m2:					 if(mc)
						 {
							pchar[6][1] = ' ';
							mc =0;
						 }
						 else
						 {
							pchar[6][1] = 'X';
							mc =1;
						 }
						 a->WriteLine(&pchar[6][0], 13, 13, -1, LIGHTGRAY*16+BLACK);
						 cursorfield =3;
						 goto reenter;

					case 257: //click occurred
						goto n1;

					default:
						goto reenter;
				}  //end of inner switch for "match case only" field

			//we were on the "replace" button
			case 4:
				switch(ch)
				{

					case 9:   //[tab] pressed on "replace" button field
					case 32:  //[spc]
					case 28:  //[right]
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =2;
						a->ShowButtons();
						cursorfield =5;
						goto reenter;

					case 13:  //[return] pressed on "replace" button field
						i = 0;
						goto ex;

					case  27:  //[esc] pressed on "replace" button field
						goto ex;


					case 30:  //[up] pressed on "replace" button field
						cursorfield = 3;
						goto reenter;


				  case 257: //click occurred while on "replace" button
					  goto n1;

					default:
						goto reenter;
				}  //end of inner switch for "ok" button field


			//we were on the "replace all" button
			case 5:
				switch(ch)
				{

					case 9:   //[tab] pressed on "replace" button field
					case 32:  //[spc]
					case 28:  //[right]
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =3;
						a->ShowButtons();
						cursorfield =6;   //goto cancel button
						goto reenter;

					case 13:  //[return] pressed on "replace all" button field
						i = 1;
						goto ex;

					case  27:  //[esc] pressed on "cancel" button field
						goto ex;


					case 30:  //[up] pressed on "replace all" button field
						cursorfield = 3;  //go up to "match case only" field
						goto reenter;

					case 29: //[left] pressed on replace all button field
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =0;
						a->ShowButtons();
						cursorfield =4;
						goto reenter;

					case 257: //click occurred
						goto n1;

					default:
						goto reenter;
				}  //end of inner switch for "cancel" button field

			//we were on the "cancel" button
			case 6:
				switch(ch)
				{

					case 9:   //[tab] pressed on "cancel" button field
					case 32:  //[spc]
					case 28:  //[right]
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =3;
						a->ShowButtons();
						cursorfield =7;   //goto help button
						goto reenter;



					case 29:  //[left]
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =1;
						a->ShowButtons();
						cursorfield =5;   //goto "replace all" button
						goto reenter;

					case 13:  //[return] pressed on "Cancel" button field
					case  27:  //[esc] pressed on "cancel" button field
						i = 27;
						goto ex;


					case 30:  //[up] pressed on "cancel" button field
						cursorfield = 3;  //go up to "match case" field
						goto reenter;

					case 257: //click occurred
						goto n1;

					default:
						goto reenter;
				}  //end of inner switch for "help" button field


			//we were on the "help" button
			case 7:
				switch(ch)
				{

					case 29:  //[left]
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =2;
						a->ShowButtons();
						cursorfield =6;   //goto "cancel" button
						goto reenter;

					case 13:  //[return] pressed on "Cancel" button field
o1:						ReplaceHelp(&b);
						goto reenter;

					case  27:  //[esc] pressed on "help" button field
						i = 27;
						goto ex;


					case 30:  //[up] pressed on "cancel" button field
						cursorfield = 3;  //go up to "match case" field
						goto reenter;

					case 257: //click occurred
						goto n1;

					default:
						goto reenter;
				}  //end of inner switch for "help" button field





	case 257: //click
n1:	ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 8);

n2:	switch(ii) //a click occurred in a field
		{

			case 0: //click occurred on input line
				cursorfield =0;
				i = cursorpos;  //old current pos;
				icursorpos -= cx;
				icursorpos += (mouse.x -29);

				t = strlen(inputbuf);
				if(icursorpos >=t)
				{
					i = icursorpos;
					icursorpos = t;  //smaller value
					gotoxy(mouse.x+1 -(i-icursorpos), mouse.y+1);
				}
				else
					gotoxy(mouse.x+1, mouse.y+1);

				a->WaitMouseButtonRelease(-1);
				sneek =1+(wherex()-30);
				goto reenter;


			case 1: //click occurred on 2nd input line
				cursorfield =1;
				i = icursorpos2;  //old current pos;
				icursorpos2 -= cx2;
				icursorpos2 += (mouse.x -29);

				t = strlen(inputbuf2);
				if(icursorpos2 >=t)
				{
					i = icursorpos2;
					icursorpos2 = t;  //smaller value
					gotoxy(mouse.x+1 -(i-icursorpos2), mouse.y+1);
				}
				else
					gotoxy(mouse.x+1, mouse.y+1);

				a->WaitMouseButtonRelease(-1);
				sneek =1+(wherex()-30);
				goto reenter;





			case 2: //click occurred in "March Whole Word Only" field
				BringBack(a);  //arrows on "ok" button
				if(mouse.ButtonState) //button pressed down
				{
					gotoxy(15,13);
					cursorfield =2;
					a->ColourLine(14-1, 12, 3, BLACK*16+LIGHTGRAY);
					a->WaitMouseButtonRelease(-1);
					goto m1;

				}
				goto get;

			case 3: //click occurred in "Match Case only" field
				BringBack(a);  //arrows on "ok" button
				if(mouse.ButtonState) //button pressed down
				{
					gotoxy(15, 14);
					cursorfield =3;
					a->ColourLine(14-1, 13, 3, BLACK*16+LIGHTGRAY);
					a->WaitMouseButtonRelease(-1);
					goto m2;
				 }
				 goto get;

		  case 4: //click occurred on "replace" button
			  a->CursorOn();
			  BringBack(a); //arrows on "ok" button
			  if(mouse.ButtonState) //button pressed down
			  {
				  gotoxy(14+2, 17); //cursor on "replace" button
				  cursorfield =4;
				  a->m_buttonpressed =1;
				  a->m_buttonscursorpos =0;
				  a->ShowButtons();
				  a->WaitMouseButtonRelease(-1);
				  a->m_buttonpressed =0;
				  a->ShowButtons();
				  gotoxy(14+2, 17);
				  iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 8);
				  if(iii !=4) //not relesed on same button
				  {
						//cursorfield =3;
						goto get;
				  }
				  i = 0;
				  goto ex;
			  }



		  case 5: //click occurred on "replace all" button
			  a->CursorOn();

			  if(mouse.ButtonState) //button pressed down
			  {
				  gotoxy(28+2, 17); //cursor on "replace all" button
				  cursorfield =4;
				  a->m_buttonpressed =2;
				  a->m_buttonscursorpos =1;
				  a->ShowButtons();
				  a->WaitMouseButtonRelease(-1);
				  a->m_buttonpressed =0;
				  a->ShowButtons();
				  gotoxy(28+2, 17);
				  iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 8);
				  if(iii !=5) //not relesed on same button
				  {
						//cursorfield =4;
						goto get;
				  }
				  i = 1;  //Replace all
				  goto ex;
			  }


		  case 6: //click occurred on "cancel" button
			  a->CursorOn();
			  //BringBack(a); //arrows on "ok" button
			  if(mouse.ButtonState) //button pressed down
			  {
				  gotoxy(46+2, 17); //cursor on "cancel" button
				  cursorfield =6;
				  a->m_buttonpressed =3;
				  a->m_buttonscursorpos =2;
				  a->ShowButtons();
				  a->WaitMouseButtonRelease(-1);
				  a->m_buttonpressed =0;
				  a->ShowButtons();
				  gotoxy(46+2, 17);
				  iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 8);
				  if(iii !=6) //not relesed on same button
				  {
						//cursorfield =4;
						goto get;
				  }
				  i = 27;
				  goto ex;
			  }


		  case 7: //click occurred on "help" button
			  a->CursorOn();
			  //BringBack(a); //arrows on "ok" button
			  if(mouse.ButtonState) //button pressed down
			  {
				  gotoxy(59+2, 17); //cursor on "Help" button
				  cursorfield =7;
				  a->m_buttonpressed =4;
				  a->m_buttonscursorpos =3;
				  a->ShowButtons();
				  a->WaitMouseButtonRelease(-1);
				  a->m_buttonpressed =0;
				  a->ShowButtons();
				  gotoxy(59+2, 17);
				  iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 8);
				  if(iii !=7) //not relesed on same button
				  {

						goto get;
				  }

				  goto o1;
			  }




			default:     //click occurred outside any field
				goto get;
		} //end of switch for mouse click

	default:
		goto get;

	}  //end of main switch for geting a key or click


//------


ex:
	a->CloseBox();
ex1:
  if(i <=2)
  {
	 if(strlen(inputbuf2)>0)
	 {
		strcpy(replacewith, inputbuf2);
		return i;
	 }
  }

	return i;
}

//--------------------------------------------------------------------
//-----------------------------------------------------------------------
//Replace the text at block.stline, block.stcol with the text in
//replacewith, but if replacewith is empty just delete
//then Parawrap the paragraph.
//If possible set block vars to the start of the replaced text
//otherwise set them to char behind or in front of original deleted text
//set current current pos to the beginning of the replaced text,
//or if no replaced text to the char after the deleted text, if no
//char after the deleted text then to the char prior to the start
//of the match text
//Entry:
// block.stline, block.stcol points to the start of the match text

int DoReplace(Disk *a)
{
	long unsigned int ubytes;
	int i, matchlen, l, myl, c, rw, d;
	matchlen = strlen(findwhat);
	ubytes = 1+ CalcPrevBytes(a); //r # of bytes before the match text
	CutToClip(a, 1);
	GetLineColumn(a, &l, &c, ubytes);
	lenbuf1 = a->GetLineBuf(linebuf1, &l);
	rw = strlen(replacewith);
	myl =l;
	if(rw)
	{
		a->InsertString(linebuf1, replacewith, c-1);
		i = PerformWrap();   //wrap linebuf1, putting left over bit
									//into linebuf2
		d =1;
		a->OverWriteLine(linebuf1, &myl);
		if(i)
		{
			myl++;
			a->InsertLine(linebuf2, &myl);
			lines = a->GetLines();
			myl++;
			ParaWrap(a, &myl, &i);
		}

out:
		ParaWrap(a, &l, &c);

	}



	GetLineColumn(a, &l, &c, ubytes);
	block.stcol = c;
	block.stline = l;

	GetLineColumn(a, &l, &c, (ubytes+rw));
	block.endcol = c;
	block.endline = l;
	col = block.stcol;
	cline = block.stline;

	return 0;
}
//--------------------------------------------------------------------
//Scan for all occurrences of the word in findwhat and replace them
//with the text in replacewith, starting from the current cursor position
//Exit:
// ax = r num of occurrences found
// or if nor found ax =0
int ReplaceAll(Disk *a)
{
	int topline, fcount, l, c;



	fcount =0;
top:
	l = cline;
	c = col;
	findfound = ScanLines(a, &c, &l, 0);  //accept first match
	if(!findfound)
	{
		CancelBlock(a);
		goto ex;
	}

	DoReplace(a);
	fcount++;
	goto top;


ex:
  lines = a->GetLines();
  if(fcount)
  {
	  topline = lines - 21;
	  if(topline <1) topline =1;
	  cline = lines;
	  row = (cline - topline)+1;
	  DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);

	  SetCursor(a, 1);
	  DispStatusLine(a);
	  col =1;
  }

	return fcount;
}
//---------------------------------------------------------------------
//Display a message saying "Replace this occurence?"
//and allow the user to click on one of three buttons
//"Replace" "Skip" or "Cancel"
// Exit:
// ax = 0 user selected "Replace"
//      1 user selected "Skip"
//      2 user selected "Cancel"

int ReplaceThisOccurence(Disk *a, int pos)
{
	int ch, i, iii, stpos, t, lenmsg, lenbuf, width, x1, y1, x2, y2;
	Disk d;
	char linebuf1[81], msgbuf[81];
	int po;
	if(pos) po = 13;
	else po =0;

	x1 = 17;
	y1 =4;
	x2 = 61;
	y2 = 9;
	width = (x2 - x1 )+1;
	d.OpenShadowBox(x1, y1+po, x2, y2+po, LIGHTGRAY*16+BLACK, 0);
	memset(linebuf1, 32, width);
	linebuf1[width] =0;

	strcpy(msgbuf, "Replace");
	lenbuf = strlen(linebuf1);
	lenmsg = strlen(msgbuf);
	stpos = (lenbuf /2) - (lenmsg /2); //pos to entr titl text into linebuf
	for(t=0; t<lenmsg; t++){
		linebuf1[stpos+t] = msgbuf[t];
	}
	d.WriteLine(linebuf1, x1, y1+po, -1, WHITE*16+BLACK);
	strcpy(&d.m_buttonstxt[0][0], "  Replace  ");
	strcpy(&d.m_buttonstxt[1][0], "  Skip  ");
	strcpy(&d.m_buttonstxt[2][0], "  Cancel  ");

	d.m_buttonsposx1[0] = 20;
	d.m_buttonsposy1[0] = 8+po;

	d.m_buttonsposx1[1] = 36;
	d.m_buttonsposy1[1] = 8+po;

	d.m_buttonsposx1[2] = 49;
	d.m_buttonsposy1[2] = 8+po;
	d.m_buttonsnum =3;
	d.m_buttonpressed =0;
	d.m_buttonscursorpos =0;
	d.ShowButtons();
	strcpy(linebuf1, "Replace this occurence?");
	d.WriteLine(linebuf1, x1+11, y1+2+po,
			 -1, LIGHTGRAY*16 +BLACK);


	TMousePos MousePos[3];

	MousePos[0].x1 = 20;   //replace
	MousePos[0].y1 =  8+po;
	MousePos[0].x2 = 30;
	MousePos[0].y2 =  8+po;

	MousePos[1].x1 = 36;  //skip
	MousePos[1].y1 = 8+po;
	MousePos[1].x2 = 43;
	MousePos[1].y2 = 8+po;

	MousePos[2].x1 = 49;     //cancel
	MousePos[2].y1 = 8+po;
	MousePos[2].x2 = 58;
	MousePos[2].y2 = 8+po;


	d.CursorOn();
	gotoxy(20+3, 9+po); //cursor on "replace" button

get:
	ch = d.WaitGetMouseOrKey();
	switch(ch)
	{
		case 27:
			ch =2;
			goto ex;


		case 28: //right
			if(d.m_buttonscursorpos ==2) goto get;
			d.m_buttonscursorpos++;
			d.m_buttonpressed =0;
			d.ShowButtons();
			if(d.m_buttonscursorpos ==1)
			{
				gotoxy(36+3, 9+po);
			}
			else
			  gotoxy(49+3, 9+po);
		  goto get;


		case 29: //left
			if(d.m_buttonscursorpos ==0) goto get;
			d.m_buttonscursorpos--;
			d.m_buttonpressed =0;
			d.ShowButtons();
			if(d.m_buttonscursorpos ==1)
			{
				gotoxy(36+3, 9+po);
			}
			else
			  gotoxy(20+3, 9+po);
		  goto get;



		case 13:
		  ch = d.m_buttonscursorpos;
		  goto ex;


		case 257:  //click
		i = d.WhichFieldIsMouseIn2(&MousePos[0], &mouse, 3);
		if(i ==-1) goto get;

		switch(i)
		{
			case 0:  //click occurred on replace button
			  d.CursorOn();

			  if(mouse.ButtonState) //button pressed down
			  {
				  gotoxy(20+3, 9+po); //cursor on "replace" button

				  d.m_buttonpressed =1;
				  d.m_buttonscursorpos =0;
				  d.ShowButtons();
				  d.WaitMouseButtonRelease(-1);
				  d.m_buttonpressed =0;
				  d.ShowButtons();
				  gotoxy(20+3, 9+po);
				  iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 3);
				  if(iii !=0) //not relesed on same button
				  {

						goto get;
				  }
				  ch = 0; //user selected "replace"
				  goto ex;

			}
			goto get;



			case 1:  //click occurred on skip button
			  d.CursorOn();

			  if(mouse.ButtonState) //button pressed down
			  {
				  gotoxy(36+3, 9+po); //cursor on "skip" button

				  d.m_buttonpressed =2;
				  d.m_buttonscursorpos =1;
				  d.ShowButtons();
				  d.WaitMouseButtonRelease(-1);
				  d.m_buttonpressed =0;
				  d.ShowButtons();
				  gotoxy(36+3, 9+po);
				  iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 3);
				  if(iii !=1) //not relesed on same button
				  {

						goto get;
				  }
				  ch = 1; //user selected "skip"
				  goto ex;

			}
			goto get;


			case 2:  //click occurred on skip button
			  d.CursorOn();

			  if(mouse.ButtonState) //button pressed down
			  {
				  gotoxy(49+3, 9+po); //cursor on "skip" button

				  d.m_buttonpressed =3;
				  d.m_buttonscursorpos =2;
				  d.ShowButtons();
				  d.WaitMouseButtonRelease(-1);
				  d.m_buttonpressed =0;
				  d.ShowButtons();
				  gotoxy(49+3, 9+po);
				  iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 3);
				  if(iii !=2) //not relesed on same button
				  {

						goto get;
				  }
				  ch = 2; //user selected "cancel"
				  goto ex;

			}
			goto get;


		}


		default:
			goto get;
	}



ex:
	d.CloseBox();
	return ch;
}


//-----------------------------------------------------------------------
//open the find dialogue box to get text to find, then
//close the dialogue box and then scan the from  current cursor pos
//for the first occurance of the string.
//if not found tell user "Swift was unable to find a match."
//else display the fighlighted word found, exit.
int ReplaceText(Disk *a)
{

	  static char *pchar[4] = {"Swift",
										 " ",
										 "  Swift was unable to find a match.  ",
										 " "};


	int tt, tmp, ii, j, topline, fcount, i, l, c, next, cl, co;
	char tempbuf[81];
	next =0;
	fcount =0;
top:
	findfound =0;
	strcpy(tempbuf, "F1=Help  Enter=Execute  Esc=Cancel  Tab=Next Field");
	a->WriteLine(tempbuf, 1, 24, 78, LIGHTGRAY*16+BLACK);

	lenbuf1 = a->GetLineBuf(linebuf1, &cline);
	if(col >lenbuf1) col = lenbuf1;

	i = ReplaceText1(a); //open dialog box to get fibdwhat string
	if(i == 27) goto ex;
	//find the text the user has entered.
	//it is now in the string findwhat
	strcpy(findwhat, inputbuf);
	strcpy(replacewith, inputbuf2);
	//if the user wants to replace all the matching words, do so without
	//prompting
	if(i ==1)
	{
		i = ReplaceAll(a);
		if(i==0) goto top;
		goto ex;
	}

	block.endcol = col;
	block.endline = cline;

again:
	l = block.endline;
	c = block.endcol;
	findfound = ScanLines(a, &c, &l, next);  //accept first match
	if(!findfound)
	{
		a->CursorOn(); //message: "Swift was unable to find a match."
		if(!fcount) a->ShowDialogBox(&pchar[0], 3, 1, 3, 0);
		CancelBlock(a);
	}
	else
	{
		fcount++;
		ShowWord(a); //highlight the word found
		tt =1;       //dialogue box on bot
		if(row > 12) tt =0; //dialogue box on top
		ii = ReplaceThisOccurence(a, tt);
		if(ii ==1)  //skip this occurrence
		{
			next =1;
tr:		topline = (block.stline - row)+1;
			if(topline < 1) topline =1;
			DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);  //normalize text screen
			next =0;
			goto again;
		}
		if(ii ==2) goto ex; //cancel


		tmp = cline;
		DoReplace(a);
		row = row + (cline-tmp);
		if(!row)
		{
			row=1;
		}
		next =0;
		goto tr;


	}
ex:
	topline = (cline - row)+1;
	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);  //normalize text screen
	CancelBlock(a);
	return i;
}
//------------------------------------------------------------------------

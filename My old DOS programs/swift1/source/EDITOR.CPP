//-------------------------------------------------------------------
// (c) Michael Finnegan   26 May 1999
// 

#include "editor.h"


char tempchar;

//-------------------------------------------------------------------
//display text line
//taking into account block vars
//Entry:
//	 r = 1 to 22
//  line = 1 to last line
int DisplayLine(Disk *a, int line, int r)
{
	int xl, xc, cline, ro, len, stcol;
	//TPaste block2;
	len =0; //len for colouring line.  Assume no coloured line
	stcol =1;
	cline = line;

	lenbuf1 = a->GetLineBuf(linebuf1, &cline);
	a->WriteLine(linebuf1, 1+lm, (r-1)+2, maxlen, gbc[2]*16+gfc[2]);



	return 0;
}
//-------------------------------------------------------------------
/* This routine looks at the key pressed and returns a number
	corrospending to the function to call
	Entry:
	 chr = keypressed
	Exit:
	 ax = -1 if no function for this key
	 else ax = c # of function to call
*/
int TranslateKey(int chr)
{
	static int tkey[20] = {30, 31,29, 28, 203, 202, 200, 201,
				9, 13, 8, 27, 127, 24, 3, 16, 5};
	int t, flg;

	if(chr >= 32 && chr <= 126)
	  return 0;
	if(chr ==156) return 0;
	flg = -1;
	for(t = 0; t<17; t++) {
	  if(chr == tkey[t]){
		 flg = t+1;
		 break;
		}
	}
	return flg;
}
//---------------------------------------------------------------------
//using the golbal vars row and col set the cursor pos
// sw = o turn cursor off, else turn it on
// row = 1 to 22, col = 1 to 78
int SetCursor(Disk *a, int sw)
{
	gotoxy(col+1+lm, row+2);
	if(!sw) a->CursorOff();
	else
		a->CursorOn();

	return 0;
}
//---------------------------------------------------------------------
//display the file title centred on the top line
void DispTitle(Disk *a)
{
	int len, t;
	char *p, buf[80];

	block.validsw =0; //no paste coordinates yet

	for(t = 0; t < 78; t++){
		buf[t] = 196;
	}

	buf[79] =0;
	a->WriteLine(buf, 1, 1, 78, gbc[0]*16+gfc[0]);

	len = strlen(editortitle);
	if(len == 0)
	{
		strcpy(editortitle, "UNTITLED");
		len = strlen(editortitle);
	}
	t =  0;
	if(len > 74) t = len - 74;
	p = &editortitle[t];
	strcpy(&filebuf[1], p);
	filebuf[0] = 32;
	len = strlen(filebuf);
	filebuf[len] =32;
	len++;
	filebuf[len] =0;
	t = 40 - (len/2);
	a->WriteLine(filebuf, t, 1, -1, gbc[3]*16+gfc[3]);


}
//---------------------------------------------------------------------

void DispScrollBar(Disk *a)
{
	float f_topline, f_cursorpos, f_cursorlen, x, f_lines, f_cline;
	int topline, tt;
	char a1, a2, a3;

	a1 = gbc[0]*16+gfc[0];
	a2 = gbc[0]*16 +gfc[0];
	a3 = gbc[4]*16+gfc[4];


	f_cline = cline;

	lines = a->GetLines();
	f_lines = lines;
	topline = cline - (row-1);
	f_topline = topline;

	if(cline == 40)
	{
		tt =1;     //for debugging only
	}

	if(lines <= 22)
	{
		f_cursorlen = 20;
	}
	else
	{
		f_cursorlen = (float) 20 * ((1 / f_lines) *22);
		if( f_cursorlen < 1) f_cursorlen =1;
	}


	if(lines <= 22)
	{
		f_cursorpos =1;
	}
	else
	{
		f_cursorpos = ((1/f_lines) * f_topline) *20;
	}
	cursorpos = f_cursorpos;

	cursorlen = f_cursorlen;
	if((cursorpos+ cursorlen) > 20)
		cursorpos--;

	if((topline + 21) >= lines)
	{
		cursorpos = 20 - cursorlen;
	}
	a->ShowVerticalScrollBar(79, 2, 20, cursorlen, cursorpos, a1, a2, a3);

}
//---------------------------------------------------------------------
//display the status line

void DispStatusLine(Disk *a)
{

	int len;
	char numbuf[40];


	strcpy(statusline,
											" F1=Help                                             ");
	len = strlen(statusline);
	statusline[len] = 179;
	statusline[len+1] =0;

	sprintf(numbuf, "  Line:%d    Col:%d       X", cline, col);

	strcat(statusline, numbuf);
	statusline[78] =0;
	a->ShowTopMenu(0);             //show top menu off
	a->WriteLine(statusline, 0, 24, 80, gbc[3]*16+gfc[3]);
	DispScrollBar(a);

}
//---------------------------------------------------------------------
//display numlines lines from r line #. Starting on row (1 to 24)
//if that number of lines are not in the text blanks will be used
//Exit:
// ax =1 if job done, else ax =0
int DispLines(Disk *a, int topln, int numlines, int row, int attrib)
{
	int j, t, botln, crow, xrow, blanklines, cline;

	blanklines = 0;
	if((row < 1) || (row > 22)) return 0;
	xrow = 22 - (row-1); //r # of lines to print
	lines = a->GetLines();
	if((topln < 1) ||  (topln >lines)) return 0;
	if((numlines <1) || (numlines > xrow)) return 0;
	botln = topln+ (numlines-1);
	t = numlines; // r # of text lines to print
	if(botln > lines)
	{
		blanklines = botln - lines; //r # of blank lines to print
		botln = lines;
		t = (botln - topln)+1;    //r # of text lines to print
	}
	for(j = 0; j <t; j++){
		cline= topln+j;
		DisplayLine(a, cline, row+j);
  }
  if(blanklines)
  {
		xrow =  (row + t); //r # of the starting row of the 1st blnk line
		strcpy(linebuf1, " ");
		for(t = 0; t < blanklines; t++){
			a->WriteLine(linebuf1, 1+lm, (xrow-1)+t+2, maxlen, attrib);
		}
  }


	return 1;
}
//-------------------------------------------------------------------
//if the block is valid turn it off and redisplay the lines as normal
int CancelBlock(Disk *a)
{
	int topline;

	block.validsw =0;

	topline = (cline - row)+1;
	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);

	SetCursor(a, 1);
	DispStatusLine(a);

	return 0;
}
//--------------------------------------------------------------------
//-------------------------------------------------------------------
//Search a string from c position pos, scanning backwards until
//the beginning of the string.
//If found return the r pos of it, else return 0
//
//Remember pos = the c position from the beginning of buf where you want
//to start the bakwards search
//if a '1' is found the code in tempchar is substituted in the comparsion.
//exit:
//  ax =0 chr not in buf
//  else ax = the r pos from the beginning of buf where ch was found

int RightFindSpc(char *buf, int pos)
{
	int len, found;
	char *p, chr;

	len = strlen(buf);
	if(!len) return 0;
	if(pos > len) return 0;  //past end of string

	found = 0;
	p = &buf[pos];
	do{
		chr = *p;
		if(chr == 1) chr = tempchar;
		if(chr == 32) {found = 1; break;}
		p--;
	}while(p >= buf);

	if(!found) return 0;
	else
		return 1+(p - buf);
}
//----------------------------------------------------------------------

//-------------------------------------------------------------------
//Find the beginning of the paragraph by searching backwards for a
// return on an above line or the start of the text
//Entry:
//	curline = the r # of the line to begin searching backwards from
//	column = the r # of the column of the line to search backwards from
// Exit:
//	 ax = the line the paragraph begins on
int FindBegPara(Disk *a, int curline)
{
	char linebuf1[160];
	int len, cline;

	lines = a-> GetLines();
	cline = curline;
	if( cline > lines) return -1; //error
	if(curline ==1) return 1;

top:
	cline--;
	a->GetLineBuf(linebuf1, &cline);
	len = strlen(linebuf1);
	if(linebuf1[len-1] == 13) return cline+1;

	if( cline > 1) goto top;
	return 1;

}
//--------------------------------------------------------------------
//word wrap the line in linebuf1, putting any leftover bit in linebuf2
//if you find a 1 code, substitue it for tempchar while comparing
//Exit:
//  flg =1 if word wrap occurred
//  the results are in linebuf1, and linebuf2
// if no left over bit lenbuf2 = 0

int PerformWrap()
{
	int i, flg;
	char ch;

	lenbuf2 =0;
	linebuf2[0] =0;
	lenbuf1 = strlen(linebuf1);
	if(lenbuf1 <maxlen) return 0; //no wrap performed
	if(lenbuf1 > maxlen) goto a1;    //definately must perform wrap

	//the line is exactly 78 chars, have a look at the last char
	ch = linebuf1[maxlen-1];
	if(ch == 1) ch = tempchar;
	if(ch == 13) return 0;    //no wrap performed
	if(ch == 32) return 0;    //no wrap performed

a1:
	//scan backwards looking for a space
	i = RightFindSpc(linebuf1, maxlen-1);   //scan backwards from char at r pos 78
	if(!i) i =maxlen-1;

	strcpy(linebuf2, &linebuf1[i]);
	lenbuf2 = strlen(linebuf2);
	linebuf1[i] =0;
	lenbuf1 = i;

	if(lenbuf2) flg =1;
	else flg =0;

	return flg;
}

//--------------------------------------------------------------------2
//Attempt to wrap all the lines of the paragraph, beginning with
//line # line
//Exit:
//	ax =1 wrap done
// ax =0 no wrap
int ParaWrap1(Disk *a, int line)
{
	int i, len, cline, t, flg, numlines;
	char endchar;

	numlines = 1;
	flg =0;
	lenbuf2 =0;
	linebuf2[0] =0;
	cline = line;
	lines = a->GetLines();
	lenbuf1 = a->GetLineBuf(linebuf1, &cline);
	if(cline == lines) goto a20;
	endchar = linebuf1[lenbuf1-1];
	if(endchar == 1) endchar = tempchar;
	if(endchar != 13) goto a10; //no rtn at end of linebuf1
	if(lenbuf1 <= maxlen) return 0; //no wrap occurred

	//linebuf1 is > 78 and there is a rtn at the end of it.
	i = PerformWrap();
	if(i) flg = 1;        //set flg if wrap occurred, otherwise no change
	a->OverWriteLine(linebuf1, &cline);
	cline++;
	a->InsertLine(linebuf1, &cline);
	lines = a->GetLines();
	return 1;

//cline < lines, and no rtn at end of linebuf1
a10:
	a->DeleteLines(&cline, &numlines);
a11: a->GetLineBuf(&linebuf1[lenbuf1], &cline); //combine line with linebuf1
	lenbuf1 = strlen(linebuf1);
	i = PerformWrap();
	if(i) flg = 1;
	a->OverWriteLine(linebuf1, &cline);
	if(lenbuf2) goto a16;             //there is a bit left over
	return 1;

a16:
	strcpy(linebuf1, linebuf2);     //copy left over bit into linebuf1
	lenbuf1 = lenbuf2;
	lenbuf2=0;
	cline++;
	lines = a->GetLines();
a17:if( cline > lines) goto a25;
	//we're not yet on last text line
	i = PerformWrap();
	if(i) flg =1;
	endchar = linebuf1[lenbuf1-1];
	if(endchar == 1) endchar = tempchar;
	if(endchar == 13)
	{
		a->InsertLine(linebuf1, &cline);
		return 1;
	}
	if(lenbuf2)
	{
		a->InsertLine(linebuf1, &cline);
		lines = a->GetLines();
		cline++;
		strcpy(linebuf1, linebuf2);
		lenbuf1 = lenbuf2;
		lenbuf2 =0;
		goto a17;
	}
	goto a11; //no left over bit

//cline = lines. Only 1 line in paragraph and its the last text line
a20:
	i = PerformWrap();
	if(i) flg =1;
	if(lenbuf2 ==0) return 0;
	a->OverWriteLine(linebuf1, &cline);
a23: cline++;
	strcpy(linebuf1, linebuf2);
	lenbuf1 = lenbuf2;
	lenbuf2 =0;
a25: i = PerformWrap();
	if(i) flg =1;
	lines = a->GetLines();
	if(cline > lines)
		a->AppendLine(linebuf1);
	if(lenbuf2 ==0) return flg;
	goto a23;
}
//--------------------------------------------------------------------
//Search the paragraph starting from line stline
//looking for the marker char code 1
// Exit:
//  ax = -1 if not found (error occurred)
//  ax = 1 = found
//  ln and colu are set to the position in the line where found
//  and the code is replaced with the original in tempchar

int FindMarker(Disk *a, int stline, int *ln, int *colu)
{
	int done, line;
	char ch;

	ch =1;
	done =0;

	line = stline-1;
	while(!done)
	{
		line++;
		a->GetLineBuf(linebuf1, &line);
		done = a->LeftFindChar(linebuf1, ch, 0);
	}

	*colu = done; //colu = r pos where found
	lines = a->GetLines();
	if(line > lines) return -1; //error
	linebuf1[done-1] = tempchar;
	a->OverWriteLine(linebuf1, &line);

	*ln = line;
	return 1;
}

//--------------------------------------------------------------------
//Before calling this function
//if you need to, enter the char into the line and overwrite
//the line with the new version before calling this function
//Entry:
//	line, col are the point in the paragraph that you want to track
// Exit:
//		line, col point to the point in the paragraph you were tracking
//		these vars are updated if an actual wrap occurred
//	ax = 0 no wrap occurred
//	ax = 1 wrap occurred
// ax =-1 an error occurred
int ParaWrap(Disk *a, int *line, int *column)
{
	int len, i, j, crntln, colm, xline, xcolumn;

	xline = *line;
	xcolumn = *column;

	crntln = xline;
	colm =  xcolumn;
	lines = a->GetLines();
	if(crntln <1) return -1; //error
	a->GetLineBuf(linebuf1, &crntln);
	len = strlen(linebuf1);
	if(colm > len) return -1;
	tempchar = linebuf1[colm-1];
	linebuf1[colm-1] =1;                //place cursor marker in text
	a->OverWriteLine(linebuf1, &crntln);

	i = FindBegPara(a, crntln);
	if(i == -1) return -1;      //error occurred
	j = ParaWrap1(a, i);     //wrap the paragraph
	if(!j)    //no wrap occurred
	{
		a->GetLineBuf(linebuf1, &xline);
		linebuf1[colm-1] = tempchar;
		a->OverWriteLine(linebuf1, &xline);
		return 0;           //no wrap
	}

	//since a wrap occurred find the line and col of the marker
	//and replace it with the original char
	//then set line, column vars to that point
	//i = the line to start the search from
	i = FindMarker(a, i, &xline, &xcolumn);
	*line = xline;
	*column = xcolumn;
	return 1; //wrap ocurred
}
//-------------------------------------------------------------------
/*
int RightFindSpc(char *buf, int pos);
int FindBegPara(Disk *a, int curline);
int PerformWrap();
int ParaWrap1(Disk *a, line);
int FindMarker(Disk *a, int stline, int *ln, int *colu);
int ParaWrap(Disk *a, int *line, int *column);
*/
//-------------------------------------------------------------------
int EnterChar(Disk *a, int chr)
{
	int i, newrow, t, topline, templine1, templine2, tempcol1, tempcol2;

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);

	topline = cline -(row-1);
	templine1 = cline;
	tempcol1 = col;
	tempcol2 = col;
top1:
	lenbuf1 = a->GetLineBuf(linebuf1, &cline);
	if(col > lenbuf1)
	{
		//chr = 32;
		CursorLeft(a, chr);
		if(lenbuf1 >1)
		{
		  a->PushA_Z(chr);
		  chr = 32;
		}
	}

	if(col > lenbuf1)   //user is entering a char past the end of the line
	{
		if(linebuf1[lenbuf1-1] == 13)
		{
			if(cline == lines)
			{
				linebuf1[0] = chr;
				linebuf1[1] = 13;
				linebuf1[2] = 0;
				a->AppendLine(linebuf1);
				lines = a->GetLines();
				col = 1;
				cline++;

				//display new line
				if(row == 22)
				{
					a->ScrollUp(1, 2, 78, 23, gbc[2]*16+gfc[2]);
					a->WriteLine(" ", 1, 23, 78, gbc[2]*16+gfc[2]);

				}
				else
					row++;
				a->WriteLine(linebuf1, 1+lm, (row-1)+2, maxlen, gbc[2]*16+gfc[2]);
ex:         CursorRight(a, chr);
ex1:			SetCursor(a, 1);
				DispStatusLine(a);
				return 0;
			}

			//The user is entering a char after the end of the current line
			//there is a rtn at the end of the line and there is at least
			//one line below it.
			if(lenbuf1 ==1)
			{
				col = 1;  //enter the char at beginning of blank line
				goto top1;
			}


			//do a cursor down, then enter the char at the beginning of the
			//line
			CursorDown(a, chr);
			col = 1;
			topline = cline -(row-1);
			templine1 = cline;
			tempcol1 = col;
			tempcol2 = col;
			a->GetLineBuf(linebuf1, &cline);
			lenbuf1 = a->String::InsertChar(linebuf1, chr, 0);
			goto  a1; //enter marker, write into mem, do parawrap, redisplay ect

		}  //end of if(linebuf1[lenbuf1-1] == 13)
		//col is > lenbuf1, there is no rtn at the end so
		//append the char to the line
		linebuf1[lenbuf1] = chr;
		lenbuf1++;
		linebuf1[lenbuf1] =0;
		col = lenbuf1;
		goto a1; //enter market, write into mem, do parawrap, redisp, ect

	}
	// user is entering a char within the text line
	lenbuf1 = a->String::InsertChar(linebuf1, chr, col-1);
	if(lenbuf1 <maxlen+1) goto a1;
	if(linebuf1[maxlen] == 13)  //if the 79th chr is a rtn
	{
		PerformWrap();
		i = lenbuf1; //i = new shorter lenbuf
		a->OverWriteLine(linebuf1, &cline);

		cline++;
		if(lines >= cline) a->InsertLine(linebuf2, &cline);
		else a->AppendLine(linebuf2);
		lines = a->GetLines();
		cline--;
		DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
		col++;
		if(i <col)       //org shorter lenbuf1;
		{
			col = (col - i);
			CursorDown(a, chr);
			goto ex1; //no cursor right
		}
		goto ex1;  //col on same line, no cursor right
	}
// write into mem, do parawrap, redisplay ect
a1:

	 a->OverWriteLine(linebuf1, &cline);
	 templine2 = cline;
	 tempcol2 = col;
	 i = ParaWrap(a, &templine2, &tempcol2);

	 DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
	 newrow = (templine2 - topline) + 1;
	 if(newrow == row)
	 {
		 cline = templine2;
		 col = tempcol2;
		 goto ex;
	 }

	 if(newrow > row)
	 {
		col = tempcol2;
		CursorDown(a, chr);

		goto ex;
	 }
	 col = tempcol2;
	 CursorUp(a, chr);
	 goto ex;

}
//-------------------------------------------------------------------
int CursorUp(Disk *a, int chr)
{

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);

	if(cline == 1) return 0;
	cline--;
	if(row == 1)
	{
		a->ScrollDown(1, 2, 78, 23, gbc[2]*16+gfc[2]);
	}
	else
		row--;


	DisplayLine(a, cline, row);
	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;

}
//-------------------------------------------------------------------
int CursorDown(Disk *a, int chr)
{

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);

	//if the last line is non blank, append a blank line below it
	//and cursor down to that blank line, otherwise exit.
	if(cline == lines)
	{
		lenbuf1 = a->GetLineBuf(linebuf1, &cline);
		strcpy(linebuf2, linebuf1);
		lenbuf2 = a->RightTrim(linebuf2);
		if(lenbuf2) //last line is non blank, append a blank line
		{
			if(linebuf1[lenbuf1-1] != 13)
			{
				if(lenbuf2 == maxlen)
				{
					//The last line is a full non blank line, with no room to
					//add a rtn to it, so append a blank line and cursor down
					//to it.
h1:				linebuf2[0] = 13;
					linebuf2[1] = 0;
					a->AppendLine(linebuf2);
					col =1;
					a->PushDown();
					return 0;
				}
				//Last line is a non blank line, with space for a rtn
				//add the rtn, then append a new blank line and cursor down
				//to it.

				linebuf2[lenbuf2] = 13;
				linebuf2[lenbuf2+1] =0;
				a->OverWriteLine(linebuf2, &cline);
				goto h1;

			}
			//last line is non blank and contains a rtn at the end
			//append a new blank line
		  goto h1;
		}
		//last line is blank so do nothing
		return 0;
	}

	//we are not on the last line so cursor down to the next line.
	cline++;
	if(row == 22)
	{
		a->ScrollUp(1, 2, 78, 23, gbc[2]*16+gfc[2]);
		a->WriteLine(" ",1, 23, 78, gbc[2]*16+gfc[2]);

	}
	else
		row++;


	DisplayLine(a, cline, row);
	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;
}
//-------------------------------------------------------------------
int CursorLeft(Disk *a, int chr)
{
	int len;

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);


	a->GetLineBuf(linebuf1, &cline);
	len = strlen(linebuf1);

	if(col > 1) goto a11;
	if(cline == 1) return 0; //at home position
	//col = 1, but lines >1
	if(row >1) goto a12;
	a->ScrollDown(1, 2, 78, 23, gbc[2]*16+gfc[2]);

a5:
	cline--;
	DisplayLine(a, cline, row);

	col = strlen(linebuf1);
a9:
	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;

a11:
	if(col > len)
	{
		col = len;
	}
	else
		col--;
	goto a9;

a12:
	row--; goto a5;

}
//-------------------------------------------------------------------
int CursorRight(Disk *a, int chr)
{
	int len;

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);

	a->GetLineBuf(linebuf1, &cline);
	len = strlen(linebuf1);
	if(col >= len) goto a8;
	col++;
	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;

a8:
	//the cursor is currently at the end of a line.
	//set cursor to beginning of next line if there is a next line, also
	//scroll the screen up if necessary
	lines = a->GetLines();
	if(cline == lines) return 0; //at end of text
	cline++;
	col = 1;
	a->GetLineBuf(linebuf1, &cline);
	if(row == 22)
	{
			a->ScrollUp(1, 2, 78, 23, gbc[2]*16+gfc[2]);
			a->WriteLine(" ", 1, 23, 78, gbc[2]*16+gfc[2]);

			row--; //will be compensated for with row++ in next instruction
	}
	row++;
	DisplayLine(a, cline, row);

	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;
}
//-------------------------------------------------------------------


//-------------------------------------------------------------------
int PageDown(Disk *a, int chr)
{
	int topline;


	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);

	topline = cline - (row-1);

	//are we displaying the last line?
	//if so move cursor to start of last line
	if((topline+21) >= lines)
	{
		cline = lines;
		row = (cline - topline)+1;
		col =1;
		goto ex;
	}
	//not yet displaying the last line
	//try to full page down
	topline+= 22;
	if((topline+21) > lines)
	{
		//can't full page down show the last 22 lines
	  topline = (lines - 21);
	}
	cline = topline + (row-1);


ex:
	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;
}
//-------------------------------------------------------------------
int PageUp(Disk *a, int chr)
{
	int topline;

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);

	topline = cline - (row-1);
	lines = a->GetLines();
	if(topline == 1)
	{
		cline = 1;
		row = 1;
		goto a8;
	}

	topline = topline -22;   //try to go back a full page (22 screen lines)
	if(topline <1) topline = 1;
	cline = topline + (row-1);
	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
a8:
	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;

}
//-------------------------------------------------------------------
int CursorHome(Disk *a, int chr)
{
	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);



	return 0;
}
//-------------------------------------------------------------------
int CursorEnd(Disk *a, int chr)
{

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);



	return 0;
}
//-------------------------------------------------------------------
int EnterTab(Disk *a, int chr)
{

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);


	return 0;
}
//-------------------------------------------------------------------
int EnterReturn(Disk *a, int chr)
{
	int topline, xcline, ccline, templine2, tempcol2, i;

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);


	ccline = cline;
	xcline = cline;
	topline = ccline - (row -1);

	lines = a->GetLines();
	lenbuf1 = a->GetLineBuf(linebuf1, &ccline);
	if(col ==1)         //if col at beginning of line
	{
		if(ccline >1)  //if there is a line above
		{
			//see if line above does not contain a retrn
			xcline = ccline-1;
			lenbuf2 = a->GetLineBuf(linebuf2, &xcline);
			if(linebuf2[lenbuf2-1] != 13)
			{
				if(lenbuf2 <maxlen)
				{                    //append rtn to line above
					linebuf2[lenbuf2] = 13;
					lenbuf2++;
					linebuf2[lenbuf2] =0;
					a->OverWriteLine(linebuf2, &xcline);
					DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
					return 0;
				}
				//the cursor is a the beginning of a line
				//the line above does not contain a rtn, but is is 78 chars
				//insert a new blank line and move the cursor down to the
				//beginning of the original line
a1:         linebuf1[0] = 13;
				linebuf1[1] =0;
				a->InsertLine(linebuf1, &ccline);
				DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
				CursorDown(a, chr);
				return 0;
			}
			//The cursor is at the beginning of a line, there is a line above,
			//the line above contains a rtn, so insert a new blank line
			//and cursor down to the original line.
			goto a1;
		}
		//the cursor is at the beginning of the first line, insert new blank
		//line and cursor down to original line
		goto a1;
	}
	//the cursor is not at the biginning of the line
	if(col < lenbuf1)  //cursor not at end of line
	{
		strcpy(linebuf2, &linebuf1[col-1]);
		lenbuf2 = strlen(linebuf2);
		linebuf1[col-1] =13;
		linebuf1[col] =0;
		a->OverWriteLine(linebuf1, &ccline);  //overwrite origial line with
														 //shorter version
		if(ccline < lines)
		{
			ccline++;
			a->InsertLine(linebuf2, &ccline);  //insert remaining part on line
														 //into memory
			if(linebuf2[lenbuf2-1] ==13)
			{
a2:			col =1;
				DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
				CursorDown(a, chr);
				return 0;
			}
			//no rtn at end of remaining part of line so perform wrap
			//to wrap the line below upto the remaining part that was inserted
			col =1;
			templine2 = ccline;
			tempcol2 = col;
			i = ParaWrap(a, &templine2, &tempcol2);
			goto a2;
		}
		//the cursor is not at the end of the line, but we on the last line
		//therefore truncate line at char position col, append rtn chr
		linebuf2[lenbuf2] = 13;
		linebuf2[lenbuf2+1] = 0;
		a->AppendLine(linebuf2);
		goto a2;
	}
	//cursor is at the end of the line
	if(linebuf1[lenbuf1-1] ==13)   //if there is a trn at the end
	{
aa4:	if(ccline < lines)
		{
			ccline++;
			linebuf1[0] = 13;
			linebuf1[1] = 0;
			a->InsertLine(linebuf1, &ccline);
			goto a2;
		}
		//we are on the last line. The cursor is at the end of the line.
		//There is a rtn at the end, so append a new blank line.
a4:	linebuf1[0] = 13;
		linebuf1[1] =0;
		a->AppendLine(linebuf1);
		goto a2;
	}
	//the cursor is at the end of the line, but there is no rtn at the end
	if(lenbuf1 < maxlen)
	{
		linebuf1[lenbuf1] = 13;
		linebuf1[lenbuf1+1] =0;
		a->OverWriteLine(linebuf1, &cline);
	  if(ccline == lines) //if we are on the last line append a blank line
	  {
			goto a4;
	  }
	  //We are not on the last line. The cursor is at the end of the line.
	  //We have just added a rtn to it. insert a new blank line. and crsr
	  //down to the beginning of it.
	  ccline++;
	  //linebuf1[0] = 13;
	  //linebuf1[1] =0;
	  //a->InsertLine(linebuf1, &ccline);
	  goto a2;
	}
	//line is 78 chars, cursor is at end
	goto  aa4;

}
//-------------------------------------------------------------------
int DeleteChar(Disk *a, int chr)
{
	int i, j, newrow, templine2, tempcol2, topline, numlines, ccline, ccol;

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);

	numlines =1;
	ccline = cline;
	ccol = col;
	topline = cline - (row-1); //r # of the top text line
	lines = a->GetLines();
	lenbuf1 = a->GetLineBuf(linebuf1, &ccline);
	if(col > lenbuf1)
	{
		CursorLeft(a, chr);
		return 0;
	}

	if(col == 1)
	{
		if(cline == 1)
			return 0;  //cant delete the 1st char of the 1st line

		//we are on the first char of the line, there is a line above it.
		//If the line above is only 1 char delete the line, else
		//delete the last char of the line above and parawrap, and redisplay.
		 //-----
		 if(row == 1) //show the previous line before deleting a char of it
		 {
			topline--;
			DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
			row =2;
		 }
		//------
		ccline--;
		lenbuf2 = a->GetLineBuf(linebuf2, &ccline);
		if(lenbuf2 ==1)
		{
			a->DeleteLines(&ccline, &numlines);
a1:		tempcol2 =1;
a2:		templine2 = ccline;
			i = ParaWrap(a, &templine2, &tempcol2);
			j = DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
			if(!j)
			{
				//display a blank line to remove the redundant bit on the
				//top screen line
				strcpy(linebuf1, " ");
				a->WriteLine(linebuf1, 1, 2, 78, gbc[2]*16+gfc[2]);
			}
			newrow = (templine2- topline)+1;
			if(newrow == row)
			{
				col = tempcol2;
				goto ex;
			}
			if(newrow < row)
			{
				col = tempcol2;
				CursorUp(a, chr);
				goto ex;
			}
			//newrow > row
			col = tempcol2;
			CursorDown(a, chr);
			goto ex;

		}
		//the cursor is at the start of a line. There is a line above.
		//The length of the line above is greater than 1.
		//Delete the last char of the line above and parawrap and redisplay
		linebuf2[lenbuf2-1] =0;
		a->OverWriteLine(linebuf2, &ccline);
		ccline++; //back to original line
		goto a1;

	}
	//col is > 1  delete a char on the line and parawrap and redisplay

	a->DelChars(linebuf1, col-2, 1);
	a->OverWriteLine(linebuf1, &ccline);
	tempcol2 = col-1;
	goto a2;

ex: SetCursor(a, 1);
	 DispStatusLine(a);

	return 0;
}
//-------------------------------------------------------------------
int EscChar(Disk *a, int chr)
{
	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);


	return 0;
}
//-------------------------------------------------------------------
int DeleteLine(Disk *a, int chr)
{

	lines = a->GetLines();
	if(block.validsw) CancelBlock(a);


	return 0;
}
//-------------------------------------------------------------------
int CutLines(Disk *a, int chr)
{


	return 0;
}
//-------------------------------------------------------------------
int CopyLines(Disk *a, int chr)
{
	return 0;
}
//-------------------------------------------------------------------
int PasteLines(Disk *a, int chr)
{
	return 0;
}
//-------------------------------------------------------------------
int ShowFreeMem(Disk *a, int chr)
{
	return 0;
}
//-------------------------------------------------------------------
//go up to a previous text line, scrolling screen if necessary
void QuickUp(Disk *a)
{
	cline--;
	if(row == 1)
	{
		a->ScrollDown(1, 2, 78, 23, gbc[2]*16+gfc[2]);
	}
	else
		row--;
	DisplayLine(a, cline, row); //line normalised

}
//-------------------------------------------------------------------
//go down to a previous text line, scrolling screen if necessary
void QuickDown(Disk *a)
{
	cline++;
	if(row == 22)
	{
		a->ScrollUp(1, 2, 78, 23, gbc[2]*16+gfc[2]);
		a->WriteLine(" ",1, 23, 78, gbc[2]*16+gfc[2]);

	}
	else
		row++;
	DisplayLine(a, cline, row); //line normalised

}


//--------------------------------------------------------------------
int BlockCursorUp(Disk *a, int chr); //prototypes
int BlockCursorDown(Disk *a, int chr);
int BlockCursorLeft(Disk *a, int chr);
int BlockCursorRight(Disk *a, int chr);
//---------------------------------------------------------------------
// match the mouse line to the block
int TrackCursor(Disk *a, int chr)
{

	int targetline, topline, t, len, newrow;

	lines = a->GetLines();

	if((mouse.y >= 2) && (mouse.y <= 22))
	{
		newrow = mouse.y-1;
		topline = (cline - row)+1;
		targetline = (topline+ newrow) -1;
		if(targetline> lines) targetline = lines;
		newrow = (targetline - topline)+1;

		while(newrow <row)
		{
			BlockCursorUp(a, chr);
		}
		while(newrow> row)
		{
			BlockCursorDown(a, chr);
		}

	}

ex:
	BlockCursorRight(a, chr); //just match mouse column to block
	return 0;
}
//---------------------------------------------------------------------
int BlockCursorUp(Disk *a, int chr)
{

	int targetline, topline, t, len, scrollflg, newrow;

	col = mouse.x-lm;

	if(col <1) col =1;
	if(col >maxlen) col= maxlen;




top:
	lines = a->GetLines();

	if(cline == 1) goto ex; //cant go up
	DisplayLine(a, cline, row); //normalise current line
	//-----------
	//set the coords of the new end point
	block.endline = cline-1;
	block.endcol = col;

	//Is the new end point either on, above or below the starting point?
	if(block.endline == block.stline) // if its on the starting point
	{
		DisplayLine(a, cline, row); //normalise the current line
		QuickUp(a);
		if(block.stcol == block.endcol)
		{
			//line above already normalised
			goto ex;
		}
		//the lines are the same, but there is a gap in the cols
		//colour the gap
		len = block.endcol - block.stcol;
		if(len <1) len = 0 - len;
		t = block.endcol;
		if(block.stcol < t) t = block.stcol;
		a->ColourLine(t+lm, row+1, len, LIGHTGRAY*16+BLACK);
		goto ex;
	}

	//The new ending line is not the same as the starting line
	//Is it above or below?
	if(block.endline < block.stline) //if above
	{
		//if the new end line is more than 1 line above the
		//starting line, completely colour the current line and
		//colour the line above from col to end.
		if((block.stline - block.endline) >1)
		{
			a->ColourLine(1+lm, row+1, maxlen, LIGHTGRAY*16+BLACK);
			QuickUp(a);
			a->ColourLine(col+lm, row+1, maxlen+1-col, LIGHTGRAY*16+BLACK);
		  goto ex;
		}
		//the end point if just 1 line above the start line
		//colour the left part of the current line to the cursor
		//then colour the right part of the line above to its end
		if(block.stcol >1) //colour left
			a->ColourLine(1+lm, row+1, block.stcol-1, LIGHTGRAY*16+BLACK);
		QuickUp(a);
		a->ColourLine(col+lm, row+1, maxlen+1-col, LIGHTGRAY*16+BLACK); //colour right
		goto ex;
	}
	//the new end line is below the start line
	//completely wipe the current line (just did upon entry to this function)
	//and colour left half of line above
	QuickUp(a);
	if(col >1) a->ColourLine(1+lm, row+1, col-1, LIGHTGRAY*16+BLACK);  //colour left


ex:
	BlockCursorRight(a, chr);  //track horozantal mouse;
	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;

}
//-------------------------------------------------------------------
int BlockCursorDown(Disk *a, int chr)
{
	int len, t;

	col = mouse.x-lm;
	if(col <1) col =1;
	if(col >maxlen) col =maxlen;
	lines = a->GetLines();
	if(cline == lines) goto ex;   //can't go down any more
	DisplayLine(a, cline, row);    //normalise current line

	block.endline = cline+1;
	block.endcol = col;

	//Is the new line either on, above or below the start line?
	if(block.endline == block.stline)   //if start and end are on same line
	{
		QuickDown(a);
		if(block.endcol == block.stcol) goto ex;
		//colour the gap between the cols
		len = block.endcol - block.stcol;
		if(len <1) len = 0 - len;
		t = block.endcol;
		if(block.stcol < t) t = block.stcol;
		a->ColourLine(t+lm, row+1, len, LIGHTGRAY*16+BLACK);
		goto ex;
	}
	//The new end line is not the same as the start line.
	//Is the new end line above or below the start?
	if(block.endline < block.stline)  //if new end above start line
	{
		//the new ending line is still above and we're going down
		//therefore we're deleting
		QuickDown(a);
		a->ColourLine(col+lm, row+1, maxlen+1-col, LIGHTGRAY*16+BLACK); //colour right
		goto ex;
	}
	//the endling line is below the start line, we're enlarging the block
	//if the ending line is more than 1 line below the start, completely
	//colour the current line,
	//else colour right half of current line
	//then colour the left half of the new end line
	if((block.endline - block.stline) >1)
		a->ColourLine(1+lm, row+1, maxlen, LIGHTGRAY*16+BLACK); //complete colr
	else //colour right half of current line
		a->ColourLine(block.stcol+lm, row+1, maxlen+1-block.stcol, LIGHTGRAY*16+BLACK);
	QuickDown(a);
	if(col >1) a->ColourLine(1+lm, row+1, col, LIGHTGRAY*16+BLACK); //colour left new end


ex:
	BlockCursorRight(a, chr);
	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;

}
//-------------------------------------------------------------------
int BlockCursorLeft(Disk *a, int chr)
{
	int t, len;
	col = mouse.x -lm;
	if(col <1) col =1;
	if(col >maxlen) col= maxlen;

	//if(col ==1) return 0;
	DisplayLine(a, cline, row);   //display current line as normal

	block.endcol = col;
	if(block.endline == block.stline)   //if start and end are on same line
	{

		if(block.endcol == block.stcol) goto ex;
		//colour the gap between the cols
		len = (block.endcol - block.stcol);
		if(len <1) len = (0 - len);
		t = block.endcol;
		if(block.stcol < t) t = block.stcol;
		a->ColourLine(t+lm, row+1, len, LIGHTGRAY*16+BLACK);
		goto ex;
	}
	//The start line and end line are not the same
	// if the endline is above we increase the right colour
	if(block.endline < block.stline)
	{
		//right colour increase
		col = block.endcol;
		a->ColourLine(col+lm, row+1, maxlen+1-col, LIGHTGRAY*16+BLACK);
		goto ex;
	}
	//the end line is below so we decrease the left colour
	a->ColourLine(1+lm, row+1, block.endcol, LIGHTGRAY*16+BLACK);

ex:
	col = block.endcol;
	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;
}
//-------------------------------------------------------------------
int BlockCursorRight(Disk *a, int chr)
{
	int t, len;
	if(mouse.x < lm+1) return 0;
	if(mouse.x >lm+maxlen) return 0;

	//if(mouse.y ==0) mouse.y =2;
	col = mouse.x - lm;
	if(col >maxlen) col= maxlen;

	//if(col ==78) return 0;
	DisplayLine(a, cline, row);   //display current line as normal

	block.endcol = col;
	if(block.endline == block.stline)   //if start and end are on same line
	{

		if(block.endcol == block.stcol) goto ex;
		//colour the gap between the cols
		len = (block.endcol - block.stcol);
		if(len <1) len = (0 - len);
		t = block.endcol;
		if(block.stcol < t) t = block.stcol;
		a->ColourLine(t+lm, row+1, len, LIGHTGRAY*16+BLACK);
		goto ex;
	}
	//The start line and end line are not the same
	// if the endline is above we decrease the right colour
	if(block.endline < block.stline)
	{
		//right colour decrease
		col = block.endcol;
		a->ColourLine(col+lm, row+1, maxlen+1-col, LIGHTGRAY*16+BLACK);
		goto ex;
	}
	//the end line is below so we increase the left colour
	if((block.endcol>1))
		a->ColourLine(1+lm, row+1, block.endcol-1, LIGHTGRAY*16+BLACK);



ex:
	col = block.endcol;
	SetCursor(a, 1);
	DispStatusLine(a);
	return 0;

}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
int SetTextCursor(Disk *a)
{
	int t, i, released, tx, ty, chr, x, y, topline, newline, newrow, ccline;
	int cx, cy, prevchr, tp1, tp2;

	//before we begin to track the cursor, first make sure there is a blank
	//line at the botton, if not append one.

	lines = a->GetLines();
	released = 0;
	block.validsw = 0;
	topline = cline - (row-1);
	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);

	orgtopline = topline;  //save these vars for the cut routine
	orgrow = row;

	lenbuf1 = a->GetLineBuf(linebuf1, &cline);
	x = mouse.x;
	y = mouse.y-1;
	newrow = y;
	//attempt to set ccline where the user desires
	ccline = topline + (newrow-1);
	if(ccline > lines)
	{
		ccline = lines;
	}
	row = (ccline - topline) +1;
	col = mouse.x - lm;
	if(mouse.x < (1+lm)) col =1;

	cline = ccline;
	lenbuf1 = a->GetLineBuf(linebuf1, &cline);
	if(col > lenbuf1) col = lenbuf1;

ex:
	SetCursor(a, 1);
	DispStatusLine(a);

	//if the user released the mouse exit
	block.validsw =0;
	block.stline = cline;
	block.stcol = col;
	block.endline = cline;
	block.endcol = col;

	tp1 = 2;
	chr =0;
get:
	/*
	cx = wherex(); //for debugging: shows block select vars
	cy = wherey();
	gotoxy(1,1);
	printf("stlcol=%5d, stline=%5d, endcol=%5d, endline=%5d", block.stcol, block.stline, block.endcol, block.endline);
	gotoxy(cx, cy);
	*/

	chr = a->WaitEvent(1);
	if(!mouse.ButtonState) goto ex2;
	switch( chr)
	{
		case 259: //up
b1:	  block.validsw =1;
		  if((mouse.y > 1) && (mouse.y < 23))
		  {
			  TrackCursor(a, chr);
			  goto get;
		  }
		  if(mouse.y ==0 )
		  {
			  BlockCursorUp(a, chr);
			  goto get;
		  }

		  if(lines == cline) goto get;
		  if(mouse.y == 24)
		  {
			  BlockCursorDown(a, chr);
		  }
		  goto get;



		case 260:   //down
		  block.validsw =1;
		  goto b1;


		case 261:  //left
			block.validsw =1;
			goto b1;

		case 262: //right
			block.validsw =1;
			goto b1;


		case 266: //down right
			goto b1;

		case 265: //down left
			goto b1;

		case 263: //up left
			goto b1;

		case 264: // up right
			goto b1;

		default:
			if(mouse.y == 0) //auto scroll up
			{
				/*if(row> 1)
				{
					mouse.y = 2;
					TrackCursor(a, chr);
					mouse.y =0;
				}
				*/
				BlockCursorUp(a, chr);
				goto get;
			}
			if(mouse.y ==24)
			{
				BlockCursorDown(a, chr);
				goto get;
			}


			goto get;
	}


ex2:

	SetCursor(a, 1);
	return 0;
}
//--------------------------------------------------------------------
//This function is called ScrollDownArrow()
//The user has clicked on the down arrow and still has the mouse button down
//Scroll the text up but leave the cursor where it is
int MouseCursorDown(Disk *a)
{
	int topline, bottomline;

	topline = cline - (row-1);
	bottomline = topline+22;
	if(bottomline > lines) return 0; //already showing last line

	a->ScrollUp(1, 2, 78, 23, gbc[2]*16+gfc[2]);

	a->GetLineBuf(linebuf1, &bottomline);
	a->WriteLine(" ",1, 23, 78, gbc[2]*16+gfc[2]);
	a->WriteLine(linebuf1, 1+lm, 23, maxlen, gbc[2]*16+gfc[2]);
	cline++;
	SetCursor(a, 1);
	DispStatusLine(a);
	//-------
	return 1;
}
//-------------------------------------------------------------------
//This function is called ScrollUpArrow()
//The user has clicked on the up arrow and still has the mouse button down
//Scroll the text down but leave the cursor where it is
int MouseCursorUp(Disk *a)
{
	int topline;

	topline = (cline - (row-1));

	if(topline ==1) return 0; //already showing last line

	a->ScrollDown(1, 2, 78, 23, gbc[2]*16+gfc[2]);
	topline--;

	a->GetLineBuf(linebuf1, &topline);
	a->WriteLine(linebuf1, 1+lm, 2, maxlen, gbc[2]*16+gfc[2]);
	cline--;
	SetCursor(a, 1);
	DispStatusLine(a);
	//-------
	return 1;
}
//-------------------------------------------------------------------





//--------------------------------------------------------------------
int ScrollDownArrow(Disk *a)
{
		int t, chr;

		t =0;
		lines = a->GetLines();

		if(!mouse.ButtonState) return 0;
da:   t = a->WaitMouseButtonRelease(30);
		goto wa45;
wa4:  t = a->WaitMouseButtonRelease(4);
wa45: if(cline == lines) return 0;
		MouseCursorDown(a);
		if(!t) return 0;
		goto wa4;

}
//--------------------------------------------------------------------
int ScrollUpArrow(Disk *a)
{

		int t, chr;

		t =0;
		lines = a->GetLines();

		if(!mouse.ButtonState) return 0;
da:   t = a->WaitMouseButtonRelease(30);
		goto wa45;
wa4:  t = a->WaitMouseButtonRelease(4);
wa45: if(cline == 1) return 0;
		MouseCursorUp(a);
		if(!t) return 0;
		goto wa4;
}
//---------------------------------------------------------------------
int CalcScrollBar(int topline)
{

	float f_cursorpos, f_lines, f_topline, f_cursorlen;
	int pos;
	f_topline = topline;
	f_lines = lines;
	f_cursorlen = cursorlen;

	f_cursorpos = ((1/f_lines) * f_topline) *20;

	pos = f_cursorpos;


	if((cursorpos+ cursorlen) > 20)
		cursorpos--;

	if((topline + 21) >= lines)
	{
		pos = 20 - cursorlen;
	}
	//a->ShowVerticalScrollBar(79, 2, 20, cursorlen, cursorpos, a1, a2, a3);
	return pos;
}
//---------------------------------------------------------------------
//Called by ClickInScrollBar()
//The user wants to move the editor's scrollbar cursor up.
//The movement the user gave was big enough to do so but is
//it a legal move?
int MoveScrollBarCursorUp(Disk *a)
{
	int t, bottomline, topline;


	topline = cline - (row-1);
	bottomline = topline +21;

	lines = a->GetLines();
	if(lines <22) return 0;

	if(topline ==1) return 0;        //cant go up, already
												 //displaying 1st line

top:
	topline--;
	if(topline == 1) goto disp;
	t = CalcScrollBar(topline);
	if(t == cursorpos) goto top;
	if((t > 0 ) && (mouse.y <=3)) goto top;
	if((mouse.y >=3) && ((mouse.y-3) < t)) goto top;

	if(t == 0) topline =1;
disp:
	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
	cline = topline + (row-1);

	SetCursor(a, 1);
	DispStatusLine(a);
	return 1;     //flg: job done

}
//---------------------------------------------------------------------
// Called by ClickInScrollBar()
//The user wants to move the editor's scrollbar cursor down.
//The movement the user gave was big enough to do so but is
//it a legal move?
int MoveScrollBarCursorDown(Disk *a)
{
	int t, bottomline, topline;


	topline = cline - (row-1);
	bottomline = topline +21;

	lines = a->GetLines();
	if(lines <22) return 0;

	if(bottomline >= lines) return 0; //cant do down, already
												 //displaying last line

top:
	topline++;
	if((topline+21) >= lines) goto disp;
	t = CalcScrollBar(topline);
	if(t == cursorpos) goto top;
	if(((t+ cursorlen) <20 ) && (mouse.y >=22)) goto top;
	if((mouse.y-2) > (t+cursorlen)) goto top;
disp:
	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
	cline = topline + (row-1);
	if(cline > lines)
	{
		cline = lines;
		row = (cline - topline)+1;
	}

	SetCursor(a, 1);
	DispStatusLine(a);

	return 1;     //flg: job done

}

//---------------------------------------------------------------------
//A click occurred in the editor's scrollbar
//Should we move the scrollbar cursor up or down?
int ClickInScrollBar(Disk *a)
{
		int t, chr;

wa5:  chr = a->GetMouseOrKey();
		if(!mouse.ButtonState)
		{
			return 0;
		}
		a->LimitMouse(79, 2, 79, 23);
wa55: t = mouse.y;
		//a->ShowCoordinates();
wa56:	chr = a->WaitGetMouseOrKey();

		if(chr!= 256) //mouse not moved
		{
			if(!mouse.ButtonState)  //if button released
			{
				a->LimitMouse(0, 0, 79, 24); //restore mouse area
				return 0;
			}
			else goto wa56;
		}

		if(t == mouse.y) goto wa56; //move not big enough

		if(t< mouse.y) //if movement was down
		{
			MoveScrollBarCursorDown(a);
			goto wa55;
		}
		else
		{
			MoveScrollBarCursorUp(a);
			goto wa55;
		}

}
//---------------------------------------------------------------------


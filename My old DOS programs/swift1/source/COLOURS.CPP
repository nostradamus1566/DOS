//-----------------------------------------------------------------
// (c) Michael Finnegan     29 June 1999
//-----------------------------------------------------------------

#include "clip.h"
#include "editor.h"
#include "colours.h"
#include "labels.h"
#include <stdlib.h>
#include "file.h"

//-------------------------------------------------------------------
static char *colournames[16] = {
		" BLACK ",     /* dark colours */
		" BLUE ",
		" GREEN ",
		" CYAN ",
		" RED ",
		" MAGENTA ",
		" BROWN ",
		" LIGHTGRAY ",
		" DARKGRAY ",    /*light colours */
		" LIGHTBLUE ",
		" LIGHTGREEN ",
		" LIGHTCYAN ",
		" LIGHTRED ",
		" LIGHTMAGENTA ",
		" YELLOW ",
		" WHITE "
	};
//-------------------------------------------------------------------
void ColoursHelp(Disk *a)
{

  static char *pchar[20] = {"Colours",
  " By selecting a colour from the Foreground and Background ",
  " colour palettes you can choose the individual colour ",
  "   for each of the five items:",
  "   Window Border",
  "   Menubar",
  "   Normal Text",
  "   Status Line",
  "   the Scrollbar.",
  "  ",
  " To select one of the items click on it, then choose its",
  " foreground and background colours from the palettes.",

														};
		  pchar[4][1] =4;
		  pchar[5][1] =4;
		  pchar[6][1] =4;
		  pchar[7][1] =4;
		  pchar[8][1] =4;
		  a->CursorOn();
		  a->ShowDialogBox(&pchar[0], 11, 1, 11, 0);

}
//-----------------------------------------------------------------------
//Ask user if he wants to save the file
//Exit:
// ax =0 yes
// ax = 1 no
// ax =2 cancel
int AskSaveModified(Disk *a)
{

	int i, c, t, l, maxsize;
	char pchar[4][128];
	char *pchar2[4];
	strcpy(&pchar[0][0], "Save File");
	strcpy(&pchar[1][0], "The file");
	pchar[2][0] = 39;
	strcpy(linebuf2, editortitle);
	l = strlen(linebuf2);
	if(l > 66)
	{

		linebuf2[66] =0;

	}
	l = strlen(linebuf2);
	linebuf2[l] = 39;
	linebuf2[l+1] =0;
	l++;
	strcpy(&pchar[2][1], linebuf2);
	strcpy(&pchar[3][0], "has not been saved yet. save it now?");
	maxsize = 38;  //size of largest string so far
	l = strlen(&pchar[2][0]);
	if(l> maxsize) maxsize =l;

	for(t = 1; t<4; t++){

		l = strlen(&pchar[t][0]);
		c = ((maxsize/2)) - (l/2);
		if(c >0)
		{
			memset(linebuf1, 32, c);
			linebuf1[c] =0;
			a->InsertString(&pchar[t][0], linebuf1, 0);
		}
	}

	pchar2[0] = &pchar[0][0];
	pchar2[1] = &pchar[1][0];
	pchar2[2] = &pchar[2][0];
	pchar2[3] = &pchar[3][0];




	a->CursorOn();
	a->m_repeatflg =2;
	i = a->ShowDialogBox(&pchar2[0], 3, 7, 3, 0, 0, 2);
	//i = 13 = yes
	//i = 27 = no
	// i= 2 = cancel
	switch(i)
	{
		case 13:
			i =0;
			break;
		case 27:
			i =1;
			break;
	}


	a->m_repeatflg =0;
	return i;
}
//-------------------------------------------------------------------
//Search the list of colours to see which one matches the users colour
//Exit:
//  ax = 0 - 15 if a match found
//  ax = -1 if invalid colour

int WhichColour(Disk *a, char *userscolour)
{
	int t, i, found;

	found = -1;   // assume users colour name is illegal


	for(t =0; t< 16; t++){
		i = a->CompareStrings(&colournames[t][0], userscolour, 1, 0);
		if(i)
		{
			found = t;
			break;
		}
	}

	return found;
}
//-------------------------------------------------------------------
int SaveSettings1(Disk *a)
{


	FILE *fp;

		fp = fopen("C:\\swift2.var", "wb");
		//write default var file to disk
		fprintf(fp, "[SWIFT.EXE Internal Settings]\n");
		if(m_wordwrap) fprintf(fp, "Word Wrap      = ON\n");
		else           fprintf(fp, "Word Wrap      = Off\n");

		fprintf(fp, "Max Line Len   = %2d\n", maxlen);
		fprintf(fp, "Lines Per Page = %2d\n", lpp);
		fprintf(fp, "Window Border  = %s, %s\n", &colournames[gbc[0]][0],
															  &colournames[gfc[0]][0]);

		fprintf(fp, "Menubar        = %s, %s\n", &colournames[gbc[1]][0],
															  &colournames[gfc[1]][0]);

		fprintf(fp, "Normal Text    = %s, %s\n", &colournames[gbc[2]][0],
															  &colournames[gfc[2]][0]);


		fprintf(fp, "Status Line    = %s, %s\n", &colournames[gbc[3]][0],
															  &colournames[gfc[3]][0]);

		fprintf(fp, "Scrollbar      = %s, %s\n",     &colournames[gbc[4]][0],
															  &colournames[gfc[4]][0]);



		if(mwwo) fprintf(fp, "Match Whole Word Only = YES\n");
		else     fprintf(fp, "Match Whole Word Only = No\n");

		fprintf(fp, "%s\n", editortitle);
		fprintf(fp, "Col = %2d, Row = %2d, Line = %4d\n", col, row, cline);
		fclose(fp);
return 0;
}
//-------------------------------------------------------------------


//Save program setting to the file "c:\ swift.var"
int SaveSettings(Disk *a)
{
	int i, ii;
	File b;


	if(!modified)
	{
		SaveSettings1(a);
		return 34;  //exit program immediately
	}
	i = AskSaveModified(a);
	switch(i)
	{
		case 0:  //user wants to save the program
			ii = b.Save(a);
			if(ii == 0 || ii == 13)
			{

				SaveSettings1(a);
				return 34; //saved ok: exit immediately
			 }

			 else
				 return ii;    //no exit

		case 1: //user want to exit without saving the modified file
			SaveSettings1(a);
			return 34;


	}
	//user chose cancel
	return i;
}
//--------------------------------------------------------------------
//Read in a background and foreground colour from the file
int GetColour(Disk *a, FILE *fp, int item)
{

	int p1len, t, i;
	char *p1, *p2;
	char bgcname[25], fgcname[25];


	fgets(linebuf1, 78, fp);    //background, foreground colours as text
	lenbuf1 = strlen(linebuf1);
	lenbuf1--;
	linebuf1[lenbuf1] =0;  //delete the "\n" char at the end of the string
	t = a->RightFindChar(linebuf1, '=', lenbuf1);
	p1 = &linebuf1[t];
	a->LeftTrim(p1);
	a->RightTrim(p1);
	p1len = strlen(p1);
	t = a->RightFindChar(p1, ',', p1len);
	if(!t) return -1;  //error
	p1[t-1] =0;
	strcpy(bgcname, p1);
	p2 = &p1[t];
	strcpy(fgcname, p2);
	a->LeftTrim(bgcname);
	a->RightTrim(bgcname);
	a->LeftTrim(fgcname);
	a->RightTrim(fgcname);
	//which background colour is it?
	i= WhichColour(a, bgcname);
	if(i == -1) return -1;
	gbc[item] = i;
	i= WhichColour(a, fgcname);
	if(i == -1) return -1;
	gfc[item] = i;

	return item;
}
//--------------------------------------------------------------------

//Load the settings from the swift.var file
int LoadSettings(Disk *a)
{
	FILE *fp;
	int itm, i, t, l, p1len, p2len;
	char *p1, *p2;
	char bgcname[25];
	char fgcname[25];



	//delete old swift files "swift.var"
	remove("C:\\swift.var");
	/* strcpy(linebuf1, a->GetPath());
	l = strlen(linebuf1);
	if(linebuf1[l-1] !='\\')
	{
		linebuf1[l] = '\\';
		l++;
		linebuf1[l] =0;
	}
	strcat(linebuf1, "readme.txt");
	remove(linebuf1);
	*/



	if((fp =fopen("c:\\swift2.var", "rb")) ==NULL)
	{
top:	fclose(fp);
		fp = fopen("C:\\swift2.var", "wb");
		//write default var file to disk
		fprintf(fp, "[SWIFT.EXE Internal Settings]\n");
		fprintf(fp, "Word Wrap      = ON\n");
		fprintf(fp, "Max Line Len   = 68\n");
		fprintf(fp, "Lines Per Page = 60\n");
		fprintf(fp, "Window Border  = BLACK, LIGHTGRAY\n");
		fprintf(fp, "Menubar        = LIGHTGRAY, BLACK\n");
		fprintf(fp, "Normal Text    = BLACK, LIGHTGRAY\n");
		fprintf(fp, "Status Line    = LIGHTGRAY, BLACK\n");
		fprintf(fp, "Scrollbar      = LIGHTGRAY, BLACK\n");
		fprintf(fp, "Match Whole Word Only = YES\n");

		lpp =60;
		gfc[0] = LIGHTGRAY;  //Window border
		gbc[0] = BLACK;
		gfc[1] = BLACK;   //Menubar
		gbc[1] = LIGHTGRAY;
		gfc[2] = LIGHTGRAY;   //Normal text
		gbc[2] = BLACK;
		gfc[3] = BLACK;   //Status line
		gbc[3] = LIGHTGRAY;
		gfc[4] = BLACK;   //Scrollbar
		gbc[4] = LIGHTGRAY;
		_getcurrentpath();

		strcpy(f_editortitle, currentpath);
		t = strlen(f_editortitle);
		if(f_editortitle[t-1] != 92)
		{
			f_editortitle[t] = 92;
			f_editortitle[t+1] =0;
		}
		strcat(f_editortitle, "readme2.txt");

		fprintf(fp, "%s\n", f_editortitle);
		fprintf(fp, "Col = 1, Row = 1, Line = 1\n");
		fclose(fp);

		m_wordwrap =1;
		mwwo =1;
		mc =0;
		maxlen =68;
	}
	else
	{
		fgets(linebuf1, 78, fp);
		fgets(linebuf1, 78, fp);   //get word wrap line
		lenbuf1 = strlen(linebuf1);
		lenbuf1--;
		linebuf1[lenbuf1] =0;  //delete the "\n" char at the end of the string
		t = a->RightFindChar(linebuf1, '=', lenbuf1);
		p1 = &linebuf1[t];
		a->LeftTrim(p1);
		a->RightTrim(p1);
		if(a->CompareStrings(p1, "ON", 0, 0) == 1) m_wordwrap =1;
		else m_wordwrap =0;

		fgets(linebuf1, 78, fp);       //max line len
		lenbuf1 = strlen(linebuf1);
		lenbuf1--;
		linebuf1[lenbuf1] =0;  //delete the "\n" char at the end of the string
		t = a->RightFindChar(linebuf1, '=', lenbuf1);
		p1 = &linebuf1[t];
		a->LeftTrim(p1);
		a->RightTrim(p1);
		maxlen = atoi(p1);

		fgets(linebuf1, 78, fp);    //lines per page
		lenbuf1 = strlen(linebuf1);
		lenbuf1--;
		linebuf1[lenbuf1] =0;  //delete the "\n" char at the end of the string
		t = a->RightFindChar(linebuf1, '=', lenbuf1);
		p1 = &linebuf1[t];
		a->LeftTrim(p1);
		a->RightTrim(p1);
		lpp = atoi(p1);
		//read in the 5 colours
		itm =0;
a1:
		i= GetColour(a, fp, itm);
		if(i == -1) goto top;
		itm++;
		if(itm <5) goto a1;

//-------------------------------

		fgets(linebuf1, 78, fp);   //get match whole word only
		lenbuf1 = strlen(linebuf1);
		lenbuf1--;
		linebuf1[lenbuf1] =0;  //delete the "\n" char at the end of the string
		t = a->RightFindChar(linebuf1, '=', lenbuf1);
		p1 = &linebuf1[t];
		a->LeftTrim(p1);
		a->RightTrim(p1);
		if(a->CompareStrings(p1, "Yes", 0, 0) == 1) mwwo =1;
		else mwwo =0;

		//try to read in values for editortitle, col, row, cline
		fgets(linebuf1, 127, fp);
		lenbuf1 = strlen(linebuf1);
		a->RightTrim(linebuf1);
		strcpy(f_editortitle, linebuf1);
		fgets(linebuf1, 78, fp);
		lenbuf1 = strlen(linebuf1);
		t = a->LeftFindChar(linebuf1, '=', 0);
		linebuf1[t-1] =32;
		p1 = &linebuf1[t];
		f_col =atoi(p1);
		t = a->LeftFindChar(linebuf1, '=', 0);
		linebuf1[t-1] =32;
		p1 = &linebuf1[t];
		f_row = atoi(p1);
		t = a->LeftFindChar(linebuf1, '=', 0);
		linebuf1[t-1] =32;
		p1 = &linebuf1[t];
		f_cline = atoi(p1);

		fclose(fp);
	}


	return 0;
}
//--------------------------------------------------------------------
//Show a colour
// if crsr = -1 don't write any char
// if crsr =0 write spaces
// if crsr = 1 write marker
int ShowAColour(Disk *a, int x1, int y1, int len, int colr, int crsr)
{

	char buf[80];

	switch(crsr)
	{
		case 0:   //write spaces
			memset(buf, 32, len);
			buf[len] =0;
			a->WriteLine(buf, x1, y1, len, colr*16+0);
			break;

		case 1:   //write markers
			memset(buf, 7, len);
			buf[len] =0;
			if(colr ==7)
				a->WriteLine(buf, x1, y1, len, colr*16+0);
			else
				a->WriteLine(buf, x1, y1, len, colr*16+LIGHTGRAY);
			break;

		case -1:
			a->ColourLine(x1, y1, len, colr*16+LIGHTGRAY);
			break;

	}

	return 0;
}
//--------------------------------------------------------------------
//Show the current colours selected
int ShowColours(Disk *a, int f, int b, int rx1, int ry1, int item)
{
	int j, t, ch, fx1, fy1, fx2, fy2, bx1, by1, bx2, by2, attrib, style;
	int x1, y1, x2, y2, len, crsr, colr;
	int fcrsrpos, bcrsrpos;

	fcrsrpos = f;
	bcrsrpos =b;
	fx1 = rx1 -1;
	fy1 = ry1-1;
	fx2 = fx1+13;
	fy2 = fy1+5;

	bx1 = fx1+19;
	by1 = fy1;
	bx2 = bx1+13;
	by2 = by1+5;


	a->DrawBox(fx1, fy1, fx2, fy2, LIGHTGRAY*16+BLACK);
	a->WriteLine("Foreground", fx1+1, fy1, -1, LIGHTGRAY*16+BLACK);
	a->DrawBox(bx1, by1, bx2, by2, LIGHTGRAY*16+BLACK);
	a->WriteLine("Background", bx1+1, by1, -1, LIGHTGRAY*16+BLACK);

//show the foreground colour pallette

	x1 = fx1+1;
	y1 = fy1+1;
	len =3;
	crsr =0;
	for(j = 0; j < 4; j++){
		x1 = fx1+1;
		for(t = 0; t <4; t++){
			colr = j*4+t;
			crsr =0;      //assume don't show matker
			if(colr == fcrsrpos)
				crsr =1;  //show marker
			ShowAColour(a, x1, y1+j, len, colr, crsr);
			x1+=3;
		}

  }


//show the background colour palette
	y1 = by1+1;
	len =3;
	crsr =0;
	for(j = 0; j < 4; j++){
		x1 = bx1+1;
		for(t = 0; t <4; t++){
			colr = j*4+t;
			crsr =0;      //assume don't show matker
			if(colr == bcrsrpos)
				crsr =1;  //show marker
			ShowAColour(a, x1, y1+j, len, colr, crsr);
			x1+=3;
		}

  }

	a->ColourLine(rx1+20, ry1+6, 12, bcrsrpos*16+fcrsrpos);
	a->ColourLine(rx1+20, ry1+7, 12, bcrsrpos*16+fcrsrpos);


	//show the cursor at the current item
	for(t = 0; t < 5; t++){
		if(t != item)
			a->ColourLine(rx1-1, ry1+6+t, 15, LIGHTGRAY*16+BLACK);
		else
			a->ColourLine(rx1-1, ry1+6+t, 15, BLACK*16+LIGHTGRAY);
	}






ex:
	return 0;
}
//--------------------------------------------------------------------
int ChangeColours(Disk *a)
{

  char tempbuf[81];
  char tempbuf2[81];
  int t, ch, m, x, y, i, ii;
  int fcrsrpos, bcrsrpos;
  int rx1, ry1, f_gbc[5], f_gfc[5];
  static int item;

  if( item <0 || item > 4) item =0;
  Disk b;
  //Copy the original global colours because we will need
  //them if the user cancels
  for(t =0; t<5; t++){
	f_gfc[t] = gfc[t];
	f_gbc[t] = gbc[t];
 }

  rx1 = 24;
  ry1 = 6;

	fcrsrpos =gfc[item];  //pos 0-15 the foureground cursor is currently on
	bcrsrpos =gbc[item];  //pos 0-7  the background cursor is currently on


  static char *pchar[20] = {"Colours",
														 "                                 ",
														 " .            .                 .",
														 " .            .     .............",
														 " .            .                 ",
														 " ..............",
														 " ",
														 "               ",
														 "  Item:                Item Colour",
														 "  Window Border        Text Text ",
														 "  Menubar              Text Text ",
														 "  Normal Text ",
														 "  Status Line ",
														 "  Scrollbar"
														};


  a->CursorOn();

	TMousePos MousePos[40];
	MousePos[0].x1 = 22;   //ok
	MousePos[0].y1 = 20;
	MousePos[0].x2 = 29;
	MousePos[0].y2 = 20;

	MousePos[1].x1 = 35;  //cancel
	MousePos[1].y1 = 20;
	MousePos[1].x2 = 44;
	MousePos[1].y2 = 20;

	MousePos[2].x1 = 49;     //help
	MousePos[2].y1 = 20;
	MousePos[2].x2 = 56;
	MousePos[2].y2 = 20;

	y = ry1; x = rx1;

	MousePos[3].x1 = x;     //foreground black
	MousePos[3].y1 = y;
	MousePos[3].x2 = x+2;
	MousePos[3].y2 = y;
	x+=3;
	MousePos[4].x1 = x;     //foreground blue
	MousePos[4].y1 = y;
	MousePos[4].x2 = x+2;
	MousePos[4].y2 = y;
	x+=3;
	MousePos[5].x1 = x;     //foreground green
	MousePos[5].y1 = y;
	MousePos[5].x2 = x+2;
	MousePos[5].y2 = y;
	x+=3;
	MousePos[6].x1 = x;     //foreground cyan
	MousePos[6].y1 = y;
	MousePos[6].x2 = x+2;
	MousePos[6].y2 = y;
	x-=9;
	y++;
	MousePos[7].x1 = x;     //foreground  red
	MousePos[7].y1 = y;
	MousePos[7].x2 = x+2;
	MousePos[7].y2 = y;
	x+=3;
	MousePos[8].x1 = x;     //foreground  magenta
	MousePos[8].y1 = y;
	MousePos[8].x2 = x+2;
	MousePos[8].y2 = y;
	x+=3;
	MousePos[9].x1 = x;     //foreground  brown
	MousePos[9].y1 = y;
	MousePos[9].x2 = x+2;
	MousePos[9].y2 = y;
	x+=3;
	MousePos[10].x1 = x;     //foreground lightgray
	MousePos[10].y1 = y;
	MousePos[10].x2 = x+2;
	MousePos[10].y2 = y;
	x-=9;
	y++;
	MousePos[11].x1 = x;     //foreground darkgray
	MousePos[11].y1 = y;
	MousePos[11].x2 = x+2;
	MousePos[11].y2 = y;
	x+=3;
	MousePos[12].x1 = x;     //foreground lightblue
	MousePos[12].y1 = y;
	MousePos[12].x2 = x+2;
	MousePos[12].y2 = y;
	x+=3;
	MousePos[13].x1 = x;     //foreground  lightgreen
	MousePos[13].y1 = y;
	MousePos[13].x2 = x+2;
	MousePos[13].y2 = y;
	x+=3;
	MousePos[14].x1 = x;     //foreground lightcyan
	MousePos[14].y1 = y;
	MousePos[14].x2 = x+2;
	MousePos[14].y2 = y;
	x-=9;
	y++;
	MousePos[15].x1 = x;     //foreground light red
	MousePos[15].y1 = y;
	MousePos[15].x2 = x+2;
	MousePos[15].y2 = y;
	x+=3;
	MousePos[16].x1 = x;     //foreground light magenta
	MousePos[16].y1 = y;
	MousePos[16].x2 = x+2;
	MousePos[16].y2 = y;
	x+=3;
	MousePos[17].x1 = x;     //foreground yellow
	MousePos[17].y1 = y;
	MousePos[17].x2 = x+2;
	MousePos[17].y2 = y;
	x+=3;
	MousePos[18].x1 = x;     //foreground white
	MousePos[18].y1 = y;
	MousePos[18].x2 = x+2;
	MousePos[18].y2 = y;

	x = rx1+19; y-=3;
	MousePos[19].x1 = x;     //background black
	MousePos[19].y1 = y;
	MousePos[19].x2 = x+2;
	MousePos[19].y2 = y;
	x+=3;
	MousePos[20].x1 = x;     //background blue
	MousePos[20].y1 = y;
	MousePos[20].x2 = x+2;
	MousePos[20].y2 = y;
	x+=3;
	MousePos[21].x1 = x;     //background green
	MousePos[21].y1 = y;
	MousePos[21].x2 = x+2;
	MousePos[21].y2 = y;
	x+=3;
	MousePos[22].x1 = x;     //background cyan
	MousePos[22].y1 = y;
	MousePos[22].x2 = x+2;
	MousePos[22].y2 = y;
	x-=9;
	y++;
	MousePos[23].x1 = x;     //background  red
	MousePos[23].y1 = y;
	MousePos[23].x2 = x+2;
	MousePos[23].y2 = y;
	x+=3;
	MousePos[24].x1 = x;     //background  magenta
	MousePos[24].y1 = y;
	MousePos[24].x2 = x+2;
	MousePos[24].y2 = y;
	x+=3;
	MousePos[25].x1 = x;     //background  brown
	MousePos[25].y1 = y;
	MousePos[25].x2 = x+2;
	MousePos[25].y2 = y;
	x+=3;
	MousePos[26].x1 = x;     //background lightgray
	MousePos[26].y1 = y;
	MousePos[26].x2 = x+2;
	MousePos[26].y2 = y;
	x-=9;
	y++;
	MousePos[27].x1 = x;     //background darkgray
	MousePos[27].y1 = y;
	MousePos[27].x2 = x+2;
	MousePos[27].y2 = y;
	x+=3;
	MousePos[28].x1 = x;     //background lightblue
	MousePos[28].y1 =  y;
	MousePos[28].x2 = x+2;
	MousePos[28].y2 = y;
	x+=3;
	MousePos[29].x1 = x;     //background  lightgreen
	MousePos[29].y1 = y;
	MousePos[29].x2 = x+2;
	MousePos[29].y2 = y;
	x+=3;
	MousePos[30].x1 = x;     //background lightcyan
	MousePos[30].y1 = y;
	MousePos[30].x2 = x+2;
	MousePos[30].y2 = y;
	x-=9;
	y++;
	MousePos[31].x1 = x;     //background light red
	MousePos[31].y1 = y;
	MousePos[31].x2 = x+2;
	MousePos[31].y2 = y;
	x+=3;
	MousePos[32].x1 = x;     //background light magenta
	MousePos[32].y1 = y;
	MousePos[32].x2 = x+2;
	MousePos[32].y2 = y;
	x+=3;
	MousePos[33].x1 = x;     //background yellow
	MousePos[33].y1 = y;
	MousePos[33].x2 = x+2;
	MousePos[33].y2 = y;
	x+=3;
	MousePos[34].x1 = x;     //background white
	MousePos[34].y1 = y;
	MousePos[34].x2 = x+2;
	MousePos[34].y2 = y;
	x = 23; y = 12;
	MousePos[35].x1 = x;     //Item Window border
	MousePos[35].y1 = y;
	MousePos[35].x2 = x+14;
	MousePos[35].y2 = y;

	MousePos[36].x1 = x;     //Item Menubar
	MousePos[36].y1 = y+1;
	MousePos[36].x2 = x+14;
	MousePos[36].y2 = y+1;

	MousePos[37].x1 = x;     //Item Normal text
	MousePos[37].y1 = y+2;
	MousePos[37].x2 = x+14;
	MousePos[37].y2 = y+2;

	MousePos[38].x1 = x;     //Item Status line
	MousePos[38].y1 = y+3;
	MousePos[38].x2 = x+14;
	MousePos[38].y2 = y+3;

	MousePos[39].x1 = x;     //Item Scroll bar
	MousePos[39].y1 = y+4;
	MousePos[39].x2 = x+14;
	MousePos[39].y2 = y+4;




		  //a->repeatflg =1;
			/*int ShowDialogBox(char *txtlines[], int numlines,
													int buttonsbits,
													int f_maxviewlines,
													int cursorstyle=1,
													int getout=0, int repeatflg=0);

		 */
		  a->ShowDialogBox(&pchar[0], 14, 7, 14,0, 1, 0);
		  SetCursor(a, 1);
		  //DispStatusLine(a);
	 gotoxy(26, 21);
top:
	fcrsrpos = gfc[item];
	bcrsrpos = gbc[item];
	ShowColours(a, fcrsrpos, bcrsrpos, rx1, ry1, item);

get:
	ch = a->WaitGetMouseOrKey();

	if(ch ==27) goto ex1;
	//a->ShowCoordinates();
	//goto get;
	switch(ch)
	{

			case 257:
				i = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 40);
				if(i ==-1) goto get;
				switch(i)
				{
					case 3:    //click occurred on a foreground colour
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
					case 16:
					case 17:
					case 18:
					if(mouse.ButtonState) //button pressed down
					{
					  a->WaitMouseButtonRelease(-1);
					  ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 40);
					  if(ii !=i) //not relesed on same button
						  goto get;
					  gfc[item] = i - 3;
					  goto top;
					}
					goto get;

					case 19:    //click occurred on a foreground colour
					case 20:
					case 21:
					case 22:
					case 23:
					case 24:
					case 25:
					case 26:
					case 27:
					case 28:
					case 29:
					case 30:
					case 31:
					case 32:
					case 33:
					case 34:
					if(mouse.ButtonState) //button pressed down
					{
					  a->WaitMouseButtonRelease(-1);
					  ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 40);
					  if(ii !=i) //not relesed on same button
						  goto get;
					  gbc[item] = i - 19;
					  goto top;
					}
					goto get;


					case 35:  //click occurred on item (ie window border)
					case 36:
					case 37:
					case 38:
					case 39:
					if(mouse.ButtonState) //button pressed down
					{
					  a->WaitMouseButtonRelease(-1);
					  ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 40);
					  if(ii !=i) //not relesed on same button
						  goto get;
					  item = i - 35;
					  goto top;
					}
					goto get;



					case 0:  //click on ok button
						if(mouse.ButtonState) //button pressed down
						{
							gotoxy(20+6, 21); //cursor on "ok" button
							a->m_buttonpressed =1;
							a->m_buttonscursorpos =0;
							a->ShowButtons();
							a->WaitMouseButtonRelease(-1);
							a->m_buttonpressed =0;
							a->ShowButtons();
							gotoxy(20+6, 21);
							ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 40);
							if(ii !=0) //not relesed on same button
							{
								goto get;
							}
							gbc[item] = bcrsrpos;
							gfc[item] = fcrsrpos;
							goto ex;
						}
						goto get;


					case 1:  //click on cancel
						if(mouse.ButtonState) //button pressed down
						{
							gotoxy(35+3, 21); //cursor on "cancel" button
							a->m_buttonpressed =2;
							a->m_buttonscursorpos =1;
							a->ShowButtons();
							a->WaitMouseButtonRelease(-1);
							a->m_buttonpressed =0;
							a->ShowButtons();
							gotoxy(35+3, 21);
							ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 40);
							if(ii !=1) //not relesed on same button
							{
								goto get;
							}
							  //restore the original global colours because
							 //the user cancels
ex1:						 for(t =0; t<5; t++){
								 gfc[t] = f_gfc[t];
								 gbc[t] = f_gbc[t];
							}
							goto ex;
						}
						goto get;



					case 2:  //click on help
						if(mouse.ButtonState) //button pressed down
						{
							gotoxy(49+3, 21); //cursor on "cancel" button
							a->m_buttonpressed =3;
							a->m_buttonscursorpos =2;
							a->ShowButtons();
							a->WaitMouseButtonRelease(-1);
							a->m_buttonpressed =0;
							a->ShowButtons();
							gotoxy(49+3, 21);
							ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 40);
							if(ii !=2) //not relesed on same button
							{
								goto get;
							}
							//Show the help dialog box
							ColoursHelp(&b);
							gotoxy(49+3, 21);

						}
						goto get;




				} //end inner switch


		default:
			goto get;

	}


ex:
		 a->CloseBox();
	return 0;
}
//----------------------------------------------------------------------


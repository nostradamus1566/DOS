//-------------------------------------------------------------------
// (c) Michael Finnegan   17 May 1999
//-------------------------------------------------------------------

#include "print.h"

//-------------------------------------------------------------------
//Find the last nonblank line in the text
//Exit:
// ax = r # of last non blank line or 1
int FindLastNonBlankLine(Disk *a)
{
	int rlines;

	rlines = a->GetLines();


	while(rlines >1)
	{
		a->GetLineBuf(linebuf1, &rlines);
		lenbuf1 = a->LeftTrim(linebuf1);
		if(!lenbuf1) //if its a blank line delete it
		{
			rlines--;
		}
		else
			break;
	}

	return rlines;
}

//-------------------------------------------------------------------
// Display a message for the user asking him to press a key before
// printing the next page.
int WaitPage(Disk *a, int p)
{
	int i;
	char buf[50];
	static char *pchar[4] = {"Printer Waiting...",
														 " ",
														 "Print page 1 ?          ",
														 " " };

		  sprintf(buf, "     Print Page %d ?", p);
		  strcpy(&pchar[2][0], buf);
		  a->CursorOn();
		  i = a->ShowDialogBox(&pchar[0], 3, 3, 3, 0);

	return i;
}

//-------------------------------------------------------------------
//for dot matrix printers
//Exit:
// ax = 1 if error
int NormalPrintOut(Disk *a)
{
	int shorterlines;
	FILE *out;
	char marginbuf[80];
	int i, page, topln, botln, off_set, actualpages;
	int j, t, actuallines, rlines;
	char ch;

	memset(marginbuf, ' ', lm+1);
	marginbuf[lm] =0;

	strcat(marginbuf,"%s\n\r");


	if ((out = fopen("LPT1", "wt")) == NULL)
	{
		fprintf(stderr, "Cannot open output file.\n");
		return 1;
	}

	lines = a->GetLines();
	shorterlines = FindLastNonBlankLine(a);
	page = 0;

	if(block.validsw)
	{
		if(block.stline > block.endline)
		{
			t = block.endline;
			block.endline = block.stline;
			block.stline = t;
			t = block.stcol;
			block.stcol = block.endcol;
			block.endcol =t;
		}

		if(block.stline == block.endline)
		{
			if(block.stcol > block.endcol)
			{
				t = block.stcol;
				block.stcol = block.endcol;
				block.endcol =t;
			}

		}

	} //end of if block valid
	else
	{  //block not valid so make block cover entire text in memory
		block.stline = 1;
		block.stcol =1;
		block.endline = lines;
		block.endcol = a->GetLineBuf(linebuf1, &block.endline);
	}
	//-----
	if(shorterlines < block.endline)
	{
		block.endline = shorterlines;
		block.endcol = a->GetLineBuf(linebuf1, &shorterlines);
	}
	if(block.stline > shorterlines)
	{
		block.stline = shorterlines;
		block.stcol =1;
	}
	rlines = (block.endline - block.stline)+1;
	off_set = block.stline -1;


	actualpages =  ((rlines -1) / lpp)+1;

	topln = 1;

top:
	page++;        //start at page 1
	botln = topln + (lpp-1);

	for( t = topln; t<= botln; t++){
		j = off_set + t;
		if(j > block.endline)
		{
			linebuf1[0] = 32;
			linebuf1[1] =0;
			lenbuf1 =1;
		}
		else
		{
			lenbuf1 = a->GetLineBuf(linebuf1, &j);
			ch = linebuf1[lenbuf1-1];
			if(ch == 13) linebuf1[lenbuf1-1] = 32;
		}


		fprintf(out, marginbuf, linebuf1);

	} //end for

	sprintf(linebuf2, "Page %d of %d", page, actualpages);
	lenbuf2 = strlen(linebuf2);
	t = 40 - (lenbuf2 /2);
	memset(linebuf1, ' ', t);
	linebuf1[t] =0;
	strcat(linebuf1, linebuf2);
	fprintf(out, "\n\r%s%c", linebuf1, 12);
	if(page < actualpages)
	{
		fclose(out);  //close printer channel
		i = WaitPage(a, page+1);   //wait for the user to press ok or cancle before
								//printing the next page
		if( i == 27) goto ex1;
		//reopen printer channel for next page
		if ((out = fopen("LPT1", "wt")) == NULL)
		{
			fprintf(stderr, "Cannot open output file.\n");
			return 1;
		}


		topln += lpp;
		goto top;
	}


ex:
	fclose(out);
ex1:
	return 0;
}
//-------------------------------------------------------------------
int PrintOut(Disk *a)
{

	NormalPrintOut(a);

	return 0;
}
//---------------------------------------------------------------------
int PrintText(Disk *a)
{
	int i, ii, iii, t, ch, lines, cursorfield;
	static char *pchar[8] = {"Print",
								 "Print to the printer on LPT1",
								 " ",
								 "( ) Selected Text Only",
								 "( ) Complete Document",
								 " ",
								 " ",
								 " "};


	FILE *out;
	TMousePos MousePos[4];

	MousePos[0].x1 = 25;
	MousePos[0].y1 = 11;
	MousePos[0].x2 = 46;
	MousePos[0].y2 = 11;

	MousePos[1].x1 = 25;
	MousePos[1].y1 = 12;
	MousePos[1].x2 = 46;
	MousePos[1].y2 = 12;

	MousePos[2].x1 = 25;
	MousePos[2].y1 = 16;
	MousePos[2].x2 = 32;
	MousePos[2].y2 = 16;

	MousePos[3].x1 = 44;
	MousePos[3].y1 = 16;
	MousePos[3].x2 = 53;
	MousePos[3].y2 = 16;



	lines = a->GetLines();
	if(block.validsw)
	{
		pchar[3][1] = 7;
		pchar[4][1] = 32;
	}
	else
	{
		pchar[3][1] = 32;
		pchar[4][1] = 7;
	}
	a->CursorOn();
	i = a->ShowDialogBox(&pchar[0], 5, 3, 5, 0, 1);

	if(pchar[3][1] ==7) cursorfield =0;
	else cursorfield =1;

get:
	switch(cursorfield)  //determine where to show the little cursor
	{
		case 0:
			gotoxy(MousePos[0].x1+2, MousePos[0].y1+1);
			a->m_buttonpressed =0;
			a->m_buttonscursorpos =0;
			a->ShowButtons();
			break;

		case 1:
		  gotoxy(MousePos[1].x1+2, MousePos[1].y1+1);
		  a->m_buttonpressed =0;
			a->m_buttonscursorpos =0;
			a->ShowButtons();
		  break;

		case 2:
		  gotoxy(MousePos[2].x1+4, MousePos[2].y1+1);
		  break;

		case 3:
		  gotoxy(MousePos[3].x1+3, MousePos[3].y1+1);
		  break;
	}





	i = a->WaitGetMouseOrKey();
	//a->ShowCoordinates();

	switch(i)
	{
		case 256:
			goto get;

		case 257:   //click
			ii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 4);
			if(i ==-1) goto get;

			switch(ii)  //a click occurred in a field
			{
				case 0: //clicked in "Selected Text Only"
					if(mouse.ButtonState) //button pressed down
					{
						cursorfield =0;
						a->ColourLine(MousePos[0].x1, MousePos[0].y1, 3, BLACK*16+LIGHTGRAY);
						a->WaitMouseButtonRelease(-1);
a1:				   pchar[3][1] = 7;
						pchar[4][1] =32;
a2:				   a->WriteLine(&pchar[3][0], MousePos[0].x1, MousePos[0].y1, -1, LIGHTGRAY*16+BLACK);
						a->WriteLine(&pchar[4][0], MousePos[1].x1, MousePos[1].y1, -1, LIGHTGRAY*16+BLACK);
						//if(pchar[3][1] ==7) gotoxy(MousePos[0].x1+2, MousePos[0].y1+1);
						//else  gotoxy(MousePos[1].x1+2, MousePos[1].y1+1);

						goto get;
					}
					//click in "Selected Text Only: button released
					//show as selected
					goto a1;

				case 1: //clicked in "Complete Document"
					if(mouse.ButtonState) //button pressed down
					{
						cursorfield =1;
						a->ColourLine(MousePos[1].x1, MousePos[1].y1, 3, BLACK*16+LIGHTGRAY);
						a->WaitMouseButtonRelease(-1);
b1:				   pchar[3][1] = 32;
						pchar[4][1] =7;
						goto a2;

					}
					//click in "Complete Document: button released
					//show as selected
					cursorfield =1;
					goto b1;

				case 2: //clicked in ok button
					if(mouse.ButtonState)  //button pressed
					{
						a->m_buttonpressed =1;
						a->m_buttonscursorpos =0;
						a->ShowButtons();
						gotoxy(MousePos[2].x1+4, MousePos[2].y1+1);
						a->WaitMouseButtonRelease(-1);
						a->m_buttonpressed =0;
						a->ShowButtons();
						gotoxy(MousePos[2].x1+4, MousePos[2].y1+1);

						iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 4);
						if(iii != 2)   //not released on button
						{
							cursorfield = 2;
							goto get;
						}
						else
							goto con;  //user chose "ok" button

					}
					//released on ok button
					goto get;


				case 3: //clicked in cancel button
					if(mouse.ButtonState)  //button pressed
					{
						a->m_buttonpressed =2;
						a->ShowButtons();
						gotoxy(MousePos[3].x1+3, MousePos[3].y1+1);
						a->WaitMouseButtonRelease(-1);
						a->m_buttonpressed =0;
						a->m_buttonscursorpos =1;
						a->ShowButtons();
						gotoxy(MousePos[3].x1+3, MousePos[3].y1+1);

						iii = a->WhichFieldIsMouseIn2(&MousePos[0], &mouse, 4);
						if(iii != 3)   //not released on button
						{
							cursorfield = 3;
							goto get;
						}
						else
							goto ex;  //user chose "cancel" button

					}
					//a click released on cancel button
					goto get;






				default:
					goto get;

			} //end of switch for a click occurred in a field


		case 9: //user pressed tab  goto next field
			if(cursorfield == 3)
				cursorfield =0;
			else
				cursorfield++;
			if(cursorfield == 3)
			{
				a->m_buttonpressed =0;
				a->m_buttonscursorpos =1;
				a->ShowButtons();

			}
			if(cursorfield==0) goto a1;
			if(cursorfield ==1) goto b1;
			goto get;


		case 13:
			if(cursorfield == 2) goto con;  //hit return on "ok" button
			if(cursorfield == 3) goto ex;   //hit return on "cancel" button
			goto get;

		default:
			goto get;
	}




con:      //user wants to print out something: either selected text or
			 //the complete document
	a->CloseBox();

	//TODO
	//must decide whether to print out complete document or selected text.
	//Must check if selected text valid, else cancle operation
	PrintOut(a);

//----------------
//user chose cancel
ex:
	a->CloseBox();
ex1:
	return 0;
}
//--------------------------------------------------------------------

//-----------------------------------------------------------------
// (c) Michael Finnegan     6 June 1999
//-----------------------------------------------------------------


#include "clip.h"
#include "labels.h"
#include "editor.h"
char linebuf3[170];
int lenbuf3;
int blocklines;
int lentop, lenbot;   //the length of the selected portion of the lines
							 //at the top and bottom of the block
//-------------------------------------------------------------------
//get the length of a text line in memory
//Entry:
//	line = r line #
//Exit:
// if ax = -1 error
// ax = the len of the line
// linebuf3 = the text of the line
int GetLineLength(Disk *a, int line)
{
	int cline;

	cline = line;
	lines = a->GetLines();
	if((cline < 1) || (cline > lines)) return -1;


	lenbuf3 = a->GetLineBuf(linebuf3, &cline);
	return lenbuf3;
}

//-------------------------------------------------------------------
//This function calculates the r # of bytes before the beginning of the
//selection.
//This number will be 0 or more
//Entry:
//	block.stline, block.stcol mark the beginning of the selection
//
long unsigned int CalcPrevBytes(Disk *a)
{
	long unsigned int ubytes;
	int cline, t;

	ubytes =0;

	lines =  a->GetLines();
	cline = 1;
	for(cline= 1; cline<= block.stline; cline++){
		ubytes += GetLineLength(a, cline);
	}

	ubytes-= lenbuf3 -(block.stcol-1);


	return ubytes;
}
//------------------------------------------------------------------
//Give this function the position in bytes a particular
//char is and it will return the text line and column
//The byte value must be between 1 to 78 * 2,000
int GetLineColumn(Disk *a, int *l, int *c, long unsigned int bytes)
{
	int cline, len, col;
	long unsigned int count;

	if(bytes == 0) return -1;
	lines = a->GetLines();
	cline = 1;
	count =0;

top:
	len = GetLineLength(a, cline);
	count += len;
	if(count < bytes)
	{
		if(cline == lines) return -1; //that many bytes not in text
		cline++;
		goto top;

	}

	count-= len;
	col = bytes - count;

	*l = cline;
	*c = col;
	return 0;
}
//-------------------------------------------------------------------
//Exit:
// ax =0 if block vars not valid
int CheckBlockVars(Disk *a)
{
	int xc, xl;
	//see if block vars need to be turned up-side-down
	if(block.stline > block.endline)
	{
		xl = block.stline;
		xc = block.stcol;
		block.stline = block.endline;
		block.stcol = block.endcol;
		block.endline = xl;
		block.endcol = xc;
	}
	else  //the stline is not greater than endline, but if they are the same
	{     //and the stcol is grater than endcol swap their cols
		if(block.stline == block.endline)
		{
			if(block.stcol > block.endcol)
			{
				xc = block.stcol;
				block.stcol = block.endcol;
				block.endcol = xc;
			}
		}
	}

	//make sure the end values are with the legal range
	if(block.endline >lines) block.endline = lines;
	lenbuf1 = a->GetLineBuf(linebuf1, &block.endline);
	if(block.endcol >lenbuf1) block.endcol = lenbuf1;

	blocklines = (block.endline - block.stline)+1; //r # of lines in block

	if(block.validsw ==2) return 1; //block vars are valid

	if(block.endcol ==1)  //set end col to end of previous line
	{
		blocklines--;
		if(!blocklines) return 0; //no lines
		block.endline--;
		block.endcol = GetLineLength(a, block.endline);
	}
	else
	{
		block.endcol--;
	}

	return 1; //block vars valid
}
//-------------------------------------------------------------------


int CopyToClip(Disk *a, int clear)
{
	int flg, t, xc, xl, i, handle, access, createflg, actualbytes;
	char *p;

	lentop =0;
	lenbot =0;
	flg =0; //job not done yet

	lines = a->GetLines();

	if(!block.validsw) return 0;
		strcpy(linebuf1, "c:\\clipbrd.txt");
	access =1;  //writing access
	createflg =1;
	if(!clear)
	{
		i=a->OpenFile(linebuf1, access, &handle, createflg);
		if(i)
		{
			gotoxy(2, 3);
			printf(" << Error opening to write to clipboard file >>\n ");
			return 0;
		}
	}
	if(!CheckBlockVars(a)) goto ex;

	//get the first block line to linebuf1
	GetLineLength(a, block.stline); //get full length of 1st line
	p = &linebuf3[block.stcol-1];
	if(blocklines ==1)
	{
		linebuf3[block.endcol] =0;

	}
	strcpy(linebuf1, p);
	lentop = strlen(linebuf1); //get the selected len of the top line
	if(!clear)
	{
		i = a->WriteFile(handle, linebuf1);
		if(i)
		{
			printf("Problem writing to clipboard file\n");
			return 0;
		}
	}
	flg =1; //job done
	if(blocklines <2) goto ex; //only 1 line in the block, we just copied it
										//to the clipbrd.txt file.
	if(blocklines >2)
	{
		//write all the full lines in the block
		for(t = 2; t< blocklines; t++){
			i = (block.stline-1)+t;
			lenbuf1 = a->GetLineBuf(linebuf1, &i);
			if(!clear) a->WriteFile(handle, linebuf1);
		}
	}
	//write the last line
	lenbuf1 = a->GetLineBuf(linebuf1, &block.endline);
	linebuf1[block.endcol] =0;
	lenbot = block.endcol;      //get the r len of the selected bottom line
	if(!clear) a->WriteFile(handle, linebuf1);


ex:
	if(!clear) i = a->CloseFile(handle);
	block.validsw =0;
	int topline;
	topline = (cline - row)+1;
	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);


	SetCursor(a, 1);
	DispStatusLine(a);
	return flg;  //flg: job done = 1, 0 = not done
}
//--------------------------------------------------------------------
//cut the selected text from text area, but don't copy to clipboard
int ClearCut(Disk *a)
{
	return CutToClip(a, 1);
}
//--------------------------------------------------------------------
int CutToClip(Disk *a, int clear)
{
	int numlines, topline, cutlen, line, t, i, flg, oldlines;
	char *p1, *p2;
	int orgtopline;
	long unsigned int ub;

	numlines = 1;    //for line deleting
	flg =0;
	oldlines = lines;
	//goto ex;

	orgtopline = cline - (row-1);

	i = CopyToClip(a, clear);
	if(!i) return 0;  //block not valid


	//blocklines = (block.endline - block.stline) +1;

	lenbuf1 = a->GetLineBuf(linebuf1, &block.stline);
	lenbuf2 = a->GetLineBuf(linebuf2, &block.endline);
	lenbuf1 = a->DelChars(linebuf1, block.stcol-1, lentop);
	if(block.endcol >= lenbuf2)
	{
		linebuf2[0] =0;
		lenbuf2 = 0;
	}
	else
	{
		lenbuf2 = a->DelChars(linebuf2, 0, block.endcol);
	}

	//if the whole of the top line was selected linebuf1 is now empty
	//if only some of the top line was selected linebuf1 contains the
	//remainder
	//if the whole of the bottom line was selected linebuf2 is now empty
	//else it contains the remainder of the bottom line.

	//if only 1 line was selected for cutting, if there is any chars
	//remaining in it overwrite the original version with it and exit
	//if only 1 line was selected and it was the whole of the top
	//line the delete that line and exit
	if(blocklines == 1)
	{
		if(lenbuf1)
			a->OverWriteLine(linebuf1, &block.stline);
		else
			a->DeleteLines(&block.stline, &numlines);
		goto ex;
	}

	//if there are two line in the selection delete them if they were
	//entirely selected and exit, else store what remains of eith or
	//both by overwriting the originals and exit.

	if(blocklines ==2)
	{
		//overwrite or delete the bottom line first
		if(lenbuf2)
			a->OverWriteLine(linebuf2, &block.endline);
		else
			a->DeleteLines(&block.endline, &numlines);

		//now overwrite or delete the top line
		if(lenbuf1)
			a->OverWriteLine(linebuf1, &block.stline);
		else
			a->DeleteLines(&block.stline, &numlines);
		goto ex;
	}

	//At this stage we know that there are 3 or more lines in the
	//selected block. We havent deleted any lines in memory yet.
	//Assume the number of lines to delete is = to blocklines
	//if the new version of the top of the block is not empty then
	//write it into memory
	//if the new version of the end of the block is not empty
	//write it into memory.

	if(lenbuf2)
		a->OverWriteLine(linebuf2, &block.endline);

	if(lenbuf1)
		a->OverWriteLine(linebuf1, &block.stline);

	//now for the lines deleting:
	//if the new bottom is empty delete line at block.endline
	//decrement blocklines
	//if blocklines is not 1 decrement next line from bottom and so on
	//when blocklines =1 check the new version of top block
	//if empty delete line at block.stline, then goto ex
	t = block.endline-1;
	if(!lenbuf2)
	{
		a->DeleteLines(&block.endline, &numlines);

	}

	while(t > block.stline)
	{
		a->DeleteLines(&t, &numlines);
		t--;
	}
	//if the top select line is empty delete it
	if(!lenbuf1)
		a->DeleteLines(&block.stline, &numlines);

ex:

	lines = a->GetLines();
	i =1;

	t =block.endline;
	if(t>lines) t = lines;
	ParaWrap(a, &t, &i);
	t =block.endline+1;
	if(t>lines) t = lines;
	ParaWrap(a, &t, &i);
	t =block.stline;
	if(t>lines) t = lines;
	ParaWrap(a, &t, &i);

disp:
	lines = a->GetLines();

	ub = CalcPrevBytes(a);

	ub++;
	i = GetLineColumn(a, &cline, &col, ub);
	if((block.stline - cline) > 1)
	{
		cline = block.stline -1;
		col =	 GetLineLength(a, cline);

	}

	//is the position of the cursor new line currently within
	//the lines being displayed?
	if((cline >= orgtopline) && (cline <= (orgtopline+21)))
	{
		row = (cline - orgtopline)+1;
		topline = orgtopline;
	 }
	 else
	 {
		row = 1; topline = cline;
	 }

	if((topline+21) > lines)
	{
		topline = lines - 21;
		if(topline < 1) topline = 1;

	}

	row = (cline - topline)+1;
	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);


	SetCursor(a, 1);
	DispStatusLine(a);

	return flg; //flg: job don =1, 0 = not done
}
//--------------------------------------------------------------------
//Paste the contents of the clipboard file starting from line
//Cline, and at column col
//Exit:
//	ax = -1 if error

int PasteClip(Disk *a)
{
	FILE *fp;
	long int file_pos, offset;
	int topline, t, topblkline, botblkline, tempbuflen, ccline, chr;
	long unsigned int ub;
	char tempbuf[170];

	ccline = cline;
	lines = a->GetLines();
	if(cline > lines) cline = lines;
	tempbuflen = a->GetLineBuf(tempbuf, &cline);
	if(col > tempbuflen) col = tempbuflen;
	if(col == 1)
	{
		tempbuflen =0;
		goto a1;     //no need to split line
	}
	//split the line. The left half is from the start to the char just before
	//the cursor, the right half is from the char at the cursor to the end
	strcpy(linebuf1, &tempbuf[col-1]);
	a->OverWriteLine(linebuf1, &cline); //the new shorter right half

a1:
	if((fp = fopen("C:\\CLIPBRD.TXT", "rb")) == NULL)
	{
		printf("<< Cannot open clipboard file >>\n");
		return -1;
	}
	rewind(fp);
	lenbuf1 =0;
top:
	chr = fgetc(fp);
	if(chr == 0) goto top;
	if(chr == -1) goto ex;
	lenbuf1++;
	linebuf1[lenbuf1-1] = chr;
	if(chr == 13)
	{ t=0;goto  b1; }
	if(lenbuf1 == maxlen) goto  b2;
	goto top;
//a rtn was found. lenbuf1 is 78 or less so we store the full line
b1: 	linebuf1[lenbuf1] =0;
		a->InsertLine(linebuf1, &ccline);
		ccline++;
		lenbuf1 =0;
		goto top;


//A full line was read in 78 chars. There is no return at the end.
//if the 78th char is a space its ok, else scan back to fine the space
b2:
	linebuf1[lenbuf1] =0;
	if(chr == 32) goto b1;
	PerformWrap();
	if(lenbuf2)  //throw linebuf2 back
	{
		file_pos = 0- lenbuf2;
		fseek(fp, file_pos, SEEK_CUR);
		lenbuf2 =0;
	}
	goto b1;


ex:
	fclose(fp);
	//if there is any remainder save it
	if(lenbuf1)
	{
		linebuf1[lenbuf1] =0;
		a->InsertLine(linebuf1, &ccline);
		ccline++;
		lenbuf1 =0;
	}
	botblkline = ccline;
	ccline = cline;
	if(tempbuflen)   //prefix the org left half of the top of the blk
	{                //to the first line that was pasted
		tempbuf[col-1] =0;
		lenbuf1 = a->GetLineBuf(linebuf1, &ccline);
		strcat(tempbuf, linebuf1);
		strcpy(linebuf1, tempbuf);
		PerformWrap();
		a->OverWriteLine(linebuf1, &ccline);
		if(lenbuf2)
		{
			ccline++;
			a->InsertLine(linebuf2, &ccline);
			ccline++;
		}
	}

	t =1;

	lines = a->GetLines();
	if(botblkline >lines) botblkline = lines;
	ParaWrap(a, &botblkline, &t);
	t =1;
	ccline = cline;
	ParaWrap(a, &ccline, &t);

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

	topline = cline -(row-1);
	//---------------
	ub = CalcPrevBytes(a);
	ub++;
	t = GetLineColumn(a, &cline, &col, ub);


	//is the position of the cursor new line currently within
	//the lines being displayed?
	if((cline >= topline) && (cline <= (topline+21)))
	{
		row = (cline - topline)+1;
	}
	 else
	 {
		row = 1; topline = cline;
	 }


	row = (cline - topline)+1;
	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);


	SetCursor(a, 1);
	DispStatusLine(a);

	//---------------






	DispLines(a, topline, 22, 1, gbc[2]*16+gfc[2]);
	SetCursor(a, 1);
	DispStatusLine(a);

	return 1;    //job done
}
//----------------------------------------------------------------------


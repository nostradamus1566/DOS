/*-------------------------------------------------------------------------
	(C) Michael Finnegan     3 June 1999

-------------------------------------------------------------------------*/

#include "file.h"
#include "editor.h"
#include "labels.h"
#include <stdlib.h>
//-----------------------------------------------------------------------
void OpenHelp(Disk *a, int rw)
{

  static char *ochar[20] =
	 {"File Open",
	  " Opens an existing file or creates a new named filed.",
	  " ",
	  " . You can use the Files and Directories boxes to select the ",
	  "   path name for the file. Or you can type the path name in the ",
	  "   File Name box.",
	  " ",
	  " . If the file doesn't exist, a new empty file is created with ",
	  "   the specified path name.",
	  " ",
	  " . If you want to reduce the left margin increase the Line Width,",
	  "   and to increase the left margin reduce the Line Width.",
	  "   To remove the margin altogeth set the Line Width to 78.",
	  " "
	  };
	  ochar[3][1] =4;
	  ochar[7][1] =4;
	  ochar[10][1] =4;



  static char *schar[20] =
	  {"File Save As",
		" Saves and names the file you are working on.",
		" ",
		" . Use this to name a new unnamed file, or to rename an existing ",
		"   file. Use the File Save Command to save an existing file ",
		"   without renaming it.",
		" ",
		" . Enter the path name in the File Name box. Or you can use ",
		"   the Directories and Existing Files boxes to help you select a ",
		"   different filename, or drive.",
		" ",
		" . If there's already an existing file with the same name, then ",
		"   Swift asks you if you're sure you want to replace it with this ",
		"   file.",
		" ",
		" . Uncheck the Word Wrap option if you want to save the file with ",
		"   line breaks at the end of each line.",
		" "
		};
		schar[3][1] =4;
		schar[7][1] =4;
		schar[11][1] =4;
		schar[15][1] =4;
		  a->CursorOn();
		  if(!rw)
			  a->ShowDialogBox(&ochar[0], 13, 1, 13, 0);
		  else
			  a->ShowDialogBox(&schar[0], 16, 1, 16, 0);


}
//-----------------------------------------------------------------------




//-----------------------------------------------------------------------
//Get a char from either the buffer or refill the buffer from the file
//if there are more chars in the file to read in, then get the
//first char from the newly refilled buffer.
//Entry:
//  clear m_eof & m_buflen if you are reading for file for first time,
//  else leave alone.
//Exit:
// when m_pastchar becomes 1 then you've read past the end of the file.
//  When the end of the file is reached m_eof will be set.


int File::GetChar(Disk *a, int handle)
{
	int t, ch, actualbytes;

	m_pastchar =0; //assume not last char yet
top:
	if(m_buflen)  //if buf not empty get 1st char and shift remaining
	{				  //chars left to take its place

		ch = m_buffer[0];
		if(m_buflen > 1)
		{
			for(t = 0; t < m_buflen; t++){  //shift left the rest
				m_buffer[t] = m_buffer[t+1];
			}
		}

		m_buflen--;

		return ch;

	}
	else   //buffer enpty, try to refill from file
	{
		if(m_eof)   //file finished
		{
			m_pastchar =1; //flag to call to indicate that past char was reached
			return 0; //end of file reached
		}

		a->ReadFile(handle, maxlen, m_buffer, &actualbytes);
		if(!actualbytes)  //if nothing read in
		{
			m_eof =1;  //set flg: no more in file
			m_pastchar =1;
			return 0;
		}

		m_buflen = actualbytes;
		goto top;  //get char from buffer
	}


}
//---------------------------------------------------------------------
//return some characters to the beginning of buffer, pushing the ones
//already in the buffer right.
//Entry:
// string = the adr of the chars to add
// num = the number of chars to add
int File::AddBuf(char *string, int num)
{
	int t;


	if(!num)
		return 0;
	if((num + m_buflen) >84)
	num = num;
	//if there are already chars in the buffer, shift them right to make
	//rome for the ones we are going to add to the beginning.

	if(m_buflen)
	{
		for(t = m_buflen-1; t>=0; t--){
			m_buffer[t+num] = m_buffer[t];
		}

	}

	for(t =0; t<num; t++){
	  m_buffer[t] = string[t];
	}

	m_buflen += num;

	m_pastchar =0;  //not past end of file yet
	return m_buflen;
}
//----------------------------------------------------------------------

//The user clicked on a file name. It was checked and found to be valid
//load it into memory
int File::LoadFile(Disk *a, char *name)
{

	char linebuf1[162], linebuf2[162], *p;
	char spcstr[86];
	char c;
	int oldcline;
	int spcs, nxttab, cline, lenbuf1, actualbytes, t, i, j, handle, ch;
	int prevch, pretied, start, times;

	times =0;
	start =0;
	pretied =0;
	strcpy(spcstr, "                                        ");
	strcat(spcstr, "                                        ");

	i = a->OpenFile(name, 0, &handle, 0);
	if(i) return i;  //error encountered    perhaps file not found
	//code to read in the file goes here
	//--------
	cline =0;     //haven't started loading lines yet
	m_eof =0;
	m_buflen =0;
	lenbuf1 =0;
	a->InitMem();     //wipe lines memory
	linebuf1[0] =0;   //empty str

	ch =0;
top:
	prevch = ch;
	ch =this->GetChar(a, handle);
	if(!m_pastchar)            //not past eof yet
	{
		 if(ch == 10)   //goto top; //ignore linefeed
		 {
			 if(prevch==13)
				 goto top; //we got a rtn, so ignore the linefeed

			 //we got a linefeed, but without a rtn: change it to rtn
		 }

		 if(ch == -1) goto top;
		 if(ch == 0) goto top;
		 if((ch ==1) && (start ==0))
		 {
			 pretied =1;
			 start =1;
		 }
		 if(ch == 1) goto top; //ignore because this code is used by wordwrap
										//as a line & column position tracker
		 if(ch != 9)  goto c1;  //not a tab
		 //---------------
		 //try to append spaces to represent the position value of the tab
		 j =1;
		 do{
				nxttab = (j * tablen);
				j++;
		 } while(nxttab <= lenbuf1);

		 spcs = nxttab - lenbuf1;  //r # of spaces to append
		 p = &spcstr[80 - spcs];
		 if(!lenbuf1)
		 {
			strcpy(linebuf1, p);
		 }
		 else
		 {
			 strcat(linebuf1, p);
		 }
		 lenbuf1 = strlen(linebuf1);
		 if(lenbuf1 < maxlen) goto top;
		 if( lenbuf1 == maxlen) goto storeln;
		 strcpy(linebuf2, linebuf1);
		 j = lenbuf1 - maxlen;    //j = r # of chars left over
		 i = a->RightFindChar(linebuf1, 32, maxlen-1);

		 if(!i) i = maxlen;  //no space on line (can't wrap )
		 p = &linebuf2[i];
		 linebuf1[i] =0;
		 this->AddBuf(p, strlen(p));  //put left over bit back into buffer
		 goto storeln;

		 //end of trying to append spaces to represent tab osition values
		 //---------------
c1:    //it wasn't a tab
		 linebuf1[lenbuf1] = ch;
		 lenbuf1++;
		 linebuf1[lenbuf1] =0;
		 if(ch == 13 || ch ==10)
		 {
			linebuf1[lenbuf1-1] =13;
		 }

		 if(ch == 13 || ch ==10) goto a1; //end of line
		 if(lenbuf1 == maxlen) goto a1; //wrap line
		 goto top;

a1:
		 if(m_wordwrap != 1)
			goto storeln;
		 if(ch == 13 || ch ==10 )
			goto storeln;


		 if(ch == 32)   goto storeln;
		 i = a->RightFindChar(linebuf1, 32, maxlen-2);
		 if(!i) goto storeln;  //no space on line (can't wrap )
		 strcpy(linebuf2, linebuf1);
		 p = &linebuf2[i];
		 linebuf1[i] =0;
		 this->AddBuf(p, strlen(p));  //put left over bit back into buffer

storeln:
		  cline++;
		  if(cline >2000) goto ex;
		  if(cline ==1)
		  {
			 a->OverWriteLine(linebuf1, &cline);
			 a->m_lines = cline;
		  }
		  else
		  {
				a->AppendLine(linebuf1);
				a->m_lines = cline;

		  }
		lenbuf1 =0;
	}
	if(!m_pastchar) goto top;

ex:

	a->CloseFile(handle);

	//If the the wordwrap flag is set remove unnecessary returns from
	//the end of each line.
	//if the end of a line contains a rtn, and there is a line below it
	//if the line below is not blank remove the return from the current line
	lines = a->GetLines();
	if(lines ==1) goto rex;
	if(m_wordwrap && pretied ==0)
	{
d2:	j = lines -1;
		for(t =1; t<=j; t++){
			lenbuf1 = a->GetLineBuf(linebuf1, &t);
			c = linebuf1[lenbuf1-1];
			if(c != 13) continue;
			i = t+1;

			lenbuf2 = a->GetLineBuf(linebuf2, &i);
			if(!a->IsBlank(linebuf2))  //if the line below is not blank
			{
				if(a->IsBlank(linebuf1)) continue; //current line is blank so keep rtn

				//there is more than 1  char on the current line
				//if the second last char is not a space convert the rtn to a spc
				//else delete the rtn
				c = linebuf1[lenbuf1-2];
				if(c != 32)
					linebuf1[lenbuf1-1] =32;
				else
					linebuf1[lenbuf1-1] =0;

				a->OverWriteLine(linebuf1, &t);
			}
		}  //end for loop

		//wrap all the lines
		cline =lines;
d1:
		col =1;
		oldcline = cline;
		i =ParaWrap(a, &cline, &col);
		lines = a->GetLines();
		if(cline >1)
		{
			cline = oldcline-1;
			goto d1;
		}
		times++;
		if(times <2) goto d2;
	} //end if(wordwrap is on)
rex:
	return 0;
}

//------------------------------------------------------------------------
//This function is called when the user presses <return> on the input line
// of the open dialog box
// See if the user typed in a path name or a full file name
// 1} if it is a full file name  see if there is a path to do to
//    then check to see if the file name really exists in among the list
//    if files in that path.
//    If so retur 0 and the filename in filenamebuf
//    if there is a full name, but it does not exist display the
//    appropriate diagog error message and return 1
// 2) if there is a path but no file name attempt to set the path
//    if unsuccessful display error message and return 1
//Exit:
//  if ax =0 successful name entered.
//  if ax = 1 caller need not redisplay the org list of files and dirs
//     again, just goto into the input function again
//  if ax = 2 caller must have original files and dirs redisplayed
//  and focus set in inpit field and input function called
int File::EvalPathOrName(Disk *a, char *filenamebuf)
{

	int handle, i, ii, j, specflg, flags, t, found, inputlen;
	char specstr[128], ch1, ch2;
	char errbuf[128];
	char orgpath[128];
	char orgmask[128];
	char drive[128], dir[128], file[128], ext[128];
	char *pchar[6];
	char *p;
	Disk b;     //temp object for dialog box
	Disk c;
	pchar[0] = "Error";

	//set up error string in advance in case there was an error
	inputlen = strlen(inputbuf);

	pchar[1] = "Swift was unable to find the path";
	errbuf[0] = 39;         //'
	if( inputlen < 50)
	{
		strcpy(&errbuf[1], inputbuf);
		errbuf[inputlen+1] = 39;
		errbuf[inputlen+2] = 0;
	}
	else
	{
		strcpy(&errbuf[1], "...");
		strncpy(&errbuf[4], inputbuf, 50);
		strcpy(&errbuf[54], "...");
		errbuf[57] = 39;
		errbuf[58] =0;
	}
	pchar[2] = &errbuf[0];
	pchar[3] = "";


	//make a backup copy of the current path
	strcpy(orgpath, m_path);
	strcpy(orgmask, m_filemask);
	//split the input string into the components of drive, dir, filename & ext
	flags = fnsplit(inputbuf, drive, dir, file, ext);
	if(!(flags & DRIVE))
	{                       //no drive letter specified, use default drive

		strcpy(drive, m_path);
	}
	else
	{     //see if drive letter is with range
		found =0;              //not found yet
		drive[0] = drive[0] & 95;     //make sure it uppercase  65-90

		for(t =0; t< a->m_numdevs; t++){
			if(a->m_devslist[t] == drive[0]) {found =1; break;}
		}
		if(!found)
		{
a1:    	b.ShowDialogBox(&pchar[0], 3, 1,3,0);
			return 1;          //error: caller must go back to input
									 //but not redisp the org files & dirs
		}
	} //end else

	//we have got a valid drive or we are using the default one
	specstr[0] =0;         //clr specstr
	if((flags & FILENAME) ==FILENAME)
		strcpy(specstr, file);
	if((flags & EXTENSION) ==EXTENSION)
		strcat(specstr, ext);
	//if specstr is empty it means there were no file and no extension
	//os use the org mask
	specflg =0;            //assume we are treating specstr as a filename
	if(!specstr[0])
	{  //since specstr is empty we must use the default mask and treat
		strcpy(specstr, orgmask);  //specstr as a mask
		specflg =1;                //therefore we set its flg



		goto z2; //skip over (the test to determine what specstr is filename
					//or mask, because we have just made it a mask, because it was
					//empty.)
	}

	//we know that specstr is not empty but we still don't know if it is
	//a file name or a mask. scan for a '*'. if found then specstr is a mask
	//else its a filename
	p = strchr(specstr, '*');
	if(p) specflg =1;    //treat specstr as a spec string for dirs
	else
	{
		specflg =0;     // assume specstr is a file name
		if(!(flags & FILENAME))
		{                  //no file name so copy  org mask and treat
			strcpy(specstr, orgmask); //specstr as a file spec
				specflg = 1;
		}

	}

z2:
	//if specstr is really a spec string and not a file name copy
	//it into m_filemask
	if(specflg)
		strcpy(m_filemask, specstr);


	//if there is a directory add it to the drive string
	if((flags & DIRECTORY))
	{
		i = strlen(drive);
		ch1 = drive[i-1];      //is the last of drive char '\'?
		ch2 = dir[0];          //is the first char or dir '\'?
		if(ch1 == 92 && ch2 == 92 ) dir[i-1] =0; //2 '\' del one of them
		strcat(drive, dir);
	}
	else
	{  //since there is no directory we must see if the new drive is
		//the same as the default one. If so we use the current default
		//directory path
		if(drive[0] == m_path[0])
		{
			strcpy(drive, m_path);
			if(drive[0] != 65) goto m1;
		}


	//the new drive is different from the old one and the user
	//specified no directory so we must set the drive first
	//then get the current path of the new drive

		i = drive[0];
		if( i == 'A')
		{

top:  	j = a->IsDriveReady(0);  //check if drive a is readable
			if(j==2)
			{
				pchar[1] = "Drive A: needs a formatted disk.";
				pchar[2] = "Insert the formatted disk in the drive first,";
				pchar[3] = "then click on the OK button.";
				pchar[4] = "Or insert a new disk, shell out to DOS,";
				pchar[5] = "then format it";
				ii= c.ShowDialogBox(&pchar[0], 5, 3, 5,0);
				if(ii ==27)
				{
					//restore original path
					strcpy(m_path, orgpath);
					strcpy(m_filemask, orgmask);

					return 1;
				}
				goto top;

			}
		}
		asm{
				push si
				push di
				mov dx, i
				sub dl, 65
				mov dh, 0
				mov ax, 0e00h     //"Change drive"
				int 21h
				pop di
				pop si
			}


			_getcurrentpath();
			strcpy(m_path, currentpath);
			goto z4;  //the path is already set


	} //end else


	strcpy(m_path, drive);
	//make sure there an '\' at the end of the path, if not append it
m1:
	i = strlen(m_path);
	if(m_path[i-1] !=  92)    // '\'
	{
		m_path[i] = 92;
		m_path[i+1] =0;
	}

	i = _set_path(m_path);
	if(!i)                  //error
	{
		//an error occurred while attempting to set the users path
		//set the original path, display the appropriate error message
		//and exit with ax =1, which means caller need not redisp the
		//orginal list, just reenter the input function
		strcpy(m_path, orgpath);
		i = _set_path(m_path);       //reset the original path
		strcpy(m_filemask, orgmask);
		goto a1;                    //disp error msg and exit
	}

z4:

	if(!(strlen(specstr))) strcpy(specstr, "*.*");

	if((specflg ==0) && ((flags & FILENAME) >0 ))
	{
		//attempt to open the file as a test to see if it really exists
		//i = a->OpenFile(specstr, 0, &handle, 0);
		p = strrchr(inputbuf, 92);    //find last occurrance of "\"
		if(p) strcpy(specstr, p+1);   //only copy the name part
		else strcpy(specstr, inputbuf);  //no "/" so copy the full name
		i = a->OpenFile(specstr, 0, &handle, 0);

		a->CloseFile(handle);
		if(i)
		{
			if(!m_rw)  //loading
			{
				pchar[1] = "Swift was unable to find the file";
				b.ShowDialogBox(&pchar[0], 3, 1,3,0);
				return 1;          //error: caller must go back to input

			}


			//we havenot found a file of the same name as the user entered
			//we are saving so we are ok
			strcpy(filenamebuf, specstr);
			return 0;
		}

		//we have successfully opened and closed the uers file: it exists
		//but if we are saving we must warn the user
		if(!m_rw) //loading
		{
			strcpy(filenamebuf, specstr);
			return 0;      //report no error
		}
		//we are save, file exists, ask user if ok to replace
		pchar[1] = "Replace existing file?";
		pchar[0] = "Swift";
		i = b.ShowDialogBox(&pchar[0], 3, 3,3,0);

		if( i == 13)  //replace existing file
		{
			strcpy(filenamebuf, specstr);
			return 0;      //report no error

		}
		return 1;  //dont replace existing file


	}

	//specstr was a mask. copy it to m_filemask and inform the caller to
	//display the dirs and files for the new path and mask
	if(strlen(specstr))
	{
		strcpy(m_filemask, specstr);

	}

	return 2;         //report: caller must reload files and dirs

}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
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

void File::ShowInputLine(Disk *a, char *buf, int cursorstyle)
{
	int x1, y1, len, stpos, displen;
	int color[2], selectstyle;
	char dotbuf[2];

	color[1] = BLACK*16+LIGHTGRAY;    //black selected
	color[0] = LIGHTGRAY*16+BLACK;    //black unselected

	selectstyle = 0;                  //assume black unselected
	if(cursorstyle == 2 || cursorstyle == 3) selectstyle =1;


	len = strlen(buf);
	if(!len)
	{
		if(!m_rw)
			strcpy(buf, filemask);       //filemask is global
		else
			buf[0] =0;

		len = strlen(buf);
	}

	stpos =0;
	if(len> 31) stpos = len-31;       //the c pos of the first char to disp

	displen = len - stpos;  //the r len of the text to disp

	x1 = 15; y1 =1;
	if(len)
		a->WriteLine(&buf[stpos], x1+14, y1+2, 32, color[0], 1);
	else
		a->WriteLine(".", x1+14, y1+2, 32, color[0], 1);

	if(len)
		a->WriteLine(&buf[stpos], x1+14, y1+2, displen, color[selectstyle], 1);

	//since we didn't write to the last char position on the line
	//it may contain rubbish from a line length of 32 or greater
	//so show the last char as a normal dot. The cursor will
	//be positioned on this dot if the len was 32 or greater
	dotbuf[0] = '.';
	a->WriteLine(dotbuf, x1+14+31, y1+2, 1, color[0], 1);

	//turn the small cursor on or off (depending on cursorstyle)
	gotoxy(1+x1+14+displen, 1+y1+2);
	if(cursorstyle == 1 || cursorstyle == 3) a->CursorOn();
	else a->CursorOff();

}
//------------------------------------------------------------------------
//Called from OpenJunior
//This function turns on or off the black select in the files listbox
//and turns on and positions the small cursor on the current item
//it the files listbox
//Entry:
// if cursorstyle =0 turn black select off
//                >0 turn black select on

void File::FocusFilesList(int cursorstyle)
{
	int c, x, y, width;

	width = (m_f.m_x2 - m_f.m_x1) -1;

	c = 0;
	if(cursorstyle) c =1;

	x = m_MousePos[1].x1+2;
	y = m_MousePos[1].y1+1;
	if(m_f.m_numitems) y += m_f.m_currentitem -m_f.m_topitem;


	gotoxy(x, y);
	m_f.CursorOn();
	if(m_numfiles)
	{
		_fetch_stored_entry(tempbuf, m_f.m_currentitem +m_numdirs);
		strcpy(&filebuf[1], &tempbuf[9]);
		filebuf[0] =32;
		m_f.WriteLine(filebuf, x-2, y-1, width, m_colour[c]);
	}
}
//------------------------------------------------------------------------
//allow the user to enter a new line width
//Entry:
//  chr2 = the first char to enter into the inputline
//Exit:
// ax =13  <return>
//     27  <esc>
//      9  <tab>
//     30  <up>
//     31  <down>
//     257  <click>
//     258  <double click>
int File::InputLineWidth(Disk *a)
{
	int i, n, icursorpos;
	static char keys[5] = {13, 27, 9, 30, 31};

	strcpy(m_linewidthbuf, "   ");

	icursorpos = 0;
	i = a->Input(&m_MousePos[0], 14, m_linewidthbuf, m_MousePos[4].x1+1, m_MousePos[4].y1,
								3, 3, &icursorpos, m_colour[0], 1+2+4+256, 0);

	a->CursorOn();    //keep the cursor on
	if(i >=0 && i <= 4)   //a # corrosponding to the Input exit key
	{
		i = keys[i];

	}
	if(i== 27) goto ex;
	if(i==13 || i== 9 || i== 30 || i == 31 || i==257)
	{//attempt to accept the users new line width
		n = atoi(m_linewidthbuf);
		if(n < 10 || n> 78) n = m_linewidth;
		m_linewidth =n;
	}

ex:
	this->FocusLineWidth(0);
	return i;
}
//------------------------------------------------------------------------
//This function turns on or off the black select in the dirs listbox
//and turns on and positions the small cursor on the current item
//it the dirs listbox
//Entry:
// if cursorstyle =0 turn black select off
//                >0 turn black select on
void File::FocusDirsList(int cursorstyle)
{
	int x, y, t, c, width;

	width = (m_d.m_x2 - m_d.m_x1) -1;

	c =0;
	if(cursorstyle) c =1;

	x = m_MousePos[2].x1+2;
	y = m_MousePos[2].y1+1;

	y += m_d.m_currentitem -m_d.m_topitem;
	gotoxy(x, y);
	m_d.CursorOn();

	if(m_d.m_currentitem < m_numdirs)   //caller wants a dir, not a dev
	{
		_fetch_stored_entry(tempbuf, m_d.m_currentitem);
		strcpy(&filebuf[1], &tempbuf[9]);
		filebuf[0] =32;
	}
	//return "[-A-] to the list box
	//and store and display on the input line something like
	//"D:*.*"
	else
	{
		strcpy(filebuf, " [-A-]");
		t = m_d.m_currentitem- m_numdirs;
		filebuf[3] = (m_d.m_devslist[t] &95); //convert drive
																//letter to upper case
	}


	m_d.WriteLine(filebuf, x-2, y-1, width, m_colour[c]);

}
//------------------------------------------------------------------------
void File::FocusWordWrap()
{
	int x, y;

	x = m_MousePos[3].x1;
	y = m_MousePos[3].y1;
	gotoxy(x+2, y+1);
	//m_f.CursorOn();

}
//------------------------------------------------------------------------
//Entry:
// cursorstyle =0 turn linewidth black select off
//             =1 turn linewidth black select on
void File::FocusLineWidth(int cursorstyle)
{
	int x, y;
	char numbuf[10];

	x = m_MousePos[4].x1;
	y = m_MousePos[4].y1;
	gotoxy(x+2, y+1);
	sprintf(numbuf, "%d  ", m_linewidth);
	m_f.WriteLine(numbuf, x+1, y, 3, m_colour[cursorstyle]);

}
//-----------------------------------------------------------------------
//Show the button cursor at the button pressed, unpressed
//and show the small cursor as well, at the the button pos
//Entry:
//  button = 0 to 2
//  pressed = 0 or 1
void File::FocusButton(Disk *a, int button, int pressed)
{
	int x, y;

	a->m_buttonscursorpos = button;
	a->m_buttonpressed = pressed;
	a->ShowButtons();

	x = m_MousePos[5+button].x1;
	y = m_MousePos[5+button].y1;
	if(!button) x++;
	gotoxy(x+3, y+1);


}
//------------------------------------------------------------------------
//This function is only included for debugging purposes.
//The structure TMouse m_mouse is defined in screen.h
//and included as a member var in keyboard.h
//the it is instanciated when var a is created
void File::ShowMouseCoordinates(Disk *a)
{
		  int i;
		  char linebuf1[40];
	i = a->WhichFieldIsMouseIn(&this->m_MousePos[0], &mouse, 13);
	//i = -1 if mouse is not in a valid field
	sprintf(linebuf1, "   %5d   %5d  %5d ", mouse.x, mouse.y, i);
	gotoxy(1,1);
	a->WriteLine(linebuf1, 0,0, GREEN*16+ BLACK);
}
//------------------------------------------------------------------------
//Constructor for class File
File::File()
{
	m_colour[0] = LIGHTGRAY*16+BLACK;    //black select off
	m_colour[1] = BLACK*16+LIGHTGRAY;    //black select on

	//input field coordinates
	m_MousePos[0].x1 = 29;
	m_MousePos[0].y1 = 3;
	m_MousePos[0].x2 = 60;
	m_MousePos[0].y2 = 3;
	m_MousePos[0].available =1;
	//file listbox coordinates
	m_MousePos[1].x1 = 18;
	m_MousePos[1].y1 = 7;
	m_MousePos[1].x2 = 37;
	m_MousePos[1].y2 = 18;
	m_MousePos[1].available =0;    //not available
	//dir listbox coordinates
	m_MousePos[2].x1 = 41;
	m_MousePos[2].y1 = 7;
	m_MousePos[2].x2 = 60;
	m_MousePos[2].y2 = 14;
	m_MousePos[2].available =1;
	//word wrap coordinates
	m_MousePos[3].x1 = 41;
	m_MousePos[3].y1 = 16;
	m_MousePos[3].x2 = 53;
	m_MousePos[3].y2 = 16;
	m_MousePos[3].available =1;
	//line width coordinates
	m_MousePos[4].x1 = 53;
	m_MousePos[4].y1 = 19;
	m_MousePos[4].x2 = 57;
	m_MousePos[4].y2 = 19;
	m_MousePos[4].available =1;
	//ok button coordinates
	m_MousePos[5].x1 = 19;
	m_MousePos[5].y1 = 21;
	m_MousePos[5].x2 = 26;
	m_MousePos[5].y2 = 21;
	m_MousePos[5].available =1;
	//cancel button coordinates
	m_MousePos[6].x1 = 33;
	m_MousePos[6].y1 = 21;
	m_MousePos[6].x2 = 42;
	m_MousePos[6].y2 = 21;
	m_MousePos[6].available =1;
	//help button coordinates
	m_MousePos[7].x1 = 51;
	m_MousePos[7].y1 = 21;
	m_MousePos[7].x2 = 58;
	m_MousePos[7].y2 = 21;
	m_MousePos[7].available =1;
	//up arrow in file list box coordinates
	m_MousePos[8].x1 = 38;
	m_MousePos[8].y1 = 7;
	m_MousePos[8].x2 = 38;
	m_MousePos[8].y2 = 7;
	m_MousePos[8].available =0;     //not available
	//down arrow in file listbox coordinates
	m_MousePos[9].x1 = 38;
	m_MousePos[9].y1 = 18;
	m_MousePos[9].x2 = 38;
	m_MousePos[9].y2 = 18;
	m_MousePos[9].available =0;     //not available

	//scrollbar in file listbox coordinates
	m_MousePos[10].x1 = 38;
	m_MousePos[10].y1 = 8;
	m_MousePos[10].x2 = 38;
	m_MousePos[10].y2 = 17;
	m_MousePos[10].available =0;    //not available

	//up arrow in dir listbox coordinates
	m_MousePos[11].x1 = 61;
	m_MousePos[11].y1 = 7;
	m_MousePos[11].x2 = 61;
	m_MousePos[11].y2 = 7;
	m_MousePos[11].available =0;      //not available

	//down arrow in dir listbox coordinates
	m_MousePos[12].x1 = 61;
	m_MousePos[12].y1 = 14;
	m_MousePos[12].x2 = 61;
	m_MousePos[12].y2 = 14;
	m_MousePos[12].available =0;       //not available
	//scroll bar in dir listbox coordinates
	m_MousePos[13].x1 = 61;
	m_MousePos[13].y1 = 8;
	m_MousePos[13].x2 = 61;
	m_MousePos[13].y2 = 13;
	m_MousePos[13].available =0;      //not available

	//m_wordwrap = 1;                 //word wrap on
	m_linewidth = maxlen;
	strcpy(m_filemask, "*.*");         //filemask is a global var

	//setup coordinates for the file listbox
	m_f.m_x1       = m_MousePos[1].x1 -1;
	m_f.m_y1       = m_MousePos[1].y1 -1;
	m_f.m_x2       = m_MousePos[1].x2 +1;
	m_f.m_y2       = m_MousePos[1].y2 +1;
	m_f.m_numitems = 0;   //this needs to be changed
	m_f.m_currentitem =0; //this will be changed during scrolling in the
																 //class WinCtrls listbox
	m_f.m_topitem  =  0;  //and this as well

	//------------------------------
	//setup coordinates for the dir listbox
	m_d.m_x1       = m_MousePos[2].x1 -1;
	m_d.m_y1       = m_MousePos[2].y1 -1;
	m_d.m_x2       = m_MousePos[2].x2 +1;
	m_d.m_y2       = m_MousePos[2].y2 +1;
	m_d.m_numitems = 0;   //this needs to be changed
	m_d.m_currentitem =0; //this will be changed during scrolling in the
																 //class WinCtrls listbox
	m_d.m_topitem  =  0;  //and this as well


}
//------------------------------------------------------------------------
// This function opens the "open" box.
void File::OpenOpenBox(Disk *a)
{
	char linebuf1[81];
	char msgbuf[80];
	int x1, y1, x2, y2, t, i, stpos, lenbuf, lenmsg;
	int box1x1, box1y1, box2x1, box2y1;

	x1 = 15; x2 = 63;
	y1 = 1; y2 = 22;
	a->OpenShadowBox(x1, y1, x2, y2, LIGHTGRAY*16+BLACK, 0);

  strcpy(linebuf1, "                                                 ");
  strcpy(msgbuf, "Open");
  if(m_rw)
	 strcpy(msgbuf, "Save As");

  lenbuf = strlen(linebuf1);
  lenmsg = strlen(msgbuf);
  stpos = 1+(lenbuf /2) - (lenmsg/2);  //pos to enter title text into
													//linebuf1
  for(t = 0; t<lenmsg; t++){
			linebuf1[stpos+t] = msgbuf[t];
  }

  a->WriteLine(linebuf1, x1, y1, -1, WHITE*16+BLACK);
  a->WriteLine("File name: [                                ]",
			x1+2, y1+2, -1, LIGHTGRAY*16+BLACK);

  if(!m_rw)
	  a->WriteLine("Files:                 Directories:",
							x1+3, y1+4, -1, LIGHTGRAY*16+BLACK);

	else
		a->WriteLine("Existing Files:        Directories:",
							 x1+3, y1+4, -1, LIGHTGRAY*16+BLACK);


 box1x1 = x1+2;        box1y1 =y1+5;
 box2x1 = box1x1+21+2; box2y1 =box1y1;
 a->DrawBox(box1x1, box1y1, box1x1+21, box1y1+13, LIGHTGRAY*16+BLACK, 1);
 a->DrawBox(box2x1, box2y1, box2x1+21, box2y1+13-4, LIGHTGRAY*16+BLACK, 1);
 a->WriteLine("[ ] Word Wrap", box2x1+1, box2y1+10, -1, LIGHTGRAY*16+BLACK);
a->WriteLine("Line Width: [   ]", box2x1+1, box2y1+13, -1, LIGHTGRAY*16+BLACK);

 a->m_buttonsposx1[0] = box1x1+2;
 a->m_buttonsposy1[0] = box1y1+15;
 a->m_buttonsposx1[1] = box1x1+2+14;
 a->m_buttonsposy1[1] = box1y1+15;
 a->m_buttonsposx1[2] = box1x1+2+32;
 a->m_buttonsposy1[2] = box1y1+15;
 a->m_buttonsnum = 3;
 a->m_buttonscursorpos = 0;
 a->m_buttonpressed =0;

  if(m_wordwrap)
  {
	  linebuf1[0] = 'x';
	  a->WriteLine(linebuf1, box2x1+2, box2y1+13-3, 1, LIGHTGRAY*16+BLACK);
  }
  sprintf(linebuf1, "%2d", m_linewidth);
  a->WriteLine(linebuf1, box2x1+2+12, box2y1+13, -1, LIGHTGRAY*16+BLACK);

	a->SetTextOnButtons(3, 1+2+4);
	a->ShowButtons();

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int File::OpenJunior(Disk *a, char *namebuf)
{
	int cx, cy, chr, f_entrycode, i, j, x1, y1, chr2;
	int cursorstyle, d_entrycode, i_entrycode;
	int icursorpos, jmpflg;
	cursorstyle =3;     //small on, black selected
	char *dummy[6];    //never used
	a->m_repeatflg =0;
	Disk oh; //openhelp , save help   dialogue box
	//--------
	//status line help for "open" or "save as" dialogue box
	char tempbuf[81];
	strcpy(tempbuf, "F1=Help  Enter=Execute  Esc=Cancel  Tab=Next Field");
	a->WriteLine(tempbuf, 1, 24, 78, LIGHTGRAY*16+BLACK);

	//--------
	i = 13;
	x1 = 15; y1 = 1;
	f_entrycode = 1+16+128+256+512+1024+2048; //exit upon tabkey, <right>
														//<return>, <esc>,
														//clk, dblclick
	d_entrycode = 1+8+128+256+512+1024+2048; //exit upon tabkey, <left>
													 //<return>, <esc>,
													 //clk, dblclick

	i_entrycode = 1+4+128+256;              //exit upon <tab>, <down key>
													//or mouse click outside field
													 //f1 key
	this->OpenOpenBox(a);

	m_focus =0;                          //focus on input field
top:
  a->GetPath();
  strcpy(m_path, currentpath);
  a->WriteLine(m_path, x1+2, y1+3, 47, LIGHTGRAY*16+BLACK);
  //read the directory of the [DIRS] and files
  a->GetDirectory(m_path, m_filemask);
  m_numfiles = _getnumfiles();
  m_numdirs = _getnumdirs();
  if(!m_rw)
	 strcpy(inputbuf, m_filemask);       //filemask if global
  strcpy(filemask, m_filemask);
  m_f.m_numitems = m_numfiles;
  m_f.m_topitem =0;
  m_f.m_currentitem =0;
  m_f.m_cursorstyle =0;           //no cursor yet in the files list box
  m_MousePos[1].available =0;      //assume no items in listbox
  if(m_f.m_numitems) m_MousePos[1].available =1; //list box items available
  m_MousePos[8].available =0;     //assume file up arrow not available
  m_MousePos[9].available =0;     //assume file down arrow not available
  m_MousePos[10].available =0;    //assume file scrollbar not available
  if(m_f.m_numitems >12)
  {
	 m_MousePos[8].available =1;     //file up arrow available
	 m_MousePos[9].available =1;     //file down arrow available
	 m_MousePos[10].available =1;    //file scrollbar available
  }

  jmpflg =0;
  if(m_numfiles)
  {
	 m_f.PushTab();     //fast escape out of list box after display
																//files list
	 m_f.DoListBox(WinCtrls::GetFileEntry, &dummy[0], f_entrycode, 1);
  }
  else
	a->DrawBox(17, 6, 38, 19, LIGHTGRAY*16+BLACK, 1); //blank list box

  //show the list of dirs and devs in the dir listbox
  m_d.m_numitems = m_numdirs+a->m_numdevs;
  m_d.m_topitem =0;
  m_d.m_currentitem =0;
  m_d.m_cursorstyle =3;
  m_MousePos[11].available =0;     //assume dir up arrow not available
  m_MousePos[12].available =0;     //assume dir down arrow not available
  m_MousePos[13].available =0;     //assume dir scrollbar not available

  if(m_d.m_numitems >8)
  {
	 m_MousePos[11].available =1;     //dir up arrow available
	 m_MousePos[12].available =1;     //dir down arrow 1available
	 m_MousePos[13].available =1;     //dir scrollbar available
  }

  m_d.PushTab();     //fast escape out of dir list box after display list
  m_d.m_cursorstyle =0;     //no cursor in dirs box
  //disp dirs+devs
  m_d.DoListBox(WinCtrls::GetDirOrDevEntry, &dummy[0], d_entrycode, 1);


  a->CursorOn();
  //set focus on the inputline
  if(!m_rw)
	 strcpy(inputbuf, filemask);
  else
  {
	  inputbuf[0] =0;
	  if(namebuf[0] != 0)
		 strcpy(inputbuf, namebuf);
  }
get2:                        //to here from tab from cancle button
  if(!m_focus)
  this->ShowInputLine(a, inputbuf, cursorstyle);
  else
  if(m_focus ==1)
  {
	 m_f.m_cursorstyle =0;        //no cursor yet in the files list box
	 this->FocusFilesList(m_f.m_cursorstyle);

  }
  else
  if(m_focus ==2)
  {
	 m_d.m_cursorstyle =3;
	 m_f.m_cursorstyle =0;
	 this->FocusFilesList(m_f.m_cursorstyle); //black select off in files box
	 this->FocusDirsList(m_d.m_cursorstyle);  //black select off in files box
	 a->CursorOn();
  }

  if(m_focus ==5)
  {  //turn small cursor on on ok button
	cursorstyle=0;
	this->ShowInputLine(a, inputbuf, cursorstyle);
	this->FocusButton(a, 0, 0);
	a->CursorOn();
  }
get:
	a->CursorOn();
	chr = a->WaitGetMouseOrKey();
	if(chr == 210)
	{
		goto hlp;
	}
	if(m_focus == 2 && (((chr >= 65) && (chr <= 90)) || ((chr >= 97) && (chr <=122)))){ m_d.PushA_Z(chr); goto dir_me;}
	if(m_focus == 1 && (((chr >= 65) && (chr <= 90)) || ((chr >= 97) && (chr <=122)))){ m_d.PushA_Z(chr); goto e1;}


got:
	if((m_focus== 0) && (chr >= 32) && (chr <= 255))
	{ //focus is on input line and the user has typed a text char
	  //erase the preanswer and enter the input function
	  inputbuf[0] =chr;
	  inputbuf[1] =0;
	  icursorpos =1;
	  goto in1;

	}
	//if the user is entering a digit on the line width field
	if((m_focus ==4)  && (chr >=48 && chr <=57))
	{

		a->PushDigit(chr-48);
		goto lw4;

	}


	switch(chr)
	{
	  case 27:
		  goto ex;
//-------------
	 case 32:      //space from main loop
		if(m_focus ==3)    //<space> toggle the word wrap flag
		{
			a->CursorOn();
			if(m_wordwrap) m_wordwrap =0;
			else m_wordwrap =1;


			if(m_wordwrap)
				a->WriteLine("x", m_MousePos[3].x1+1, m_MousePos[3].y1, 1,
									m_colour[0]);

			else
				a->WriteLine(" ", m_MousePos[3].x1+1, m_MousePos[3].y1, 1,
																	 m_colour[0]);
		}
		if(m_focus ==4)   //clear the line width number and enter its
		{                  //functionality
			a->PushDel();
			goto lw4; //line width functionality
		}
		goto get;    //ignore other <spaces>
//-------------
		case 31:     //down key from main loop
		case 30:    //up key from main loop
		switch(m_focus)
		{
			case 0:  //<down key> from the input focus to the files listbox
a1:         cursorstyle =0;
				this->ShowInputLine(a, inputbuf, 0);                 //focus
				m_d.m_cursorstyle =0;           //black select off in dirs box
				this->FocusDirsList(m_d.m_cursorstyle);
				goto b3;

b1:         m_f.m_cursorstyle =0;      //no cursor yet in the files list box
b3:         this->FocusFilesList(m_f.m_cursorstyle);//files small crsr is on
				m_focus =1;                //focus in files box
				if(m_f.m_cursorstyle) goto c1; //enter files functionality
				goto get;


			case 1: //focus 1: in files box, <down pressed>
e1:         if(!m_numfiles) goto get;  //no files: ignore key
				this->FocusDirsList(0);  //black select off in dirs box

			//enter the files box functionality
c1:      m_f.m_cursorstyle =3;
			chr = m_f.DoListBox(WinCtrls::GetFileEntry, &dummy[0], f_entrycode,
																m_f.m_cursorstyle, jmpflg);
			jmpflg =0;
			this->FocusFilesList(1); //black select on
			if(chr == 210) goto hlp;
			if(chr ==9) goto a3; //tab to dirs list box
			if(chr ==28) goto a3; //<right key> to dirs box
			//code goes here to accept <return> in files box
			if(chr ==13)  goto ev;
			if(chr != -1) goto get;
			goto mm;     //test which field click occurred in


			case 2: //focus 2: in dirs box, <down pressed>
								//enter the dirs box functionality
c2:         if(m_d.m_cursorstyle ==3)
				{
					if(chr == 31) m_d.PushDown();
					else m_d.PushUp();
				}
dir_me:		m_d.m_cursorstyle =3;
				this->FocusFilesList(0);  //black select off in files box
cc:         chr = m_d.DoListBox(WinCtrls::GetDirOrDevEntry, &dummy[0],
											d_entrycode, m_d.m_cursorstyle, jmpflg);

				jmpflg =0;
				this->FocusDirsList(1); //black select on  in dirs box
				a->CursorOn();
				if(chr == 210) goto hlp;
				if(chr ==9) goto b2; //tab to word wrap button
				if(chr ==29) goto b1; //<right key> to files box
				//code goes here to accept <return> in files box
				if(chr ==13) goto ev; //evaluate path
				if(chr != -1) goto get;
				goto mm;     //test which field click occurred in



				case 3:    //down or up key pressed currently in wordwrap field
				if(chr ==30 )  //up key?
				{
					//m_f.m_cursorstyle =0;    //black select off in files box
					//this->FocusFilesList(m_f.m_cursorstyle);
					goto a4 ;//up key goto dirs box
				}
				if(chr ==31)  goto lw;//down key goto line width

				case 4:  //focus is on linewidth: user pressed either up or down
				if(chr==30)
				{
					this->FocusLineWidth(0);  //turn off focus at linewidth
					goto b2;   //up: goto wordwrap field
				}
				else goto lw2;         //down: tab to ok button



				case 5:  //focus on ok button, if up key goto to line width field
				if(chr ==30) goto a1; //listbox

				case 6: //focus is on either cancel or help buttons
				case 7: //if up key set ok button and focus on linewidth
				if(chr ==30)
				{
					this->FocusButton(a, 0, 0);
					goto lw;
				}

		}  //end of switch focus when down key is pressed from main
		goto get;
//-----------------
			 case 28: // <right key pressed> from main loop
			 switch(m_focus)
			 {
				case 1:
				if(m_d.m_cursorstyle) goto c2; //enter dirs box functionality
				goto a4;   //tab from files box to dirs box focus only

				case 5:  //on ok button  go <right> to cancel
					goto tb1;

				case 6:  //on cancel go <right> to help
					goto  tb2;

			 }
			 goto get;//end of switch focus when right key pressed from main loop

//-------------------
			 case 29: //<left key pressed from main loop
				switch(m_focus)
				{
					case 0: //left key pressed while on input line: enter input
						icursorpos = strlen(inputbuf)-1;
						if(icursorpos <0) icursorpos =0;
in1:               i = a->Input(&m_MousePos[0], 14, inputbuf, 29, 3, 32, 128, &icursorpos,
														m_colour[0], i_entrycode, 1);
						a->CursorOn();    //keep the cursor on
												// i= -2 a click occurred
						if(i==4 || i==2) goto a1;//user exited input using <down>
														 //or <tab> to tab to files
						if(i ==9) goto hlp;
						if( i== 1) //user exited by pressing <esc>
						{          //so close the filebox
							chr = 27;
							goto ex;
						}
						if(i==0) //user exited input by pressing <return>
						{ //evaluate the user input line

ev:                  i = this->EvalPathOrName(a, filebuf);
							icursorpos = strlen(inputbuf);
							if(!i)
							{
								chr =13;  //got valid filename
								goto ex;
							}
							if(i==2)     //redisp files and dirs?
							{
								//gotoinput =1;
								goto top;
							}
							goto in1; //goto input func again


						} //end of user exited input by pressing <return>
						if( i == 257) goto mm;
						goto get;


				case 2:     //left key pressed from dirs focus, goto files focus
					if(m_f.m_cursorstyle)
					{
						m_d.m_cursorstyle=0;
						this->FocusDirsList(m_d.m_cursorstyle);//turn off dirs box
					}
					goto b3;


				case 6:    //on cancel button go <left> to ok
					goto lw2;

				case 7:   //on help button, bo <left to cancel
					goto tb1;

				}
				goto get;

//-------------------   (end of left key)
			 case 9:  //tab from main loop
				switch(m_focus)
				{
					case 0:       //tab from input focus to file box
						goto a1;

					case 1:  //tab from files listbox to dir listbox
a2:            	m_f.m_cursorstyle =0;    //black select off in files box
						this->FocusFilesList(m_f.m_cursorstyle);

a3:               m_d.m_cursorstyle =0; //no black select yet in the dirs box
a4:               this->FocusDirsList(m_d.m_cursorstyle);//small crsr on in
																			//dirs
						m_focus =2;            //focus in dirs box
						goto get;

					case 2:    //tab from dirs box to word wrap field
b2:            	cursorstyle=0;
						this->ShowInputLine(a, inputbuf, cursorstyle); //focus  off
						this->FocusWordWrap();
						m_focus =3;
						goto get;

					case 3:  //tab from wordwrap to linewidth
lw:            	this->FocusLineWidth(1);
						m_focus =4;
						goto get;

					case 4:  //tab from linewidth to ok button
lw2:           	this->FocusLineWidth(0);  //turn off focus at linewidth

						this->FocusButton(a, 0, 0);
						m_focus =5;
						goto get;


					case 5: //tab to cancel button
tb1:           	this->FocusButton(a, 1, 0);
						m_focus =6;
						goto get;


					case 6: //tab to help button
tb2:                 this->FocusButton(a, 2, 0);
							m_focus = 7;
							goto get;

					case 7: //focus from help button to input line and ok button
						this->FocusButton(a, 0, 0);
						m_focus =0;
						cursorstyle =3;
						goto get2;        //goto focus on input line


				}
				goto get; //end of tab switch()
//----------------------
				case 8:   //del key pressed on main loop
				switch(m_focus)
				{
					case 0:
						icursorpos = strlen(inputbuf);
						if(icursorpos)
						{
							inputbuf[icursorpos-1] =0;  //del last char
							icursorpos--;
						}
						goto in1;  //enter inputfunction


					case 4:
						a->PushDel();
						goto lw4; //line width functionality


				}
				goto get;
//------------------------
				case 13: //<return> pressed from main
					switch(m_focus)
					{

						case 0:   //user pressed return on input line
							m_d.PushReturn();
							goto in1;      //enter input functionality

						case 1:  //return pressed while focus is in files box
							m_f.PushReturn();
							goto e1;


						case 2:    //focus was in dirs box , user pressed <return>
							m_d.PushReturn();
							goto c2;     //goto dirs box functionality


						case 4:
lw4:              	chr =this->InputLineWidth(a);
							if(chr == 9) goto lw2;       //tab to ok button
							if(chr == 13) goto lw2;      //tab to ok button
							if(chr == 30) goto b2;      //tab up to wordwrap field
							if(chr ==31) goto lw2;       //tab down to ok button
							if(chr == 27) goto ex;
							if(chr != 257) goto get;
							goto mm;     //test which field click occurred in

						 case 5: //focus on ok button, <return> was pressed
									//only accept filename if its a valid name
							i = this->EvalPathOrName(a, filebuf);
							icursorpos = strlen(inputbuf);
							if(!i)
							{
								chr =13;  //got valid filename
								goto ex;
							}
							if(i==2)     //redisp files and dirs?
							{
								//gotoinput =1;
								goto top;
							}
							else  //error: file name not found
							{     //set small cursor on  on ok button
								cursorstyle=0;
								this->ShowInputLine(a, inputbuf, cursorstyle);
								this->FocusButton(a, 0, 0);

								a->CursorOn();
							}
							goto get; //goto input func again

						case 6: //on cancel: user hit return
							chr =27;
							goto ex;
					  //TODO: write the code for for when user presses <return> on
					  //the help button ie., case 7:
						case 7:
hlp:						int t = wherex();
							i = wherey();
							OpenHelp(&oh, m_rw);
							gotoxy(t,i);
							goto get;

				}
				goto get;
//-------------------------
			 case 257:      //the mouse was clicked from main
mm:       	i = a->WhichFieldIsMouseIn(&m_MousePos[0], &mouse, 14);
				if(i == -1) goto get;
				switch(i)
				{
					case 0:  //the mouse was clicked on the input line
													  //enter the input functionality
					this->FocusLineWidth(0);  //turn off line width
					m_focus =0;
					icursorpos = strlen(inputbuf);
					j =   mouse.x - m_MousePos[0].x1;
					if(j >=0 && j <= icursorpos) icursorpos =j;

					if(!cursorstyle)          //if black select off in input line
					{                         //set black select on but only
						cursorstyle =3;        //focus on input line
						this->ShowInputLine(a, inputbuf, cursorstyle);
						a->WaitMouseButtonRelease();
						goto get;
					}
					else      //black select was on in input line so enter
					{         //input functionality
						cursorstyle =0;
						goto in1;
					}


					case 1: //mouse was clicked in filebox (from main loop)
						this->FocusLineWidth(0);  //turn off line width
						if(!m_numfiles) goto get;  //no files: ignore key
						m_d.m_cursorstyle=0;
						this->FocusDirsList(m_d.m_cursorstyle);  //black select off in dirs box
						cursorstyle =0;  //black select off in input line
						this->ShowInputLine(a, inputbuf, cursorstyle);

						//enter the files box functionality
						m_f.m_cursorstyle =3;
						j = m_f.m_topitem +(mouse.y - m_MousePos[1].y1);
						if((j+1)> m_f.m_numitems) j = m_f.m_numitems-1;
						m_f.m_currentitem =j;
						m_focus =1;
						goto c1;

					case 2: //mouse was clicked in the dirbox (from main loop)
						this->FocusLineWidth(0);  //turn off line width
						m_f.m_cursorstyle=0;
						this->FocusFilesList(m_f.m_cursorstyle);  //black select off
																				//in filesbox
						cursorstyle =0;  //black select off in input line
						this->ShowInputLine(a, inputbuf, cursorstyle);

						//enter the dirs box functionality
						m_d.m_cursorstyle =3;
						j = m_d.m_topitem +(mouse.y - m_MousePos[2].y1);
						if((j+1)> m_d.m_numitems) j = m_d.m_numitems-1;
						m_d.m_currentitem =j;
						m_focus =2;
						goto cc; //enter dirs box functionality

					case 3:  //user clicked  on word wrap; just goto to focus
						 this->FocusLineWidth(0);  //turn off line width
						 a->WaitMouseButtonRelease();
						 a->PushSpace();
						 goto  b2;

					case 4: //the user clicked on the line width field
						goto lw;

					case 5: //user click on ok button
						if(m_focus ==0)
						{
							cursorstyle =0;  //turn black select off input line
							this->ShowInputLine(a, inputbuf, cursorstyle);
						}
						if(m_focus ==4)
						{                     //turn off line width
							cursorstyle =0;
							FocusLineWidth(0);
						}

						//show cursor on ok button
						a->CursorOn();
						m_focus = 5;
						this->FocusButton(a, 0, 1);
						a->WaitMouseButtonRelease();
						this->FocusButton(a, 0, 0);
						//if the mouse upon release is on the ok button
						//push <return>
						j = a->WhichFieldIsMouseIn(&m_MousePos[0], &mouse, 14);
						if(j ==5) a->PushReturn();
						goto get;


					case 6: //user click on cancel button
						if(m_focus ==0)
						{
							cursorstyle =0;  //turn black select off input line
							this->ShowInputLine(a, inputbuf, cursorstyle);
						}
						if(m_focus ==4)
						{                     //turn off line width
							cursorstyle =0;
							FocusLineWidth(0);
						}

						//show cursor on cancel button
						a->CursorOn();
						m_focus = 6;
						this->FocusButton(a, 1, 2);   //show button pressed
						a->WaitMouseButtonRelease();
						this->FocusButton(a, 1, 0);    //show button released
						//if the mouse upon release is on the cancel
						//button push <esc>
						j = a->WhichFieldIsMouseIn(&m_MousePos[0], &mouse, 14);
						if(j ==6) a->PushEsc();
						goto get;


					case 7: //user click on the help button
						if(m_focus ==0)
						{
							cursorstyle =0;  //turn black select off input line
							this->ShowInputLine(a, inputbuf, cursorstyle);
						}
						if(m_focus ==4)
						{                     //turn off line width
							cursorstyle =0;
							FocusLineWidth(0);
						}

						//show cursor on help button
						a->CursorOn();
						m_focus = 7;
						this->FocusButton(a, 2, 3);   //show button pressed
						a->WaitMouseButtonRelease();
						this->FocusButton(a, 2, 0);    //show button released
					//if the mouse upon release is on the  help button push <esc>
						j = a->WhichFieldIsMouseIn(&m_MousePos[0], &mouse, 14);
						if(j ==7)
						{
							int t = wherex();
							i = wherey();
							OpenHelp(&oh, m_rw);
							gotoxy(t,i);
						}
						goto get;


					case 8: //user clicked on the up arrow in the file listbox
						this->FocusLineWidth(0);  //turn off line width
						if(!m_numfiles) goto get;  //no files: ignore key
						m_d.m_cursorstyle=0;
						this->FocusDirsList(m_d.m_cursorstyle);  //black select off in dirs box
						cursorstyle =0;  //black select off in input line
						this->ShowInputLine(a, inputbuf, cursorstyle);

						//enter the files box functionality
						m_f.m_cursorstyle =3;
						m_focus =1;
						jmpflg=1; //received by files list box for up arrow function
						goto c1;

					case 9: //user clicked on the down arrow in the file listbox
						this->FocusLineWidth(0);  //turn off line width
						if(!m_numfiles) goto get;  //no files: ignore key
						m_d.m_cursorstyle=0;
						this->FocusDirsList(m_d.m_cursorstyle);  //black select off in dirs box
						cursorstyle =0;  //black select off in input line
						this->ShowInputLine(a, inputbuf, cursorstyle);

						//enter the files box functionality
						m_f.m_cursorstyle =3;
						m_focus =1;
						jmpflg=2; //received by files list box for down arrow
						goto c1; //function



					case 10: //user clicked on the scrollbar in the file listbox
						this->FocusLineWidth(0);  //turn off line width
						if(!m_numfiles) goto get;  //no files: ignore key
						m_d.m_cursorstyle=0;
						this->FocusDirsList(m_d.m_cursorstyle);  //black select off in dirs box
						cursorstyle =0;  //black select off in input line
						this->ShowInputLine(a, inputbuf, cursorstyle);

						//enter the files box functionality
						m_f.m_cursorstyle =3;
						m_focus =1;
						jmpflg=3; //received by files list box for down
						goto c1;  //arrow function



						case 11: //user clicked on up arrow in the dir listbox
							this->FocusLineWidth(0);  //turn off line width
							m_f.m_cursorstyle=0;
							this->FocusFilesList(m_f.m_cursorstyle); //black select
																					//off in files
							cursorstyle =0;  //black select off in input line
							this->ShowInputLine(a, inputbuf, cursorstyle);

							//enter the dirs box functionality
							m_d.m_cursorstyle =3;
							m_focus =2;
							jmpflg=1;   //received by dirs list box for up arrow
							goto cc;    //function


					case 12: //user clicked on down arrow in the dir listbox
						this->FocusLineWidth(0);  //turn off line width
						m_f.m_cursorstyle=0;
						this->FocusFilesList(m_f.m_cursorstyle); //black select off in files
						cursorstyle =0;  //black select off in input line
						this->ShowInputLine(a, inputbuf, cursorstyle);

						//enter the firs box functionality
						m_d.m_cursorstyle =3;
						m_focus =2;
						jmpflg=2; //received by dirs list box for down arrow function
						goto cc;

						case 13: //user clicked on scrollbar the dir listbox
							this->FocusLineWidth(0);  //turn off line width
							m_f.m_cursorstyle=0;
							this->FocusFilesList(m_f.m_cursorstyle); //black
																		//select off in files
							cursorstyle =0;  //black select off in input line
							this->ShowInputLine(a, inputbuf, cursorstyle);

							//enter the firs box functionality
							m_d.m_cursorstyle =3;
							m_focus =2;
							jmpflg=3;	//received by dirs list box for
											//scrollbar function
							goto cc;

					  }
					  goto get;  //end of switch for single click




					  case 258: //double click from main
					  i = a->WhichFieldIsMouseIn(&m_MousePos[0], &mouse, 14);
					  if(i == -1) goto get;
					  switch(i)
					  {
						case 0:  //the mouse was double clicked on the input line
													  //enter the input functionality
							this->FocusLineWidth(0);  //turn off line width
							m_focus =0;
							icursorpos = strlen(inputbuf);
							j =   mouse.x - m_MousePos[0].x1;
							if(j >=0 && j <= icursorpos) icursorpos =j;

							cursorstyle =3;
							this->ShowInputLine(a, inputbuf, cursorstyle);
							goto in1;



					  }
					  goto get;  //end of switch for double click






	}//end of main switch (chr)
	goto get;
//--------------------

ex:
			a->CloseBox();
	if( chr ==13) strcpy(namebuf,filebuf);  //return the path + name
	return chr;
}
//-------------------------------------------------------------------------

//control passes to here when the user choses "Open" from the file menu.
int File::Open(Disk *a)
{
	int i, t, l, lenbuf1;
	char linebuf1[170];
	char namebuf[128];         //buf to hold file name returned
												//from OpenJunior()

	m_rw =0; //open for loading
	namebuf[0] =0;
	i =this->OpenJunior(a, namebuf);
	maxlen = m_linewidth;
	lm = (78 - maxlen)/2;
	gotoxy(1, 10);
	if(i ==13)   //user wants this file loaded
	{
		this->LoadFile(a, namebuf);
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
		strcat(editortitle, namebuf);
	}

	a->CursorOff();
	//clear text area of screen
	for(t =0; t<22; t++){
		a->WriteLine(" ", 1, 2+t, 78, gbc[2]*16+gfc[2]);
	}
	modified =0;
	return 0;
}
//-------------------------------------------------------------------------
int File::SaveFile(Disk *a, char *name)
{
	int t, i, handle, len;
	char spcbuf[40];
	memset(spcbuf, 255, 39);
	spcbuf[lm] =0;
	lines = a->GetLines();
	i = a->OpenFile(name, 1, &handle, 1);
	maxlen = m_linewidth;
	lm = (78 - maxlen)/2;
	if(i)
	{
		printf("Error opening file\n");
		goto ex;
	}
	if(m_wordwrap)
	{
		linebuf1[0] = 1;  //code: file contains tied lines
		linebuf1[1] =0;
		lenbuf1 =1;
		a->WriteFile(handle, linebuf1);
	}

	for(t = 1; t<= lines; t++){
		len = a->GetLineBuf(linebuf1, &t);
		//if wordwrap flg is off while saving, break the lines
		if(!m_wordwrap)
		{
			if(linebuf1[len-1] != 13)
			{
				if(len ==maxlen)
				{
					if(linebuf1[len-1] == 32)
					{
						linebuf1[len-1] =13;
						goto a1;
					}
				}

				linebuf1[len] =13;
				linebuf1[len+1] =0;
				len++;
			}


a1:		//prefix line with spaces
			if(lm)
			{
				len = a->InsertString(linebuf1, spcbuf, 0);

			}
		}

		 //if there is a retrn code add a linefeed code
		 if(linebuf1[len-1] ==13)
		 {
			linebuf1[len] = 10;
			linebuf1[len+1] =0;
			len++;
		}
		a->WriteFile(handle, linebuf1);
	}
		modified =0;
ex:
	a->CloseFile(handle);

	return i;
}
//------------------------------------------------------------------------
//if there is a path and name, get the the name only
//remove the path
char *File::getname(Disk *a, char *namebuf)
{
		int i, l;
		char *p;

		i = strlen(namebuf);
		l = a->RightFindChar(namebuf, '\\', i-1);
		if(l)
		{
			p = &namebuf[l];
			strcpy(linebuf1, p);
			strcpy(namebuf, linebuf1);
		}

	return namebuf;
}

//------------------------------------------------------------------------
//control passes to here when the user choses SaveAs from the file menu.
int File::SaveAs(Disk *a)
{
	int done, i, l, lenbuf1;
	char *p;
	char linebuf1[170];
	char namebuf[128];         //buf to hold file name returned
												//from OpenJunior()

	strcpy(namebuf, editortitle);
	i = strcmp(namebuf, "UNTITLED");
	if(!i)
		namebuf[0] =0;


	else  //search the string for the rightmost \, if found
			//keep the filename, cut the rest at the left hand side.
	{
		this->getname(a, namebuf);
	}


	m_rw =1; //open for saving
	done =this->OpenJunior(a, namebuf);
	maxlen = m_linewidth;
	lm = (78 - maxlen)/2;
	gotoxy(1, 10);
	if(done == 13)   //user wants file saved under this name
	{

		i = this->SaveFile(a, namebuf);

		strcpy(editortitle, a->GetPath());
		l = strlen(editortitle);
		if(editortitle[l-1] !='\\')
		{
			 editortitle[l] = '\\';
			 l++;
			 editortitle[l] =0;
		 }
		this->getname(a, namebuf);
		strcat(editortitle, namebuf);
	}

	a->CursorOff();
	return done;
}
//-------------------------------------------------------------------------
int File::Save(Disk *a)
{
	int i, ii, done;
	char namebuf[128];


	strcpy(namebuf, editortitle);
	i = strcmp(namebuf, "UNTITLED");
	if(!i)   //if no title given choose save as dialog box
		done = this->SaveAs(a);
	else
	{
		this->getname(a, namebuf);
		done = this->SaveFile(a, namebuf);

	}

	if(done == 13 || done ==0) done =0;
	else done =1;
	return done;
}
//-------------------------------------------------------------------------


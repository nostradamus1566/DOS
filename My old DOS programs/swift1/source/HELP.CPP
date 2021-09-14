/*-------------------------------------------------------------------------
	(C) Michael Finnegan    3 June 1999
-------------------------------------------------------------------------*/

#include "help.h"
#include <dos.h>
#include "editor.h"
//----------------------------------------------------------------------
//Given a date such as 2 June 1999 in the format of 2, 6, 1999
//This program return the number of bigdays this date is and also it
//returns a string containing the name of the day it falls on ie., "Sunday".

long int GetDay(char *daystr, int dd, int mm, int yyyy)
{


	char *dchar[7] = {
	"Sunday", "Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday", "Saturday"};

	int NumDays[12]={
// jan feb mar apr may jun jul aug sep oct nov dec
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	int PrevDays[12] ={
//  j   f   m   a   m   j     j   a    s    o    n    d}
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};


	long int bigday, temp, year, pyear, day, month, lp;

	lp =0;
	if(!(yyyy %4)) lp =1;
	if(!(yyyy %100)) lp =0;
	if(!(yyyy %400)) lp =1;


	pyear = yyyy -1;
	temp = (pyear *365) + (pyear /4) - (pyear /100) + (pyear /400);
	bigday = temp + PrevDays[mm-1];
	if(mm >2) bigday += lp;
	bigday += dd;
	int daynum = bigday %7;
	strcpy(daystr, dchar[daynum]);

	return bigday;
}

//--------------------------------------------------------------------------
//This function receives the version date
//It then computes the number of days the program is old
//it then gets the current date based on the computer clock
//and fills in the values for the structure TDate
//for day, month, year, progage, day_str, month_str
//hour, minute, second
int GetDate(TDate *b, int dd, int mm, int yyyy)
{

static char *mchar[12] ={
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"};

	struct date d;
	struct time t;
	char day_str[12], month_str[14];
	long int big1, big2;

	getdate(&d);
	gettime(&t);

	b->year = d.da_year;
	b->month = d.da_mon;
	b->date = d.da_day;
	b->hour = t.ti_hour;
	b->minute = t.ti_min;
	b->second = t.ti_sec;
	strcpy(b->month_str, mchar[b->month-1]);
	big2 = GetDay(b->day_str, b->date, b->month, b->year);

	big1 = GetDay(day_str, dd, mm, yyyy);
	b->progage = big2 - big1;

	return 0;
}
//-----------------------------------------------------------------------


void Help::About(Disk *a)
{
  char tempbuf[81];
  char tempbuf2[81];
  int m, x, y;
  static char *pchar[20] = {"About Swift",
														 "(c) Michael Finnegan",
														 "  ",
														 "                                                                                                     ",
														 "                                                                                                     ",
														 "                                                                                                     "
														};

		  struct TDate b;
		  GetDate(&b, 12, 7, 1999);
		  sprintf(tempbuf, "Today is %s, %d %s %d ", b.day_str, b.date, b.month_str, b.year);
		  m = 0; //assume AM
		  if(b.hour >12)
		  {
			  b.hour -=12;
			  m =1; //PM
		  }
		  else
		  {
			  if(b.hour == 12)
			  m=1;
		  }
		  if(!m) sprintf(tempbuf2, "(%2d:%2d AM)", b.hour, b.minute);
		  else sprintf(tempbuf2, "(%2d:%2d PM)", b.hour, b.minute);
		  if(b.hour < 10) tempbuf2[1] = '0';
		  if(b.minute <10) tempbuf2[4] ='0';
		  strcat(tempbuf, tempbuf2);
		  strcpy(&pchar[3][0], tempbuf);
		  strcpy(&pchar[4][0], "                                              ");
		  if(b.progage >1)
			  sprintf(tempbuf,"This version compiled on 12 July 1999 -- %d days ago.", b.progage);
		  else if(b.progage ==1)
			  sprintf(tempbuf,"This version compiled yesterday.");
		  else if(b.progage ==0)
			  sprintf(tempbuf,"This version of was compiled today.");
		  else
		  {
			  sprintf(tempbuf, "The Computer's clock says today is %s, %d %s %d %s", b.day_str, b.date, b.month_str, b.year, tempbuf2);
			  strcpy(&pchar[3][0], tempbuf);
			  sprintf(tempbuf,"But I think the computer's clock is incorrect.");
		  }


		  strcpy(&pchar[5][0], tempbuf);



		  a->CursorOn();

		  //a->repeatflg =1;
			/*int ShowDialogBox(char *txtlines[], int numlines,
													int buttonsbits,
													int f_maxviewlines,
													int cursorstyle=1,
													int getout=0, int repeatflg=0);

		 */
		  a->ShowDialogBox(&pchar[0], 5, 1, 5,0);
		  SetCursor(a, 1);
		  //DispStatusLine(a);
}
//-----------------------------------------------------------------------
void Help::Commands(Disk *a)
{
  char tempbuf[81];

  static char *pchar[60] = {"Swift Commands",
" ",
" Cursor Movement Commands",
" ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ",
" Home       - Move to the start of the current line.",
" End        - Move to the end of the current line.",
" Ctrl+Up    - Scroll up one line.",
" Ctrl+Down  - Scroll down one line.",
" PageUp     - Scroll up one screen.",
" PageDown   - Scroll down one screen.",
" Ctrl+PgUp  - Scroll left one screen.",
" Ctrl+PgDn  - Scroll right one screen.",
" Ctrl+Home  - Scroll to the start of the document.",
" Ctrl+End   - Scroll to the end of the document.",
" Ctrl+Left  - Move left one word. ",
" Ctrl+Right - Move right one word.",
" ",
" Editing Commands",
" ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ",
" Enter      - Start a new line.",
" Delete     - Delete the character that the cursor is on.",
" Backspace  - Delete the character to the left of the cursor.",
" Tab        - Move the cursor to next tab stop.",
" Insert     - Switch between insert and overwrite modes.",
" Ctrl+Y     - Delete the current line.",
" Ctrl+V     - Paste buffer contents into file.",
" Ctrl+P     - Allows special characters to be inserted.",
" ",
" Commands for Working with Selected Regions",
" ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ",
" Shift      - Use the shift key in conjunction with the cursor",
"              movement functions to select regions.",
" Ctrl+C     - Copy the current selection to buffer.",
" Ctrl+X     - Delete the current selection and copy it to buffer.",
" Delete     - Delete the current selection.",
" Tab        - Indent the selected lines.",
" Shift+Tab  - Unindent the selected lines.",
" ",
" Commands for Finding and Replacing Text",
" ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ",
" Ctrl+Q+F   - Find text.",
" Ctrl+Q+A   - Find text and replace it.",
" F3         - Repeat the last search.",
" ",
" ",
" Other Commands",
" ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ",
" F1         - Display context-sensitive help.",
" ",
" Many of the shortcut keys similar to those in versions",
" of MS-DOS EDIT are also supported.",
" " };


	strcpy(tempbuf, "Enter=Cancel  User cursor keys to scroll through the help list.");
	a->WriteLine(tempbuf, 1, 24, 78, LIGHTGRAY*16+BLACK);
	gotoxy(40, 20);
	a->CursorOn();
	a->ShowDialogBox(&pchar[0], 51, 1, 12, 0);
	SetCursor(a, 1);
	//DispStatusLine(a);


}
//-----------------------------------------------------------------------

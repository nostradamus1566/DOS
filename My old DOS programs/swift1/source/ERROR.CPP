
/***NOTE:
	 This is an interrupt service routine.  You can NOT compile this
	 program with Test Stack Overflow turned on and get an executable
	 file which will operate correctly. */

#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include "labels.h"
//#include "clip.h"
#include "editor.h"
//#include "colours.h"
//#include "labels.h"
//#include <stdlib.h>




//When a critical error is generated this functions traps it and
//sets the global error vars to the results

void interrupt criticalhandler(__CPPARGS)
{
	Disk a;
	char buf[81];

	  static char pchar[4][50];
	  static char *pchar2[4];
	  strcpy(&pchar[0][0], "System Error");

	  strcpy(&pchar[2][0], " ");
	  strcpy(&pchar[3][0], " ");


		pchar2[0] = &pchar[0][0];
		pchar2[1] = &pchar[1][0];
		pchar2[2] = &pchar[2][0];
		pchar2[3] = &pchar[3][0];





	asm{
		sti
		mov ax, 05900h
		mov bx, 0
		int 21h      //get extended info
		mov errnum, ax
		mov byte ptr errclass, bh
		mov byte ptr erraction, bl
		mov byte ptr errlocus, ch
		}
	if(nodialog) goto ex;

	switch( errnum)
	{
		case 2:
			strcpy(buf, "file not found");
		  break;
		case 3:
			strcpy(buf, "Path not found");
		  break;
		case 4:
			strcpy(buf, "Too many files open");
		  break;
		case 5:
			strcpy(buf, "Access denied");
		  break;
		case 15:
			strcpy(buf, "Disk drive invalid");
		  break;
		case 19:
			strcpy(buf, "disk write-protected");
		  break;
		case 21:
			strcpy(buf, "Drive not ready. Check if disk is inside.");
		  break;
		case 32:
			strcpy(buf, "file sharing violation");
		  break;
		default:
			strcpy(buf, "An error occurred");
			break;

	}


		 strcpy(&pchar[1][0], buf);
		 a.CursorOn();

		  //a->repeatflg =1;
			/*int ShowDialogBox(char *txtlines[], int numlines,
													int buttonsbits,
													int f_maxviewlines,
													int cursorstyle=1,
													int getout=0, int repeatflg=0);

		  */
		  a.ShowDialogBox(&pchar2[0], 3, 1, 3,0);




ex:

}
//-----------------------------------------------------------------


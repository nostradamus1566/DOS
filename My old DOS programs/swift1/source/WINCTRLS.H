/*-------------------------------------------------------------------------
   (C) Michael Finnegan        15 May 1999
-------------------------------------------------------------------------*/

//This header files contains a function for
//displaying text in dialog boxes. When the dialog box appears it
//saves the previous screen, and restores when the dialog box exits.
#define IDI_OK = 1
#define IDI_CANCLE = 2
#define IDI_HELP = 4
#define IDI_YES = 8
#define IDI_NO = 16
#include "menu.h"

#if !defined WINCTRLS_H
#define WINCTRLS_H

class WinCtrls : public Menu
{
	public:
	int m_box_x1;
	int m_box_y1;
	int m_box_x2;
	int m_box_y2;
	int m_linesview;          	//r # of lines that can be displayed in
										//in a dialog box
	char linebuf1[160];
  int m_repeatflg;

	int m_buttonsposx1[4];    	//coordinates of each of the 4 buttons
	int m_buttonsposy1[4];
	int m_buttonsnum;         //r # of of buttons being used
	char m_buttonstxt[4][20]; //current text of the buttons
	int m_buttonscursorpos;   //pos of the button cursor is on
	int m_buttonpressed;     //0 = not pressed, else the r # =
									 //number of the button pressed
											 //ie., 2 = button 2 pressed

	int m_buttonstextlen;   //len of text of all valid buttons
	int m_scrollflg;
	int m_width;
	int m_height;



	void ShowVerticalScrollBar(int x1, int y1, int len, int cursorlen,
        int cursorpos, char attrib1 = LIGHTGRAY *16+BLACK,
                       char attrib2 = BLACK *16 + WHITE,
                       char attrib3 = WHITE * 16 + LIGHTGRAY);



   void ShowButtons();
	void SetTextOnButtons(int numbuttons, int buttonsbits);
	int CountButtons(int buttonsbits);

	float m_ranges[20];   //scroll bar increment line ranges
	int GetScrollbarCursorPos(int len, int line, int cursorsize);

	int m_currentline[20];

		  //these pars are need as entry pars by the following function
	int m_x1, m_y1, m_x2, m_y2;   //1
	int m_relativecurrentline;    //2
	int m_cursorstyle;            //3
	int m_numitems;               //4
	int m_topitem;                //5
	int m_currentitem;
	int m_cursorsize;
	int m_maxviewlines;
	int m_cursorpos;
	int m_maxscrollsize;

	TMousePos m_mousefields[4];   //4 mouse fields in the list box:
											//inside area 0
											//up arrow    1
											//down arrow  2
											//scrollbar   3



	//This function is used by the listbox in class WinCtrls
	//for normal listing
	static char *GetText(char *txtlines[],int item);


	//This function is used by class File for listing file entries through
	//the functionality of a listbox in class WinCtrls
	//The par txtlines[] is a dummy for conformity with the listbox pars

	static char *GetFileEntry(char *txtlines[], int item);
	static char *GetDirOrDevEntry(char *txtlines[], int item);
//------------------------------------------------------------------


	int DispListBoxLines(char*(*gettext)(char *txtlines[], int item),
																		  char *txtlines2[]);

	int MoveScrollbarCursorDown(char *(*gettext)(char *txtlines[], int item),
																			 char *txtlines2[]);


	int MoveScrollbarCursorUp(char *(*gettext)(char *txtlines[],int item),
																		  char *txtlines2[]);

	int DoListBox(char *(*gettext)(char *txtlines[], int item),
								  char *txtlines2[], int entrycode=-1,
								  int boxstyle =1, int jmpflg=0);

	int FindLongestLen(char *(*gettext)(char *txtlines[], int item),
																 char *txtlines2[]);

	int ShowDialogBox(char *txtlines[], int numlines,
													int buttonsbits,
													int f_maxviewlines,
													int cursorstyle=1,
													int getout=0, int repeatflg=0);


};
//---------------------------------------------------------------------
#endif

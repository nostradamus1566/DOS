/*-------------------------------------------------------------------------
   (C) Michael Finnegan   15 May 1999
   
-------------------------------------------------------------------------*/

#if !defined __EDITOR_H
#define __EDITOR_H
#include "disk2.h"
#include "labels.h"
	int SetCursor(Disk *a, int sw);
	int TranslateKey(int chr);
	void DispTitle(Disk *a);
	void DispStatusLine(Disk *a);
	void DispScrollBar(Disk *a); //display the editor's scrollbar
	int  DispLines(Disk *a, int topln, int numlines, int row, int attrib);

	int RightFindSpc(char *buf, int pos);
	int FindBegPara(Disk *a, int curline);
	int PerformWrap();
	int ParaWrap1(Disk *a, int line);
	int FindMarker(Disk *a, int stline, int *ln, int *colu);
	int ParaWrap(Disk *a, int *line, int *column);

	int EnterChar(Disk *a, int chr);
	int CursorUp(Disk *a, int chr);
	int CursorDown(Disk *a, int chr);
	int CursorLeft(Disk *a, int chr);
	int CursorRight(Disk *a, int chr);
	int PageDown(Disk *a, int chr);
	int PageUp(Disk *a, int chr);
	int CursorHome(Disk *a, int chr);
	int CursorEnd(Disk *a, int chr);
	int EnterTab(Disk *a, int chr);
	int EnterReturn(Disk *a, int chr);
	int DeleteChar(Disk *a, int chr);
	int EscChar(Disk *a, int chr);
	int DeleteLine(Disk *a, int chr);
	int CutLines(Disk *a, int chr);
	int CopyLines(Disk *a, int chr);
	int PasteLines(Disk *a, int chr);
	int ShowFreeMem(Disk *a, int chr);
   int SetTextCursor(Disk *a);
   int ScrollDownArrow(Disk *a);
   int ScrollUpArrow(Disk *a);
   int ClickInScrollBar(Disk *a);
	int CancelBlock(Disk *a);
#endif
//-----------------------------------------------------------------------


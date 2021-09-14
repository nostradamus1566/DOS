/*-------------------------------------------------------------------------
   (C) Michael Finnegan       15 May 1999
-------------------------------------------------------------------------*/
#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<mem.h>
#include "labels.h"     //for mouse struct

#if !defined __SCREEN_H
#define __SCREEN_H
struct POINT
{
		  int x;
		  int y;
};

struct TMousePos
{
	int x1;
	int y1;
	int x2;
	int y2;
	int available;  //available yes =1 no =0
};



class Screen
{
		  private:
	 //int m_boxlevel;
		  int m_attrib;

		  int m_x1, m_y1;

		  char m_tlc[3];          //styles of box outline
		  char m_trc[3];
		  char m_blc[3];
		  char m_brc[3];
		  char m_hline1[3];
		  char m_hline2[3];
		  char m_vline1[3];
		  char m_vline2[3];

		  char m_spacestring[81];
		  char m_dotstring[81];

		  public:
	 int m_mousehidden;         //0 = no, 1 = yes
		  Screen();

		  int WriteLine(char *text, int x1 =-1, int y1 =-1, int size =-1,
																			 int attrib= -1, int blanks =0);

		  POINT Getxy();

		  int  Setxy(int x, int y);

		  int  DrawBox(int x1, int y1, int x2, int y2,
											int attrib =-1, int style =-1);

		  int ColourLine( int x1, int y1, int len, int attrib =-1);
		  int OpenShadowBox(int x1, int y1, int x2, int y2,
													 int attrib =-1, int style =-1);

		  int ClrScr(int x1, int y1, int x2, int y2, int attrib =-1);

		  int ScrollUp(int x1, int y1, int x2, int y2, int attrib = -1);
		  int ScrollDown(int x1, int y1, int x2, int y2, int attrib);

		  void CursorOff();
		  void CursorOn();
		  int Pcopy(int src, int dest);
		  int OpenBox(int x1, int y1, int x2, int y2,
										  int attrib =-1, int style=-1);


		  void ShowMouse();
		  void HideMouse();
		  void SetMousePos(int x1, int y1);  //set the char pos of the mouse
																									//and show the mouse
		  void LimitMouse(int x1, int y1, int x2, int y2);
		  void CloseBox();
		  int WhichFieldIsMouseIn(TMousePos *field, TMouse *mouse, int numfields);
		  int WhichFieldIsMouseIn2(TMousePos *field, TMouse *mouse, int numfields);
};
//-------------------------------------------------------------------------
#endif

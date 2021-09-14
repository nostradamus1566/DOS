/*-------------------------------------------------------------------------
   (C) Michael Finnegan     15 May 1999
-------------------------------------------------------------------------*/

#include "lines.h"

#define maxtbl       25

#define up_char        30
#define down_char      31
#define left_char      29
#define right_char     28
#define bol_char      200
#define eol_char      201
#define pageup_char   202
#define pagedown_char 203
#define bof_char      204
#define eof_char      205
#define ins_char      206
#define del_char      207
#define nextword_char 208
#define prevword_char 209
#define f1            210
#define f2            211
#define f3            212
#define f4            213
#define f5            214
#define f6            215
#define f7            216
#define f8            217
#define f9            218
#define f10           219

#if !defined KEYBOARD_H
#define KEYBOARD_H
class Keyboard : public Lines         //inherits from class lines
{
		  private:
		  char m_convert[maxtbl][2];        //key translations


		  public:
        void ClrMouse();
		  Keyboard();             //constructor
        int WaitEvent(int t);
        int GetChr();
		  int DelChar(char *text, int pos);     //delete a char from a string
		  int InsertChar(char *text, int pos, char chr);  //insert a char into a string
		  int Input(TMousePos *MousePos, int numfields,
										  char *preanswer, int x1, int y1,
										  int fieldsize = 36, int maxsize = 40,
										  int *cursorpos=0, int attrib = BLUE*16+WHITE,
                                int entrycode = 0, int dots=0, int sneek=0);

			int RightTrim(char *text);    //strips white spaces


			long unsigned int m_longtime;

		  long unsigned int GetLongTime();   //used by GetMouse()
		  public:
		  int GetMouseOrKey();        //see if a mouse or keyboard event occurred
		  int WaitGetMouseOrKey();    //wait for a mouse or keyboard enent to occur
		  int WaitMouseButtonRelease(int timeflg =-1); //wait for the left button to be released

		  void PushTab();     //push a tab char into the keyboard buffer
	 void PushReturn();  //push <return> into keyboard buffer
	 void PushDown();
	 void PushUp();
	 void PushDigit(int digit);    //push a number 0-9 into the keyboard buffer
	 void PushSpace();
	 void PushEsc();
	 void PushF1();           //push help key into buffer
	 void PushF2();           //simulate a click pushed into the buffer
	 void PushF3();
	 void PushDel();
	 void PushF10();          //push menu key into buffer
    void PushA_Z(int ch);          //push key A-Z into buffer


};
//----------------------------------------------------------------------
#endif


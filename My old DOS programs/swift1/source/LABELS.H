/*-------------------------------------------------------------------------
	(C) Michael Finnegan
	Dingle, Co. Kerry, Ireland

	Last update: 25 June 1999
-------------------------------------------------------------------------*/

//The global vars, visable to all files, go here
//each files includes labels.h to see then
extern int gbc[5];
extern int gfc[5];
extern int tablen;
extern char inputbuf[300];
extern char inputbuf2[181];
extern char filemask[300];
extern char tempbuf[300];
extern char filebuf[300];
extern int mousehidden;
extern int m_boxlevel;
extern char editortitle[250];
extern char statusline[160];
extern int row, col, lines, cline;

#if !defined __MOUSE
#define __MOUSE
struct TMouse
{
	int index;                   //time index 0 -3
	long unsigned int DownTime[2];
	long unsigned int UpTime[2];
	long unsigned int Length; //the amount of time that elapsed from when a
																			//button was press to when it was released
	int Recording;         //0 = off, 1 = on
	int x;                  //column position of mouse 0-79
	int y;                  //row position 0-24
	int ButtonState;      //0 = up 1 = down
	int Click;             //0 = no, 1 = yes a click either pressed/released
	int DblClick;          //0= no, 1 = yes  (double round click)
	int CoordChange;        //0 = no, 1 = yes (a change in either x or y)
	int HCoordChange;       //0 = no, 1 =yes (a horazantal change)
	int VCoordChange;       //0 =no, 1 = yes (a vertical change)

};
#endif
extern TMouse mouse;
extern int cursorpos, cursorlen;
#if !defined __TPASTE
#define __TPASTE
struct TPaste
{
   int stline;  //the r # of a text line (the start of the block)
   int stcol;   //the r col of the line (the start of the block)
   int endline;  //the r # of the end line of the block
   int endcol;    //the r col # of the endline
   int validsw;   //whether the block vars are valid or not
};
#endif


extern TPaste block;
extern char linebuf1[170];
extern char linebuf2[170];
extern int lenbuf1;
extern int lenbuf2;
extern int orgtopline;
extern int orgrow;
extern int m_rw ; //save or load file
extern int deskjet; //1 = yes, 0 = dot matrix
extern int lpp; //lines per page
extern int lm;  //left margin 0 or more
extern int maxlen; //maximum text line len 78 or less
extern int helpkey; //esc if the help key is set
extern int m_wordwrap;
extern int mwwo;      //"Match whole word only"
extern int mc;        //"Match Case"
extern char findwhat[181];
extern char replacewith[181];
extern int prevchr;
extern findfound;  //found var for find dialogue box
extern int clickfield; //used by input to tell caller which
							  //field the click occurred in  0 - x

extern int colourscheme; //0 = blue scheme, 1 = black scheme
extern char f_editortitle[260];
extern int f_row, f_col, f_cline;

//-----------------------------------------------------------------------
#if !defined INTR
	#define INTR 36
#endif

#if !defined _SETARGS
	#define _SETARGS
	#ifdef __cplusplus
		#define __CPPARGS ...
	#else
		#define __CPPARGS
	#endif
#endif
extern void interrupt (* oldhandler) (__CPPARGS);
extern int errnum;
extern unsigned char errclass;
extern unsigned char erraction;
extern unsigned char errlocus;
extern int nodialog; //if = 1 tell critical irq not to show message
extern int modified; //1 = text was modified since last save

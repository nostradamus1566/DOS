/*-------------------------------------------------------------------------
   (C) Michael Finnegan    1 Aug 1999
-------------------------------------------------------------------------*/

//The global vars, visable to all files, go here
//each files includes labels.h to see then
#include "labels.h"
int gbc[7];
int gfc[7];
int lines;
int cline;
int row, col;
int mousehidden;
int m_boxlevel;
int tablen; //num of spaces a tab represents
char inputbuf[300];
char inputbuf2[181];
char filemask[300];
char tempbuf[300];
char filebuf[300];
char editortitle[250];
char statusline[160];
TMouse mouse;
int cursorpos, cursorlen;
TPaste block;
char linebuf1[170];
char linebuf2[170];
int lenbuf1;
int lenbuf2;
int orgtopline;
int orgrow;
int m_rw; //load or save 0 = load, 1 = save
int deskjet;  //1 = yes, 0 = dot matrix
int lpp; //lines per page
int lm; //left margin 0 to 10.
int maxlen; //maximum text line length:  78 or less
int helpkey; //escape if help key is set
int m_wordwrap;
int mwwo;  //"Match Whole Word Only"
int mc;    //"Match Case"
char findwhat[181];
char replacewith[181];
int prevchr;
int findfound; //found var for find dialogue box
int clickfield; //used by input to tell caller which
					 //field the click occurred in  0 - x

int colourscheme; //0 = blue scheme, 1 = black scheme
char f_editortitle[260];
int f_row, f_col, f_cline;
void interrupt (* oldhandler) (__CPPARGS);
int errnum;
unsigned char errclass;
unsigned char erraction;
unsigned char errlocus;
int nodialog; //if = 1 tell critical function not to show the message
int modified; //1 = text was modified since last save

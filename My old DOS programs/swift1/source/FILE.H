/*-------------------------------------------------------------------------
   (C) Michael Finnegan    15 May 1999
-------------------------------------------------------------------------*/

#if !defined __FILE_H
#define __FILE_H
#include "disk2.h"
#include "labels.h"

class File
{
   public:
   char m_buffer[185];
   int m_buflen;
   int m_eof;    //1 = end of file reached
   int m_pastchar;

   TMousePos m_MousePos[15];
 
   int m_linewidth;
   char m_filemask[128];
   Disk m_f, m_d;
   int m_focus;
   char m_path[270];
   int m_numfiles;
   int m_numdirs;
   int m_colour[2];
   char m_linewidthbuf[10];  //but to hold text of ine width during input

   void ShowInputLine(Disk *a, char *buf, int cursorstyle);
   void FocusFilesList(int cursorstyle);  //focus or unfocus
   void FocusDirsList(int cursorstyle);   //focus or unfocus
   void FocusWordWrap();      
   void FocusLineWidth(int cursorstyle);
   void FocusButton(Disk *a, int button, int pressed); //focus on one of the
                                                       //3 buttons

   int EvalPathOrName(Disk *a, char *filenamebuf);
   void ShowMouseCoordinates(Disk *a);
        File();
   void OpenOpenBox(Disk *a);
   int OpenJunior(Disk *a, char *namebuf);
   int Open(Disk *a);
   int Save(Disk *a);
   int SaveAs(Disk *a);
   int InputLineWidth(Disk *a);     //allow the user to enter a new line length
	int LoadFile(Disk *a, char *name); 
   int SaveFile(Disk *a, char *name);
   int GetChar(Disk *a, int handle);
   int AddBuf(char *string, int num);
	char *getname(Disk *a, char *namebuf);

};

#endif
//-----------------------------------------------------------------------


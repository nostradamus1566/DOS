/*-------------------------------------------------------------------------
   (C) Michael Finnegan    15 May 1999
-------------------------------------------------------------------------*/

#include "strings.h"
extern "C" {
#include "memory.h"
}
#if !defined LINES_H
#define LINES_H

class Lines : public String         //inherits from class screen
{
		  private:

		  public:
        int m_lines;        //key translations
        int m_cline;

         int InitMem();
        int GetLineBuf(char *textbuf, int *cline);
        int OverWriteLine(char *textbuf, int *cline);
        int InsertLine(char *textbuf, int *cline);
        int DeleteLines(int *clines, int *numlines);
        int AppendLine(char *textbuf);
        int GetLines();
};
//----------------------------------------------------------------------
#endif


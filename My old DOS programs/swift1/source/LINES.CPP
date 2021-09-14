//--------------------------------------------------------------------
// (c) Michael Finnegan       15 May 1999
//--------------------------------------------------------------------

//include the asm functions that are defined in memory.asm
//These class functions only call the functions in the memory.asm file
//


#include "lines.h"
#include "labels.h"

//---------------------------------------------------------------------
int Lines::InitMem()
{
	_initmem();
   m_lines = Lines::GetLines();
   m_cline =1;
	cline = 1;
	editortitle[0] =0;
	col = 1; row = 1; //init global screen vars
	modified = 0;
	return 0;
}
//---------------------------------------------------------------------
int Lines::GetLineBuf(char *textbuf, int *cline)
{
	return _getlinebuf(textbuf, cline);
}
//----------------------------------------------------------------------
int Lines::OverWriteLine(char *textbuf, int *cline)
{
	modified = 1;
	return _overwriteline(textbuf, cline);
}
//------------------------------------------------------------------------
int Lines::InsertLine(char *textbuf, int *cline)
{
	modified =1;
	return _insertline(textbuf, cline);
}
//------------------------------------------------------------------------
//----------------------------------------------------------------------
int Lines::DeleteLines(int *cline, int *numlines)
{
	modified =1;
	return _deleteline(cline);
}
//------------------------------------------------------------------------
int Lines::AppendLine(char *textbuf)
{
	modified =1;
	return _appendline(textbuf);
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
int Lines::GetLines()
{
	return _getlines();
}
//------------------------------------------------------------------------

/*-------------------------------------------------------------------------
   (C) Michael Finnegan     15 May 1999
-------------------------------------------------------------------------*/

#if !defined __STRINGS_H
#define __STRINGS_H

//#include <stdio.h>
//#include <stdlib.h>
//#include <conio.h>
//#include <string.h>

#include <ctype.h>
#include "Screen.h"

class String : public Screen
{
	public:
		char m_spacebuf[255];

		int LeftFindChar(char *buf, char chr, int pos); //search a string
																		//from the left for
																		//given character
																		//return r position
																		//where found
		int RightFindChar(char *buf, char chr, int pos); //search from right
		int LeftTrim(char *buf);
		int RightTrim(char *buf);
		int InsertChar(char *buf, char chr, int pos);
		int AppendChar(char *buf, char chr);
		int AppendString(char *buf, char *string);
		int InsertString(char *buf, char *string, int pos);
		int DelChars(char *buf, int pos, int numchars);
		int GetLeftString(char *dest, char *src, int numchars);
		int GetRightString(char *dest, char *src, int numchars);
		int GetMidString(char *dest, char *src, int pos, int numchars);
		int EnterChar(char *buf, char chr, int pos, int overwriteflg);
		int CompareStrings(char *s1, char *s2, int mwwo, int mc);
		int IsBlank(char *buf); //returns nonzero if string is blank or spaces
};
#endif
//-----------------------------------------------------------------------

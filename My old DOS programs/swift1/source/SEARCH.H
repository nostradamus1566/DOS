//-----------------------------------------------------------------
// (c) Michael Finnegan     9 June 1999
//-----------------------------------------------------------------

#include "disk2.h"

extern int mwwo;    //match whole word only
extern int mc;      //match case
extern char findwhat[181];  //text to find
int FindText(Disk *a);
int RepeatFindText(Disk *a);
int ReplaceText(Disk *a);
//----------------------------------------------------------------------

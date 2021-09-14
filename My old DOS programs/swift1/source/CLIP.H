//-----------------------------------------------------------------
// (c) Michael Finnegan     4 June 1999
//-----------------------------------------------------------------

#include "disk2.h"


int CopyToClip(Disk *a, int clear=0);
int CutToClip(Disk *a, int clear=0);
int PasteClip(Disk *a);
int ClearCut(Disk *a);
long unsigned int CalcPrevBytes(Disk *a);
int GetLineColumn(Disk *a, int *l, int *c, long unsigned int bytes);
//----------------------------------------------------------------------

/*-------------------------------------------------------------------------
   (C) Michael Finnegan   2 June 1999
-------------------------------------------------------------------------*/

#if !defined __HELP_H
#define __HELP_H
#include "file.h"

struct TDate
{
   int date, month, year;
   int hour, minute, second;
   long int progage;
   char day_str[10];
   char month_str[13];
};


class Help
{

   public:
      void About(Disk *a);        //show the about message box
      void Commands(Disk *a);     //show the commands message box
};
//-------------------------------------------------------------------
#endif

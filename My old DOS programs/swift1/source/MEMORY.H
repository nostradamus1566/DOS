//---------------------------------------------------------------------
// (c) Michael Finnegan      15 May 1999
//
/*---------------------------------------------------------------------
  
  see "memory.asm" for the definitions.
*/

#ifndef __MEMORY_H
#define __MEMORY_H

   void _initmem(void);
   unsigned int _getfreemem();
   int _getlinebuf(char *textbuf, int *cline);
   int _overwriteline(char *textbuf, int *cline);
   int _insertline(char *textbuf, int *cline);
   int _deleteline(int *cline);
   int _appendline(char *textbuf);
   int _getlines();
#endif

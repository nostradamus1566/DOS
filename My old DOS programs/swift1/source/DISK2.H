/*-------------------------------------------------------------------------
   (C) Michael Finnegan    15 May 1999
  
-------------------------------------------------------------------------*/
#if !defined _DISK2_H
#define _DISK2_H

#include "winctrls.h"
#include<ctype.h>
extern "C"{
#include "disk.h"
						 }
#include<direct.h>

extern char currentpath[];      //defined in disk.asm

//disk.h is the header file for the asm functions in disk.obj
// menu.h includes kyeboard, which in turn includes screen.h
class Disk : public WinCtrls
{

		  public:
			  char m_devslist[26];       //buffer for letters for each drive
			  int m_numdevs;             //number of devices on system
			  int m_dirs;                //number of dirs in directory
			  int m_files;               //number of files in directory
			  int m_enteries;            //total number of dirs & files
			  int m_getentry;            //flag; was dir read?
		 char m_freespace[15];      //will hold string telling # of bytes
											 //free when GetFreeSpace() is called
	 public:
			  int m_driveStatus;       //if an error look here for detailes
																					 //0 = ok
																					 //1 = reading error
																					 //2 = writing error

			  Disk();                 //constructor
			  int GetDirectory(char *path, char *spec);    //reads in & sorts dirs
																																		  //and files
		 char *GetPath();       //returns the adr of a buffer containing
										//the current path

		 int ReadEntry(char *usersbuf, int entrynum);  //read an entry from
																	 //fardata into uses's buf


			  int IsDriveReady(int flg);  //see if drive is ready by testing
																						 //its readibility and or writeability
																						 //if flg =0 (ignore writeability)
																						 //returns 0 ok
																						 //        1 no floppy drive present
																						 //        2 not readable
																						 //        3 not writeable
		 int commaShortNumber(char *numbuf, unsigned int number, int justify);
		 int commaLongNumber(char *numbuf, unsigned long number, int justify);
		 unsigned long GetFreeSpace();

			  int OpenFile(char *name, int access, int *handle, int createflg);
			  int WriteFile( int handle, char *buffer);
			  int ReadFile( int handle, int numbytes, char *buffer,
																													 int *actualbytes);
			  int CloseFile(int handle);

};
//---------------------------------------------------------------------
#endif

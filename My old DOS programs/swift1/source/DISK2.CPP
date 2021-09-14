/*-------------------------------------------------------------------------
   (C) Michael Finnegan  15 May 1999
   
-------------------------------------------------------------------------*/



#include "disk2.h"
//--------------------------------------------------------------------
//This routine reads in the list of devices attatched to the system
//and stores them in m_devslist[]
//and the number of devices in m_numlist

Disk::Disk()
{
	char dev , sav, t, i;

	sav = getdisk();       //get current drive to restore later
	i = 0;
	for(t = 0; t< 25; t++){
	  dev = t + 65;

	  if(dev == 'B') continue;
	  if(_chdrive(dev - 64) == 0)
	  {
		 m_devslist[i] = dev;
		 i++;
	  }
   }
   m_devslist[i] = 0;
   m_numdevs = i;
	setdisk(sav);              //restore original disk
   m_getentry = 0;            //flg: can't get entry yet cause no
							  //directory read yet
   m_driveStatus =0;          //drive ok
}
//--------------------------------------------------------------------
//This function reads in the directory of the disk and sorts
//The [DIRS] and files
//Entry:
// path = something like "C:\WINDOWS"  or "D:\"
// spec = something like "*.*
// There must be a '\' on the end of path
//If a drive is specified other than the curent one this function
//changes to that drive
//Exit:
//  return the total number of enteries (dirs + files)

int Disk::GetDirectory(char *path, char *spec)
{
   int i;
	char linebuf1[128];
	char dev;

   m_enteries = 0;        //reinitialise vars
   m_dirs = 0;            //this indicates we are disregarding the previous
   m_files = 0;           //dir values
   m_driveStatus =0;      //clr previous error

	strcpy(linebuf1, path);
   dev = linebuf1[0];
   dev = dev & 95;         //convert to uppercase

//if we are reading from drive a, first check that its disk is readable
	if(dev == 'A')
	{
	   i = this->IsDriveReady(0);  //check if drive a is readable
	   if(i)
	   {
		  m_driveStatus =1;      //cant read
		  return 0;             //error drive not  ready
		}                        //return no enteries
   }


   _set_path(linebuf1);
   strcpy(linebuf1, spec);
   _sort_all_enteries(linebuf1);

	m_dirs = _getnumdirs();
   m_files = _getnumfiles();
   m_enteries = m_dirs + m_files;
   if(m_enteries) m_getentry =1;
   else m_getentry =0;

   return m_enteries;
}
//----------------------------------------------------------------------
//This function returns the address of the currentpath[]
//where the full current path is stored (defined in disk.asm)

char * Disk::GetPath()
{

	currentpath[0] =0;
   _getcurrentpath();   //_currentpath() returns = 1 if error
   
   return currentpath;
}
//-----------------------------------------------------------------------
//This function reads an entry from  a previously read in directory
//call Disk::GetDirectory() to read in a directory before calling
//this function or else it will fail.
//Entry:
//  usersbuf must be at least 22 chars long to hold the entry
//  entrynum is the c # of the entry to read
//
//Exit:
// ax = -1 if function failed
// else ax = r # of enteries held in fardata

int Disk::ReadEntry(char *usersbuf, int entrynum)
{

   if(!m_getentry) return -1;        //no directory read in yet

   if(entrynum <0 ) return -1;
	if(entrynum >=m_enteries)  return -1;

	_fetch_stored_entry(usersbuf, entrynum);

   return m_enteries;
}
//---------------------------------------------------------------------
//Check to see if there is the floppy drive A is readable and or
//writeable
//Entry:
//  if sw =0 igore  writeaility test
//Exit:
// ax = 0 ok
// ax = 1 no floppy drive present
// ax = 2 not readable
// ax = 3 not writeable

int Disk::IsDriveReady(int flg)
{
   return _is_drive_ready(flg);
}
//----------------------------------------------------------------------
//This function converts an short integer into a string and
//if the integer is > 1,000 it inserts commas into the string
//to make format the number  ie 16376 becomes "16,376"
//Entry:
//  numbuf = buffer reserved for string to be returned
//  number = the unsigned int number to be converted
//  justify =0 left justify
//           1  right justify

int Disk::commaShortNumber(char *numbuf, unsigned int number, int justify)
{

	int t, i;
   char *p;

   sprintf(numbuf, "%5u", number);
   p = &numbuf[0];      //points to zero at end of string

   if(number > 999)
	{
		 t = 4;              //4 bytes to shift right


	   //open string to insert (first coma from right)
	   do {
			 t--;
			 *(p + t +3) = *(p +t +2);


	   } while(t);

	   *(p+2) = ',';
	}

   if(!justify)       //remove any prefixs paces justify =0: leftjustify
   {

	   if(*(p) != 32) return 0;    //no spaces to remove

		//find first digit
		t = 0;
	   do{
		   t++;
		} while( *(p+t) == 32);

	  //t = the pos where the first digit was found
	  i =0;
	  do{
		  *(p+i) = *(p+i+t);
		  i++;
	  } while( *(p+i-1));          //loop until z byte is stored
   }

   return 0;
}
//------------------------------------------------------------------------
//This function converts an unsigned long integer into a string and
//if the integer is > 1,000 it inserts commas into the string
//to make format the number  ie 16376 becomes "16,376"
//Entry:
//  numbuf = buffer reserved for string to be returned
//  number = the unsigned int number to be converted
//  justify =0 left justify
//           1  right justify

int Disk::commaLongNumber(char *numbuf, unsigned long number, int justify)
{

	int t, i;
	char *p;

	sprintf(numbuf, "%10lu", number);
	p = &numbuf[0];      //points to zero at end of string

	if(number >= 1000)
	{
		 t = 4;              //4 bytes to shift right


		//open string to insert (first coma from right)
		do {
			 t--;
			 *(p + t +8) = *(p +t +7);


		} while(t);

		*(p+7) = ',';
	}


	if(number >= 1000000)
	{
		 t = 8;              //8 bytes to shift right


		//open string to insert (2nd coma from right)
		do {
			 t--;
			 *(p + t +5) = *(p +t +4);


		} while(t);

		*(p+4) = ',';
	}


	if(number >= 1000000000)
	{
		 t = 12;              //12 bytes to shift right


		//open string to insert (3rd coma from right)
		do {
			 t--;
			 *(p + t +2) = *(p +t +1);


		} while(t);

		*(p+1) = ',';
	}




	if(!justify)       //remove any prefixs paces justify =0: leftjustify
	{

		if(*(p) != 32) return 0;    //no spaces to remove

		//find first digit
		t = 0;
		do{
			t++;
		} while( *(p+t) == 32);

	  //t = the pos where the first digit was found
	  i =0;
	  do{
		  *(p+i) = *(p+i+t);
		  i++;
	  } while( *(p+i-1));          //loop until z byte is stored
	}

	return 0;
}
//------------------------------------------------------------------------
/* This routine returns the amount of free k on the disk           */
/* The current_path_string must have been previously set up        */

unsigned long Disk::GetFreeSpace()
{
long freespace;
unsigned int rax;
unsigned int rbx;
unsigned int rcx;
unsigned int rdx;

long totalspace;
long bpc, temp;
asm{
				mov     ax, 1900h          //"Get current disk"
				int     21h
				mov      dx, ax
				inc      dl
											//make sure its a char
										  //dx=current drive
				mov     ax, 3600h                //get disk space info
				int     21h
				mov     rax, ax       //sectors per cluster
				mov     rbx, bx       //# of available clusters
				mov     rcx, cx       //bytes per sector
				mov     rdx, dx       //total # of clusters
	}
if( rax == 65535) freespace =0;        /* no free space */

else
{
 bpc = rax;                              /* sectors per cluster */
 bpc = bpc * rcx;                        /* bytes per sector    */
 temp = bpc * (rbx & 32767);          /* mul by total # of avail clusters*/

	if(rbx >32767) {
	 temp = temp + (bpc * 16384);
	 temp = temp + (bpc * 16384);
	}
 freespace = temp;
 bpc = rax;
 bpc = bpc * rcx;                /* calc bytes per cluster  */
 temp =bpc * (rdx & 32767);     /* mul by total clusters */

	if(rdx > 32767) {
	 temp = temp +( bpc * 16384);
	 temp = temp +(bpc * 16384 );
	}

 totalspace = temp;
 }   /* end of ELSE block  */

	this->commaLongNumber(m_freespace, freespace, 1);


	return freespace;
}
/*----------------------------------------------------------------------*/

//This example demonstrates how to open a file for either reading or writing
//
//Entry:
//  name = the name of the file to open (not the drive and path)
//  access = 0 reading, 1 = writing
//  if createflg =0 opens an existing file
//  if creatflg = 1 creates a new file, or if it already exists
//     the function truncates it to zero length when it opens it.

//Exit:
//   ax = 0 of ok and handle = the handle assing to this file
//   else ax =1 error

int Disk::OpenFile(char *name, int access, int *handle, int createflg)
{



asm{
		  push di
		  push si
					 }
		  _GetDosVer();


		  //call _getdosver    //get version

asm{		  cmp  al, 7        //if less than dos 7 use old
		  jb   old          //method of opening file

		  mov  ax, 716ch             //open file new way (long name way)
		  mov  bx, access
		  mov  cx, 0                 //normal file attribute
		  mov  dx, createflg
		  cmp  dx, 1
		  je  a2                  //create file, or delete if exists
		  mov  dx, 0001h         //only open don't create if non existant
		  jmp  a3 }
a2:asm{   mov  dx, 0010h}      //create a new file if it doesnt already exist
a3:asm{   mov  si, name
		  mov  di, 1           //alias hint
		  int  21h
		  jnc  ex             //successful: no error ax = handle

//unsuccessful trying to open a new file cause file of same name exists.
//try opening an existing file for either reading or writing
		  mov si, name
		  mov ax, 716ch
		  mov cx, 0           //normal file (not hidden)
		  mov bx, access      //read or write
		  mov dx, 0001h       //assume open for reading
		  cmp bx, 0          //open for reading
		  je  fn
		  mov dx, 0002h     //open for writing, deleting previous version
		  jmp fn }


old:asm{  mov  dx, name        //file name
		  mov  ax, access         //read or write
		  mov  cx, 0              //normal file
		  mov bx, createflg
		  cmp  bx, 1
		  jne  a1                 //don't create it, just open it
		  mov  ax, 3c00h         //create file
		  jmp  fn}

a1:asm{   mov ah, 3dh}             //open file for either reading or writing
fn:asm{   int 21h  }

ex:asm{  mov si, handle
		  mov  [si], ax

		  pop si
		  pop di
		  jc  er}
noer:  return 0;
er:    return 1;
}
//--------------------------------------------------------------------
//closes a previously opened file
//Entry:
//  handle = handle # of perviously opened file
//Exit:
//  ax = 0 no error
//  else ax =  error  code

int Disk::CloseFile(int handle)
{
asm {
		 push di
		 push si
		 mov bx, handle
		 mov ah, 3eh
		 int 21h
		 jc  ex                //error
		 mov ax, 0 }

ex:asm{  pop si
		 pop di  }
}
//---------------------------------------------------------------------
//This function writes to a previously opened file
//Entry:
//  handle
//  numbytes r # of bytes to write
//  buffer = the offset buffer where the text to be written to disk is
//           held.
//
// Exit:
//   ax = 0 if no error
//   ax = 05h  access denied
//        06h  illegal handle


int Disk::WriteFile( int handle, char *buffer)
{
  int numbytes = strlen(buffer);
  if(!numbytes) return 1;    //buffer empty

asm{
	  push di
	  push si
	  mov ah, 40h    //write to file
	  mov bx, handle
	  mov cx, numbytes
	  mov dx, buffer
	  int 21h
	  jc  ex              //error: return code in ax
	  mov ax, 0 }         //ax=no error

ex:asm{pop si
		pop di }

}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//This function reads from a previously opened file
//Entry:
//  handle
//  numbytes r # of desired bytes to read in.
//  buffer = the offset buffer where the text to be read into from disk
//
//
// Exit:
//   ax = 0 if no error
//   and actualbytes = the r # of bytes read in.
//   ax = 05h  access denied
//        06h  illegal handle


int Disk::ReadFile( int handle, int numbytes, char *buffer, int *actualbytes)
{
asm{

	  push di
	  push si
	  mov si, actualbytes    //init the actual # of bytes read in
	  xor ax, ax
	  mov [si], ax

	  mov ah, 3fh    //read from file
	  mov bx, handle
	  mov cx, numbytes
	  mov dx, buffer
	  int 21h
	  jc  ex              //error: return code in ax
	  mov si, actualbytes
	  mov [si], ax         //return the actual # of bytes read
	  add ax, dx
	  mov  bx,ax
	  xor ax, ax
	  mov [bx],al      //nul terminate buffer
				 }         //ax=no error

ex:asm{ pop si
		pop di
	  }
}
//---------------------------------------------------------------------
//This is a demonstration of how to use
// OpenFile
// WriteFile
// ReadFile
// CloseFile
/*
int main()
{

   int i, handle, access, createflg, actualbytes;
   char linebuf1[81];
   clrscr();
   strcpy(linebuf1, "test this for a long name.txt");
   access =1;             //writing access
   createflg =1;

   i =OpenFile(linebuf1, access, &handle, createflg);
   if(i)
	  printf("Error opening for writing file\n");


   else    //try writing something to the file
   {
	  printf("File opened for writing ok\n");
	  printf("handle number is %d\n", handle);
	  strcpy(linebuf1,"Hello michael!\n");
	  i= WriteFile(handle, linebuf1);
	  if(i)
		printf("Problem writing to file\n");
	  else
		printf("Ok writing to file\n");

   }

   i = CloseFile(handle);
   if( i)
	 printf("problem closing file\n");
   else
   {
	 printf("File closed ok\n");

	  //now try reading back from the file
	   strcpy(linebuf1, "test this for a long name.txt");
	  i =OpenFile(linebuf1, 0, &handle, 0);
	  if(i)
		printf("Error opening file for reading\n");


	  else    //ok opening file for reading. Now read something in
	  {
		 printf("File for reading opened ok\n");
		 printf("handle number is %d\n", handle);

		 i= ReadFile(handle, 20, linebuf1, &actualbytes);
		 if(i)
			printf("Problem writing to file\n");
		 else
		 {
		   printf("Ok reading in okt\n");
		   printf("I read in [%s]\n", linebuf1);
		   printf("the len =%d\n", actualbytes);
		 }

	  }
   }

   i = CloseFile(handle);

   return 0;
}
//----------------------------------------------------------------------
*/


//-----------------------------------------------------------------------
/* demonstration of how to use dir related functions in class DISK
int main()
{
	char linebuf1[271];
	gfc = LIGHTGRAY;
	gbc = BLUE;
	int attrib, chr, i, t, s, x, len;

	attrib = gbc *16 +gfc;
	Disk a;

	//a.ClrScr(0, 0, 79, 24, attrib);
	a.DrawBox(0, 1, 79, 24, attrib, 1);
	a.ShowTopMenu(1);             //show top menu off
	a.WriteLine("Hello world.", 1, 2, -1, attrib);

	i = a.GetTopMenuItem();
	if(i != -1)
	{
	   i = a.GetSubMenuItem();       //show menu box for that top menu item

	}
	   */
	/*
	i = a.GetDirectory("a:\\swift\\", "*.* ");
	if(i >0)
	{
	   for( t = 0; t< i; t++){
		  a.ReadEntry(linebuf1, t);
		  printf("%3d %s\n", t+1, &linebuf1[9]);
		  chr = getch();
	   }

	}

	else
	{
	   if(a.m_driveStatus)
	   {
		  printf("Problem reading floppy drive\n");
		}
		else printf("no list\n");
	}


	x = 40;
	a.WriteLine("File name? ", x + 0, 10, BLUE+16 +WHITE);
	strcpy(linebuf1, "noname.txt");
	len = strlen(linebuf1);
	i= a.Input(linebuf1, x+ 11, 10, 13, 13,  len, BLUE*16+WHITE, 0);
	//if(i == -1 || i >0)  no valid input


	//strcpy(linebuf1, "C:\\WINDOWS\\SYSTEM\\");
	int ndirs, nfiles, j;
	long unsigned free;
	strcpy(linebuf1, "c:\\windows\\");

	clrscr();
	gotoxy(1,1);

	a.GetDirectory(linebuf1, "*.*");



   ndirs = a.m_dirs;
   t = ndirs + a.m_files;

   gotoxy(1,1); printf("%s\n", currentpath);

   for ( j = 0; j < t; j++){

	 i = a.ReadEntry(linebuf1, j);

	 printf("%3d %s ", j, &linebuf1[9]);

	 if((ndirs >0) && j< ndirs) printf(" <DIR>");
	 printf("\n");

	 chr = getch();
  }
	   printf("finished.\n");
	   a.GetFreeSpace();

	   printf("%s bytes free\n", a.m_freespace);

	  chr = getch();

 }
*/
//------------------------------------------------------------------------










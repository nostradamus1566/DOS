//-----------------------------------------------------------------------
// (c) Michael Finnegan 4 June 1999
//-----------------------------------------------------------------------

#include "strings.h"
#include <ctype.h>
//----------------------------------------------------------------------
//compare two strings to see if string 2 is wholly contained in
//string 1.
// Enter:
//  mwwo = match whole word only
//  mc = match case
//if mc = 0 convert both strings to lower case
//if mwwo = 1 the result of the comparsion is false if
//            there is a letter immediately to the left of the position
//            where the matchs was found or a letter immediately after the
//            where the last letter of the match was found.
//
// Exit:
//  ax = 0 if match uncessful, else
//  ax = the r # pos where the match was found in string 1

int String::CompareStrings(char *s1, char *s2, int mwwo, int mc)
{
	int len1, t, len2, flg, ch1, ch2;

	flg = 0; //assume no match found
	char *p, *p1, *p2, *offset;
	char tempbuf1[130];
	char tempbuf2[130];


	len1 = strlen(s1);
	len2 = strlen(s2);
	if(!len1) goto ex;
	if(!len2) goto ex;

	strcpy(tempbuf1, s1);
	strcpy(tempbuf2, s2);
	p1 = &tempbuf1[0];
	p2 = &tempbuf2[0];
	offset = p1;
	//if the "match case" is off convert the two strings to lower case
	if(!mc)
	{
		for(t = 0; t< len1; t++){   //convert string1
			p1[t] = tolower(p1[t]);
		}

		for(t = 0; t< len2; t++){   //convert string2
			p2[t] = tolower(p2[t]);
		}
	}

b1:
  p = strstr(p1, p2);
  if(!p) goto ex;  //string2 not in string1

  //we have found a match, but if mwwo is on then the match is only
  //valid if
  //        1. The match is at the beginning of the string and the next
  //            char after the match is not a letter.
  //     0r 2. The match is not at the beginning of the string and the
  //           char prior to the start of the match is not a letter and
  //           the char after the match is not a letter.
  flg = (p - &tempbuf1[0])+1; // r pos where match starts in string1
  if(!mwwo) goto ex;  //"Whole Word Only" is off so we found a valid match

  if(flg ==1)  //the match is at the beginning of the string, check up on
  {            //char after the last char of the match
a1: ch1 = *(p+len2);
	 if(isalnum(ch1))
	 {
										//a digit or a letter found after the end of
										//the match so if we are not at the end
										//of the string we must keep searching
c1:	 if(((flg-1)+len2)+len2 <=len1)
		 {
			p1 = offset + (flg-1) +len2;
			flg =0;
			goto b1;     //keep searching
		 }
		 else
		 {         //we've come to the end of the string
			 flg =0; //without finding a case match
			 goto ex;
		 }

	 }
	 //if not digit or letter  flg is unaltered, either case exit.
	 goto ex;

  }
  //the match is not at the beginning of the string. check the char prior
  //to the match
  ch1 = *(p-1);
  if(!isalnum(ch1))
  {
	  //The prior char is not a digit or letter: good.
	  //check the char at the end
	  goto a1;
  }

  //The char prior is a digit or string
  //so skip to next word
  goto c1;



ex:
	return flg;
}
//----------------------------------------------------------------



//----------------------------------------------------------------------
//scan a string to see if it contains white spaces
//if so return 1
// else if it contains a visable char return 0
int String::IsBlank(char *buf)
{
	int len, i, t;
	char ch;
	i =1;       // assume string is blank
	len = strlen(buf);
	if(!len) goto ex;  //blank

	for(t =0; t<len; t++){
		ch = buf[t];
		if(!isspace(ch))   //if it's not a space exit
		{
			i =0;
			break;
		}
	}

ex:
	return i;
}
//-----------------------------------------------------------------------
//Search a string from the left starting from c position pos
//looking for char chr, if found return the r position of it, else return 0

int String::LeftFindChar(char *buf, char chr, int pos)
{
	int len;
	char *p;

	len = strlen(buf);
	if(!len) return 0;
	if((pos+1) > len) return 0; // past end of string

	p = strchr(&buf[0], chr);
	if(!p) return 0;

	return 1+(p- buf);
}
//---------------------------------------------------------------------
//Search a string from c position pos, scanning backwards until
//the beginning of the string.
//If found return the r pos of it, else return 0
//If the string is "Hello world" and you want to find the first space
//the entry parameters would be
//chr = 32, pos = strlen(buf);
//i = RightFindChar(buf, chr, pos);
//if the space is found i = 6 (the r pos of the space in buf count
//from the beginning of buf
//Remember pos = the c position from the beginning of buf where you want
//to start the bakwards search
//exit:
//  ax =0 chr not in buf
//  else ax = the r pos from the beginning of buf where ch was found

int String::RightFindChar(char *buf, char chr, int pos)
{
	int len, found;
	char *p;

	len = strlen(buf);
	if(!len) return 0;
	if(pos > len) return 0;  //past end of string

	found = 0;
	p = &buf[pos];
	do{
		if(*p == chr) {found = 1; break;}
		p--;
	}while(p >= buf);

	if(!found) return 0;
	else
		return 1+(p - buf);
}
//----------------------------------------------------------------------
//Cut prefix tabs, spaces and returns from buf
//Exit:
//Return r length of string
int String::LeftTrim(char *buf)
{
	int len, t, found;
	char *p, *p2;

	len = strlen(buf);
	if(!len) return 0;
	p = buf;
	found =0;
	for(t = 0; t<len; t++){
		if(!isspace(*p)) {found = 1; break;} // found a non-space
		p++;
	}

	if(!found)    //no non-spaces found, (string is all spaces)
	{             //so del string
		buf[0] = 0;
		return 0;
	}
	if(p == buf) return len; //no space at beginning of string to cut

	p2 = buf -1;         //starting from the first non-white space, shift the
	p--;
	do{                  //string left to the beginning of buf, in this way
		 *(++p2) = *(++p);   //you remove the prefix white spaces
	}while(*p);
	len = (p2 - buf);

	return len;
}
//---------------------------------------------------------------------
//Cut suffix tabs, spaces and returns from buf
//Exit:
//Return r length of string

int String::RightTrim(char *buf)
{
	int len, t, found;
	char *p, *p2;

	len = strlen(buf);
	if(!len) return 0;
	p = &buf[len-1];
	found =0;
	for(t = len; t> 0; t--){
		if(!isspace(*p)) {found = 1; break;}  //found a non-space
		p--;
	}
	if(!found)    //no non-spaces found, (string is all spaces)
	{              //so del string
		buf[0] =0;
		return 0;
	}

	if(p == &buf[len-1]) return len; //no space at beginning of string to cut

	len = (p - buf) +1;
	buf[len] =0;         //cut suffix white spaces from buf
	return len;
}
//----------------------------------------------------------------------
int String::AppendChar(char *buf, char chr)
{
	int len;

	len = strlen(buf);
	buf[len] = chr;
	buf[len+1] =0;
	return len+1;
}
//-----------------------------------------------------------------------
//Append a string to buf
//Exit:
//  ax = r len of new length of buf
//  if ax =0 an error occurred
int String::AppendString(char *buf, char *string)
{
	int lenbuf;
	int lenstring;

	lenbuf = strlen(buf);
	lenstring = strlen(string);
	if(!lenstring) return 0;         // no string to append
	strcat(buf, string);

	return lenbuf + lenstring;
}
//--------------------------------------------------------------------
//Insert a char into buf at c position pos
//characters at and to the right of pos are shifted to make room for chr
//Exit:
//  ax = len of buf
//  if ax = 0 an error occurred
int String::InsertChar(char *buf, char chr, int pos)
{
	int t, len, j;
	char *p;

	len = strlen(buf);
	if(pos > len) return 0;

	if(!len)       //the string is empty so just ad a char
	{              // and return the r len of the string
		return this->AppendChar(buf, chr);
	}
	//shift all the chars from c position pos to end of buf right
	//to make room for the new character
	p = &buf[len];    //p = adr of zero at end of string
	j = (len+1) - pos;   // j = r # chars to shift right
	for(t = 0; t<j; t++){
		*(p+1) = *p;
		p--;
	}
	buf[pos] = chr;

	return len+1;
}
//-------------------------------------------------------------------
//Insert a string into buf at c position pos
//if buf is empty just add the string on
//if pos beyond the buf: error
int String::InsertString(char *buf, char *string, int pos)
{
	int t, lenbuf, lenstring, j;
	char *p, *p2;

	lenbuf = strlen(buf);
	if(pos > lenbuf) return 0;

	if(!lenbuf)       //buf is empty so just add string
	{                 //and return the r len of buf
		return this->AppendString(buf, string);
	}

	lenstring = strlen(string);
	if(!lenstring) return 0;      // no string to insert

	//shift all the chars from c position pos to end of buf right
	//to make room for the new char string
	p = &buf[lenbuf]; //p = adr of zero at end of string
	j = (lenbuf+1) - pos;   //j = r # of chars to shift right
	for(t =0; t < j; t++){
		*(p + lenstring) = *p;
		p--;
	}

	//enter the contents of string into the space we opened up
	p = &buf[pos];
	p2 = string;
	for(t =0; t < lenstring; t++){
		*p++ = *p2++;
	}

	return lenbuf + lenstring;
}
//--------------------------------------------------------------------
//Delete 1 or more chars from anywhere within buf
//pos = the c position of the 1st char to delete
//numchars  = the r # of chars to delete
//if pos  = 0 the chars are deleted from the beginning of buf
//if (pos + numchars) is at the end of the string
//    the chars are cut off from the right side of the string (the string is
//    truncated)
//Otherwise the chars are cut off from the midstring beginning at pos
//until pos+numchars-1
//Exit:
//  ax = the new length of the string
int String::DelChars(char *buf, int pos, int numchars)
{
	int len, t, j;
	char *p, *p2;

	if(!numchars) return 0;    //illegal value

	len = strlen(buf);
	if(!len) return 0;      //no string in buf

	if(pos+1 > len) return 0; //error: past end of string

	if((pos + numchars) >= len) //cut suffix chars from string
	{
		buf[pos] = 0;
		return pos;
	}

	//shift string from c position (pos+ numchars) to end of string back to
	//c position pos
	j = len - (pos +(numchars-1)); //j = the r # of bytes in the block to
												//be shifted left back to c position pos
	p = &buf[pos];
	p2 = &buf[pos+numchars];   //p2 = st adr of block to shift back
	for(t =0; t < j; t++){
		*p++ = *p2++;
	}

	return len - numchars;
}
//---------------------------------------------------------------------
//Copy into dest numchars of src string
//Exit:
//  ax = r len of string
//  if ax = 0 an error occurred
int String::GetLeftString(char *dest, char *src, int numchars)
{
	int len;

	len = strlen(src);
	if(numchars > len) numchars = len;
	if(!numchars)
	{
		dest[0] =0;
		return 0;
	}
	strncpy(dest, src, numchars);

	return numchars;
}
//---------------------------------------------------------------------
//Copy into dest numchars of src string
//starting from the end of the string
//Exit:
//  ax = len of string
//  if ax =0 an error occurred
int String::GetRightString(char *dest, char *src, int numchars)
{
	int len, p;

	len = strlen(src);
	if(numchars > len) numchars = len;
	if(!numchars)
	{
		dest[0] =0;
		return 0;
	}

	p = len - numchars;

	strncpy(dest, &src[p], numchars);

	return numchars;
}
//---------------------------------------------------------------------
//Copy a mid string into dest from c position pos of src
//to copy from pos to end of string numchars can be equal to or greater
//than the length of src. Example numchars can be 32767
//Exit:
//  ax = r len of dest
//  if ax = 0 an error occurred

int String::GetMidString(char *dest, char *src, int pos, int numchars)
{
	int len;

	if(!numchars) return 0;   //error: don't know how many chars wanted
	len = strlen(src);
	if(pos+1 > len) return 0;  // error: pos of 1st char outside of src range
	if((numchars +pos) > len)
	{
		numchars = len - pos;
	}

	strncpy(dest, &src[pos], numchars);

	return numchars;
}
//----------------------------------------------------------------------
//Enter a char into a string at c position pos
//If overwriteflg is on don't insert the char, just overwrite
//the existing char at that pos
//if overwriteflg is off open up memory in the string to make room
//for the char.
//If pos is past the last char in the string by one char then
// append the char to the string
//if pos is past the last char in the string by more than one char
//then fill the gap with spaces and then append chr
//Exit:
// the string will be null terminated
//  ax = the new length of buf
//  if ax =0 an error occurred

int String::EnterChar(char *buf, char chr, int pos, int overwriteflg)
{
	int len, t, j;

	len = strlen(buf);
									//are we past the end of the string?
	if(pos+1 > len) goto a1; //append char to string, but we may have to
									//fill in a gap first if there is a gap
									//before we add the char

//We are within the string. If the overwrite flag is on just
//overwrite the existing char, otherwise we have to open the string
	if(overwriteflg)
	{
		buf[pos] = chr;
		return len;
	}
	else
	{
		return this->InsertChar(buf, chr, pos);
	}

//We are past the end of the string. If there is a gap before we
//add on the char, we must fill the gap with spaces
a1: j = pos -len;  // j = r # of spaces to fill in before adding char
	if(j)
	{
		for(t =0; t< j; t++){
			buf[len+t] = 32;
		}
	}
	buf[pos] = chr;
	buf[pos+1] =0;

	return pos+1;
}
//--------------------------------------------------------------------

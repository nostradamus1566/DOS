/*-------------------------------------------------------------------------
   (C) Michael Finnegan   15 May 1999
   

-------------------------------------------------------------------------*/



int _sort_all_enteries( char *filespec);
int _fetch_stored_entry( char *linebuf1, int num);
int _getnumfiles();
int _getnumdirs();
int _set_path(char *path);        //changes drive and dir
int _is_drive_ready(int sw);      // 0 = test readability only
int _getcurrentpath();
int _GetDosVer();

/*-------------------------------------------------------------------------
   (C) Michael Finnegan      15 May 1999
-------------------------------------------------------------------------*/


#include "keyboard.h"

#if !defined MENU_H
#define MENU_H

struct TMENU
{
        char * txt;   //pointer to name of item
        int len;           //len of text in item name
        char qletter;      //quick letter for item
        int qletterpos;    //position in item name of quick letter
        int qlettersw;    //quick letter on or off
        int barsw;        //horozentol bar under item on/off
        char *exptxt;      //pointer to text that explains option
        int availablesw;    //item available no/yes
};
//-------------------------------------------------------------------
class Menu : public Keyboard     //inherits from keyboard class,
{                                                                 //Keyboard class in turn inherits from
                                                                  //class Screen
	  public:
         TMENU  m_menu[5][8];            //contains menu item details
         int m_topMenuPos[5];            //pos on top line of each item
         int m_subMenuCursorPos[16];      //separate cursor y pos in each menu box
         int m_numOfSubOpts[5];          //num of items in each box
         int m_topMenuCursorPos;           //current menu cursor pos of line menu

         int m_bc, m_fc;                //background & foreground colours of screen


   public:
          Menu();                         //constructor: initialises the items
          int GetTopMenuItem();           //wait for user to cursor select item
          int ShowTopMenu(int sw);        //show top menu on or off

          //show box and items, but don't wait
          int ShowMenuBox(int drawboxflg, int subitemsw=1);
       
          int GetSubMenuItem(int keybrd=1); //wait for user to select an option
			 int MouseMovement(int boxon);  //button held down. wait for mouse move
          int CheckTopMenu();   //see if mouse on top menu item
          int ShowCoordinates();  //for debugging only

          int CopyCoordinates();  //copy from bigmapper into m_boxmapper
          int IsMouseInBox();
          TMousePos m_boxmapper[16];     //sub items to screen rows, cols
          TMousePos m_bigmapper[5][16];  //sun items rows, cols for all boxes



};
//----------------------------------------------------------------------
#endif

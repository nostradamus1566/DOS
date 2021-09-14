//---------------------------------------------------------------------
// Michael Finnegan
// 2 December 1999
//---------------------------------------------------------------------


#include<stdio.h>
#include<conio.h>
#include<math.h>
#include "labels.h"
#include "keyboard.h"
#include "formulas.h"
#include <stdlib.h>

//see the Int() to see how these vars are initialised

struct Money{
	char s[6][16];  //string versions of years, x12, nominal, eff, principal, pmt
				   / /these are filled in the  DisplayFigures() function
	int at;         //actual number of monthly payments
	double t;		//terms
	double x12;     //compounding per year
	double apr;     //nominal rate
	double eff;     //True rate
	double p;      //princiapl
	double pmt;    //monthly payment
	double dpa;    //deferred payment amount
	double ti;     //total interest
	double tip;    //total interest percent
	int focus;     //the item that has the input focus 1 = terms, 2 = arp etc.
	int find;      //1 = terms, 2 = arp, 3 = principal, etc.
	double tmi[360], tmp[360];      //each element holds total monthly interest
											//for year & total monthly principal
	double mi[12], mp[12], mb[12]; //monthly interest, principal & balance
	int boxtype;          // 0 = small box table, 1 = big box table
	int el;               // # of years elements for use w/ money.tim[el]
	int modified;    //do figures need to be recalced? 1 = yes, 0 = no
};

Money money;


//------------------------------------------------------------------------
// This function acts as the coordinater between the functions in the
// formulas.cpp file and the money structure.
// Entry:
//   fine = 1, 2, 3, etc., = the # of the function in the formulas file
//   to call to solve for.
//   The variables that these functions will use come from the
//   global money structure.
//   After caling the desired function, Calc then fills in the values
//   for money.dpa, money.ti, money.tip

int Calc()
{
	double v, fraction, integer, t;
	int mo;



	switch(money.find)
	{
		case 1:  //find number of payments to repay loan
			money.t = Terms(money.p, money.apr, money.pmt, money.x12);
			break;

		case 2: //find # of compounding periods per year
				  //this is skipped (user can only input this, not calculate it)
			break;

		case 3:  //find nominal rate
		case 4:  //and find effective rate
			money.apr = Irate(money.p, money.t, money.x12, money.pmt);
			money.eff = GetEff(money.apr, money.x12);
			break;

		case 5:  //find amount to borrow
			money.p = Pv(money.pmt, money.apr, money.t, money.x12);
			break;
		case 6:
			money.pmt = Payment(money.p, money.apr, money.t, money.x12);
			break;
	}

	//fill in last three values:
	//Total deferred payment amount, total interest paid, total interest%
	money.dpa = money.pmt * money.t;
	money.ti = money.dpa - money.p;
	money.tip = (1- money.p / money.dpa) *100;
	//round to to 2 digit percision
	//money.dpa = Percision(money.dpa, 2);
	//money.ti = Percision(money.ti, 2);
	//money.tip = Percision(money.tip, 2);

	v = money.t;
	fraction = modf(v, &integer);
	if(fraction > .001) integer++;

	double terms = integer;
	money.at = integer;
	double pmt = money.dpa / money.t;
	double balance = money.p;

	double factor = money.apr / (money.x12 *100);
	double totMonI =0;
	double totMonP =0;
	int count =0, x12 = money.x12;
	int i, el = 0;
	int months = terms;        //a whole number
	double monthlyInterest;
	double monthlyPrincipal;
	double oldbalance;

	mo = 12;
	if(money.at < mo) mo = money.at;
	if ( months > 432) months = 432;
	//fill the arrays money.tmi[] & money.tmp[] with the yearly results
	for( i = 1; i <= months; i++){
		monthlyInterest = balance * factor;
		monthlyPrincipal = pmt - monthlyInterest;

		if(monthlyPrincipal >= balance)
		{
			i = months;   //flag: end of loop
			monthlyPrincipal = balance;
			balance =0;
		}
		else
		{
			balance = balance - monthlyPrincipal;
		}

		//collect the data for the month-by-month schedule
		if(i <= mo)
		{
			money.mi[i-1] = monthlyInterest;
			money.mp[i-1] = monthlyPrincipal;
			money.mb[i-1] = balance;

		}

		//collect totals for each years
		totMonI = totMonI + monthlyInterest;
		totMonP = totMonP + monthlyPrincipal;
		count++;
		if(count == x12)   //if end of that year store totals
		{
			el++;
			money.tmi[el-1] = totMonI;
			money.tmp[el-1] = totMonP;
			count = 0;
			totMonI = 0;
			totMonP = 0;
		}
	} //next i

	if(count)
	{
		el++;
		money.tmi[el-1] = totMonI;
		money.tmp[el-1] = totMonP;
	}

	money.el = el;
	money.modified =1;  //for display purposes

	return 0;
}
//-------------------------------------------------------------------------
//Draw all the little boxes that belong on the top half of the screen
int DrawBox1(Keyboard *a, int attrib)
{

	a->DrawBox(0, 0, 34, 7, attrib, 1);


	linebuf1[0] =194;
	a->WriteLine(linebuf1, 21, 0, 1, attrib);
	strcpy(linebuf1, " Number of payments +            ");
	linebuf1[20] = 179;
	a->WriteLine(linebuf1, 1, 1, 33, attrib);
	strcpy(linebuf1, " Compounding/year   +            ");
	linebuf1[20] = 179;
	a->WriteLine(linebuf1, 1, 2, 33, attrib);
	strcpy(linebuf1, " Nominal rate       +            ");
	linebuf1[20] = 179;
	a->WriteLine(linebuf1, 1, 3, 33, attrib);
	strcpy(linebuf1, " True rate          +            ");
	linebuf1[20] = 179;
	a->WriteLine(linebuf1, 1, 4, 33, attrib);
	strcpy(linebuf1, " Amount borrowed    +            ");
	linebuf1[20] = 179;
	a->WriteLine(linebuf1, 1, 5, 33, attrib);
	strcpy(linebuf1, " Periodic payment   +            ");
	linebuf1[20] = 179;
	a->WriteLine(linebuf1, 1, 6, 33, attrib);
	linebuf1[0] = 193;
	a->WriteLine(linebuf1, 21, 7, 1, attrib);


	//draw second three boxes on the upper right half of the screen
	a->DrawBox(38, 0, 77, 6, attrib, 1);
	memset(linebuf1, 196, 40);
	linebuf1[0] = 195;
	linebuf1[26] =197;
	linebuf1[39] = 180;

	linebuf1[40] =0;
	a->WriteLine(linebuf1, 38, 2, 40, attrib);
	a->WriteLine(linebuf1, 38, 4, 40, attrib);


	strcpy(linebuf1, " Deferred payment amount +            ");
	linebuf1[25] = 179;
	a->WriteLine(linebuf1, 39, 1, 38, attrib);
	strcpy(linebuf1, " Total interest          +            ");
	linebuf1[25] = 179;
	a->WriteLine(linebuf1, 39, 3, 38, attrib);
	strcpy(linebuf1, " Total interest percent  +            ");
	linebuf1[25] = 179;
	a->WriteLine(linebuf1, 39, 5, 38, attrib);


	linebuf1[0] = 194;
	a->WriteLine(linebuf1, 64, 0, 1, attrib);
	linebuf1[0] = 193;
	a->WriteLine(linebuf1, 64, 6, 1, attrib);

	strcpy(linebuf1, " ESC = quit,   CrsrUp/Dn = select,   PgUp/Dn = solve for,   Home = mo/yr screen ");
	a->WriteLine(linebuf1, 0, 24, -1, LIGHTGRAY*16+BLACK);
	return 0;
}
//--------------------------------------------------------------------
//Draw the small version of the amortization box
// Before calling this function call Calc() to update the figures in
// the money structure.
// as well as drawing the the small box
// This function displays the Amortization table for 12 months

int DrawAmortBox1(Keyboard *a, int attrib)
{

	int t, months;


	if(money.modified)
	{
		a->DrawBox(0, 9, 79, 23, attrib, 0);        //wipe previous box
		a->DrawBox(0, 9, 40, 23, attrib, 1);
		strcpy(linebuf1, " Period Interest   Principal   Balance");

		linebuf1[41] =0;
		a->WriteLine(linebuf1, 1, 10, 39, attrib);
		months = 12;

		if(months > money.at) months = money.at;
		for(t = 1; t <=months; t++){
			sprintf(linebuf2, "%3d", t);
			sprintf(linebuf1, "%12.2f", money.mi[t-1]);
			strcat(linebuf2, linebuf1);
			sprintf(linebuf1, "%12.2f", money.mp[t-1]);
			strcat(linebuf2, linebuf1);
			sprintf(linebuf1, "%11.2f", money.mb[t-1]);
			strcat(linebuf2, linebuf1);
			a->WriteLine(linebuf2, 1, 10+t, -1, gbc[2]*16+gfc[3]);
		}
	}

	return 0;
}
//--------------------------------------------------------------------
//	Draw the large version of the years box
//	Before calling this function call Calc() to update the figures in
// the money structure.
// As well as drawing the large box
// this function displays the table of total interest,
// principal & balance for the end of each year of the loan's life

int DrawAmortBox2(Keyboard *a, int attrib)
{

	int t;

	if(money.modified)
	{
		a->DrawBox(0, 9, 79, 23, attrib, 1);
		strcpy(linebuf1, " Yr  Interest  Principal    Yr  Interest  Principal    Yr  Interest  Principal");

		linebuf1[80] =0;
		a->WriteLine(linebuf1, 1, 10, 78, attrib);
		for(t = 1; t<= 12; t++){
			if(t <= money.el)
			{
				//years 1 - 12 or less
					sprintf(linebuf1, "%3d %9.2lf %10.2lf   ", t, money.tmi[t-1], money.tmp[t-1]);
				//years 13 to 24 or less
				if(t+ 12 <= money.el)
				{
					sprintf(linebuf2, "%3d %9.2lf %10.2lf   ", t+12, money.tmi[t+12-1], money.tmp[t+12-1]);
					strcat(linebuf1, linebuf2);
				}
				//years 25 to 36 or less
				if(t+24 <= money.el)
				{
					sprintf(linebuf2, "%3d %9.2lf %10.2lf   ", t+24, money.tmi[t+24-1], money.tmp[t+24-1]);
					strcat(linebuf1, linebuf2);
				}

			}
			//el is < 12 blank line
			else
			{
				strcpy(linebuf1, " ");
			}

			a->WriteLine(linebuf1, 1, 10+t, 78, gbc[2]*16+gfc[3]);

		} //next t


	} //end of if (money.modified)

	return 0;
}
//-------------------------------------------------------------------------
// Initialise the program and set up default values for money
int Init(Keyboard *a)
{
	gbc[0] = BLACK;
	gfc[0] = LIGHTGRAY;
	gbc[1] = LIGHTGRAY;
	gfc[1] = DARKGRAY;
	gbc[2] = BLUE;
	gfc[2] = LIGHTGRAY;
	gbc[3] = BLACK;     //figures
	gfc[3] = LIGHTGREEN;

	money.modified =1;
	a->DrawBox(0, 0, 79, 24, gbc[2]*16+gfc[2], 0);
	DrawBox1(a, gbc[2] * 16+ gfc[2]);


   money.x12 = 6;
   money.t = 3 * money.x12;
   money.apr = 21;          //nominal rate
   money.eff = 22.926;       //true rate
   money.p = 999;
	money.pmt = 0;    //figure it out
	money.dpa = 0;   //these 0 values will be replaced when calculated
	money.ti = 0;
	money.tip = 0;
	money.focus = 5; //amount borrowed has input focus
	money.find = 6; // pmt has input focus
	money.boxtype = 0;  // flag: small box table


	return 0;
}
//-------------------------------------------------------------------------
//This function shows the position the find arrow is at

int DisplayArrow(Keyboard *a, int attrib)
{
	int t;
	linebuf1[0] = 27;
	linebuf1[1] =0;
	linebuf2[0] = 32;
	linebuf2[1] = 0;

	for(t = 1; t <=6; t++){
		if(money.find == t)  //arrow ?
			a->WriteLine(linebuf1, 36, t, 1, attrib);
		else
			a->WriteLine(linebuf2, 36, t, 1, attrib);
	}


	return 0;
}
//-------------------------------------------------------------------------
//This function doesn't do any calculation. It just
//displays all the figures in the money structure
//in their proper fields
//It also displays the amortisation table.
int DisplayFigures(Keyboard *a, int attrib, int attrib2)
{
	double v, fraction, integer;

	v = money.t;
	fraction = modf(v, &integer);
	if(fraction > 0.001) integer++;


	sprintf(linebuf1, "%10.f", money.t);
	a->WriteLine(linebuf1, 23, 1, -1, attrib);     //# of payments
	strcpy(&money.s[0][0], linebuf1);

	sprintf(linebuf1, "%10.f", money.x12);   //compounding per year
	a->WriteLine(linebuf1, 23, 2, -1, attrib);
	strcpy(&money.s[1][0], linebuf1);

	sprintf(linebuf1, "%10.3f", money.apr);   //apr (nominal rate)
	a->WriteLine(linebuf1, 23, 3, -1, attrib);
	strcpy(&money.s[2][0], linebuf1);

	sprintf(linebuf1, "%10.3f", money.eff);   //true rate
	a->WriteLine(linebuf1, 23, 4, -1, attrib);
	strcpy(&money.s[3][0], linebuf1);

	sprintf(linebuf1, "%10.2f", money.p);
	a->WriteLine(linebuf1, 23, 5, -1, attrib);
	strcpy(&money.s[4][0], linebuf1);    //principal

	sprintf(linebuf1, "%10.2f", money.pmt);
	a->WriteLine(linebuf1, 23, 6, -1, attrib);
	strcpy(&money.s[5][0], linebuf1);       //pmt

	sprintf(linebuf1, "%10.2f", money.dpa);
	a->WriteLine(linebuf1, 66, 1, -1, attrib);
	sprintf(linebuf1, "%10.2f", money.ti);
	a->WriteLine(linebuf1, 66, 3, -1, attrib);
	sprintf(linebuf1, "%10.2f", money.tip);
	a->WriteLine(linebuf1, 66, 5, -1, attrib);

	//show the focus
	a->ColourLine(23, money.focus, 10, LIGHTGRAY*16+BLACK);
	DisplayArrow(a, attrib);
	//draw the small or box
	if(money.modified)
	{
		if(!money.boxtype) DrawAmortBox1(a, attrib2);
		else DrawAmortBox2(a, attrib2);
	}
	money.modified =0;

	return 0;
}
//------------------------------------------------------------------------


int main()
{
	double result, v, fraction, integer;
	Keyboard a;
	int x1, y1, ch, push, numfields, cursorpos, entrycode, i;
	char preanswer[13], *endptr;
	TMousePos mouse;

	Init(&a);

vtop:

	Calc();
top1:
	DisplayFigures(&a, gbc[3]*16+ gfc[3], gbc[2]*16+ gfc[2]);
	//clrscr();

	gotoxy(24, money.focus+1);
	a.CursorOn();


get:
	ch = a.WaitGetMouseOrKey();
got:
	switch(ch)
	{

		case 13: // <return>
			//recalculate the figures based on the value in the input field
			strcpy(preanswer, &money.s[money.focus-1][0]);
			goto h1;


		case 200: // <home> toggle big/small box
			money.modified =1;
			if(!money.boxtype) money.boxtype = 1;
			else money.boxtype = 0;
			goto top1;
			break;

		case 30:  // <CrsrUp> move input focus up
m1:		money.focus--;
			if(money.focus == 0) money.focus = 6;
			if(money.focus == money.find) goto m1; //move input up again
			goto top1;
			break;

		case 31: //<CrsrDown> move input focus down
m2:		money.focus++;
			if(money.focus == 7) money.focus = 1;
			if(money.focus == money.find) goto m2; //move input down again
			goto top1;
			break;


		case 202: // <PgUP> move arrow up. Avoid meeting input selector
j1:		money.find--;
			if(money.find == 0) money.find = 6;
			if(money.find == 2) goto j1; //can't calc compounding per year
			if(money.find == money.focus)
			{
				a.PushUp();    //do an input secector cursor up
				goto get;
			}
			goto top1;

		case 203: // <PgDown> move arrow down. Avoid meeting input selector
j2:		money.find++;
			if(money.find == 7) money.find = 1;
			if(money.find ==2) goto j2; //can't calc compounding per year
			if(money.find == money.focus)
			{
				a.PushDown();  //do an input selector cursor down
				goto get;
			}
			goto top1;




		case 27: // quit
				goto ex;
				break;

		case 8:  // <del> key pressed: enter input function
			goto g1;
			break;

		default:
			push = -1;           // assume don't push
			if((ch >= '0') && (ch <= '9')) push = ch -48;
			if(ch == '.') push = ch;
			if(push != -1)   //if we should push
			{
				if(push == '.') a.PushA_Z(ch); //the pushed char will be received
				else a.PushDigit(ch -48);       //by input()
g1:
				cursorpos =0;
				preanswer[0] =0;
				entrycode = 614;
				i = a.Input(&mouse, 0, preanswer, 23, money.focus, 10, 10, &cursorpos, LIGHTGRAY*16+BLACK, entrycode);
				//-----------
				//based on the value of i perform the desired operation
				if(i)  //user doesn't want input evaluated.
				{      //Revert to old value at input field, then
						 //Perform the action depending on which key the user
						 //pressed to exit.
						 //reverting to old input happens automatically in
						 //when DisplayFigures() is called
						 switch(i)
						 {
							case 1:  //<esc>
								ch = 27;
								break;
							case 3:
								ch = 30; // cursor up
								break;
							case 4:
								ch = 31; // cursor down
								break;
							case 7:
								ch = 202; //page up
								break;
							case 8:
								ch = 203; // page down
								break;
							case 10:
								ch = 200; // <home>
								break;
							default:
								goto top1;  //only if non of these keys
						 }
						 goto got;  //perform the action the user desired
										//indicated by the key he pressed when he
										//aborted the input routine.
				}
				//-----------
				else
				{  //evaluate the users input and if valid, recalc &
					//display the new figures
					i = i +0;
h1:				result = strtod(preanswer, &endptr);
					//decide which variable in the money structure to assign
					//the newly inputted value to.
					//first round up the old number of payments
					v = money.t;
					fraction = modf(v, &integer);
					if(fraction > 0.001) integer++;
					money.t = integer;
					switch(money.focus)
					{
						case 1:   //number of payments
							money.t = Precision(result, 0);
							break;
						case 2:     //compounding per year
							if(result <1) result =1;
							money.x12 = Precision(result, 0);
							if(money.x12 <1) money.x12 =1;
							else if(money.x12 > 12) money.x12 = 12;
							break;
						case 3:  //nominal rate
							money.apr = result;
							money.eff = GetEff(money.apr, money.x12);
							//if arrow is at eff rate change payment amount
							if(money.find == 4) money.find = 6;
							break;
						case 4:  //true rate
							money.eff = result;
							money.apr = GetApr(money.eff, money.x12);
							//if arrow is at nominal rate change payment amount
							if(money.find == 3) money.find = 6;


							break;
						case 5:
							money.p = result;
							break;
						case 6:
							money.pmt = result;
							break;
					}
					goto vtop; //recalc and display new figures

				} //end of else{} for storing the returned result from input

			}// end of (if pushed = -1)

			goto get;
	} //end of main switch()

ex:
	clrscr();
	a.DrawBox(0, 0, 79, 24, BLACK*16+LIGHTGRAY, 0);
	gotoxy(1, 24);
	a.CursorOn();

	return 0;
}
//----------------------------------------------------------------------


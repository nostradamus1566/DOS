//----------------------------------------------------------------------
// Michael Finnegan
// 2 December 1999
//
// This file contains a collection of functions for
// solving percentages and loans etc. It is needed by
// the borrow program. This is a stand-alone file. You can use these
// formulas in your own programs.


#include <stdio.h>
#include <math.h>
#include "formulas.h"
//----------------------------------------------------------------------
// Given the nominal rate (apr) return the effective rate (true rate)
// Entry:
//   apr = flat rate
//   x12 = compounding periods per year
double GetEff(double apr, double x12)
{
	double x, y, result;

	x = 1+ (apr / 100 / x12);
	y = x12;

	result = (pow(x, y) -1) *100;

	return result;
}
//------------------------------------------------------------------------
// Given the effective rate (true rate) return the apr (nominal rate)
// Entry:
//   eff = effective rate
//   x12 = compounding periods per year
double GetApr(double eff, double x12)
{
	double x, y, result;

	x = 1 +(eff / 100);
	y = (1 / x12);
	result = (pow(x, y) -1) * x12 * 100;

	return result;
}
//------------------------------------------------------------------------
// calculate the apr rate (nominal rate)
// Entry:
//    p     = principle  (the amount borrowed)
//    terms = total # of payments
//    x12   = compounding periods per year, usually 12
//		pmt   = amount of each payment
//
//Exit:
// results are to 3 digits after decimal point
// for example, if p = œ4667, terms = 48 months, and periodic pmt = œ127
// the result is ARP 13.772%

double Irate(double p, double terms, double x12, double pmt)
{
	double app, x, y, lastopr, ny, np, stp, Pmt, i, result;

	app = pmt ; //app = the payment we are to target
	i = .001;

	stp = 0.01;
	lastopr = 1;  //last operation on stp: -1 = smaller, +1 = bigger


top:

		i = i + (stp* lastopr);
		//calculate payment
		x = (i/x12+1);
		y = -1 *terms;
		x = 1 - pow(x, y);
		Pmt = (p * (i/x12)) /x;
		//round payment
		if((Pmt >= (app -.0001 )) &&  (Pmt <= (app + .0001))) goto found;

		if(Pmt <  app)
		{

			if(lastopr == -1)
			{
				stp = stp /2;
				lastopr = 1;
				goto top;
			}

			stp = stp * 2;
			goto top;
		}

		if( Pmt > app)
		{
			 if(lastopr == 1)
			 {
				 stp = stp /2;
				 lastopr = -1;
				 goto top;
			 }

			 lastopr = -1;

			 goto top;
		}


found:
		i = i * 100;

	result = i;
ex:
	return result;   //nominal rate  (ie., 13.772%)
}

//----------------------------------------------------------------------
// This function calculates the amount of the monthly payment
// Entry:
//	  p     = principal  999
//   apr   = (ie., 41.94%)
//	  terms = total # of payments
//   x12   = compounding periods per year
// Exit:
//   result = œ71.74  (precision is to 2 digits after decimal point)

double Payment(double p, double apr, double terms, double x12)
{
	double x, y, Pmt, i, ny, np;


	i = apr / 100;

	x = i / x12+1;
	y = -1 * terms;
	x = 1- pow(x, y);
	Pmt = (p *(i / x12)) / x;


	return Pmt;           //monthly payment amount
}
//----------------------------------------------------------------------
//This function finds the amount of time in years needed to
//repay a loan
// Entry:
//	  p = principal (the amount borrowed)
//   arp = the yearly interest rate (ie., 13.772%)
//   pmt = the amount of each payment (ie., œ127)
//   x12 = compounding periods per year
// Exit:
//   the result is (ie., 48 months )
//    result is significant to a precision of 2 digits after decimal point

double Terms(double p, double apr, double pmt, double x12)
{
	double  result, t, i, x1, x2, n;

	n = x12;      //assume 12 payments per year
	i = apr / 100;
	x1 = 1- ((p * i) / (n * pmt));
	x2 = 1 + (i/n);
	t = (- (log(x1) / (n * log(x2)))) * x12;


	//result = Precision(t, 0);
	result = t;

	return result;
}
//-----------------------------------------------------------------------
//This is the formula for figuring out the present value of a loan.
//Example:
// suppose you know a loan of œx at an nominal rate of 13.772% over 48 months
// will cost œ6096 what is the present value of the loan today,
// in this example the present value is œx = 4667

double Pv(double pmt, double apr, double terms, double x12)
{
	double i, x, y, p, result;


	i = apr / 100;
	x = (i / x12 +1);
	y = -1 * terms;
	x = (1 - pow(x, y)) * pmt;

	p = x * x12 / i;
	result = p;

	return result;
}
//------------------------------------------------------------------------
// This function rounds a number to a certain number of digits
// after the decimal point
// Enter:
//  val = number to round
//  p = precision
// Example:
//  x = Precision(12.453, 2);        x = 12.45
double Precision(double val, int p)
{
	double dec[4] = {1, 10, 100, 1000};
	double x, result;


	x= val * dec[p] +.5;
	modf(x, &result);  result = result / dec[p];

	return result;
}
//------------------------------------------------------------------------

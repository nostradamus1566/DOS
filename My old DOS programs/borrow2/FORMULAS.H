#if !defined __FORMULAS_H
#define __FORMULAS_H

double GetEff(double apr, double x12);
double GetApr(double eff, double x12);

double Irate(double p, double years, double x12, double pmt);  //find APR
double Payment(double p, double apr, double terms, double x12); //find amount
                                                       //of periodic payment

double Terms(double p, double apr, double pmt, double x12); 
double Pv(double pmt, double apr, double terms, double x12); //find present
                                                             //value
double Precision(double val, int p);              //round a number up
//-------------------------------------------------------------------------
#endif

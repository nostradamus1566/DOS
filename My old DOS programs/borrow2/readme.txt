 

Welcome to 'Borrow.exe' vers 2 
2 December 1999



There are quite a number of financial or APR calculator programs available on the Internet. Two of the well know ones at the time of writing  are Wheatworks Spread Calculator, and AprCalc 3.0. Both of these are available from the popular shareware sites such as www.download.com. or from the websites of the authors: www.wheatworks.com or www.DefinitiveSolutions.com

Unfortunately those programs are shareware and there is no source code included. I prefer free software with the source code.

Borrow is freeware and comes with the full source code written in C++.

It can tell you:
1) how many months it will take to repay the loan 

2) the nominal rate% 

3) the effective rate%

3) the amount you can borrow, if you supply it with figures for the number of instalments spread over the loan's life, the interest rate (either the nominal or the effective rate) and amount of each regular fixed payment. For example. If the cost of the loan is £6096 and the nominal is 13.772% and you decided to pay over 4 years, it will tell you you can borrow £4667

4) amount of the periodic payment if you supply figures for the number of instalments per year, and the number of times the interest is compounded each year (usually every month), the interest rate can be expressed as either the nominal rate or the effective rate, and the amount you want to borrow. This is the most frequently used option.

As well as that the program automatically displays an amortization schedule for the first 12 instalments showing a breakdown of the fixed regular payment. It shows the amount of interest on the declining, outstanding balance of the loan at the end of each instalment period, and the amount of the loan you are repaying out of the fixed regular instalment. It also shows the outstanding loan balance. If you press the <home> key you can toggle the display. Then it shows the total interest payable for each year and the total principal payable for each year of the loan's life. These figures are useful when you need to deducted the annual mortgage interest on a loan from taxable income.  

The latest version contains two new features. You can now enter either the nominal or the effective interest rate. In America they call the nominal rate the APR%, but in Ireland & the UK we call the APR% the effective or true rate. For example, if you see a product, such as a computer, advertised in an Irish or British paper at an apr of 16%, this means the effective interest rate. Version 1 of the borrow program only accepted the nominal rate. Instead of having to figure out the conversion you can how enter the apr directly into the effective rate input field and the program will automatically display the nominal rate and the rest of the results, including the amortisation schedule.

The second feature is the option to change the number of compounding periods per year. In version 1 of the program it was fixed at 12. But it is quite common to repay loan instalments every 2 months, or quarterly. For example if you bought a computer through the ESB in Ireland you would be paying every two months with your electricity bill, and the term apr% in their advertisement would not refer to the American definition, but the effective interest rate. Before deciding which computer to go for you might like to consider a number of options, such as paying over 3 years or 2 years, and you might also like to compare ESB's figures with those of a regular TV, Video and computer shop that charges different rates, and requires you to make the regular instalments every month.  

If you are into programming in C++ and have Borland's Turbo C++ 3.0 compiler you will be able to customise and improve the program. You can also use the 4 financial formulas in the file formulas.cpp in your own programs. 


#include <stdio.h>
#include <conio.h>
void main()
{
	FILE *out;
	int topln, botln, chr, l, lpp, lines, page, sides, actualpages;
	int extrablankside, lasteven;
	int fileopened;

	fileopened = 0; //printer file not opened yet
	page =0;
	lpp = 62;
	lines = 159;
	extrablankside = 0;   //assume extra blank side not needed

	lasteven =0;          //assume last page is odd
	actualpages = 1+ ((lines-1) / lpp);
	sides = actualpages;
	//if the last page is even set lasteven to 1, else the last
	//page is odd, so clear lasteven
	asm {
		  mov ax, actualpages
		  test ax, 1
		  jne con
		  inc lasteven
		 }

con:if(actualpages >2)
	 {
		 if(lasteven) //last page is even and greater than 2
		 {            //last odd page will be one less so print lines for
						  //last odd page, and each previous odd page until
						  //we reach odd page number 1.
			 botln = ((actualpages-1)* lpp)
n1:
			 topln = botln - (lpp-1);
			 for(t = topln; t<= botln; t++){
				printf("%t\n", t);
			 }
			 if(topln >1) //print previous odd page
			 {
				botln -= (lpp *2);
				goto n1;
			 }
			 //we have printed all the odd pages now ask user to rotate
			 //stack of paper and print the even numbered pages starting
			 //from page 2
			 printf("rotate paper\n");
			 chr = getch();
			 if(chr == 27) goto ex;
			 //---------------
			 topln = lpp+1;
n2:
			 botln = topln + (lpp-1);
			 for(t = topln; t<= botln; t++){
				 if( t <= lines ) printf("%d\n", t);
				 else printf("\n");
			 }
			 topln += lpp;
			 if(topln <= lines) goto n2;
			 //we have finished print odd and even pages
			 goto ex;

		 }
		 //---------------------------------------------------
		 //number of pages to print a greates than 2
		 //and the last page is odd.
		 //last even page will be one less. Later ee will need to feed out
		 //a blank even page to get at the last even page.
		 //but first lets prints all the odd numbered pages starting
		 //from the last odd page until we reach page 1.
		 topln = 1+((actualpages-1) * lpp);
n3:
		 botln = topln+ (lpp-1);
		 for(t =topln; t<= botln; t++){
			if(t<= lines) printf("%d\n", t);
			else printf("\n");
		 }
		 if(topln > 1)
		 {
			 top -= lpp;
			 goto n2;
		 }
		 //we have now printed out all the odd numbered pages.
		 //ask the user to rotate paper, them feed out a blank page
		 //then start printing all the even numbered pages
		 printf("rotate paper\n");
		 chr = getch();
		 if(chr == 27) goto ex;
		 //feed out a blank page
		 for(t = 1 t <=(lpp+2); t++){
			printf("\n\r");
		 }
		 topln = lpp+1;
n4:
		 botln = topln + (lpp-1);
		 for(t = topln; t<= botln; t++){
			printf("%d\n\r", t);
		 }
		 if(botln < (actualpages-1)*lpp))
		 {
			topln += lpp;
			goto n4;
		 }
		 //all odd and even pages printed out.
		 goto ex;



	 }

	 //The actual number of pages to print is either 1 or 2
	 //print out the first page as normal
	 for(t = 1; t<= lpp; t++){
		if(t <= lines) printf("%d\n\r", t);
		else printf("\n\r");
	 }

	 if(actualpages == 1) goto ex;
	 //ask user to rotate the page
	 printf("Rotate page\n\r");
	 chr = getch();
	 if(chr == 27) goto ex;
	 topln = lpp+1;  //top of second page
	 botln = topln +(lpp-1);
	 for(t = topln; t<= botln; t++){
		if(t <= lines) printf("%d\n\r", t);
		else printf("\n\r");
	 }






ex:
	if(fileopened) fclose(out);

	chr = getch();
}
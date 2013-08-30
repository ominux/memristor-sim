/*
 * xpoint_w_selector.c
 *
 *  Created on: Aug 14, 2013
 *      Author: Cong Xu
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*
#include <time.h>
#ifndef M_PI
#define M_PI 3.14159265359
#endif
float drand(){
    return (rand()+1.0)/(RAND_MAX+1.0);
}

float random_normal(){
    return sqrt(-2*log(drand())) * cos(2*M_PI*drand());
}

float random_gaussian(float my_mu, float my_sigma){
    return my_mu + my_sigma * random_normal();
}
*/

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}

int main(int argc, char *argv[])
{
	int r,c,sel_r,sel_c,w_selector;
	float y_lrs,y_hrs,v_w;
	r = atoi(argv[1]);
	c = atoi(argv[2]);
	//sel_r = atoi(argv[3]);
	//sel_c = atoi(argv[4]);
	y_lrs = atof(argv[3]);
	y_hrs = atof(argv[4]);
	v_w = atof(argv[5]);
	w_selector = atoi(argv[6]);

	/* Default: select the farthest cell */
	sel_r = r;
	sel_c = c;

	char filename[30] = "xpoint_";
	char * surfix;

	if (w_selector)
	  strcat(filename,"w_sel_");
	else
	  strcat(filename,"wo_sel_");
	surfix = itoa(r, 10);
	strcat(filename, surfix);
	strcat(filename,"x");
	surfix = itoa(c, 10);
	strcat(filename,surfix);
	strcat(filename,"_w.sp");
	//surfix = itoa(sel_r, 10);
	//strcat(filename,surfix);
	//strcat(filename,"c");
	//surfix = itoa(sel_c, 10);
	//strcat(filename,surfix);
	//strcat(filename,".sp");
	FILE *fp = fopen(filename,"w");

	fprintf(fp,"*Array size %d*%d y_lrs:%.5f y_hrs:%.5f Vw: %.3f\n",r,c,y_lrs,y_hrs,v_w);
	//fprintf(fp,"*Selected row:  %d Selected column: %d\n",sel_r,sel_c);

	fprintf(fp,".subckt nl_tamem top bot\n");
	fprintf(fp,".param b1=7.2e-6, b2=4.7, yInit=0.4, gON=0.025\n");
	fprintf(fp,"Gtamem top bot CUR='V(top,bot)*(gON*yInit+(1-yInit)*b1*exp(b2*sqrt(abs(V(top,bot)))))'\n");
	fprintf(fp,".ends nl_tamem\n");
	fprintf(fp,"\n");
	
	if (w_selector){
		fprintf(fp,".subckt selector top bot\n");
		fprintf(fp,".param a=1e-11, b=0.7, c=0.15\n");
		fprintf(fp,"Gsel top bot CUR='a*sinh(V(top,bot)/b)*exp(abs(V(top,bot))/c)'\n");
		fprintf(fp,".ends selector\n");
		fprintf(fp,"\n");
	}

	int i,j;
	for (i=1;i<=r;i++)
	  for (j=1;j<=c;j++){
		  if (w_selector){
			fprintf(fp,"Xs%dt%d w%dt%d m%dt%d selector\n",i,j,i,j,i,j);
		    fprintf(fp,"Xm%dt%d m%dt%d b%dt%d nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_lrs);
		  //if (i==sel_r && j==sel_c)
			//fprintf(fp,"Xm%dt%d m%dt%d b%dt%d nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_hrs);
		  //else
		  }else{
			  fprintf(fp,"Xm%dt%d w%dt%d b%dt%d nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_lrs);
		  }
	  }
	fprintf(fp,"\n");

	for (i=1;i<=r;i++){
		if (i==sel_r)
		  fprintf(fp,"Rwr%dt0 wst0 w%dt1 0.65\n",i,i);
		else
		  fprintf(fp,"Rwr%dt0 wht0 w%dt1 0.65\n",i,i);
		for (j=1;j<=c-1;j++)
		  fprintf(fp,"Rwr%dt%d w%dt%d w%dt%d 0.65\n",i,j,i,j,i,j+1);
	}
	fprintf(fp,"\n");

	for (j=1;j<=c;j++){
		if (j==sel_c)
		  fprintf(fp,"Rbr0t%d b0ts b1t%d 0.65\n",j,j);
		else
		  fprintf(fp,"Rbr0t%d b0th b1t%d 0.65\n",j,j);
		for (i=1;i<=r-1;i++)
		  fprintf(fp,"Rbr%dt%d b%dt%d b%dt%d 0.65\n",i,j,i,j,i+1,j);
	}
	fprintf(fp,"\n");

	fprintf(fp,".param vw=%.3f\n",v_w);
	fprintf(fp,"\n");
	fprintf(fp,"Vwh wht0 GND 'vw/2'\n");
	fprintf(fp,"Vws wst0 GND 'vw'\n");
	fprintf(fp,"\n");
	fprintf(fp,"Vbh b0th GND 'vw/2'\n");
	fprintf(fp,"Vbs b0ts GND 0\n");
	fprintf(fp,"\n");

	fprintf(fp,"Vtest test GND 'vw/2'\n");
	if (w_selector){
		fprintf(fp,"Xstest test middle selector\n");
		fprintf(fp,"Xmtest middle GND nl_tamem yInit=%.6f\n",y_hrs);
	} else
		fprintf(fp,"Xmtest test GND nl_tamem yInit=%.6f\n",y_hrs);

	fprintf(fp,".param tab = 123000\n");
	fprintf(fp,".DC tab 123000 123000 100\n");
	fprintf(fp,".print vd_sel=par('V(Xm%dt%d.top)-V(Xm%dt%d.bot)') vd_half_sel=par('V(Xmtest.top)-V(Xmtest.bot)') i_selr=par('-I(Vws)')\n",sel_r,sel_c,sel_r,sel_c);
	fprintf(fp,"*i_hselr=par('-I(Vwh)') i_selc=par('-I(Vbs)') i_hselc=par('-I(Vbh)')\n");
	fprintf(fp,"*.op\n");
	fprintf(fp,".end\n");
	fclose(fp);
	return 0;
}


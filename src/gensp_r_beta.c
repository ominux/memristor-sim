/*
 * gensp_r_beta.c
 *
 *  Created on: Oct 04, 2013
 *      Author: Cong Xu
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}

int main(int argc, char *argv[])
{
	int r,c,w_selector,r_ron_rwire;
	float y_lrs,y_hrs,v_r;

        if (argc == 8) {
	    r = atoi(argv[1]);
	    c = atoi(argv[2]);
	    y_lrs = atof(argv[3]);
	    y_hrs = atof(argv[4]);
	    v_r = atof(argv[5]);
	    w_selector = atoi(argv[6]);
            r_ron_rwire = atof(argv[7]);
        } else {
           printf("format: #row, #column, y_lrs, y_hrs, read voltage, selector, ration of Ron/Rwire\n");
        }
 
	/* Default: read the farthest cell */
	int sel_r = r;
	int sel_c = c;

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
	strcat(filename,"_r.sp");

        float gON = 0.025;
        float b1 = 7.2e-6;
        float b2 = 4.7;
        float von = 0.8;
        float ron = 1 / (gON * y_lrs + (1 - y_lrs) * b1 * exp (b2 * sqrt(von)));
        float res_wire = ron / r_ron_rwire;

	FILE *fp = fopen(filename,"w");

	fprintf(fp,"*Array size %d*%d y_lrs:%.5f y_hrs:%.5f Vw: %.3f\n",r,c,y_lrs,y_hrs,v_r);
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
			fprintf(fp,"Xs%dt%dh w%dt%dh m%dt%dh selector\n",i,j,i,j,i,j);
			//if (i==sel_r && j==sel_c)
			if (i==sel_r)
			  fprintf(fp,"Xm%dt%dh m%dt%dh b%dt%dh nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_hrs);
			else
			  fprintf(fp,"Xm%dt%dh m%dt%dh b%dt%dh nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_lrs);
		  }else{
			//if (i==sel_r && j==sel_c)
			if (i==sel_r)
			  fprintf(fp,"Xm%dt%dh w%dt%dh b%dt%dh nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_hrs);
			else
			  fprintf(fp,"Xm%dt%dh w%dt%dh b%dt%dh nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_lrs);
		  }
	  }
	fprintf(fp,"\n");

	for (i=1;i<=r;i++){
		if (i==sel_r)
		  fprintf(fp,"Rwr%dt0h wst0h w%dt1h %.6f\n",i,i,res_wire);
		else
		  fprintf(fp,"Rwr%dt0h wut0h w%dt1h %.6f\n",i,i,res_wire);
		for (j=1;j<=c-1;j++)
		  fprintf(fp,"Rwr%dt%dh w%dt%dh w%dt%dh %.6f\n",i,j,i,j,i,j+1,res_wire);
	}
	fprintf(fp,"\n");

	for (j=1;j<=c;j++){
		if (j==sel_c)
		  fprintf(fp,"Rbr0t%dh b0tsh b1t%dh %.6f\n",j,j,res_wire);
		//else
		  //fprintf(fp,"Rbr0t%dih b0thh b1t%dh %.6f\n",j,j,res_wire);
		for (i=1;i<=r-1;i++)
		  fprintf(fp,"Rbr%dt%dh b%dt%dh b%dt%dh %.6f\n",i,j,i,j,i+1,j,res_wire);
	}
	fprintf(fp,"\n");

	fprintf(fp,".param vr=%.3f\n",v_r);
	fprintf(fp,"\n");
	fprintf(fp,"Vwuh wut0h GND 0\n");
	fprintf(fp,"Vwsh wst0h GND 'vr'\n");
	fprintf(fp,"\n");
	//fprintf(fp,"Vbh b0th GND 'vw/2'\n");
	fprintf(fp,"Vbsh b0tsh GND 0\n");
	fprintf(fp,"\n");

	for (i=1;i<=r;i++)
	  for (j=1;j<=c;j++){
		  if (w_selector){
			fprintf(fp,"Xs%dt%dl w%dt%dl m%dt%dl selector\n",i,j,i,j,i,j);
			//if (i==sel_r && j==sel_c)
			if (i==sel_r)
			  fprintf(fp,"Xm%dt%dl m%dt%dl b%dt%dl nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_lrs);
			else
			  fprintf(fp,"Xm%dt%dl m%dt%dl b%dt%dl nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_hrs);
		  }else{
			//if (i==sel_r && j==sel_c)
			if (i==sel_r)
			  fprintf(fp,"Xm%dt%dl w%dt%dl b%dt%dl nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_lrs);
			else
			  fprintf(fp,"Xm%dt%dl w%dt%dl b%dt%dl nl_tamem yInit=%.6f\n",i,j,i,j,i,j,y_hrs);
		  }
	  }
	fprintf(fp,"\n");

	for (i=1;i<=r;i++){
		if (i==sel_r)
		  fprintf(fp,"Rwr%dt0l wst0l w%dt1l %.6f\n",i,i,res_wire);
		else
		  fprintf(fp,"Rwr%dt0l wut0l w%dt1l %.6f\n",i,i,res_wire);
		for (j=1;j<=c-1;j++)
		  fprintf(fp,"Rwr%dt%dl w%dt%dl w%dt%dl %.6f\n",i,j,i,j,i,j+1,res_wire);
	}
	fprintf(fp,"\n");

	for (j=1;j<=c;j++){
		if (j==sel_c)
		  fprintf(fp,"Rbr0t%dl b0tsl b1t%dl %.6f\n",j,j,res_wire);
		//else
		  //fprintf(fp,"Rbr0t%dil b0thl b1t%dl %.6f\n",j,j,res_wire);
		for (i=1;i<=r-1;i++)
		  fprintf(fp,"Rbr%dt%dl b%dt%dl b%dt%dl %.6f\n",i,j,i,j,i+1,j,res_wire);
	}
	fprintf(fp,"\n");

	fprintf(fp,"*.param vr=%.3f\n",v_r);
	fprintf(fp,"Vwul wut0l GND 0\n");
	fprintf(fp,"Vwsl wst0l GND 'vr'\n");
	fprintf(fp,"\n");
	//fprintf(fp,"Vbh b0thl GND 'vw/2'\n");
	fprintf(fp,"Vbsl b0tsl GND 0\n");
	fprintf(fp,"\n");

	fprintf(fp,".param tab = 123000\n");
	fprintf(fp,".DC tab 123000 123000 100\n");
	fprintf(fp,".print vd_sel_h=par('V(Xm%dt%dh.top)-V(Xm%dt%dh.bot)') i_selr_l=par('-I(Vwsl)') i_selc_l=par('I(Vbsl)') i_selc_h=par('I(Vbsh)')\n",sel_r,sel_c,sel_r,sel_c);
	fprintf(fp,"*.print vd_sel_l=par('V(Xm%dt%dl.top)-V(Xm%dt%dl.bot)') i_selr_h=par('-I(Vwsh)')\n",sel_r,sel_c,sel_r,sel_c);
	fprintf(fp,"*.op\n");
	fprintf(fp,".end\n");
	fclose(fp);
	return 0;
}


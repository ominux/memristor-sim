#!/usr/bin/python

import sys
from pylab import *

def parse(ifilename,y_lrs,y_hrs,iScaleFactor):
	log = open(ifilename,'r')
	ofilename = ifilename.split('.')[0]+".res"
	ofile = open(ofilename,'w')

	yl = float(y_lrs)
	yh = float(y_hrs)
	scaling = float(iScaleFactor)

	for line in log:
		if line.find('123.00000k') != -1:
			#print "result ",line.find('123.00000k')
			data = line.split()
			#print data
			print "Voltage drop on selected memristor (worst-case): "+data[1]+"V"
			print "voltage drop on half-selected memristor (best-case): "+data[2]+"V"
			#print "Total current on selected row (origin): "+data[3]+"A"
			dr_v = returnValue(data[1])
			dr_i_org = returnValue(data[3])
			dr_i = 1e6 * dr_i_org / scaling
			print "Total current of selected row (scaled): %fuA" % dr_i
			t_w = calLatency(yl,yh,dr_v)
			if (t_w < 0):
				print "voltage too small to switch the cell"
				sys.exit(2)
			else:
				t_w = 1e9 * t_w
			#set up minimum switching time to be 0.3ns (reported in ITRI HfOx memristor
			if (t_w < 0.3):
				t_w = 0.3
			print "Switching time of the selected memristor (worst-case): %f ns" %t_w
			ofile.write("voltage(V) %f \n" % dr_v)
			ofile.write("current(uA) %f \n" % dr_i)
			ofile.write("latency(ns) %f \n" % t_w)
	
	log.close()
	ofile.close()
	
def returnValue(s):
	unit_s = s[-1]
	num_s = s[:-1]
	if (unit_s == 'k'):
		return 1e3 * float(num_s)
	elif (unit_s == 'm'):
		return 1e-3 * float(num_s)
	elif (unit_s == 'u'):
		return 1e-6 * float(num_s)
	elif (unit_s.isdigit()):
		return float(s)
	else:
		print "Bogus unit in ",s
		sys.exit(2)

def calLatency(y_lrs,y_hrs,v_w):

	y_th = y_hrs * pow(y_lrs/y_hrs,0.8)
	# set time steps
	dt=1e-11
	tf=1e-6
	
	#set parameters
	b1=7.2e-6
	b2=4.7
	gon=0.025
	aon=1e-4
	aoff=4e-9
		
	sigvon=0.45
	sigpon=4e-5
	sigvoff=0.013
	sigpoff=500
	
	#set initial value
	t=0
	y = y_hrs * pow(y_lrs/y_hrs,0.04)
	#print "y_hrs ",y_hrs
	#print "y_lrs ",y_lrs
	#print "v_w ",v_w
	#print "yinit ", y
	#print "y_th ", y_th
	i=v_w*(gon*y+(1-y)*b1*exp(b2*sqrt(fabs(v_w))))
	dydt = aon*exp(-1*(y/y_hrs)**2)*sinh(v_w/sigvon)*exp(v_w*i/sigpon)
	switched=False
	
	while t<tf:
		dydt = aon*exp(-1*pow((y/y_hrs),2))*sinh(v_w/sigvon)*exp(v_w*i/sigpon)
		y = y + dydt*dt
		if y>y_th:
			y=y_th
			switched=True
			break
		i=v_w*(gon*y+(1-y)*b1*exp(b2*sqrt(fabs(v_w))))
		t = t + dt
		
	if switched:
		#print "t ",t
		return t
	else:
		return -1

if __name__ == "__main__":
	if len(sys.argv) != 5:
		print "Usage: extract.py <log file> <y_lrs> <y_hrs> <scaling factor of cell current>\n"
	else:
		parse(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4])

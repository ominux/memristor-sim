#!/usr/bin/python

# Solve a differential equation using the Euler-Cromer method

from pylab import *
import sys

y_lrs = 0.4
y_hrs = 0.06
v_w = 0.72

if len(sys.argv) < 4:
    print "usage: cal_t_w.py <y_LRS> <y_HRS> <v_w>\n"
    sys.exit(2)
else:
    y_lrs = float(sys.argv[1])
    y_hrs = float(sys.argv[2])
    v_w = float(sys.argv[3])

y_th = (y_lrs + y_hrs) / 2     

# make empty lists
tlist = [] 
ilist = []
ylist = []
vlist = []

# set time steps
dt=1e-12
tf=1e-5

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
print "y_hrs ",y_hrs
print "y_lrs ",y_lrs
y=y_hrs + 0.01 * (y_lrs - y_hrs)
print "yinit ", y
i=v_w*(gon*y+(1-y)*b1*exp(b2*sqrt(fabs(v_w))))
dydt = aon*exp(-1*(y/y_hrs)**2)*sinh(v_w/sigvon)*exp(v_w*i/sigpon)

#append inital values to lists
tlist.append(t)
ylist.append(y)
ilist.append(i)
vlist.append(dydt)

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
	tlist.append(t)
	ylist.append(y)
	ilist.append(i)
	vlist.append(dydt)

if switched:
	print(t)
else:
	print("inf")

#figure()
#plot(tlist,ylist, ls='-', c='r')
#xlabel('time')
#ylabel('y')
#show()


# Solve a differential equation using the Euler-Cromer method

from pylab import *

# make empty lists
#tlist = [] 
#ilist = []
#ylist = []
#vlist = []

delay = []
ymin=0.4
ymax=0.06
voltage=[0.65,0.66,0.67,0.68]

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

for voltage in vw:
#set initial value
    t=0
    y=ymax
    i=voltage*(gon*y+(1-y)*b1*exp(b2*sqrt(fabs(voltage))))
    dydt = aon*exp(-1*(y/ymax)**2)*sinh(voltage/sigvon)*exp(voltage*i/sigpon)
    
    switched=False
    switch_th=0.9*voltage*(gon*ymin+(1-ymin)*b1*exp(b2*sqrt(fabs(voltage))))
    while t<tf:
    	dydt = aon*exp(-1*pow((y/ymax),2))*sinh(voltage/sigvon)*exp(voltage*i/sigpon)
    	y = y + dydt*dt
    	if y>ymin:
    		y=ymin
    	i=voltage*(gon*y+(1-y)*b1*exp(b2*sqrt(fabs(voltage))))
    	t = t + dt
    	#tlist.append(t)
    	#ylist.append(y)
    	#ilist.append(i)
    	#vlist.append(dydt)
    	if i>switch_th:
    		switched=True
    		break
    
    if switched:
    	delay.append(t)
    else:
    	delay.append(-1)

print delay
    
#figure()
#plot(tlist,ylist, ls='-', c='r')
#xlabel('time')
#ylabel('y')
#show()


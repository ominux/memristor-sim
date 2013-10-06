#!/usr/bin/python

import sys

def parse(ifilename,iScaleFactor):
	log = open(ifilename,'r')
	ofilename = ifilename.split('.')[0]+".res"
	ofile = open(ofilename,'w')

	scaling = float(iScaleFactor)

	for line in log:
		if line.find('123.00000k') != -1:
			#print "result ",line.find('123.00000k')
			data = line.split()
			#print data
			print "Voltage drop on selected memristor (worst-case) ", data[1]
			print "voltage drop on half-selected memristor (best-case) ", data[2]
			print "Total current on selected row ", data[3]
			d1 = returnValue(data[1])
			d2 = returnValue(data[3])
			d2 = 1e6 * d2 / scaling
			ofile.write("voltage(V) %f \n" % d1)
			ofile.write("current(uA) %f \n" % d2)
			#print "test d1 %f" % d1
			#print "test d2 %f" % d2
	
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
		print "Unrecognized unit in ",s
		sys.exit(2)

if __name__ == "__main__":
	if len(sys.argv) != 3:
		print "Usage: extract.py <log file> <scaling factor of write current>\n"
	else:
		parse(sys.argv[1],sys.argv[2])

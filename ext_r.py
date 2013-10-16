#!/usr/bin/python

import sys

def parse(filename):
	
	log = open(filename,'r')
	
	for line in log:
		if line.find('123.00000k') != -1:
			#print "result ",line.find('123.00000k')
			data = line.split()
			#print "Read current of a selected LRS ", data[2] 
			#print "Read current of a selected HRS ", data[3]
			i_lrs_org = returnValue(data[1])
			i_hrs_org = returnValue(data[2])
			i_usel_lrs = returnValue(data[3])
			v_usel_lrs = returnValue(data[4])
			i_usel_lrs = -1e9 * i_usel_lrs
			v_usel_lrs = 1e3 * v_usel_lrs
			i_lrs = 1e6 * i_lrs_org
			i_hrs = 1e6 * i_hrs_org
			print "Miminum sensing current when select a LRS cell %fuA " % i_lrs
			print "Maximum sensing current when select a HRS cell %fuA " % i_hrs
			print "Maximum current of an unselected cell in the selected row %fnA " % i_usel_lrs
			print "Maximum voltage drop on an unselected memristor in the selected row %fmV " % v_usel_lrs
			if (i_lrs < i_hrs):
				print "Read failure!"
			elif (i_lrs - i_hrs < 0.1):
				print "Sensing margin < 0.1 uA which makes sensing is challenge!"

	log.close()
	
def returnValue(s):
	unit_s = s[-1]
	num_s = s[:-1]
	if (unit_s == 'k'):
		return 1e3 * float(num_s)
	elif (unit_s == 'm'):
		return 1e-3 * float(num_s)
	elif (unit_s == 'u'):
		return 1e-6 * float(num_s)
	elif (unit_s == 'n'):
		return 1e-9 * float(num_s)
	elif (unit_s == 'p'):
		return 1e-12 * float(num_s)
	elif (unit_s.isdigit()):
		return float(s)
	else:
		print "Bogus unit in ",s
		sys.exit(2)

if __name__ == "__main__":
	if len(sys.argv) != 2:
		print "Usage: ext_r.py <log file>\n"
	else:
		parse(sys.argv[1])

#!/usr/bin/python

import sys

def parse(filename):

    log = open(filename,'r')
    
    for line in log:
        if line.find('123.00000k') != -1:
            #print "result ",line.find('123.00000k')
            data = line.split()
            #print data
            print "Voltage drop on selected memristor (worst-case) ", data[1]
            print "voltage drop on half-selected memristor (best-case) ", data[2]
            print "Total current on selected row ", data[3]

    log.close()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "Usage: extract.py <log file>\n"
    else:
        parse(sys.argv[1])

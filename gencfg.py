#!/usr/bin/python

import sys
import getopt
from pylab import *

letters = "r:c:b:i:o:"
keywords = ["row=", "column=", "bit=", "inputfile=", "outputfile="]

row = 128
column = 128
bit = 8

t_w = "100"
dr_v = "1.8"
dr_i = "500.0"

ifilename = ""
ofilename = "memristor_mat.cfg"

try:
	opts, args = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetoptError:
	sys.exit(2)

for opt, arg in opts:
	if opt in ("-r", "--row"):
		row = int(arg)
	elif opt in ("-c", "--column"):
		column = int(arg)
	elif opt in ("-b", "--bit"):
		bit = int(arg)
	elif opt in ("-i", "--inputfile"):
		ifilename = arg
	elif opt in ("-o", "--outputfile"):
		ofilename = arg
	else:
		assert False, "unhandled options"

print "row is ", row
print "column is", column
print "bit is", bit
print "inputfile is "+ifilename
print "outputfile is "+ofilename

log  = open(ifilename,'r')

for line in log:
	if line.find('voltage') != -1:
		dr_v = line.split()[1]
	elif line.find('current') != -1:
		dr_i = line.split()[1]

samplefilename = "sample.cfg"
samplefile = open(samplefilename,'r')
samplefiledata = samplefile.read()
samplefile.close()

blk_size = bit / 2
print "block size (bytes) is ", blk_size
blk_size_line = "-block size (bytes) " + str(blk_size) + "\n"
data2write = blk_size_line + samplefiledata

ram_size = row * column / 2
print "size (bytes) is ", ram_size
ram_size_line = "-size (bytes) " + str(ram_size) + "\n"
data2write = ram_size_line + data2write

nspd = ram_size / (2 * row * blk_size)
print "Nspd is ", nspd
nspd_line = "-Nspd " + str(nspd) + "\n"
data2write = data2write + nspd_line

ndcm = column / bit
ndcm_line = "-Ndcm " + str(ndcm) + "\n"
data2write = data2write + ndcm_line

ndsam1 = 1
ndsam1_line = "-Ndsam1 " + str(ndsam1) + "\n"
data2write = data2write + ndsam1_line

ndsam2 = 1
ndsam2_line = "-Ndsam2 " + str(ndsam2) + "\n\n"
data2write = data2write + ndsam2_line

t_w_line = "-Memristor write latency (ns) " + t_w + "\n"
data2write = data2write + t_w_line

dr_v_line = "-Output voltage of write driver (V) " + dr_v + "\n"
data2write = data2write + dr_v_line

dr_i_line = "-Output current of write driver (uA) " + dr_i + "\n"
data2write = data2write + dr_i_line

t_rc = 5.5
t_rc_line = "-Xpoint RC latency (ns) " + str(t_rc) + "\n"
data2write = data2write + t_rc_line

targetfile = open(ofilename,'w')
targetfile.write(data2write)
targetfile.close()


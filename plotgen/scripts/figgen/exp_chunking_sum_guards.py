import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("CARM L1 guards") > 0):
            lp = x.split(":")
            ls.append(float(lp[1]))
        elif (x.count("Major (requiring I/O)") > 0):
            lp = x.split(":")
            major = float(lp[1])
        elif (x.count("Minor (reclaiming a frame)") > 0):
            lp = x.split(":")
            minor = float(lp[1])
            ls.append(float(major + minor))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
cm2d = cm2d("{}/{}".format(path, fname), 
            'proportion of local memory', 
             r'#guards/faults ($\times$10M)',
            'Copy', 16, 16, 18, 18, 'bold') 

list1 = [1, 2, 3, 4, 5]
fs = []
cm = []
cm_no_chunk = []


nam = "scripts/figgen/results/chunking_results_nov28/carm_guards/sum/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/chunking_results_nov28/carm_guards/sum_no_chunk/log."
for i in list1: 
    readFile(nam, i, cm_no_chunk)
nam = "scripts/figgen/results/chunking_results_nov28/fastswap/sum/log."
for i in list1: 
    readFile(nam, i, fs)

j=0


x=[]
for i in list1:
    x.append(i)

cm2d.plot(x, [(c)/10000000 for c in cm], '-', 'white', 'o', 10)
cm2d.plot(x, [f/10000000 for f in fs], '--', 'white', '*', 10)
cm2d.plot(x, [(c)/10000000 for c in cm_no_chunk], '-', 'white', 'o', 10)

#cm2d.set_y_lg_scale()
#cm2d.plot(x, cm, '-', 'white', 'o', 10)
#cm2d.plot(x, fs, '--', 'white', '*', 10)
cm2d.set_legend(['L2 guards w/ chunk', 'FastSwap faults', ' L2 guards w/o chunk'], 1, 16)

cm2d.save()

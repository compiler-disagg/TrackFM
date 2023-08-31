import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Remote") > 0):
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
xlabel  = "local mem [% of 12GB]" 
cm2d = cm2d("{}/{}".format(path, fname), 
            xlabel, 
             r'#relative fetches x(1000)',
            'Sum', 16, 16, 18, 18, 'bold') 

list1 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
fs = []
cm = []
cm_no_prefetch = []
sd = []


nam = "scripts/figgen/results/chunking_results_nov28/prefetch_rates/carm/triad/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/chunking_results_nov28/prefetch_rates/carm/triad_no_prefetch/log."
for i in list1: 
    readFile(nam, i, cm_no_prefetch)

j=0


x=[]
for i in list1:
    if (int(cm[j]) != 0):
        sd.append(float(cm_no_prefetch[j]/cm[j]))
    else:
        sd.append(cm_no_prefetch[j])
    x.append(i/max(list1))
    j = j + 1

#cm2d.plot(x, sd, '-', 'white', 'o', 10)
cm2d.plot(x, [c/1000 for c in sd], '-', 'white', 'o', 10)

#cm2d.set_y_lg_scale()
#cm2d.plot(x, cm, '-', 'white', 'o', 10)
#cm2d.plot(x, fs, '--', 'white', '*', 10)
#cm2d.set_legend(['prefetching enabled', 'baseline'], 1, 16)

cm2d.set_y_limit(-0.5, 8)
cm2d.save()

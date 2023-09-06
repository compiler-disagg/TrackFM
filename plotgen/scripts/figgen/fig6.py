import sys
import numpy as np
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt
from carmfig import Carm2DFig as cm2d
path    = sys.argv[1]
fname   = sys.argv[2]

def readFile (fname, i, ls, ls1, mem_filter):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Iterations") > 0):
            lp = x.split(":")
            ls.append(float(lp[1]))
            ls1.append(float(lp[7]))


path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
cm2d = cm2d("{}/{}".format(path, fname), 
            r'# elements per object', 
            'speedup vs. baseline transform', 
            'Loop cost model', 18, 18, 20, 20, 'bold') 

cm_x = []
cm_y = []
cm_no_chunk_x = []
cm_no_chunk_y = []
mem_filter=1

nam = "scripts/figgen/results/fig6/chunk/log."
readFile(nam, 32768, cm_x, cm_y, mem_filter)
nam = "scripts/figgen/results/fig6/no_chunk/log."
readFile(nam, 32768, cm_no_chunk_x, cm_no_chunk_y, mem_filter)
j = 0

sd = []
#lim=13

for i in cm_x:
    sd.append(float(cm_no_chunk_y[j]/cm_y[j]))
    j = j + 1

#print(sd)
cm2d.scatter([x for x in cm_no_chunk_x], sd)
#cm2d.plot(cm_x, cm_y, '--', 'white', '+', 10)


#cm2d.set_x_lg_scale()
#cm2d.set_y_lg_scale()
cm2d.set_y_limit(0, 2)
cm2d.set_x_limit(0, 1000)
plt.axhline(y = 1.0, color = 'black', linestyle = '--', zorder=1)
# KCH TODO: x val for this line needs to match the predicted loop count from model
PREDICTED_CROSSOVER=724
plt.axvline(x = PREDICTED_CROSSOVER, color= 'r', linestyle='-', lw=2, zorder=1)
plt.text(PREDICTED_CROSSOVER-420, 1.5, "predicted" + r"$\rightarrow$" + "\ncrossover\npoint", fontsize=18, color='r')
#cm2d.set_legend(['w/o chunk opt', 'w/ chunk opt'], 1, 20)

cm2d.save()

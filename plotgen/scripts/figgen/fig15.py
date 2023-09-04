import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Total") > 0):
            lp = x.split(":")
            ls.append(float(float(lp[1].split("us")[0].rstrip())))

path    = sys.argv[1]
fname   = sys.argv[2]
xlabel  = "local mem [% of 31GB]" 

plt.figure(figsize=(5.5,5))
cm2d = cm2d("{}/{}".format(path, fname), xlabel, 'slowdown vs. local-only', 'Dataframe', 16, 16, 18, 18, 'bold') 

list1 = [1, 3,  5,  7,  9, 11,  13,  15,  17, 19, 19, 21,  23,  25, 27, 29]
cm1 = []
cm2 = []
cm3 = []

cdataframe_local = 24183543
nam = "scripts/figgen/results/fig15/TrackFM/no_chunk/log."
for i in list1: 
    readFile(nam, i, cm1)
nam = "scripts/figgen/results/fig15/TrackFM/chunk/log."
for i in list1: 
    readFile(nam, i, cm2)
nam = "scripts/figgen/results/fig15/TrackFM/chunk_prof/log."
for i in list1: 
    readFile(nam, i, cm3)
j=0

for i in list1:
    cm1[j] = float(cm1[j]/cdataframe_local)
    cm2[j] = float(cm2[j]/cdataframe_local)
    cm3[j] = float(cm3[j]/cdataframe_local)
    j = j + 1


x=[]
for i in list1:
    x.append(i/max(list1))
cm2d.plot(x, cm1, '-', 'none', next(cm2d.marker), 10)
cm2d.plot(x, cm2, '--', 'none', next(cm2d.marker), 10)
cm2d.plot(x, cm3, '-', 'none', 'o', 8)

plt.ylim(ymin=0)
plt.axhline(y=1, color='black', linestyle='--', zorder=1)
cm2d.set_legend([ 'baseline', 'all loops', 'high-density loops only'], 1, 18)

cm2d.save()

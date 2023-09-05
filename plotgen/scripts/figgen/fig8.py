import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Time") > 0):
            lp = x.split(":")
            ls.append(float(float(lp[1].split("us")[0].rstrip())))

path    = sys.argv[1]
fname   = sys.argv[2]
xlabel  = "local mem [% of 1GB]" 

plt.figure(figsize=(5.5,5))
cm2d = cm2d("{}/{}".format(path, fname), xlabel, 'speedup', 'Kmeans', 16, 16, 18, 18, 'bold') 

list1 = [128, 256,  384,  512,  640, 768,  896,  1024]
cm1 = []
cm2 = []
cm3 = []

kmeans_local = 16192453
nam = "scripts/figgen/results/fig8/TrackFM/no_chunk/log."
for i in list1: 
    readFile(nam, i, cm1)
nam = "scripts/figgen/results/fig8/TrackFM/chunk/log."
for i in list1: 
    readFile(nam, i, cm2)
nam = "scripts/figgen/results/fig8/TrackFM/chunk_prof/log."
for i in list1: 
    readFile(nam, i, cm3)
j=0

for i in list1:
    cm2[j] = float(cm1[j]/cm2[j])
    cm3[j] = float(cm1[j]/cm3[j])
    j = j + 1


x=[]
for i in list1:
    x.append(i/max(list1))
#cm2d.plot(x, cm1, '-', 'none', next(cm2d.marker), 10)
cm2d.plot(x, cm2, '--', 'none', next(cm2d.marker), 10)
cm2d.plot(x, cm3, '-.', 'none', 'o', 10)

plt.axhline(y=1, color='black', linestyle='--', zorder=1)

cm2d.set_legend([ 'all loops', 'high-density loops only'], 1, 18)

cm2d.save()

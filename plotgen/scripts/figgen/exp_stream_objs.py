import matplotlib.pyplot as plt
import sys
sys.path.append("scripts/figgen/lib")

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname + str(i)
    f = open(lin, "r")
    flag = 0
    for x in f:
        lp = x.split(":")
        if (len(lp) > 1 and x.count("Time") > 0):
            ls.append((float(lp[1].rstrip())))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
cm2d = cm2d("{}/{}".format(path, fname), 
            'object size', 
            'execution time (s)', 
            'STL Unordered Map', 19, 19, 22, 22, 'bold') 

list1 = [4096, 2048, 1024, 512, 256]
cm = []
ops = 50000000
nam = "scripts/figgen/results/umap_results_nov27/carm/log."

for i in list1: 
    readFile(nam, i, cm)

micro = 1000000
j=0
for i in list1:
    cm[j] = float((cm[j]/micro))
    j = j + 1

x =[]
j = 0
for i in list1:
    x.append(str(i))
    j = j + 1

cm2d.barplot(x, cm, 0.5)

xlabs = ['4KB', '2KB', '1KB', '512B', '256B']
plt.xticks(x, xlabs)
cm2d.set_y_limit(0, 850)

cm2d.save()

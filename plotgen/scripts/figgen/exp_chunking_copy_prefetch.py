import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Copy") > 0):
            lp = x.split(":")
            ls.append(float(lp[3]))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
xlabel  = "local mem [% of 12GB]" 
cm2d = cm2d("{}/{}".format(path, fname), 
             xlabel, 
            'Speedup', 
            'Dataframe', 20, 20, 22, 22, 'bold') 

list1 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
lfnam = "results/NYC_results_oct22/linux_local/log"
cm = []
cm_no_chunk = []


nam = "scripts/figgen/results/chunking_results_nov28/carm_chunking_prefetch_enabled/carm_copy/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/chunking_results_nov28/carm_chunk_copy/log."
for i in list1: 
    readFile(nam, i, cm_no_chunk)
j=0

sd = []

for i in list1:
    sd.append(float(cm_no_chunk[j]/cm[j]))
    j = j + 1

x=[]
for i in list1:
    x.append(i/max(list1))
cm2d.plot(x, sd, '--', 'white', 'o', 10)

cm2d.set_y_limit(2.5, 4.8)
cm2d.save()

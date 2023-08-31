import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Sum") > 0):
            lp = x.split(":")
            ls.append(float(lp[3]))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
xlabel  = "local mem [% of 12GB]" 
cm2d = cm2d("{}/{}".format(path, fname), 
             xlabel, 
            'speedup', 
            'Chunking', 20, 20, 22, 22, 'bold') 

list1 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]
ln = []
am = []
fs = []
cm = []
sp = []

nam = "scripts/figgen/results/chunking_results_nov28/local/log."
readFile(nam, 1, ln)
cpp_local_sum = ln[0]

for i in list1: 
    ln.append(float(cpp_local_sum))
nam = "scripts/figgen/results/chunking_results_nov28/carm_chunking_prefetch_enabled/carm_sum/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/chunking_results_nov28/fastswap/sum/log."
for i in list1: 
    readFile(nam, i, fs)
nam = "scripts/figgen/results/chunking_results_nov28/aifm/sum/log."
for i in list1: 
    readFile(nam, i, am)
j=0


for i in list1:
    sp.append(float(fs[j]/cm[j]))
#    am[j] = float(am[j]/ln[j])
    j = j + 1



x=[]
for i in list1:
    x.append(i/max(list1))
cm2d.plot(x, sp, '--', 'white', 'o', 10)
#cm2d.plot(x, fs, '-.', 'white', '*', 10)
#cm2d.plot(x, am, '-', 'white', 's', 10)


#cm2d.set_legend(['TrackFM', 'Fastswap', 'AIFM'], 1, 14)
#cm2d.set_y_limit(0, 16)

cm2d.save()

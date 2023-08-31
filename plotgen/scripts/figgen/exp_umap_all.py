import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt
import statistics

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Time") > 0 and x.count("Wall") == 0):
            lp = x.split(":")
            ls.append(float(float(lp[1])))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
xlabel  = "local mem [% of 2GB]"
cm2d = cm2d("{}/{}".format(path, fname), 
             xlabel, 
            'execution time (s)', 
            'UMAP', 18, 18, 20, 20, 'bold') 

list1 = [128, 256, 384, 512, 768, 1024, 1536, 2048]
ln = []
am = []
fs = []
cm = []   #64
cm4k = []
cm1k = []
cm256 = []
cm512 = []

c_umap_local   = 369222 
for i in list1: 
    ln.append(float(c_umap_local))
nam = "scripts/figgen/results/umap_results_nov27/carm_logs/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/umap_results_nov27/fastswap_logs/log."
for i in list1: 
    readFile(nam, i, fs)
nam = "scripts/figgen/results/umap_results_nov27/carm_logs_4K/log."
for i in list1: 
    readFile(nam, i, cm4k)
nam = "scripts/figgen/results/umap_results_nov27/carm_logs_1K/log."
for i in list1: 
    readFile(nam, i, cm1k)
nam = "scripts/figgen/results/umap_results_nov27/carm_logs_256/log."
for i in list1: 
    readFile(nam, i, cm256)
nam = "scripts/figgen/results/umap_results_nov27/carm_logs_512/log."
for i in list1: 
    readFile(nam, i, cm512)
j=0
ops=50000000
micro=1000000
sd = []
for i in list1:
    sd.append(float(fs[j]/cm[j]))
    cm[j] = float((cm[j]/micro))
    #cm4k[j] = float((cm4k[j]/micro))
    fs[j] = float((fs[j]/micro))
    j = j + 1

print(sd)
print(min(sd))
print(max(sd))
x=[]
for i in list1:
    x.append(i/2048)

cm2d.plot(x, cm, '-', 'white', 'o', 10)
cm2d.plot(x, fs, '--', 'white', '*', 10)
#cm2d.plot(x, cm4k, '-.', 'white', 'o', 10)

cm2d.set_legend(['TrackFM 64B', 'Fastswap'], 1, 18)

cm2d.save()

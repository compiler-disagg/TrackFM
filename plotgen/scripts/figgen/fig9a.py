import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

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
            'throughput (MOps/s)', 
            'Dataframe', 20, 20, 22, 22, 'bold') 

list1 = [128, 256, 384, 512, 768, 1024, 1536, 2048]
ln = []
am = []
fs = []
cm = []   #64
cm4k = []
cm1k = []
cm2k = []
cm256 = []
cm512 = []
cm128 = []
cm64 = []
c_umap_local   = 369222 
for i in list1: 
    ln.append(float(c_umap_local))
nam = "scripts/figgen/results/fig9/carm_logs_4K/log."
for i in list1: 
    readFile(nam, i, cm4k)
nam = "scripts/figgen/results/fig9/carm_logs_1K/log."
for i in list1: 
    readFile(nam, i, cm1k)
nam = "scripts/figgen/results/fig9/carm_logs_2K/log."
for i in list1: 
    readFile(nam, i, cm2k)
nam = "scripts/figgen/results/fig9/carm_logs_256/log."
for i in list1: 
    readFile(nam, i, cm256)
nam = "scripts/figgen/results/fig9/carm_logs_512/log."
for i in list1: 
    readFile(nam, i, cm512)
#nam = "scripts/figgen/results/fig9/carm_logs_128/log."
#for i in list1: 
#    readFile(nam, i, cm128)
nam = "scripts/figgen/results/fig9/carm_logs/log."
for i in list1: 
    readFile(nam, i, cm64)
j=0
ops=50000000
micro=1000000
m = 1000000
for i in list1:
    cm256[j] = float(((ops/cm256[j]) * micro) /m)
    cm512[j] = float(((ops/cm512[j])* micro) /m)
    cm1k[j] = float(((ops/cm1k[j]) * micro) /m)
    cm4k[j] = float(((ops/cm4k[j]) * micro)/m)
    cm2k[j] = float(((ops/cm2k[j]) * micro)/m)
    j = j + 1

print(cm512)
#print(cm256)
print(cm64)

x=[]
for i in list1:
    x.append(i/2048)


cm2d.plot(x, cm4k, '-', 'white', 'o', 10)
cm2d.plot(x, cm2k, '--', 'white', '*', 10)
cm2d.plot(x, cm1k, '-.', 'white', '<', 10)
cm2d.plot(x, cm512, '-', 'white', 's', 10)
cm2d.plot(x, cm256, '-', 'white', '|', 10)
#cm2d.plot(x, cm128, '-', 'white', '>', 10)
#cm2d.plot(x, cm64, '-', 'white', '|', 10)
#cm2d.plot(x, fs, '-.', 'white', '>', 10)

cm2d.set_legend(['4KB', '2KB', '1KB', '512B', '256B'], 1, 17)
#cm2d.set_y_limit(0, 8)

cm2d.save()

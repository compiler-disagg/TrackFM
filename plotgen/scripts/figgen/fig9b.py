import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, ls):
    lin = fname 
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

list1 = [4096, 2048, 1024, 512, 256]
cm = []   #64
nam = "scripts/figgen/results/fig9/TrackFM/4096/log.512"
readFile(nam, cm)
nam = "scripts/figgen/results/fig9/TrackFM/2048/log.512"
readFile(nam, cm)
nam = "scripts/figgen/results/fig9/TrackFM/1024/log.512"
readFile(nam,  cm)
nam = "scripts/figgen/results/fig9/TrackFM/512/log.512"
readFile(nam, cm)
nam = "scripts/figgen/results/fig9/TrackFM/256/log.512"
readFile(nam,  cm)
#nam = "scripts/figgen/results/fig9/carm_logs_128/log."
#for i in list1: 
#    readFile(nam, i, cm128)
#nam = "scripts/figgen/results/fig9/carm_logs/log."
#for i in list1: 
#    readFile(nam, i, cm64)
j=0
ops=50000000
micro=1000000
m = 1000000

print(cm)

ops = 50000000

micro = 1000000
m = 1000000
j=0
for i in list1:
    cm[j] = float(((ops/cm[j])* micro) /m)
    j = j + 1

x =[]
j = 0
for i in list1:
    x.append(str(i))
    j = j + 1

cm2d.barplot(x, cm, 0.5)

xlabs = ['4KB', '2KB', '1KB', '512B', '256B']
plt.xticks(x, xlabs)
cm2d.save()

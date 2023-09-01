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
            lp1 = x.split(" ")
            op = []
            for i in lp1:
                if i != '':
                    op.append(i.rstrip())
            ls.append(float(op[1]))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
xlabel  = "local mem [% of 9GB]" 
cm2d = cm2d("{}/{}".format(path, fname), 
            xlabel, 
            'memory bandwith (MB/s)', 
            'Dataframe', 20, 20, 22, 22, 'bold') 

list1 = [1, 3, 5,  7,  9]
ln = []
am = []
fs = []
cm = []   #64
cm4k = []
cm2k = []
cm1k = []
cm512 = []
cm256 = []
nam = "scripts/figgen/results/fig10/log.4096/log."
for i in list1: 
    readFile(nam, i, cm4k)
nam = "scripts/figgen/results/fig10/log.2048/log."
for i in list1: 
    readFile(nam, i, cm2k)
nam = "scripts/figgen/results/fig10/log.1024/log."
for i in list1: 
    readFile(nam, i, cm1k)
nam = "scripts/figgen/results/fig10/log.512/log."
for i in list1: 
    readFile(nam, i, cm512)
nam = "scripts/figgen/results/fig10/log.256/log."
for i in list1: 
    readFile(nam, i, cm256)
j=0
for i in list1:
    cm256[j] = float((cm256[j]))
    cm512[j] = float((cm512[j]))
    cm1k[j] = float((cm1k[j]))
    cm2k[j] = float((cm2k[j]))
    cm4k[j] = float((cm4k[j]))
    j = j + 1


x=[]
for i in list1:
    x.append(i/9)

#cm2d.plot(x, cm16k, '-', 'white', '+', 10)
#cm2d.plot(x, cm8k, '--', 'white', '|', 10)
cm2d.plot(x, cm4k, '-', 'white', 'o', 10)
cm2d.plot(x, cm2k, '--', 'white', '*', 10)
cm2d.plot(x, cm1k, '-.', 'white', '<', 10)
cm2d.plot(x, cm512, '-', 'white', 's', 10)
cm2d.plot(x, cm256, '-', 'white', '|', 10)

cm2d.set_legend(['4KB', '2KB', '1KB', '512B', '256B'], 2, 18)
#cm2d.set_y_limit(0, 920)

cm2d.save()

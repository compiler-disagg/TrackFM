import matplotlib.pyplot as plt
import sys
sys.path.append("scripts/figgen/lib")

from carmfig import Carm2DFig as cm2d

def readFile (fname, ls):
    lin = fname
    f = open(lin, "r")
    flag = 0
    for x in f:
        lp = x.split(":")
        if (len(lp) > 1):
            ls.append((float(lp[2].rstrip())))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
cm2d = cm2d("{}/{}".format(path, fname), 
            'zipf skew param.', 
            'throughput (KOps/s)', 
            'Memcached', 16, 16, 18, 18, 'bold') 

list1 = [1.01, 1.02, 1.04, 1.08, 1.1, 1.12, 1.14, 1.18, 1.20, 1.22, 1.24, 1.28, 1.3]
ln = []
cm = []
fs = []
ops = 100000000
#for i in list1: 
#    linl.append(float(24183543))
nam = "scripts/figgen/results/fig16a/linux_local/log"
readFile(nam, ln)
nam = "scripts/figgen/results/fig16a/fastswap/log"
readFile(nam, fs)
nam = "scripts/figgen/results/fig16a/TrackFM/log"
readFile(nam, cm)
j=0
fsd = []
for i in list1:
    fsd.append(float(fs[j]/cm[j]))
    cm[j] = float(ops/cm[j]/1000)
    fs[j] = float(ops/fs[j]/1000)
    ln[j] = float(ops/ln[j]/1000)
    j = j + 1


x=[]
for i in list1:
    x.append(i)

print(" speedup over fastswap " + str(fsd))

cm2d.plot(x, cm, '--', 'white', 'o', 9)
cm2d.plot(x, fs, '-.', 'white', '*', 9)
cm2d.plot(x, ln, '-', 'white', 's', 9)

cm2d.set_legend(['TrackFM', 'Fastswap', 'All local'], 1, 16)

cm2d.save()

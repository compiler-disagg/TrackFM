import matplotlib.pyplot as plt
import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d


path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
cm2d = cm2d("{}/{}".format(path, fname), 'far memory bandwith (GB/s)', 'percentage of memory', 'Stream', 20, 20, 22, 22, 'bold') 


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

list1 = [1, 2, 3, 4, 5, 6, 7, 8, 9]
cm = []
fs = []

nam = "scripts/figgen/results/stream_results_oct22/stream_with_loops/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/stream_results_oct22/fastswap_stream_results/log."
for i in list1: 
    readFile(nam, i, fs)

j=0


x=[]

for i in list1:
    x.append(float(i/9))
j=0

for i in list1:
    cm[j] = float(cm[j]/1024)
    fs[j] = float(fs[j]/1024)
    j = j + 1


cm2d.plot(x, cm, '-', 'none', next(cm2d.marker), 10)
cm2d.plot(x, fs, '--', 'none', next(cm2d.marker), 10)

cm2d.set_legend([ 'TrackFM', 'FastSwap'], 1, 20)

plt.tight_layout()
cm2d.save()

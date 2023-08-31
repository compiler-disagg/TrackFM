import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, mj, mn):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("CARM L1 guards") > 0):
            lp = x.split(":")
            mj.append(float(lp[1]))
        elif (x.count("CARM L2 guards") > 0):
            lp = x.split(":")
            mn.append(float(lp[1]))
        elif (x.count("Major (requiring I/O)") > 0):
            lp = x.split(":")
            major = float(lp[1])
            mj.append(float(major))
        elif (x.count("Minor (reclaiming a frame)") > 0):
            lp = x.split(":")
            minor = float(lp[1])
            mn.append(float(minor))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
cm2d = cm2d("{}/{}".format(path, fname), 
            'proportion of local memory', 
            r'#guards/faults ($\times$10M)', 
            'Dataframe', 20, 20, 22, 22, 'bold') 

list1 = [1, 2, 3, 4, 5, 6, 7, 8, 9]
fs_mj = []
fs_mn = []
cm_l1 = []
cm_l2 = []

nam = "scripts/figgen/results/stream_results_oct22/stream_guards/carm/log."
for i in list1: 
    readFile(nam, i, cm_l1, cm_l2)
nam = "scripts/figgen/results/stream_results_oct22/fastswap_stream_results/log."
for i in list1: 
    readFile(nam, i, fs_mj, fs_mn)
j=0

x=[]
for i in list1:
    x.append(i/max(list1))
cm2d.plot(x, [c/10000000 for c in cm_l1], '-', 'white', 'o', 10)
cm2d.plot(x, [c/1000000000 for c in cm_l2], '--', 'white', '*', 10)
cm2d.plot(x, [c/10000000 for c in fs_mj], '-', 'white', 'o', 10)
cm2d.plot(x, [c/10000000 for c in fs_mn], '--', 'white', '*', 10)

cm2d.set_legend(['TrackFM Lvl1 guard', 'TrackFM Lvl2 guard', 'Fastswap major page faults','Fastswap minor page faults'], 1, 20)

cm2d.save()

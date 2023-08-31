import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Total") > 0):
            lp = x.split(":")
            ls.append(float(float(lp[1].split("us")[0].rstrip())))

path    = sys.argv[1]
fname   = sys.argv[2]
cm2d = cm2d("{}/{}".format(path, fname), 'slowdown relative to local memory', 'percentage of local memory allowed for app', 'Dataframe', 16, 16, 12, 12, 'bold') 

list1 = [1, 3,  5,  7,  9, 11,  13,  15,  17, 19, 19, 21,  23,  25, 27, 29]
cm1 = []
cm2 = []
cm3 = []

cdataframe_local = 24183543
cppdataframe_local = 38791481
nam = "scripts/figgen/results/NYC_results_oct22/carm_without_loops/log."
for i in list1: 
    readFile(nam, i, cm1)
nam = "scripts/figgen/results/NYC_results_oct22/carm_without_loops/log."
for i in list1: 
    readFile(nam, i, cm2)
nam = "scripts/figgen/results/NYC_results_oct22/carm_with_profile/log."
for i in list1: 
    readFile(nam, i, cm3)
j=0

for i in list1:
    cm1[j] = float(cm1[j]/cdataframe_local)
    cm2[j] = float(cm2[j]/cdataframe_local)
    cm3[j] = float(cm3[j]/cdataframe_local)
    j = j + 1


x=[]
for i in list1:
    x.append(i/max(list1))
cm2d.plot(x, cm1, '-', 'none', next(cm2d.marker), 8)
cm2d.plot(x, cm2, '-', 'none', next(cm2d.marker), 8)
cm2d.plot(x, cm3, '-', 'none', next(cm2d.marker), 8)

cm2d.set_legend([ 'TrackFM', 'TrackFM OPT1', 'TrackFM OPT2'], 1, 12)

cm2d.save()

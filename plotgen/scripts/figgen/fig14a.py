import sys
import statistics
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
plt.figure(figsize=(5,5))

xlabel  = "local mem [% of 31GB]" 
cm2d = cm2d("{}/{}".format(path, fname), 
            xlabel, 
            'slowdown vs. local-only', 
            'Dataframe', 20, 20, 22, 22, 'bold') 

list1 = [1, 3,  5,  7,  9, 11,  13,  15,  17, 19, 19, 21,  23,  25, 27, 29]
ln = []
am = []
fs = []
cm = []

cdataframe_local   = 38578007
cppdataframe_local = 62827537 
for i in list1: 
    ln.append(float(cdataframe_local))
nam = "scripts/figgen/results/fig14a/TrackFM/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/fig14a/fastswap/log."
for i in list1: 
    readFile(nam, i, fs)
nam = "scripts/figgen/results/fig14a/AIFM/log."
for i in list1: 
    readFile(nam, i, am)
j=0

tsd = []
fsd = []

for i in list1:
    cm[j] = float(cm[j]/ln[j])
    fs[j] = float(fs[j]/ln[j])
    am[j] = float(am[j]/cppdataframe_local)
    tsd.append(float(cm[j]/am[j]))
    fsd.append(float(fs[j]/cm[j]))
    j = j + 1

print(" speedup over fastswap " + str(fsd))
print(" slowdown over AIFM " + str(statistics.median(tsd)))

x=[]
for i in list1:
    x.append(i/max(list1))
cm2d.plot(x, cm, '--', 'white', 'o', 10)
cm2d.plot(x, fs, '-.', 'white', '*', 10)
cm2d.plot(x, am, '-', 'white', 's', 10)
plt.axhline(y=1, color='black', linestyle='--', zorder=1)

lina = [number / number for number in ln]

cm2d.set_legend(['TrackFM', 'Fastswap', 'AIFM'], 1, 20)

cm2d.save()

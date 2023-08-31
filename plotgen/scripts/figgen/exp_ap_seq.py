import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

#Array sequential Time : 4122144
#rray random Time : 204439296
#Array strided Time : 41421057
#Array strided stride across objectTime : 74780709




def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("sequential Time") > 0):
            lp = x.split(":")
            ls.append(float(lp[1]))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
xlabel  = "local mem [% of 16GB]" 
cm2d = cm2d("{}/{}".format(path, fname), 
            xlabel, 
            'compute delaye per remote object access', 
            'AP', 20, 20, 22, 22, 'bold') 

list1 = [1, 2, 3, 4,5, 6,7,8,9,10,11, 12, 13, 14, 15, 16]
list1 = [0, 300, 600, 900, 1200, 1600, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000]
ln = []
cm = []
fs = []
am = []

nam = "scripts/figgen/results/access_pattern_results/local/log."
for i in list1: 
    readFile(nam, i, ln)
nam = "scripts/figgen/results/access_pattern_results/carm/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/access_pattern_results/fastswap/log."
for i in list1: 
    readFile(nam, i, fs)
nam = "scripts/figgen/results/access_pattern_results/aifm/log."
for i in list1: 
    readFile(nam, i, am)
j=0


for i in list1:
    cm[j] = round(float(cm[j]/ln[j]))
    am[j] = round(float(am[j]/ln[j]))
    fs[j] = round(float(fs[j]/ln[j]))
    j = j + 1

x=[]
for i in list1:
    x.append(i)
cm2d.plot(x, cm, '--', 'white', 'o', 10)
cm2d.plot(x, am, '-.', 'white', '+', 10)
cm2d.plot(x, fs, '-.', 'white', '+', 10)

print(cm)
print(am)
print(fs)


#cm2d.set_y_lg_scale()

cm2d.set_legend(['TrackFM', 'AIFM'], 1, 20)
cm2d.set_y_limit(0.9, 1.16)

cm2d.save()

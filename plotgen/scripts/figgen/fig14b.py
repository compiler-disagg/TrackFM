import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Remote") > 0):
            lp = x.split(":")
            ls.append(float(lp[1]))
        elif (x.count("Major (requiring I/O)") > 0):
            lp = x.split(":")
            major = float(lp[1])
        elif (x.count("Minor (reclaiming a frame)") > 0):
            lp = x.split(":")
            minor = float(lp[1])
            ls.append(float(major + minor))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
xlabel = "local mem [% of 31GB]"
cm2d = cm2d("{}/{}".format(path, fname), 
             xlabel, 
            r'#guards/faults ($\times$10M)', 
            'Dataframe', 20, 20, 22, 22, 'bold') 

list1 = [1, 3,  5,  7,  9, 11,  13,  15,  17, 19, 19, 21,  23,  25, 27, 29]
fs = []
cm = []

nam = "scripts/figgen/results/fig14b/TrackFM/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/fig14a/fastswap/log."
for i in list1: 
    readFile(nam, i, fs)
j=0

x=[]
for i in list1:
    x.append(i/max(list1))
cm2d.plot(x, [c/10000000 for c in cm], '-', 'white', 'o', 10)
cm2d.plot(x, [f/10000000 for f in fs], '--', 'white', '*', 10)

#cm2d.set_legend(['TrackFM runtime checks', 'Fastswap page faults'], 1, 20)

cm2d.save()

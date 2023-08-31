import matplotlib.pyplot as plt
import sys
import matplotlib as mpl
import numpy as np
sys.path.append("scripts/figgen/lib")

from carmfig import Carm2DFig as cm2d

mpl.rcParams['hatch.linewidth'] = 0.5

path    = sys.argv[1]
fname   = sys.argv[2]

cm2d = cm2d("{}/{}".format(path, fname), 
            'Application', 
            'Time (seconds)', 
            'NAS benchmarks', 19, 19, 22, 22, 'bold') 

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Time in seconds") > 0):
            lp = x.split("=")
            ls.append(float(lp[1]))

# data to plot
n_groups = 5
apps = []
stds = []
std_means_linux = []
std_means_mc = []
std_means_mos = []

list1 = ["cg.log.2", "ft.log.2", "is.log.9", "mg.log.7", "sp.log.3"]

ln = []
fs = []
cm = []

nam = "scripts/figgen/results/nas/local/"
for i in list1: 
    readFile(nam, i, ln)

nam = "scripts/figgen/results/nas/nas_fs/"
for i in list1: 
    readFile(nam, i, fs)

nam = "scripts/figgen/results/nas/nas_simp_carm/loop/"
for i in list1: 
    readFile(nam, i, cm)
# create plot
fig, ax = plt.subplots()
index = np.arange(n_groups)
bar_width = 0.2
opacity = 0.8

rects1 = ax.bar(index, ln, bar_width, 
align='center',
alpha=opacity,
color='#012172',
hatch='...',
label='Only Linux',
edgecolor='black',
)

rects2 = ax.bar(index + bar_width, fs, bar_width,
align='center',
alpha=opacity,
color='#0486db',
hatch='////',
label='FastSwap',
edgecolor='black')

rects3 = ax.bar(index + bar_width + bar_width, cm, bar_width,
align='center',
alpha=opacity,
color='#05acd3',
hatch='---',
label='TrackFM with redundant code elimination optimization',
edgecolor='black')

#plt.xlabel('applications')
plt.ylabel('runtime (s)', fontsize=16)
plt.yscale('log')
plt.legend()
plt.xticks(index + bar_width/2, ('CG', 'FT', 'IS', 'MG', 'SP'), rotation=45, rotation_mode='anchor', ha='right', va='top')

fig.tight_layout()
cm2d.save()

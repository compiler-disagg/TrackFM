import matplotlib.pyplot as plt
import sys
import matplotlib as mpl
import numpy as np
from scipy.stats import gmean
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
n_groups = 6
apps = []
stds = []
std_means_linux = []
std_means_mc = []
std_means_mos = []

list1 = ["cg.log.2", "ft.log.2", "is.log.9", "mg.log.7", "sp.log.3"]

ln = []
fs = []
cm = []
cmo1 = []
n_fs = []
n_cm = []
n_cmo1 = []



nam = "scripts/figgen/results/nas/local/"
for i in list1: 
    readFile(nam, i, ln)

nam = "scripts/figgen/results/nas/nas_fs/"
for i in list1: 
    readFile(nam, i, fs)

nam = "scripts/figgen/results/nas/nas_norm_carm/loop_profile/"
for i in list1: 
    readFile(nam, i, cm)

nam = "scripts/figgen/results/nas/nas_O1_carm/loop/"
for i in list1: 
    readFile(nam, i, cmo1)

for i in range(0, 5):
    n_cm.append(float(cm[i]/ln[i]))
    n_fs.append(float(fs[i]/ln[i]))
    n_cmo1.append(float(cmo1[i]/ln[i]))


#geo mean hack
n_cm.append(float(gmean(n_cm)))
n_fs.append(float(gmean(n_fs)))
n_cmo1.append(float(gmean(n_cmo1)))


# create plot
fig, ax = plt.subplots()
index = np.arange(n_groups)
bar_width = 0.3
opacity = 0.8

#rects1 = ax.bar(index, ln, bar_width, 
#align='center',
#alpha=opacity,
#color='#012172',
#hatch='...',
#label='Only Linux',
#edgecolor='black',
#)

rects1 = ax.bar(index, n_fs, bar_width,
align='center',
alpha=opacity,
color='#0486db',
hatch='////',
label='Fastswap',
edgecolor='black')

rects2 = ax.bar(index + bar_width, n_cm, bar_width,
align='center',
alpha=opacity,
color='#05acd3',
hatch='---',
label='TrackFM',
edgecolor='black')

#rects3 = ax.bar(index + float(bar_width * 2), n_cmo1, bar_width,
#align='center',
#alpha=opacity,
#color='#012172',
#hatch='....',
#label='O1 + TrackFM',
#edgecolor='black')


#rects2 = ax.bar(index + float(bar_width * 3), gm_cm, bar_width,
#align='center',
#alpha=opacity,
#color='#05acd3',
#hatch='---',
#label='TrackFM geomean',
#edgecolor='black')
#plt.xlabel('applications')
plt.ylabel('slowdown vs. local-only', fontsize=18)
#plt.yscale('log')
plt.legend(fontsize=16)
plt.xticks(index + bar_width/2, ('CG', 'FT', 'IS', 'MG', 'SP', 'GeoM.'))
#plt.xticks(index + bar_width/2, ('CG', 'FT', 'IS', 'MG', 'SP', 'GM'), rotation=0, rotation_mode='anchor', ha='right', va='top')

fig.tight_layout()
cm2d.save()

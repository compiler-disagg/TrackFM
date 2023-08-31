import sys
import numpy as np
import matplotlib.pyplot as plt
from collections import defaultdict

def readFile(fname, i, ls):
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

path  = sys.argv[1]
fname = sys.argv[2]

list1 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
x_vals = []
for i in list1:
    x_vals.append(i/max(list1))

logs    = [defaultdict(dict) for i in range(3)]
results = [defaultdict(dict) for i in range(3)]
names   = ['Sum', 'Copy', 'Triad']

# sum
logs[0]['with']    = "scripts/figgen/results/chunking_results_nov28/prefetch_rates/carm/sum/log."
logs[0]['without'] = "scripts/figgen/results/chunking_results_nov28/prefetch_rates/carm/sum_no_prefetch/log."

# copy
logs[1]['with']    = "scripts/figgen/results/chunking_results_nov28/prefetch_rates/carm/copy/log."
logs[1]['without'] = "scripts/figgen/results/chunking_results_nov28/prefetch_rates/carm/copy_no_prefetch/log."

# triad
logs[2]['with']    = "scripts/figgen/results/chunking_results_nov28/prefetch_rates/carm/triad/log."
logs[2]['without'] = "scripts/figgen/results/chunking_results_nov28/prefetch_rates/carm/triad_no_prefetch/log."

speedups = [[], [], []]
for j, m in enumerate(logs):
    results[j]['with']    = []
    results[j]['without'] = []
    for i in list1:
        readFile(m['with'],    i, results[j]['with'])
        readFile(m['without'], i, results[j]['without'])

for i, elm in enumerate(list1):
    for j in range(3):
        if int(results[j]['with'][i]) != 0:
            speedups[j].append(float(results[j]['without'][i]/results[j]['with'][i]))
        else:
            speedups[j].append(50000) # deal with singularity
            #speedups[j].append(float(results[j]['without'][i]))

fig, axs = plt.subplots(nrows=2, ncols=1, sharex=True, figsize=(7,4))

for i, ax in enumerate(axs):
    pc = ax.plot(x_vals, 
                 [s/1000 for s in speedups[i]],
                 marker='o', 
                 zorder=3, 
                 markeredgewidth=3, 
                 ms=10, 
                 markerfacecolor='white', 
                 lw=3)
    ax.set_title(names[i], fontsize=18)
    ax.grid(axis='both', zorder=0, alpha=0.5)
    ax.tick_params(axis='both', which='major', labelsize=18)
    ax.tick_params(axis='both', which='minor', labelsize=18)
    ax.set_ylim(1.0, 8)
    if i == 1:
        ax.set_xlabel('local mem [% of 12GB]', fontsize=20)
    if i == 1:
        ax.set_ylabel(r"fetch decrease ($\times$1000)", fontsize=20)
        ax.yaxis.set_label_coords(-0.13, 1.2)

plt.tight_layout()
plt.savefig("{}/{}".format(path,fname))

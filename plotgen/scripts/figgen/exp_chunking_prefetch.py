import sys
import matplotlib.pyplot as plt
from collections import defaultdict

def readFile(fname, i, ls, col_hdr):
    lin = fname + str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count(col_hdr) > 0):
            lp = x.split(":")
            ls.append(float(lp[3]))


path  = sys.argv[1]
fname = sys.argv[2]

list1 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
x_vals = []
for i in list1:
    x_vals.append(i/max(list1))

logs    = [defaultdict(dict) for i in range(3)]
results = [defaultdict(dict) for i in range(3)]
names   = ['Sum', 'Copy', 'Triad']

# sum
logs[0]['with']    = "scripts/figgen/results/chunking_results_nov28/carm_chunking_prefetch_enabled/carm_sum/log."
logs[0]['without'] = "scripts/figgen/results/chunking_results_nov28/carm_chunk_sum/log."

# copy
logs[1]['with']    = "scripts/figgen/results/chunking_results_nov28/carm_chunking_prefetch_enabled/carm_copy/log."
logs[1]['without'] = "scripts/figgen/results/chunking_results_nov28/carm_chunk_copy/log."

# triad
logs[2]['with']    = "scripts/figgen/results/chunking_results_nov28/carm_chunking_prefetch_enabled/carm_triad/log."
logs[2]['without'] = "scripts/figgen/results/chunking_results_nov28/carm_chunk_triad/log."

speedups = [[], [], []]
for j, m in enumerate(logs):
    results[j]['with']    = []
    results[j]['without'] = []
    for i in list1:
        readFile(m['with'],    i, results[j]['with'],    names[j])
        readFile(m['without'], i, results[j]['without'], names[j])

for i, elm in enumerate(list1):
    for j in range(3):
        speedups[j].append(float(results[j]['without'][i]/results[j]['with'][i]))

fig, axs = plt.subplots(nrows=2, ncols=1, sharex=True, figsize=(7,4))

for i, ax in enumerate(axs):
    pc = ax.plot(x_vals, 
                 speedups[i],
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
    ax.set_ylim(3.0, 5)
    if i == 1:
        ax.set_xlabel('local mem [% of 12GB]', fontsize=20)
    if i == 1:
        ax.set_ylabel('speedup', fontsize=20)
        ax.yaxis.set_label_coords(-0.05, 1.2)

plt.tight_layout()
plt.savefig("{}/{}".format(path,fname))

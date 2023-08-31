import sys
import numpy as np
import matplotlib.pyplot as plt
import math

# plot params
LABELSIZE=22
LEGSIZE=20
TICKSIZE=18

path = sys.argv[1]
fname = sys.argv[2]

# model params
o  = 4096  # AIFM obj size
e  = 2   # size of type of array element
cf = 30    # cost of fast path guard (in cycles)
cs = 800   # cost of slow path guard (in cycles)
cr = 10

cl = 15000 # cost of locality invariant guard (in cycles)

K = 1
# number of iterations in loop (also elements in collection, also total number of guards)
Na = 1   # only enough elements to fill up one object
Nb = K*(o/e) # enough elements to fill up K objects

xs = np.arange(Na, Nb, 2) # iteration counts (N)

x = []
ys = []
ysopt = []
for n in xs:
    di = math.ceil(o/n)
    x.append(di)
    ys.append(((di - 1) * cf) + cs)
    ysopt.append(((di - 1) * cr) + cl)

fig, ax = plt.subplots()

crossover = (cs - cl) / (cr - cf)
print('inflection point should be at {}'.format(crossover))

plt.figure(figsize=(5,5))
plt.grid(which='both', alpha=0.5, zorder=0)
plt.xlabel(r'# elements per object', fontsize=LABELSIZE)
plt.ylabel('projected loop cost', fontsize=LABELSIZE)


plt.plot(x, ysopt, '--', zorder=3, lw=3)
plt.plot(x, ys, '-', zorder=3, lw=3)

#print(ysopt)
#print(ys)

plt.xlim(0, 1000)
plt.ylim(0, 40000)
# ax.tick_params(axis='both', which='major', labelsize=TICKSIZE)
# ax.tick_params(axis='both', which='minor', labelsize=TICKSIZE)
plt.xticks(fontsize=TICKSIZE)
plt.yticks(fontsize=TICKSIZE)
#plt.xscale('log')
plt.legend(['loop optimized', 'baseline'], fontsize=LEGSIZE)
plt.tight_layout()
plt.savefig("{}/{}".format(path, fname))

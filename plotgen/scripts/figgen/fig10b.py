import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Copy") > 0):
            lp = x.split(":")
            lp1 = x.split(" ")
            op = []
            for i in lp1:
                if i != '':
                    op.append(i.rstrip())
            ls.append(float(op[1]))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
xlabel  = "local mem [% of 9GB]" 
cm2d = cm2d("{}/{}".format(path, fname), 
            'object size', 
            '', 
            'Stream', 18, 18, 22, 22, 'bold') 

list1 = [1, 3, 5,  7,  9]
cm = []   
i = 2
#nam = "scripts/figgen/results/fig10/log.16384/log."
#readFile(nam, i, cm)
#nam = "scripts/figgen/results/fig10/log.8192/log."
#readFile(nam, i, cm)
nam = "scripts/figgen/results/fig10/log.4096/log."
readFile(nam, i, cm)
nam = "scripts/figgen/results/fig10/log.2048/log."
readFile(nam, i, cm)
nam = "scripts/figgen/results/fig10/log.1024/log."
readFile(nam, i, cm)
nam = "scripts/figgen/results/fig10/log.512/log."
readFile(nam, i, cm)
nam = "scripts/figgen/results/fig10/log.256/log."
readFile(nam, i, cm)

j=0


x =[]
j = 0
for i in range(0, len(cm)):
    x.append(str(i))
    j = j + 1

cm2d.barplot(x, cm, 0.4)

xlabs = ['4KB', '2KB', '1KB', '512B', '256B']
plt.tight_layout()
plt.xticks(x, xlabs)
#cm2d.set_y_limit(0, 920)

cm2d.save()
#cm2d.set_y_limit(0, 850)

cm2d.save()

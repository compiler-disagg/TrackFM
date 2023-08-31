import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("Sum") > 0):
            lp = x.split(":")
            ls.append(float(lp[3]))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
xlabel  = "local mem [% of 12GB]" 
cm2d = cm2d("{}/{}".format(path, fname), 
            xlabel, 
            'Speedup', 
            'Chunking', 20, 20, 22, 22, 'bold') 

list1 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]
ln = []
cm = []
cm_no_chunk = []

nam = "scripts/figgen/results/chunking_results_nov28/local/log."
readFile(nam, 1, ln)
cpp_local_sum = ln[0]
print(cpp_local_sum)


for i in list1: 
    ln.append(float(cpp_local_sum))
nam = "scripts/figgen/results/chunking_results_nov28/carm_chunk_sum/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/chunking_results_nov28/carm_chunk_no_sum/log."
for i in list1: 
    readFile(nam, i, cm_no_chunk)
j=0

sd = [] 

#for i in list1:
#    cm[j] = float(cm[j]/ln[j])
#    cm_no_chunk[j] = float(cm_no_chunk[j]/ln[j])
#    j = j + 1

for i in list1:
    sd.append(float(cm_no_chunk[j]/cm[j]))
    j = j + 1


x=[]
for i in list1:
    x.append(i/max(list1))
cm2d.plot(x, sd, '--', 'white', 'o', 10)
#cm2d.plot(x, cm_no_chunk, '-', 'white', '+', 10)

#cm2d.set_legend(['loop optimized', 'baseline'], 1, 20)
cm2d.set_y_limit(1.4, 2.2)

cm2d.save()

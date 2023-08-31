import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt
import statistics

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
            ls.append(float(major))

path    = sys.argv[1]
fname   = sys.argv[2]
plt.figure(figsize=(5,5))
xlabel  = "local mem [% of 2GB]" 
cm2d = cm2d("{}/{}".format(path, fname), 
             xlabel, 
             r'total data Fetched (GB)',
            'UMAP', 18, 18, 20, 20, 'bold') 

list1 = [128, 256, 384, 512, 768, 1024, 1536, 2048]
cm_4096 = []
cm_64 = []
fs = []

nam = "scripts/figgen/results/umap_results_nov27/carm_fetches/4K/log."
for i in list1: 
    readFile(nam, i, cm_4096)
nam = "scripts/figgen/results/umap_results_nov27/carm_fetches/64K/log."
for i in list1: 
    readFile(nam, i, cm_64)
nam = "scripts/figgen/results/umap_results_nov27/fastswap_logs/log."
for i in list1: 
    readFile(nam, i, fs)

x=[]
for i in list1:
    x.append(i/max(list1))
cm2d.plot(x, [float((c * 64)  /1000000000) for c in cm_64], '-', 'white', 'o', 10)
cm2d.plot(x, [float((f * 4096)/1000000000) for f in fs], '--', 'white', '*', 10)
#cm2d.plot(x, [float((c * 4096)/1000000000) for c in cm_4096], '-', 'white', 'o', 10)

j=0
sd1 = []
sd2 = []
sd3 = []

for f in fs:
  sd1.append(float((cm_64[j] * 64)/1000000000/2))
  #sd2.append(float((cm_4096[j] * 4096)/1000000000/2))
  sd3.append(float((fs[j] * 4096)/1000000000/2))
  print(str(float(list1[j]/max(list1))) +" : "+ str(float((cm_64[j] * 64)/1000000000)))
  #print(str(float(list1[j]/max(list1))) +" : "+ str(float((cm_4096[j] * 4096)/1000000000)))
  print(str(float(list1[j]/max(list1))) +" : " + str(float((fs[j] * 4096)/1000000000)))
  j = j + 1

print(statistics.mean(sd1))
#print(statistics.mean(sd2))
print(statistics.mean(sd3))
#cm2d.set_y_lg_scale()
#cm2d.plot(x, cm, '-', 'white', 'o', 10)
#cm2d.plot(x, fs, '--', 'white', '*', 10)
cm2d.set_legend(['TrackFM 64B', 'Fastswap'], 1, 18)

cm2d.save()

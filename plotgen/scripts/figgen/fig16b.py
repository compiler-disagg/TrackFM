import sys
sys.path.append("scripts/figgen/lib")
import matplotlib.pyplot as plt

from carmfig import Carm2DFig as cm2d

def readFile (fname, i, ls):
    lin = fname +str(i);
    f = open(lin, "r")
    flag = 0
    for x in f:
        if (x.count("$1 =") > 0):
            lp = x.split("=")
            remote = float(lp[1])
        elif (x.count("$2 =") > 0):
            lp = x.split("=")
            local = float(lp[1])
            ls.append(float(local + remote))
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
cm2d = cm2d("{}/{}".format(path, fname), 
            'zipf skew param.', 
             r'#guards/faults ($\times$100M)',
            'Memcached', 16, 16, 18, 18, 'bold') 

list1 = [1.01, 1.02, 1.04, 1.08, 1.1, 1.12, 1.14, 1.18, 1.20, 1.22, 1.24, 1.28, 1.3]
fs = []
cm = []

##############################################
#exclude setup cost for both systems#
#Major (requiring I/O) page faults: 114616337
#Minor (reclaiming a frame) page faults: 238552900



##############################################
#do not include KV setup cost
cm_setup_const_guards_cost = 1350665234
fs_setup_const_guards_cost = 114616337 + 238552900 
cm_obj_size = 128
fastswap_page_size = 4096

nam = "scripts/figgen/results/fig16b/TrackFM/log."
for i in list1: 
    readFile(nam, i, cm)
nam = "scripts/figgen/results/fig16b/fastswap/trace_log."
for i in list1: 
    readFile(nam, i, fs)
j=0


for i in cm:
    cm[j] = (cm[j] - cm_setup_const_guards_cost) 
    fs[j] = ((fs[j] - fs_setup_const_guards_cost)) 
    j = j + 1
x=[]
for i in list1:
    x.append(i)
cm2d.plot(x, [c/100000000 for c in cm], '-', 'white', 'o', 10)
cm2d.plot(x, [f/100000000 for f in fs], '--', 'white', '*', 10)

#cm2d.set_y_lg_scale()
#cm2d.plot(x, cm, '-', 'white', 'o', 10)
#cm2d.plot(x, fs, '--', 'white', '*', 10)
#cm2d.set_legend(['TrackFM Data Transfered', 'Fastswap Data Transfered'], 1, 16)

cm2d.save()

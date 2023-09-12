import numpy as np
import sys

DATA_SIZE = 50000000

def gen_access_trace():
    zipf =  1.02 
    hot_keys = np.random.zipf(zipf, DATA_SIZE)
    hot_keys = hot_keys * 256
    with open('zipf.txt', 'w') as f:
        for line in hot_keys:
            f.write("%s\n" % (str(line % DATA_SIZE)))

gen_access_trace()

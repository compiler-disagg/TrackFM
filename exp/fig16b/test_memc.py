from pymemcache.client.base import Client
from pymemcache.client.retrying import RetryingClient
from pymemcache.exceptions import MemcacheUnexpectedCloseError
from timeit import default_timer as timer
import time
from random import randrange
from scipy.special import zeta
import numpy as np
import sys


DATA_SIZE = 100000000
GET_OPS   = 100000000


key_suffix = ""
value_suffix = ""
second=1000000000
micro_delay=10
delay=float(micro_delay/second)
f = open('log.'+str(sys.argv[1]), 'w')

def set_key_value(client):
    for i in range(0, DATA_SIZE):
        client.set(str(i)+key_suffix, str(i)+value_suffix)
        time.sleep(delay)


def get_zipf_key_value(client, a):

    hot_keys = np.random.zipf(a, GET_OPS)
    hot_keys = hot_keys * 128
    iterations =  1
    start = timer()
    for j in range(0,iterations):
        for i in range(0, len(hot_keys)):
            result = client.get(str(hot_keys[i]%DATA_SIZE) + key_suffix)
            if (result.rstrip().count((str(hot_keys[i]%DATA_SIZE)+value_suffix) == 0)):
                print("verification failed expect " + str(hot_keys[i]%DATA_SIZE)+value_suffix + "found" + str(result))
                exit(0)
    end = timer()
    f.write(" a : "+ str(a) + " : " + str(end - start)+"\n")
    f.flush()

def zipf_access_pattern():
    client = Client('127.0.0.1:9999')
    f.write("client connected\n")
    f.flush()
    set_key_value(client)
    f.write("setup complete\n")
    f.flush()
    get_zipf_key_value(client, float(sys.argv[1]))

zipf_access_pattern()

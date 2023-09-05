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

def set_key_value(client):
    for i in range(0, DATA_SIZE):
        client.set(str(i)+key_suffix, str(i)+value_suffix)


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
    print(" a : "+ str(a) + " : " + str(end - start))

def zipf_access_pattern():
    zipfr = [ 1.01, 1.02, 1.04, 1.08, 1.1, 1.12, 1.14, 1.18, 1.2, 1.22, 1.24, 1.28, 1.3 ]
    client = Client('127.0.0.1:9999')
    set_key_value(client)
    print("setup complete")
    for a in zipfr:
        get_zipf_key_value(client, a)

zipf_access_pattern()

import random
import sys
import os
from functools import lru_cache
import pylru
from tqdm import tqdm

class RandoCache(object):
    def __init__(self, sz):
        self.d = dict()
        self.sz = sz
    
    def __contains__(self, item):
        return item in self.d
    
    def __getitem__(self, key):
        return self.d[key]
    
    def __setitem__(self, key, value):
        if len(self.d) < self.sz:
            self.d[key] = value
            return
        rand_key = random.choice(list(self.d))
        del self.d[rand_key]
        self.d[key] = value


fname = '/scratch/karthik/projects/ShapeWorks/Examples/Python/Output/femur_mesh/shape_models/distance_queries.txt'
fd = open(fname)
N = 10982
total_lines = os.path.getsize(fname) // 10 # approx
total_iters = 250 # HACK TO GET MORE SAMPLES
lines_per_iter = total_lines // total_iters


caches = {}
cache_hits = {}
cache_misses = {}
for cache_size in [3072]:
    # caches[cache_size] = pylru.lrucache(cache_size)
    caches[cache_size] = RandoCache(cache_size)
    cache_hits[cache_size] = 0
    cache_misses[cache_size] = 0

for n_lines, line in tqdm(enumerate(fd), total=total_lines):
    n_lines += 1
    i, j = [int(x) for x in line.split()]

    for cache_size in caches.keys():
        if i in caches[cache_size]:
            _ = caches[cache_size][i] # force it to be "recently used"
            cache_hits[cache_size] += 1
        else:
            cache_misses[cache_size] += 1
            caches[cache_size][i] = 0.0
    
    if n_lines % 5000000 == 0:
        for cache_size in caches.keys():
            cache_size_bytes = cache_size * N * 8
            print(f'{cache_size},{cache_size_bytes},{n_lines},{total_lines},{cache_hits[cache_size]},{cache_misses[cache_size]},{cache_misses[cache_size]/n_lines}')
            print(f"SANITY: {len(caches[cache_size].d)}")

for cache_size in caches.keys():
    cache_size_bytes = cache_size * N * 8
    print(f'{cache_size},{cache_size_bytes},{n_lines},{total_lines},{cache_hits[cache_size]},{cache_misses[cache_size]},{cache_misses[cache_size]/n_lines}')

# print(f'number of accesses: {n_lines} / ~{total_lines}')
# print(f'approx iters: {n_iters} / {total_iters}')
# print(f'Cache(size={cache_size}): {cache_hits:>9} hits | {cache_misses:>9} misses | {cache_misses / (cache_misses + cache_hits):.2} miss rate | ~{cache_size_bytes/2**20:.3} MB')

import random
import sys
import os
from functools import lru_cache
import pylru
from tqdm import tqdm

fname = '/scratch/karthik/projects/ShapeWorks/Examples/Python/Output/femur_mesh/shape_models/distance_queries.txt'
fd = open(fname)
N = 10982
total_lines = os.path.getsize(fname) // 10 # approx
total_iters = 250 # HACK TO GET MORE SAMPLES
lines_per_iter = total_lines // total_iters
cache_size = int(sys.argv[1])
cache_size_bytes = cache_size * N * 8

for n_iters in tqdm(range(10)):
    cache = pylru.lrucache(cache_size)

    # seek to a random position
    seek_to = random.randrange(0, total_lines*10)
    fd.seek(seek_to)
    fd.readline()


    n_lines = 0
    cache_hits = 0
    cache_misses = 0

    for _ in range(lines_per_iter):
        line = fd.readline()
        if not line:
            break # oops, seeked to a point close to the end
        n_lines += 1
        i, j = [int(x) for x in line.split()]

        if i in cache:
            _ = cache[i] # force it to be "recently used"
            cache_hits += 1
        else:
            cache_misses += 1
            cache[i] = 0.0
    print(f'{cache_size},{cache_size_bytes},{n_lines},{total_lines},{cache_hits},{cache_misses}')

# print(f'number of accesses: {n_lines} / ~{total_lines}')
# print(f'approx iters: {n_iters} / {total_iters}')
# print(f'Cache(size={cache_size}): {cache_hits:>9} hits | {cache_misses:>9} misses | {cache_misses / (cache_misses + cache_hits):.2} miss rate | ~{cache_size_bytes/2**20:.3} MB')

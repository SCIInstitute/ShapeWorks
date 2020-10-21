from tqdm import tqdm
from collections import defaultdict
import numpy as np
import matplotlib.pyplot as plt
import matplotlib


font = {'family' : 'normal',
        'weight' : 'bold',
        'size'   : 22}
matplotlib.rc('font', **font)



filepath='/scratch/karthik/projects/ShapeWorks/Examples/Python/Output/femur_mesh/shape_models/distance_queries_counts.txt'
D = np.fromfile('/scratch/karthik/projects/geodesic-sandbox/python/distance_matrix/femur_10982.bin', dtype=np.float64).reshape((10982, 10982)).T

hists = {}
key = ''
stop_distance = 45.0
misses = 0
with open(filepath) as fd:
    for line in fd:
        if line.startswith('#'):
            key = line[2:]
            hists[key] = defaultdict(int)
            continue

        i,j,count = [int(x) for x in line.split()]
        dist = D[i,j]
        if dist > stop_distance:
            misses += 1
        hists[key][dist] += count

# print(f'Misses with stop_distance=={stop_distance}: {misses}')
# print(f'Total: {sum(hist.values())}')
# print(f'Unique: {len(hist)} / {10982*10982}')


total_queries = 0
for i,(key, hist) in enumerate(hists.items()):
    val, weight = zip(*[(k, v) for k,v in hist.items()])
    c1 = np.array([0, 1.0, 0])
    c2 = np.array([1.0, 0, 0])
    t = i / len(hists)
    c = (1-t) * c1  + t * c2;

    total_queries += sum(hist.values())

    plt.hist(val, weights=weight, bins=400, label=key, histtype='step', stacked=True, fill=True, color=c)
print('total queries: {}', total_queries)

plt.grid(axis='y')
# plt.yscale('log')
plt.legend()
plt.title('Geodesic distances queried for 500 optimization iterations')
plt.xlabel('Distance between points')
plt.ylabel('Frequency')
plt.show()
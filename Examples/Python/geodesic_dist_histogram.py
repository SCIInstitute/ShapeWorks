from tqdm import tqdm
from collections import defaultdict
import numpy as np
import matplotlib.pyplot as plt
import matplotlib


font = {'family' : 'normal',
        'weight' : 'bold',
        'size'   : 22}
matplotlib.rc('font', **font)



filepath='/scratch/karthik/projects/ShapeWorks/Examples/Python/Output/femur_mesh/shape_models/dq_onlyfinal.txt'
D = np.fromfile('/scratch/karthik/projects/geodesic-sandbox/python/distance_matrix/femur_10982.bin', dtype=np.float64).reshape((10982, 10982)).T

hist = defaultdict(int)
with open(filepath) as fd:
    for line in fd:
        i,j,count = [int(x) for x in line.split()]
        dist = D[i,j]
        hist[dist] += count

print(f'Total: {sum(hist.values())}')

val, weight = zip(*[(k, v) for k,v in hist.items()])
plt.hist(val, weights=weight, bins=80)
plt.grid(axis='y')
plt.title('Geodesic distances queried for 500 optimization iterations')
plt.xlabel('Distance between points')
plt.ylabel('Frequency')
plt.show()
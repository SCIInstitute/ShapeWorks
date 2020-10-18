import numpy as np
import matplotlib.pyplot as plt
from matplotlib import colors

c = 'C0'
cmap = colors.LinearSegmentedColormap.from_list(
        'incr_alpha', [(0, (*colors.to_rgb(c),0.1)), (1, (1.0, 0.0, 0.0, 1.0))])

filepath = 'distance_queries.txt'

curr = {}
n = 0
with open(filepath) as fd:
    for line in fd:
        if line.startswith('#'):
            n = int(line.split()[2])
            curr[n] = np.zeros((10982, 10982))
            continue
        x,y,v = [int(x) for x in line.split()]
        curr[n][x,y] = v

print(n)
nz_x, nz_y = np.nonzero(curr[n])
max_value = np.max(curr[n])
plt.spy(curr[n])
plt.scatter(nz_x, nz_y, s=1, c=curr[n][nz_x,nz_y], cmap=cmap)
plt.colorbar()
plt.show()

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import colors

c = 'C0'
cmap = colors.LinearSegmentedColormap.from_list(
        'incr_alpha', [(0, (*colors.to_rgb(c),0.1)), (1, (1.0, 0.0, 0.0, 1.0))])

filepath = '/scratch/karthik/projects/ShapeWorks/Examples/Python/Output/femur_mesh/shape_models/dq_onlyfinal.txt'

dist = {}
with open(filepath) as fd:
    for line in fd:
        x,y,v = [int(x) for x in line.split()]
        dist[(x,y)] = v

print(f'Sparsity: {len(dist)} / {10982*10982}')
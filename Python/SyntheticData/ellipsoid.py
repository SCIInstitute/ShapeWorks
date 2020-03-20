import numpy as np
import nrrd
import matplotlib.pyplot as plt
from tqdm import trange
import random

N = 256

def func(x, y, z):
    a = 1
    b = 1
    c = random.random() * 3
    return (x*x/a + y*y/b + z*z/c - 1) < 0

x = np.linspace(-2, 2, N)[:, np.newaxis, np.newaxis]
y = np.linspace(-2, 2, N)[np.newaxis, :, np.newaxis]
z = np.linspace(-2, 2, N)[np.newaxis, np.newaxis, :]

for i in trange(30):
    data = func(x, y, z).astype(np.float)
    nrrd.write(f'out/seg.ellipsoid_{i}.nrrd', data, index_order='C')

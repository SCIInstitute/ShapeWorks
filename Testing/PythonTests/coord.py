import os
import sys
import numpy as np
from shapeworks import *

success = True

# note: we just use numpy arrays for coordinates/indices, points, dimensions, vectors, and matrices
def coordTest():
  c1 = np.array([1.0, 1.0, 1.0])
  c2 = np.array([2.0, 2.0, 1.0])
  
  c2[2] = 2

  c3 = c1 + c2
  c4 = c2 - c1
  c5 = c1 * c2
  c1 += c2
  c1 -= c2
  c6 = c2 * 10
  c7 = c2 / 2
  c6 *= 4
  c6 /= 2

  return c6[0] == 40 and c6[1] == 40 and c6[2] == 40

success &= utils.test(coordTest)

sys.exit(not success)

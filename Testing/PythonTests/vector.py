import os
import sys
from shapeworks import *

def vectorTest():
  v1 = Vector(1, 1, 1)
  v2 = Vector(2, 2, 1)
  
  v2[2] = 2

  v3 = v1 + v2
  v4 = v2 - v1
  v5 = v1 * v2
  v1 += v2
  v1 -= v2
  v6 = v2 * 10
  v7 = v2 / 2.0
  v6 *= 4
  v6 /= 2

  return v6[0] == 40 and v6[1] == 40 and v6[2] == 40

val = vectorTest()

if val is False:
  sys.exit(1)
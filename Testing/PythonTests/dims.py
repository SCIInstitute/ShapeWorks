import os
import sys
from shapeworks import *

def dimsTest():
  d1 = Dims(1, 1, 1)
  d2 = Dims(2, 2, 1)
  
  d2[2] = 2

  d3 = d1 + d2
  d4 = d2 - d1
  d5 = d1 * d2
  d1 += d2
  d1 -= d2
  d6 = d2 * 10
  d7 = d2 / 2
  d6 *= 4
  d6 /= 2

  return d6[0] == 40 and d6[1] == 40 and d6[2] == 40

val = dimsTest()

if val is False:
  sys.exit(1)
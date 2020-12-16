import os
import sys
from shapeworks import *

def coordTest():
  c1 = Coord(1, 1, 1)
  c2 = Coord(2, 2, 1)
  
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

val = coordTest()

if val is False:
  sys.exit(1)
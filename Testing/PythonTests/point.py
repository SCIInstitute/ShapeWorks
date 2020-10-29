import os
import sys
from shapeworks import *

def pointTest():
  p1 = Point(1, 1, 1)
  p2 = Point(2, 2, 1)
  
  p2[2] = 2

  p3 = p1 + p2
  p4 = p2 - p1
  p5 = p1 * p2
  p1 += p2
  p1 -= p2
  p6 = p2 * 10
  p7 = p2 / 2
  p6 *= 4
  p6 /= 2

  return p6[0] == 40 and p6[1] == 40 and p6[2] == 40

val = pointTest()

if val is False:
  sys.exit(1)
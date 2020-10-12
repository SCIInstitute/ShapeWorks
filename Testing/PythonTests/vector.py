from pythonTestSetup import *

setup()

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
#   v7 = v2 / 2 # TODO: need to bind them
#   v6 *= 4 # TODO: need to bind them
#   v6 /= 2 # TODO: need to bind them

  return v6[0] == 20 and v6[1] == 20 and v6[2] == 20

val = vectorTest()

if val is False:
  sys.exit(1)
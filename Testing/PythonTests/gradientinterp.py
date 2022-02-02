import os
import sys
from shapeworks import *

success = True

def gradientinterpTest():
  img = Image(os.environ["DATA"] + "/computedt2.nrrd")
  interpolator = VectorImage(img)
  vec = interpolator.evaluate([10.0,20.0,30.0])

  return vec == [0.7374552488327026, 0.5726523995399475, 0.39806556701660156]

success &= utils.test(gradientinterpTest)

sys.exit(not success)

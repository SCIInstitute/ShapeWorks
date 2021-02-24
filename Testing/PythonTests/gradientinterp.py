import os
import sys
from shapeworks import *

def gradientInterpTest1():
  img = Image(os.environ["DATA"] + "/computedt2.nrrd")
  interpolator = GradientInterpolator(img)
  vec = interpolator.evaluate([10.0,20.0,30.0])

  return vec == [0.7374552488327026, 0.5726523995399475, 0.39806556701660156]

val = gradientInterpTest1()

if val is False:
  sys.exit(1)

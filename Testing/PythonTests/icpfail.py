import os
import sys
from shapeworks import *

def icpfailTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  xform = img.createRigidRegistrationTransform()

  compareImg = Image(os.environ["DATA"] + "/icpfail.nrrd")

  return img.compare(compareImg)

val = icpfailTest1()

if val is False:
  sys.exit(1)
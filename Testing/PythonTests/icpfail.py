import os
import sys
from shapeworks import *

def icpfailTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  xform = ImageUtils.createRigidRegistrationTransform()

  compareImg = Image(os.environ["DATA"] + "/icpfail.nrrd")

  return img.compare(compareImg)

val = icpfailTest1()

if val is False:
  sys.exit(1)

def icpfailTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  imgSource = Image(os.environ["DATA"] + "/smooth1.nrrd")
  xform = ImageUtils.createRigidRegistrationTransform(imgSource)

  compareImg = Image(os.environ["DATA"] + "/icpfail.nrrd")

  return img.compare(compareImg)

val = icpfailTest2()

if val is False:
  sys.exit(1)
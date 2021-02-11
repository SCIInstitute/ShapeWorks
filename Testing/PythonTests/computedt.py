import os
import sys
import shapeworks as sw

def computedtTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.computeDT()

  compareImg = sw.Image(os.environ["DATA"] + "/computedt1.nrrd")

  return img.compare(compareImg)

val = computedtTest1()

if val is False:
  sys.exit(1)

def computedtTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.computeDT(1.0)

  compareImg = sw.Image(os.environ["DATA"] + "/computedt2.nrrd")

  return img.compare(compareImg)

val = computedtTest2()

if val is False:
  sys.exit(1)

def computedtTest3():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.computeDT(-1.0)

  compareImg = sw.Image(os.environ["DATA"] + "/computedt3.nrrd")

  return img.compare(compareImg)

val = computedtTest3()

if val is False:
  sys.exit(1)
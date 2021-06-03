import os
import sys
from shapeworks import *

def topoTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = img.topologyPreservingSmooth(20.0, 10.0, 10.0)

  compareImg = Image(os.environ["DATA"] + "/topo1.nrrd")

  return topoImg.compare(compareImg)

val = topoTest1()

if val is False:
  print("topoTest1 failed")
  sys.exit(1)

def topoTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = img.topologyPreservingSmooth(10.0, 10.5, 10.5)

  compareImg = Image(os.environ["DATA"] + "/topo2.nrrd")

  return topoImg.compare(compareImg)

val = topoTest2()

if val is False:
  print("topoTest2 failed")
  sys.exit(1)

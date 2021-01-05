import os
import sys
from shapeworks import *

def topoTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = ImageUtils.topologyPreservingSmooth(img, 20.0, 10.0, 10.0)

  compareImg = Image(os.environ["DATA"] + "/topo1.nrrd")

  return topoImg.compare(compareImg)

val = topoTest1()

if val is False:
  sys.exit(1)

def topoTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = ImageUtils.topologyPreservingSmooth(img, 10.0, 10.5, 10.5)

  compareImg = Image(os.environ["DATA"] + "/topo2.nrrd")

  return topoImg.compare(compareImg)

val = topoTest2()

if val is False:
  sys.exit(1)
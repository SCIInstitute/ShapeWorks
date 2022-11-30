import os
import sys
from shapeworks import *

success = True

def topoTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = img.topologyPreservingSmooth(20.0, 10.0, 10.0)

  compareImg = Image(os.environ["DATA"] + "/topo1.nrrd")

  return topoImg.compare(compareImg)

success &= utils.test(topoTest1)

def topoTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = img.topologyPreservingSmooth(10.0, 10.5, 10.5)

  compareImg = Image(os.environ["DATA"] + "/topo2.nrrd")

  return topoImg.compare(compareImg)

success &= utils.test(topoTest2)

sys.exit(not success)

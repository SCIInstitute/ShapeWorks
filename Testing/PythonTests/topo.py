import os
import sys
import shapeworks as sw

def topoTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = img.topologyPreservingSmooth(20.0, 10.0, 10.0)

  compareImg = sw.Image(os.environ["DATA"] + "/topo1.nrrd")

  return topoImg.compare(compareImg)

val = topoTest1()

if val is False:
  sys.exit(1)

def topoTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = img.topologyPreservingSmooth(10.0, 10.5, 10.5)

  compareImg = sw.Image(os.environ["DATA"] + "/topo2.nrrd")

  return topoImg.compare(compareImg)

val = topoTest2()

if val is False:
  sys.exit(1)

import os
import sys
import shapeworks as sw

def setoriginTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.setOrigin()

  compareImg = sw.Image(os.environ["DATA"] + "/setorigin1.nrrd")

  return img.compare(compareImg)

val = setoriginTest1()

if val is False:
  sys.exit(1)

def setoriginTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.setOrigin([1, 0, 0])

  compareImg = sw.Image(os.environ["DATA"] + "/setorigin2.nrrd")

  return img.compare(compareImg)

val = setoriginTest2()

if val is False:
  sys.exit(1)

def setoriginTest3():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.setOrigin([-1, 0, 0])

  compareImg = sw.Image(os.environ["DATA"] + "/setorigin3.nrrd")

  return img.compare(compareImg)

val = setoriginTest3()

if val is False:
  sys.exit(1)

def setoriginTest4():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.setOrigin([1, 1, 1])

  compareImg = sw.Image(os.environ["DATA"] + "/setorigin4.nrrd")

  return img.compare(compareImg)

val = setoriginTest4()

if val is False:
  sys.exit(1)
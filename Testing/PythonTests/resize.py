from pythonTestSetup import *

setup()

from shapeworks import *

def resizeTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.resize(Dims(0, 0, 0), InterpolationType.Linear)

  compareImg = Image(os.environ["DATA"] + "1x2x2.nrrd")

  return img.compare(compareImg)

val = resizeTest1()

if val is False:
  sys.exit(1)

def resizeTest2():
  img = Image(os.environ["DATA"] + "la1-small.nrrd")
  img.resize(Dims(2, 0, 0), InterpolationType.Linear)

  compareImg = Image(os.environ["DATA"] + "resize2.nrrd")

  return img.compare(compareImg)

val = resizeTest2()

if val is False:
  sys.exit(1)

def resizeTest3():
  img = Image(os.environ["DATA"] + "la1-small.nrrd")
  img.resize(Dims(96, 96, 12), InterpolationType.Linear)

  compareImg = Image(os.environ["DATA"] + "resize3.nrrd")

  return img.compare(compareImg)

val = resizeTest2()

if val is False:
  sys.exit(1)

def resizeTest4():
  img = Image(os.environ["DATA"] + "la1-small.nrrd")
  img.resize(Dims(24, 120, 6), InterpolationType.Linear)

  compareImg = Image(os.environ["DATA"] + "resize4.nrrd")

  return img.compare(compareImg)

val = resizeTest4()

if val is False:
  sys.exit(1)
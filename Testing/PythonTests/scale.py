from pythonTestSetup import *

setup()

from shapeworks import *

def scaleTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.scale(Vector(1.0, 2.0, 2.0))

  compareImg = Image(os.environ["DATA"] + "scale1.nrrd")

  return img.compare(compareImg)

val = scaleTest1()

if val is False:
  sys.exit(1)

def scaleTest2():
  img = Image(os.environ["DATA"] + "la-bin-centered.nrrd")
  img.scale(Vector(1.0, -2.0, -1.0))

  compareImg = Image(os.environ["DATA"] + "scale2.nrrd")

  return img.compare(compareImg)

val = scaleTest2()

if val is False:
  sys.exit(1)
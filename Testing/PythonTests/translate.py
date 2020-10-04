from pythonTestSetup import *

setup()

from shapeworks import *

def translateTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.translate(Vector(10.0, 10.0, 10.0))

  compareImg = Image(os.environ["DATA"] + "translate1.nrrd")

  return img.compare(compareImg)

val = translateTest1()

if val is False:
  sys.exit(1)

def translateTest2():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.translate(Vector(-10.0, -10.0, -10.0))

  compareImg = Image(os.environ["DATA"] + "translate2.nrrd")

  return img.compare(compareImg)

val = translateTest2()

if val is False:
  sys.exit(1)
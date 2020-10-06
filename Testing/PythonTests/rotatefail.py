from pythonTestSetup import *

setup()

from shapeworks import *

def rotatefailTest1():
  img = Image(os.environ["DATA"] + "la-bin-centered.nrrd")
  img.rotate(0.785, Vector(0.0, 0.0, 1.0))

  compareImg = Image(os.environ["DATA"] + "rotate1.nrrd")

  return img.compare(compareImg)

val = rotatefailTest1()

if val is False:
  sys.exit(1)

def rotatefailTest2():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.rotate(0, Vector(0.0, 0.0, 1.0))

  compareImg = Image(os.environ["DATA"] + "rotatefail.nrrd")

  return img.compare(compareImg)

val = rotatefailTest2()

if val is False:
  sys.exit(1)
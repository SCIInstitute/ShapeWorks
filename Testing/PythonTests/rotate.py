from pythonTestSetup import *

setup()

from shapeworks import *

def rotateTest1():
  img = Image(os.environ["DATA"] + "la-bin-centered.nrrd")
  img.rotate(0.7854, Vector(1.0, 1.0, 1.0))

  compareImg = Image(os.environ["DATA"] + "rotate2.nrrd")

  return img.compare(compareImg)

val = rotateTest1()

if val is False:
  sys.exit(1)
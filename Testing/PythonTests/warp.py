from pythonTestSetup import *

setup()

from shapeworks import *

def warpTest1():
  img = Image(os.environ["DATA"] + "input.nrrd")
  Image
  img.reflect(Axis.Z)

  compareImg = Image(os.environ["DATA"] + "reflect1.nrrd")

  return img.compare(compareImg)

val = warpTest1()

if val is False:
  sys.exit(1)
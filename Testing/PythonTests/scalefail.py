from pythonTestSetup import *

setup()

from shapeworks import *

def scalefailTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.scale(Vector(1.0, 1.0, 1.0))

  compareImg = Image(os.environ["DATA"] + "scalefail.nrrd")

  return img.compare(compareImg)

val = scalefailTest1()

if val is False:
  sys.exit(1)
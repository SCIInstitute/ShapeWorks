from pythonTestSetup import *

setup()

from shapeworks import *

def compareFailTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")

  compareImg = Image(os.environ["DATA"] + "la-bin.nrrd")

  return img.compare(compareImg)

val = compareFailTest1()

if val is False:
  sys.exit(1)

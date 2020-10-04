from pythonTestSetup import *

setup()

from shapeworks import *

def recenterTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.recenter()

  compareImg = Image(os.environ["DATA"] + "recenter.nrrd")

  return img.compare(compareImg)

val = recenterTest1()

if val is False:
  sys.exit(1)
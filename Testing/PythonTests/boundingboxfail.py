from pythonTestSetup import *

setup()

from shapeworks import *

def boundingboxfailTest1():
  filenames = os.environ["DATA"] + "empty/*.nrrd"
  region = ImageUtils.boundingBox(filenames, 1.0)

  return region.valid()

val = boundingboxTest1()

if val is False:
  sys.exit(1)

def boundingboxfailTest2():
  filenames = os.environ["DATA"] + "single/*.nrrd"
  region = ImageUtils.boundingBox(filenames, 1.0)

  return region.valid()

val = boundingboxTest2()

if val is False:
  sys.exit(1)
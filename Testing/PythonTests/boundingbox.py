from pythonTestSetup import *

setup()

from shapeworks import *

def boundingboxTest():
  filenames = os.environ["DATA"] + "many/*.nrrd"
  region = ImageUtils.boundingBox(filenames, 1.0)

  return region.valid()

val = boundingboxTest()

if val is False:
  sys.exit(1)
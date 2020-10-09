from pythonTestSetup import *

setup()

from shapeworks import *
import glob

def boundingboxTest():
  filenames = os.environ["DATA"] + "many/"
  region = ImageUtils.boundingBox(glob.glob(filenames + "/*.nrrd"))

  return region.valid()

val = boundingboxTest()

if val is False:
  sys.exit(1)
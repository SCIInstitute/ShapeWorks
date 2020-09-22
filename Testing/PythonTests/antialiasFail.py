from pythonTestSetup import *

setup()

from shapeworks import *

def antialiasFailTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.antialias(iterations=1)

  compareImg = Image(os.environ["DATA"] + "antialias3.nrrd")

  return img.compare(compareImg)

val = antialiasFailTest1()

if val is False:
  sys.exit(1)
from pythonTestSetup import *

setup()

from shapeworks import *

def compareTest1():
  img = Image(os.environ["DATA"] + "la-bin.nrrd")
  compareImg = Image(os.environ["DATA"] + "la-bin.nrrd")

  return img.compare(compareImg)

val = compareTest1()

if val is False:
  sys.exit(1)

def compareTest2():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  compareImg = Image(os.environ["DATA"] + "1x2x2-diff.nrrd")

  return img.compare(compareImg, tolerance=1.0)

val = compareTest2()

if val is False:
  sys.exit(1)
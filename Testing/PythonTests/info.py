from pythonTestSetup import *

setup()

from shapeworks import *

def infoTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  size1 = img.size()

  compareImg = Image(os.environ["DATA"] + "1x2x2.nrrd")
  size2 = compareImg.size()

  return size1[0] == size2[0] # It should compare the Dims array and not just one element

val = infoTest1()

if val is False:
  sys.exit(1)

def infoTest2():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  dims1 = img.dims()

  compareImg = Image(os.environ["DATA"] + "1x2x2.nrrd")
  dims2 = compareImg.dims()

  return dims1[0] == dims2[0] # It should compare the Dims array and not just one element

val = infoTest2()

if val is False:
  sys.exit(1)

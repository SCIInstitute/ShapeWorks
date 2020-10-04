from pythonTestSetup import *

setup()

from shapeworks import *

def padTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.pad(0, 0.0)

  compareImg = Image(os.environ["DATA"] + "pad1.nrrd")

  return img.compare(compareImg)

val = padTest1()

if val is False:
  sys.exit(1)

def padTest2():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.pad(-1, 0.0)

  compareImg = Image(os.environ["DATA"] + "pad2.nrrd")

  return img.compare(compareImg)

val = padTest2()

if val is False:
  sys.exit(1)

def padTest3():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.pad(0, 1.0)

  compareImg = Image(os.environ["DATA"] + "pad3.nrrd")

  return img.compare(compareImg)

val = padTest3()

if val is False:
  sys.exit(1)

def padTest4():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.pad(100, 25, 1, 10)

  compareImg = Image(os.environ["DATA"] + "pad4.nrrd")

  return img.compare(compareImg)

val = padTest4()

if val is False:
  sys.exit(1)
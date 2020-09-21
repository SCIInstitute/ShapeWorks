from pythonTestSetup import *

setup()

from shapeworks import *

def antialiasTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.antialias()

  compareImg = Image(os.environ["DATA"] + "antialias1.nrrd")

  return img.compare(compareImg)

val = antialiasTest1()

if val is False:
  sys.exit(1)

def antialiasTest2():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.antialias(maxRMSErr=-1)

  compareImg = Image(os.environ["DATA"] + "antialias2.nrrd")

  return img.compare(compareImg)

val = antialiasTest2()

if val is False:
  sys.exit(1)

def antialiasTest3():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.antialias(iterations=5, maxRMSErr=0.1)

  compareImg = Image(os.environ["DATA"] + "antialias4.nrrd")

  return img.compare(compareImg)

val = antialiasTest3()

if val is False:
  sys.exit(1)
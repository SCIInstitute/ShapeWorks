from pythonTestSetup import *

setup()

from shapeworks import *

def extractlabelTest1():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.extractLabel()

  compareImg = Image(os.environ["DATA"] + "extractlabel1.nrrd")

  return img.compare(compareImg)

val = extractlabelTest1()

if val is False:
  sys.exit(1)

def extractlabelTest2():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.extractLabel(2.0)

  compareImg = Image(os.environ["DATA"] + "extractlabel2.nrrd")

  return img.compare(compareImg)

val = extractlabelTest2()

if val is False:
  sys.exit(1)

def extractlabelTest3():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.extractLabel(-2.0)

  compareImg = Image(os.environ["DATA"] + "extractlabel3.nrrd")

  return img.compare(compareImg)

val = extractlabelTest3()

if val is False:
  sys.exit(1)

def extractlabelTest4():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.extractLabel(0)

  compareImg = Image(os.environ["DATA"] + "extractlabel4.nrrd")

  return img.compare(compareImg)

val = extractlabelTest4()

if val is False:
  sys.exit(1)
import os
import sys
from shapeworks import *

success = True

def extractlabelTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel()

  compareImg = Image(os.environ["DATA"] + "/extractlabel1.nrrd")

  return img.compare(compareImg)

success &= utils.test(extractlabelTest1)

def extractlabelTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel(2.0)

  compareImg = Image(os.environ["DATA"] + "/extractlabel2.nrrd")

  return img.compare(compareImg)

success &= utils.test(extractlabelTest2)

def extractlabelTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel(-2.0)

  compareImg = Image(os.environ["DATA"] + "/extractlabel3.nrrd")

  return img.compare(compareImg)

success &= utils.test(extractlabelTest3)

def extractlabelTest4():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel(0)

  compareImg = Image(os.environ["DATA"] + "/extractlabel4.nrrd")

  return img.compare(compareImg)

success &= utils.test(extractlabelTest4)

sys.exit(not success)

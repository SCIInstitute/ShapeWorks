import os
import sys
import shapeworks as sw

def extractlabelTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel()

  compareImg = sw.Image(os.environ["DATA"] + "/extractlabel1.nrrd")

  return img.compare(compareImg)

val = extractlabelTest1()

if val is False:
  sys.exit(1)

def extractlabelTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel(2.0)

  compareImg = sw.Image(os.environ["DATA"] + "/extractlabel2.nrrd")

  return img.compare(compareImg)

val = extractlabelTest2()

if val is False:
  sys.exit(1)

def extractlabelTest3():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel(-2.0)

  compareImg = sw.Image(os.environ["DATA"] + "/extractlabel3.nrrd")

  return img.compare(compareImg)

val = extractlabelTest3()

if val is False:
  sys.exit(1)

def extractlabelTest4():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.extractLabel(0)

  compareImg = sw.Image(os.environ["DATA"] + "/extractlabel4.nrrd")

  return img.compare(compareImg)

val = extractlabelTest4()

if val is False:
  sys.exit(1)
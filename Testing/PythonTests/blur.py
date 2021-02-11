import os
import sys
import shapeworks as sw

def blurTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.gaussianBlur()

  compareImg = sw.Image(os.environ["DATA"] + "/blur1.nrrd")

  return img.compare(compareImg)

val = blurTest1()

if val is False:
  sys.exit(1)

def blurTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.gaussianBlur(1.0)

  compareImg = sw.Image(os.environ["DATA"] + "/blur2.nrrd")

  return img.compare(compareImg)

val = blurTest2()

if val is False:
  sys.exit(1)

def blurTest3():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.gaussianBlur(-1.0)

  compareImg = sw.Image(os.environ["DATA"] + "/blur3.nrrd")

  return img.compare(compareImg)

val = blurTest3()

if val is False:
  sys.exit(1)
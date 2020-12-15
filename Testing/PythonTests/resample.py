import os
import sys
from shapeworks import *

def resampleTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.resample([1, 1, 1])

  compareImg = Image(os.environ["DATA"] + "/resample1.nrrd")

  return img.compare(compareImg)

val = resampleTest1()

if val is False:
  sys.exit(1)

def resampleTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img = ImageUtils.isoresample(img, 1.5)

  compareImg = Image(os.environ["DATA"] + "/resample2.nrrd")

  return img.compare(compareImg)

val = resampleTest2()

if val is False:
  sys.exit(1)

def resampleTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.resample([1.5, 1.5, 1.5])

  compareImg = Image(os.environ["DATA"] + "/resample3.nrrd")

  return img.compare(compareImg)

val = resampleTest3()

if val is False:
  sys.exit(1)

def resampleTest4():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.resample([0.9, 3.14159, 2.5])

  compareImg = Image(os.environ["DATA"] + "/resample4.nrrd")

  return img.compare(compareImg)

val = resampleTest4()

if val is False:
  sys.exit(1)

def resampleTest5():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  transform = createTransform(Matrix())
  img.resample(transform, [-100, -63.2, -42.9], [32, 32, 4], [6.25, 6.25, 13.75], img.coordsys())

  compareImg = Image(os.environ["DATA"] + "/resample5.nrrd")

  return img.compare(compareImg)

val = resampleTest5()

if val is False:
  sys.exit(1)
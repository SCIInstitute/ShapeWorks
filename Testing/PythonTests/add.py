import os
from shapeworks import *

def addTest1():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img += img

  compareImg = Image(os.environ["DATA"] + "/la-bin-doubled.nrrd")

  return img.compare(compareImg)

utils.test(addTest1)

def addTest2():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img += 3.14

  compareImg = Image(os.environ["DATA"] + "/la-bin-plus-pi.nrrd")

  return img.compare(compareImg)

utils.test(addTest2)

def addTest3():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img += img
  img += 3.14

  compareImg = Image(os.environ["DATA"] + "/la-bin-doubled-plus-pi.nrrd")

  return img.compare(compareImg)

utils.test(addTest3)

def addfailTest():
  img1 = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img2 = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img = img1 + img2

  compareImg = Image(os.environ["DATA"] + "/addfail.nrrd")

  return img.compare(compareImg)

utils.expectException(addfailTest, ValueError)

import os
import sys
import shapeworks as sw

def addTest1():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img += img

  compareImg = sw.Image(os.environ["DATA"] + "/la-bin-doubled.nrrd")

  return img.compare(compareImg)

val = addTest1()

if val is False:
  sys.exit(1)

def addTest2():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img += 3.14

  compareImg = sw.Image(os.environ["DATA"] + "/la-bin-plus-pi.nrrd")

  return img.compare(compareImg)

val = addTest2()

if val is False:
  sys.exit(1)

def addTest3():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img += img
  img += 3.14

  compareImg = sw.Image(os.environ["DATA"] + "/la-bin-doubled-plus-pi.nrrd")

  return img.compare(compareImg)

val = addTest3()

if val is False:
  sys.exit(1)
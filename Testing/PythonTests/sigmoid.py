import os
import sys
import shapeworks as sw

def sigmoidTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applySigmoidFilter()

  compareImg = sw.Image(os.environ["DATA"] + "/sigmoid1.nrrd")

  return img.compare(compareImg)

val = sigmoidTest1()

if val is False:
  sys.exit(1)

def sigmoidTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applySigmoidFilter(10.5, 10.5)

  compareImg = sw.Image(os.environ["DATA"] + "/sigmoid2.nrrd")

  return img.compare(compareImg)

val = sigmoidTest2()

if val is False:
  sys.exit(1)

def sigmoidTest3():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applySigmoidFilter(-10.5)

  compareImg = sw.Image(os.environ["DATA"] + "/sigmoid3.nrrd")

  return img.compare(compareImg)

val = sigmoidTest3()

if val is False:
  sys.exit(1)

def sigmoidTest4():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applySigmoidFilter(beta=-10.5)

  compareImg = sw.Image(os.environ["DATA"] + "/sigmoid4.nrrd")

  return img.compare(compareImg)

val = sigmoidTest4()

if val is False:
  sys.exit(1)
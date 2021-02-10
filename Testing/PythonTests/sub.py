import os
import sys
import shapeworks as sw

def subTest1():
  img1 = sw.Image(os.environ["DATA"] + "/img1.nrrd")
  img2 = sw.Image(os.environ["DATA"] + "/img2.nrrd")
  img = img1 - img2

  compareImg = sw.Image(os.environ["DATA"] + "/img_diff.nrrd")

  return img.compare(compareImg)

val = subTest1()

if val is False:
  sys.exit(1)

def subTest2():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img -= 3.14

  compareImg = sw.Image(os.environ["DATA"] + "/la-bin-minus-pi.nrrd")

  return img.compare(compareImg)

val = subTest2()

if val is False:
  sys.exit(1)

def subTest3():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img -= img
  img -= 3.14

  compareImg = sw.Image(os.environ["DATA"] + "/la-bin-doubled-minus-pi.nrrd")

  return img.compare(compareImg)

val = subTest3()

if val is False:
  sys.exit(1)
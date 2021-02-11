import os
import sys
import shapeworks as sw

def centerofmassTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  xform = img.createTransform()
  img.applyTransform(xform)

  compareImg = sw.Image(os.environ["DATA"] + "/centerofmass1.nrrd")

  return img.compare(compareImg)

val = centerofmassTest1()

if val is False:
  sys.exit(1)

def centerofmassTest2():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  xform = img.createTransform()
  img.applyTransform(xform)

  compareImg = sw.Image(os.environ["DATA"] + "/centerofmass2.nrrd")

  return img.compare(compareImg)

val = centerofmassTest2()

if val is False:
  sys.exit(1)

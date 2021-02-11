import os
import sys
import shapeworks as sw

def padfailTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad()

  compareImg = sw.Imageage(os.environ["DATA"] + "/padfail.nrrd")

  return img.compare(compareImg)

val = padfailTest1()

if val is False:
  sys.exit(1)

def padfailTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad(0)

  compareImg = sw.Imageage(os.environ["DATA"] + "/padfail.nrrd")

  return img.compare(compareImg)

val = padfailTest2()

if val is False:
  sys.exit(1)
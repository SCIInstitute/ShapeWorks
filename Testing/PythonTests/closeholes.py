import os
import sys
import shapeworks as sw

def closeholesTest1():
  img = sw.Image(os.environ["DATA"] + "/image_with_holes.nrrd")
  img.closeHoles()

  compareImg = sw.Image(os.environ["DATA"] + "/closedholes.nrrd")

  return img.compare(compareImg)

val = closeholesTest1()

if val is False:
  sys.exit(1)
import os
import sys
import shapeworks as sw

def setspacingfailTest():
  img = sw.Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing([0,0,0])

  compareImg = sw.Image(os.environ["DATA"] + "/spacing1.nrrd")

  return img.compare(compareImg)

val = setspacingfailTest()

if val is False:
  sys.exit(1)
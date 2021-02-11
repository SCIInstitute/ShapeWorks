import os
import sys
import shapeworks as sw

def addfailTest():
  img1 = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img2 = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img = img1 + img2

  compareImg = sw.Image(os.environ["DATA"] + "/addfail.nrrd")

  return img.compare(compareImg)

val = addfailTest()

if val is False:
  sys.exit(1)
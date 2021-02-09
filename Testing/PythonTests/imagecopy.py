import os
import sys
from shapeworks import *

def copyTest1():
  img1 = Image(os.environ["DATA"] + "/femur.nrrd")
  img2 = img1.copy()
  img1.recenter()
  compareImg1 = Image(os.environ["DATA"] + "/femurRecentered.nrrd")
  compareImg2 = Image(os.environ["DATA"] + "/femur.nrrd")

  return img1 == compareImg1 and img2 == compareImg2;

val = copyTest1()

if val is False:
  sys.exit(1)


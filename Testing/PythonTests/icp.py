import os
import sys
from shapeworks import *

success = True

def icpTest1():
  imgSource = Image(os.environ["DATA"] + "/smooth1.nrrd")
  imgTarget = Image(os.environ["DATA"] + "/smooth2.nrrd")
  xform = imgSource.createRigidRegistrationTransform(imgTarget, 1.0, 5)
  imgSource.applyTransform(xform, imgTarget.origin(), imgTarget.dims(), imgTarget.spacing(), imgTarget.coordsys(), InterpolationType.NearestNeighbor)

  compareImg = Image(os.environ["DATA"] + "/icp.nrrd")

  return imgSource == compareImg

success &= utils.test(icpTest1)

def icpTest2():
  img1 = Image(os.environ["DATA"] + "/femur1_dt.nrrd")
  img2 = Image(os.environ["DATA"] + "/femur2_dt.nrrd")
  xform1_to_2 = img1.createRigidRegistrationTransform(img2)
  xform2_to_1 = img2.createRigidRegistrationTransform(img1)
  img1.applyTransform(xform1_to_2)
  img2.applyTransform(xform2_to_1)

  compareImg1 = Image(os.environ["DATA"] + "/femur1_to_2_icp.nrrd")
  compareImg2 = Image(os.environ["DATA"] + "/femur2_to_1_icp.nrrd")

  return img1 == compareImg1 and img2 == compareImg2

success &= utils.test(icpTest2)

sys.exit(not success)

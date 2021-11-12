import os
import sys
from shapeworks import *

success = True

def centerofmassTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  xform = img.createCenterOfMassTransform()
  img.applyTransform(xform)

  compareImg = Image(os.environ["DATA"] + "/centerofmass1.nrrd")

  return img.compare(compareImg)

success &= utils.test(centerofmassTest1)

def centerofmassTest2():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  xform = img.createCenterOfMassTransform()
  img.applyTransform(xform)

  compareImg = Image(os.environ["DATA"] + "/centerofmass2.nrrd")

  return img.compare(compareImg)

success &= utils.test(centerofmassTest2)

def centerofmassTest3():
  img1 = Image(os.environ["DATA"] + "/femur1_seg.nrrd")
  img2 = Image(os.environ["DATA"] + "/femur2_seg.nrrd")
  xform1 = img1.createCenterOfMassTransform()
  xform2 = img2.createCenterOfMassTransform()
  img1.applyTransform(xform1, Linear)
  img2.applyTransform(xform2, Linear)

  compareImg1 = Image(os.environ["DATA"] + "/femur1_centered_lin.nrrd")
  compareImg2 = Image(os.environ["DATA"] + "/femur2_centered_lin.nrrd")

  return img1.compare(compareImg1) and img2.compare(compareImg2)

success &= utils.test(centerofmassTest3)

def centerofmassTest4():
  img1 = Image(os.environ["DATA"] + "/femur1_seg.nrrd")
  img2 = Image(os.environ["DATA"] + "/femur2_seg.nrrd")
  xform1 = img1.createCenterOfMassTransform()
  xform2 = img2.createCenterOfMassTransform()
  img1.applyTransform(xform1, NearestNeighbor)
  img2.applyTransform(xform2, NearestNeighbor)

  compareImg1 = Image(os.environ["DATA"] + "/femur1_centered_nn.nrrd")
  compareImg2 = Image(os.environ["DATA"] + "/femur2_centered_nn.nrrd")

  return img1.compare(compareImg1) and img2.compare(compareImg2)

success &= utils.test(centerofmassTest3)

sys.exit(not success)

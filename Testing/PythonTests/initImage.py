import os
import sys
import builtins
import numpy as np
from shapeworks import *

success = True

# load an image
def initImageTest1():
  img = Image(os.environ["DATA"] + "/femurImage.nrrd")

  return img

success &= utils.test(initImageTest1)

# pass invalid path to make sure an exception is thrown
def initImageTest2():
  img = Image(os.environ["DATA"] + "/not_a_valid_path.nrrd")

  return img

success &= utils.expectException(initImageTest2, ValueError)

# load from a numpy array of the correct type
def initImageTest3a():
  arr = np.ones([25,15,5], dtype=np.float32)
  img = Image(arr)

  return img

success &= utils.test(initImageTest3a)

# ensure ownership of numpy array is transferred to image
def initImageTest3b():
  arr = np.ones([25,15,5], dtype=np.float32)
  img = Image(arr)

  return not arr.flags['OWNDATA']

success &= utils.test(initImageTest3b)

# load from a numpy array of an unsupported type
def initImageTest4():
  arr = np.ones([25,15,5], dtype=np.float64)
  img = Image(arr)

  return img

success &= utils.expectException(initImageTest4, ValueError)

# load from a numpy array and ensure the data is correct
def initImageTest5():
  arr0 = np.zeros([10,15,5], dtype=np.float32)
  arr1 = np.ones([25,15,5], dtype=np.float32)
  arr2 = np.zeros([10,15,5], dtype=np.float32)
  arr = np.concatenate((arr0, arr1, arr2))
  img = Image(arr)

  compareImg = Image(os.environ["DATA"] + "/imgFromArray.nrrd")
  return img == compareImg

success &= utils.test(initImageTest5)

# copy constructor
def initImageTest6():
  img = Image(os.environ["DATA"] + "/femurImage.nrrd")
  img_copy = Image(img)
  return img == img_copy

success &= utils.test(initImageTest6)

# assignment operator
def initImageTest7():
  img = Image(os.environ["DATA"] + "/femurImage.nrrd")
  img_copy = img
  return img == img_copy

success &= utils.test(initImageTest7)

# ensure numpy array is shared, not copied after (non-ITK) sw.Image operation
def initImageTest8():
  arr1 = np.ones([25,15,5], dtype=np.float32)
  img1 = Image(arr1)
  arr2 = np.ones([25,15,5], dtype=np.float32)
  img2 = Image(arr2)

  img1 += img2

  # Image + Image modifies original array (ones + ones = twos)
  return np.array_equal(arr1, np.ones([25,15,5], dtype=np.float32) + 1)

success &= utils.test(initImageTest8)

# helper function for testing images initialized from arrays
def imageArr():
  dims = (7,3,2) # NOTE: numpy dims are specified in z, y, x order
  farr = np.ndarray(dims, dtype=np.float32)

  ival = 10; jval = 50; kval = 1.75
  for i in builtins.range(0, farr.shape[2]):
    for j in builtins.range(0, farr.shape[1]):
      for k in builtins.range(0, farr.shape[0]):
        farr[k][j][i] = ival*(i/farr.shape[2]) + jval*(j/farr.shape[1]) + kval/farr.shape[0]

  return farr

def imageInitTest1():
  arr = imageArr()
  img = Image(arr)
  compareImg = Image(os.environ["DATA"] + "/image_from_numpy.nrrd")

  # verify image initialized and data ownership was transferred from numpy
  return not arr.flags['OWNDATA'] and img.compare(compareImg)

success &= utils.test(imageInitTest1)

def imageInitTest2():
  arr = imageArr()
  img = Image(arr)

  # verify arr and img still point to same memory
  img += 100
  return abs(img.mean() - arr.mean()) <= 1e-4
    
success &= utils.test(imageInitTest2)

def imageInitTest3():
  arr = np.array(imageArr(), dtype = np.float64)

  # verify image cannot be initialized from an array dtype other than np.float32
  img = Image(arr)
    
success &= utils.expectException(imageInitTest3, ValueError)

def imageAssignTest1():
  from scipy import ndimage

  img = Image(os.environ["DATA"] + "/femur.nrrd")

  # set spacing to show that it's preserved on both copy and assign
  img.setSpacing((1.5, 0.75, 1))

  # image won't be modified by this filter (it creates a copy)
  ck = ndimage.gaussian_filter(img.toArray(), 12.0)
  if np.array_equal(ck, img.toArray()):
      raise AssertionError("unexpected exception: filter not applied")

  # now test actual assignment
  img.assign(ck)
  compareImg = Image(os.environ["DATA"] + "/gaussian_of_femur.nrrd")

  return img.compare(compareImg)

success &= utils.test(imageAssignTest1)

def imageToArrayTest1():
  img = Image(os.environ["DATA"] + "/image_from_numpy.nrrd")
  arr = img.toArray()
  compareArr = imageArr()

  return np.array_equal(arr, compareArr)

success &= utils.test(imageToArrayTest1)

sys.exit(not success)

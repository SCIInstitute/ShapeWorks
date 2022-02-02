import os
import sys
import numpy as np
from shapeworks import *

success = True

# check for an exception
def toArrayTest1():
  arr = Image(os.environ["DATA"] + "/femurImage.nrrd").toArray()

  return isinstance(arr, np.ndarray)

success &= utils.test(toArrayTest1)

# verify data type
def toArrayTest2():
  arr = Image(os.environ["DATA"] + "/femurImage.nrrd").toArray()

  return arr.dtype == np.float32

success &= utils.test(toArrayTest2)

# verify constructed data
def toArrayTest3():
  arr0 = np.zeros([10,15,5], dtype=np.float32)
  arr1 = np.ones([25,15,5], dtype=np.float32)
  arr2 = np.zeros([10,15,5], dtype=np.float32)
  arr = np.concatenate((arr0, arr1, arr2))
  img = Image(arr)
  img_arr = img.toArray()

  return (arr==img_arr).all()

success &= utils.test(toArrayTest3)

# verify loaded data
def toArrayTest4():
  arr0 = np.zeros([10,15,5], dtype=np.float32)
  arr1 = np.ones([25,15,5], dtype=np.float32)
  arr2 = np.zeros([10,15,5], dtype=np.float32)
  arr = np.concatenate((arr0, arr1, arr2))
  img = Image(arr)

  compareImg = Image(os.environ["DATA"] + "/imgFromArray.nrrd")
  img_arr = compareImg.toArray()

  return (arr==img_arr).all()

success &= utils.test(toArrayTest4)

def imageToArrayTest2a():
    img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
    arr = img.toArray()

    # verify they both reference the same memory
    img += 100
    return abs(img.mean() - arr.mean()) <= 1e-4

success &= utils.test(imageToArrayTest2a)

def imageToArrayTest2b():
    img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
    arr = img.toArray()

    # verify they both reference the same memory (this time change arr)
    arr *= 42
    return abs(img.mean() - arr.mean()) <= 1e-4

success &= utils.test(imageToArrayTest2b)

def imageToArrayTest3():
    arr = np.copy(Image(os.environ["DATA"] + "/1x2x2.nrrd").toArray(copy=True))

    # when Image is an rvalue that goes out of scope, toArray must provide a copy
    img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
    return np.array_equal(arr, img.toArray())
    
success &= utils.test(imageToArrayTest3)

def imageToArrayTest4():
    # when Image is an rvalue that goes out of scope, toArray must provide a copy
    arrcopy = Image(os.environ["DATA"] + "/1x2x2.nrrd").toArray(copy=True)
    img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
    
    return arrcopy.flags['OWNDATA'] == True and np.array_equal(arrcopy, img.toArray())

success &= utils.test(imageToArrayTest4)

def imageToArrayTest5():
    img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
    arr = img.toArray(for_viewing=True)

    # for viewing, the array must be in fortran order to avoid being copied
    return arr.flags['F_CONTIGUOUS'] == True and arr.flags['C_CONTIGUOUS'] == False 

success &= utils.test(imageToArrayTest5)

def imageToArrayTest6():
    img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
    arr = img.toArray()

    # for viewing, the array must be in fortran order to avoid being copied
    return arr.flags['C_CONTIGUOUS'] == True and arr.flags['F_CONTIGUOUS'] == False 

success &= utils.test(imageToArrayTest6)

sys.exit(not success)

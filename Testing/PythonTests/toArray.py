import os
import sys
import numpy as np
from shapeworks import *

# check for an exception
def toArrayTest1():
  arr = Image(os.environ["DATA"] + "/femurImage.nrrd").toArray()

  return isinstance(arr, np.ndarray)

utils.test(toArrayTest1)

# verify data type
def toArrayTest2():
  arr = Image(os.environ["DATA"] + "/femurImage.nrrd").toArray()

  return arr.dtype == np.float32

utils.test(toArrayTest2)

# verify constructed data
def toArrayTest3():
  arr0 = np.zeros([10,15,5], dtype=np.float32)
  arr1 = np.ones([25,15,5], dtype=np.float32)
  arr2 = np.zeros([10,15,5], dtype=np.float32)
  arr = np.concatenate((arr0, arr1, arr2))
  img = Image(arr)
  img_arr = img.toArray()

  return (arr==img_arr).all()

utils.test(toArrayTest3)

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

utils.test(toArrayTest4)

import os
import sys
import numpy as np
from shapeworks import *

# check for an exception
def toArrayTest1():
  arr = Image(os.environ["DATA"] + "/femurImage.nrrd").toArray()

try:
  toArrayTest1()
except ValueError:
  print("toArrayTest1 failed")
  sys.exit(1)

# verify data type
def toArrayTest2():
  arr = Image(os.environ["DATA"] + "/femurImage.nrrd").toArray()
  return arr.dtype == np.float32

try:
  if not toArrayTest2():
    print("toArrayTest2 failed")
    sys.exit(1)
except ValueError:
  print("toArrayTest2 failed (exception)")
  sys.exit(1)

# verify constructed data
def toArrayTest3():
  arr0 = np.zeros([10,15,5], dtype=np.float32)
  arr1 = np.ones([25,15,5], dtype=np.float32)
  arr2 = np.zeros([10,15,5], dtype=np.float32)
  arr = np.concatenate((arr0, arr1, arr2))
  img = Image(arr)
  img_arr = img.toArray()
  np.testing.assert_equal(arr, img_arr)  # throws exception if not equal

try:
  toArrayTest3()
except ValueError:
  print("toArrayTest3")
  sys.exit(1)

# verify loaded data
def toArrayTest4():
  arr0 = np.zeros([10,15,5], dtype=np.float32)
  arr1 = np.ones([25,15,5], dtype=np.float32)
  arr2 = np.zeros([10,15,5], dtype=np.float32)
  arr = np.concatenate((arr0, arr1, arr2))
  img = Image(arr)

  compareImg = Image(os.environ["DATA"] + "/imgFromArray.nrrd")
  img_arr = compareImg.toArray()
  np.testing.assert_equal(arr, img_arr)  # throws exception if not equal

try:
  toArrayTest4()
except ValueError:
  print("toArrayTest4 failed")
  sys.exit(1)

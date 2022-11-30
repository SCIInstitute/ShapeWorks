import os
import sys
import glob
import numpy as np
from shapeworks import *

success = True

### Image bounding box tests

# compute bounding box of all files
def imageBoundingBoxTest1():
  filenames = os.environ["DATA"] + "/many/"
  region = ImageUtils.boundingBox(glob.glob(filenames + "/*.nrrd"))
  ground_truth = PhysicalRegion([8,16,16], [42,34,34])
  
  return region == ground_truth

success &= utils.test(imageBoundingBoxTest1)

# compute bounding box of a subset of the files
def imageBoundingBoxTest2():
  img1 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_1.nrrd")
  img2 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_3.nrrd")

  imgList = []
  imgList.append(img1)
  imgList.append(img2)

  region = ImageUtils.boundingBox(imgList)
  ground_truth = PhysicalRegion([9,16,16], [41,34,34])

  return region == ground_truth

success &= utils.test(imageBoundingBoxTest2)

# try to compute bounding box of nothing
def imageBoundingBoxfailTest():
  filenames = os.environ["DATA"] + "/empty/"
  region = ImageUtils.boundingBox(glob.glob(filenames + "/*.nrrd"))
  return True

success &= utils.expectException(imageBoundingBoxfailTest, ValueError)

### Mesh bounding box tests

# compute bounding box of all files
def meshBoundingBoxTest1():
  filenames = [os.environ["DATA"] + "/m03_L_femur.ply", 
               os.environ["DATA"] + "/m04_L_femur.ply",
               os.environ["DATA"] + "/femur.ply",
               os.environ["DATA"] + "/ellipsoid_0.ply",
               os.environ["DATA"] + "/femur.vtk"]

  region = MeshUtils.boundingBox(filenames)
  ground_truth = PhysicalRegion([-112.139, -192.471, -1217.76],
                                [135.026, 21.495, 1248.45])
  
  return (np.allclose(region.min, ground_truth.min) and
          np.allclose(region.max, ground_truth.max))

success &= utils.test(meshBoundingBoxTest1)

# try to compute bounding box of nothing
def meshBoundingBoxfailTest():
  filenames = os.environ["DATA"] + "/empty/"
  region = MeshUtils.boundingBox(glob.glob(filenames + "/*.nrrd"))
  return True

success &= utils.expectException(meshBoundingBoxfailTest, ValueError)

sys.exit(not success)

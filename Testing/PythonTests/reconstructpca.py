import os
import sys
from shapeworks import *

success = True

def pcamodesTestRBFS():
  print("\npython pcamodesTestRBFS")
  denseFile = os.environ["DATA"] + "/_dense.vtk"
  sparseFile = os.environ["DATA"] + "/_sparse.particles"
  goodPointsFile = os.environ["DATA"] + "/_goodPoints.txt"

  worldParticles = []
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_00.world.particles")
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_01.world.particles")
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_02.world.particles")

  reconstructor = ReconstructSurface_RBFSSparseTransform(denseFile, sparseFile, goodPointsFile)
  reconstructor.setOutPrefix("rbfs")
  reconstructor.setOutPath(".")
  reconstructor.setNumOfParticles(128)
  reconstructor.setNumOfModes(1)
  reconstructor.setNumOfSamplesPerMode(3)
  reconstructor.samplesAlongPCAModes(worldParticles)

  baselineDenseMesh1 = Mesh(os.environ["DATA"] + "/reconstruct_pca_python/rbfs_mode-00_sample-000_dense.vtk")
  baselineDenseMesh2 = Mesh(os.environ["DATA"] + "/reconstruct_pca_python/rbfs_mode-00_sample-001_dense.vtk")
  baselineDenseMesh3 = Mesh(os.environ["DATA"] + "/reconstruct_pca_python/rbfs_mode-00_sample-002_dense.vtk")

  denseMesh1 = Mesh("mode-00/rbfs_mode-00_sample-000_dense.vtk")
  denseMesh2 = Mesh("mode-00/rbfs_mode-00_sample-001_dense.vtk")
  denseMesh3 = Mesh("mode-00/rbfs_mode-00_sample-002_dense.vtk")

  print("comparing dense mesh 1...")
  success = baselineDenseMesh1 == denseMesh1
  print("comparing dense mesh 2...")
  success = success and baselineDenseMesh2 == denseMesh2
  print("comparing dense mesh 3...")
  success = success and baselineDenseMesh3 == denseMesh3
  return success

success &= utils.test(pcamodesTestRBFS)

def pcamodesTestThinPlateSpline():
  print("\npython pcamodesTestThinPlateSpline")
  denseFile = os.environ["DATA"] + "/_dense.vtk"
  sparseFile = os.environ["DATA"] + "/_sparse.particles"
  goodPointsFile = os.environ["DATA"] + "/_goodPoints.txt"

  worldParticles = []
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_00.world.particles")
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_01.world.particles")
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_02.world.particles")

  reconstructor = ReconstructSurface_ThinPlateSplineTransform(denseFile, sparseFile, goodPointsFile)
  reconstructor.setOutPrefix("tps")
  reconstructor.setOutPath(".")
  reconstructor.setNumOfParticles(128)
  reconstructor.setNumOfModes(1)
  reconstructor.setNumOfSamplesPerMode(3)
  reconstructor.samplesAlongPCAModes(worldParticles)

  baselineDenseMesh1 = Mesh(os.environ["DATA"] + "/reconstruct_pca_python/tps_mode-00_sample-000_dense.vtk")
  baselineDenseMesh2 = Mesh(os.environ["DATA"] + "/reconstruct_pca_python/tps_mode-00_sample-001_dense.vtk")
  baselineDenseMesh3 = Mesh(os.environ["DATA"] + "/reconstruct_pca_python/tps_mode-00_sample-002_dense.vtk")

  denseMesh1 = Mesh("mode-00/tps_mode-00_sample-000_dense.vtk")
  denseMesh2 = Mesh("mode-00/tps_mode-00_sample-001_dense.vtk")
  denseMesh3 = Mesh("mode-00/tps_mode-00_sample-002_dense.vtk")

  success = True
  print("comparing dense mesh 1...")
  success = baselineDenseMesh1 == denseMesh1
  print("comparing dense mesh 2...")
  success = success and baselineDenseMesh2 == denseMesh2
  print("comparing dense mesh 3...")
  success = success and baselineDenseMesh3 == denseMesh3
  return success

success &= utils.test(pcamodesTestThinPlateSpline)

sys.exit(not success)

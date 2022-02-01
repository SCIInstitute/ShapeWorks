import os
import sys
from shapeworks import *

success = True

def pcamodesTestRBFS():
  denseFile = os.environ["DATA"] + "/_dense.vtk"
  sparseFile = os.environ["DATA"] + "/_sparse.particles"
  goodPointsFile = os.environ["DATA"] + "/_goodPoints.txt"

  worldParticles = []
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_00.world.particles")
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_01.world.particles")
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_02.world.particles")

  reconstructor = ReconstructSurface_RBFSSparseTransform(denseFile, sparseFile, goodPointsFile)
  reconstructor.setOutPrefix(os.environ["DATA"])
  reconstructor.setOutPath(os.environ["DATA"])
  reconstructor.setNumOfParticles(128)
  reconstructor.setNumOfModes(1)
  reconstructor.setNumOfSamplesPerMode(3)
  reconstructor.samplesAlongPCAModes(worldParticles)

  baselineDenseMesh1 = Mesh(os.environ["DATA"] + "/RBFSSparseTransform/mode-00_sample-000_dense.vtk")
  baselineDenseMesh2 = Mesh(os.environ["DATA"] + "/RBFSSparseTransform/mode-00_sample-001_dense.vtk")
  baselineDenseMesh3 = Mesh(os.environ["DATA"] + "/RBFSSparseTransform/mode-00_sample-002_dense.vtk")

  denseMesh1 = Mesh(os.environ["DATA"] + "/mode-00/data_mode-00_sample-000_dense.vtk")
  denseMesh2 = Mesh(os.environ["DATA"] + "/mode-00/data_mode-00_sample-001_dense.vtk")
  denseMesh3 = Mesh(os.environ["DATA"] + "/mode-00/data_mode-00_sample-002_dense.vtk")

  baselineSparseParticleFiles = []
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/mode-00_sample-000_sparse.particles")
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/mode-00_sample-001_sparse.particles")
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/mode-00_sample-002_sparse.particles")

  sparseParticlesFiles = []
  sparseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-000_sparse.particles")
  sparseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-001_sparse.particles")
  sparseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-002_sparse.particles")

  baselineDenseParticleFiles = []
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/mode-00_sample-000_dense.particles")
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/mode-00_sample-001_dense.particles")
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/mode-00_sample-002_dense.particles")

  denseParticlesFiles = []
  denseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-000_dense.particles")
  denseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-001_dense.particles")
  denseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-002_dense.particles")

  baselineSparseParticles = ParticleSystem(baselineSparseParticleFiles)
  sparseParticles = ParticleSystem(sparseParticlesFiles)

  baselineDenseParticles = ParticleSystem(baselineDenseParticleFiles)
  denseParticles = ParticleSystem(denseParticlesFiles)

  success = True
  print("comparing sparse particles...")
  success = success and baselineSparseParticles.EvaluationCompare(sparseParticles)
  print("comparing dense particles...")
  success = success and baselineDenseParticles.EvaluationCompare(denseParticles)
  print("comparing dense mesh 1...")
  success = success and baselineDenseMesh1 == denseMesh1
  print("comparing dense mesh 2...")
  success = success and baselineDenseMesh2 == denseMesh2
  print("comparing dense mesh 3...")
  success = success and baselineDenseMesh3 == denseMesh3
  return success

success &= utils.test(pcamodesTestRBFS)

def pcamodesTestThinPlateSpline():
  denseFile = os.environ["DATA"] + "/_dense.vtk"
  sparseFile = os.environ["DATA"] + "/_sparse.particles"
  goodPointsFile = os.environ["DATA"] + "/_goodPoints.txt"

  worldParticles = []
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_00.world.particles")
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_01.world.particles")
  worldParticles.append(os.environ["DATA"] + "/ellipsoid_02.world.particles")

  reconstructor = ReconstructSurface_ThinPlateSplineTransform(denseFile, sparseFile, goodPointsFile)
  reconstructor.setOutPrefix(os.environ["DATA"])
  reconstructor.setOutPath(os.environ["DATA"])
  reconstructor.setNumOfParticles(128)
  reconstructor.setNumOfModes(1)
  reconstructor.setNumOfSamplesPerMode(3)
  reconstructor.samplesAlongPCAModes(worldParticles)

  baselineDenseMesh1 = Mesh(os.environ["DATA"] + "/ThinPlateSplineTransform/mode-00_sample-000_dense.vtk")
  baselineDenseMesh2 = Mesh(os.environ["DATA"] + "/ThinPlateSplineTransform/mode-00_sample-001_dense.vtk")
  baselineDenseMesh3 = Mesh(os.environ["DATA"] + "/ThinPlateSplineTransform/mode-00_sample-002_dense.vtk")

  denseMesh1 = Mesh(os.environ["DATA"] + "/mode-00/data_mode-00_sample-000_dense.vtk")
  denseMesh2 = Mesh(os.environ["DATA"] + "/mode-00/data_mode-00_sample-001_dense.vtk")
  denseMesh3 = Mesh(os.environ["DATA"] + "/mode-00/data_mode-00_sample-002_dense.vtk")

  baselineSparseParticleFiles = []
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/mode-00_sample-000_sparse.particles")
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/mode-00_sample-001_sparse.particles")
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/mode-00_sample-002_sparse.particles")

  sparseParticlesFiles = []
  sparseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-000_sparse.particles")
  sparseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-001_sparse.particles")
  sparseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-002_sparse.particles")

  baselineDenseParticleFiles = []
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/mode-00_sample-000_dense.particles")
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/mode-00_sample-001_dense.particles")
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/mode-00_sample-002_dense.particles")

  denseParticlesFiles = []
  denseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-000_dense.particles")
  denseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-001_dense.particles")
  denseParticlesFiles.append(os.environ["DATA"] + "/mode-00/data_mode-00_sample-002_dense.particles")

  baselineSparseParticles = ParticleSystem(baselineSparseParticleFiles)
  sparseParticles = ParticleSystem(sparseParticlesFiles)

  baselineDenseParticles = ParticleSystem(baselineDenseParticleFiles)
  denseParticles = ParticleSystem(denseParticlesFiles)

  success = True
  print("comparing sparse particles...")
  success = success and baselineSparseParticles.EvaluationCompare(sparseParticles)
  print("comparing dense particles...")
  success = success and baselineDenseParticles.EvaluationCompare(denseParticles)
  print("comparing dense mesh 1...")
  success = success and baselineDenseMesh1 == denseMesh1
  print("comparing dense mesh 2...")
  success = success and baselineDenseMesh2 == denseMesh2
  print("comparing dense mesh 3...")
  success = success and baselineDenseMesh3 == denseMesh3
  return success

success &= utils.test(pcamodesTestThinPlateSpline)

sys.exit(not success)

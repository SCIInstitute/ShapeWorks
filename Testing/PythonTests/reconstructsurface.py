import os
import sys
from shapeworks import *

success = True

def surfaceTest1():
  denseFile = os.environ["DATA"] + "/_dense.vtk"
  sparseFile = os.environ["DATA"] + "/_sparse.particles"
  goodPointsFile = os.environ["DATA"] + "/_goodPoints.txt"

  localParticles = []
  localParticles.append(os.environ["DATA"] + "/ellipsoid_00.local.particles")
  localParticles.append(os.environ["DATA"] + "/ellipsoid_01.local.particles")
  localParticles.append(os.environ["DATA"] + "/ellipsoid_02.local.particles")

  reconstructor = ReconstructSurface_RBFSSparseTransform(denseFile, sparseFile, goodPointsFile)
  reconstructor.setOutPrefix(os.environ["DATA"])
  reconstructor.surface(localParticles)

  baselineDenseMesh1 = Mesh(os.environ["DATA"] + "/RBFSSparseTransform/ellipsoid_00.dense.vtk")
  baselineDenseMesh2 = Mesh(os.environ["DATA"] + "/RBFSSparseTransform/ellipsoid_01.dense.vtk")
  baselineDenseMesh3 = Mesh(os.environ["DATA"] + "/RBFSSparseTransform/ellipsoid_02.dense.vtk")

  denseMesh1 = Mesh(os.environ["DATA"] + "/ellipsoid_00.local_dense.vtk")
  denseMesh2 = Mesh(os.environ["DATA"] + "/ellipsoid_01.local_dense.vtk")
  denseMesh3 = Mesh(os.environ["DATA"] + "/ellipsoid_02.local_dense.vtk")

  baselineSparseParticleFiles = []
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/ellipsoid_00.sparse.particles")
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/ellipsoid_01.sparse.particles")
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/ellipsoid_02.sparse.particles")

  sparseParticlesFiles = []
  sparseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_00.local_sparse.particles")
  sparseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_01.local_sparse.particles")
  sparseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_02.local_sparse.particles")

  baselineDenseParticleFiles = []
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/ellipsoid_00.dense.particles")
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/ellipsoid_01.dense.particles")
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/RBFSSparseTransform/ellipsoid_02.dense.particles")

  denseParticlesFiles = []
  denseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_00.local_dense.particles")
  denseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_01.local_dense.particles")
  denseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_02.local_dense.particles")

  baselineSparseParticles = ParticleSystem(baselineSparseParticleFiles)
  sparseParticles = ParticleSystem (sparseParticlesFiles)

  baselineDenseParticles = ParticleSystem(baselineDenseParticleFiles)
  denseParticles = ParticleSystem (denseParticlesFiles)

  return (baselineSparseParticles.EvaluationCompare(sparseParticles) and
          baselineDenseParticles.EvaluationCompare(denseParticles) and     
          baselineDenseMesh1 == denseMesh1 and baselineDenseMesh2 == denseMesh2 and baselineDenseMesh3 == denseMesh3)

success &= utils.test(surfaceTest1)

def surfaceTest2():
  denseFile = os.environ["DATA"] + "/_dense.vtk"
  sparseFile = os.environ["DATA"] + "/_sparse.particles"
  goodPointsFile = os.environ["DATA"] + "/_goodPoints.txt"

  localParticles = []
  localParticles.append(os.environ["DATA"] + "/ellipsoid_00.local.particles")
  localParticles.append(os.environ["DATA"] + "/ellipsoid_01.local.particles")
  localParticles.append(os.environ["DATA"] + "/ellipsoid_02.local.particles")

  reconstructor = ReconstructSurface_ThinPlateSplineTransform(denseFile, sparseFile, goodPointsFile)
  reconstructor.setOutPrefix(os.environ["DATA"])
  reconstructor.surface(localParticles)

  baselineDenseMesh1 = Mesh(os.environ["DATA"] + "/ThinPlateSplineTransform/ellipsoid_00.dense.vtk")
  baselineDenseMesh2 = Mesh(os.environ["DATA"] + "/ThinPlateSplineTransform/ellipsoid_01.dense.vtk")
  baselineDenseMesh3 = Mesh(os.environ["DATA"] + "/ThinPlateSplineTransform/ellipsoid_02.dense.vtk")

  denseMesh1 = Mesh(os.environ["DATA"] + "/ellipsoid_00.local_dense.vtk")
  denseMesh2 = Mesh(os.environ["DATA"] + "/ellipsoid_01.local_dense.vtk")
  denseMesh3 = Mesh(os.environ["DATA"] + "/ellipsoid_02.local_dense.vtk")

  baselineSparseParticleFiles = []
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/ellipsoid_00.sparse.particles")
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/ellipsoid_01.sparse.particles")
  baselineSparseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/ellipsoid_02.sparse.particles")

  sparseParticlesFiles = []
  sparseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_00.local_sparse.particles")
  sparseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_01.local_sparse.particles")
  sparseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_02.local_sparse.particles")

  baselineDenseParticleFiles = []
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/ellipsoid_00.dense.particles")
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/ellipsoid_01.dense.particles")
  baselineDenseParticleFiles.append(os.environ["DATA"] + "/ThinPlateSplineTransform/ellipsoid_02.dense.particles")

  denseParticlesFiles = []
  denseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_00.local_dense.particles")
  denseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_01.local_dense.particles")
  denseParticlesFiles.append(os.environ["DATA"] + "/ellipsoid_02.local_dense.particles")

  baselineSparseParticles = ParticleSystem(baselineSparseParticleFiles)
  sparseParticles = ParticleSystem (sparseParticlesFiles)

  baselineDenseParticles = ParticleSystem(baselineDenseParticleFiles)
  denseParticles = ParticleSystem (denseParticlesFiles)

  return (baselineSparseParticles.EvaluationCompare(sparseParticles) and
          baselineDenseParticles.EvaluationCompare(denseParticles) and     
          baselineDenseMesh1 == denseMesh1 and baselineDenseMesh2 == denseMesh2 and baselineDenseMesh3 == denseMesh3)

success &= utils.test(surfaceTest2)

sys.exit(not success)

import os
import sys
import numpy as np
from shapeworks import *

success = True

def warpTest():
  img = Image(os.environ["DATA"] + "/input.nrrd")
  transform = ImageUtils.createWarpTransform(os.environ["DATA"] + "/source.particles", os.environ["DATA"] + "/target.particles", 1)
  img.applyTransform(transform)

  compareImg = Image(os.environ["DATA"] + "/warp1.nrrd")

  return img.compare(compareImg)

success &= utils.test(warpTest)

def warpfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  transform = ImageUtils.createWarpTransform(os.environ["DATA"] + "/source.particles")

  compareImg = Image(os.environ["DATA"] + "/warpfail.nrrd")

  return img.compare(compareImg)

success &= utils.expectException(warpfailTest, TypeError)

def warpMesh():
  reference_mesh = Mesh(os.environ["DATA"] + "/mesh_warp/mesh_warp2.vtk")
  particles = ParticleSystem([os.environ["DATA"] + "/mesh_warp/mesh_warp2.particles"]).ShapeAsPointSet(0)
  
  warper = MeshWarper()
  warper.generateWarp(reference_mesh, particles)

  # Warp the mesh using the same particles as the reference --> warped mesh should be close to the reference mesh
  warped_mesh = warper.buildMesh(particles)
  dists_and_indexes = warped_mesh.distance(target=reference_mesh, method=Mesh.DistanceMethod.PointToCell)
  distances = np.array(dists_and_indexes[0])
  
  mean_error = np.mean(distances)
  print("Distances: mean={} max={}".format(mean_error, np.max(distances)))

  return mean_error < 0.3

success &= utils.test(warpMesh)

def warpMeshWithLandmarks():
  reference_mesh = Mesh(os.environ["DATA"] + "/mesh_warp/mesh_warp2.vtk")
  particles = ParticleSystem([os.environ["DATA"] + "/mesh_warp/mesh_warp2.particles"]).ShapeAsPointSet(0)
  
  warper = MeshWarper()
  warper.generateWarp(reference_mesh, particles, particles)  # with landmarks

  warped_mesh = warper.buildMesh(particles)
  warped_landmarks = warper.extractLandmarks(warped_mesh)

  max_error = np.max(np.abs(particles-warped_landmarks))
  print("Distances: max={}".format(max_error))

  return max_error < 1e-5

success &= utils.test(warpMeshWithLandmarks)

sys.exit(not success)

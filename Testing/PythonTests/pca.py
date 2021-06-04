import os
import sys
import shapeworks as sw
import glob
from copy import copy
from sklearn.decomposition import PCA
import numpy as np

def readData():
  # create ParticleSystem
  particleFilesDir = os.environ["DATA"] + "/ellipsoid_particles/"
  particleFilesList = glob.glob(particleFilesDir+ "*world.particles")
  ps = sw.ParticleSystem(particleFilesList)
  return ps

def sklearn_pca(particlesData):
  pca = PCA(svd_solver = 'auto')
  particlesDataMatrix = particlesData.Particles().T
  particlesData_mean = np.mean(particlesData.Particles().T, axis=0)
  particlesDataMatrix -= particlesData_mean 
  pcaLoadings = pca.fit_transform(particlesDataMatrix) 

  scree = pca.explained_variance_ratio_.cumsum()
  return scree

def shapeworks_pca(particlesData):
  # create ParticleShapeStatistics
  pss = sw.ParticleShapeStatistics()
  pss.PCA(particlesData)
  pss.principalComponentProjections()
  pcaLoadings = pss.pcaLoadings()

  scree = np.array(pss.percentVarByMode())
  return scree

particlesData = readData()
sklearn_scree = sklearn_pca(particlesData)
sw_scree = shapeworks_pca(particlesData)

if np.allclose(sklearn_scree, sw_scree) == False:
  sys.exit(1)

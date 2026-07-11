import os
import sys
import numpy as np
from shapeworks import *

success = True

def medianShapeTest():
  # the project's particle paths are relative, so load from its directory (as the CLI analyze test does)
  os.chdir(os.environ["DATA"] + "/shapeworksTests/analyze")
  project = Project()
  project.load("ellipsoid.swproj")
  analyze = Analyze(project)

  mean = np.asarray(analyze.get_mean_shape_points()).ravel()
  median = np.asarray(analyze.get_median_shape_points()).ravel()

  # the median shape is an actual sample: same size as the mean, finite, and (for this data) distinct from it
  if len(median) == 0 or len(median) != len(mean) or not np.all(np.isfinite(median)):
    return False
  if np.allclose(mean, median):
    return False

  # the reconstructed median mesh should be valid
  mesh = analyze.get_median_shape()
  return mesh.numPoints() > 0

success &= utils.test(medianShapeTest)

sys.exit(not success)

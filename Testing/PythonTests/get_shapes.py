import os
import sys
import numpy as np
from shapeworks import *

success = True

def getShapesTest():
  # the project's particle paths are relative, so load from its directory
  os.chdir(os.environ["DATA"] + "/shapeworksTests/analyze")
  project = Project()
  project.load("ellipsoid.swproj")
  analyze = Analyze(project)

  shapes = analyze.get_shapes()
  if len(shapes) != analyze.get_num_subjects():
    return False

  for shape in shapes:
    pts = np.asarray(shape.get_particles().get_combined_global_particles()).ravel()
    if len(pts) == 0 or not np.all(np.isfinite(pts)):
      return False
    # id / name accessors should work
    shape.get_id()
    shape.get_display_name()

  return True

success &= utils.test(getShapesTest)

sys.exit(not success)

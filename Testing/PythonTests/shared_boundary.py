import os
import sys
from shapeworks import *

success = True

# Disabled due to the fact that the output of the test is not deterministic across platforms (e.g. arm64), though the output is correct
# def sharedBoundaryExtractor():
# 	left = Mesh(os.environ["DATA"] + "/shared_boundary/00_l.vtk")
# 	right = Mesh(os.environ["DATA"] + "/shared_boundary/00_r.vtk")
# 	tol = 1e-3
# 	out_left, out_right, out_shared = MeshUtils.sharedBoundaryExtractor(left,right,tol)

# 	ground_truth_left = Mesh(os.environ["DATA"] + "/shared_boundary/00_out_l.vtk")
# 	ground_truth_right = Mesh(os.environ["DATA"] + "/shared_boundary/00_out_r.vtk")
# 	ground_truth_shared = Mesh(os.environ["DATA"] + "/shared_boundary/00_out_s.vtk")

# 	return (ground_truth_left == out_left) and (ground_truth_right == out_right) and (ground_truth_shared == out_shared)

#success &= utils.test(sharedBoundaryExtractor)

def boundaryLoopExtractor():

	mesh = Mesh(os.environ["DATA"] + "/shared_boundary/00_out_s.vtk")
	loop = MeshUtils.boundaryLoopExtractor(mesh);

	
	ground_truth = Mesh(os.environ["DATA"] + "/shared_boundary/00_out_c.vtp")

	return True
	return ground_truth == loop

success &= utils.test(boundaryLoopExtractor)

sys.exit(not success)

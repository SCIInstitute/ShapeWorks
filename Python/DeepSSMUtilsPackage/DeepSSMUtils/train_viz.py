import os
import numpy as np
import vtk
from DeepSSMUtils import eval_utils

# predicted_particles and true_particles are numpy array with dims: # in validation set, # of particles, 3 coordinates
def generate_validation_meshes(predicted_particles, true_particles, out_dir, mean_prefix):
	if not os.path.exists(out_dir):
		os.makedirs(out_dir)
	# get min, mean, and max errors
	mses = np.mean(np.mean((predicted_particles - true_particles)**2, axis=2), axis=1)
	mean_index = np.argmin(np.abs(mses - np.mean(mses)))
	indices = [np.argmin(mses), mean_index,  np.argmax(mses)]
	names = ["best", "mean", "worst"]
	particle_files = []
	vtk_text_files = []
	for i in range(3):
		# get particles
		pred = predicted_particles[indices[i]]
		# write particle file
		out_particle_file = out_dir + "/" + names[i] + "_validation.particles"
		np.savetxt(out_particle_file, pred)
		particle_files.append(out_particle_file)
		# get scalar field for error
		scalars = np.mean((pred - true_particles[indices[i]])**2, axis=1)
		# write points and scalar field to a text file
		text_filename = out_dir + "/" + names[i] + ".txt"
		text_file = open(text_filename, "w+")
		text_file.write("x	y	z	val\n")
		for index in range(pred.shape[0]):
			out = ''
			for pt_index in range(pred.shape[1]):
				out += str(pred[index][pt_index]) + "	"
			out += str(scalars[index]) + '\n'
			text_file.write(out)
		text_file.close()
		vtk_text_files.append(text_filename)
	# generate meshes without scalar fields
	meshes = eval_utils.get_mesh_from_particles(particle_files, out_dir, mean_prefix)
	# interpolate scalar field to entire mesh
	for index in range(3):
		interpolate(vtk_text_files[index], meshes[index], out_dir)
	return meshes


# Interpolate particle-wise scalars to entire mesh
def interpolate(text_file, mesh_file, out_dir):
	# read point
	points_reader = vtk.vtkDelimitedTextReader()
	points_reader.SetFileName(text_file)
	points_reader.DetectNumericColumnsOn()
	points_reader.SetFieldDelimiterCharacters('\t')
	points_reader.SetHaveHeaders(True)
	table_points = vtk.vtkTableToPolyData()
	table_points.SetInputConnection(points_reader.GetOutputPort())
	table_points.SetXColumn('x')
	table_points.SetYColumn('y')
	table_points.SetZColumn('z')
	table_points.Update()
	points = table_points.GetOutput()
	points.GetPointData().SetActiveScalars('val')
	range = points.GetPointData().GetScalars().GetRange()
	# read mesh
	reader = vtk.vtkPolyDataReader()
	reader.SetFileName(mesh_file)
	reader.ReadAllVectorsOn()
	reader.ReadAllScalarsOn()
	reader.Update()
	surface = reader.GetOutput()
	# Set Gaussian kernel
	gaussian_kernel = vtk.vtkGaussianKernel()
	gaussian_kernel.SetSharpness(2)
	gaussian_kernel.SetRadius(12)
	# Interpolate
	interpolator = vtk.vtkPointInterpolator()
	interpolator.SetInputData(surface)
	interpolator.SetSourceData(points)
	interpolator.SetKernel(gaussian_kernel)
	# Write
	writer = vtk.vtkDataSetWriter()
	writer.SetInputConnection(interpolator.GetOutputPort())
	writer.SetFileName(mesh_file)
	writer.Write()
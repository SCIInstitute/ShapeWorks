# Jadie Adams
import os
import json
import numpy as np
from numpy import matlib
import re
import math
import shapeworks as sw
import shutil
import vtk
import trimesh
import subprocess
import matplotlib.tri as mtri
import matplotlib as mpl
mpl.use('agg')
import matplotlib.pyplot as plt 
from scipy.stats import pearsonr
from vtk.util.numpy_support import numpy_to_vtk, numpy_to_vtkIdTypeArray, vtk_to_numpy

def make_dir(dirPath):
	if not os.path.exists(dirPath):
		os.makedirs(dirPath)

def get_names(test_names_file):
	f = open(test_names_file, 'r')
	test_names_string = f.read()
	f.close()
	test_names_string = test_names_string.replace("[","").replace("]","").replace("'","").replace(" ","")
	test_names = test_names_string.split(",")
	return test_names

def RRMSEparticles(true, pred):
	return np.sqrt(np.sum(np.square(true - pred), 1)/3)

def RRMSE(true, pred):
	return np.sqrt(np.sum(np.square(true - pred))/(1024*3))

def scatter_plot(x_values, x_label, y_values, y_label, out_dir, model_name):
	try:
		corr, _ = pearsonr(x_values, y_values)
	except:
		corr = 0.0
	print("Correlation between "+x_label+" and "+y_label+": "+str(round(corr,5)))
	plt.scatter(x_values, y_values)
	plt.rcParams.update({'figure.figsize':(7,4), 'figure.dpi':100})
	plt.title(model_name + '\nCorrelation coefficient: ' + str(corr))
	plt.xlabel(x_label)
	plt.ylabel(y_label)
	plt.savefig(out_dir+x_label.replace(" ", "_")+"__VS__"+y_label.replace(" ", "_")+".png")
	plt.clf()
	return corr

def medoid_viz(name, scalars, main_dir, viz_dir):
	medoid_mesh_path = main_dir + "medoid_dense.vtk"
	medoid_particle_path = main_dir + "medoid_sparse.particles"
	particles = np.loadtxt(medoid_particle_path)
	scalar_file = viz_dir + name + '.txt'
	text_file = open(scalar_file, "w+")
	text_file.write("x	y	z	val\n")
	for index in range(particles.shape[0]):
		out = ''
		for pt_index in range(particles.shape[1]):
			out += str(particles[index][pt_index]) + "	"
		out += str(scalars[index]) + '\n'
		text_file.write(out)
	text_file.close()
	interpolate(scalar_file, medoid_mesh_path, viz_dir + name + "_medoid_mesh.vtk")

def interpolate(text_file, mesh_file, out_file, label="val"):
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
	points.GetPointData().SetActiveScalars(label)
	# range = points.GetPointData().GetScalars().GetRange()
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
	writer.SetFileName(out_file)
	writer.Write()

def save_particles(particles, names, out_dir):
	make_dir(out_dir)
	out_files = []
	for i in range(particles.shape[0]):
		np.savetxt(out_dir + names[i] + ".particles", particles[i].reshape(1024,3))
		out_files.append(out_dir + names[i] + ".particles")
	return out_files

def get_closest_index(particles, train_particles):
	min_dist = np.inf
	for i in range(len(train_particles)):
		dist = np.sum((particles - train_particles[i])**2)
		if dist < min_dist:
			index = i
			min_dist = dist
	return index

def get_mesh_from_particles(particles, template_mesh, template_points, out_dir):
	particle_dir = os.path.dirname(particles) + '/'
	execCommand = ["shapeworks", 
			"warp-mesh", "--reference_mesh", template_mesh,
			"--reference_points", template_points,
			"--target_points" ]
	execCommand.append(particles)
	execCommand.append('--')
	subprocess.check_call(execCommand)

	if not os.path.exists(out_dir):
		os.makedirs(out_dir)

	infnm = particles.replace('.particles', '.vtk')
	outfnm = infnm.replace(particle_dir, out_dir)
	shutil.move(infnm, outfnm)

	return outfnm

def get_meshes_from_particles(particle_list, data_dir, mesh_dir):
	make_dir(mesh_dir)
	with open(data_dir + "data_info.json") as json_file: 	
		data_info = json.load(json_file)
	train_particles = []
	train_names = []
	for name in data_info:
		if data_info[name]['group'] == 'train':
			train_names.append(name)
			train_particles.append(np.loadtxt(data_dir+'all_particles/'+name+'_world.particles'))
	train_particles = np.array(train_particles)
	outmeshes = []
	for pt_file in particle_list:
		particles = np.loadtxt(pt_file)
		closest_index = get_closest_index(particles, train_particles)
		closest_name = train_names[closest_index]
		closest_mesh = data_dir + 'meshes/' + closest_name + ".vtk"
		closest_particles = data_dir + 'all_particles/' + closest_name + "_local.particles"
		try:
			out_mesh = get_mesh_from_particles(pt_file, closest_mesh, closest_particles, mesh_dir)
		except: 
			print(pt_file)
			out_mesh = get_mesh_from_particles(pt_file, data_dir + "mean_dense.vtk", data_dir + "mean_sparse.particles", mesh_dir)
		outmeshes.append(out_mesh)
	return outmeshes

def add_uncertainty_to_mesh(mesh_file, particles, uncertainty):
	scalar_file = 'temp.txt'
	text_file = open(scalar_file, "w+")
	text_file.write("x	y	z	uncertainty\n")
	for index in range(particles.shape[0]):
		out = ''
		for pt_index in range(particles.shape[1]):
			out += str(particles[index][pt_index]) + "	"
		out += str(np.mean(uncertainty[index])) + '\n'
		text_file.write(out)
	text_file.close()
	interpolate(scalar_file, mesh_file, mesh_file, "uncertainty")

def add_RRMSE_to_mesh(mesh_file, particles, rrmse, model_name):
	scalar_file = f'temp_{model_name}.txt'
	text_file = open(scalar_file, "w+")
	text_file.write("x	y	z	rrmse\n")
	for index in range(particles.shape[0]):
		out = ''
		for pt_index in range(particles.shape[1]):
			out += str(particles[index][pt_index]) + "	"
		out += str(rrmse[index]) + '\n'
		text_file.write(out)
	text_file.close()
	interpolate(scalar_file, mesh_file, mesh_file.replace("input", "RRMSE"), "RRMSE")

def RMSD(mesh_file):
	mesh = sw.Mesh(mesh_file)
	COM = np.array([mesh.centerOfMass()])
	reader = vtk.vtkPolyDataReader()
	reader.SetFileName(mesh_file)
	reader.ReadAllVectorsOn()
	reader.ReadAllScalarsOn()
	reader.Update()
	surface = reader.GetOutput()
	points = vtk_to_numpy(surface.GetPoints().GetData())
	k = points.shape[0]
	COM_array = np.repeat(COM, k, axis=0)
	RMSD = np.sqrt(np.sum((points - COM_array)**2)/k)
	return RMSD

def main(model_dir, data_dir, size, model_name, groups):
	print('\n' + model_name)
	# Results json
	results_json = model_dir + size + '/results.json'
	if os.path.exists(results_json):
		with open(results_json) as json_file: 
			results = json.load(json_file)
	else:
		results = {}
	groups_name = "&".join(groups)
	if model_name not in results:
		results[model_name] = {}
	if groups_name not in results[model_name]:
		results[model_name][groups_name] = {}
	# Viz dir
	viz_dir = model_dir + size + '/' + model_name + "/visualizations/" + groups_name + "/"
	make_dir(viz_dir)
	# Get data info
	with open(data_dir + "data_info.json") as json_file: 
		data_info = json.load(json_file)
	# Get predictions and names
	pred_dir = model_dir + size + '/' + model_name + "/predictions/"
	preds = np.load(pred_dir + groups[0] + "_particle_predictions.npy").squeeze()
	true_particles = np.load(pred_dir + groups[0] + "_true_particles.npy").squeeze()
	names = get_names(data_dir + groups[0] +  '_names.txt')
	for i in range(1, len(groups)):
		preds = np.concatenate((preds, np.load(pred_dir + groups[i] + "_particle_predictions.npy").squeeze()), axis=1) 
		true_particles = np.concatenate((true_particles, np.load(pred_dir + groups[i] + "_true_particles.npy").squeeze()), axis=0)
		names += get_names(data_dir + groups[i] +  '_names.txt')
	pred_particles = preds[0]
	pred_vars = preds[1]
	# Get MSE
	rrmse = []
	particle_rrmse = []
	particle_entropy = []
	sample_entropy = []
	for idx in range(len(pred_particles)):
		rrmse.append(RRMSE(true_particles[idx], pred_particles[idx]))
		particle_rrmse.append(RRMSEparticles(true_particles[idx], pred_particles[idx]))
		var = pred_vars[idx]
		particle_entropy.append(np.sum(var,1))
		sample_entropy.append(np.sum(var))
	sampleRRMSE = np.array(rrmse)
	np.save(model_dir + size + '/' + model_name + '/' + groups_name + '_sampleRRMSE.npy', sampleRRMSE)
	particleRRMSE = np.array(particle_rrmse)
	meanParticleRRMSE = np.mean(particleRRMSE, axis=0)
	stdParticleRRMSE = np.std(particleRRMSE, axis=0)
	particle_entropy = np.array(particle_entropy)
	particle_unc = np.product(np.sqrt(np.exp(pred_vars)), axis=2)
	meanParticleUnc = np.mean(particle_unc, axis=0)
	sample_entropy = np.array(sample_entropy)
	np.save(model_dir + size + '/' + model_name + '/' + groups_name + '_sampleEntropy.npy', sample_entropy)
	results[model_name][groups_name]["mean_sample_RRMSE"] = str(np.mean(sampleRRMSE))
	results[model_name][groups_name]["std_sample_RRMSE"] = str(np.std(sampleRRMSE))
	print("Sample-RRMSE: " + str(np.mean(sampleRRMSE).round(5)) + "+-" +str(np.std(sampleRRMSE).round(5)))
	# Get image outlier degree
	out = []
	for name in names:
		out.append(data_info[name]['image'])
	out = np.array(out)
	# Plots
	corr = scatter_plot(out, "Outlier Degree", sampleRRMSE, "Sample-RRMSE", viz_dir, model_name)
	results[model_name][groups_name]["Corr_out_sampleRRMSE"] = corr
	corr = scatter_plot(sample_entropy, "Sample-Uncertainty", out, "Outlier Degree", viz_dir, model_name)
	results[model_name][groups_name]["Corr_sampleUnc_out"] = corr
	corr = scatter_plot(sample_entropy, "Entropy", sampleRRMSE, "RRMSE", viz_dir, model_name)
	results[model_name][groups_name]["Corr_sampleUnc_sampleRMSE"] = corr
	corr = scatter_plot(particle_entropy.flatten(), "Particle-Entropy", particleRRMSE.flatten(), "Particle-RRMSE", viz_dir, model_name)
	results[model_name][groups_name]["Corr_particleEntropy_particleRMSE"] = corr
	corr = scatter_plot(particle_unc.flatten(), "Particle-Uncertainty", particleRRMSE.flatten(), "Particle-RRMSE", viz_dir, model_name)
	results[model_name][groups_name]["Corr_particleUnc_particleRMSE"] = corr
	# Mean mesh visualizations
	medoid_viz("meanRRMSE", meanParticleRRMSE, data_dir, viz_dir)
	medoid_viz("stdRRMSE", stdParticleRRMSE, data_dir, viz_dir)
	medoid_viz("Uncertainty", meanParticleUnc, data_dir, viz_dir)

	# Save pred particles to files
	pred_particle_files = save_particles(pred_particles, names, pred_dir + 'particles/')
	true_particle_files = save_particles(true_particles, names, pred_dir + 'true_particles/')
	pred_mesh_dir = viz_dir + "predicted_meshes/"
	get_meshes_from_particles(pred_particle_files, data_dir, pred_mesh_dir)
	# recon_true_mesh_dir = viz_dir + "reconstructed_true_meshes/"
	# get_meshes_from_particles(true_particle_files, data_dir, recon_true_mesh_dir)
	# Get distance to true mesh
	true_mesh_dist_dir = viz_dir + "surface_to_surface_distance/true_meshes/"
	make_dir(true_mesh_dist_dir)
	pred_mesh_dist_dir = viz_dir + "surface_to_surface_distance/predicted_meshes/"
	make_dir(pred_mesh_dist_dir)
	make_dir(pred_mesh_dir + "RRMSE/")
	s2s_dists = []
	for i in range(len(names)):
		mesh_name = names[i] + ".vtk"
		true_mesh_file = data_dir + 'meshes/' + mesh_name
		true_mesh = sw.Mesh(true_mesh_file)
		pred_mesh_file = pred_mesh_dir + mesh_name
		# add_RRMSE_to_mesh(pred_mesh_file, pred_particles[i], particleRRMSE[i])
		if "vanilla" not in model_name:
			add_uncertainty_to_mesh(pred_mesh_file, pred_particles[i], pred_vars[i])
		pred_mesh = sw.Mesh(pred_mesh_file)
		scale = RMSD(true_mesh_file)/RMSD(pred_mesh_file)
		pred_mesh.scale([scale,scale,scale])
		transform = pred_mesh.createTransform(true_mesh, sw.Mesh.AlignmentType.Rigid, 100)
		pred_mesh.applyTransform(transform)
		dist1 = np.mean(true_mesh.distance(pred_mesh).write(true_mesh_dist_dir + mesh_name).getField("distance"))
		dist2 = np.mean(pred_mesh.distance(true_mesh).write(pred_mesh_dist_dir + mesh_name).getField("distance"))
		s2s_dists.append((dist1 + dist2)/2)
	s2s_dists = np.array(s2s_dists)
	np.save(model_dir + size + '/' + model_name + '/' + groups_name + '_s2s_dists.npy', s2s_dists)
	print("Surf-to-surf distances: " + str(np.mean(s2s_dists).round(5)) + "+-" +str(np.std(s2s_dists).round(5)))
	results[model_name][groups_name]["mean_s2s_dist"] = str(np.mean(s2s_dists))
	results[model_name][groups_name]["std_s2s_dist"] = str(np.std(s2s_dists))
	
	# Write results
	with open(results_json, 'w') as outfile:
		json.dump(results, outfile, indent=2)

def get_SW_surf2suf_dist():
	recon_true_mesh_dir = '/home/sci/jadie/CHPC_VIB/FinalLA/vanilla_pca/visualizations/test&outlier_test/reconstructed_true_meshes/'
	out_dir = "/home/sci/jadie/CHPC_VIB/Final/vanilla_pca/visualizations/test&outlier_test/surface_to_surface_distance/recon_mesh/"
	make_dir(out_dir)
	true_mesh_dir = '/home/sci/jadie/Data/NewLA/meshes/'
	s2s_dists = []
	min_dist = np.inf
	for mesh in os.listdir(recon_true_mesh_dir):
		recon_mesh_file = recon_true_mesh_dir + mesh
		recon_mesh = sw.Mesh(recon_mesh_file)
		true_mesh_file = true_mesh_dir + mesh
		true_mesh = sw.Mesh(true_mesh_file)
		scale = RMSD(true_mesh_file)/RMSD(recon_mesh_file)
		recon_mesh.scale([scale,scale,scale])
		transform = recon_mesh.createTransform(true_mesh, sw.Mesh.AlignmentType.Rigid, 100)
		recon_mesh.applyTransform(transform)
		dist1 = np.mean(true_mesh.distance(recon_mesh).getField("distance"))
		dist2 = np.mean(recon_mesh.distance(true_mesh).write(out_dir+mesh).getField("distance"))
		dist = (dist1 + dist2)/2
		if dist < min_dist:
			min_dist = dist
			best_mesh = mesh
		s2s_dists.append(dist)
	s2s_dists = np.array(s2s_dists)
	print("Surf-to-surf distances: " + str(np.mean(s2s_dists).round(5)) + "+-" +str(np.std(s2s_dists).round(5)))
	print("Best mesh: " + mesh)

def box_plots_RRMSE(main_dir, model_names):
	errors = []
	for model_name in model_names:
		rrmse = np.load(main_dir + model_name + '/' + 'test&outlier_test_sampleRRMSE.npy')
		errors.append(rrmse)
	dif_fig = plt.figure(1, figsize=(9, 6))
	ax = dif_fig.add_subplot(111)
	bp = ax.boxplot(errors)		
	ax.set_xticklabels(model_names)
	plt.ylabel("RRMSE")
	dif_fig.savefig(main_dir + 'RRMSE_box_plot.png', bbox_inches='tight')
	plt.clf()

def	box_plots_unc(main_dir, model_names):
	for model_name in model_names:
		test = np.load(main_dir + model_name + '/' + 'test_sampleEntropy.npy')
		out_test = np.load(main_dir + model_name + '/' + 'outlier_test_sampleEntropy.npy')
		dif_fig = plt.figure(1, figsize=(7, 4))
		ax = dif_fig.add_subplot(111)
		bp = ax.boxplot([test, out_test], vert=False)		
		ax.set_yticklabels(["Inlier Test", "Outlier Test"])
		plt.xlabel("Entropy")
		dif_fig.savefig(main_dir + model_name + '/Entropy_box_plot.png', bbox_inches='tight')
		plt.clf()

def get_bmw(main_dir, model_name):
	rrmse = np.load(main_dir + model_name + '/' + 'test&outlier_test_s2s_dists.npy')
	names = get_names("data/test_names.txt")
	names += get_names('data/outlier_test_names.txt')
	print(np.mean(rrmse))
	input(rrmse)
	indices = np.argsort(rrmse)
	best_index = indices[0]
	print("Best: " + names[best_index])
	median_index = indices[len(rrmse)//2]
	print("Median: " + names[9])
	worst_index = indices[-2]
	print("Worst: " + names[worst_index])

if __name__ == '__main__':
	model_dir = 'models/'
	data_dir = 'data/'
	size = 'size_1/'
	model_names = ['vanilla_pca', 'uncertain_pca', 'ppca_offset', 'vib_linear', 'vib_nonlinear']
	model_names = ['vib_linear']
	groups = ["test"]
	for model_name in model_names:
		main(model_dir, data_dir, size, model_name, groups)

	get_bmw(model_dir+size+'/', 'vanilla_pca')

	get_SW_surf2suf_dist()

	box_plots_RRMSE(main_dir, model_names)
	model_names = ['vanilla_pca', 'uncertain_pca', 'ppca_offset', 'vib_linear', 'vib_nonlinear']
	box_plots_unc(model_dir + size, model_names)
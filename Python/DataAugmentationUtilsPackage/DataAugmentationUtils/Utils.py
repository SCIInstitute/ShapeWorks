import shapeworks as sw
import numpy as np
import os


def create_data_matrix(file_list):
	"""Reads data from files in given list and turns into one np matrix"""
	data_matrix = []
	for file in file_list:
		data_matrix.append(np.loadtxt(file))
	return np.array(data_matrix)


def pad_index(index):
	"""Pad index"""
	name = str(index)
	while len(name) < 4:
		name = "0" + name
	return name


def make_CSV(filename, orig_imgs, orig_points, orig_embeddings, gen_imgs, gen_points, gen_embeddings):
	"""Makes csv of real and augmented data with format:
	image path, particles path, PCA scores"""

	csv_out = open(filename, "w+")
	# write originals
	for orig_index in range(len(orig_imgs)):
		string = orig_imgs[orig_index] + "," + orig_points[orig_index]
		for score in orig_embeddings[orig_index]:
			string += "," + str(score)
		csv_out.write(string + "\n")
	# write generated
	for gen_index in range(len(gen_imgs)):
		string = gen_imgs[gen_index] + "," + gen_points[gen_index]
		for score in gen_embeddings[gen_index]:
			string += "," + str(score)
		csv_out.write(string + "\n")
	csv_out.close()


def generate_image(out_dir, gen_particles, base_image, base_particles):
	"""Use warp between particles to warp original image into a new image"""
	image_name = os.path.basename(gen_particles).replace(".particles",".nrrd")
	gen_image = out_dir + "Generated-Images/" + image_name
	img = sw.Image(base_image)
	transform = sw.ImageUtils.createWarpTransform(base_particles, gen_particles, 2)
	img.applyTransform(transform).write(gen_image)
	return gen_image


def get_homogeneous_coordinates(x):
	"""
	get the homogeneous coordinates
	by Wenzheng Tao
	---
	x:
		(n_coordinates, n_samples), input points
	---
	x_h:
		(n_coordinates + 1, n_samples), input sample homogeneous coordinates 
	"""
	x_h = np.concatenate([x, np.ones((1, x.shape[1]))], axis=0,)
	return x_h


def estimate_similar_transform(x, y):
	"""
	calculate the optimal parameters of similarity transformation from points x to points y
	x and y should have the same number of points
	minimizing sum_i(||y_i - (c * R @ x_i + t)||^2)
	
	using method in the following paper:
	http://web.stanford.edu/class/cs273/refs/umeyama.pdf
	by Wenzheng Tao
	---
	x:
		(n_coordinates, n_samples), first input points
	y:
		(n_coordinates, n_samples), second input points		
	---
	R:
		(n_coordinates, n_coordinates), optimum rotation matrix
	
	t:
		(n_coordinates, 1), optimal tranlation
	c:
		scalar, optimal scale
	"""
	m, n = x.shape
	assert (y.shape[1] == n), 'numbers of points in x and y should be the same, but now they are:' + ','.join([n, y.shape[1]])

	mu_x = np.mean(x, axis=1).reshape((m,1))
	mu_y = np.mean(y, axis=1).reshape((m,1))

	# center the points
	x_tilt = x - mu_x
	y_tilt = y - mu_y

	sigma_sq_x = np.sum((x_tilt) ** 2) / n
	sigma_sq_y = np.sum((y_tilt) ** 2) / n

	cov_xy = (y_tilt @ x_tilt.T) / n

	rank_cov_xy = np.linalg.matrix_rank(cov_xy)

	assert (rank_cov_xy >= m-1), 'rank (cov_xy) < m - 1, refer to Equations (40-43) in http://web.stanford.edu/class/cs273/refs/umeyama.pdf'

	U, D, VT = np.linalg.svd(cov_xy)
	D = np.diag(D)

	S = np.identity(m)
	if rank_cov_xy == m-1:
		if np.linalg.det(U) * np.linalg.det(VT) == -1:
			S[m, m] = -1		
	else:
		if np.linalg.det(cov_xy) < 0:
			S[m, m] = -1		


	R = U @ S @ VT
	c = (sigma_sq_x ** (-1)) * np.trace(D @ S)
	t = mu_y - c * R @ mu_x

	return R, t, c


def get_homogeneous_similar_transform(R, t, c):
	"""
	build the homogeneous similarity transformation matrix
	by Wenzheng Tao
	---
	R:
		(n_coordinates, n_coordinates), optimum rotation matrix
	
	t:
		(n_coordinates, 1), optimal tranlation
	c:
		scalar, optimal scale	
	---
	T:
		(n_coordinates+1, n_coordinates+1) homogeneous similarity transformation matrix
	"""
	m, m = R.shape
	T = np.zeros((m+1, m+1))
	T[0:m, 0:m] = c * R
	T[0:m, [3]] = t
	T[m,m] = 1
	return T


def estimate_homogeneous_similar_transform(x, y):
	"""
	estimate the optimal homogeneous similarity transformation from points x to points y
    by Wenzheng Tao
	---
	x:
		(n_coordinates, n_samples), first input points
	y:
		(n_coordinates, n_samples), second input points		
	---
	T:
		(n_coordinates+1, n_coordinates+1) optimal homogeneous similarity transformation from points x to points y
	"""
	R, t, c = estimate_similar_transform(x=x, y=y)
	T = get_homogeneous_similar_transform(R=R, t=t, c=c)
	return T
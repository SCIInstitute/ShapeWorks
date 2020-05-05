import os
import numpy as np
import nrrd #@TODO not in conda installs (python -m pip install pynrrd)
import random

# @TODO
# reads .particles file and returns numpy array
def readModel(filename):
	return np.zeros((100,3))

# gets numpy data from image dir and model dir
def getNumpyData(image_dir, model_dir, output_dir):
	print("Getting data in numpy format...")
	if not os.path.exists(output_dir):
		os.mkdir(output_dir)
	# get lists of file paths
	imglist = []
	for file in sorted(os.listdir(image_dir)):
		imglist.append(image_dir + file)
	modelList = []
	for file in sorted(os.listdir(model_dir)):
		modelList.append(model_dir + file)
	# get all data and targets
	data = []
	targets = []
	if len(imglist) != len(modelList):
		print("Error: number of images does not equal number of targets")
		exit()
	for index in range(len(imglist)):
		# print(str((index/len(imglist))*100)+'%')
		[img,f] = nrrd.read(imglist[index])
		print(np.array([img]).shape)
		data.append([img])
		model = readModel(modelList[index])
		targets.append(model)
	# shuffle
	c = list(zip(data, targets))
	random.shuffle(c)
	data, targets = zip(*c)
	data = np.array(data)
	targets = np.array(targets)
	# split into train, validation, and test
	num_train = int(len(imglist)*.8) # 80%
	num_val = int(len(imglist)*1) # 10%
	train_data = data[:num_train]
	np.save(output_dir + 'train_data.npy', train_data)
	val_data = data[num_train:num_train+num_val]
	np.save(output_dir + 'val_data.npy', val_data)
	test_data = data[num_train+num_val:]
	np.save(output_dir + 'test_data.npy', test_data)
	train_targets = targets[:num_train]
	np.save(output_dir + 'train_targets.npy', train_targets)
	val_targets = targets[num_train:num_train+num_val]
	np.save(output_dir + 'val_targets.npy', val_targets)
	test_targets = targets[num_train+num_val:]
	np.save(output_dir + 'test_targets.npy', test_targets)
	print("Numpy data saved.")

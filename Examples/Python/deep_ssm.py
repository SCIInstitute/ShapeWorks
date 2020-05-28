# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks DeepSSM
====================================================================
Jadie Adams
"""

from zipfile import ZipFile
import os
import sys
import csv
import argparse
import glob
import re
import numpy as np

from DeepSSMUtils import *

def Run_Pipeline(args):
	if args.tiny_test:
		print("\nRunning a tiny test.")

	# set parent dir
	parent_dir="TestDeepSSM/"
	if not os.path.exists(parent_dir):
		os.makedirs(parent_dir)
	datasetName = "femur"
	input_dir = parent_dir + datasetName + '/'

	# print("\nStep 1. Get Data")
	# # get zipfile if not there
	# filename = datasetName + ".zip"
	# if not os.path.exists(filename):
	# 	print("Can't find " + filename + " in the current directory.")
	# 	import DatasetUtils
	# 	DatasetUtils.downloadDataset(datasetName)

	# # extract the zipfile
	# print("Extracting data from " + filename + "...")
	# with ZipFile(filename, 'r') as zipObj:
	# 	zipObj.extractall(path=parent_dir)

	# Get lists of paths
	# image path list
	img_dir = input_dir + "groomed/images/"
	img_list = []
	for file in os.listdir(img_dir):
		img_list.append(img_dir + file)
	img_list = sorted(img_list)
	# get particles path list
	model_dir =  input_dir + "model/" 
	particle_list = []
	for file in os.listdir(model_dir):
		if "local" in file:
			particle_list.append(model_dir + file)
	particle_list = sorted(particle_list)

	if args.tiny_test:
		img_list = img_list[:4]
		particle_list = particle_list[:4]

	print("\n\n\nStep 2. Augment data\n")
	# num_samples is how many samples to generate 
	num_samples = 3950
	if args.tiny_test:
		num_samples = 16
	# PCA_var_cutoff dictates how many modes of variation are preserved for points and images (a smaller value will result in fewer PCA modes, setting to 1 will use all of them)
	PCA_var_cutoff = 0.97
	# If doResample is 1 it will resample the image to be half the size for faster training (default is 0)
	# If doPad is 1 it will add padding around the image in case the anatomy lies on the image boundary (default is 0)
	aug_data_csv, PCA_scores_path = dataAugment(parent_dir + "augmentation/", img_list, particle_list, num_samples, PCA_var_cutoff, doResample=0, doPad=0)

	# # Debug
	# aug_data_csv = '/home/sci/jadie/NewShapeworks/ShapeWorks/Examples/Python/TestDeepSSM/augmentation/totalData.csv'
	# PCA_scores_path = '/home/sci/jadie/NewShapeworks/ShapeWorks/Examples/Python/TestDeepSSM/augmentation/PCA-Info-Particles/'

	print("\n\n\nStep 3. Reformat Data for Pytorch\n")
	# Hyper-paramter batch_size for training
	# Higher batch size will help speed up training but uses more cuda memory
	# If you get a memory error try reducing the batch size
	batch_size = 1
	train_loader, val_loader, test_loader, test_sample_names = getTorchDataLoaders(parent_dir + 'TorchDataLoaders/', aug_data_csv, batch_size)

	# Debug
	# train_loader = '/home/sci/jadie/NewShapeworks/ShapeWorks/Examples/Python/TestDeepSSM/TorchDataLoaders/train'
	# val_loader = '/home/sci/jadie/NewShapeworks/ShapeWorks/Examples/Python/TestDeepSSM/TorchDataLoaders/validation'
	# test_loader = '/home/sci/jadie/NewShapeworks/ShapeWorks/Examples/Python/TestDeepSSM/TorchDataLoaders/test'
	# test_sample_names = ['Generated_sample_3835', 'Generated_sample_3666', 'Generated_sample_4143', 'Generated_sample_803', 'Generated_sample_2358', 'Generated_sample_4038', 'Generated_sample_2318', 'Generated_sample_4352', 'Generated_sample_3172', 'Generated_sample_2718', 'Generated_sample_287', 'Generated_sample_3781', 'Generated_sample_2805', 'Generated_sample_1241', 'Generated_sample_3833', 'Generated_sample_2716', 'Generated_sample_4962', 'Generated_sample_1517', 'Generated_sample_3352', 'Generated_sample_3295', 'Generated_sample_1308', 'n06_L', 'Generated_sample_337', 'Generated_sample_115', 'Generated_sample_548', 'Generated_sample_866', 'Generated_sample_4302', 'Generated_sample_3956', 'Generated_sample_4899', 'Generated_sample_4818', 'Generated_sample_4018', 'Generated_sample_3500', 'Generated_sample_732', 'Generated_sample_3140', 'Generated_sample_268', 'Generated_sample_2179', 'Generated_sample_3868', 'Generated_sample_4902', 'Generated_sample_4880', 'Generated_sample_4988', 'Generated_sample_3121', 'm08_L', 'Generated_sample_150', 'Generated_sample_905', 'Generated_sample_1321', 'Generated_sample_873', 'Generated_sample_3241', 'Generated_sample_3853', 'Generated_sample_4022', 'Generated_sample_3321', 'Generated_sample_2235', 'Generated_sample_1426', 'Generated_sample_4702', 'Generated_sample_4024', 'Generated_sample_2283', 'Generated_sample_4185', 'Generated_sample_1561', 'Generated_sample_2847', 'Generated_sample_2158', 'Generated_sample_2406', 'Generated_sample_3437', 'Generated_sample_4079', 'Generated_sample_277', 'Generated_sample_1165', 'Generated_sample_2490', 'Generated_sample_436', 'Generated_sample_438', 'Generated_sample_3009', 'Generated_sample_3663', 'Generated_sample_2191', 'Generated_sample_1582', 'Generated_sample_2205', 'Generated_sample_1060', 'Generated_sample_3399', 'Generated_sample_3553', 'Generated_sample_3409', 'Generated_sample_2062', 'Generated_sample_4832', 'Generated_sample_4890', 'Generated_sample_4534', 'Generated_sample_2146', 'Generated_sample_4264', 'Generated_sample_1654', 'Generated_sample_2316', 'Generated_sample_2763', 'Generated_sample_756', 'Generated_sample_2672', 'Generated_sample_1557', 'Generated_sample_4134', 'Generated_sample_4843', 'Generated_sample_1847', 'Generated_sample_3800', 'Generated_sample_3385', 'Generated_sample_2150', 'Generated_sample_1420', 'Generated_sample_3081', 'Generated_sample_2175', 'Generated_sample_4854', 'Generated_sample_1537', 'Generated_sample_4740', 'Generated_sample_2340', 'Generated_sample_4000', 'Generated_sample_4918', 'Generated_sample_1687', 'Generated_sample_4118', 'Generated_sample_2874', 'Generated_sample_924', 'Generated_sample_4476', 'Generated_sample_3642', 'Generated_sample_123', 'Generated_sample_3794', 'Generated_sample_4915', 'Generated_sample_3071', 'Generated_sample_1205', 'Generated_sample_2154', 'Generated_sample_3419', 'Generated_sample_1044', 'Generated_sample_306', 'Generated_sample_2992', 'Generated_sample_1179', 'Generated_sample_4144', 'Generated_sample_1567', 'Generated_sample_4747', 'Generated_sample_121', 'Generated_sample_3662', 'Generated_sample_1868', 'Generated_sample_3792', 'Generated_sample_2332', 'Generated_sample_2974', 'Generated_sample_2394', 'Generated_sample_2330', 'Generated_sample_779', 'Generated_sample_921', 'Generated_sample_3780', 'Generated_sample_2754', 'Generated_sample_1898', 'Generated_sample_400', 'Generated_sample_1090', 'Generated_sample_2133', 'Generated_sample_2077', 'Generated_sample_4967', 'Generated_sample_3986', 'Generated_sample_1816', 'Generated_sample_158', 'Generated_sample_4784', 'Generated_sample_4086', 'Generated_sample_1396', 'Generated_sample_4297', 'Generated_sample_4215', 'Generated_sample_3650', 'Generated_sample_1004', 'Generated_sample_2755', 'Generated_sample_2988', 'Generated_sample_3570', 'Generated_sample_4922', 'Generated_sample_4095', 'Generated_sample_2078', 'Generated_sample_349', 'Generated_sample_3318', 'Generated_sample_4519', 'Generated_sample_2113', 'Generated_sample_4739', 'Generated_sample_490', 'Generated_sample_2132', 'Generated_sample_4634', 'Generated_sample_3846', 'Generated_sample_340', 'm06_L', 'Generated_sample_964', 'Generated_sample_4444', 'Generated_sample_1054', 'Generated_sample_3974', 'Generated_sample_2413', 'Generated_sample_925', 'Generated_sample_558', 'Generated_sample_4109', 'Generated_sample_2997', 'Generated_sample_4482', 'Generated_sample_3751', 'Generated_sample_4179', 'Generated_sample_2668', 'Generated_sample_4012', 'Generated_sample_4707', 'Generated_sample_3701', 'Generated_sample_4742', 'Generated_sample_3739', 'Generated_sample_1156', 'Generated_sample_4731', 'Generated_sample_1692', 'Generated_sample_2578', 'Generated_sample_4668', 'Generated_sample_2869', 'Generated_sample_4730', 'Generated_sample_3001', 'Generated_sample_4600', 'Generated_sample_2675', 'Generated_sample_4693', 'Generated_sample_4217', 'Generated_sample_3550', 'Generated_sample_4367', 'Generated_sample_1142', 'Generated_sample_1172', 'Generated_sample_742', 'Generated_sample_1795', 'Generated_sample_2477', 'Generated_sample_4121', 'Generated_sample_2289', 'Generated_sample_3340', 'Generated_sample_2079', 'Generated_sample_2106', 'Generated_sample_2458', 'Generated_sample_4835', 'Generated_sample_4745', 'Generated_sample_625', 'Generated_sample_1035', 'Generated_sample_4636', 'Generated_sample_2480', 'Generated_sample_2855', 'Generated_sample_1221', 'Generated_sample_3988', 'Generated_sample_2951', 'Generated_sample_2420', 'Generated_sample_1560', 'Generated_sample_1283', 'Generated_sample_170', 'Generated_sample_1231', 'Generated_sample_4937', 'Generated_sample_140', 'Generated_sample_357', 'Generated_sample_589', 'Generated_sample_3177', 'Generated_sample_1152', 'Generated_sample_430', 'Generated_sample_4233', 'Generated_sample_2965', 'Generated_sample_2124', 'Generated_sample_3097', 'Generated_sample_1587', 'Generated_sample_4368', 'Generated_sample_138', 'Generated_sample_203', 'Generated_sample_2921', 'Generated_sample_508', 'Generated_sample_650', 'Generated_sample_3918', 'Generated_sample_16', 'Generated_sample_524', 'Generated_sample_3423', 'Generated_sample_4593', 'Generated_sample_1011', 'Generated_sample_3384', 'Generated_sample_1358', 'Generated_sample_769', 'Generated_sample_933', 'Generated_sample_2164', 'Generated_sample_4897', 'Generated_sample_3733', 'Generated_sample_674', 'Generated_sample_2030', 'Generated_sample_350', 'Generated_sample_1026', 'Generated_sample_3977', 'Generated_sample_2317', 'Generated_sample_4906', 'Generated_sample_3507', 'Generated_sample_1046', 'Generated_sample_2871', 'Generated_sample_3993', 'Generated_sample_3017', 'Generated_sample_1624', 'Generated_sample_4230', 'Generated_sample_3969', 'Generated_sample_3639', 'Generated_sample_106', 'Generated_sample_3782', 'Generated_sample_831', 'Generated_sample_4184', 'Generated_sample_2495', 'Generated_sample_171', 'Generated_sample_40', 'Generated_sample_3286', 'Generated_sample_2649', 'Generated_sample_931', 'Generated_sample_1927', 'Generated_sample_1260', 'Generated_sample_4487', 'Generated_sample_192', 'Generated_sample_4431', 'Generated_sample_4051', 'Generated_sample_2142', 'Generated_sample_68', 'Generated_sample_388', 'Generated_sample_2538', 'Generated_sample_2075', 'Generated_sample_2653', 'Generated_sample_2977', 'Generated_sample_2208', 'Generated_sample_1870', 'Generated_sample_223', 'Generated_sample_1125', 'Generated_sample_538', 'Generated_sample_1500', 'Generated_sample_1433', 'Generated_sample_4471', 'Generated_sample_4920', 'Generated_sample_1276', 'Generated_sample_175', 'Generated_sample_4334', 'Generated_sample_491', 'Generated_sample_4082', 'Generated_sample_1065', 'Generated_sample_3209', 'Generated_sample_882', 'Generated_sample_700', 'Generated_sample_3381', 'Generated_sample_1386', 'Generated_sample_3515', 'Generated_sample_2651', 'Generated_sample_4384', 'Generated_sample_3080', 'Generated_sample_4344', 'Generated_sample_4994', 'Generated_sample_1045', 'Generated_sample_375', 'Generated_sample_4453', 'Generated_sample_2662', 'Generated_sample_58', 'Generated_sample_2257', 'Generated_sample_3271', 'Generated_sample_181', 'Generated_sample_1215', 'Generated_sample_4258', 'Generated_sample_1223', 'Generated_sample_1676', 'Generated_sample_2750', 'Generated_sample_1704', 'Generated_sample_4828', 'Generated_sample_2592', 'Generated_sample_67', 'Generated_sample_2732', 'Generated_sample_1200', 'Generated_sample_2917', 'Generated_sample_1759', 'Generated_sample_36', 'Generated_sample_3444', 'Generated_sample_3546', 'Generated_sample_1938', 'Generated_sample_2908', 'Generated_sample_3527', 'Generated_sample_2634', 'Generated_sample_724', 'Generated_sample_3362', 'Generated_sample_270', 'Generated_sample_4313', 'Generated_sample_3971', 'Generated_sample_2076', 'Generated_sample_4824', 'Generated_sample_603', 'Generated_sample_2351', 'Generated_sample_2633', 'Generated_sample_405', 'Generated_sample_2905', 'Generated_sample_3572', 'Generated_sample_2523', 'Generated_sample_470', 'Generated_sample_3880', 'Generated_sample_3115', 'Generated_sample_1523', 'Generated_sample_3762', 'Generated_sample_4479', 'Generated_sample_221', 'Generated_sample_2001', 'Generated_sample_3078', 'Generated_sample_4475', 'Generated_sample_3945', 'Generated_sample_4423', 'Generated_sample_4654', 'Generated_sample_841', 'Generated_sample_1789', 'Generated_sample_1415', 'Generated_sample_275', 'Generated_sample_3688', 'Generated_sample_4148', 'Generated_sample_4565', 'Generated_sample_1732', 'Generated_sample_4186', 'Generated_sample_2882', 'Generated_sample_2195', 'Generated_sample_488', 'Generated_sample_809', 'Generated_sample_25', 'Generated_sample_469', 'Generated_sample_4882', 'Generated_sample_4605', 'Generated_sample_2984', 'Generated_sample_659', 'Generated_sample_3254', 'Generated_sample_4255', 'Generated_sample_3778', 'Generated_sample_1556', 'Generated_sample_2654', 'Generated_sample_62', 'Generated_sample_2948', 'Generated_sample_3519', 'Generated_sample_22', 'Generated_sample_890', 'Generated_sample_1985', 'Generated_sample_3803', 'Generated_sample_3593', 'Generated_sample_4428', 'n12_L', 'Generated_sample_3184', 'Generated_sample_3402', 'Generated_sample_684', 'Generated_sample_2118', 'Generated_sample_2275', 'Generated_sample_2726', 'Generated_sample_4240', 'Generated_sample_3731', 'Generated_sample_2720', 'Generated_sample_2761', 'Generated_sample_2237', 'Generated_sample_1343', 'Generated_sample_3320', 'Generated_sample_1527', 'Generated_sample_2023', 'Generated_sample_4985', 'Generated_sample_745', 'Generated_sample_4304', 'Generated_sample_4628', 'Generated_sample_597', 'Generated_sample_1740', 'Generated_sample_3692', 'Generated_sample_4252', 'Generated_sample_2919', 'Generated_sample_253', 'Generated_sample_3565', 'Generated_sample_1254', 'Generated_sample_3702', 'Generated_sample_2109', 'Generated_sample_1960', 'Generated_sample_1135', 'Generated_sample_3288', 'Generated_sample_4908', 'Generated_sample_235', 'Generated_sample_2405', 'Generated_sample_4063', 'Generated_sample_3156', 'Generated_sample_797', 'Generated_sample_4421', 'Generated_sample_4939', 'Generated_sample_1170', 'Generated_sample_4343', 'Generated_sample_1614', 'Generated_sample_2632', 'Generated_sample_793', 'Generated_sample_2362', 'Generated_sample_3653', 'Generated_sample_1158', 'm14_R', 'Generated_sample_3330', 'Generated_sample_4049', 'Generated_sample_4006', 'Generated_sample_2709', 'Generated_sample_763', 'Generated_sample_4225', 'Generated_sample_1633', 'Generated_sample_2788', 'Generated_sample_4559', 'Generated_sample_2751', 'Generated_sample_609', 'Generated_sample_4145', 'Generated_sample_3839', 'Generated_sample_370', 'Generated_sample_1495', 'Generated_sample_3982', 'Generated_sample_4146', 'Generated_sample_3057', 'Generated_sample_602', 'Generated_sample_3289', 'Generated_sample_773', 'Generated_sample_4374', 'Generated_sample_4062', 'Generated_sample_965', 'Generated_sample_97', 'm11_R', 'Generated_sample_473', 'Generated_sample_1952', 'Generated_sample_3579', 'Generated_sample_1166', 'Generated_sample_4544', 'Generated_sample_536', 'Generated_sample_2501', 'Generated_sample_1232', 'Generated_sample_4494', 'Generated_sample_2295', 'Generated_sample_2594', 'Generated_sample_497', 'Generated_sample_1497', 'Generated_sample_3506', 'Generated_sample_1548', 'Generated_sample_202', 'Generated_sample_2531', 'Generated_sample_4848', 'Generated_sample_3435']

	print("\n\n\nStep 4. Train model.\n")
	# Training parameters dict
	# val_freq sets how often too test on validation set and log
	# for example val_freq=1 is every epoch and val_freq=2 is every other
	parameters = {"epochs":10, "learning_rate":0.001, "val_freq":1}
	if args.tiny_test:
		parameters = {"epochs":5, "learning_rate":0.001, "val_freq":1}
	model_path = train(train_loader, val_loader, parameters, parent_dir)

	print("\n\n\nStep 5. Test DeepSSM\n") 
	# Test DeepSSM
	mr_error, rel_error = test(parent_dir + 'test/', model_path, test_loader, test_sample_names, PCA_scores_path)
	print("Average mean root MSE on test set:")
	print(mr_error)
	print("Average relative error on test set:")
	print(rel_error)
	print('Original and predicted particles saved at: ' + parent_dir + 'test/')
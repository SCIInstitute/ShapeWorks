# Jadie Adams
import os
import json
import numpy as np
from numpy import matlib
import torch
from torch.utils.data import DataLoader
from DeepSSMUtils import model
from shapeworks.utils import sw_message
from shapeworks.utils import sw_progress
from shapeworks.utils import sw_check_abort

'''
Network Test Function
	predicts the PCA scores using the trained networks
	returns the error measures and saves the predicted and poriginal particles for comparison
'''
def test(config_file):
	with open(config_file) as json_file: 
		parameters = json.load(json_file)
	model_dir = parameters["paths"]["out_dir"] + parameters["model_name"]+ '/'
	pred_dir = model_dir + 'predictions/'
	if not os.path.exists(pred_dir):
		os.makedirs(pred_dir)
	if parameters["use_best_model"]:
		model_path = model_dir + 'best_model.torch'
	else:
		model_path = model_dir + 'final_model.torch'
	if parameters["fine_tune"]["enabled"]:
		model_path_ft = model_path.replace(".torch", "_ft.torch")
	else:
		model_path_ft = model_path
	loader_dir = parameters["paths"]["loader_dir"]

	# load the loaders
	sw_message("Loading test data loader...")
	test_loader = torch.load(loader_dir + "test")
	print("Done.\n")
	# initalizations
	sw_message("Loading trained model...")
	model_pca = model.DeepSSMNet(config_file)
	model_pca.load_state_dict(torch.load(model_path))
	device = model_pca.device
	model_pca.to(device)
	model_pca.eval()
	model_ft = model.DeepSSMNet(config_file)
	model_ft.load_state_dict(torch.load(model_path_ft))
	model_ft.to(device)
	model_ft.eval()

	# Get test names 
	test_names_file = loader_dir + 'test_names.txt'
	f = open(test_names_file, 'r')
	test_names_string = f.read()
	f.close()
	test_names_string = test_names_string.replace("[","").replace("]","").replace("'","").replace(" ","")
	test_names = test_names_string.split(",")
	print("Done.\n")
	sw_message("Predicting for test images...")
	index = 0
	pred_scores = []
	predPath_ft = pred_dir + '/FT_Predictions'
	if not os.path.exists(predPath_ft):
		os.makedirs(predPath_ft)
	predPath_pca = pred_dir + '/PCA_Predictions'
	if not os.path.exists(predPath_pca):
		os.makedirs(predPath_pca)

	for img, pca, mdl in test_loader:
		if sw_check_abort():
			sw_message("Aborted")
			return
		sw_message(f"Predicting {index+1}/{len(test_loader)}")
		sw_progress((index+1) / len(test_loader))

		img = img.to(device)
		[pred, pred_mdl_pca] = model_pca(img)
		[pred, pred_mdl_ft] = model_ft(img)
		pred_scores.append(pred.cpu().data.numpy()[0])
		nmpred = predPath_pca + '/predicted_pca_' + test_names[index] + '.particles'
		np.savetxt(nmpred, pred_mdl_pca.squeeze().detach().cpu().numpy())
		nmpred = predPath_ft + '/predicted_ft_' + test_names[index] + '.particles'
		np.savetxt(nmpred, pred_mdl_ft.squeeze().detach().cpu().numpy())
		index += 1
		print("Predicted test " + str(index) + ".")
	print("Done.\n")
	return pred_dir

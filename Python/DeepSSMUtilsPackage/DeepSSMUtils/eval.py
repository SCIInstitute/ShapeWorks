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
def test(config_file, loader="test"):
	with open(config_file) as json_file: 
		parameters = json.load(json_file)
	model_dir = parameters["paths"]["out_dir"] + parameters["model_name"]+ '/'
	pred_dir = model_dir + loader + '_predictions/'
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
	sw_message("Loading "+ loader + " data loader...")
	test_loader = torch.load(loader_dir + loader)
	print("Done.\n")
	# initalizations
	sw_message("Loading trained model...")
	if parameters['tl_net']['enabled']:
		model_tl = model.DeepSSMNet_TLNet(config_file)
		model_tl.load_state_dict(torch.load(model_path))
		device = model_tl.device
		model_tl.to(device)
		model_tl.eval()
	else:
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
	test_names_file = loader_dir + loader + '_names.txt'
	f = open(test_names_file, 'r')
	test_names_string = f.read()
	f.close()
	test_names_string = test_names_string.replace("[","").replace("]","").replace("'","").replace(" ","")
	test_names = test_names_string.split(",")
	sw_message("Predicting for test images...")
	index = 0
	pred_scores = []

	if parameters['tl_net']['enabled']:
		predPath_tl = pred_dir + '/TL_Predictions'
		if not os.path.exists(predPath_tl):
			os.makedirs(predPath_tl)
	else:
		predPath_ft = pred_dir + 'FT_Predictions/'
		if not os.path.exists(predPath_ft):
			os.makedirs(predPath_ft)
		predPath_pca = pred_dir + 'PCA_Predictions/'
		if not os.path.exists(predPath_pca):
			os.makedirs(predPath_pca)
		predicted_particle_files = []
	for img, pca, mdl in test_loader:
		if sw_check_abort():
			sw_message("Aborted")
			return
		sw_message(f"Predicting {index+1}/{len(test_loader)}")
		sw_progress((index+1) / len(test_loader))
		img = img.to(device)
		if parameters['tl_net']['enabled']:
			[pred_tf, pred_mdl_tl] = model_tl(mdl, img)
			pred_scores.append(pred_tf.cpu().data.numpy())
			nmpred = predPath_tl + '/' + test_names[index] + '.particles'
			np.savetxt(nmpred, pred_mdl_tl.squeeze().detach().cpu().numpy())
			# save the AE latent space as shape descriptors
			nmpred = predPath_tl + '/' + test_names[index] + '.npy'
			np.save(nmpred, pred_tf.squeeze().detach().cpu().numpy())
		else:
			[pred, pred_mdl_pca] = model_pca(img)
			[pred, pred_mdl_ft] = model_ft(img)
			pred_scores.append(pred.cpu().data.numpy()[0])
			nmpred = predPath_pca + '/predicted_pca_' + test_names[index] + '.particles'
			np.savetxt(nmpred, pred_mdl_pca.squeeze().detach().cpu().numpy())
			nmpred = predPath_ft + '/predicted_ft_' + test_names[index] + '.particles'
			np.savetxt(nmpred, pred_mdl_ft.squeeze().detach().cpu().numpy())
		predicted_particle_files.append(nmpred)
		index += 1
	sw_message("Test completed.")
	return predicted_particle_files

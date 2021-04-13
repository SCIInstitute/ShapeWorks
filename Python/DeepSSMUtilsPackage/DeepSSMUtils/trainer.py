import os
import time
import json
import numpy as np
from numpy import matlib
import torch
from torch import nn
from torch.nn import functional as F
from torch.utils.data import DataLoader
from torch.optim.lr_scheduler import StepLR
from DeepSSMUtils import model
from DeepSSMUtils import losses

'''
Train helper
	Initilaizes all weights using initialization function specified by initf
'''
def weight_init(module, initf):
	def foo(m):
		classname = m.__class__.__name__.lower()
		if isinstance(m, module):
			initf(m.weight)
	return foo

'''
Train helper
	prints and logs values during training
'''
def log_print(logger, values):
	# print values
	if isinstance(values[0], str):
		print('	  '.join(values))
	else:
		format_values = ['%.5f' % i for i in values]
		print('	  '.join(format_values))
	# csv format
	string_values = [str(i) for i in values]
	log_string = ','.join(string_values)
	logger.write(log_string + '\n')


def train(config_file):
	with open(config_file) as json_file: 
		parameters = json.load(json_file)
	if parameters["loss"]["supervised_latent"]:
		supervised_train(config_file)

'''
Network training method
	defines, initializes, and trains the models
	logs training and validation errors
	saves the model and returns the path it is saved to
'''
def supervised_train(config_file):
	with open(config_file) as json_file: 
		parameters = json.load(json_file)
	model_dir = parameters['paths']['out_dir'] + parameters['model_name'] + '/'
	if not os.path.exists(model_dir):
		os.makedirs(model_dir)
	loader_dir = parameters['paths']['loader_dir']
	aug_dir = parameters['paths']['aug_dir']
	num_epochs = parameters['trainer']['epochs']
	learning_rate = parameters['trainer']['learning_rate']
	eval_freq = parameters['trainer']['val_freq']
	decay_lr = parameters['trainer']['decay_lr']
	fine_tune = parameters['fine_tune']['enabled']
	loss_func = method_to_call = getattr(losses, parameters["loss"]["function"])
	# load the loaders
	train_loader_path = loader_dir + "train"
	validation_loader_path = loader_dir + "validation"
	print("Loading data loaders...")
	train_loader = torch.load(train_loader_path)
	val_loader = torch.load(validation_loader_path)
	print("Done.")
	# initalizations
	num_pca = train_loader.dataset.pca_target[0].shape[0]
	num_corr = train_loader.dataset.mdl_target[0].shape[0]
	print("Defining net...")
	net = model.DeepSSMNet(config_file)
	device = net.device
	net.to(device)
	# intialize model weights
	net.apply(weight_init(module=nn.Conv2d, initf=nn.init.xavier_normal_))	
	net.apply(weight_init(module=nn.Linear, initf=nn.init.xavier_normal_))

	# these lines are for the fine tuning layer initialization
	whiten_mean = np.load(loader_dir + '/mean_PCA.npy')
	whiten_std = np.load(loader_dir + '/std_PCA.npy')
	orig_mean = np.loadtxt(aug_dir + '/PCA_Particle_Info/mean.particles')
	orig_pc = np.zeros([num_pca, num_corr*3])
	for i in range(num_pca):
		temp = np.loadtxt(aug_dir + '/PCA_Particle_Info/pcamode' + str(i) + '.particles')
		orig_pc[i, :] = temp.flatten()
	
	bias = torch.from_numpy(orig_mean.flatten()).to(device) # load the mean here
	weight = torch.from_numpy(orig_pc.T).to(device) # load the PCA vectors here
	net.decoder.fc_fine.bias.data.copy_(bias)
	net.decoder.fc_fine.weight.data.copy_(weight)

	# define optimizer
	# for the initial steps set the gradient of the final layer to be zero
	for param in net.decoder.fc_fine.parameters():
		param.requires_grad = False
	
	train_params = net.parameters()
	opt = torch.optim.Adam(train_params, learning_rate)
	opt.zero_grad()
	scheduler = StepLR(opt, step_size=1, gamma=0.99)
	print("Done.")
	# train
	print("Beginning training on device = " + device + '\n')
	logger = open(model_dir + "train_log.csv", "w+")
	log_print(logger, ["Epoch", "LR", "Train_Err", "Train_Rel_Err", "Val_Err", "Val_Rel_Err", "Sec"])
	t0 = time.time()
	best_val_rel_error = np.Inf
	for e in range(1, num_epochs + 1):
		torch.cuda.empty_cache()
		# train
		net.train()
		train_losses = []
		train_rel_losses = []
		for img, pca, mdl in train_loader:
			opt.zero_grad()
			img = img.to(device)
			pca = pca.to(device)
			[pred_pca, pred_mdl] = net(img)
			loss = loss_func(pred_pca, pca)
			loss.backward()
			opt.step()
			train_losses.append(loss.item())
			train_rel_loss = loss_func(pred_pca, pca) / loss_func(pred_pca*0, pca)
			train_rel_losses.append(train_rel_loss.item())
		# test validation
		if ((e % eval_freq) == 0 or e == 1):
			net.eval()
			val_losses = []
			val_rel_losses = []
			for img, pca, mdl in val_loader:
				opt.zero_grad()
				img = img.to(device)
				pca = pca.to(device)
				[pred_pca, pred_mdl] = net(img)
				v_loss = loss_func(pred_pca, pca)
				val_losses.append(v_loss.item())
				val_rel_loss = loss_func(pred_pca, pca) / loss_func(pred_pca*0, pca)
				val_rel_losses.append(val_rel_loss.item())
			# log
			train_mr_MSE = np.mean(np.sqrt(train_losses))
			val_mr_MSE = np.mean(np.sqrt(val_losses))
			train_rel_err = np.mean(train_rel_losses)
			val_rel_err =  np.mean(val_rel_losses)
			log_print(logger, [e, scheduler.get_lr()[0], train_mr_MSE, train_rel_err, val_mr_MSE, val_rel_err, time.time()-t0])
			# save
			if val_rel_err < best_val_rel_error:
				best_val_rel_error = val_rel_err
				best_epoch = e
				torch.save(net.state_dict(), os.path.join(model_dir, 'best_net.torch'))
			t0 = time.time()
		if decay_lr:
			scheduler.step()
	logger.close()

	torch.save(net.state_dict(), os.path.join(model_dir, 'final_net.torch'))
	parameters['best_model_epochs'] = best_epoch
	with open(config_file, "w") as json_file:
		json.dump(parameters, json_file, indent=2) 
	print("Training complete, model saved. Best model after epoch " + str(best_epoch))

	# now commence the fine tuning model if present on best model
	if fine_tune:
		print("Beginning fine tuning training step on device = ", device)
		net = model.DeepSSMNet(config_file)
		if parameters["use_best_model"]:
			model_path = os.path.join(model_dir, 'best_net.torch')
		else:
			model_path = os.path.join(model_dir, 'final_net.torch')
		net.load_state_dict(torch.load(model_path))
		net.to(device)
		logger = open(model_dir + "train_log_ft.csv", "w+")
		log_print(logger, ["Epoch", "Train_Err_mdl", "Train_Rel_Err_mdl", "Val_Err_mdl", "Val_Rel_Err_mdl", "Sec"])
		ft_epochs = parameters['fine_tune']['epochs']
		learning_rate = parameters['fine_tune']['learning_rate']
		eval_freq = parameters['fine_tune']['val_freq']
		decay_lr = parameters['fine_tune']['decay_lr']
		loss_func = method_to_call = getattr(losses, parameters['fine_tune']["loss"])
		# free the last params
		for param in net.decoder.fc_fine.parameters():
			param.requires_grad=True
		# train on the corr loss
		best_ft_val_rel_error = np.Inf
		for e in range(1, ft_epochs + 1):
			torch.cuda.empty_cache()
			# train
			net.train()
			train_losses = []
			train_rel_losses = []
			for img, pca, mdl in train_loader:
				opt.zero_grad()
				img = img.to(device)
				mdl = mdl.to(device)
				[pred_pca, pred_mdl] = net(img)
				loss = torch.mean((pred_mdl - mdl)**2)
				loss.backward()
				opt.step()
				train_losses.append(loss.item())
				train_rel_loss = F.mse_loss(pred_mdl, mdl) / F.mse_loss(pred_mdl*0, mdl)
				train_rel_losses.append(train_rel_loss.item())
			# test validation
			if ((e % eval_freq) == 0 or e == 1):
				net.eval()
				val_losses = []
				val_rel_losses = []
				for img, pca, mdl in val_loader:
					opt.zero_grad()
					img = img.to(device)
					mdl = mdl.to(device)
					[pred_pca, pred_mdl] = net(img)
					v_loss = torch.mean((pred_mdl - mdl)**2)
					val_losses.append(v_loss.item())
					val_rel_loss = (F.mse_loss(pred_mdl, mdl) / F.mse_loss(pred_mdl*0, mdl)).item()
					val_rel_losses.append(val_rel_loss)
					if val_rel_loss < best_ft_val_rel_error:
						best_ft_val_rel_error = val_rel_loss
						best_ft_epoch = e
						torch.save(net.state_dict(), os.path.join(model_dir, 'best_model_ft.torch'))
				# log
				train_mr_MSE = np.mean(np.sqrt(train_losses))
				val_mr_MSE = np.mean(np.sqrt(val_losses))
				train_rel_err = np.mean(train_rel_losses)
				val_rel_err =  np.mean(val_rel_losses)
				log_print(logger, [e, train_mr_MSE, train_rel_err, val_mr_MSE, val_rel_err, time.time()-t0])
				t0 = time.time()
		
		logger.close()
		torch.save(net.state_dict(), os.path.join(model_dir, 'final_model_ft.torch'))
		parameters['best_ft_model_epochs'] = best_ft_epoch
		with open(config_file, "w") as json_file:
			json.dump(parameters, json_file, indent=2) 
		print("Fine tuning complete, model saved. Best model after epoch " + str(best_ft_epoch))

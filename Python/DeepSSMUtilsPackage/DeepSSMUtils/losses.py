import torch 

def MSE(predicted, ground_truth):
	return torch.mean((predicted - ground_truth)**2)

def focal_loss(predicted, ground_truth, a = 0, c = 0):
	'''
	Emplioys general formulation of focal loss, the pred and groundtruth can either be PCA/latent space or the correspondences directly.
	a = 0 will results in standard MSE loss
	'''
	l = torch.abs(predicted - ground_truth)
	out = l**2 / (1 + torch.exp(a*(c - l)))
	return torch.mean(out)

def focal_rel_loss(predicted, ground_truth, mean, a = 0, c = 0):
	return focal_loss(predicted, ground_truth, a, c) / focal_loss(mean, ground_truth, a, c)
import torch 

def MSE(predicted, ground_truth):
	return torch.mean((predicted - ground_truth)**2)

def Focal(predicted, ground_truth, a = 1.32, c = 10):
	'''
	Employs general formulation of focal loss, the pred and groundtruth can either be PCA/latent space or the correspondences directly.
	a = 0 results in standard MSE loss
	'''
	l = torch.abs(predicted - ground_truth)
	out = l**2 / (1 + torch.exp(a*(c - l)))
	return torch.mean(out)

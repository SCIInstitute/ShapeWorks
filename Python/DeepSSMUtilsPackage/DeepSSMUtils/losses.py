import torch 

def MSE(predicted, ground_truth):
	return torch.mean((predicted - ground_truth)**2)
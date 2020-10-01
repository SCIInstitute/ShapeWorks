from DeepSSMUtils import TorchLoaders
from DeepSSMUtils import DeepSSM
from DeepSSMUtils import Analyze
import torch

def getTrainValLoaders(loader_dir, aug_data_csv, batch_size=1, down_sample=False):
	testPytorch()
	TorchLoaders.getTrainValLoaders(loader_dir, aug_data_csv, batch_size, down_sample)

def getTestLoader(loader_dir, test_img_list, down_sample=False):
	testPytorch()
	TorchLoaders.getTestLoader(loader_dir, test_img_list, down_sample)

def trainDeepSSM(loader_dir, parameters, parent_dir):
	testPytorch()
	model_path = DeepSSM.train(loader_dir, parameters, parent_dir)
	return model_path

def testDeepSSM(out_dir, model_path, loader_dir, PCA_scores_path, num_PCA):
	testPytorch()
	DeepSSM.test(out_dir, model_path, loader_dir, PCA_scores_path, num_PCA)
	return

def analyzeResults(out_dir, DT_dir, prediction_dir, mean_prefix):
	avg_distance = Analyze.getDistance(out_dir, DT_dir, prediction_dir, mean_prefix)
	return avg_distance

def testPytorch():
	if torch.cuda.is_available():
		print("Running on GPU.")
	else:
		print("\n\n\n********************* WARNING ****************************")
		print("Pytorch is running on your CPU!")
		print("This will be very slow. If your machine has a GPU,") 
		print("please reinstall Pytorch to your shapeworks conda ")
		print("environment with the correct CUDA version.")
		print("**********************************************************")
		exit()
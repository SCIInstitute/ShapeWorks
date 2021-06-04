from DeepSSMUtils import trainer 
from DeepSSMUtils import loaders
from DeepSSMUtils import eval
from DeepSSMUtils import eval_utils
import torch

def getTrainValLoaders(loader_dir, aug_data_csv, batch_size=1, down_factor=1, down_dir=None):
	testPytorch()
	loaders.get_train_val_loaders(loader_dir, aug_data_csv, batch_size, down_factor, down_dir)

def getTestLoader(loader_dir, test_img_list, down_factor=1, down_dir=None):
	testPytorch()
	loaders.get_test_loader(loader_dir, test_img_list, down_factor, down_dir)

def trainDeepSSM(config_file):
	testPytorch()
	trainer.train(config_file)
	return

def testDeepSSM(config_file):
	testPytorch()
	eval.test(config_file)
	return

def analyzeResults(out_dir, DT_dir, prediction_dir, mean_prefix):
	avg_distance = eval_utils.get_distance_meshes(out_dir, DT_dir, prediction_dir, mean_prefix)
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
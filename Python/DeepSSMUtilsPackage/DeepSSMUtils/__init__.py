from DeepSSMUtils import TorchLoaders
from DeepSSMUtils import DeepSSM

def getTrainValLoaders(loader_dir, aug_data_csv, batch_size=1, down_sample=False):
	TorchLoaders.getTrainValLoaders(loader_dir, aug_data_csv, batch_size, down_sample)

def getTestLoader(loader_dir, test_img_list, test_particle_list, down_sample=False):
	TorchLoaders.getTestLoader(loader_dir, test_img_list, test_particle_list, down_sample)

def trainDeepSSM(loader_dir, parameters, parent_dir):
	model_path = DeepSSM.train(loader_dir, parameters, parent_dir)
	return model_path

def testDeepSSM(out_dir, model_path, loader_dir, PCA_scores_path):
	mr_error, rel_error = DeepSSM.test(out_dir, model_path, loader_dir, PCA_scores_path)
	return mr_error, rel_error
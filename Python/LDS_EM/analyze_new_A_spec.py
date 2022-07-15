import SetCudaDevice
SetCudaDevice.CUDA_DEVICE = 'cuda:3'
from LDS_Analysis import LDS_Analysis

l_val = 64
em_iters = 10
scale_val=True
model_names = ['model_A', 'model_B']


print(f'----Building model - {model_names[0]} for L = {l_val}, having {em_iters} EM iterations and scaling = {scale_val} ------')

analyze_specificity_A = LDS_Analysis(model_name=model_names[0], em_iterations=em_iters, T=25, d=3, M=256, L=l_val, 
                                    scaling=scale_val, baseline_model=True, expt_type='SPECIFICITY')


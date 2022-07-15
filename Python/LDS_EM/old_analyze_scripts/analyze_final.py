from LDS_Analysis import LDS_Analysis
l_val = 256
em_iters = 5
scale_val=True
model_name = 'model_A'
# model_name = 'model_B'
print(f'----Building model - {model_name} for L = {l_val}, having {em_iters} EM iterations and scaling = {scale_val} ------')
analyze_cross_entropy_model = LDS_Analysis(model_name=model_name, em_iterations=em_iters, T=25, d=3, M=256, L=l_val, scaling=scale_val, baseline_model=True)
# analyze_cross_entropy_model = LDS_Analysis(model_name=model_name, em_iterations=em_iters, T=25, d=3, M=256, L=l_val, scaling=scale_val, baseline_model=False)

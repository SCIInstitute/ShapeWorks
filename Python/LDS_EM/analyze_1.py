from LDS_Analysis import LDS_Analysis
# l_val = 256
l_val = 256
em_iters = 80
scale_val=True
print(f'----Building model for L = {l_val}, having {em_iters} EM iterations and scaling = {scale_val} ------')
analyze_cross_entropy_model = LDS_Analysis(model_name='pre_post_model_2500', em_iterations=em_iters, T=25, d=3, M=256, L=l_val, scaling=scale_val)

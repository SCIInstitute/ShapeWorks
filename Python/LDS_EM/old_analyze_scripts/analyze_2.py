from LDS_Analysis import LDS_Analysis
l_val = 96
em_iters = 6
print(f'----Building model for L = {l_val}, having {em_iters} EM iterations ------')
analyze_cross_entropy_model = LDS_Analysis(model_name='pre_post_model_2500_64', em_iterations=em_iters, T=25, d=3, M=64, L=l_val, scaling=False)

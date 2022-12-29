
import os
import json
import glob
import logging

import numpy as np
import numpy.linalg as la
import torch
import torch.optim as optim
from torch.distributions.multivariate_normal import MultivariateNormal as MVN

import shapeworks as sw

from utils import *
from nflib.flows import (
    MAF, NormalizingFlowModel,
)
params = DictMap(json.load(open('params_file.json')))
DEVICE = torch.device(params.device if torch.cuda.is_available() else 'cpu')
torch.cuda.empty_cache()

def get_prior_distribution(params):
    M = params.num_corr
    N = params.num_samples
    d = params.dimension
    prior_mean = torch.zeros(d * M)
    prior_cov = torch.eye(d * M)
    if params.prior_type == 'diagonal':
        prior_cov[0, 0] = 1.0
        for i in range(1, 2):
            prior_cov[i, i] = 0.5 * prior_cov[i-1, i-1]
        for i in range(2, d*M):
            prior_cov[i, i] = 0.0001

    prior_mean = prior_mean.to(DEVICE)
    prior_cov = prior_cov.to(DEVICE)
    prior = MVN(prior_mean, prior_cov)
    return prior_mean, prior_cov, prior

def get_flow_model(flow_model_type, d, M):
    if flow_model_type == 'MAF':
        return [MAF(dim=d*M, parity=i%2, device=DEVICE, nh=128) for i in range(10)]

class NonLinearSSM:
    def __init__(self, init_particles_dir, flow_model_type='MAF') -> None:
        self.prior_mean, self.prior_cov, self.prior = get_prior_distribution(params)
        M = params.num_corr
        N = params.num_samples
        d = params.dimension
        self.flow_model_type = flow_model_type
        self.init_particles = load_shape_matrix(init_particles_dir, N, M, d)
        self.init_particles_data = DatasetSuperShapes(self.init_particles)
        self.flows = get_flow_model(flow_model_type. d, M)
        self.norm_model = NormalizingFlowModel(self.prior, self.flows, DEVICE)
        pass
    
    def optimize_shape_model(self):
        # TODO: add shapeworks optimize callbacks and complete the pipeline 
        pass

    def train_invertible_network(self):
        num_iter = params.num_initial_iterations
        optimizer = optim.Adam(self.model.parameters(), 
                            lr=params.learning_rate,
                            weight_decay=params.weight_decay)
        self.norm_model.train()



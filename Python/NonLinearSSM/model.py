
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
    MAF, AffineHalfFlow, NormalizingFlowModel,
)
params = DictMap(json.load(open('params_file.json')))
M = params.num_corr
N = params.num_samples
d = params.dimension

global DEVICE
DEVICE = torch.device(params.device if torch.cuda.is_available() else 'cpu')

def get_prior_distribution(params):
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
    if flow_model_type == 'RealNVP':
        return [AffineHalfFlow(dim=d*M, parity=i%2, device=DEVICE, nh=128) for i in range(10)]
        

class InvertibleNetwork:
    def __init__(self, init_particles_dir, flow_model_type='RealNVP') -> None:
        torch.cuda.empty_cache()
        self.prior_mean, self.prior_cov, self.prior = get_prior_distribution(params)
        self.flow_model_type = flow_model_type
        self.init_particles = load_shape_matrix(init_particles_dir, N, M, d)
        self.init_particles_data = Dataset(self.init_particles)
        self.flows = get_flow_model(flow_model_type, d, M)
        self.norm_model = NormalizingFlowModel(self.prior, self.flows, DEVICE)
        
    
    def optimize_shape_model(self):
        # TODO: add shapeworks optimize callbacks and complete the pipeline 
        pass

    def train_invertible_network(self):
        logging.info('*********** Training Invertible Network *****************')
        num_iter = params.num_initial_iterations # burn in iterations
        batch_size = params.batch_size
        optimizer = optim.Adam(self.model.parameters(), 
                            lr=params.learning_rate,
                            weight_decay=params.weight_decay)
        self.norm_model.train()
        particles = self.init_particles
        for k in range(num_iter):
            x = particles.sample(batch_size).to(DEVICE) # 
            x = x.reshape(batch_size, -1)
            z_all, prior_logprob, log_det = self.norm_model(x.float())
            logprob = prior_logprob + log_det
            loss = -torch.sum(logprob)
            self.norm_model.zero_grad()
            loss.backward()
            optimizer.step()
            if k % 1000 == 0:
                logging.info(f"Epoch = {k} | Loss = {loss.item()}")

        logging.info(f'********** Training Done **********')
        
        # Save model
        







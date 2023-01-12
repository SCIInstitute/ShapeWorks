
import os
import json
import glob
import logging

import numpy as np
import numpy.linalg as la
import torch
import torch.optim as optim
from torch.distributions.multivariate_normal import MultivariateNormal as MVN


from utils import *
from nflib.flows import (
   ModifiedMAF, MAF, AffineHalfFlow, NormalizingFlowModel,
)
params = DictMap(json.load(open('params_file.json')))
M = params.num_corr
N = params.num_samples
d = params.dimension
MODEL_SAVE_DIR = params.model_save_dir

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

def update_prior_util(z0_particles):
    pass

def get_flow_model(flow_model_type, d, M):
    if flow_model_type == 'MAF':
        # return [MAF(dim=d*M, parity=i%2, device=DEVICE, nh=128) for i in range(10)]
        return [ModifiedMAF(dim=d*M, parity=i%2, device=DEVICE, nh=128) for i in range(10)]
    if flow_model_type == 'RealNVP':
        return [AffineHalfFlow(dim=d*M, parity=i%2, device=DEVICE, nh=128) for i in range(10)]
        

class InvertibleNetwork:
    def __init__(self, flow_model_type='MAF') -> None:
        torch.cuda.empty_cache()
        self.prior_mean, self.prior_cov, self.prior = get_prior_distribution(params)
        self.flow_model_type = flow_model_type
        self.flows = get_flow_model(flow_model_type, d, M)
    
    def initialize_particles(self, init_particles_dir):
        self.init_particles = load_shape_matrix(init_particles_dir, N, M, d)
        self.init_particles_data = Dataset(self.init_particles)

    def update_prior_distribution(self, z0_particles):
        self.prior_mean, self.prior_cov, self.prior = update_prior_util(z0_particles)
    
    def get_prior_distribution(self):
        return self.prior_mean, self.prior_cov, self.prior

    def initialize_model(self):
        self.norm_model = NormalizingFlowModel(self.prior, self.flows, DEVICE)
        self.optimizer = optim.Adam(self.norm_model.parameters(), 
                            lr=params.learning_rate,
                            weight_decay=params.weight_decay)

    def train_invertible_network_from_scratch(self):
        print('*********** Training Invertible Network | Burn-in Particles *****************')
        num_iter = params.num_initial_iterations # burn in iterations
        batch_size = params.batch_size
        optimizer = self.optimizer
        self.norm_model.train()
        particles = self.init_particles_data
        loss = None
        for k in range(num_iter):
            x = particles.sample(batch_size).to(DEVICE)
            x = x.reshape(batch_size, -1)
            z_all, prior_logprob, log_det = self.norm_model(x.float())
            logprob = prior_logprob + log_det
            loss = -torch.sum(logprob)
            self.norm_model.zero_grad()
            loss.backward()
            optimizer.step()
            if k % 1000 == 0:
                print(f"Epoch = {k} | Loss = {loss.item()}")
        print(f'********** Training Done **********')
        # Save last checkpoint
        checkpoint_path = f'{MODEL_SAVE_DIR}/model_{self.flow_model_type}.pt'
        torch.save({
            'epoch': num_iter,
            'model_state_dict': self.norm_model.state_dict(),
            'optimizer_state_dict': self.optimizer.state_dict(),
            'loss': loss.item(),
            }, checkpoint_path)
        print('burn in model saved')

    def train_invertible_network_from_last_checkpoint(self):
        print('*********** Training Invertible Network from Last Checkpoint *****************')
        num_iter = params.train_iterations # burn in iterations
        batch_size = params.batch_size
        optimizer = self.optimizer

        checkpoint_path = f'{MODEL_SAVE_DIR}/model_{self.flow_model_type}.pt'
        checkpoint = torch.load(checkpoint_path)
        self.norm_model.load_state_dict(checkpoint['model_state_dict'])
        optimizer.load_state_dict(checkpoint['optimizer_state_dict'])
        epoch = checkpoint['epoch']
        loss = checkpoint['loss']
        self.norm_model.train()
        particles = self.init_particles_data
        loss = None
        for k in range(num_iter):
            x = particles.sample(batch_size).to(DEVICE)
            x = x.reshape(batch_size, -1)
            z_all, prior_logprob, log_det = self.norm_model(x.float())
            logprob = prior_logprob + log_det
            loss = -torch.sum(logprob)
            self.norm_model.zero_grad()
            loss.backward()
            optimizer.step()
            if k % 1000 == 0:
                print(f"Epoch = {k} | Loss = {loss.item()}")
        print(f'********** Training Done **********')
        # Save last checkpoint
        checkpoint_path = f'{MODEL_SAVE_DIR}/model.pt'
        torch.save({
            'epoch': num_iter,
            'model_state_dict': self.norm_model.state_dict(),
            'optimizer_state_dict': self.optimizer.state_dict(),
            'loss': loss.item(),
            }, checkpoint_path)

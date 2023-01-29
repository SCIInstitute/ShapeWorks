
import os
import json
import glob
import logging
import time
import numpy as np
import numpy.linalg as la
import torch
import torch.optim as optim
from torch.distributions.multivariate_normal import MultivariateNormal as MVN

from utils import *
from nflib.flows import (
   ModifiedMAF, MAF, AffineHalfFlow, NormalizingFlowModel,
)


def get_prior_distribution(params, device):
    d = params.dimension
    M = params.num_particles
    prior_mean = torch.zeros(d * M)
    prior_cov = torch.eye(d * M)
    if params.prior_type == 'diagonal':
        prior_cov[0, 0] = 1.0
        for i in range(1, 2):
            prior_cov[i, i] = 0.5 * prior_cov[i-1, i-1]
        for i in range(2, d*M):
            prior_cov[i, i] = 0.0001

    prior_mean = prior_mean.to(device)
    prior_cov = prior_cov.to(device)
    prior = MVN(prior_mean, prior_cov)
    return prior_mean, prior_cov, prior

def update_prior_util(mean, cov, device, modify_cov=True):
    cov = torch.from_numpy(cov).float().to(device)
    if modify_cov:
        S= torch.linalg.svdvals(cov)
        cov = torch.diag(S)
    mean = torch.from_numpy(mean).float().to(device)
    mean = mean.squeeze()
    prior = MVN(mean, cov)
    return mean, cov, prior

def get_flow_model(flow_model_type, d, M, device):
    if flow_model_type == 'MAF':
        return [ModifiedMAF(dim=d*M, parity=i%2, device=device, nh=128) for i in range(10)]
    elif flow_model_type == 'RealNVP':
        return [AffineHalfFlow(dim=d*M, parity=i%2, device=device, nh=128) for i in range(10)]
    elif flow_model_type == 'OriginalMAF':
        return [MAF(dim=d*M, parity=i%2, device=device, nh=128) for i in range(10)]
        

class InvertibleNetwork:
    def __init__(self, params) -> None:
        torch.cuda.empty_cache()
        self.params = params
        self.device = torch.device(params.device if torch.cuda.is_available() else 'cpu')
        self.prior_mean, self.prior_cov, self.prior = get_prior_distribution(params, device=self.device)
        self.flow_model_type = params.flow_model_type
        self.flows = get_flow_model(self.flow_model_type, d=params.dimension, M=params.num_particles, device=self.device)
        self.model_save_dir = f'{self.params.working_dir}/pytorch-models/'
        if not os.path.exists(self.model_save_dir):
            os.makedirs(self.model_save_dir)
    
    def initialize_particles(self, init_particles_dir, particle_system='warped'):
        self.init_particles = load_shape_matrix(init_particles_dir, N=self.params.num_samples,
                                                M=self.params.num_particles, particle_system=particle_system)
        self.init_particles_data = Dataset(self.init_particles)

    def update_prior_distribution(self, mean, cov):
        self.prior_mean, self.prior_cov, self.prior = update_prior_util(mean, cov, device=self.device)
    
    def get_prior_distribution(self):
        return self.prior_mean, self.prior_cov, self.prior

    def initialize_model(self):
        self.norm_model = NormalizingFlowModel(self.prior, self.flows, self.device)
        self.optimizer = optim.Adam(self.norm_model.parameters(), 
                            lr=self.params.learning_rate,
                            weight_decay=self.params.weight_decay)

    def train_invertible_network_from_scratch(self):
        print('*********** Training Invertible Network | Burn-in Particles *****************')
        num_iter = self.params.num_initial_iterations # burn in iterations
        batch_size = self.params.batch_size
        optimizer = self.optimizer
        self.norm_model.train()
        particles = self.init_particles_data
        loss = None
        for k in range(num_iter):
            st_time = time.time()
            x = particles.sample(batch_size).to(self.device)
            x = x.reshape(batch_size, -1)
            z_all, prior_logprob, log_det = self.norm_model(x.float())
            logprob = prior_logprob + log_det
            loss = -torch.sum(logprob)
            self.norm_model.zero_grad()
            loss.backward()
            optimizer.step()
            if k % 100 == 0:
                end_time = time.time()
                print(f"Epoch = {k} | Loss = {loss.item()} | Time = {end_time-st_time:.4f}")
        print(f'********** Training Done **********')
        # Save last checkpoint
        checkpoint_path = f'{self.model_save_dir}/model_{self.flow_model_type}.pt'
        torch.save({
            'epoch': num_iter,
            'model_state_dict': self.norm_model.state_dict(),
            'optimizer_state_dict': self.optimizer.state_dict(),
            'loss': loss.item(),
            }, checkpoint_path)
        print('burn in model saved')

    def train_invertible_network_from_last_checkpoint(self):
        print('*********** Training Invertible Network from Last Checkpoint *****************')
        num_iter = self.params.train_iterations # burn in iterations
        batch_size = self.params.batch_size
        optimizer = self.optimizer

        checkpoint_path = f'{self.model_save_dir}/model_{self.flow_model_type}.pt'
        checkpoint = torch.load(checkpoint_path)
        self.norm_model.load_state_dict(checkpoint['model_state_dict'], map_location=self.device)
        optimizer.load_state_dict(checkpoint['optimizer_state_dict'])
        epoch = checkpoint['epoch']
        loss = checkpoint['loss']
        self.norm_model.train()
        particles = self.init_particles_data
        loss = None
        for k in range(num_iter):
            x = particles.sample(batch_size).to(self.device)
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
        checkpoint_path = f'{self.model_save_dir}/model.pt'
        torch.save({
            'epoch': num_iter,
            'model_state_dict': self.norm_model.state_dict(),
            'optimizer_state_dict': self.optimizer.state_dict(),
            'loss': loss.item(),
            }, checkpoint_path)
        print(f'Model saved')

    def serialize_model(self):
        print('*********** Serializing to TorchScript Module *****************')
        optimizer = self.optimizer
        checkpoint_path = f'{self.model_save_dir}/model_{self.flow_model_type}.pt'
        checkpoint = torch.load(checkpoint_path, map_location=self.device)
        self.norm_model.load_state_dict(checkpoint['model_state_dict'])
        optimizer.load_state_dict(checkpoint['optimizer_state_dict'])
        self.norm_model.eval()

        sm = torch.jit.script(self.norm_model)
        sm.save(f"{self.model_save_dir}/serialized_model.pt")

        print(f'******************** Serialized Module saved ************************')


import os
import time
import math
import numpy as np
import numpy.linalg as la
import torch
import torch.optim as optim

from data import fetch_dataloaders
from NormalizingFlowsLib.maf import (MAF)

def train(model, dataloader, optimizer, epoch, args):

    for i, data in enumerate(dataloader):
        model.train()

        # check if labeled dataset
        if len(data) == 1:
            x, y = data[0], None
        else:
            x, y = data
            y = y.to(args.device)
        x = x.view(x.shape[0], -1).to(args.device)

        # loss = - model.log_prob(x, y if args.cond_label_size else None).mean(0)
        loss = - model.log_prob(x).mean(0)


        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        if i % args.log_interval == 0:
            print('epoch {:3d} / {}, step {:4d} / {}; loss {:.4f}'.format(
                epoch, args.start_epoch + args.n_epochs, i, len(dataloader), loss.item()))

@torch.no_grad()
def evaluate(model, dataloader, epoch, args):
    model.eval()

    # conditional model
    if args.cond_label_size is not None:
        logprior = torch.tensor(1 / args.cond_label_size).log().to(args.device)
        loglike = [[] for _ in range(args.cond_label_size)]

        for i in range(args.cond_label_size):
            # make one-hot labels
            labels = torch.zeros(args.batch_size, args.cond_label_size).to(args.device)
            labels[:,i] = 1

            for x, y in dataloader:
                x = x.view(x.shape[0], -1).to(args.device)
                loglike[i].append(model.log_prob(x, labels))

            loglike[i] = torch.cat(loglike[i], dim=0)   # cat along data dim under this label
        loglike = torch.stack(loglike, dim=1)           # cat all data along label dim

        # log p(x) = log ∑_y p(x,y) = log ∑_y p(x|y)p(y)
        # assume uniform prior      = log p(y) ∑_y p(x|y) = log p(y) + log ∑_y p(x|y)
        logprobs = logprior + loglike.logsumexp(dim=1)
        # TODO -- measure accuracy as argmax of the loglike

    # unconditional model
    else:
        logprobs = []
        for data in dataloader:
            x = data[0].view(data[0].shape[0], -1).to(args.device)
            logprobs.append(model.log_prob(x))
        logprobs = torch.cat(logprobs, dim=0).to(args.device)

    logprob_mean, logprob_std = logprobs.mean(0), 2 * logprobs.var(0).sqrt() / math.sqrt(len(dataloader.dataset))
    output = 'Evaluate ' + (epoch != None)*'(epoch {}) -- '.format(epoch) + 'logp(x) = {:.3f} +/- {:.3f}'.format(logprob_mean, logprob_std)
    print(output)
    print(output, file=open(args.results_file, 'a'))
    return logprob_mean, logprob_std

def train_and_evaluate(model, train_loader, test_loader, optimizer, args):
    best_eval_logprob = float('-inf')

    for i in range(args.start_epoch, args.start_epoch + args.n_epochs):
        train(model, train_loader, optimizer, i, args)
        eval_logprob, _ = evaluate(model, test_loader, i, args)

        # save training checkpoint
        torch.save({'epoch': i,
                    'model_state': model.state_dict(),
                    'optimizer_state': optimizer.state_dict()},
                    os.path.join(args.output_dir, 'model_checkpoint.pt'))
        # save model only
        torch.save(model.state_dict(), os.path.join(args.output_dir, 'model_state.pt'))

        # save best state
        if eval_logprob > best_eval_logprob:
            best_eval_logprob = eval_logprob
            torch.save({'epoch': i,
                        'model_state': model.state_dict(),
                        'optimizer_state': optimizer.state_dict()},
                        os.path.join(args.output_dir, 'best_model_checkpoint.pt'))

class InvertibleNetwork:
    def __init__(self, params) -> None:
        torch.cuda.empty_cache()
        self.params = params
        self.device = params.device
        self.model_type = params.model

    def initialize_particles(self, init_particles_dir, particle_system='world'):
        self.train_dataloader, self.test_dataloader = fetch_dataloaders(init_particles_dir, batch_size=self.params.batch_size, device=self.params.device,
                                                 particle_system=particle_system, seed=self.params.seed, train_test_split=self.params.train_test_split)
        self.params.input_size = self.train_dataloader.dataset.input_size
        self.params.input_dims = self.train_dataloader.dataset.input_dims
        self.params.cond_label_size = None

    def initialize_model(self):
        args = self.params
        self.model = MAF(args.n_blocks, args.input_size, args.hidden_size, args.n_hidden, args.cond_label_size,
                    args.activation_fn, args.input_order, batch_norm=not args.no_batch_norm)
        self.model = self.model.to(args.device)
        self.optimizer = torch.optim.Adam(self.model.parameters(), lr=args.lr, weight_decay=1e-6)
    
    def train_model_from_scratch(self):
        train_and_evaluate(self.model, self.train_dataloader, self.test_dataloader, self.optimizer, self.params)
    
    def serialize_model(self):
        print('*********** Serializing to TorchScript Module *****************')

        checkpoint_path = f'{self.params.output_dir}/model_checkpoint.pt'
        state = torch.load(checkpoint_path, map_location=self.device)
        print(state.keys())
        self.model.load_state_dict(state['model_state'])
        self.optimizer.load_state_dict(state['optimizer_state'])
        self.model.eval()

        sm = torch.jit.script(self.model)
        sm.save(f"{self.params.output_dir}/serialized_model.pt")

        print(f'******************** Serialized Module saved ************************')

import argparse
import json
import os
import random
import subprocess

import numpy as np

import ray
from ray import tune
from ray.tune import Trainable, run, sample_from
from ray.tune.schedulers import AsyncHyperBandScheduler, ASHAScheduler
from ray.tune.automl import ContinuousSpace, DiscreteSpace, SearchSpace

class MyTrainableClass(Trainable):
    """Example agent whose learning curve is a random sigmoid.

    The dummy hyperparameters "width" and "height" determine the slope and
    maximum loss value reached.
    """

    def _setup(self, config):
        self.x, self.y = config['x'], config['y']
        self.timestep = 0

    def _train(self):
        self.timestep += 1
        x, y = self.x, self.y
        v = x**2 + y**2

        # Here we use `episode_reward_mean`, but you can also report other
        # objectives such as loss or accuracy.
        return {"loss": -abs(v)}

    def _save(self, checkpoint_dir):
        path = os.path.join(checkpoint_dir, "checkpoint")
        with open(path, "w") as f:
            f.write(json.dumps({"timestep": self.timestep}))
        return path

    def _restore(self, checkpoint_path):
        with open(checkpoint_path) as f:
            self.timestep = json.loads(f.read())["timestep"]


if __name__ == '__main__':
    ray.init(num_cpus=6, num_gpus=0)
    search_space = {
        "x": tune.uniform(-1, 1),
        "y": tune.uniform(-1, 1)
    }
    analysis = tune.run(MyTrainableClass,
                        config=search_space,
                        num_samples=100,
                        scheduler=ASHAScheduler(metric="loss", mode="min"))

    print('best: {}', analysis.get_best_config('loss'))
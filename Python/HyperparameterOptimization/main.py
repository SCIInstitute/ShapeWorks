import argparse
import json
import os
import random

import numpy as np

import ray
from ray import tune
from ray.tune import Trainable, run, sample_from
from ray.tune.schedulers import AsyncHyperBandScheduler
from ray.tune.automl import ContinuousSpace, DiscreteSpace, SearchSpace

class MyTrainableClass(Trainable):
    """Example agent whose learning curve is a random sigmoid.

    The dummy hyperparameters "width" and "height" determine the slope and
    maximum reward value reached.
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
        return {"episode_reward_mean": -abs(v)}

    def _save(self, checkpoint_dir):
        path = os.path.join(checkpoint_dir, "checkpoint")
        with open(path, "w") as f:
            f.write(json.dumps({"timestep": self.timestep}))
        return path

    def _restore(self, checkpoint_path):
        with open(checkpoint_path) as f:
            self.timestep = json.loads(f.read())["timestep"]


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--smoke-test", action="store_true", help="Finish quickly for testing")
    parser.add_argument(
        "--ray-address",
        help="Address of Ray cluster for seamless distributed execution.")
    args, _ = parser.parse_known_args()
    ray.init(address=args.ray_address)

    # asynchronous hyperband early stopping, configured with
    # `episode_reward_mean` as the
    # objective and `training_iteration` as the time unit,
    # which is automatically filled by Tune.
    ahb = AsyncHyperBandScheduler(
        time_attr="training_iteration",
        metric="episode_reward_mean",
        mode="max",
        grace_period=5,
        max_t=100)

    analysis = run(MyTrainableClass,
        name="asynchyperband_test",
        scheduler=ahb,
        stop={"training_iteration": 1},
        num_samples=500,
        resources_per_trial={
            "cpu": 1,
            "gpu": 0
        },
        config={
            'x': tune.uniform(-1, 1),
            'y': tune.uniform(-1, 1)
        })
    print("Best config: ", analysis.get_best_config(metric="episode_reward_mean"))
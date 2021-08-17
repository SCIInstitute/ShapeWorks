import json


def prepare_config_file(config_file, model_name, embedded_dim, out_dir, loader_dir, aug_dir, epochs, learning_rate,
                        decay_lr, fine_tune):
    """Prepares a JSON config file"""
    # Define model parameters
    model_parameters = {
        "model_name": model_name,
        "num_latent_dim": int(embedded_dim),
        "paths": {
            "out_dir": out_dir,
            "loader_dir": loader_dir,
            "aug_dir": aug_dir
        },
        "encoder": {
            "deterministic": True
        },
        "decoder": {
            "deterministic": True,
            "linear": True
        },
        "loss": {
            "function": "MSE",
            "supervised_latent": True,
        },
        "trainer": {
            "epochs": epochs,
            "learning_rate": learning_rate,
            "decay_lr": decay_lr,
            "val_freq": 1
        },
        "fine_tune": {
            "enabled": fine_tune,
            "loss": "MSE",
            "epochs": 2,
            "learning_rate": 0.001,
            "decay_lr": True,
            "val_freq": 1
        },
        "use_best_model": True
    }
    # Save config file
    with open(config_file, "w") as outfile:
        json.dump(model_parameters, outfile, indent=2)

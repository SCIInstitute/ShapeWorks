import json


def prepare_config_file(config_file, model_name, embedded_dim, out_dir, loader_dir, aug_dir, epochs, learning_rate,
                        decay_lr, fine_tune, fine_tune_epochs, fine_tune_learning_rate, loss="MSE", tl_net=False,
                        tl_ae_epochs=100, tl_tf_epochs=100, tl_joint_epochs=25, tl_alpha=1, tl_a_ae=10, tl_c_ae=1.32,
                        tl_a_lat=10, tl_c_lat=6.3):
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
            "function": loss,
            "supervised_latent": True,
        },

        "trainer": {
            "epochs": epochs,
            "learning_rate": learning_rate,
            "decay_lr": {
                "enabled": decay_lr,
                "type": "CosineAnnealing",
                "parameters": {
                    "T_max": 10,
                    "eta_min": 0,
                    "step_size": 1,
                    "gamma": 0.99
                }
            },
            "val_freq": 1
        },
        "fine_tune": {
            "enabled": fine_tune,
            "loss": loss,
            "epochs": fine_tune_epochs,
            "learning_rate": fine_tune_learning_rate,
            "decay_lr": True,
            "val_freq": 1
        },
        "use_best_model": True,
        "tl_net": {
            "enabled": tl_net,
            "ae_epochs": tl_ae_epochs,
            "tf_epochs": tl_tf_epochs,
            "joint_epochs": tl_joint_epochs,
            "alpha": tl_alpha,
            "a_ae": tl_a_ae,
            "c_ae": tl_c_ae,
            "a_lat": tl_a_lat,
            "c_lat": tl_c_lat
        }
    }
    # Save config file
    with open(config_file, "w") as outfile:
        json.dump(model_parameters, outfile, indent=2)

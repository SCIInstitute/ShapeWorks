import json

import DeepSSMUtils
import shapeworks as sw


def prepare_project_config_file(project):
    params = sw.DeepSSMParameters(project)

    config_file = "deepssm/configuration.json"

    out_dir = "deepssm/"
    aug_dir = out_dir + "augmentation/"
    loader_dir = out_dir + "torch_loaders/"

    epochs = params.get_training_epochs()
    learning_rate = params.get_training_learning_rate()
    decay_lr = params.get_training_decay_learning_rate()
    fine_tune = params.get_training_fine_tuning()
    fine_tune_epochs = params.get_training_fine_tuning_epochs()
    fine_tune_learning_rate = params.get_training_fine_tuning_learning_rate()
    num_dims = params.get_training_num_dims()

    DeepSSMUtils.prepare_config_file(config_file, "model", num_dims, out_dir, loader_dir, aug_dir, epochs,
                                     learning_rate, decay_lr,
                                     fine_tune, fine_tune_epochs, fine_tune_learning_rate, params.get_loss_function(),
                                     params.get_tl_net_enabled(), params.get_tl_net_ae_epochs(),
                                     params.get_tl_net_tf_epochs(),
                                     params.get_tl_net_joint_epochs(), params.get_tl_net_alpha(),
                                     params.get_tl_net_a_ae(),
                                     params.get_tl_net_c_ae(), params.get_tl_net_a_lat(), params.get_tl_net_c_lat())


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

import os
import time
import json
import numpy as np
from numpy import matlib
import matplotlib.pyplot as plt
import torch
from torch import nn
from torch.nn import functional as F
from torch.utils.data import DataLoader
from torch.optim.lr_scheduler import StepLR
from DeepSSMUtils import model
from DeepSSMUtils import losses
from DeepSSMUtils import train_viz
from DeepSSMUtils import loaders
import DeepSSMUtils
from shapeworks.utils import *

'''
Train helper
	Initializes all weights using initialization function specified by initf
'''


def weight_init(module, initf):
    def foo(m):
        classname = m.__class__.__name__.lower()
        if isinstance(m, module):
            initf(m.weight)

    return foo


'''
Train helper
	prints and logs values during training
'''


def log_print(logger, values):
    # for each value, if it is a string, print it, otherwise print it with 5 decimal places
    for i in range(len(values)):
        if isinstance(values[i], str):
            print(values[i], end='	  ')
        else:
            print('%.5f' % values[i], end='	  ')

    # csv format
    string_values = [str(i) for i in values]
    log_string = ','.join(string_values)
    logger.write(log_string + '\n')


def set_scheduler(opt, sched_params):
    if sched_params["type"] == "Step":
        step_size = sched_params['parameters']['step_size']
        gamma = sched_params['parameters']['gamma']
        scheduler = torch.optim.lr_scheduler.StepLR(opt, step_size=step_size, gamma=gamma)
    elif sched_params["type"] == "CosineAnnealing":
        T_max = sched_params["parameters"]["T_max"]
        eta_min = sched_params["parameters"]["eta_min"]
        scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(opt, T_max=T_max, eta_min=eta_min)
    else:
        print("Error: Learning rate scheduler not recognized or implemented.")
    return scheduler


def train(project, config_file):
    sw.utils.initialize_project_mesh_warper(project)

    with open(config_file) as json_file:
        parameters = json.load(json_file)
    if parameters["tl_net"]["enabled"]:
        supervised_train_tl(config_file)
    else:
        if parameters["loss"]["supervised_latent"]:
            supervised_train(config_file)


'''
Network training method
	defines, initializes, and trains the models
	logs training and validation errors
	saves the model and returns the path it is saved to
'''


def supervised_train(config_file):
    with open(config_file) as json_file:
        parameters = json.load(json_file)
    model_dir = parameters['paths']['out_dir'] + parameters['model_name'] + '/'
    loaders.make_dir(model_dir)
    loader_dir = parameters['paths']['loader_dir']
    aug_dir = parameters['paths']['aug_dir']
    num_epochs = parameters['trainer']['epochs']
    learning_rate = parameters['trainer']['learning_rate']
    eval_freq = parameters['trainer']['val_freq']
    decay_lr = parameters['trainer']['decay_lr']['enabled']
    fine_tune = parameters['fine_tune']['enabled']
    loss_func = method_to_call = getattr(losses, parameters["loss"]["function"])
    # load the loaders
    train_loader_path = loader_dir + "train"
    validation_loader_path = loader_dir + "validation"
    print("Loading data loaders...")
    train_loader = torch.load(train_loader_path)
    val_loader = torch.load(validation_loader_path)
    print("Done.")
    # initializations
    num_pca = train_loader.dataset.pca_target[0].shape[0]
    num_corr = train_loader.dataset.mdl_target[0].shape[0]
    print("Defining net...")
    net = model.DeepSSMNet(config_file)
    device = net.device
    net.to(device)
    # initialize model weights
    net.apply(weight_init(module=nn.Conv3d, initf=nn.init.xavier_normal_))
    net.apply(weight_init(module=nn.Linear, initf=nn.init.xavier_normal_))

    # these lines are for the fine tuning layer initialization
    whiten_mean = np.load(loader_dir + '/mean_PCA.npy')
    whiten_std = np.load(loader_dir + '/std_PCA.npy')
    orig_mean = np.loadtxt(aug_dir + '/PCA_Particle_Info/mean.particles')
    orig_pc = np.zeros([num_pca, num_corr * 3])
    for i in range(num_pca):
        temp = np.loadtxt(aug_dir + '/PCA_Particle_Info/pcamode' + str(i) + '.particles')
        orig_pc[i, :] = temp.flatten()

    bias = torch.from_numpy(orig_mean.flatten()).to(device)  # load the mean here
    weight = torch.from_numpy(orig_pc.T).to(device)  # load the PCA vectors here
    net.decoder.fc_fine.bias.data.copy_(bias)
    net.decoder.fc_fine.weight.data.copy_(weight)

    # define optimizer
    # for the initial steps set the gradient of the final layer to be zero
    for param in net.decoder.fc_fine.parameters():
        param.requires_grad = False

    train_params = net.parameters()
    opt = torch.optim.Adam(train_params, learning_rate)
    opt.zero_grad()
    if decay_lr:
        scheduler = set_scheduler(opt, parameters['trainer']['decay_lr'])
    print("Done.")
    # train
    print("Beginning training on device = " + device + '\n')
    # Initialize logger
    logger = open(model_dir + "train_log.csv", "w+", buffering=1)
    log_print(logger, ["Training_Stage", "Epoch", "LR", "Train_Err", "Train_Rel_Err", "Val_Err", "Val_Rel_Err", "Sec"])
    # Initialize training plot
    train_plot = plt.figure()
    axe = train_plot.add_subplot(111)
    axe.set_title('DeepSSM Training')
    sp_train, = axe.plot([], [], label='Training', ms=10, color='b', marker='o', ls='')
    sp_val, = axe.plot([], [], label='Validation', ms=10, color='r', marker='o', ls='')
    axe.set_xlabel('Epochs')
    axe.set_xlim(0, num_epochs + 1)
    axe.set_ylabel('Particle MSE')
    axe.legend()
    train_plot.savefig(model_dir + "training_plot.png", dpi=300)
    # initialize
    epochs = []
    plot_train_losses = []
    plot_val_losses = []
    t0 = time.time()
    best_val_rel_error = np.Inf
    for e in range(1, num_epochs + 1):
        if sw_check_abort():
            sw_message("Aborted")
            return
        sw_message(f"Epoch {e}/{num_epochs}")
        sw_progress(e / (num_epochs + 1))

        torch.cuda.empty_cache()
        # train
        net.train()
        train_losses = []
        train_rel_losses = []
        pred_particles = []
        true_particles = []
        train_names = []
        for img, pca, mdl, names in train_loader:
            train_names.extend(names)
            opt.zero_grad()
            img = img.to(device)
            pca = pca.to(device)
            mdl = mdl.to(device)
            [pred_pca, pred_mdl] = net(img)
            loss = loss_func(pred_mdl, mdl)
            loss.backward()
            opt.step()
            train_losses.append(loss.item())
            train_rel_loss = loss_func(pred_mdl, mdl) / loss_func(pred_mdl * 0, mdl)
            train_rel_losses.append(train_rel_loss.item())
            pred_particles.extend(pred_mdl.detach().cpu().numpy())
            true_particles.extend(mdl.detach().cpu().numpy())
        train_viz.write_examples(np.array(pred_particles), np.array(true_particles), train_names,
                                 model_dir + "examples/train_")
        # test validation
        pred_particles = []
        true_particles = []
        val_names = []
        if ((e % eval_freq) == 0 or e == 1):
            net.eval()
            val_losses = []
            val_rel_losses = []
            for img, pca, mdl, names in val_loader:
                val_names.extend(names)
                opt.zero_grad()
                img = img.to(device)
                pca = pca.to(device)
                mdl = mdl.to(device)
                [pred_pca, pred_mdl] = net(img)
                v_loss = loss_func(pred_mdl, mdl)
                val_losses.append(v_loss.item())
                val_rel_loss = loss_func(pred_mdl, mdl) / loss_func(pred_mdl * 0, mdl)
                val_rel_losses.append(val_rel_loss.item())
                pred_particles.extend(pred_mdl.detach().cpu().numpy())
                true_particles.extend(mdl.detach().cpu().numpy())
            train_viz.write_examples(np.array(pred_particles), np.array(true_particles), val_names,
                                     model_dir + "examples/validation_")
            # log
            train_mr_MSE = np.mean(np.sqrt(train_losses))
            val_mr_MSE = np.mean(np.sqrt(val_losses))
            train_rel_err = np.mean(train_rel_losses)
            val_rel_err = np.mean(val_rel_losses)
            last_learning_rate = learning_rate
            if decay_lr:
                last_learning_rate = scheduler.get_last_lr()[0]
            log_print(logger,
                      ["Base_Training", e, last_learning_rate, train_mr_MSE, train_rel_err, val_mr_MSE, val_rel_err,
                       time.time() - t0])
            # plot
            epochs.append(e)
            plot_train_losses.append(train_mr_MSE)
            plot_val_losses.append(val_mr_MSE)
            sp_train.set_data(epochs, plot_train_losses)
            sp_val.set_data(epochs, plot_val_losses)
            axe.set_ylim(0, max(max(plot_train_losses), max(plot_val_losses)) + 3)
            train_plot.canvas.draw()
            train_plot.savefig(model_dir + "training_plot.png")
            # save
            if val_rel_err < best_val_rel_error:
                best_val_rel_error = val_rel_err
                best_epoch = e
                torch.save(net.state_dict(), os.path.join(model_dir, 'best_model.torch'))
            t0 = time.time()
        if decay_lr:
            scheduler.step()

    torch.save(net.state_dict(), os.path.join(model_dir, 'final_model.torch'))
    parameters['best_model_epochs'] = best_epoch
    with open(config_file, "w") as json_file:
        json.dump(parameters, json_file, indent=2)
    print("Training complete, model saved. Best model after epoch " + str(best_epoch))

    # now commence the fine tuning model if present on best model
    if fine_tune:
        print("Beginning fine tuning training step on device = ", device)
        net = model.DeepSSMNet(config_file)
        if parameters["use_best_model"]:
            model_path = os.path.join(model_dir, 'best_model.torch')
        else:
            model_path = os.path.join(model_dir, 'final_model.torch')
        net.load_state_dict(torch.load(model_path))
        net.to(device)

        # logger = open(model_dir + "train_log_ft.csv", "w+")
        # log_print(logger, ["Epoch", "Train_Err_mdl", "Train_Rel_Err_mdl", "Val_Err_mdl", "Val_Rel_Err_mdl", "Sec"])
        ft_epochs = parameters['fine_tune']['epochs']
        learning_rate = parameters['fine_tune']['learning_rate']
        eval_freq = parameters['fine_tune']['val_freq']
        loss_func = method_to_call = getattr(losses, parameters['fine_tune']["loss"])

        # Initialize optimizer
        train_params = net.parameters()
        opt = torch.optim.Adam(train_params, learning_rate)
        opt.zero_grad()

        # Initialize fine-tuning training plot
        train_plot = plt.figure()
        axe = train_plot.add_subplot(111)
        sp_train, = axe.plot([], [], label='Fine Tuning', ms=10, color='b', marker='o', ls='')
        sp_val, = axe.plot([], [], label='Validation', ms=10, color='r', marker='o', ls='')
        axe.set_title('DeepSSM Fine Tuning')
        axe.set_xlabel('Epochs')
        axe.set_xlim(0, ft_epochs + 1)
        axe.set_ylabel('Particle MSE')
        axe.legend()
        train_plot.savefig(model_dir + "training_plot_ft.png", dpi=300)
        epochs = []
        plot_train_losses = []
        plot_val_losses = []
        t0 = time.time()

        # free the last params
        for param in net.decoder.fc_fine.parameters():
            param.requires_grad = True
        # train on the corr loss
        best_ft_val_rel_error = np.Inf
        for e in range(1, ft_epochs + 1):
            if sw_check_abort():
                sw_message("Aborted")
                return
            sw_message(f"Fine Tuning, Epoch {e}/{ft_epochs}")
            sw_progress(e / (num_epochs + 1))

            torch.cuda.empty_cache()
            # train
            net.train()
            train_losses = []
            train_rel_losses = []
            pred_particles = []
            true_particles = []
            train_names = []
            for img, pca, mdl, names in train_loader:
                train_names.extend(names)
                opt.zero_grad()
                img = img.to(device)
                pca = pca.to(device)
                mdl = mdl.to(device)
                [pred_pca, pred_mdl] = net(img)
                loss = loss_func(pred_mdl, mdl)
                loss.backward()
                opt.step()
                train_losses.append(loss.item())
                train_rel_loss = loss_func(pred_mdl, mdl) / loss_func(pred_mdl * 0, mdl)
                train_rel_losses.append(train_rel_loss.item())
                pred_particles.extend(pred_mdl.detach().cpu().numpy())
                true_particles.extend(mdl.detach().cpu().numpy())
            train_viz.write_examples(np.array(pred_particles), np.array(true_particles), train_names,
                                     model_dir + "examples/train_")
            # test validation
            pred_particles = []
            true_particles = []
            val_names = []
            if ((e % eval_freq) == 0 or e == 1):
                net.eval()
                val_losses = []
                val_rel_losses = []
                for img, pca, mdl, names in val_loader:
                    val_names.extend(names)
                    opt.zero_grad()
                    img = img.to(device)
                    pca = pca.to(device)
                    mdl = mdl.to(device)
                    [pred_pca, pred_mdl] = net(img)
                    v_loss = loss_func(pred_mdl, mdl)
                    val_losses.append(v_loss.item())
                    val_rel_loss = (loss_func(pred_mdl, mdl) / loss_func(pred_mdl * 0, mdl)).item()
                    val_rel_losses.append(val_rel_loss)
                    if val_rel_loss < best_ft_val_rel_error:
                        best_ft_val_rel_error = val_rel_loss
                        best_ft_epoch = e
                        torch.save(net.state_dict(), os.path.join(model_dir, 'best_model_ft.torch'))
                    pred_particles.extend(pred_mdl.detach().cpu().numpy())
                    true_particles.extend(mdl.detach().cpu().numpy())
                train_viz.write_examples(np.array(pred_particles), np.array(true_particles), val_names,
                                         model_dir + "examples/validation_")
                # log
                train_mr_MSE = np.mean(np.sqrt(train_losses))
                val_mr_MSE = np.mean(np.sqrt(val_losses))
                train_rel_err = np.mean(train_rel_losses)
                val_rel_err = np.mean(val_rel_losses)
                log_print(logger,
                          ["Fine_Tuning", e, learning_rate, train_mr_MSE, train_rel_err, val_mr_MSE, val_rel_err,
                           time.time() - t0])
                # plot
                epochs.append(e)
                plot_train_losses.append(train_mr_MSE)
                plot_val_losses.append(val_mr_MSE)
                sp_train.set_data(epochs, plot_train_losses)
                sp_val.set_data(epochs, plot_val_losses)
                axe.set_ylim(0, max(max(plot_train_losses), max(plot_val_losses)) + 3)
                train_plot.canvas.draw()
                train_plot.savefig(model_dir + "training_plot_ft.png")

                t0 = time.time()

        logger.close()
        torch.save(net.state_dict(), os.path.join(model_dir, 'final_model_ft.torch'))

        parameters['best_ft_model_epochs'] = best_ft_epoch
        with open(config_file, "w") as json_file:
            json.dump(parameters, json_file, indent=2)
        print("Fine tuning complete, model saved. Best model after epoch " + str(best_ft_epoch))


'''
Network training method for TL-Net model
	defines, initializes, and trains the models
	logs training and validation errors
	saves the model and returns the path it is saved to
'''


def supervised_train_tl(config_file):
    with open(config_file) as json_file:
        parameters = json.load(json_file)
    model_dir = parameters['paths']['out_dir'] + parameters['model_name'] + '/'
    loaders.make_dir(model_dir)
    loader_dir = parameters['paths']['loader_dir']
    learning_rate = parameters['trainer']['learning_rate']
    eval_freq = parameters['trainer']['val_freq']
    decay_lr = parameters['trainer']['decay_lr']['enabled']
    a_ae = parameters["tl_net"]["a_ae"]
    c_ae = parameters["tl_net"]["c_ae"]
    a_lat = parameters["tl_net"]["a_lat"]
    c_lat = parameters["tl_net"]["c_lat"]
    # load the loaders
    train_loader_path = loader_dir + "train"
    validation_loader_path = loader_dir + "validation"
    print("Loading data loaders...")
    train_loader = torch.load(train_loader_path)
    val_loader = torch.load(validation_loader_path)
    print("Done.")
    print("Defining model...")
    net = model.DeepSSMNet_TLNet(config_file)
    device = net.device
    net.to(device)
    # initialize model weights
    net.apply(weight_init(module=nn.Conv3d, initf=nn.init.xavier_normal_))
    net.apply(weight_init(module=nn.Linear, initf=nn.init.xavier_normal_))

    train_params = net.parameters()
    opt = torch.optim.Adam(train_params, learning_rate)
    opt.zero_grad()
    if decay_lr:
        scheduler = set_scheduler(opt, parameters['trainer']['decay_lr'])
    print("Done.")
    # train
    print("Beginning training on device = " + device + '\n')
    logger = open(model_dir + "train_log.csv", "w+")

    ae_epochs = parameters['tl_net']['ae_epochs']

    # Initialize training plot
    train_plot = plt.figure()
    axe = train_plot.add_subplot(111)
    axe.set_title('TL-DeepSSSM(Autoencoder) Training')
    sp_train, = axe.plot([], [], label='Training', ms=10, color='b', marker='o', ls='')
    sp_val, = axe.plot([], [], label='Validation', ms=10, color='r', marker='o', ls='')
    axe.set_xlabel('Epochs')
    axe.set_xlim(0, ae_epochs + 1)
    axe.set_ylabel('Particle MSE')
    axe.legend()
    train_plot.savefig(model_dir + "training_plot_ae.png", dpi=300)
    # initialize
    epochs = []
    plot_train_losses = []
    plot_val_losses = []
    t0 = time.time()
    best_val_rel_error = np.Inf

    # train the AE first

    log_print(logger, ["Training_Stage", "Epoch", "LR", "Train_Err", "Train_Rel_Err", "Val_Err", "Val_Rel_Err", "Sec"])

    mean_mdl = torch.mean(train_loader.dataset.mdl_target, 0).to(device)

    for e in range(1, ae_epochs + 1):
        if sw_check_abort():
            sw_message("Aborted")
            return
        sw_message(f"Autoencoder Epoch {e}/{ae_epochs}")
        sw_progress(e / (ae_epochs + 1))

        torch.cuda.empty_cache()
        # train
        net.train()
        ae_train_losses = []
        ae_train_rel_losses = []
        pred_particles = []
        true_particles = []
        train_names = []
        for img, pca, mdl, names in train_loader:
            train_names.extend(names)
            opt.zero_grad()
            img = img.to(device)
            pca = pca.to(device)
            mdl = mdl.to(device)
            [pred_pt, lat, lat_img] = net(mdl, img)
            if parameters["loss"]["function"] == "MSE":
                loss = losses.MSE(pred_pt, mdl)
                train_rel_loss = losses.MSE(pred_pt, mdl) / losses.MSE(pred_pt * 0, mdl)
            else:
                loss = losses.Focal(pred_pt, mdl, a_ae, c_ae)
                train_rel_loss = losses.Focal(pred_pt, mdl, a_ae, c_ae) / losses.Focal(pred_pt * 0, mdl, a_ae, c_ae)
            loss.backward()
            opt.step()
            ae_train_losses.append(loss.item())
            ae_train_rel_losses.append(train_rel_loss.item())
            pred_particles.extend(pred_pt.detach().cpu().numpy())
            true_particles.extend(mdl.detach().cpu().numpy())
        train_viz.write_examples(np.array(pred_particles), np.array(true_particles), train_names,
                                 model_dir + "examples/train_")
        # test validation
        pred_particles = []
        true_particles = []
        val_names = []
        if ((e % eval_freq) == 0 or e == 1):
            net.eval()
            ae_val_losses = []
            ae_val_rel_losses = []
            for img, pca, mdl, names in val_loader:
                val_names.extend(names)
                opt.zero_grad()
                img = img.to(device)
                pca = pca.to(device)
                mdl = mdl.to(device)
                [pred_pt, lat, lat_img] = net(mdl, img)
                # again in validation we simply compute standard l2 loss
                loss_ae = losses.MSE(pred_pt, mdl)
                ae_val_rel_loss = losses.MSE(pred_pt, mdl) / losses.MSE(pred_pt * 0, mdl)
                ae_val_losses.append(loss_ae.item())
                ae_val_rel_losses.append(ae_val_rel_loss.item())
                pred_particles.extend(pred_pt.detach().cpu().numpy())
                true_particles.extend(mdl.detach().cpu().numpy())
            train_viz.write_examples(np.array(pred_particles), np.array(true_particles), val_names,
                                     model_dir + "examples/validation_")
            # log
            train_ae_err = np.mean(ae_train_losses)
            train_rel_ae_err = np.mean(ae_train_rel_losses)
            val_ae_err = np.mean(ae_val_losses)
            val_rel_ae_err = np.mean(ae_val_rel_losses)
            last_learning_rate = learning_rate
            if decay_lr:
                last_learning_rate = scheduler.get_last_lr()[0]
            log_print(logger,
                      ["AE", e, last_learning_rate, train_ae_err, train_rel_ae_err, val_ae_err, val_rel_ae_err,
                       time.time() - t0])
            # plot
            epochs.append(e)
            plot_train_losses.append(train_ae_err)
            plot_val_losses.append(val_ae_err)
            sp_train.set_data(epochs, plot_train_losses)
            sp_val.set_data(epochs, plot_val_losses)
            axe.set_ylim(0, max(max(plot_train_losses), max(plot_val_losses)) + 3)
            train_plot.canvas.draw()
            train_plot.savefig(model_dir + "training_plot_ae.png")
            t0 = time.time()
    # save
    torch.save(net.state_dict(), os.path.join(model_dir, 'final_model_ae.torch'))
    # fix the autoencoder and train the TL-net
    for param in net.CorrespondenceDecoder.parameters():
        param.requires_grad = False
    for param in net.CorrespondenceEncoder.parameters():
        param.requires_grad = False

    # train the t-flank
    tf_epochs = parameters['tl_net']['tf_epochs']

    # Initialize training plot
    train_plot = plt.figure()
    axe = train_plot.add_subplot(111)
    axe.set_title('TL-DeepSSM(T-Flank) Training')
    sp_train, = axe.plot([], [], label='Training', ms=10, color='b', marker='o', ls='')
    sp_val, = axe.plot([], [], label='Validation', ms=10, color='r', marker='o', ls='')
    axe.set_xlabel('Epochs')
    axe.set_xlim(0, tf_epochs + 1)
    axe.set_ylabel('Particle MSE')
    axe.legend()
    train_plot.savefig(model_dir + "training_plot_tf.png", dpi=300)
    # initialize
    t0 = time.time()
    epochs = []
    plot_train_losses = []
    plot_val_losses = []
    for e in range(1, tf_epochs + 1):
        if sw_check_abort():
            sw_message("Aborted")
            return
        sw_message(f"T-Flank Epoch {e}/{tf_epochs}")
        sw_progress(e / (tf_epochs + 1))

        torch.cuda.empty_cache()
        # train
        net.train()
        tf_train_losses = []
        tf_train_rel_losses = []
        pred_particles = []
        true_particles = []
        train_names = []
        for img, pca, mdl, names in train_loader:
            train_names.extend(names)
            opt.zero_grad()
            img = img.to(device)
            pca = pca.to(device)
            mdl = mdl.to(device)
            [pred_pt, lat, lat_img] = net(mdl, img)
            if parameters["loss"]["function"] == "MSE":
                loss = losses.MSE(lat, lat_img)
                train_rel_loss = losses.MSE(lat, lat_img) / losses.MSE(lat * 0, lat_img)
            else:
                loss = losses.Focal(lat, lat_img, a_lat, c_lat)
                train_rel_loss = losses.Focal(lat, lat_img, a_lat, c_lat) / losses.Focal(lat * 0, lat_img, a_lat, c_lat)
            loss.backward()
            opt.step()
            tf_train_losses.append(loss.item())
            tf_train_rel_losses.append(train_rel_loss.item())
            pred_particles.extend(pred_pt.detach().cpu().numpy())
            true_particles.extend(mdl.detach().cpu().numpy())
        train_viz.write_examples(np.array(pred_particles), np.array(true_particles), train_names,
                                 model_dir + "examples/train_")
        # test validation
        pred_particles = []
        true_particles = []
        val_names = []

        if ((e % eval_freq) == 0 or e == 1):
            net.eval()

            tf_val_losses = []
            tf_val_rel_losses = []
            for img, pca, mdl, names in val_loader:
                val_names.extend(names)
                opt.zero_grad()
                img = img.to(device)
                pca = pca.to(device)
                mdl = mdl.to(device)
                [pred_pt, lat, lat_img] = net(mdl, img)
                loss_tf = losses.MSE(lat_img, lat)
                tf_val_rel_loss = losses.MSE(lat_img, lat) / losses.MSE(lat_img * 0, lat)
                tf_val_losses.append(loss_tf.item())
                tf_val_rel_losses.append(tf_val_rel_loss.item())
                pred_particles.extend(pred_pt.detach().cpu().numpy())
                true_particles.extend(mdl.detach().cpu().numpy())
            train_viz.write_examples(np.array(pred_particles), np.array(true_particles), val_names,
                                     model_dir + "examples/validation_")

            # log
            train_tf_err = np.mean(tf_train_losses)
            train_rel_tf_err = np.mean(tf_train_rel_losses)
            val_tf_err = np.mean(tf_val_losses)
            val_rel_tf_err = np.mean(tf_val_rel_losses)
            last_learning_rate = learning_rate
            if decay_lr:
                last_learning_rate = scheduler.get_last_lr()[0]
            log_print(logger,
                      ["T-Flank", e, last_learning_rate, train_tf_err, train_rel_tf_err, val_tf_err, val_rel_tf_err,
                       time.time() - t0])
            # plot
            epochs.append(e)
            plot_train_losses.append(train_tf_err)
            plot_val_losses.append(val_tf_err)
            sp_train.set_data(epochs, plot_train_losses)
            sp_val.set_data(epochs, plot_val_losses)
            axe.set_ylim(0, max(max(plot_train_losses), max(plot_val_losses)) + 3)
            train_plot.canvas.draw()
            train_plot.savefig(model_dir + "training_plot_tf.png")
            t0 = time.time()
    # save
    torch.save(net.state_dict(), os.path.join(model_dir, 'final_model_tf.torch'))
    # jointly train the model
    joint_epochs = parameters['tl_net']['joint_epochs']
    alpha = parameters['tl_net']['alpha']

    for param in net.CorrespondenceDecoder.parameters():
        param.requires_grad = True
    for param in net.CorrespondenceEncoder.parameters():
        param.requires_grad = True

    # Initialize training plot
    train_plot = plt.figure()
    axe = train_plot.add_subplot(111)
    axe.set_title('TL-DeepSSM(Joint) Training')
    sp_train, = axe.plot([], [], label='Training', ms=10, color='b', marker='o', ls='')
    sp_val, = axe.plot([], [], label='Validation', ms=10, color='r', marker='o', ls='')
    axe.set_xlabel('Epochs')
    axe.set_xlim(0, joint_epochs + 1)
    axe.set_ylabel('Particle MSE')
    axe.legend()
    train_plot.savefig(model_dir + "training_plot_joint.png", dpi=300)
    # initialize
    epochs = []
    plot_train_losses = []
    plot_val_losses = []
    t0 = time.time()
    for e in range(1, joint_epochs + 1):
        if sw_check_abort():
            sw_message("Aborted")
            return
        sw_message(f"Joint Epoch {e}/{joint_epochs}")
        sw_progress(e / (joint_epochs + 1))

        # train
        net.train()
        ae_train_rel_losses = []
        tf_train_rel_losses = []
        joint_train_losses = []
        pred_particles = []
        true_particles = []
        train_names = []
        for img, pca, mdl, names in train_loader:
            train_names.extend(names)
            opt.zero_grad()
            img = img.to(device)
            pca = pca.to(device)
            mdl = mdl.to(device)
            [pred_pt, lat, lat_img] = net(mdl, img)
            if parameters["loss"]["function"] == "MSE":
                loss_ae = losses.MSE(pred_pt, mdl)
                loss_tf = losses.MSE(lat_img, lat)
                ae_train_rel_loss = losses.MSE(pred_pt, mdl) / losses.MSE(pred_pt * 0, mdl)
                tf_train_rel_loss = losses.MSE(lat_img, lat) / losses.MSE(lat_img * 0, lat)
            else:
                loss_ae = losses.Focal(pred_pt, mdl, a_ae, c_ae)
                loss_tf = losses.Focal(lat, lat_img, a_lat, c_lat)
                ae_train_rel_loss = losses.Focal(pred_pt, mdl, a_ae, c_ae) / losses.Focal(pred_pt * 0, mdl, a_ae, c_ae)
                tf_train_rel_loss = losses.Focal(lat, lat_img, a_lat, c_lat) / losses.Focal(lat * 0, lat_img, a_lat,
                                                                                            c_lat)
            loss = loss_ae + alpha * loss_tf
            loss.backward()
            opt.step()
            joint_train_losses.append(loss.item())
            ae_train_rel_losses.append(ae_train_rel_loss.item())
            tf_train_rel_losses.append(tf_train_rel_loss.item())
            pred_particles.extend(pred_pt.detach().cpu().numpy())
            true_particles.extend(mdl.detach().cpu().numpy())
        train_viz.write_examples(np.array(pred_particles), np.array(true_particles), train_names,
                                 model_dir + "examples/train_")
        # test validation
        pred_particles = []
        true_particles = []
        val_names = []
        if ((e % eval_freq) == 0 or e == 1):
            net.eval()
            ae_val_rel_losses = []
            tf_val_rel_losses = []
            joint_val_losses = []
            for img, pca, mdl, names in val_loader:
                val_names.extend(names)
                opt.zero_grad()
                img = img.to(device)
                pca = pca.to(device)
                mdl = mdl.to(device)
                [pred_pt, lat, lat_img] = net(mdl, img)
                loss_ae = losses.MSE(pred_pt, mdl)
                ae_val_rel_loss = losses.MSE(pred_pt, mdl) / losses.MSE(pred_pt * 0, mdl)
                loss_tf = losses.MSE(lat_img, lat)
                tf_val_rel_loss = losses.MSE(lat, lat_img) / losses.MSE(lat * 0, lat_img)

                joint_val_losses.append(loss_ae.item() + alpha * loss_tf.item())
                ae_val_rel_losses.append(ae_val_rel_loss.item())
                tf_val_rel_losses.append(tf_val_rel_loss.item())
                pred_particles.extend(pred_pt.detach().cpu().numpy())
                true_particles.extend(mdl.detach().cpu().numpy())
            train_viz.write_examples(np.array(pred_particles), np.array(true_particles), val_names,
                                     model_dir + "examples/validation_")

            # log
            train_rel_ae_err = np.mean(ae_train_rel_losses)
            train_rel_tf_err = np.mean(tf_train_rel_losses)
            val_rel_ae_err = np.mean(ae_val_rel_losses)
            val_rel_tf_err = np.mean(tf_val_rel_losses)
            joint_train_losses = np.mean(joint_train_losses)
            joint_val_losses = np.mean(joint_val_losses)
            last_learning_rate = learning_rate
            if decay_lr:
                last_learning_rate = scheduler.get_last_lr()[0]
            log_print(logger, ["Joint", e, last_learning_rate, train_rel_ae_err, val_rel_ae_err, train_rel_tf_err,
                               val_rel_tf_err, time.time() - t0])
            # plot
            epochs.append(e)
            plot_train_losses.append(joint_train_losses)
            plot_val_losses.append(joint_val_losses)
            sp_train.set_data(epochs, plot_train_losses)
            sp_val.set_data(epochs, plot_val_losses)
            axe.set_ylim(0, max(max(plot_train_losses), max(plot_val_losses)) + 3)
            train_plot.canvas.draw()
            train_plot.savefig(model_dir + "training_plot_joint.png")
            # save
            val_rel_err = val_rel_ae_err + alpha * val_rel_tf_err
            if val_rel_err < best_val_rel_error:
                best_val_rel_error = val_rel_err
                best_epoch = e
                torch.save(net.state_dict(), os.path.join(model_dir, 'best_model.torch'))
            t0 = time.time()
        if decay_lr:
            scheduler.step()

    logger.close()
    torch.save(net.state_dict(), os.path.join(model_dir, 'final_model.torch'))
    parameters['best_model_epochs'] = best_epoch
    with open(config_file, "w") as json_file:
        json.dump(parameters, json_file, indent=2)
    print("Training complete, model saved. Best model after epoch " + str(best_epoch))

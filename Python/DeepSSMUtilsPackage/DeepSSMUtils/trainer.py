import matplotlib.pyplot as plt
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
from torchvision import transforms
from torchvision.transforms import functional as TF

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
            string = values[i]
            if len(string) < 8:
                # pad with spaces up to 8 characters
                string = string + ' ' * (8 - len(string))
            print(string, end='	  ')
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
    if project is not None:
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


#
# # Helper function to create a 3D rotation matrix
# def create_3d_rotation_matrix(angle_x, angle_y, angle_z):
#     # Convert angles to radians
#     ax, ay, az = np.deg2rad([angle_x, angle_y, angle_z])
#
#     # Rotation matrix around x-axis
#     Rx = np.array([
#         [1, 0, 0],
#         [0, np.cos(ax), -np.sin(ax)],
#         [0, np.sin(ax), np.cos(ax)]
#     ])
#
#     # Rotation matrix around y-axis
#     Ry = np.array([
#         [np.cos(ay), 0, np.sin(ay)],
#         [0, 1, 0],
#         [-np.sin(ay), 0, np.cos(ay)]
#     ])
#
#     # Rotation matrix around z-axis
#     Rz = np.array([
#         [np.cos(az), -np.sin(az), 0],
#         [np.sin(az), np.cos(az), 0],
#         [0, 0, 1]
#     ])
#
#     return Rz @ Ry @ Rx

# Helper function to create a 3D rotation matrix
def create_3d_rotation_matrices(angle1, angle2, angle3):
    # Convert angles to radians
    ax, ay, az = np.deg2rad([angle1, angle2, angle3])

    # Rotation matrix around x-axis
    Rx = np.array([
        [1, 0, 0],
        [0, np.cos(ax), -np.sin(ax)],
        [0, np.sin(ax), np.cos(ax)]
    ])

    # Rotation matrix around y-axis
    Ry = np.array([
        [np.cos(ay), 0, np.sin(ay)],
        [0, 1, 0],
        [-np.sin(ay), 0, np.cos(ay)]
    ])

    # Rotation matrix around z-axis
    Rz = np.array([
        [np.cos(az), -np.sin(az), 0],
        [np.sin(az), np.cos(az), 0],
        [0, 0, 1]
    ])

    return Rx, Ry, Rz


f_counter = 0


def augment_sample2(image, particles, center):
    # return image, particles
    # print("Augmenting sample...")
    # print(f"Image shape: {image.shape}")
    # print(f"Particles shape: {particles.shape}")

    original_sw_image = sw.Image(image.numpy().squeeze().transpose().copy())

    angle_mean = 0  # Mean of the distribution
    angle_std_dev = 10  # Standard deviation of the distribution

    # Determine a random angle from a normal distribution for each axis
    angle1 = np.random.normal(loc=angle_mean, scale=angle_std_dev)
    angle2 = np.random.normal(loc=angle_mean, scale=angle_std_dev)
    angle3 = np.random.normal(loc=angle_mean, scale=angle_std_dev)

    # but only rotate 50% of the time
    if np.random.rand() > 0.5:
        angle1 = 0
        angle2 = 0
        angle3 = 0

    # print(f"Angle 1: {angle1}")
    # print(f"Angle 2: {angle2}")
    # print(f"Angle 3: {angle3}")

    original = image.numpy().copy()

    # Convert angle to radians for rotation matrix calculation
    radians1 = np.deg2rad(angle1)
    radians2 = np.deg2rad(angle2)
    radians3 = np.deg2rad(angle3)

    sw_image = sw.Image(image.numpy().squeeze().transpose().copy())

    # print(f"Setting center to {center}")
    sw_image.setOrigin(-(sw_image.size() / 2) + center.tolist())
    # print(f"Center is now: {sw_image.center()}")

    # We want the center to be at "center", set the origin correctly

    sw_image.rotate(radians1, [1.0, 0.0, 0.0])
    sw_image.rotate(radians2, [0.0, 1.0, 0.0])
    sw_image.rotate(radians3, [0.0, 0.0, 1.0])

    image = sw_image.toArray(copy=True, for_viewing=True)
    # convert back to tensor
    image = torch.from_numpy(image).unsqueeze(0)

    # Apply the rotation matrix to the particles

    # Create rotation matrices for each axis
    Rx, Ry, Rz = create_3d_rotation_matrices(-angle1, -angle2, -angle3)

    original_particles = particles

    # Because the images are transposed for some reason in the loader, we need to swap X and Z for the particles as well
    # before applying the rotation matrices, then swap them back

    # Apply the rotation matrices to the particles

    particles = particles.numpy()

    # print shape
    # print(f"Particles shape: {particles.shape}")
    # Particles shape: (1024, 3)

    # print the first 3 particles
    # print(f"Original particles: {particles[0:3, :]}")
    # need to swap the first and last columns to match
    ###particles = particles[:, [2, 1, 0]]
    # print(f"Swapped particles: {particles[0:3, :]}")

    particles = np.dot(particles, Rx)
    particles = np.dot(particles, Ry)
    particles = np.dot(particles, Rz)
    ###particles = particles[:, [2, 1, 0]]
    particles = torch.from_numpy(particles).type_as(original_particles)

    global f_counter
    f_counter += 1
    if f_counter < 10:
        # # Save image and particles to a temporary directory
        temp_dir = "/tmp"
        os.makedirs(temp_dir, exist_ok=True)

        # save as nrrd too
        image_nrrd_path = os.path.join(temp_dir, f"rotated_{f_counter}.nrrd")

        print(f" Saving rotated image to {image_nrrd_path}")
        sw_image.write(image_nrrd_path)

        original_nrrd_path = os.path.join(temp_dir, f"original_{f_counter}.nrrd")
        original = original.squeeze().transpose().copy()
        im = sw.Image(original)
        # im.setOrigin([center[0], center[1], center[2]])
        # im.recenter()

        im.setOrigin(-(im.size() / 2) + center.tolist())

        print(f" Saving original image to {original_nrrd_path}")
        im.write(original_nrrd_path)
        #
        # # save original particles
        np.savetxt(os.path.join(temp_dir, f"particles_{f_counter}.particles"), original_particles.numpy())
        # # save rotated particles
        np.savetxt(os.path.join(temp_dir, f"rotated_particles_{f_counter}.particles"), particles.numpy())

        original_sw_image.write(os.path.join(temp_dir, f"1_original_{f_counter}.nrrd"))
        original_sw_image.recenter()
        original_sw_image.write(os.path.join(temp_dir, f"2_recentered_{f_counter}.nrrd"))
        original_sw_image.setOrigin([center[0], center[1], center[2]])
        original_sw_image.write(os.path.join(temp_dir, f"3_origin_{f_counter}.nrrd"))
    # print the first 3 particles

    return image, particles


def augment_sample(image, particles, center):
    sw_image = sw.Image(image.numpy().squeeze().transpose().copy())

    # this line?!?!?
    sw_image.setOrigin(-(sw_image.size() / 2) + center.tolist())

    image = sw_image.toArray(copy=True, for_viewing=True)
    # convert back to tensor
    image = torch.from_numpy(image).unsqueeze(0)

    return image, particles


def augment_batch(images, particles, centers):
    # print("Augmenting batch...")
    # print(f"Images shape: {images.shape}")
    # print(f"Particles shape: {particles.shape}")
    for i in range(images.shape[0]):
        images[i], particles[i] = augment_sample(images[i], particles[i], centers[i])
    return images, particles


def get_reference_origin(filename):
    # Load the reference image
    reference = sw.Image(filename)
    # Get the origin of the reference image
    reference_origin = reference.origin()
    print(f"Reference origin: {reference_origin}")
    return reference_origin


def supervised_train(config_file):
    with open(config_file) as json_file:
        parameters = json.load(json_file)
    model_dir = parameters['paths']['out_dir'] + parameters['model_name'] + '/'
    loaders.make_dir(model_dir)
    loader_dir = parameters['paths']['loader_dir']
    aug_dir = parameters['paths']['aug_dir']
    deepssm_dir = parameters['paths']['out_dir']
    # reference_origin = get_reference_origin(deepssm_dir + "cropped_reference_image.nrrd")
    num_epochs = parameters['trainer']['epochs']
    learning_rate = parameters['trainer']['learning_rate']
    eval_freq = parameters['trainer']['val_freq']
    decay_lr = parameters['trainer']['decay_lr']['enabled']
    fine_tune = parameters['fine_tune']['enabled']
    loss_func = getattr(losses, parameters["loss"]["function"])
    # load the loaders
    train_loader_path = loader_dir + "train"
    validation_loader_path = loader_dir + "validation"
    print("Loading data loaders...")
    train_loader = torch.load(train_loader_path)
    val_loader = torch.load(validation_loader_path)

    print(f"Train loader shape: {len(train_loader.dataset)}")

    print("Done.")
    # initializations
    num_pca = train_loader.dataset.pca_target[0].shape[0]
    num_corr = train_loader.dataset.mdl_target[0].shape[0]
    print("Defining net...")

    conditional = "conditional_deepssm" in parameters and parameters["conditional_deepssm"]["enabled"]
    # first check if "conditional_deepssm" key is present in the parameters
    if conditional:
        print("Conditional DeepSSM Enabled")
        net = model.ConditionalDeepSSMNet(config_file)
    else:
        print("Regular DeepSSM Enabled")
        net = model.DeepSSMNet(config_file)

    device = net.device
    net.to(device)
    # initialize model weights
    net.apply(weight_init(module=nn.Conv3d, initf=nn.init.xavier_normal_))
    net.apply(weight_init(module=nn.Linear, initf=nn.init.xavier_normal_))

    if not conditional:
        # these lines are for the fine tuning layer initialization
        orig_mean = np.loadtxt(aug_dir + '/PCA_Particle_Info/mean.particles')
        orig_pc = np.zeros([num_pca, num_corr * 3])
        for i in range(num_pca):
            temp = np.loadtxt(aug_dir + '/PCA_Particle_Info/pcamode' + str(i) + '.particles')
            orig_pc[i, :] = temp.flatten()

        bias = torch.from_numpy(orig_mean.flatten()).to(device)  # load the mean here
        weight = torch.from_numpy(orig_pc.T).to(device)  # load the PCA vectors here
        # net.decoder.fc_fine.bias.data.copy_(bias)
        # net.decoder.fc_fine.weight.data.copy_(weight)

        # define optimizer
        # for the initial steps set the gradient of the final layer to be zero
    ##        for param in net.decoder.fc_fine.parameters():
    ##            param.requires_grad = False

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
    best_val_loss = np.Inf
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
        for img, pca, mdl, names, anatomy, center in train_loader:
            img, mdl = augment_batch(img, mdl, center)
            train_names.extend(names)
            opt.zero_grad()
            img = img.to(device)
            pca = pca.to(device)
            mdl = mdl.to(device)
            anatomy = anatomy.to(device)

            [pred_pca, pred_mdl] = net(img, anatomy_type=anatomy)
            # [pred_pca, pred_mdl] = net(img)

            # print first 3 particles
            loss = loss_func(pred_mdl, mdl)

            mdl_numpy = mdl.detach().cpu().numpy()
            pred_mdl_numpy = pred_mdl.detach().cpu().numpy()

            #            print(f"\n---\nPred: {pred_mdl_numpy[0, 0:3, :]}\nTrue: {mdl_numpy[0, 0:3, :]}\nAnatomy: {anatomy[0]}, Loss: {loss.item()}")

            # To do loss on PCA instead of particles
            # loss = loss_func(pred_pca, pca)
            loss.backward()
            opt.step()
            train_losses.append(loss.item())
            train_rel_loss = loss_func(pred_mdl, mdl) / loss_func(pred_mdl * 0, mdl)
            train_rel_losses.append(train_rel_loss.item())
            pred_particles.extend(pred_mdl.detach().cpu().numpy())
            true_particles.extend(mdl.detach().cpu().numpy())
        train_viz.write_examples(np.array(pred_particles), np.array(true_particles), train_names,
                                 model_dir + "examples/train")
        # test validation
        pred_particles = []
        true_particles = []
        val_names = []
        if ((e % eval_freq) == 0 or e == 1):
            net.eval()
            val_losses = []
            val_rel_losses = []
            for img, pca, mdl, names, anatomy, center in val_loader:
                # print(f"Validation anatomy: {anatomy}")
                # augment the validation data
                img, mdl = augment_batch(img, mdl, center)
                val_names.extend(names)
                opt.zero_grad()
                img = img.to(device)
                pca = pca.to(device)
                mdl = mdl.to(device)

                anatomy = anatomy.to(device)
                [pred_pca, pred_mdl] = net(img, anatomy_type=anatomy)
                v_loss = loss_func(pred_mdl, mdl)
                val_losses.append(v_loss.item())
                val_rel_loss = loss_func(pred_mdl, mdl) / loss_func(pred_mdl * 0, mdl)
                val_rel_losses.append(val_rel_loss.item())
                pred_particles.extend(pred_mdl.detach().cpu().numpy())
                true_particles.extend(mdl.detach().cpu().numpy())
            train_viz.write_examples(np.array(pred_particles), np.array(true_particles), val_names,
                                     model_dir + "examples/validation")
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
            if val_mr_MSE < best_val_loss:
                best_val_loss = val_mr_MSE
            t0 = time.time()
        if decay_lr:
            scheduler.step()

    torch.save(net.state_dict(), os.path.join(model_dir, 'final_model.torch'))
    parameters['best_model_epochs'] = best_epoch
    with open(config_file, "w") as json_file:
        json.dump(parameters, json_file, indent=2)
    print("\n")
    print("Training complete, model saved. Best model after epoch " + str(best_epoch))
    print(f"Best validation relative error: {best_val_rel_error}")
    print(f"Best validation loss: {best_val_loss}")

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
            for img, pca, mdl, names, anatomy in train_loader:
                train_names.extend(names)
                opt.zero_grad()
                img = img.to(device)
                pca = pca.to(device)
                mdl = mdl.to(device)
                [pred_pca, pred_mdl] = net(img, anatomy_type=anatomy)
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
                for img, pca, mdl, names, anatomy in val_loader:
                    val_names.extend(names)
                    opt.zero_grad()
                    img = img.to(device)
                    pca = pca.to(device)
                    mdl = mdl.to(device)
                    [pred_pca, pred_mdl] = net(img, anatomy_type=anatomy)
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

    print(f"learning rate: {learning_rate}")

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
        for img, pca, mdl, names, anatomy in train_loader:
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

import numpy as np
np.random.seed(1)

import tensorflow as tf
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' #supress warnings
import tensorflow_probability as tfp
tfd = tfp.distributions

import lds_model
import example

if __name__=="__main__":
	# Define dimensions
    N = 3    # number of samples
    d = 2    # dimension of correspondence points
    M = 9    # number of correspondence points
    L = 2    # dimension of latent space
    P = d*M  # dimension of observation space 
    T = 5   # number of time points

    # Generate data
    X = example.generate_ellipse_data(N,T,False)

    # Initialize Tensorflow model
    tf_model = tfd.LinearGaussianStateSpaceModel(
        num_timesteps=T,
        transition_matrix=tf.ones([L,L]),
        transition_noise=tfd.MultivariateNormalDiag(
            scale_diag= tf.ones([L])),
        observation_matrix=tf.ones([P,L]),
        observation_noise=tfd.MultivariateNormalDiag(
            scale_diag=tf.ones([P])),
        initial_state_prior=tfd.MultivariateNormalDiag(loc=tf.ones([L]),
            scale_diag=tf.ones([L]))
    )

    # Initialize my model
    my_model = lds_model.LDS_Model(data_dim=P, latent_dim=L, num_time=T, num_samples=3)
    # Initialize identically
    my_model.W = tf_model.observation_matrix.numpy()
    my_model.obs_Sigma = tf_model.observation_noise.covariance().numpy()
    my_model.A = tf_model.transition_matrix.numpy()
    my_model.state_Sigma = tf_model.transition_noise.covariance().numpy()
    my_model.mu_0 = tf_model.initial_state_prior.mean().numpy()
    my_model.V_0 = tf_model.initial_state_prior.covariance().numpy()


    ### Forward filter comparison
    # TF forward filter
    tf_X = tf.convert_to_tensor(X, np.float32)
    tf_log_likelihoods, tf_filtered_means, tf_filtered_covs, tf_predicted_means, tf_predicted_covs, \
        tf_observation_means, tf_observation_covs = tf_model.forward_filter(tf_X)
    # My forward filter
    my_model.set_observation_data(X)
    my_model._forward_filter()
    # Equivalency
    print("\nChecking forward filter equivalency... ")
    print("Filtered means: " + str(np.allclose(tf_filtered_means.numpy(), my_model.filtered_mu, rtol=1e-4)))
    print("Filtered covs: " + str(np.allclose(tf_filtered_covs.numpy(), my_model.filtered_V, rtol=1e-4)))
    print("Predicted means: " + str(np.allclose(tf_predicted_means.numpy(), my_model.predicted_mu, rtol=1e-4)))
    print("Predicted covs: " + str(np.allclose(tf_predicted_covs.numpy(), my_model.predicted_V[0], rtol=1e-3)))

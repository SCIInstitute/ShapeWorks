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
    N = 2    # number of samples
    d = 2    # dimension of correspondence points
    M = 9    # number of correspondence points
    L = 2    # dimension of latent space
    P = d*M  # dimension of observation space 
    T = 2    # number of time points

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
    my_model = lds_model.LDS_Model(data_dim=P, latent_dim=L, num_time=T, num_samples=N)
    # Initialize identically
    my_model.W = tf_model.observation_matrix.numpy()
    my_model.obs_Sigma = tf_model.observation_noise.covariance().numpy()
    my_model.A = tf_model.transition_matrix.numpy()
    my_model.state_Sigma = tf_model.transition_noise.covariance().numpy()
    my_model.mu_0 = tf_model.initial_state_prior.mean().numpy()
    my_model.V_0 = tf_model.initial_state_prior.covariance().numpy()

    ### Forward filter comparison
    # TF forward filter
    tf_X = tf.convert_to_tensor(np.asarray(X, np.float32), np.float32)
    tf_log_likelihoods, tf_filtered_means, tf_filtered_covs, tf_predicted_means, tf_predicted_covs, \
        tf_observation_means, tf_observation_covs = tf_model.forward_filter(tf_X)
    # My forward filter
    my_model.set_observation_data(X)
    my_model._forward_filter()
    log_likelihood = my_model.data_log_prob(X)
    # Equivalency
    print("\nChecking forward filter equivalency... ")
    print("Filtered means: " + str(np.allclose(tf_filtered_means.numpy(), my_model.filtered_mu, rtol=1e-4)))
    print("Filtered covs: " + str(np.allclose(tf_filtered_covs.numpy(), my_model.filtered_V, rtol=1e-4)))
    print("Predicted means: " + str(np.allclose(tf_predicted_means.numpy(), my_model.predicted_mu, rtol=1e-4)))
    print("Predicted covs: " + str(np.allclose(tf_predicted_covs.numpy(), my_model.predicted_V[0], rtol=1e-3)))
    print("Log likelihood: " + str(np.allclose(np.sum(tf_log_likelihoods.numpy(),axis=1), log_likelihood, rtol=.1e-1)))

    # TF backward
    tf_posterior_means, tf_posterior_covs = tf_model.backward_smoothing_pass(tf_filtered_means, \
        tf_filtered_covs, tf_predicted_means, tf_predicted_covs)
    # My backward
    my_model._backward_smooth()
    print("\nChecking backward smoothing equivalency... ")
    print("Smoothed means: " + str(np.allclose(tf_posterior_means.numpy(), my_model.smoothed_mu, rtol=1e-1)))
    print("Smoothed covs: " + str(np.allclose(tf_posterior_covs.numpy(), my_model.smoothed_V[0], rtol=1e-2)))
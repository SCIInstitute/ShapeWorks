"""
Jadie Adams
Test for LDS_model

Data is generated using a true time-varying LDS model with known parameters.
New LDS models (both time in varaint and time varying) are then fit to the data
and the difference in observation log likelihood between the true and trained 
models is analyzed.
"""
import numpy as np
import matplotlib.pyplot as plt
import LDS_model
import utils

def test():
    # Define dimensions
    N = 100    # number of samples
    L = 2    # dimension of latent space
    P = 3    # dimension of observation space 
    T = 6    # number of time points

    # Define true parameters
    W = utils.repeat(T, np.random.randn(P, L)) # Random observation matrix
    obs_Sigma = 0.2*np.eye(P) # Observation covariance
    # Random rotation transition matrix (time-dependent)
    A = []
    for i in range(T-1):
        A.append(0.9*utils.random_rotation_matrix(L))
    A = np.array(A)
    state_Sigma = 0.1*np.eye(L) # State covariance
    # Initial state ditribution - standard normal
    mu_0 = np.zeros(L)
    V_0 = np.eye(L)

    # Define true model
    true_model = LDS_model.LDS( n_dim_obs=P, n_dim_state=L,
        transition_matrices=A, observation_matrices=W,
        transition_covariance=state_Sigma, observation_covariance=obs_Sigma,
        transition_offsets=np.zeros(L), observation_offsets=np.zeros(P),
        initial_state_mean=mu_0, initial_state_covariance=V_0
    )

    # Sample training and test data from true model
    train_obs_samples = []
    test_obs_samples = []
    for i in range(N):
        (state_sample, obs_sample) = true_model.sample(T)
        train_obs_samples.append(obs_sample)
        (state_sample, obs_sample) = true_model.sample(T)
        test_obs_samples.append(obs_sample)
    train_obs_samples = np.array(train_obs_samples)
    test_obs_samples = np.array(test_obs_samples)

    # Get loglikelihood of true model 
    target_loglikelihood = true_model.loglikelihood(test_obs_samples)

    # Initialize invaraint model - default parameters
    trained_invariant_model = LDS_model.LDS( n_dim_obs=P, n_dim_state=L,
            em_vars=[
            'transition_matrices', 'observation_matrices',
            'transition_covariance', 'observation_covariance',
            'transition_offsets', 'observation_offsets',
            'initial_state_mean', 'initial_state_covariance'
        ])

    # Initialize time varying model - default parameters (A and W passed to ensure time-varying)
    initial_A = utils.repeat(T-1, np.eye(L))
    initial_W = utils.repeat(T, np.random.normal(0.0, 1.0, (P, L)))
    trained_model = LDS_model.LDS( n_dim_obs=P, n_dim_state=L,
        transition_matrices = initial_A, 
        observation_matrices = initial_W,
        em_vars=[
        'transition_matrices', 'observation_matrices',
        'transition_covariance', 'observation_covariance',
        'transition_offsets', 'observation_offsets',
        'initial_state_mean', 'initial_state_covariance'
    ])

    # TRAINING - Learn good values for parameters named in `em_vars` using the EM algorithm
    iterations = 20
    invariant_loglikelihoods = np.zeros(iterations)
    loglikelihoods = np.zeros(iterations)
    for i in range(iterations):
        trained_invariant_model.em(train_obs_samples, n_iter=1)
        invariant_loglikelihoods[i] = trained_invariant_model.loglikelihood(test_obs_samples)

        trained_model.em(train_obs_samples, n_iter=1)
        loglikelihoods[i] = trained_model.loglikelihood(test_obs_samples)

    # Draw log likelihood of observations as a function of EM iteration number.
    # Notice how it is increasing (this is guaranteed by the EM algorithm)
    plt.figure()
    plt.axhline(y=target_loglikelihood, color='r', linestyle='-', label='True Model')
    plt.plot(loglikelihoods, label='Trained Time-Varying Model')
    plt.plot(invariant_loglikelihoods, label='Trained Time-Invariant Model')
    plt.xlabel('EM Iteration Number')
    plt.ylabel('Average Log Likelihood')
    plt.legend()
    plt.savefig("loglikelihood_varying.png")

    # Reconstruction mean square error
    reconstructed_test_obs = trained_invariant_model.reconstruct(test_obs_samples)
    print("Time invariant LDS reconstruction MSE:")
    print(np.mean((test_obs_samples - reconstructed_test_obs)**2))

    print("Time-varying LDS reconstruction MSE:")
    reconstructed_test_obs = trained_model.reconstruct(test_obs_samples)
    print(np.mean((test_obs_samples - reconstructed_test_obs)**2))

if __name__=="__main__":
    test()
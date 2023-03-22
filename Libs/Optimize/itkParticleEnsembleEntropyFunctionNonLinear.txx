#pragma once

#include "ParticleImageDomainWithGradients.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkParticleGaussianModeWriter.h"
#include <Utils/Utils.h>
#include <string>

namespace itk
{
template <unsigned int VDimension>
void
ParticleEnsembleEntropyFunctionNonLinear<VDimension>
::WriteModes(const std::string &prefix, int n) const
{
    typename ParticleGaussianModeWriter<VDimension>::Pointer writer =
            ParticleGaussianModeWriter<VDimension>::New();
    writer->SetShapeMatrix(m_ShapeMatrix);
    writer->SetFileName(prefix.c_str());
    writer->SetNumberOfModes(n);
    writer->Update();
}

template <unsigned int VDimension>
void
ParticleEnsembleEntropyFunctionNonLinear<VDimension>
::ComputeGradientUpdates()
{
    auto start = std::chrono::system_clock::now();
    {
    // 1. Forward Pass and get latent space representation, jacobians
    // All tensors in CPU and double precision here
    MSG("Hi! Before Iterations in FINAL CPP COMPUTE GRADS | COMPUTING Gradient Updates...");
    unsigned int dM = m_ShapeMatrix->rows();
    int M = (int)(dM / 3);
    unsigned int N = m_ShapeMatrix->cols();
    unsigned int L = m_ShapeMatrix->GetLatentDimensions();
    int B  = m_ShapeMatrix->GetBatchSize();
    m_BaseShapeMatrix->clear();
    m_BaseShapeMatrix->set_size(L, N);
    m_BaseShapeMatrix->fill(0.0);
    m_GradientUpdatesNet->clear();
    m_GradientUpdatesNet->set_size(dM, N);
    m_GradientUpdatesNet->fill(0.0);

    m_log_det_g_ar->clear(); m_log_det_g_ar->resize(N);
    m_log_det_j_ar->clear(); m_log_det_j_ar->resize(N);
    m_log_prob_u_ar->clear(); m_log_prob_u_ar->resize(N);

    Tensor jacobian_matrix_all = torch::zeros({N, L, dM}, torch::TensorOptions(torch::kCPU).dtype(torch::kDouble));
    std::cout << "AAA" << std::endl;

    try{        
        // B forward passes with jacobian computation
        int n = 0;
        DEBUG(n); DEBUG(N);
        while (n < N)
        {
            unsigned int block_size = ((n+B) < N) ? B : (N-n);
            std::cout << "n = " << n << "and BS = " << block_size << std::endl;
            std::vector<double> log_det_g(block_size, 0.0); 
            std::vector<double> log_det_j(block_size, 0.0); 
            std::vector<double> log_prob_u(block_size, 0.0); 

            auto input_vec = m_ShapeMatrix->get_n_columns(n, block_size); // B vectors
            input_vec = input_vec.inplace_transpose(); // B X dM
            Tensor jacobian_matrix;
            Tensor output_tensor; // returned in Double CPU
            auto vmar = input_vec.data_array();
            Tensor input_tensor = torch::from_blob(input_vec.data_block(), {block_size, dM}, torch::TensorOptions(torch::kCPU).dtype(torch::kDouble)).clone();
            this->m_ShapeMatrix->ForwardPass(input_tensor, output_tensor, 
                                                    jacobian_matrix, log_prob_u, 
                                                    log_det_g, log_det_j);
            std::cout << "CJ 1 | bs = " << block_size << std::endl;
            jacobian_matrix_all.index_put_({torch::indexing::Slice(n, n+block_size), "..."}, jacobian_matrix);
            std::cout << "Forward Pass Done | output size " << output_tensor.sizes() << std::endl;
            double* output_data_ptr = output_tensor.data_ptr<double>();
            std::cout << "CJ 2" << std::endl;
            vnl_matrix<double> output_vec;
            output_vec.set_size(block_size, L);
            std::cout << "CJ 2.1" << std::endl;
            output_vec.set(output_data_ptr);
            std::cout << "CJ 3" << std::endl;
            output_vec = output_vec.inplace_transpose();
            std::cout << "CJ 4" << std::endl;
            m_BaseShapeMatrix->set_columns(n, output_vec);
            std::cout << "CJ 5" << std::endl;
            m_log_det_g_ar->insert(m_log_det_g_ar->begin() + n, log_det_g.begin(), log_det_g.end());
            m_log_det_j_ar->insert(m_log_det_j_ar->begin() + n, log_det_j.begin(), log_det_j.end());
            m_log_prob_u_ar->insert(m_log_prob_u_ar->begin() + n, log_prob_u.begin(), log_prob_u.end());
            std::cout << "CJ 6" << std::endl;
            n += block_size;
            std::cout << "CJ loop end | n = " << n << std::endl; 
        }
    }
    catch (const c10::Error& e) {
        std::cout << "Errors in Libtorch operations during Gradient Computations - part 1 | " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }

    // 2. Do regular gradient computations in latent space assuming Gaussian Distrubution
    std::cout << "COMPUTING SW grads" << std::endl;
    this->ComputeBaseSpaceGradientUpdates();
    
    // 3. Compute Final Gradient Updates in Data space (Z)
    try
    {
        vnl_matrix_type tmp(*m_PointsUpdateBase);
        tmp = tmp.inplace_transpose(); // N X L
        auto vmar2 = tmp.data_array();
        Tensor dH_dU = torch::from_blob(tmp.data_block(), {N, L}, torch::TensorOptions(torch::kCPU).dtype(torch::kDouble)).clone();
        dH_dU = dH_dU.view({N, 1, L});

        dH_dU = dH_dU.to(torch::Device(torch::kCUDA, 0));
        std::cout << " dH_dU is in  " << std::endl;
        jacobian_matrix_all = jacobian_matrix_all.to(torch::Device(torch::kCUDA, 0));

        // N X 1 X L \times N X L X dM ===> N X 1 x dM
        Tensor dH_dZ = torch::bmm(dH_dU, jacobian_matrix_all); // N X 1 X dM
        dH_dZ = dH_dZ.view({N, dM}).to(torch::kCPU);
        dH_dZ = dH_dZ.transpose(0, 1); // dM X N
        double* grad_data_ptr = dH_dZ.data_ptr<double>();
        m_GradientUpdatesNet->set(grad_data_ptr);

        dH_dZ = dH_dZ.view({M, -1, N});
        auto norms = torch::norm(dH_dZ, 2, {1});
        m_MaxMove = torch::max(norms).item<double>();
    }
    catch (const c10::Error& e) {
        std::cout << "Errors in Libtorch operations during Gradient Computations - part 2 | " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }
    } c10::cuda::CUDACachingAllocator::emptyCache();
    auto end = std::chrono::system_clock::now();
    std::cout << "Net Gradient Computation Execution Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

}

template <unsigned int VDimension>
void
ParticleEnsembleEntropyFunctionNonLinear<VDimension>
::ComputeBaseSpaceGradientUpdates()
{

    // Do computations in Z0 space assuming Gaussian Distrubution
    const unsigned int num_samples = m_BaseShapeMatrix->cols(); // N
    const unsigned int num_dims    = m_BaseShapeMatrix->rows(); // L


    // Do we need to resize the covariance matrix?
    if (m_PointsUpdateBase->rows() != num_dims || m_PointsUpdateBase->cols() != num_samples)
    {
        m_PointsUpdateBase->set_size(num_dims, num_samples);
        m_PointsUpdateBase->fill(0.0);
    }
    vnl_matrix_type points_minus_mean; // Z0 space
    points_minus_mean.clear();
    points_minus_mean.set_size(num_dims, num_samples);
    points_minus_mean.fill(0.0);

    m_points_mean->clear();
    m_points_mean->set_size(num_dims, 1);

    // Compute the covariance matrix.
    // (A is D' in Davies paper)
    // Compute the mean shape vector.
    double _total = 0.0;
    for (unsigned int j = 0; j < num_dims; j++)
    {
        double total = 0.0;
        for (unsigned int i = 0; i < num_samples; i++)
        {
            total += m_BaseShapeMatrix->operator()(j, i);
        }
        m_points_mean->put(j,0, total/(double)num_samples);
        _total += total;
    }


    for (unsigned int j = 0; j < num_dims; j++)
    {
        for (unsigned int i = 0; i < num_samples; i++)
        {
            points_minus_mean(j, i) = m_BaseShapeMatrix->operator()(j, i) - m_points_mean->get(j,0);
        }
    }
//    std:cout << points_minus_mean.extract(num_dims, num_samples, 0, 0) << std::endl;

#ifdef PARTICLE_DEBUG
    std::cout << "Shape Matrix : " << std::endl;
    std::cout << "total : " << _total << std::endl;
    for (unsigned int j = 0; j < num_dims; j++)
    {
        for(unsigned int i = 0; i < num_samples; i++)
        {
            std::cout << points_minus_mean(j,i) << " " ;
        }
        std::cout << "\n";
    }
    std::cout << "Done\n";
#endif

    vnl_diag_matrix<double> W;

    vnl_matrix_type gramMat(num_samples, num_samples, 0.0);
    vnl_matrix_type pinvMat(num_samples, num_samples, 0.0); //gramMat inverse

    if (this->m_UseMeanEnergy)
    {
        pinvMat.set_identity();
        m_InverseCovMatrix->clear();
    }
    else
    {
        gramMat = points_minus_mean.transpose()* points_minus_mean;

        vnl_svd <double> svd(gramMat);

        vnl_matrix_type UG = svd.U();
        W = svd.W();

        vnl_diag_matrix<double> invLambda = svd.W();

        invLambda.set_diagonal(invLambda.get_diagonal()/(double)(num_samples-1) + m_MinimumVariance);
        invLambda.invert_in_place();

        pinvMat = (UG * invLambda) * UG.transpose();

        vnl_matrix_type projMat = points_minus_mean * UG;
        const auto lhs = projMat * invLambda;
        const auto rhs = invLambda * projMat.transpose(); // invLambda doesn't need to be transposed since its a diagonal matrix
        m_InverseCovMatrix->set_size(num_dims, num_dims);
        Utils::multiply_into(*m_InverseCovMatrix, lhs, rhs);
    }
    m_PointsUpdateBase->update(points_minus_mean * pinvMat);

    m_CurrentEnergy = 0.0;

    if (m_UseMeanEnergy)
        m_CurrentEnergy = points_minus_mean.frobenius_norm();
    else
    {
        m_MinimumEigenValue = W(0)*W(0) + m_MinimumVariance;
        for (unsigned int i = 0; i < num_samples; i++)
        {
            double val_i = W(i)*W(i) + m_MinimumVariance;
            if ( val_i < m_MinimumEigenValue)
                m_MinimumEigenValue = val_i;
            m_CurrentEnergy += log(val_i);
        }
    }

    m_CurrentEnergy /= 2.0;
    if (m_UseMeanEnergy)
        m_MinimumEigenValue = m_CurrentEnergy / 2.0;
}

template <unsigned int VDimension>
typename ParticleEnsembleEntropyFunctionNonLinear<VDimension>::VectorType
ParticleEnsembleEntropyFunctionNonLinear<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();

    maxdt  = m_MaxMove; // maximum gradient update possible

    VectorType gradE;
    unsigned int k = 0;
    int dom = d % DomainsPerShape;
    for (int i = 0; i < dom; i++)
        k += system->GetNumberOfParticles(i) * VDimension;
    k += idx*VDimension;

    //TODO: consider mean energy strategies later
    double energy_in_data_space = 0.0;
    int subject_id = d/DomainsPerShape;

    auto start = std::chrono::system_clock::now();
    {try{
        auto input_vec = m_ShapeMatrix->get_n_columns(subject_id, 1);
        auto vmar = input_vec.data_array();
        int dM = m_ShapeMatrix->rows();
        auto vmarr = input_vec.data_array();
        torch:: Tensor input_tensor = torch::from_blob(input_vec.data_block(), {1, dM}, torch::TensorOptions(torch::kCPU).dtype(torch::kDouble)).clone();
        this->m_ShapeMatrix->ComputeEnergy(input_tensor, energy_in_data_space);
    }
    catch (const c10::Error& e) {
        std::cout << "Errors in Libtorch operations during Evaluate | " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }} c10::cuda::CUDACachingAllocator::emptyCache();
    auto end = std::chrono::system_clock::now();
    std::cout <<  "Energy Computation Execution Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;   

    energy = energy_in_data_space;
    double det_g = std::exp(m_log_det_g_ar->at(subject_id));
    double det_j = std::exp(m_log_det_j_ar->at(subject_id));
    double p_u = std::exp(m_log_prob_u_ar->at(subject_id));
    double term2 = ((p_u)/(std::sqrt(det_g) * det_j)) * (m_log_det_j_ar->at(subject_id) - (0.5 * m_log_det_g_ar->at(subject_id)));

    for (unsigned int i = 0; i< VDimension; i++)
    {
        gradE[i] = m_GradientUpdatesNet->get(k + i, d / DomainsPerShape) + term2;
    }


    return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}

} // end namespace

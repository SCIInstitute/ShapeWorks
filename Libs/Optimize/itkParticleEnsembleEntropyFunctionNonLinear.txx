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
::ComputeCovarianceMatrix()
{

    // Do computations in Z0 space assuming Gaussian Distrubution
    auto base_shape_matrix = m_ShapeMatrix->GetBaseShapeMatrix();
    const unsigned int num_samples = base_shape_matrix->cols(); // N
    const unsigned int num_dims    = base_shape_matrix->rows(); // L


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
            total += base_shape_matrix->operator()(j, i);
        }
        m_points_mean->put(j,0, total/(double)num_samples);
        _total += total;
    }


    for (unsigned int j = 0; j < num_dims; j++)
    {
        for (unsigned int i = 0; i < num_samples; i++)
        {
            points_minus_mean(j, i) = base_shape_matrix->operator()(j, i) - m_points_mean->get(j,0);
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
    MSG("Evaluate Non-Linear 0");
    const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();

    // maxdt  = m_MinimumEigenValue;

    VectorType gradE;
    unsigned int k = 0;
    int dom = d % DomainsPerShape;
    for (int i = 0; i < dom; i++)
        k += system->GetNumberOfParticles(i) * VDimension;
    k += idx*VDimension;

    auto base_shape_matrix = m_ShapeMatrix->GetBaseShapeMatrix();
    vnl_matrix_type Xi(3,1,0.0);
    Xi(0,0) = base_shape_matrix->operator()(k  , d/DomainsPerShape) - m_points_mean->get(k, 0);
    Xi(1,0) = base_shape_matrix->operator()(k+1, d/DomainsPerShape) - m_points_mean->get(k+1, 0);
    Xi(2,0) = base_shape_matrix->operator()(k+2, d/DomainsPerShape) - m_points_mean->get(k+2, 0);


    vnl_matrix_type tmp1(3, 3, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1.set_identity();
    else
        tmp1 = m_InverseCovMatrix->extract(3,3,k,k);

    vnl_matrix_type tmp = Xi.transpose()*tmp1;

    tmp *= Xi;

    // Get New gradient updates
    vnl_matrix<double> shape_vec_new = base_shape_matrix->get_n_columns(d/DomainsPerShape, 1); // shape: dM X 1
    unsigned int dM = m_ShapeMatrix->rows();
    unsigned int L = base_shape_matrix->rows();
    double log_det_g = 0.0;
    double log_det_j = 0.0;
    double log_prob_u = 0.0;
    {
    try{
        auto ten_options = torch::TensorOptions().dtype(torch::kDouble);
        torch::Tensor shape_vec_new_tensor = torch::from_blob(shape_vec_new.data_block(), {1, dM}, ten_options);
        shape_vec_new_tensor = shape_vec_new_tensor.to(torch::kFloat);

        torch::Tensor jacobian_matrix = torch::zeros({1, L, dM});
        this->m_ShapeMatrix->RunForwardPassWithJacbian(log_prob_u, log_det_g, log_det_j, shape_vec_new_tensor, jacobian_matrix);
        jacobian_matrix = jacobian_matrix.view({L, dM}).to(torch::kFloat);
        MSG("Forward Pass Done");
        auto det_g = std::exp(log_det_g); auto det_j = std::exp(log_det_j); auto p_u = std::exp(log_prob_u);
        
        DEBUG(det_g); DEBUG(det_j); DEBUG(p_u);

        vnl_matrix cur_shape_grad = m_PointsUpdateBase->get_n_columns(d / DomainsPerShape, 1);
        torch::Tensor dH_u = torch::from_blob(cur_shape_grad.data_block(), {1, L}, ten_options);
        torch::Tensor term1 = torch::linalg_matmul(dH_u, jacobian_matrix); // 1 X dM
        auto term2 = ((p_u)/(std::sqrt(det_g) * det_j)) * (log_det_j - (0.5 * log_det_g));

        for (unsigned int i = 0; i< VDimension; i++)
        {
            gradE[i] = term1[0][k+i].item<double>() + term2;
        }
        maxdt  = gradE.magnitude();
        // TODO: Look alternate strategies
        energy = -(log_prob_u + log_det_g + log_det_j);
    }
    catch (const c10::Error& e) {
        std::cout << "Errors in Libtorch operations while Gradient Set | " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
  }
  }
   c10::cuda::CUDACachingAllocator::emptyCache();
    MSG("Evaluate Non-Linear 1");
    return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}

} // end namespace

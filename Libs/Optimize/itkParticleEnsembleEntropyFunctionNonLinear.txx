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
    const unsigned int num_samples = base_shape_matrix->cols();
    const unsigned int num_dims    = base_shape_matrix->rows();


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
    std::cout << "****** Start Evaluate Non Linear 0" << std::endl;
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

    energy = tmp(0,0); // Z0 Energy

    // Get New gradient updates
    vnl_matrix<double> shape_vec_new = base_shape_matrix->get_n_columns(d/DomainsPerShape, 1); // shape: dM X 1
    unsigned int dM = m_ShapeMatrix->rows();
    double log_det_jacobian_val = 0.0;
    double det_jacobian_val, p_z_val;
    try{
        torch::Tensor shape_vec_new_tensor = torch::from_blob(shape_vec_new.data_block(), {dM});
        torch::Tensor jacobian_matrix = torch::zeros({dM, dM}, this->m_ShapeMatrix->GetDevice());
        torch::Tensor p_z_0 = torch::zeros({1, dM}, this->m_ShapeMatrix->GetDevice());
        this->m_ShapeMatrix->DoForwardPass(shape_vec_new_tensor, jacobian_matrix, log_det_jacobian_val, p_z_0);
        p_z_0 = p_z_0.view({1, dM});

        det_jacobian_val = std::exp(log_det_jacobian_val);
        torch::Tensor p_z = det_jacobian_val * p_z_0;
        vnl_matrix cur_shape_grad = m_PointsUpdateBase->get_n_columns(d / DomainsPerShape, 1);
        torch::Tensor dH_dz0 = torch::from_blob(cur_shape_grad.data_block(), {1,dM});
        torch::Tensor dH_dz = torch::linalg_matmul(dH_dz0, jacobian_matrix) + (log_det_jacobian_val * p_z);

        for (unsigned int i = 0; i< VDimension; i++)
        {
            gradE[i] = dH_dz.index({0, k+1}).item<double>();
        }
        // maxdt  = (m_MinimumEigenValue * det_jacobian_val) - (p_z_val * log_det_jacobian_val);
        maxdt  = gradE.magnitude();
        // TODO: Look alternate strategies
        
    }
    catch (const c10::Error& e) {
        std::cerr << "Errors in Libtorch operations while Gradient Set | " << e.what() << "\n";
  }
    return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}

} // end namespace

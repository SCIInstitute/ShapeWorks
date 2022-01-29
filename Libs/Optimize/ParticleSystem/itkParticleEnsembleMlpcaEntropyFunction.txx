#ifndef __itkParticleEnsembleMlpcaEntropyFunction_txx
#define __itkParticleEnsembleMlpcaEntropyFunction_txx

#include "itkParticleImageDomainWithGradients.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkParticleGaussianModeWriter.h"
#include "Libs/Utils/Utils.h"
#include <string>

namespace itk
{
template <unsigned int VDimension>
void
ParticleEnsembleMlpcaEntropyFunction<VDimension>
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
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::ComputeCovarianceMatricesWithinResiduals()
{
    // NOTE: This code requires that indices be contiguous, i.e. it wont work if
    // you start deleting particles.

    unsigned int dps = m_ShapeMatrix->GetDomainsPerShape();
    unsigned int N = m_ShapeMatrix->cols();
    std::vector<int> num_particles = m_ShapeMatrix->GetAllNumberOfParticles();
    for(unsigned int k = 0; k < dps; k++){ // for each domain build covariance matrix with within residuals from mlpca terms
        // 1. Compute grand_mean_sub---> 3m X N
        vnl_matrix_type grand_mean_sub;
        grand_mean_sub.set_size(VDimension * num_particles[k], N);
        for(unsigned int i = 0; i < N; i++){
            for(unsigned int j = 0; j < num_particles[k]; j++){
                grand_mean_sub(j * VDimension, i) = m_grand_mean->get(0, i);
                grand_mean_sub(j * VDimension + 1, i) = m_grand_mean->get(1, i);
                grand_mean_sub(j * VDimension + 2, i) = m_grand_mean->get(2, i);
            }
        }

        // 2. Compute within parts
        vnl_matrix_type within_part = m_within_scores->at(k) * m_within_loadings->at(k).transpose(); //-->3m X N

    }
    const unsigned int num_samples = m_ShapeMatrix->cols();
    const unsigned int num_dims    = m_ShapeMatrix->rows();


    // Do we need to resize the covariance matrix?
    if (m_PointsUpdate->rows() != num_dims || m_PointsUpdate->cols() != num_samples)
    {
        m_PointsUpdate->set_size(num_dims, num_samples);
        m_PointsUpdate->fill(0.0);
    }
    vnl_matrix_type points_minus_mean;
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
            total += m_ShapeMatrix->operator()(j, i);
        }
        m_points_mean->put(j,0, total/(double)num_samples);
        _total += total;
    }


    for (unsigned int j = 0; j < num_dims; j++)
    {
        for (unsigned int i = 0; i < num_samples; i++)
        {
            points_minus_mean(j, i) = m_ShapeMatrix->operator()(j, i) - m_points_mean->get(j,0);
        }
    }


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
    m_PointsUpdate->update(points_minus_mean * pinvMat);


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
void
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::ComputeMlpcaTerms()
{
    const unsigned int dps = m_ShapeMatrix->GetDomainsPerShape();
    const unsigned int N  = m_ShapeMatrix->cols(); // num_samples 
    std::vector<int> num_particles = m_ShapeMatrix->GetAllNumberOfParticles();
    unsigned int total_particles = std::accumulate(num_particles.begin(), num_particles.end(), 0);
    unsigned int total_particles_ = (int)(m_ShapeMatrix->rows() / dps * VDimension);
    if(total_particles != total_particles_){
        std::cout << "Particles inconsistent while building ShapeMatrix for within and Between"<< std::endl;
    }

    // Part A: Build the Super Matrix(with Points Transpose -- needed for within and between calculation) from m_ShapeMatrix

    unsigned int n = N * VDimension; 
    std::cout << " n " << n << std::endl;
    unsigned int m = num_points * dps;
    std::cout << "m " << m << std::endl;

    m_super_matrix->set_size(m, n);

    for(unsigned int i = 0; i < N; i++){
        for(unsigned int j = 0; j < total_particles; j++){
            m_super_matrix->put(j, i * VDimension, m_ShapeMatrix->get(j * VDimension, i));
            m_super_matrix->put(j, i * VDimension + 1, m_ShapeMatrix->get(j * VDimension + 1, i));
            m_super_matrix->put(j, i * VDimension + 2, m_ShapeMatrix->get(j * VDimension + 2, i));
        }
    }

    std::cout << "Super matrix constructed " << std::endl;
    // Part B: Compute grand mean
    vnl_matrix_type ones_M;
    ones_M.set_size(total_particles, 1);
    ones_M.fill(1.0);
    vnl_matrix_type grand_mean = (ones_M.transpose() * m_super_matrix) * (1.0/((double)total_particles)); // --> 1 X 3N
    grand_mean = grand_mean.transpose(); // --> 3N X 1

    m_grand_mean->clear();
    m_grand_mean->set_size(VDimension, N); // --> 3 X N
    for(unsigned int i = 0; i < N; i++){
        m_grand_mean->put(0, i, grand_mean(i*VDimension, 0)); 
        m_grand_mean->put(1, i, grand_mean(i*VDimension + 1, 0)); 
        m_grand_mean->put(2, i, grand_mean(i*VDimension + 2, 0)); 
    }
    std::cout << "----Grand Mean Computed----" << std::endl;
    // 1. Within terms
    m_within_loadings->clear();
    m_within_scores->clear();

    for(unsigned int k = 0; k < dps; k++){

        vnl_matrix_type centering_matrix; // Build centering matrix for within
        centering_matrix.set_size(num_particles[k], num_particles[k]);
        centering_matrix.set_identity();
        vnl_matrix_type ones_m_k;
        ones_m_k.set_size(num_particles[k], 1);
        ones_m_k.fill(1.0);
        centering_matrix = centering_matrix - (ones_m_k * ones_m_k.transpose()) * (1.0 / ((double)num_particles[k]);

        vnl_matrix_type z_k;
        z_k.set_size(num_particles[k], VDimension * N);
        m_super_matrix.extract(z_k, k * num_particles[k], 0); // Get shape matrix of the kth organ


        vnl_matrix_type z_within_k = centering_matrix.transpose() * z_k; // m  X 3N
        
        // Compute Svd obejctive --> 3m X N
        vnl_matrix_type z_within_k_objective;
        z_within_k_objective.set_size(VDimension * num_particles[k], N);

        for(unsigned int i = 0 ; i < N; i++){
            for(unsigned int j = 0; num_particles[k]; j++){
                z_within_k_objective(j * VDimension, i) = z_within_k(j, i * VDimension);
                z_within_k_objective(j * VDimension + 1, i) = z_within_k(j, i * VDimension + 1);
                z_within_k_objective(j * VDimension + 2, i) = z_within_k(j, i * VDimension + 2);
            }
        }

        // Do within SVD
        vnl_svd <double> svd_within(z_within_k_objective);
        m_within_scores->push_back(svd_within.U()); // --> 3m X 3m 
        m_within_loadings->push_back(svd_within.V() * svd_within.W().transpose()); // ----> N X N  X  N X 3m ===> N X 3m 
    }
    
    std::cout << "-----Within Terms computed----" << std::endl;

    // 2. Between terms
    m_between_scores->clear();
    m_between_loadings->clear();
    unsigned int M = total_particles;
    vnl_matrix_type between_centering_matrix; // Build centering matrix for between
    between_centering_matrix.set_size(M, M);
    between_centering_matrix.set_identity();
    vnl_matrix_type ones_m;
    ones_m.set_size(M, 1);
    ones_m.fill(1.0);
    between_centering_matrix = between_centering_matrix - (ones_m * ones_m.transpose()) * (1.0 / ((double) M));
    vnl_matrix_type z_centered = between_centering_matrix * m_super_matrix; // M * 3N

    vnl_matrix_type z_between;
    z_between.set_size(dps, VDimension * N);
    z_between.fill(0.0);
    for(unsinged int k = 0; k < dps; k++){
        vnl_matrix_type z_centered_k;
        z_centered_k.set_size(num_particles[k], VDimension * N);
        z_centered.extract(z_centered_k, k * num_particles[k], 0); // extract sub-matrix for kth organ from centred matrix and Center that suub-matrix(bor between)
        vnl_matrix_type ones_m_k;
        ones_m_k.set_size(num_particles[k], 1);
        ones_m_k.fill(1.0);
        vnl_matrix_type z_temp = (ones_m_k.transpose() * z_centered_k) * (1.0 / ((double) num_particles[k])); // --> 1 X 3N
        z_between.update(z_temp, k, 0);
    }

    // tranform K X 3N --> 3K X N
    vnl_matrix_type z_between_objective;
    z_between_objective.set_size(VDimension * dps, N);
    z_between_objective.fill(0.0);
    for(unsigned int k = 0; k < dps; k++){
        for(unsigned int i = 0; i < N; i++){
            z_between_objective(k * VDimension, i) = z_between(k, i * VDimension);
            z_between_objective(k * VDimension + 1, i) = z_between(k, i * VDimension + 1);
            z_between_objective(k * VDimension + 2, i) = z_between(k, i * VDimension + 2);
        }
    }

    vnl_matrix_type avg_matrix;
    avg_matrix.set_size(VDimension * dps, VDimension * dps);
    avg_matrix.set_diagonal();

    for (unsigned int k = 0; k < dps; k++){
        avg_matrix(k * VDimension, k * VDimension) = sqrt(num_particles[k]);
        avg_matrix(k * VDimension + 1, k * VDimension + 1) = sqrt(num_particles[k]);
        avg_matrix(k * VDimension + 2, k * VDimension + 2) = sqrt(num_particles[k]);
    }

    z_between_objective = avg_matrix * z_between_objective;
    vnl_svd <double> svd_between(z_between_objective); // Do Between svd
    m_between_loadings = svd_between.V() * svd_between.W();
    
    for (unsigned int k = 0; k < dps; k++){
        avg_matrix(k * VDimension, k * VDimension) = 1 / sqrt(num_particles[k]);
        avg_matrix(k * VDimension + 1, k * VDimension + 1) = 1 / sqrt(num_particles[k]);
        avg_matrix(k * VDimension + 2, k * VDimension + 2) = 1 / sqrt(num_particles[k]);
    }


    vnl_matrix_type between_scores = avg_matrix *  svd_between.U(); // divide by weights 

    for(unsigned int k = 0; k < dps; k++){
        vnl_matrix_type temp_score; 
        temp_score.set_size(VDimension, N);
        between_scores.extract(temp_score, k * VDimension, 0);
        m_between_scores->push_back(temp_score);
    }
    std::cout << "----ML-PCA terms calculated-----" << std::endl;
}


template <unsigned int VDimension>
typename ParticleEnsembleMlpcaEntropyFunction<VDimension>::VectorType
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // NOTE: This code requires that indices be contiguous, i.e. it won't work if
    // you start deleting particles.
    const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();

    maxdt  = m_MinimumEigenValue;

    VectorType gradE;
    unsigned int k = 0;
    int dom = d % DomainsPerShape;
    for (int i = 0; i < dom; i++)
        k += system->GetNumberOfParticles(i) * VDimension;
    k += idx*VDimension;

    vnl_matrix_type Xi(3,1,0.0);
    Xi(0,0) = m_ShapeMatrix->operator()(k  , d/DomainsPerShape) - m_points_mean->get(k, 0);
    Xi(1,0) = m_ShapeMatrix->operator()(k+1, d/DomainsPerShape) - m_points_mean->get(k+1, 0);
    Xi(2,0) = m_ShapeMatrix->operator()(k+2, d/DomainsPerShape) - m_points_mean->get(k+2, 0);



    vnl_matrix_type tmp1(3, 3, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1.set_identity();
    else
        tmp1 = m_InverseCovMatrix->extract(3,3,k,k);

    vnl_matrix_type tmp = Xi.transpose()*tmp1;

    tmp *= Xi;

    energy = tmp(0,0); // sum up all energies for each matrix

    for (unsigned int i = 0; i< VDimension; i++)
    {
        gradE[i] = m_PointsUpdate->get(k + i, d / DomainsPerShape);
    }


    return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}





template <unsigned int VDimension>
typename ParticleEnsembleMlpcaEntropyFunction<VDimension>::VectorType
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::EvaluateWithin(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // NOTE: This code requires that indices be contiguous, i.e. it won't work if
    // you start deleting particles.
    const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();

    maxdt  = m_MinimumEigenValue;

    VectorType gradE;
    unsigned int k = 0;
    int dom = d % DomainsPerShape;
    for (int i = 0; i < dom; i++)
        k += system->GetNumberOfParticles(i) * VDimension;
    k += idx*VDimension;

    vnl_matrix_type Xi(3,1,0.0);
    Xi(0,0) = m_ShapeMatrix->operator()(k  , d/DomainsPerShape) - m_points_mean->get(k, 0);
    Xi(1,0) = m_ShapeMatrix->operator()(k+1, d/DomainsPerShape) - m_points_mean->get(k+1, 0);
    Xi(2,0) = m_ShapeMatrix->operator()(k+2, d/DomainsPerShape) - m_points_mean->get(k+2, 0);



    vnl_matrix_type tmp1(3, 3, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1.set_identity();
    else
        tmp1 = m_InverseCovMatrix->extract(3,3,k,k);

    vnl_matrix_type tmp = Xi.transpose()*tmp1;

    tmp *= Xi;

    energy = tmp(0,0); // sum up all energies for each matrix

    for (unsigned int i = 0; i< VDimension; i++)
    {
        gradE[i] = m_PointsUpdate->get(k + i, d / DomainsPerShape);
    }


    return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}




template <unsigned int VDimension>
typename ParticleEnsembleMlpcaEntropyFunction<VDimension>::VectorType
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::EvaluateBetween(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // NOTE: This code requires that indices be contiguous, i.e. it won't work if
    // you start deleting particles.
    const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();

    maxdt  = m_MinimumEigenValue;

    VectorType gradE;
    unsigned int k = 0;
    int dom = d % DomainsPerShape;
    for (int i = 0; i < dom; i++)
        k += system->GetNumberOfParticles(i) * VDimension;
    k += idx*VDimension;

    vnl_matrix_type Xi(3,1,0.0);
    Xi(0,0) = m_ShapeMatrix->operator()(k  , d/DomainsPerShape) - m_points_mean->get(k, 0);
    Xi(1,0) = m_ShapeMatrix->operator()(k+1, d/DomainsPerShape) - m_points_mean->get(k+1, 0);
    Xi(2,0) = m_ShapeMatrix->operator()(k+2, d/DomainsPerShape) - m_points_mean->get(k+2, 0);



    vnl_matrix_type tmp1(3, 3, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1.set_identity();
    else
        tmp1 = m_InverseCovMatrix->extract(3,3,k,k);

    vnl_matrix_type tmp = Xi.transpose()*tmp1;

    tmp *= Xi;

    energy = tmp(0,0); // sum up all energies for each matrix

    for (unsigned int i = 0; i< VDimension; i++)
    {
        gradE[i] = m_PointsUpdate->get(k + i, d / DomainsPerShape);
    }


    return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}






} // end namespace
#endif

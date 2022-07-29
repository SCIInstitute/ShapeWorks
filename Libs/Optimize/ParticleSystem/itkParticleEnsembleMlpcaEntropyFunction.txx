#ifndef __itkParticleEnsembleMlpcaEntropyFunction_txx
#define __itkParticleEnsembleMlpcaEntropyFunction_txx

#include "ParticleImageDomainWithGradients.h"
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
::ComputeCentroidForShapeVector(vnl_matrix_type &shape_vector, vnl_matrix_type &centroid_results)
{
    // Helper function to dinf centroid for each organ given the shape vector
    centroid_results.set_size(VDimension * m_total_organs, 1);
    // 1. Transform 3M vector to M X 3 matrix
    vnl_matrix_type temp_matrix;
    unsigned int M = centroid_results.rows();
    temp_matrix.set_size(M, VDimension);
    temp_matrix.fill(0.0);
    for(unsigned int i = 0; i < M; i++){
        temp_matrix.put(i, 0, shape_vector.get(i * VDimension, 0));
        temp_matrix.put(i, 1, shape_vector.get(i * VDimension + 1, 0));
        temp_matrix.put(i, 2, shape_vector.get(i * VDimension + 2, 0));
    }
    for(unsigned int k = 0; k < m_total_organs; k++){
        // Extract organ block 
        vnl_matrix_type temp_k;
        temp_k.set_size(num_particles_ar[k], 3);
        temp_k.clear();
        temp_k.fill(0.0);
        unsigned int idx = num_particles_ar[k] * k;
        temp_matrix.extract(temp_k, idx, 0);
        // find mean
        Eigen::MatrixXd temp_eigen = Eigen::Map<Eigen::MatrixXd>(temp_k.data_block(), temp_k.rows(), temp_k.cols());
        auto centroid_k = temp_eigen.colwise().sum();
        centroid_k = centroid_k / temp_eigen.rows(); // 1 X 3
        centroid_results.put(k * VDimension, 0, centroid_k(0, 0));
        centroid_results.put(k * VDimension + 1, 0, centroid_k(0, 1));
        centroid_results.put(k * VDimension + 2, 0, centroid_k(0, 2));
        
    }

}

template <unsigned int VDimension>
void
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::ComputeShapeRelPoseDeviations()
{
    std::cout << "********Computing Shape Pose Deviation Terms********" << std::endl;
    m_total_organs = m_ShapeMatrix->GetDomainsPerShape(); // K
    unsigned int N  = m_ShapeMatrix->cols(); // num_samples --> N
    std::vector<int> num_particles = m_ShapeMatrix->GetAllNumberOfParticles();
    unsigned int total_particles = std::accumulate(num_particles.begin(), num_particles.end(), 0);
    unsigned int total_particles_ = (int)(m_ShapeMatrix->rows() / (VDimension)); // M
    if(total_particles != total_particles_){
        std::cerr << "Particles inconsistent while building ShapeMatrix....."<< std::endl;
    }
    unsigned int M = total_particles;
    std::cout << "K = " << m_total_organs << " N = " << N << " M = " << M << std::endl;

    m_InverseCovMatrix_shape_dev->clear();
    m_InverseCovMatrix_rel_pose->clear();

    m_PointsUpdate_rel_pose->clear();
    m_PointsUpdate_shape_dev->clear();

    m_points_mean_rel_pose->clear();
    m_points_mean_shape_dev->clear();

    for(unsigned int i = 0; i < m_total_organs; i++){
      vnl_matrix_type inv_cov_matrix_temp;
      vnl_matrix_type points_update_temp;
      vnl_matrix_type mean_temp;

      m_InverseCovMatrix_shape_dev->push_back(inv_cov_matrix_temp);
      m_PointsUpdate_shape_dev->push_back(points_update_temp);
      m_points_mean_shape_dev->push_back(mean_temp);
    }

    // 1. Build Super Matrix from Shape Matrix
    m_super_matrix->set_size(M, N * VDimension);
    for(unsigned int i = 0; i < N; i++){
        for(unsigned int j = 0; j < M; j++){
            m_super_matrix->put(j, i * VDimension, m_ShapeMatrix->get(j * VDimension, i));
            m_super_matrix->put(j, i * VDimension + 1, m_ShapeMatrix->get(j * VDimension + 1, i));
            m_super_matrix->put(j, i * VDimension + 2, m_ShapeMatrix->get(j * VDimension + 2, i));
        }
    }
    vnl_matrix_type super_matrix = m_super_matrix->transpose().transpose();

    // 2. Perform all computations for shape deviations for each organ
    tbb::parallel_for(tbb::blocked_range<size_t>{0, m_total_organs},
    [&](const tbb::blocked_range<size_t>& r){
        for(size_t k = r.begin(); k < r.end(); ++k){
            //Part A: Build objective Matrix containing deviations of particles from COM of organ k
            vnl_matrix_type centering_matrix; // Build centering matrix for within
            centering_matrix.set_size(num_particles[k], num_particles[k]); // M_k X M_k
            centering_matrix.set_identity();
            vnl_matrix_type ones_m_k;
            ones_m_k.set_size(num_particles[k], 1); // m X 1
            ones_m_k.fill(1.0);
            centering_matrix = centering_matrix - (ones_m_k * ones_m_k.transpose()) * (1.0 / ((double)num_particles[k]));
            
            vnl_matrix_type z_k;
            z_k.set_size(num_particles[k], VDimension * N);
            z_k.fill(0.0);
            unsigned int row = 0;
            for(unsigned int x = 0; x < k; x++){ row += num_particles[x];} // k * num_particles[k]
            m_super_matrix->extract(z_k, row, 0); // Extract sub matrix for the kth organ from the super matrix
            vnl_matrix_type z_shape_dev_k = centering_matrix.transpose() * z_k; // ----->  m  X 3N
            // Raw matrix of shape deviations done here
            // Convert to standard form and find residual from its mean
            // Convert to Objective dimensions : -----> 3m X N
            vnl_matrix_type z_shape_dev_k_objective;
            z_shape_dev_k_objective.set_size(VDimension * num_particles[k], N);
            z_shape_dev_k_objective.fill(0.0);

            for(unsigned int i = 0; i < N; i++){
                for(unsigned int j = 0; j < num_particles[k]; j++){
                    unsigned int r = j * VDimension;
                    z_shape_dev_k_objective.put(j * VDimension, i, z_shape_dev_k.get(j, i * VDimension));
                    z_shape_dev_k_objective.put(j * VDimension + 1, i, z_shape_dev_k.get(j, i * VDimension + 1));
                    z_shape_dev_k_objective.put(j * VDimension + 2, i, z_shape_dev_k.get(j, i * VDimension + 2));
                }
            }

            // Compute mean for this objective
            vnl_matrix_type temp;
            temp.clear();
            temp.set_size(VDimension * num_particles[k], 1);
            temp.fill(0);
            for(unsigned int x = 0; x < N; x++){
                temp += z_shape_dev_k_objective.get_n_columns(x, 1);
            }
            temp /= N;
            m_points_mean_shape_dev->at(k) = temp;
            // Subtract mean from the objective
            for(unsigned int x = 0; x < N; x++){
                vnl_matrix_type change_within = z_shape_dev_k_objective.get_n_columns(x, 1) - temp;
                z_shape_dev_k_objective.update(change_within, 0, x);
            }

            // Perform svd and compute gradients for the shape deviations
            vnl_diag_matrix<double> W_k;
            vnl_matrix_type InverseCovMatrix_k;
            vnl_matrix_type gramMat_k(N, N, 0.0); // ---> N X N
            vnl_matrix_type pinvMat_k(N, N, 0.0); //gramMat inverse ---> N X N
            unsigned int num_dims = num_particles[k] * VDimension;
            if (this->m_UseMeanEnergy)
            {
                pinvMat_k.set_identity();
                InverseCovMatrix_k.clear();
            }
            else
            {
                gramMat_k = z_shape_dev_k_objective.transpose()* z_shape_dev_k_objective; // Y'Y where Y = z_shape_dev - mean --> N X N
                vnl_svd<double> svd(gramMat_k);
                vnl_matrix_type UG = svd.U(); // ---> N X N
                W_k = svd.W(); // ---> N X N
                vnl_diag_matrix<double> invLambda_k = svd.W(); // ----> N X N
                invLambda_k.set_diagonal(invLambda_k.get_diagonal()/(double)(N-1) + m_MinimumVariance_within_ar[k]); 
                invLambda_k.invert_in_place();
                pinvMat_k = (UG * invLambda_k) * UG.transpose(); // ---> N X N
                vnl_matrix_type projMat_k = points_minus_mean_k * UG; // 3m X N   X   N X N ---> 3m X N
                const auto lhs = projMat_k * invLambda_k; // ---> 3m X N
                const auto rhs = invLambda_k * projMat_k.transpose(); //---> N X 3m invLambda doesn't need to be transposed since its a diagonal matrix
                InverseCovMatrix_k.set_size(num_dims, num_dims); //---> 3m X 3m 
                Utils::multiply_into(InverseCovMatrix_k, lhs, rhs);
            }
            .update(z_k_between_residual * pinvMat_k); // ---> 3m X N   X  N X N ---> 3m X N update the gradient

            m_CurrentBetweenResidualEnergies.at(k) = 0.0;
            if (m_UseMeanEnergy)
                m_CurrentBetweenResidualEnergies.at(k) = points_minus_mean_k.frobenius_norm();
            else
            {
                m_MinimumBetweenResidualEigenValues.at(k) = W_k(0)*W_k(0) + m_MinimumVariance_within_ar[k];
                for (unsigned int i = 0; i < num_samples; i++)
                {
                    double val_i = W_k(i)*W_k(i) + m_MinimumVariance_within_ar[k];
                    if ( val_i < m_MinimumBetweenResidualEigenValues.at(k))
                         m_MinimumBetweenResidualEigenValues.at(k) = val_i;
                    m_CurrentBetweenResidualEnergies.at(k) += log(val_i);
                }
            }
            m_CurrentBetweenResidualEnergies.at(k) /= 2.0;
            if (m_UseMeanEnergy)
                m_MinimumBetweenResidualEigenValues.at(k) = m_CurrentBetweenResidualEnergies.at(k) / 2.0;

            m_PointsUpdateAllBetween->at(k) = PointsUpdate_k;
            m_InverseCovMatricesAllBetween->at(k) = InverseCovMatrix_k;
            m_points_meanAllBetween->at(k) = points_mean_k;
        }
    });

    // 3. Perform all the computations for the between part

    for (unsigned int n = 0; n < N; n++){
        vnl_matrix_type vec_n, centers_n;
        vec_n.set_size(VDimension * M, 1);
        vec_n.clear();
        m_ShapeMatrix->extract(vec_n, 0, n);
        this->ComputeCentroidForShapeVector(vec_n, centers_n);
        
    }






}


template <unsigned int VDimension>
void
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::ComputeCovarianceMatrix()
{
    // NOTE: This code requires that indices be contiguous, i.e. it wont work if
    // you start deleting particles.
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

//     std::cout << m_PointsUpdate.extract(num_dims, num_samples,0,0) << std::endl;

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

    // (shape_matrix - (within_whole z) )- mean of between
    // (between_comp) - mean of between
    //
    //Another idea think about getting organ mean of that subject
    // similar computation with within
    // multiply with corresponding inv covariances and sum up     
    vnl_matrix_type tmp1(3, 3, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1.set_identity();
    else
        tmp1 = m_InverseCovMatrix->extract(3,3,k,k);

    vnl_matrix_type tmp = Xi.transpose()*tmp1;

    tmp *= Xi;

    energy = tmp(0,0);

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

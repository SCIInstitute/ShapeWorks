#ifndef __itkParticleEnsembleMultiLevelEntropyFunction_txx
#define __itkParticleEnsembleMultiLevelEntropyFunction_txx

#include "ParticleImageDomainWithGradients.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkParticleGaussianModeWriter.h"
#include "Libs/Utils/Utils.h"
#include <string>

namespace itk
{
template <unsigned int VDimension>
void
ParticleEnsembleMultiLevelEntropyFunction<VDimension>
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
ParticleEnsembleMultiLevelEntropyFunction<VDimension>
::ComputeCentroidForShapeVector(const vnl_matrix_type &shape_vector, vnl_matrix_type &centroid_results) const
{
    // Helper function to dinf centroid for each organ given the shape vector
    centroid_results.set_size(VDimension * m_total_organs, 1);
    centroid_results.fill(0.0);

    // 1. Transform 3M vector to M X 3 matrix
    vnl_matrix_type temp_matrix;
    unsigned int M = shape_vector.rows();
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
        temp_k.set_size(m_num_particles_ar[k], 3);
        temp_k.clear();
        temp_k.fill(0.0);
        unsigned int idx = 0;
        for(unsigned int k_prev = 0; k_prev < k; k_prev++){ idx += m_num_particles_ar[k_prev];}
        temp_matrix.extract(temp_k, idx, 0);
        // find mean
        Eigen::MatrixXd temp_eigen = Eigen::Map<Eigen::MatrixXd>(temp_k.data_block(), temp_k.rows(), temp_k.cols());
        Eigen::MatrixXd centroid_k = temp_eigen.colwise().sum();
        centroid_k = (1/temp_eigen.rows()) * centroid_k; // 1 X 3
        centroid_results.put(k * VDimension, 0, centroid_k(0, 0));
        centroid_results.put(k * VDimension + 1, 0, centroid_k(0, 1));
        centroid_results.put(k * VDimension + 2, 0, centroid_k(0, 2));
    }
    return;
}

template <unsigned int VDimension>
void
ParticleEnsembleMultiLevelEntropyFunction<VDimension>
::ComputeShapeRelPoseDeviations()
{
    std::cout << "********Computing Shape Pose Deviation Terms********" << std::endl;
    m_total_organs = m_ShapeMatrix->GetDomainsPerShape(); // K
    unsigned int N  = m_ShapeMatrix->cols(); // num_samples --> N
    m_num_particles_ar = m_ShapeMatrix->GetAllNumberOfParticles();
    unsigned int total_particles = std::accumulate(m_num_particles_ar.begin(), m_num_particles_ar.end(), 0);
    unsigned int total_particles_ = (int)(m_ShapeMatrix->rows() / (VDimension)); // M
    if(total_particles != total_particles_){
        std::cerr << "Particles inconsistent while building ShapeMatrix....."<< std::endl;
    }
    unsigned int M = total_particles;
    std::cout << "K = " << m_total_organs << " N = " << N << " M = " << M << std::endl;

    m_InverseCovMatrix_shape_dev->clear();
    m_InverseCovMatrix_rel_pose->clear(); // Single Matrix

    m_PointsUpdate_rel_pose->clear(); // Single Matrix
    m_PointsUpdate_shape_dev->clear();

    m_points_mean_rel_pose->clear(); // Single Matrix
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
            centering_matrix.set_size(m_num_particles_ar[k], m_num_particles_ar[k]); // M_k X M_k
            centering_matrix.set_identity();
            vnl_matrix_type ones_m_k;
            ones_m_k.set_size(m_num_particles_ar[k], 1); // m X 1
            ones_m_k.fill(1.0);
            centering_matrix = centering_matrix - (ones_m_k * ones_m_k.transpose()) * (1.0 / ((double)m_num_particles_ar[k]));
            
            vnl_matrix_type z_k;
            z_k.set_size(m_num_particles_ar[k], VDimension * N);
            z_k.fill(0.0);
            unsigned int row = 0;
            for(unsigned int x = 0; x < k; x++){ row += m_num_particles_ar[x];} // k * m_num_particles_ar[k]
            m_super_matrix->extract(z_k, row, 0); // Extract sub matrix for the kth organ from the super matrix
            vnl_matrix_type z_shape_dev_k = centering_matrix.transpose() * z_k; // ----->  m  X 3N
            // Raw matrix of shape deviations done here
            // Convert to standard form and find residual from its mean
            // Convert to Objective dimensions : -----> 3m X N
            vnl_matrix_type z_shape_dev_k_objective;
            vnl_matrix_type points_update_shape_dev_k;
            unsigned int num_dims = m_num_particles_ar[k] * VDimension;
            z_shape_dev_k_objective.set_size(num_dims, N);
            z_shape_dev_k_objective.fill(0.0);
            points_update_shape_dev_k.set_size(num_dims, N);
            points_update_shape_dev_k.fill(0.0);

            for(unsigned int i = 0; i < N; i++){
                for(unsigned int j = 0; j < m_num_particles_ar[k]; j++){
                    unsigned int r = j * VDimension;
                    z_shape_dev_k_objective.put(j * VDimension, i, z_shape_dev_k.get(j, i * VDimension));
                    z_shape_dev_k_objective.put(j * VDimension + 1, i, z_shape_dev_k.get(j, i * VDimension + 1));
                    z_shape_dev_k_objective.put(j * VDimension + 2, i, z_shape_dev_k.get(j, i * VDimension + 2));
                }
            }

            // Compute mean for this objective
            vnl_matrix_type mean_shape_dev_k;
            mean_shape_dev_k.clear();
            mean_shape_dev_k.set_size(num_dims, 1);
            mean_shape_dev_k.fill(0);
            for(unsigned int x = 0; x < N; x++){
                mean_shape_dev_k += z_shape_dev_k_objective.get_n_columns(x, 1);
            }
            mean_shape_dev_k /= N;
            m_points_mean_shape_dev->at(k) = mean_shape_dev_k;
            // Subtract mean from the objective
            for(unsigned int x = 0; x < N; x++){
                vnl_matrix_type diff_ = z_shape_dev_k_objective.get_n_columns(x, 1) - mean_shape_dev_k;
                z_shape_dev_k_objective.update(diff_, 0, x);
            }

            // Perform svd and compute gradient updates for the shape deviations
            vnl_diag_matrix<double> W_shape_dev_k;
            vnl_matrix_type InverseCovMatrix_shape_dev_k;
            vnl_matrix_type gramMat_shape_dev_k(N, N, 0.0); // ---> N X N
            vnl_matrix_type pinvMat_shape_dev_k(N, N, 0.0); //gramMat inverse ---> N X N

            if (this->m_UseMeanEnergy)
            {
                pinvMat_shape_dev_k.set_identity();
                InverseCovMatrix_shape_dev_k.clear();
            }
            else
            {
                gramMat_shape_dev_k = z_shape_dev_k_objective.transpose()* z_shape_dev_k_objective; // Y'Y where Y = z_shape_dev - mean --> N X N
                vnl_svd<double> svd(gramMat_shape_dev_k);
                vnl_matrix_type UG = svd.U(); // ---> N X N
                W_shape_dev_k = svd.W(); // ---> N X N
                vnl_diag_matrix<double> invLambda_k = svd.W(); // ----> N X N
                invLambda_k.set_diagonal(invLambda_k.get_diagonal()/(double)(N-1) + m_MinimumVariance_shape_dev_ar[k]); 
                invLambda_k.invert_in_place();
                pinvMat_shape_dev_k = (UG * invLambda_k) * UG.transpose(); // ---> N X N
                vnl_matrix_type projMat_k = z_shape_dev_k_objective * UG; // 3m X N   X   N X N ---> 3m X N
                const auto lhs = projMat_k * invLambda_k; // ---> 3m X N
                const auto rhs = invLambda_k * projMat_k.transpose(); //---> N X 3m invLambda doesn't need to be transposed since its a diagonal matrix
                InverseCovMatrix_shape_dev_k.set_size(num_dims, num_dims); //---> 3m X 3m 
                Utils::multiply_into(InverseCovMatrix_shape_dev_k, lhs, rhs);
            }
            points_update_shape_dev_k.update(z_shape_dev_k_objective * pinvMat_shape_dev_k); // ---> 3m X N   X  N X N ---> 3m X N update the gradient

            m_CurrentEnergy_shape_dev_ar.at(k) = 0.0;
            if (m_UseMeanEnergy)
                m_CurrentEnergy_shape_dev_ar.at(k) = z_shape_dev_k_objective.frobenius_norm();
            else
            {
                m_CurrentEnergy_shape_dev_ar.at(k) = W_shape_dev_k(0)*W_shape_dev_k(0) + m_MinimumVariance_shape_dev_ar[k];
                for (unsigned int i = 0; i < N; i++)
                {
                    double val_i = W_shape_dev_k(i)*W_shape_dev_k(i) + m_MinimumVariance_shape_dev_ar[k];
                    if ( val_i < m_MinimumEigenValue_shape_dev_ar.at(k))
                         m_MinimumEigenValue_shape_dev_ar.at(k) = val_i;
                    m_CurrentEnergy_shape_dev_ar.at(k) += log(val_i);
                }
            }
            m_CurrentEnergy_shape_dev_ar.at(k) /= 2.0;
            if (m_UseMeanEnergy)
                m_MinimumEigenValue_shape_dev_ar.at(k) = m_MinimumEigenValue_shape_dev_ar.at(k) / 2.0;

            m_PointsUpdate_shape_dev->at(k) = points_update_shape_dev_k;
            m_InverseCovMatrix_shape_dev->at(k) = InverseCovMatrix_shape_dev_k;
            m_points_mean_shape_dev->at(k) = mean_shape_dev_k;
        }
    });

    // 3. Perform all the computations for the between part
    vnl_matrix_type z_rel_pose_objective;
    m_PointsUpdate_rel_pose->set_size(1, 1);
    z_rel_pose_objective.set_size(VDimension * m_total_organs, N); // 3K X N
    z_rel_pose_objective.fill(0.0);
    // A. Build Rel Pose Objective Matrix
    for (unsigned int n = 0; n < N; n++){
        vnl_matrix_type centers_n;
        vnl_matrix_type vec_n = m_ShapeMatrix->get_n_columns(n, 1);
        this->ComputeCentroidForShapeVector(vec_n, centers_n);
        z_rel_pose_objective.set_columns(n, centers_n);
    }
    // Find mean
    vnl_matrix_type points_mean_rel_pose;
    points_mean_rel_pose.set_size(VDimension * m_total_organs, N);
    m_points_mean_rel_pose->set_size(VDimension * m_total_organs, N);
    m_points_mean_rel_pose->fill(0.0);
    points_mean_rel_pose.fill(0.0);
    for(unsigned int x = 0; x < N; x++){
        points_mean_rel_pose += z_rel_pose_objective.get_n_columns(x, 1);
    }
    points_mean_rel_pose /= N;
    m_points_mean_rel_pose->update(points_mean_rel_pose, 0, 0);
    // Subtract mean from the objective
    for(unsigned int x = 0; x < N; x++){
        vnl_matrix_type diff = z_rel_pose_objective.get_n_columns(x, 1) - points_mean_rel_pose;
        z_rel_pose_objective.update(diff, 0, x);
    }
    // Perform svd and find inv covariance matrix
    vnl_diag_matrix<double> W_rel_pose;

    vnl_matrix_type gramMat_rel_pose(N, N, 0.0);
    vnl_matrix_type pinvMat_rel_pose(N, N, 0.0); //gramMat inverse

    if (this->m_UseMeanEnergy)
    {
        pinvMat_rel_pose.set_identity();
        m_InverseCovMatrix_rel_pose->clear();
    }
    else
    {
        gramMat_rel_pose = z_rel_pose_objective.transpose()* z_rel_pose_objective;

        vnl_svd <double> svd(gramMat_rel_pose);

        vnl_matrix_type UG = svd.U();
        W_rel_pose = svd.W();

        vnl_diag_matrix<double> invLambda = svd.W();
        invLambda.set_diagonal(invLambda.get_diagonal()/(double)(N-1) + m_MinimumVariance_rel_pose);
        invLambda.invert_in_place();
        pinvMat_rel_pose = (UG * invLambda) * UG.transpose();
        vnl_matrix_type projMat = z_rel_pose_objective * UG;
        const auto lhs = projMat * invLambda;
        const auto rhs = invLambda * projMat.transpose(); // invLambda doesn't need to be transposed since its a diagonal matrix
        m_InverseCovMatrix_rel_pose->set_size(VDimension * m_total_organs, VDimension * m_total_organs);
        Utils::multiply_into(*m_InverseCovMatrix_rel_pose, lhs, rhs);
    }
    m_PointsUpdate_rel_pose->update(z_rel_pose_objective * pinvMat_rel_pose);
    m_CurrentEnergy_rel_pose = 0.0;
    if (m_UseMeanEnergy)
        m_CurrentEnergy_rel_pose = z_rel_pose_objective.frobenius_norm();
    else
    {
        m_MinimumEigenValue_rel_pose = W_rel_pose(0)*W_rel_pose(0) + m_MinimumVariance_rel_pose;
        for (unsigned int i = 0; i < N; i++)
        {
            double val_i = W_rel_pose(i)*W_rel_pose(i) + m_MinimumVariance_rel_pose;
            if ( val_i < m_MinimumEigenValue_rel_pose)
                m_MinimumEigenValue_rel_pose = val_i;
            m_CurrentEnergy_rel_pose += log(val_i);
        }
    }
    m_CurrentEnergy_rel_pose /= 2.0;
    if (m_UseMeanEnergy)
        m_MinimumEigenValue_rel_pose = m_CurrentEnergy_rel_pose / 2.0;
}

template <unsigned int VDimension>
typename ParticleEnsembleMultiLevelEntropyFunction<VDimension>::VectorType
ParticleEnsembleMultiLevelEntropyFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // d / dps ---> Subject idx (n)
    // d % dps ---> Organ idx (k)
    const unsigned int K = m_ShapeMatrix->GetDomainsPerShape();
    const unsigned int cur_organ = d % K;
    const unsigned int cur_sub = d / K;
    maxdt  = m_MinimumEigenValue_rel_pose + m_MinimumEigenValue_shape_dev_ar.at(cur_organ);

    VectorType gradE;
    unsigned int k = 0;
    for (int i = 0; i < cur_organ; i++)
        k += system->GetNumberOfParticles(i) * VDimension;
    k += idx*VDimension;

    vnl_matrix_type Xi_shape_dev(3,1,0.0);
    vnl_matrix_type centroid_new;
    vnl_matrix_type cur_sub_shape_vector = m_ShapeMatrix->get_n_columns(cur_sub, 1);
    this->ComputeCentroidForShapeVector(cur_sub_shape_vector, centroid_new);
    Xi_shape_dev(0,0) = m_ShapeMatrix->operator()(k  , cur_sub) - centroid_new(cur_organ*VDimension, 0) - m_points_mean_shape_dev->at(cur_organ).get(k, 0);
    Xi_shape_dev(1,0) = m_ShapeMatrix->operator()(k  , cur_sub) - centroid_new(cur_organ*VDimension + 1, 0) - m_points_mean_shape_dev->at(cur_organ).get(k+1, 0);
    Xi_shape_dev(2,0) = m_ShapeMatrix->operator()(k  , cur_sub) - centroid_new(cur_organ*VDimension + 2, 0) - m_points_mean_shape_dev->at(cur_organ).get(k+2, 0);

    vnl_matrix_type tmp1_shape_dev(3, 3, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1_shape_dev.set_identity();
    else
        tmp1_shape_dev = m_InverseCovMatrix_shape_dev->at(cur_organ).extract(3,3,k,k);
    vnl_matrix_type tmp_shape_dev = Xi_shape_dev.transpose()*tmp1_shape_dev;

    tmp_shape_dev *= Xi_shape_dev;

    double shape_dev_energy = tmp_shape_dev(0,0);



    vnl_matrix_type Xi_rel_pose(3,1,0.0);
    Xi_rel_pose(0,0) = centroid_new(cur_organ*VDimension, 0) - m_points_mean_rel_pose->get(cur_organ*VDimension, 0);
    Xi_rel_pose(1,0) = centroid_new(cur_organ*VDimension + 1, 0) - m_points_mean_rel_pose->get(cur_organ*VDimension+1, 0);
    Xi_rel_pose(2,0) = centroid_new(cur_organ*VDimension + 2, 0) - m_points_mean_rel_pose->get(cur_organ*VDimension+2, 0);

    vnl_matrix_type tmp1_rel_pose(3, 3, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1_rel_pose.set_identity();
    else
        tmp1_rel_pose = m_InverseCovMatrix_rel_pose->extract(3,3,cur_organ*VDimension,cur_organ*VDimension);
    vnl_matrix_type tmp_rel_pose = Xi_rel_pose.transpose()*tmp1_rel_pose;

    tmp_rel_pose *= Xi_rel_pose;

    energy = tmp_shape_dev(0, 0) + tmp_rel_pose(0, 0);

    for (unsigned int i = 0; i< VDimension; i++)
    {
        gradE[i] = m_PointsUpdate_rel_pose->get(cur_organ*VDimension+i, cur_sub) + m_PointsUpdate_shape_dev->at(cur_organ).get(k+i, cur_sub);
    }
    return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}

} // end namespace
#endif

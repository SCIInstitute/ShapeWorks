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
::ComputeCentroidForShapeVector(const vnl_matrix_type &shape_vector, vnl_matrix_type &centroid_results, vnl_matrix_type &shape_center_all) const
{
    // Helper function to dinf centroid for each organ given the shape vector
    // std::cout << "Inside Compute Centroid " << std::endl;
    unsigned int K = m_ShapeMatrix->GetDomainsPerShape(); // K
    centroid_results.set_size(VDimension * K, 1);
    centroid_results.fill(0.0);
    std::vector<int> num_particles_ar = m_ShapeMatrix->GetAllNumberOfParticles();

    shape_center_all.clear();
    shape_center_all.set_size(1, VDimension);
    shape_center_all.fill(0.0);

    // 1. Transform 3M vector to M X 3 matrix
    vnl_matrix_type temp_matrix;
    unsigned int M = shape_vector.rows();
    unsigned int total_particles = (int)(M/VDimension);
    temp_matrix.set_size(M, VDimension);
    temp_matrix.fill(0.0);
    for(unsigned int i = 0; i < total_particles; i++){
        temp_matrix.put(i, 0, shape_vector.get(i * VDimension, 0));
        temp_matrix.put(i, 1, shape_vector.get(i * VDimension + 1, 0));
        temp_matrix.put(i, 2, shape_vector.get(i * VDimension + 2, 0));
    }
    // std::cout << "Temp matrix constructed"  << std::endl;
    for(unsigned int k = 0; k < K; k++){
        // std::cout << " k = " << k << "total organs = " << K << std::endl;
        // std::cout << " Num particles for kth organ = " << num_particles_ar[k] << std::endl;
        // Extract organ block  
        vnl_matrix_type temp_k;
        temp_k.set_size(num_particles_ar[k], 3);
        // std::cout << "Temp k set size done " << std::endl;
        temp_k.fill(0.0);
        // std::cout << " temp k all done " << std::endl;

        unsigned int idx = 0;
        for(unsigned int k_prev = 0; k_prev < k; k_prev++){ idx += num_particles_ar[k_prev];}
        // std:: cout << " idx = " << idx << std::endl;
        temp_matrix.extract(temp_k, idx, 0);
        // std::cout << "Extract done from temp matrix " << std::endl;
        // find mean
        Eigen::MatrixXd temp_eigen = Eigen::Map<Eigen::MatrixXd>(temp_k.data_block(), temp_k.rows(), temp_k.cols());
        Eigen::RowVectorXd centroid_k = temp_eigen.colwise().mean();
        // std::cout << " Converted to Eigen " << std::endl;
        
        centroid_results.put(k * VDimension, 0, centroid_k(0));
        centroid_results.put(k * VDimension + 1, 0, centroid_k(1));
        centroid_results.put(k * VDimension + 2, 0, centroid_k(2));
    }
    // std::cout << "centroid results done " << std::endl;
    Eigen::MatrixXd all_shapes = Eigen::Map<Eigen::MatrixXd>(temp_matrix.data_block(), temp_matrix.rows(), temp_matrix.cols());
    Eigen::RowVectorXd centroid_all = all_shapes.colwise().mean();
    shape_center_all.put(0, 0, centroid_all(0));
    shape_center_all.put(0, 1, centroid_all(1));
    shape_center_all.put(0, 2, centroid_all(2));
    std::cout << "ComputeCentroidForShapeVector All done " << std::endl;
    return;
}

template <unsigned int VDimension>
void
ParticleEnsembleMultiLevelEntropyFunction<VDimension>
::ComputeCovarianceMatrix()
{
    std::cout << " Multi - Level Covariance Matrix Computation " << std::endl;
    unsigned int K = m_ShapeMatrix->GetDomainsPerShape(); // K
    unsigned int N  = m_ShapeMatrix->cols(); // num_samples --> N
    std::vector<int> num_particles_ar = m_ShapeMatrix->GetAllNumberOfParticles();
    unsigned int total_particles = std::accumulate(num_particles_ar.begin(), num_particles_ar.end(), 0);
    unsigned int total_particles_ = (int)(m_ShapeMatrix->rows() / (VDimension)); // M
    if(total_particles != total_particles_){
        std::cerr << "Particles inconsistent while building ShapeMatrix....."<< std::endl;
    }
    unsigned int M = total_particles;
    std::cout << "K = " << K << " N = " << N << " M = " << M << std::endl;

    m_InverseCovMatrix_shape_dev->clear();
    m_InverseCovMatrix_rel_pose->clear(); // Single Matrix

    m_PointsUpdate_rel_pose->clear(); // Single Matrix
    m_PointsUpdate_shape_dev->clear();

    m_points_mean_rel_pose->clear(); // Single Matrix
    m_points_mean_shape_dev->clear();

    for(unsigned int i = 0; i < K; i++){
      vnl_matrix_type inv_cov_matrix_temp;
      vnl_matrix_type points_update_temp;
      vnl_matrix_type mean_temp;
      m_InverseCovMatrix_shape_dev->push_back(inv_cov_matrix_temp);
      m_PointsUpdate_shape_dev->push_back(points_update_temp);
      m_points_mean_shape_dev->push_back(mean_temp);
    }
    std::cout << "Blank Matrices created ... " << std::endl;

    // 1. Build Super Matrix from Shape Matrix
    m_super_matrix->set_size(M, N * VDimension);
    for(unsigned int i = 0; i < N; i++){
        for(unsigned int j = 0; j < M; j++){
            m_super_matrix->put(j, i * VDimension, m_ShapeMatrix->get(j * VDimension, i));
            m_super_matrix->put(j, i * VDimension + 1, m_ShapeMatrix->get(j * VDimension + 1, i));
            m_super_matrix->put(j, i * VDimension + 2, m_ShapeMatrix->get(j * VDimension + 2, i));
        }
    }
    std::cout << " Super Matrix Constructed " << std::endl;
    vnl_matrix_type ones_M;
    ones_M.set_size(M, 1); // M X 1
    ones_M.fill(1.0);
    vnl_matrix_type super_matrix_temp = m_super_matrix->transpose().transpose();
    vnl_matrix_type grand_mean = ((ones_M.transpose() * super_matrix_temp) * (1.0/((double)total_particles))); // --> 1 X 3N
    super_matrix_temp.clear();
    std::cout << " Grand mean computed .....  " << std::endl;

    // 2. Perform all computations for shape deviations for each organ
    tbb::parallel_for(tbb::blocked_range<size_t>{0, K},
    [&](const tbb::blocked_range<size_t>& r){
        for(size_t k = r.begin(); k < r.end(); ++k){
            //Part A: Build objective Matrix containing deviations of particles from COM of organ k
            vnl_matrix_type centering_matrix; // Build centering matrix for within
            centering_matrix.set_size(num_particles_ar[k], num_particles_ar[k]); // M_k X M_k
            centering_matrix.set_identity();
            vnl_matrix_type ones_m_k;
            ones_m_k.set_size(num_particles_ar[k], 1); // m X 1
            ones_m_k.fill(1.0);
            centering_matrix = centering_matrix - (ones_m_k * ones_m_k.transpose()) * (1.0 / ((double)num_particles_ar[k]));
            std::cout << " centering_matrix computed .....  " << std::endl;

            vnl_matrix_type z_k;
            z_k.set_size(num_particles_ar[k], VDimension * N);
            z_k.fill(0.0);
            unsigned int row = 0;
            for(unsigned int x = 0; x < k; x++){ row += num_particles_ar[x];} // k * num_particles_ar[k]
            m_super_matrix->extract(z_k, row, 0); // Extract sub matrix for the kth organ from the super matrix
            vnl_matrix_type z_shape_dev_k = centering_matrix.transpose() * z_k; // ----->  m  X 3N
            vnl_matrix_type grand_mean_k;
            grand_mean_k.set_size(num_particles_ar[k], VDimension * N);
            grand_mean_k.fill(0.0);
            for(unsigned int t = 0; t < num_particles_ar[k]; t++){
                grand_mean_k.set_row(t, grand_mean.get_row(0));
            }
            std::cout << " grand_mean_k done ....." << std::endl;
            z_shape_dev_k = z_shape_dev_k + grand_mean_k;
            // Convert to Objective dimensions : -----> 3m X N
            vnl_matrix_type z_shape_dev_k_objective;
            vnl_matrix_type points_update_shape_dev_k;
            unsigned int num_dims = num_particles_ar[k] * VDimension;
            z_shape_dev_k_objective.set_size(num_dims, N);
            z_shape_dev_k_objective.fill(0.0);
            points_update_shape_dev_k.set_size(num_dims, N);
            points_update_shape_dev_k.fill(0.0);

            for(unsigned int i = 0; i < N; i++){
                for(unsigned int j = 0; j < num_particles_ar[k]; j++){
                    unsigned int r = j * VDimension;
                    z_shape_dev_k_objective.put(j * VDimension, i, z_shape_dev_k.get(j, i * VDimension));
                    z_shape_dev_k_objective.put(j * VDimension + 1, i, z_shape_dev_k.get(j, i * VDimension + 1));
                    z_shape_dev_k_objective.put(j * VDimension + 2, i, z_shape_dev_k.get(j, i * VDimension + 2));
                }
            }
            std::cout << " z_shape_dev_k_objective done ......" << std::endl;

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
            std::cout << " z_shape_dev_k_objective - mean done ......" << std::endl;


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
            std::cout << " Covariance matrix computation done ......" << std::endl;

            m_CurrentEnergy_shape_dev_ar[k] = 0.0;
            std::cout << " R1 ......" << std::endl;

            if (m_UseMeanEnergy)
                m_CurrentEnergy_shape_dev_ar[k] = z_shape_dev_k_objective.frobenius_norm();
            else
            {
                std::cout << " R2.0 ......" << std::endl;

                m_CurrentEnergy_shape_dev_ar[k] = W_shape_dev_k(0)*W_shape_dev_k(0) + m_MinimumVariance_shape_dev_ar[k];
                std::cout << " R2.1 ......" << std::endl;
                
                for (unsigned int i = 0; i < N; i++)
                {
                    std::cout << " R2.3.1 ......" << std::endl;

                    double val_i = W_shape_dev_k(i)*W_shape_dev_k(i) + m_MinimumVariance_shape_dev_ar[k];
                    if ( val_i < m_MinimumEigenValue_shape_dev_ar[k])
                         m_MinimumEigenValue_shape_dev_ar[k] = val_i;
                    m_CurrentEnergy_shape_dev_ar[k] += log(val_i);
                    std::cout << " R2.3.2 ......" << std::endl;

                }
            }
            std::cout << " R2.4 ......" << std::endl;

            std::cout << " Shape dev energy done ......" << std::endl;

            m_CurrentEnergy_shape_dev_ar[k] /= 2.0;
            if (m_UseMeanEnergy)
                m_MinimumEigenValue_shape_dev_ar[k] = m_MinimumEigenValue_shape_dev_ar[k] / 2.0;

            m_PointsUpdate_shape_dev->at(k) = points_update_shape_dev_k;
            m_InverseCovMatrix_shape_dev->at(k) = InverseCovMatrix_shape_dev_k;
            std::cout << " Shape dev ALL done  for k = " << k << "......" << std::endl;

        }
    });
    std::cout << " Out of Shape dev parallel loop ......" << std::endl;


    // 3. Perform all the computations for the between part
    vnl_matrix_type z_rel_pose_objective;
    m_PointsUpdate_rel_pose->set_size(VDimension * K, N);
    m_PointsUpdate_rel_pose->fill(0.0);
    z_rel_pose_objective.set_size(VDimension * K, N); // 3K X N
    z_rel_pose_objective.fill(0.0);
    // A. Build Rel Pose Objective Matrix
    for (unsigned int n = 0; n < N; n++){
        vnl_matrix_type centers_n;
        vnl_matrix_type temp;
        vnl_matrix_type vec_n = m_ShapeMatrix->get_n_columns(n, 1);
        this->ComputeCentroidForShapeVector(vec_n, centers_n, temp);
        std::cout << " After ComputeCentroidForShapeVector sizes are " << vec_n.rows() << " " <<  centers_n.rows() << " " <<  temp.rows() << std::endl;
        vnl_matrix_type all_center;
        all_center.set_size(VDimension * K, 1);
        all_center.fill(0.0);
        for (unsigned int k = 0; k < K; k++){
            all_center.put(k*VDimension, 0, temp(0, 0));
            all_center.put(k*VDimension+1, 0, temp(0, 1));
            all_center.put(k*VDimension+2, 0, temp(0, 2));
        }
        std::cout << "All centers done" << std::endl;
        centers_n = centers_n + all_center;
        std::cout << "All centers Sum done" << std::endl;
        z_rel_pose_objective.set_columns(n, centers_n);
    }
    std::cout << " Rel Pose objective done ......" << std::endl;

    // Find mean
    vnl_matrix_type points_mean_rel_pose;
    points_mean_rel_pose.set_size(VDimension * K, 1);
    m_points_mean_rel_pose->set_size(VDimension * K, 1);
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

    std::cout << " Rel Pose objective - mean done ......" << std::endl;

    // Perform svd and find inv covariance matrix
    vnl_diag_matrix<double> W_rel_pose;

    vnl_matrix_type gramMat_rel_pose(N, N, 0.0);
    vnl_matrix_type pinvMat_rel_pose(N, N, 0.0); //gramMat inverse
    std::cout << " A " << std::endl;

    if (this->m_UseMeanEnergy)
    {
        std::cout << " A.1 " << std::endl;

        pinvMat_rel_pose.set_identity();
        m_InverseCovMatrix_rel_pose->clear();
    }
    else
    {
        std::cout << " B " << std::endl;

        gramMat_rel_pose = z_rel_pose_objective.transpose()* z_rel_pose_objective;
        std::cout << " B.1 " << std::endl;

        vnl_svd <double> svd_rel_pose(gramMat_rel_pose);
        std::cout << " B.2 " << std::endl;

        vnl_matrix_type UG_rel_pose = svd_rel_pose.U();
        std::cout << " B.3 " << std::endl;

        W_rel_pose = svd_rel_pose.W();
        std::cout << " B.4 " << std::endl;


        vnl_diag_matrix<double> invLambda_rel_pose = svd_rel_pose.W();
        std::cout << " B.5 " << std::endl;

        invLambda_rel_pose.set_diagonal(invLambda_rel_pose.get_diagonal()/(double)(N-1) + m_MinimumVariance_rel_pose);
        invLambda_rel_pose.invert_in_place();
        std::cout << " B.6 " << std::endl;

        pinvMat_rel_pose = (UG_rel_pose * invLambda_rel_pose) * UG_rel_pose.transpose();
        std::cout << " B.7 " << std::endl;
        
        vnl_matrix_type projMat_rel_pose = z_rel_pose_objective * UG_rel_pose;
        std::cout << " B.8 " << std::endl;

        const auto lhs_rel = projMat_rel_pose * invLambda_rel_pose;
        const auto rhs_rel = projMat_rel_pose * projMat_rel_pose.transpose();
        m_InverseCovMatrix_rel_pose->set_size(VDimension * K, VDimension * K);
        std::cout << " B.9 " << std::endl;
        
        Utils::multiply_into(*m_InverseCovMatrix_rel_pose, lhs_rel, rhs_rel);
        std::cout << " B.10 " << std::endl;

    }
    std::cout << " Rel Pose objective - SVD done ......" << std::endl;

    m_PointsUpdate_rel_pose->update(z_rel_pose_objective * pinvMat_rel_pose);
    std::cout << " C.1 " << std::endl;

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
    std::cout << " Rel Pose objective - Energy and eigen computation done ......" << std::endl;
    
}

template <unsigned int VDimension>
typename ParticleEnsembleMultiLevelEntropyFunction<VDimension>::VectorType
ParticleEnsembleMultiLevelEntropyFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // d / dps ---> Subject idx (n)
    // d % dps ---> Organ idx (k)
    std::cout << " Grad Descent -- Evaluate .... " << std::endl;
    const unsigned int K = m_ShapeMatrix->GetDomainsPerShape();
    const unsigned int cur_organ = d % K;
    const unsigned int cur_sub = d / K;
    std::cout << "Cur_organ = " << cur_organ << " cur_sub " << cur_sub << std::endl;
    maxdt  = m_MinimumEigenValue_rel_pose + m_MinimumEigenValue_shape_dev_ar.at(cur_organ);
    // std::cout << " Maxdt computed .... " << std::endl;

    VectorType gradE;
    unsigned int k = 0;
    for (int i = 0; i < cur_organ; i++)
        k += system->GetNumberOfParticles(i) * VDimension;
    k += idx*VDimension;
    std::cout << " gradE computed .... " << std::endl;

    vnl_matrix_type Xi_shape_dev(3,1,0.0);
    vnl_matrix_type centroid_new;
    vnl_matrix_type shape_center_all_organ;
    vnl_matrix_type cur_sub_shape_vector = m_ShapeMatrix->get_n_columns(cur_sub, 1);
    this->ComputeCentroidForShapeVector(cur_sub_shape_vector, centroid_new, shape_center_all_organ);
    Xi_shape_dev(0,0) = m_ShapeMatrix->operator()(k  , cur_sub) - centroid_new(cur_organ*VDimension, 0) + shape_center_all_organ(0, 0) - m_points_mean_shape_dev->at(cur_organ).get(idx * VDimension, 0);
    Xi_shape_dev(1,0) = m_ShapeMatrix->operator()(k+1  , cur_sub) - centroid_new(cur_organ*VDimension + 1, 0)  + shape_center_all_organ(0, 1) - m_points_mean_shape_dev->at(cur_organ).get(idx * VDimension + 1, 0);
    Xi_shape_dev(2,0) = m_ShapeMatrix->operator()(k+2, cur_sub) - centroid_new(cur_organ*VDimension + 2, 0)  + shape_center_all_organ(0, 2) - m_points_mean_shape_dev->at(cur_organ).get(idx * VDimension + 2, 0);
    std::cout << " Xi_shape_dev computed .... " << std::endl;

    vnl_matrix_type tmp1_shape_dev(3, 3, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1_shape_dev.set_identity();
    else
        tmp1_shape_dev = m_InverseCovMatrix_shape_dev->at(cur_organ).extract(3,3,idx * VDimension, idx * VDimension);
    vnl_matrix_type tmp_shape_dev = Xi_shape_dev.transpose()*tmp1_shape_dev;

    tmp_shape_dev *= Xi_shape_dev;

    double shape_dev_energy = tmp_shape_dev(0,0);



    vnl_matrix_type Xi_rel_pose(3,1,0.0);
    Xi_rel_pose(0,0) = centroid_new(cur_organ*VDimension, 0)  - shape_center_all_organ(0, 0)  - m_points_mean_rel_pose->get(cur_organ*VDimension, 0);
    Xi_rel_pose(1,0) = centroid_new(cur_organ*VDimension + 1, 0)  - shape_center_all_organ(0, 1) - m_points_mean_rel_pose->get(cur_organ*VDimension+1, 0);
    Xi_rel_pose(2,0) = centroid_new(cur_organ*VDimension + 2, 0)  - shape_center_all_organ(0, 2) - m_points_mean_rel_pose->get(cur_organ*VDimension+2, 0);
    std::cout << " Xi_rel_pose computed .... " << std::endl;

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
        gradE[i] = m_PointsUpdate_rel_pose->get(cur_organ*VDimension+i, cur_sub) + m_PointsUpdate_shape_dev->at(cur_organ).get(idx * VDimension +i, cur_sub);
    }
    std::cout << " All Evaluate done .... " << std::endl;

    return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}

} // end namespace
#endif

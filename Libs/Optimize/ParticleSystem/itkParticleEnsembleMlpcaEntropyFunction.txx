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

    m_CurrentWithinEnergy = 0.0;
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

        vnl_matrix_type z_k;
        z_k.set_size(VDimension * num_particles[k], N);
        z_k.fill(0.0);
        unsigned int row = 0;
        for(unsigned int x = 0; x < k; x++){ row += (num_particles[k] * VDimension);}
        m_ShapeMatrix->extract(z_k, row, 0);

        vnl_matrix_type z_k_within_residual;
        z_k_within_residual.set_size(VDimension * num_particles[k], N);
        z_k_within_residual.fill(0.0);
        z_k_within_residual = z_k - grand_mean_sub - within_part;

        // 4. Compute covariance matrix of this residual to optimize further

        vnl_matrix_type PointsUpdate_k; //stores gradient of kth organ
        const unsigned int num_samples = m_ShapeMatrix->cols();
        const unsigned int num_dims    = num_particles[k] * VDimension;
        PointsUpdate_k.set_size(num_dims, num_samples);
        PointsUpdate_k.fill(0.0);

        vnl_matrix_type points_minus_mean_k;
        points_minus_mean_k.clear();
        points_minus_mean_k.set_size(num_dims, num_samples); // ---> 3m X N
        points_minus_mean_k.fill(0.0);

        vnl_matrix_type points_mean_k; // stores mean of objective matrix
        points_mean_k.clear();
        points_mean_k.set_size(num_dims, 1); // ---> 3m X 1

        double _total = 0.0;
        for (unsigned int j = 0; j < num_dims; j++)
        {
            double total = 0.0;
            for (unsigned int i = 0; i < num_samples; i++)
            {
                total += z_k_within_residual(j, i);
            }
            points_mean_k(j,0) =  total/(double)num_samples;
            _total += total;
        }

        for (unsigned int j = 0; j < num_dims; j++)
        {
            for (unsigned int i = 0; i < num_samples; i++)
            {
                points_minus_mean_k(j, i) = z_k_within_residual(j, i) - points_mean_k(j,0);
            }
        }

        vnl_diag_matrix<double> W_k;
        vnl_matrix_type InverseCovMatrix_k;

        vnl_matrix_type gramMat_k(num_samples, num_samples, 0.0); // ---> N X N
        vnl_matrix_type pinvMat_k(num_samples, num_samples, 0.0); //gramMat inverse ---> N X N

        if (this->m_UseMeanEnergy)
        {
            pinvMat_k.set_identity();
            InverseCovMatrix_k.clear();
        }
        else
        {
            gramMat_k = points_minus_mean_k.transpose()* points_minus_mean_k; // Y^TY where Y = z_k - grand mean part - within part --> N X N

            vnl_svd<double> svd(gramMat_k);
            vnl_matrix_type UG = svd.U(); // ---> N X N
            W_k = svd.W(); // ---> N X N

            vnl_diag_matrix<double> invLambda_k = svd.W(); // ----> N X N

            invLambda_k.set_diagonal(invLambda_k.get_diagonal()/(double)(num_samples-1) + m_MinimumVariance); // TODO: check if Variance needs to differ for within and between part
            invLambda_k.invert_in_place();

            pinvMat_k = (UG * invLambda_k) * UG.transpose(); // ---> N X N

            vnl_matrix_type projMat_k = points_minus_mean_k * UG; // 3m X N   X   N X N ---> 3m X N
            const auto lhs = projMat_k * invLambda_k; // ---> 3m X N
            const auto rhs = invLambda_k * projMat_k.transpose(); //---> N X 3m invLambda doesn't need to be transposed since its a diagonal matrix
            InverseCovMatrix_k.set_size(num_dims, num_dims); //---> 3m X 3m 
            Utils::multiply_into(InverseCovMatrix_k, lhs, rhs);
        }
        PointsUpdate_k.update(points_minus_mean_k * pinvMat_k); // ---> 3m X N   X  N X N ---> 3m X N update the gradient
        double CurrentWithinEnergy_k = 0.0;
        double MinimumWithinEigenValue_k = 0.0;
        if (m_UseMeanEnergy)
            CurrentWithinEnergy_k = points_minus_mean_k.frobenius_norm();
        else
        {
            MinimumWithinEigenValue_k = W_k(0)*W_k(0) + m_MinimumVariance;
            for (unsigned int i = 0; i < num_samples; i++)
            {
                double val_i = W_k(i)*W_k(i) + m_MinimumVariance;
                if ( val_i < MinimumWithinEigenValue_k)
                    MinimumWithinEigenValue_k = val_i;
                CurrentWithinEnergy_k += log(val_i);
            }
        }
        m_MinimumWithinEigenValue += MinimumWithinEigenValue_k;
        m_CurrentWithinEnergy += CurrentWithinEnergy_k;

        m_PointsUpdateAllWithin->at(k) = PointsUpdate_k;
        m_InverseCovMatricesAllWithin->at(k) = InverseCovMatrix_k;
        m_points_meanAllWithin->at(k) = points_mean_k;
        


    }// end of all domains/organs calculation
    m_CurrentWithinEnergy /= 2.0;
    if (m_UseMeanEnergy)
        m_MinimumWithinEigenValue = m_CurrentWithinEnergy / 2.0;
}



template <unsigned int VDimension>
void
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::ComputeCovarianceMatricesBetweenResiduals()
{
    // NOTE: This code requires that indices be contiguous, i.e. it wont work if
    // you start deleting particles.
    unsigned int dps = m_ShapeMatrix->GetDomainsPerShape();
    unsigned int N = m_ShapeMatrix->cols();
    std::vector<int> num_particles = m_ShapeMatrix->GetAllNumberOfParticles();

    m_CurrentBetweenEnergy = 0.0;
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

        // 2. Compute Between parts
        // vnl_matrix_type between_part = m_between_scores->at(k).transpose() * m_between_loadings->transpose(); //-->3m X N
        vnl_matrix_type between_scores_part;
        between_scores_part.set_size(VDimension * num_particles[k], N); // 3m X N
        between_scores_part.fill(0.0);
        for(unsigned int j = 0; j < num_particles[k]; j++){
            between_scores_part.update(m_between_scores->at(k), j * VDimension, 0);
        }
        vnl_matrix_type between_part = between_scores_part * m_between_loadings->transpose(); //-->3m X N X N X N ---> 3m X N

        vnl_matrix_type z_k;
        z_k.set_size(VDimension * num_particles[k], N);
        z_k.fill(0.0);
        unsigned int row = 0;
        for(unsigned int x = 0; x < k; x++){ row += (num_particles[k] * VDimension);}
        m_ShapeMatrix->extract(z_k, row, 0);

        vnl_matrix_type z_k_between_residual;
        z_k_between_residual.set_size(VDimension * num_particles[k], N);
        z_k_between_residual.fill(0.0);
        z_k_between_residual = z_k - grand_mean_sub - between_part;

        // 4. Compute covariance matrix of this residual to optimize further

        vnl_matrix_type PointsUpdate_k; //stores gradient of kth organ
        const unsigned int num_samples = m_ShapeMatrix->cols();
        const unsigned int num_dims    = num_particles[k] * VDimension;
        PointsUpdate_k.set_size(num_dims, num_samples);
        PointsUpdate_k.fill(0.0);

        vnl_matrix_type points_minus_mean_k;
        points_minus_mean_k.clear();
        points_minus_mean_k.set_size(num_dims, num_samples); // ---> 3m X N
        points_minus_mean_k.fill(0.0);

        vnl_matrix_type points_mean_k; // stores mean of objective matrix
        points_mean_k.clear();
        points_mean_k.set_size(num_dims, 1); // ---> 3m X 1

        double _total = 0.0;
        for (unsigned int j = 0; j < num_dims; j++)
        {
            double total = 0.0;
            for (unsigned int i = 0; i < num_samples; i++)
            {
                total += z_k_between_residual(j, i);
            }
            points_mean_k(j,0) =  total/(double)num_samples;
            _total += total;
        }

        for (unsigned int j = 0; j < num_dims; j++)
        {
            for (unsigned int i = 0; i < num_samples; i++)
            {
                points_minus_mean_k(j, i) = z_k_between_residual(j, i) - points_mean_k(j,0);
            }
        }

        vnl_diag_matrix<double> W_k;
        vnl_matrix_type InverseCovMatrix_k;

        vnl_matrix_type gramMat_k(num_samples, num_samples, 0.0); // ---> N X N
        vnl_matrix_type pinvMat_k(num_samples, num_samples, 0.0); //gramMat inverse ---> N X N

        if (this->m_UseMeanEnergy)
        {
            pinvMat_k.set_identity();
            InverseCovMatrix_k.clear();
        }
        else
        {
            gramMat_k = points_minus_mean_k.transpose()* points_minus_mean_k; // Y^TY where Y = z_k - grand mean part - within part --> N X N

            vnl_svd<double> svd(gramMat_k);
            vnl_matrix_type UG = svd.U(); // ---> N X N
            W_k = svd.W(); // ---> N X N

            vnl_diag_matrix<double> invLambda_k = svd.W(); // ----> N X N

            invLambda_k.set_diagonal(invLambda_k.get_diagonal()/(double)(num_samples-1) + m_MinimumVariance); // TODO: check if Variance needs to differ for within and between part
            invLambda_k.invert_in_place();

            pinvMat_k = (UG * invLambda_k) * UG.transpose(); // ---> N X N

            vnl_matrix_type projMat_k = points_minus_mean_k * UG; // 3m X N   X   N X N ---> 3m X N
            const auto lhs = projMat_k * invLambda_k; // ---> 3m X N
            const auto rhs = invLambda_k * projMat_k.transpose(); //---> N X 3m invLambda doesn't need to be transposed since its a diagonal matrix
            InverseCovMatrix_k.set_size(num_dims, num_dims); //---> 3m X 3m 
            Utils::multiply_into(InverseCovMatrix_k, lhs, rhs);
        }
        PointsUpdate_k.update(points_minus_mean_k * pinvMat_k); // ---> 3m X N   X  N X N ---> 3m X N update the gradient
        double CurrentBetweenEnergy_k = 0.0;
        double MinimumBetweenEigenValue_k = 0.0;
        if (m_UseMeanEnergy)
            CurrentBetweenEnergy_k = points_minus_mean_k.frobenius_norm();
        else
        {
            MinimumBetweenEigenValue_k = W_k(0)*W_k(0) + m_MinimumVariance;
            for (unsigned int i = 0; i < num_samples; i++)
            {
                double val_i = W_k(i)*W_k(i) + m_MinimumVariance;
                if ( val_i < MinimumBetweenEigenValue_k)
                    MinimumBetweenEigenValue_k = val_i;
                CurrentBetweenEnergy_k += log(val_i);
            }
        }
        m_MinimumBetweenEigenValue += MinimumBetweenEigenValue_k;
        m_CurrentBetweenEnergy += MinimumBetweenEigenValue_k;

        m_PointsUpdateAllBetween->at(k) = PointsUpdate_k;
        m_InverseCovMatricesAllBetween->at(k) = InverseCovMatrix_k;
        m_points_meanAllBetween->at(k) = points_mean_k;

    }// end of all domains/organs calculation
    m_CurrentBetweenEnergy /= 2.0;
    if (m_UseMeanEnergy)
        m_MinimumBetweenEigenValue = m_CurrentBetweenEnergy / 2.0;
}


template <unsigned int VDimension>
void
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::ComputeMlpcaTerms()
{
    std::cout << "computing mlpca terms" << std::endl;
    const unsigned int dps = m_ShapeMatrix->GetDomainsPerShape();
    const unsigned int N  = m_ShapeMatrix->cols(); // num_samples 
    // std::cout << "N = " << N << std::endl;
    m_dps = dps;
    m_within_loadings->clear();
    m_within_scores->clear();
    m_between_scores->clear();
    m_InverseCovMatricesAllBetween->clear();
    m_InverseCovMatricesAllWithin->clear();
    m_points_meanAllBetween->clear();
    m_points_meanAllWithin->clear();
    m_PointsUpdateAllBetween->clear();
    m_PointsUpdateAllWithin->clear();    
    for(unsigned int i = 0; i < dps; i++){
      vnl_matrix_type within_loadings;
      vnl_matrix_type within_scores;
      vnl_matrix_type points_update_within;
      vnl_matrix_type points_update_between;
      vnl_matrix_type points_mean_within;
      vnl_matrix_type points_mean_between;
      vnl_matrix_type between_scores;
      vnl_matrix_type inv_cov_matrix_within;
      vnl_matrix_type inv_cov_matrix_between;

      m_within_loadings->push_back(within_loadings);
      m_within_scores->push_back(within_scores);
      m_between_scores->push_back(between_scores);

      m_InverseCovMatricesAllBetween->push_back(inv_cov_matrix_between);
      m_InverseCovMatricesAllWithin->push_back(inv_cov_matrix_within);

      m_points_meanAllBetween->push_back(points_mean_between);
      m_points_meanAllWithin->push_back(points_mean_within);
      
      m_PointsUpdateAllBetween->push_back(points_update_between);
      m_PointsUpdateAllWithin->push_back(points_update_within);
    }

    std::vector<int> num_particles = m_ShapeMatrix->GetAllNumberOfParticles();
    unsigned int total_particles = std::accumulate(num_particles.begin(), num_particles.end(), 0);
    unsigned int total_particles_ = (int)(m_ShapeMatrix->rows() / (VDimension));
    // std::cout << "total particles" << total_particles << "total_particles_" << total_particles_ << std::endl;
    if(total_particles != total_particles_){
        std::cerr << "Particles inconsistent while building ShapeMatrix for within and Between"<< std::endl;
    }
    for(int y = 0; y < num_particles.size(); y++){
        std::cout << " y " << num_particles[y] << " ";
    }
    // std::cout << std::endl << "dps = " << dps << std::endl;

    // Part A: Build the Super Matrix(with Points Transpose -- needed for within and between calculation) from m_ShapeMatrix

    unsigned int n = N * VDimension; //3N
    // std::cout << " n " << n << std::endl;
    unsigned int m = total_particles * dps * VDimension; //MK
    // std::cout << "m " << m << std::endl;

    m_super_matrix->set_size(m, n);

    for(unsigned int i = 0; i < N; i++){
        for(unsigned int j = 0; j < total_particles; j++){
            m_super_matrix->put(j, i * VDimension, m_ShapeMatrix->get(j * VDimension, i));
            m_super_matrix->put(j, i * VDimension + 1, m_ShapeMatrix->get(j * VDimension + 1, i));
            m_super_matrix->put(j, i * VDimension + 2, m_ShapeMatrix->get(j * VDimension + 2, i));
        }
    }

    // std::cout << "Super matrix constructed " << std::endl;
    vnl_matrix_type super_matrix = m_super_matrix->transpose().transpose();
    // Part B: Compute grand mean
    vnl_matrix_type ones_M;
    ones_M.set_size(total_particles, 1);
    ones_M.fill(1.0);

    vnl_matrix_type grand_mean = (ones_M.transpose() * super_matrix) * (1.0/((double)total_particles)); // --> 1 X 3N
    grand_mean = grand_mean.transpose(); // --> 3N X 1

    m_grand_mean->clear();
    m_grand_mean->set_size(VDimension, N); // --> 3 X N
    for(unsigned int i = 0; i < N; i++){
        m_grand_mean->put(0, i, grand_mean(i*VDimension, 0)); 
        m_grand_mean->put(1, i, grand_mean(i*VDimension + 1, 0)); 
        m_grand_mean->put(2, i, grand_mean(i*VDimension + 2, 0)); 
    }
    // std::cout << "----Grand Mean Computed----" << std::endl;
    // 1. Within terms
    // m_within_loadings->clear();
    // m_within_scores->clear();

    for(unsigned int k = 0; k < dps; k++){

        vnl_matrix_type centering_matrix; // Build centering matrix for within
        centering_matrix.set_size(num_particles[k], num_particles[k]);
        centering_matrix.set_identity();
        vnl_matrix_type ones_m_k;
        ones_m_k.set_size(num_particles[k], 1);
        ones_m_k.fill(1.0);
        centering_matrix = centering_matrix - (ones_m_k * ones_m_k.transpose()) * (1.0 / ((double)num_particles[k]));
        // std::cout << "within centering done" << std::endl;
        vnl_matrix_type z_k;
        z_k.set_size(num_particles[k], VDimension * N);
        z_k.fill(0.0);
        unsigned int row = 0;
        for(unsigned int x = 0; x < k; x++){ row += num_particles[k];} // k * num_particles[k]
        m_super_matrix->extract(z_k, row, 0); // Get shape matrix of the kth organ
        // std::cout << "z_k size " << z_k.rows() << " X " << z_k.cols() << std::endl;
        // std::cout << "within extract  for z_k done" << std::endl;

        vnl_matrix_type z_within_k = centering_matrix.transpose() * z_k; // m  X 3N
        // std::cout << "z_within_k size " << z_within_k.rows() << " X " << z_within_k.cols() << std::endl;
        
        // Compute Svd obejctive --> 3m X N
        vnl_matrix_type z_within_k_objective;
        z_within_k_objective.set_size(VDimension * num_particles[k], N);
        z_within_k_objective.fill(0.0);
        // std::cout << "within obj cleared" << std::endl;

        for(unsigned int i = 0; i < N; i++){
            for(unsigned int j = 0; j < num_particles[k]; j++){
                unsigned int r = j * VDimension;
                // std::cout << "row = " << r << std::endl;
                z_within_k_objective.put(j * VDimension, i, z_within_k.get(j, i * VDimension));
                z_within_k_objective.put(j * VDimension + 1, i, z_within_k.get(j, i * VDimension + 1));
                z_within_k_objective.put(j * VDimension + 2, i, z_within_k.get(j, i * VDimension + 2));
            }
        }
        // std::cout << "*********z_within_k_objective******";
        // for(int p = 0; p < z_within_k_objective.rows(); p++){
        //     for(int q = 0; q < z_within_k_objective.cols(); q++){
        //         std::cout << z_within_k_objective.get(p, q) << " ";
        //     }
        //     std::cout << std::endl;
        // }
        // std::cout << "within svd objective made" << std::endl;

        // Do within SVD
        vnl_svd<double> svd_within(z_within_k_objective);
        // std::cout << "within svd of " << k << "done" << std::endl; 
        m_within_scores->at(k) = svd_within.U(); // --> 3m X N 
        m_within_loadings->at(k) = svd_within.V() * svd_within.W(); // ----> N X N  X  N X N ===> N X N 
        // std::cout << "pushed loadings and score for k = " << k << std::endl;
    }
    
    std::cout << "-----Within Terms computed----" << std::endl;

    // 2. Between terms
    // m_between_scores->clear();
    m_between_loadings->clear();
    unsigned int M = total_particles;
    vnl_matrix_type between_centering_matrix; // Build centering matrix for between
    between_centering_matrix.set_size(M, M);
    between_centering_matrix.set_identity();
    vnl_matrix_type ones_m;
    ones_m.set_size(M, 1);
    ones_m.fill(1.0);
    between_centering_matrix = between_centering_matrix - (ones_m * ones_m.transpose()) * (1.0 / ((double) M));
    vnl_matrix_type z_centered = between_centering_matrix * super_matrix; // M * 3N
    // std::cout << "Between centering done" << std::endl;
    vnl_matrix_type z_between;
    z_between.set_size(dps, VDimension * N);
    z_between.fill(0.0);
    for(unsigned int k = 0; k < dps; k++){
        vnl_matrix_type z_centered_k;
        z_centered_k.set_size(num_particles[k], VDimension * N);
        unsigned int row = 0;
        for(unsigned int x = 0; x < k; x++){ row += num_particles[k];} // k * num_particles[k]
        z_centered.extract(z_centered_k, row, 0); // extract sub-matrix for kth organ from centred matrix and Center that suub-matrix(bor between)
        // std::cout << "z_centred_between extracted" << std::endl;
        vnl_matrix_type ones_m_k;
        ones_m_k.set_size(num_particles[k], 1);
        ones_m_k.fill(1.0);
        vnl_matrix_type z_temp = (ones_m_k.transpose() * z_centered_k) * (1.0 / ((double) num_particles[k])); // --> 1 X 3N
        z_between.update(z_temp, k, 0);
        // std::cout << "z_between updated" << std::endl;
    }

    // tranform K X 3N --> 3K X N
    vnl_matrix_type z_between_objective_temp;
    z_between_objective_temp.set_size(VDimension * dps, N); // ---> 3K X N 
    z_between_objective_temp.fill(0.0);
    for(unsigned int k = 0; k < dps; k++){
        for(unsigned int i = 0; i < N; i++){
            z_between_objective_temp.put(k * VDimension, i, z_between(k, i * VDimension));
            z_between_objective_temp.put(k * VDimension + 1, i, z_between(k, i * VDimension + 1));
            z_between_objective_temp.put(k * VDimension + 2, i, z_between(k, i * VDimension + 2));
        }
    }
    // std::cout << "z_between objective_temp done" << std::endl;

    vnl_matrix_type avg_matrix;
    avg_matrix.clear();
    avg_matrix.set_size(VDimension * dps, VDimension * dps);
    avg_matrix.set_identity();

    for (unsigned int k = 0; k < dps; k++){
        avg_matrix.put(k * VDimension, k * VDimension, sqrt(num_particles[k]));
        avg_matrix.put(k * VDimension + 1, k * VDimension + 1, sqrt(num_particles[k]));
        avg_matrix.put(k * VDimension + 2, k * VDimension + 2, sqrt(num_particles[k]));
    }

    vnl_matrix_type z_between_objective = avg_matrix * z_between_objective_temp; //---> 3K X N
    // std::cout<< "z_between_objective made" << std::endl;
    vnl_svd<double> svd_between(z_between_objective); // Do Between svd
    // std::cout << "svd done of between" << std::endl;
    m_between_loadings->set_size(N, N);
    m_between_loadings->fill(0.0);
    m_between_loadings->update(svd_between.V() * svd_between.W());  // ----> N X N    X  N X N  --> N X N   
    // std::cout << "between loadings updated" << std::endl;
    
    for (unsigned int k = 0; k < dps; k++){
        avg_matrix.put(k * VDimension, k * VDimension, 1 / sqrt(num_particles[k]));
        avg_matrix.put(k * VDimension + 1, k * VDimension + 1, 1 / sqrt(num_particles[k]));
        avg_matrix.put(k * VDimension + 2, k * VDimension + 2, 1 / sqrt(num_particles[k]));
    }


    vnl_matrix_type between_scores = avg_matrix *  svd_between.U(); //---> 3K X 3K   X  3K X N   -----> 3K X N    divide by weights 

    for(unsigned int k = 0; k < dps; k++){
        vnl_matrix_type temp_score; 
        temp_score.set_size(VDimension, N);
        temp_score.fill(0.0); // --->3 X N
        between_scores.extract(temp_score, k * VDimension, 0);
        m_between_scores->at(k) = temp_score; // ---> 3 X N
    }
    std::cout << "----ML-PCA terms calculated-----" << std::endl;
}


template <unsigned int VDimension>
typename ParticleEnsembleMlpcaEntropyFunction<VDimension>::VectorType
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::EvaluateWithinResiduals(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // NOTE: This code requires that indices be contiguous, i.e. it won't work if
    // you start deleting particles.
    // std::cout << " Evaluating within Function" << std::endl;
    const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();

    maxdt  = m_MinimumWithinEigenValue;

    VectorType gradE;
    unsigned int k = 0;
    int dom = d % DomainsPerShape;
    for (int i = 0; i < dom; i++)
        k += system->GetNumberOfParticles(i) * VDimension;
    k += idx*VDimension;

    vnl_matrix_type Xi(3,1,0.0);
    vnl_matrix_type within_part = m_within_scores->at(dom) * m_within_loadings->at(dom).transpose(); //-->3m X N
    // std::cout << " In eval within func within part calculated" << std::endl;
    // std::cout << "within part sizee :  " << within_part.rows() << " X " << within_part.cols() << std::endl;


    Xi(0,0) = m_ShapeMatrix->operator()(k  , d/DomainsPerShape) - m_grand_mean->get(0, d/DomainsPerShape) - within_part(idx*VDimension, d/DomainsPerShape) - m_points_meanAllWithin->at(dom).get(idx*VDimension, 0);
    Xi(1,0) = m_ShapeMatrix->operator()(k+1, d/DomainsPerShape) - m_grand_mean->get(1, d/DomainsPerShape) - within_part(idx*VDimension + 1, d/DomainsPerShape) - m_points_meanAllWithin->at(dom).get(idx*VDimension + 1, 0);
    Xi(2,0) = m_ShapeMatrix->operator()(k+2, d/DomainsPerShape) - m_grand_mean->get(2, d/DomainsPerShape) - within_part(idx*VDimension + 2, d/DomainsPerShape) - m_points_meanAllWithin->at(dom).get(idx*VDimension + 2, 0);

    vnl_matrix_type tmp1(3, 3, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1.set_identity();
    else
        tmp1 = m_InverseCovMatricesAllWithin->at(dom).extract(3,3, idx*VDimension, idx*VDimension);
    // std::cout << " tmp1 extracted" << std::endl;

    vnl_matrix_type tmp = Xi.transpose()*tmp1;

    tmp *= Xi;

    energy = tmp(0,0); 

    for (unsigned int i = 0; i< VDimension; i++)
    {
        gradE[i] = m_PointsUpdateAllWithin->at(dom).get(idx*VDimension + i, d / DomainsPerShape);
    }
    // std::cout << "grade e done " << std::endl;


    return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}

template <unsigned int VDimension>
typename ParticleEnsembleMlpcaEntropyFunction<VDimension>::VectorType
ParticleEnsembleMlpcaEntropyFunction<VDimension>
::EvaluateBetweenResiduals(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // NOTE: This code requires that indices be contiguous, i.e. it won't work if
    // you start deleting particles.
    const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();

    maxdt  = m_MinimumBetweenEigenValue;

    VectorType gradE;
    unsigned int k = 0;
    int dom = d % DomainsPerShape;
    for (int i = 0; i < dom; i++)
        k += system->GetNumberOfParticles(i) * VDimension;
    k += idx*VDimension;

    vnl_matrix_type Xi(3,1,0.0);
    vnl_matrix_type between_scores_part;
    between_scores_part.set_size(VDimension * system->GetNumberOfParticles(dom), m_ShapeMatrix->cols()); // 3m X N
    between_scores_part.fill(0.0);
    for(unsigned int j = 0; j < system->GetNumberOfParticles(dom); j++){
        between_scores_part.update(m_between_scores->at(dom), j * VDimension, 0);
    }
    vnl_matrix_type between_part = between_scores_part * m_between_loadings->transpose(); //-->3m X N X N X N ---> 3m X N
    // TODO: ENSURE THIS WORKS HERE (IMP)
    Xi(0,0) = m_ShapeMatrix->operator()(k  , d/DomainsPerShape) - m_grand_mean->get(0, d/DomainsPerShape) - between_part(idx*VDimension, d/DomainsPerShape) - m_points_meanAllBetween->at(dom).get(idx*VDimension, 0);
    Xi(1,0) = m_ShapeMatrix->operator()(k+1, d/DomainsPerShape) - m_grand_mean->get(1, d/DomainsPerShape) - between_part(idx*VDimension + 1, d/DomainsPerShape) - m_points_meanAllBetween->at(dom).get(idx*VDimension + 1, 0);
    Xi(2,0) = m_ShapeMatrix->operator()(k+2, d/DomainsPerShape) - m_grand_mean->get(2, d/DomainsPerShape) - between_part(idx*VDimension + 2, d/DomainsPerShape) - m_points_meanAllBetween->at(dom).get(idx*VDimension + 2, 0);

    vnl_matrix_type tmp1(3, 3, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1.set_identity();
    else
        tmp1 = m_InverseCovMatricesAllBetween->at(dom).extract(3,3, idx*VDimension, idx*VDimension);

    vnl_matrix_type tmp = Xi.transpose()*tmp1;

    tmp *= Xi;

    energy = tmp(0,0); 

    for (unsigned int i = 0; i< VDimension; i++)
    {
        gradE[i] = m_PointsUpdateAllBetween->at(dom).get(idx*VDimension + i, d / DomainsPerShape);
    }

    return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}


} // end namespace
#endif

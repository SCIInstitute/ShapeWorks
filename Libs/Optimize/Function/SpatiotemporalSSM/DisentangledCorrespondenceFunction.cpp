

#include "DisentangledCorrespondenceFunction.h"

#include <string>

#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "Libs/Optimize/Utils/ParticleGaussianModeWriter.h"
#include "Libs/Utils/Utils.h"
#include <tbb/parallel_for.h>
#include "vnl/algo/vnl_symmetric_eigensystem.h"

namespace shapeworks {
void DisentangledCorrespondenceFunction ::WriteModes(const std::string& prefix, int n) const {
  typename ParticleGaussianModeWriter<VDimension>::Pointer writer = ParticleGaussianModeWriter<VDimension>::New();
  writer->SetShapeMatrix(m_ShapeMatrix);
  writer->SetFileName(prefix.c_str());
  writer->SetNumberOfModes(n);
  writer->Update();
}

void DisentangledCorrespondenceFunction::ComputeCovarianceMatrices() {

  const unsigned int num_N = m_ShapeMatrix->cols(); // Total Number of subjects
  const unsigned int num_T = m_ShapeMatrix->GetDomainsPerShape(); // Total Number of Time points
  const unsigned int num_dims    = m_ShapeMatrix->rows() / num_T; // (dM X T) / T = dM
  this->Initialize();
  // computation across time cohort 
  tbb::parallel_for(tbb::blocked_range<size_t>{0, num_T}, [&](const tbb::blocked_range<size_t>& r)
  {
    // Iterate t = 1....T
    for (size_t time_inst = r.begin(); time_inst < r.end(); ++time_inst) {
      auto num_dims = num_particles_ar.at(time_inst) * VDimension;
      // Build objective matrix Z
      vnl_matrix_type z;
      z.clear();
      z.set_size(num_dims, num_N);
      z.fill(0.0);
      unsigned int dim_start = 0; // starting row index of the shape matrix for current time instance
      for (auto t = 0; t < time_inst; ++t)
      {
        dim_start += (num_particles_ar.at(t) * VDimension);
      }
      z = m_ShapeMatrix->extract(num_dims, num_N, dim_start, 0);

      // Resize Gradient Updates matrix for current time instance
      if (m_Time_PointsUpdate->at(time_inst).rows() != num_dims || m_Time_PointsUpdate->at(time_inst).cols() != num_N)
      {
        m_Time_PointsUpdate->at(time_inst).set_size(num_dims, num_N);
        m_Time_PointsUpdate->at(time_inst).fill(0.0);
      }

      // Compute mean and mean centred objective matrix for current time instance t_i
      vnl_matrix_type points_minus_mean_t_i;
      points_minus_mean_t_i.clear();
      points_minus_mean_t_i.set_size(num_dims, num_N);
      points_minus_mean_t_i.fill(0.0);
      vnl_matrix_type inv_cov_t_i;
      inv_cov_t_i.clear();

      m_points_mean_time_cohort->at(time_inst).clear();
      m_points_mean_time_cohort->at(time_inst).set_size(num_dims, 1);

      for (unsigned int j = 0; j < num_dims; ++j)
      {
        double sum_across_col = 0.0;
        for (unsigned int i = 0; i < num_N; ++i)
        {
          sum_across_col += z(j, i);
        }
        m_points_mean_time_cohort->at(time_inst).put(j,0, sum_across_col/(double)num_N);
      }

      for (unsigned int j = 0; j < num_dims; j++)
      {
        for (unsigned int i = 0; i < num_N; i++)
        {
          points_minus_mean_t_i(j, i) = z(j, i) - m_points_mean_time_cohort->at(time_inst).get(j,0);
        }
      }

      vnl_diag_matrix<double> W_t_i;
      vnl_matrix_type gramMat_t_i(num_N, num_N, 0.0); // gram matrix = Y^T X Y
      vnl_matrix_type pinvMat_t_i(num_N, num_N, 0.0); // inverse of gram Matrix

      if (this->m_UseMeanEnergy)
      {
        pinvMat_ti.set_identity();
        m_InverseCovMatrices_time_cohort->at(time_inst).clear();
      }
      else
      {
        gramMat_t_i = points_minus_mean_ti.transpose()* points_minus_mean_t_i;
        vnl_svd <double> svd(gramMat_t_i);
        vnl_matrix_type UG = svd.U();
        W_t_i = svd.W();
        vnl_diag_matrix<double> invLambda_t_i = svd.W();
        invLambda_t_i.set_diagonal(invLambda_ti.get_diagonal()/(double)(num_N-1) + m_MinimumVariance);
        invLambda_t_i.invert_in_place();

        pinvMat_t_i = (UG * invLambda_t_i) * UG.transpose();
        vnl_matrix_type projMat_t_i = points_minus_mean_t_i * UG;
        const auto lhs = projMat_t_i * invLambda_t_i;
        const auto rhs = invLambda_t_i * projMat_t_i.transpose();
        inv_cov_t_i.set_size(num_dims, num_dims);
        Utils::multiply_into(inv_cov_t_i, lhs, rhs);
      }
      // Update Gradient points update infor
      m_Time_PointsUpdate->at(time_inst).update(points_minus_mean_t_i * pinvMat_t_i);
      double currentEnergy_t_i = 0.0;
      if (m_UseMeanEnergy) currentEnergy_t_i = points_minus_mean_ti.frobenius_norm();
      else
      {
        m_MinimumEigenValue_time_cohort[time_inst] = W_ti(0)*W_ti(0) + m_MinimumVariance;
        for (unsigned int i = 0; i < num_N; i++)
        {
          double val_i = W_ti(i)*W_ti(i) + m_MinimumVariance;
          if ( val_i < m_MinimumEigenValue_time_cohort[time_inst])
            m_MinimumEigenValue_time_cohort[time_inst] = val_i;
          currentEnergy_t_i += log(val_i);
        }
      }
      currentEnergy_t_i /= 2.0;
      if (m_UseMeanEnergy) m_MinimumEigenValue_time_cohort[time_inst] = currentEnergy_t_i / 2.0;
      // Update Inv Covariance Matrix
      m_InverseCovMatrices_time_cohort->at(time_inst) = inv_cov_t_i;
    }
  });

  // computation across shape cohort 
  tbb::parallel_for(tbb::blocked_range<size_t>{0, num_N}, [&](const tbb::blocked_range<size_t>& r)
  {
    // Iterate n = 1....N
    for (size_t sub = r.begin(); sub < r.end(); ++sub) {
      auto num_dims = num_particles_ar.at(time_inst) * VDimension;
      // Build objective matrix Z
      vnl_matrix_type z;
      z.clear();
      z.set_size(num_dims, num_N);
      z.fill(0.0);
      for(unsigned int t = 0; t < num_T; ++t){
        unsigned int row_start = num_dims * t;
        vnl_matrix_type time_vec = m_ShapeMatrix->extract(num_dims, 1, row_start, sub);
        z.set_columns(t, time_vec);
      }

      // Resize Gradient Updates matrix for current time instance
      if (m_Shape_PointsUpdate->at(sub).rows() != num_dims || m_Shape_PointsUpdate->at(sub).cols() != num_T)
      {
        m_Shape_PointsUpdate->at(sub).set_size(num_dims, num_N);
        m_Shape_PointsUpdate->at(sub).fill(0.0);
      }

      // Compute mean and mean centred objective matrix for current time instance t_i
      vnl_matrix_type points_minus_mean_n;
      points_minus_mean_n.clear();
      points_minus_mean_n.set_size(num_dims, num_T);
      points_minus_mean_n.fill(0.0);
      vnl_matrix_type inv_cov_n;
      inv_cov_n.clear();

      m_points_mean_shape_cohort->at(sub).clear();
      m_points_mean_shape_cohort->at(sub).set_size(num_dims, 1);

      for (unsigned int j = 0; j < num_dims; ++j)
      {
        double sum_across_col = 0.0;
        for (unsigned int i = 0; i < num_N; ++i)
        {
          sum_across_col += z(j, i);
        }
        m_points_mean_shape_cohort->at(sub).put(j,0, sum_across_col/(double)num_N);
      }

      for (unsigned int j = 0; j < num_dims; j++)
      {
        for (unsigned int i = 0; i < num_T; i++)
        {
          points_minus_mean_t_i(j, i) = z(j, i) - m_points_mean_shape_cohort->at(sub).get(j,0);
        }
      }

      vnl_diag_matrix<double> W_n;
      vnl_matrix_type gramMat_n(num_T, num_T, 0.0); // gram matrix = Y^T X Y
      vnl_matrix_type pinvMat_n(num_T, num_T, 0.0); // inverse of gram Matrix

      if (this->m_UseMeanEnergy)
      {
        pinvMat_n.set_identity();
        m_InverseCovMatrices_shape_cohort->at(sub).clear();
      }
      else
      {
        gramMat_t_i = points_minus_mean_n.transpose()* points_minus_mean_n;
        vnl_svd <double> svd(gramMat_n);
        vnl_matrix_type UG = svd.U();
        W_n = svd.W();
        vnl_diag_matrix<double> invLambda_n = svd.W();
        invLambda_n.set_diagonal(invLambda_n.get_diagonal()/(double)(num_T-1) + m_MinimumVariance);
        invLambda_n.invert_in_place();

        pinvMat_n = (UG * invLambda_n) * UG.transpose();
        vnl_matrix_type projMat_n = points_minus_mean_n * UG;
        const auto lhs = projMat_n * invLambda_n;
        const auto rhs = invLambda_n * projMat_n.transpose();
        inv_cov_n.set_size(num_dims, num_dims);
        Utils::multiply_into(inv_cov_n, lhs, rhs);
      }

      // Update Gradient points update infor
      m_Shape_PointsUpdate->at(sub).update(points_minus_mean_n * pinvMat_n);
      double currentEnergy_n = 0.0;
      if (m_UseMeanEnergy) currentEnergy_n = points_minus_mean_n.frobenius_norm();
      else
      {
        m_MinimumEigenValue_shape_cohort[sub] = W_ti(0)*W_ti(0) + m_MinimumVariance;
        for (unsigned int i = 0; i < num_N; i++)
        {
          double val_i = W_ti(i)*W_ti(i) + m_MinimumVariance;
          if ( val_i < m_MinimumEigenValue_shape_cohort[sub])
            m_MinimumEigenValue_shape_cohort[sub] = val_i;
          currentEnergy_n += log(val_i);
        }
      }
      currentEnergy_n /= 2.0;
      if (m_UseMeanEnergy) m_MinimumEigenValue_shape_cohort[sub] = currentEnergy_n / 2.0;
      // Update Inv Covariance Matrix
      m_InverseCovMatrices_shape_cohort->at(sub) = inv_cov_n;
    }
  });

}

DisentangledCorrespondenceFunction::VectorType DisentangledCorrespondenceFunction ::Evaluate(unsigned int idx, unsigned int d,
                                                                                       const ParticleSystem* system,
                                                                                       double& maxdt,
                                                                                       double& energy) const {

  const unsigned int num_N = m_ShapeMatrix->cols(); // Total number of subjects
  const unsigned int num_T = m_ShapeMatrix->GetDomainsPerShape(); // Total number of time points
    
  const unsigned int cur_sub = d / num_T; // index of current subject
  const unsigned int cur_time_point = d % num_T;

  // maximum update possible = sum of max possible updates across both cohorts (time and shape)
  maxdt  = m_MinimumEigenValue_shape_cohort[cur_sub] + m_MinimumEigenValue_time_cohort[cur_time_point];

  VectorType gradE; // gradient update vector for Point defined by ParticleSystem system of domain d and dimension index idx
  unsigned int shape_matrix_start_idx = 0;

  int dom = d % num_T;
  for (int i = 0; i < dom; i++)
    shape_matrix_start_idx += system->GetNumberOfParticles(i) * VDimension;
  shape_matrix_start_idx += idx*VDimension;

  unsigned int particle_idx = VDimension * idx;

  // Energy computation across time cohort
  vnl_matrix_type Xi_time_cohort(3,1,0.0);
  Xi_time_cohort(0,0) = m_ShapeMatrix->operator()(shape_matrix_start_idx  , cur_sub) - m_points_mean_time_cohort->at(cur_time_point).get(particle_idx, 0);
  Xi_time_cohort(1,0) = m_ShapeMatrix->operator()(shape_matrix_start_idx+1, cur_sub) - m_points_mean_time_cohort->at(cur_time_point).get(particle_idx+1, 0);
  Xi_time_cohort(2,0) = m_ShapeMatrix->operator()(shape_matrix_start_idx+2, cur_sub) - m_points_mean_time_cohort->at(cur_time_point).get(particle_idx+2, 0);
  vnl_matrix_type tmp1_time(3, 3, 0.0);
  if (this->m_UseMeanEnergy)
    tmp1_time.set_identity();
  else
    tmp1_time = m_InverseCovMatrices_time_cohort->at(cur_time_point).extract(3,3,particle_idx, particle_idx);
  vnl_matrix_type tmp_time = Xi_time_cohort.transpose()*tmp1_time;
  tmp_time *= Xi_time_cohort;

  // Energy computation across shape cohort
  vnl_matrix_type Xi_shape_cohort(3,1,0.0);
  Xi_shape_cohort(0,0) = m_ShapeMatrix->operator()(shape_matrix_start_idx  , cur_sub) - m_points_mean_shape_cohort->at(cur_sub).get(particle_idx, 0);
  Xi_shape_cohort(1,0) = m_ShapeMatrix->operator()(shape_matrix_start_idx+1, cur_sub) - m_points_mean_shape_cohort->at(cur_sub).get(particle_idx+1, 0);
  Xi_shape_cohort(2,0) = m_ShapeMatrix->operator()(shape_matrix_start_idx+2, cur_sub) - m_points_mean_shape_cohort->at(cur_sub).get(particle_idx+2, 0);
  vnl_matrix_type tmp1_shape(3, 3, 0.0);
  if (this->m_UseMeanEnergy)
    tmp1_shape.set_identity();
  else
    tmp1_shape = m_InverseCovMatrices_shape_cohort->at(cur_sub).extract(3,3,particle_idx, particle_idx);
  vnl_matrix_type tmp_shape = Xi_shape_cohort.transpose()*tmp1_shape;
  tmp_shape *= Xi_shape_cohort;

  // Net Energy
  energy = tmp_time(0,0) + tmp_shape(0, 0); 

  // Net Gradient
  for (unsigned int i = 0; i< VDimension; i++)
  {
    gradE[i] = m_Time_PointsUpdate->at(cur_time_point).get(particle_idx + i, cur_sub) + m_Shape_PointsUpdate->at(cur_sub).get(particle_idx + i, cur_time_point);
  }
  return system->TransformVector(gradE,
                                   system->GetInversePrefixTransform(d) *
                                   system->GetInverseTransform(d));
}

}  // namespace shapeworks

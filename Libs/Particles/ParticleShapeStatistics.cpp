
#include "ParticleShapeStatistics.h"

#include <Logging.h>
#include <Particles/ParticleFile.h>
#include <Project/Project.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#include "ExternalLibs/tinyxml/tinyxml.h"
#include "ShapeEvaluation.h"

namespace shapeworks {

//---------------------------------------------------------------------------
int ParticleShapeStatistics::simple_linear_regression(const std::vector<double>& y, const std::vector<double>& x,
                                                      double& a, double& b) {
  if (x.size() != y.size()) {
    return -1;
  }

  double xmean = 0.0;
  double ymean = 0.0;
  double cross = 0.0;
  double xvar = 0.0;
  double n = static_cast<double>(y.size());

  for (unsigned int i = 0; i < y.size(); i++) {
    xmean += x[i];
    ymean += y[i];
  }
  xmean /= n;
  ymean /= n;

  for (unsigned int i = 0; i < y.size(); i++) {
    double xm = x[i] - xmean;
    cross += xm * (y[i] - ymean);
    xvar += xm * xm;
  }

  b = cross / xvar;
  a = ymean - (b * xmean);

  return 0;
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::compute_median_shape(const int ID) {
  int ret = -1;
  double min_L1 = 1.0e300;
  // Compile list of indices for groupIDs == ID
  std::vector<unsigned int> set;
  for (unsigned int i = 0; i < group_ids_.size(); i++) {
    if (group_ids_[i] == ID || ID == -32)  // -32 means use both groups
    {
      set.push_back(i);
    }
  }

  // Find min sum L1 norms
  for (unsigned int i = 0; i < set.size(); i++) {
    double sum = 0.0;

    for (unsigned int j = 0; j < set.size(); j++) {
      if (i != j) {
        sum += this->l1_norm(set[i], set[j]);
      }
    }
    if (sum < min_L1) {
      min_L1 = sum;
      ret = static_cast<int>(set[i]);
    }
  }
  return ret;  // if there has been some error ret == -1
}

//---------------------------------------------------------------------------
double ParticleShapeStatistics::l1_norm(unsigned int a, unsigned int b) {
  double norm = 0.0;
  for (unsigned int i = 0; i < shapes_.rows(); i++) {
    norm += fabs(shapes_(i, a) - shapes_(i, b));
  }
  return norm;
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::import_points(std::vector<Eigen::VectorXd> points, std::vector<int> group_ids) {
  // local copy of points
  points_ = points;

  group_ids_ = group_ids;
  domains_per_shape_ = 1;

  int num_points = points[0].size() / values_per_particle_;

  // Read the point files.  Assumes all the same size.
  num_samples_group1_ = 0;
  num_samples_group2_ = 0;
  num_samples_ = points.size() / domains_per_shape_;
  num_dimensions_ = num_points * values_per_particle_ * domains_per_shape_;

  // If there are no group IDs, make up some bogus ones
  if (group_ids_.size() != num_samples_) {
    if (group_ids_.size() > 0) {
      std::cerr << "Group ID list does not match shape list in size." << std::endl;
      return 1;
    }

    group_ids_.resize(num_samples_);
    for (unsigned int k = 0; k < num_samples_ / 2; k++) {
      group_ids_[k] = 1;
      num_samples_group1_++;
    }
    for (unsigned int k = num_samples_ / 2; k < num_samples_; k++) {
      group_ids_[k] = 2;
      num_samples_group2_++;
    }
  } else {
    for (int i = 0; i < group_ids_.size(); i++) {
      if (group_ids_[i] == 1) {
        num_samples_group1_++;
      } else {
        num_samples_group2_++;
      }
    }
  }

  points_minus_mean_.resize(num_dimensions_, num_samples_);
  points_minus_mean_.fill(0);
  shapes_.resize(num_dimensions_, num_samples_);
  mean_.resize(num_dimensions_);
  mean_.fill(0);

  mean1_.resize(num_dimensions_);
  mean1_.fill(0);
  mean2_.resize(num_dimensions_);
  mean2_.fill(0);

  // Compile the "meta shapes"
  for (unsigned int i = 0; i < num_samples_; i++) {
    for (unsigned int k = 0; k < domains_per_shape_; k++) {
      unsigned int q = points[i].size();
      for (unsigned int j = 0; j < q; j++) {
        points_minus_mean_(q * k * values_per_particle_ + j, i) = points[i][j];

        mean_(q * k * values_per_particle_ + j) += points[i][j];

        if (group_ids_[i] == 1) {
          mean1_(q * k * values_per_particle_ + j) += points[i][j];
        } else {
          mean2_(q * k * values_per_particle_ + j) += points[i][j];
        }

        shapes_(q * k * values_per_particle_ + j, i) = points[i][j];
      }
    }
  }

  for (unsigned int i = 0; i < num_dimensions_; i++) {
    mean_(i) /= (double)num_samples_;
    mean1_(i) /= (double)num_samples_group1_;
    mean2_(i) /= (double)num_samples_group2_;
  }

  for (unsigned int j = 0; j < num_dimensions_; j++) {
    for (unsigned int i = 0; i < num_samples_; i++) {
      points_minus_mean_(j, i) -= mean_(j);
    }
  }

  groupdiff_ = mean2_ - mean1_;

  // copy to Eigen matrix
  matrix_.resize(num_points * values_per_particle_, num_samples_);
  group1_matrix_.resize(num_points * values_per_particle_, num_samples_group1_);
  group2_matrix_.resize(num_points * values_per_particle_, num_samples_group2_);
  int group1_idx = 0;
  int group2_idx = 0;
  for (unsigned int i = 0; i < num_samples_; i++) {
    unsigned int q = points[i].size();
    for (unsigned int j = 0; j < q; j++) {
      matrix_(j, i) = points[i][j];

      if (group_ids_[i] == 1) {
        group1_matrix_(j, group1_idx) = points[i][j];
      } else {
        group2_matrix_(j, group2_idx) = points[i][j];
      }
    }
    if (group_ids_[i] == 1) {
      group1_idx++;
    } else {
      group2_idx++;
    }
  }

  return 0;
}

//---------------------------------------------------------------------------
void ParticleShapeStatistics::set_num_particles_per_domain(const std::vector<int>& num_particles_array) {
  num_particles_array_ = num_particles_array;
}

//---------------------------------------------------------------------------
void ParticleShapeStatistics::compute_multi_level_analysis_statistics(std::vector<Eigen::VectorXd> points,
                                                                      unsigned int dps) {
  domains_per_shape_ = dps;
  num_samples_ = points.size();                          // Number of Subjects
  unsigned int n = num_samples_ * values_per_particle_;  // for super matrix
  unsigned int m = 0;
  for (unsigned int idx = 0; idx < dps; idx++) {
    m += (this->num_particles_array_[idx]);
  }
  super_matrix_.resize(m, n);
  for (unsigned int i = 0; i < points.size(); i++) {
    unsigned int p = super_matrix_.rows();
    for (unsigned int j = 0; j < p; j++) {
      super_matrix_(j, i * values_per_particle_) = points[i][j * values_per_particle_];
      super_matrix_(j, i * values_per_particle_ + 1) = points[i][j * values_per_particle_ + 1];
      super_matrix_(j, i * values_per_particle_ + 2) = points[i][j * values_per_particle_ + 2];
    }
  }

  Eigen::RowVectorXd grand_mean;
  grand_mean.resize(n);
  grand_mean.fill(0.0);
  grand_mean = super_matrix_.colwise().mean();
  Eigen::MatrixXd z_shape_dev_centered;
  Eigen::MatrixXd z_rel_pose_centered;
  z_shape_dev_centered.resize(m, n);
  z_rel_pose_centered.resize(domains_per_shape_, n);
  z_shape_dev_centered.fill(0.0);
  z_rel_pose_centered.fill(0.0);
  for (unsigned int k = 0; k < domains_per_shape_; k++) {
    unsigned int row = 0;
    for (unsigned int idx = 0; idx < k; idx++) {
      row += this->num_particles_array_[idx];
    }
    Eigen::MatrixXd z_k = super_matrix_.block(row, 0, this->num_particles_array_[k], n);
    Eigen::RowVectorXd mean_k;
    mean_k.resize(n);
    mean_k.fill(0.0);
    mean_k = z_k.colwise().mean();
    for (unsigned int x = 0; x < n; x++) {
      z_rel_pose_centered(k, x) = mean_k(x) - grand_mean(x);
    }

    Eigen::MatrixXd z_shape_dev_centered_k;
    z_shape_dev_centered_k.resize(this->num_particles_array_[k], n);
    z_shape_dev_centered_k.fill(0.0);
    Eigen::RowVectorXd diff_vec;
    diff_vec = mean_k;
    z_shape_dev_centered_k = z_k.rowwise() - diff_vec;
    z_shape_dev_centered.block(row, 0, z_shape_dev_centered_k.rows(), z_shape_dev_centered_k.cols()) =
        z_shape_dev_centered_k;
  }
  Eigen::MatrixXd z_shape_dev_objective;
  unsigned int M = 0;
  for (unsigned int idx = 0; idx < dps; idx++) {
    M += (this->num_particles_array_[idx] * values_per_particle_);
  }
  z_shape_dev_objective.resize(M, num_samples_);
  for (unsigned int i = 0; i < num_samples_; i++) {
    unsigned int p = m;
    for (unsigned int j = 0; j < p; j++) {
      z_shape_dev_objective(j * values_per_particle_, i) = z_shape_dev_centered(j, i * values_per_particle_);
      z_shape_dev_objective(j * values_per_particle_ + 1, i) = z_shape_dev_centered(j, i * values_per_particle_ + 1);
      z_shape_dev_objective(j * values_per_particle_ + 2, i) = z_shape_dev_centered(j, i * values_per_particle_ + 2);
    }
  }
  points_minus_mean_shape_dev_.resize(M, num_samples_);
  points_minus_mean_shape_dev_.fill(0.0);
  mean_shape_dev_ = z_shape_dev_objective.rowwise().mean();
  points_minus_mean_shape_dev_ = z_shape_dev_objective.colwise() - mean_shape_dev_;

  Eigen::MatrixXd z_rel_pose_objective;
  z_rel_pose_objective.resize(domains_per_shape_ * values_per_particle_, num_samples_);
  for (unsigned int k = 0; k < domains_per_shape_; k++) {
    for (unsigned int i = 0; i < num_samples_; i++) {
      z_rel_pose_objective(k * values_per_particle_, i) = z_rel_pose_centered(k, i * values_per_particle_);
      z_rel_pose_objective(k * values_per_particle_ + 1, i) = z_rel_pose_centered(k, i * values_per_particle_ + 1);
      z_rel_pose_objective(k * values_per_particle_ + 2, i) = z_rel_pose_centered(k, i * values_per_particle_ + 2);
    }
  }
  points_minus_mean_rel_pose_.resize(domains_per_shape_ * values_per_particle_, num_samples_);
  points_minus_mean_rel_pose_.fill(0.0);
  mean_rel_pose_ = z_rel_pose_objective.rowwise().mean();
  points_minus_mean_rel_pose_ = z_rel_pose_objective.colwise() - mean_rel_pose_;
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::compute_shape_dev_modes_for_mca() {
  unsigned int m = points_minus_mean_shape_dev_.rows();
  Eigen::MatrixXd A =
      points_minus_mean_shape_dev_.transpose() * points_minus_mean_shape_dev_ * (1.0 / ((double)(num_samples_ - 1)));

  vnl_matrix<double> vnlA = vnl_matrix<double>(A.data(), A.rows(), A.cols());
  vnl_symmetric_eigensystem<double> symEigen(vnlA);
  Eigen::MatrixXd shape_dev_eigenSymEigenV =
      Eigen::Map<Eigen::MatrixXd>(symEigen.V.transpose().data_block(), symEigen.V.rows(), symEigen.V.cols());
  Eigen::VectorXd shape_dev_eigenSymEigenD = Eigen::Map<Eigen::VectorXd>(symEigen.D.data_block(), symEigen.D.rows(), 1);

  eigenvectors_shape_dev_ = points_minus_mean_shape_dev_ * shape_dev_eigenSymEigenV;
  eigenvalues_shape_dev_.resize(num_samples_);

  for (unsigned int i = 0; i < num_samples_; i++) {
    double total = 0.0f;
    for (unsigned int j = 0; j < m; j++) {
      total += eigenvectors_shape_dev_(j, i) * eigenvectors_shape_dev_(j, i);
    }
    total = sqrt(total);

    for (unsigned int j = 0; j < m; j++) {
      eigenvectors_shape_dev_(j, i) = eigenvectors_shape_dev_(j, i) / (total + 1.0e-15);
    }
    eigenvalues_shape_dev_[i] = shape_dev_eigenSymEigenD(i);
  }
  return 0;
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::compute_relative_pose_modes_for_mca() {
  unsigned int m = points_minus_mean_rel_pose_.rows();
  Eigen::MatrixXd A =
      points_minus_mean_rel_pose_.transpose() * points_minus_mean_rel_pose_ * (1.0 / ((double)(num_samples_ - 1)));
  auto vnlA = vnl_matrix<double>(A.data(), A.rows(), A.cols());
  vnl_symmetric_eigensystem<double> symEigen(vnlA);
  Eigen::MatrixXd rel_pose_eigenSymEigenV =
      Eigen::Map<Eigen::MatrixXd>(symEigen.V.transpose().data_block(), symEigen.V.rows(), symEigen.V.cols());

  Eigen::VectorXd rel_pose_eigenSymEigenD = Eigen::Map<Eigen::VectorXd>(symEigen.D.data_block(), symEigen.D.rows(), 1);

  eigenvectors_rel_pose_ = points_minus_mean_rel_pose_ * rel_pose_eigenSymEigenV;
  eigenvalues_rel_pose_.resize(num_samples_);

  for (unsigned int i = 0; i < num_samples_; i++) {
    double total = 0.0f;
    for (unsigned int j = 0; j < m; j++) {
      total += eigenvectors_rel_pose_(j, i) * eigenvectors_rel_pose_(j, i);
    }
    total = sqrt(total);

    for (unsigned int j = 0; j < m; j++) {
      eigenvectors_rel_pose_(j, i) = eigenvectors_rel_pose_(j, i) / (total + 1.0e-15);
    }

    eigenvalues_rel_pose_[i] = rel_pose_eigenSymEigenD(i);
  }
  return 0;
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::read_point_files(const std::string& s) {
  TiXmlDocument doc(s.c_str());
  bool loadOkay = doc.LoadFile();
  if (!loadOkay) std::cerr << "invalid parameter file..." << std::endl;
  TiXmlHandle docHandle(&doc);
  TiXmlElement* elem;
  std::stringstream inputsBuffer;

  // Collect point file names and group id's
  std::vector<std::string> pointsfiles;
  std::string ptFileName;
  elem = docHandle.FirstChild("point_files").Element();
  if (elem) {
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> ptFileName) {
      pointsfiles.push_back(ptFileName);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  this->domains_per_shape_ = 1;
  elem = docHandle.FirstChild("domains_per_shape").Element();
  if (elem) this->domains_per_shape_ = atoi(elem->GetText());

  // Read the point files.  Assumes all the same size.
  num_samples_group1_ = 0;
  num_samples_group2_ = 0;
  num_samples_ = pointsfiles.size() / domains_per_shape_;
  num_dimensions_ =
      particles::read_particles_as_vector(pointsfiles[0]).size() * values_per_particle_ * domains_per_shape_;

  // Read the group ids
  int tmpID;
  elem = docHandle.FirstChild("group_ids").Element();
  if (elem) {
    inputsBuffer.str(elem->GetText());
    for (unsigned int shapeCount = 0; shapeCount < num_samples_; shapeCount++) {
      inputsBuffer >> tmpID;
      group_ids_.push_back(tmpID);
      if (tmpID == 1) {
        num_samples_group1_++;
      } else {
        num_samples_group2_++;
      }
    }
  }

  // If there are no group IDs, make up some bogus ones
  if (group_ids_.size() != num_samples_) {
    if (group_ids_.size() > 0) {
      std::cerr << "Group ID list does not match shape list in size." << std::endl;
      return 1;
    }

    group_ids_.resize(num_samples_);
    for (unsigned int k = 0; k < num_samples_ / 2; k++) {
      group_ids_[k] = 1;
      num_samples_group1_++;
    }
    for (unsigned int k = num_samples_ / 2; k < num_samples_; k++) {
      group_ids_[k] = 2;
      num_samples_group2_++;
    }
  };

  points_minus_mean_.resize(num_dimensions_, num_samples_);
  shapes_.resize(num_dimensions_, num_samples_);
  mean_.resize(num_dimensions_);
  mean_.fill(0);

  mean1_.resize(num_dimensions_);
  mean1_.fill(0);
  mean2_.resize(num_dimensions_);
  mean2_.fill(0);

  // Compile the "meta shapes"
  for (unsigned int i = 0; i < num_samples_; i++) {
    for (unsigned int k = 0; k < domains_per_shape_; k++) {
      // read file
      auto points = particles::read_particles_as_vector(pointsfiles[i * domains_per_shape_ + k]);
      unsigned int q = points.size();
      for (unsigned int j = 0; j < q; j++) {
        mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 0) +=
            points_minus_mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 0, i) = points[j][0];
        mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 1) +=
            points_minus_mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 1, i) = points[j][1];
        mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 2) +=
            points_minus_mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 2, i) = points[j][2];

        if (group_ids_[i] == 1) {
          mean1_(q * k * values_per_particle_ + (values_per_particle_ * j) + 0) += points[j][0];
          mean1_(q * k * values_per_particle_ + (values_per_particle_ * j) + 1) += points[j][1];
          mean1_(q * k * values_per_particle_ + (values_per_particle_ * j) + 2) += points[j][2];
        } else {
          mean2_(q * k * values_per_particle_ + (values_per_particle_ * j) + 0) += points[j][0];
          mean2_(q * k * values_per_particle_ + (values_per_particle_ * j) + 1) += points[j][1];
          mean2_(q * k * values_per_particle_ + (values_per_particle_ * j) + 2) += points[j][2];
        }

        shapes_(q * k * values_per_particle_ + (values_per_particle_ * j) + 0, i) = points[j][0];
        shapes_(q * k * values_per_particle_ + (values_per_particle_ * j) + 1, i) = points[j][1];
        shapes_(q * k * values_per_particle_ + (values_per_particle_ * j) + 2, i) = points[j][2];
      }
    }
  }

  for (unsigned int i = 0; i < num_dimensions_; i++) {
    mean_(i) /= (double)num_samples_;
    mean1_(i) /= (double)num_samples_group1_;
    mean2_(i) /= (double)num_samples_group2_;
  }

  for (unsigned int j = 0; j < num_dimensions_; j++) {
    for (unsigned int i = 0; i < num_samples_; i++) {
      points_minus_mean_(j, i) -= mean_(j);
    }
  }

  groupdiff_ = mean2_ - mean1_;

  return 0;
}

//---------------------------------------------------------------------------
ParticleShapeStatistics::ParticleShapeStatistics(std::shared_ptr<Project> project) {
  std::vector<Eigen::VectorXd> points;
  std::vector<int> groups;
  for (auto& s : project->get_subjects()) {
    auto world_files = s->get_world_particle_filenames();
    Eigen::VectorXd particles;
    for (auto& file : world_files) {
      Eigen::VectorXd domain_particles;
      ParticleSystemEvaluation::read_particle_file(file, domain_particles);
      Eigen::VectorXd combined(particles.size() + domain_particles.size());
      combined << particles, domain_particles;
      particles = combined;
    }
    points.push_back(particles);
    groups.push_back(1);
  }
  import_points(points, groups);
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::do_pca(std::vector<std::vector<Point>> global_pts, int domainsPerShape) {
  this->domains_per_shape_ = domainsPerShape;

  // Assumes all the same size.
  num_samples_ = global_pts.size() / domains_per_shape_;
  num_dimensions_ = global_pts[0].size() * values_per_particle_ * domains_per_shape_;

  points_minus_mean_.resize(num_dimensions_, num_samples_);
  shapes_.resize(num_dimensions_, num_samples_);
  mean_.resize(num_dimensions_);
  mean_.fill(0);

  std::cout << "VDimension = " << values_per_particle_ << "-------------\n";
  std::cout << "m_numSamples = " << num_samples_ << "-------------\n";
  std::cout << "m_domainsPerShape = " << domains_per_shape_ << "-------------\n";
  std::cout << "global_pts.size() = " << global_pts.size() << "-------------\n";

  // Compile the "meta shapes"
  for (unsigned int i = 0; i < num_samples_; i++) {
    for (unsigned int k = 0; k < domains_per_shape_; k++) {
      // std::cout << "i*m_domainsPerShape + k = " << i*m_domainsPerShape + k << "-------------\n";
      std::vector<Point> curDomain = global_pts[i * domains_per_shape_ + k];
      unsigned int q = curDomain.size();

      // std::cout << "q = " << q << "-------------\n";
      for (unsigned int j = 0; j < q; j++) {
        mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 0) +=
            points_minus_mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 0, i) = curDomain[j][0];
        mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 1) +=
            points_minus_mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 1, i) = curDomain[j][1];
        mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 2) +=
            points_minus_mean_(q * k * values_per_particle_ + (values_per_particle_ * j) + 2, i) = curDomain[j][2];

        shapes_(q * k * values_per_particle_ + (values_per_particle_ * j) + 0, i) = curDomain[j][0];
        shapes_(q * k * values_per_particle_ + (values_per_particle_ * j) + 1, i) = curDomain[j][1];
        shapes_(q * k * values_per_particle_ + (values_per_particle_ * j) + 2, i) = curDomain[j][2];
      }
    }
  }

  for (unsigned int i = 0; i < num_dimensions_; i++) {
    mean_(i) /= (double)num_samples_;
  }

  for (unsigned int j = 0; j < num_dimensions_; j++) {
    for (unsigned int i = 0; i < num_samples_; i++) {
      points_minus_mean_(j, i) -= mean_(j);
    }
  }

  compute_modes();
  return 0;
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::do_pca(ParticleSystemEvaluation ParticleSystemEvaluation, int domainsPerShape) {
  Eigen::MatrixXd p = ParticleSystemEvaluation.get_matrix();

  std::vector<std::vector<Point>> particlePoints;

  for (int i = 0; i < p.cols(); i++) {
    std::vector<Point> particle;
    for (int j = 0; j < p.rows() / 3; j++) {
      Point point;
      for (int k = 0; k < 3; k++) {
        point[k] = p.coeff(j * 3 + k, i);
      }
      particle.push_back(point);
    }
    particlePoints.push_back(particle);
  }

  return do_pca(particlePoints, domainsPerShape);
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::compute_modes() {
  SW_DEBUG("computing PCA modes");
  Eigen::MatrixXd A = points_minus_mean_.transpose() * points_minus_mean_ * (1.0 / ((double)(num_samples_ - 1)));

  auto vnlA = vnl_matrix<double>(A.data(), A.rows(), A.cols());
  vnl_symmetric_eigensystem<double> symEigen(vnlA);

  Eigen::MatrixXd eigenSymEigenV =
      Eigen::Map<Eigen::MatrixXd>(symEigen.V.transpose().data_block(), symEigen.V.rows(), symEigen.V.cols());
  Eigen::VectorXd eigenSymEigenD = Eigen::Map<Eigen::VectorXd>(symEigen.D.data_block(), symEigen.D.rows(), 1);

  eigenvectors_ = points_minus_mean_ * eigenSymEigenV;
  eigenvalues_.resize(num_samples_);

  // normalize the eigenvectors
  for (unsigned int i = 0; i < num_samples_; i++) {
    double total = 0.0f;
    for (unsigned int j = 0; j < num_dimensions_; j++) {
      total += eigenvectors_(j, i) * eigenvectors_(j, i);
    }
    total = sqrt(total);

    for (unsigned int j = 0; j < num_dimensions_; j++) {
      eigenvectors_(j, i) = eigenvectors_(j, i) / (total + 1.0e-15);
    }

    eigenvalues_[i] = eigenSymEigenD(i);
  }

  float sum = 0.0;
  for (unsigned int n = 0; n < num_samples_; n++) {
    sum += eigenvalues_[(num_samples_ - 1) - n];
  }

  float sum2 = 0.0;
  bool found = false;
  for (unsigned int n = 0; n < num_samples_; n++) {
    sum2 += eigenvalues_[(num_samples_ - 1) - n];
    percent_variance_by_mode_.push_back(sum2 / sum);

    if ((sum2 / sum) >= 0.95 && found == false) {
      found = true;
    }
  }

  return 0;
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::get_num_modes() const { return num_samples_ - 1; }

//---------------------------------------------------------------------------
int ParticleShapeStatistics::principal_component_projections() {
  // Now print the projection of each shape
  principals_.resize(num_samples_, num_samples_);

  for (unsigned int n = 0; n < num_samples_; n++) {
    for (unsigned int s = 0; s < num_samples_; s++) {
      double p = eigenvectors_.col((num_samples_ - 1) - n).dot(points_minus_mean_.col(s));
      principals_(s, n) = p;  // each row is a sample, columns index PC
    }
  }

  return 0;
}

//---------------------------------------------------------------------------
int ParticleShapeStatistics::write_csv_file(const std::string& s) {
  // Write csv file
  std::ofstream outfile;
  outfile.open(s.c_str());
  if (!outfile.good()) {
    throw std::runtime_error("Unable to open " + s + " for writing");
  }

  outfile << "Group";
  for (unsigned int i = 0; i < num_samples_; i++) {
    outfile << ",P" << i;
  }
  outfile << "\n";

  for (unsigned int r = 0; r < num_samples_; r++) {
    outfile << group_ids_[r];
    for (unsigned int c = 0; c < num_samples_; c++) {
      outfile << "," << principals_(r, c);
    }
    outfile << "\n";
  }

  outfile.close();
  return 0;
}

//---------------------------------------------------------------------------
Eigen::VectorXd ParticleShapeStatistics::get_compactness(const std::function<void(float)>& progress_callback) const {
  auto ps = ParticleSystemEvaluation(matrix_);
  return shapeworks::ShapeEvaluation::compute_full_compactness(ps, progress_callback);
}

//---------------------------------------------------------------------------
Eigen::VectorXd ParticleShapeStatistics::get_specificity(const std::function<void(float)>& progress_callback) const {
  auto ps = ParticleSystemEvaluation(matrix_);
  ps.set_meshes(meshes_);
  return shapeworks::ShapeEvaluation::compute_full_specificity(ps, progress_callback, particle_to_surface_mode_);
}

//---------------------------------------------------------------------------
Eigen::VectorXd ParticleShapeStatistics::get_generalization(const std::function<void(float)>& progress_callback) const {
  auto ps = ParticleSystemEvaluation(matrix_);
  ps.set_meshes(meshes_);
  return shapeworks::ShapeEvaluation::compute_full_generalization(ps, progress_callback, particle_to_surface_mode_);
}

//---------------------------------------------------------------------------
Eigen::MatrixXd ParticleShapeStatistics::get_group1_matrix() const { return group1_matrix_; }

//---------------------------------------------------------------------------
Eigen::MatrixXd ParticleShapeStatistics::get_group2_matrix() const { return group2_matrix_; }

}  // namespace shapeworks

#include "ParticleSystemEvaluation.h"

#include <Particles/ParticleFile.h>

#include "ShapeEvaluation.h"
#include "Shapeworks.h"

namespace shapeworks {

//---------------------------------------------------------------------------
ParticleSystemEvaluation::ParticleSystemEvaluation(const std::vector<std::string>& paths) {
  if (paths.empty()) {
    throw std::runtime_error("No filenames passed to readParticleSystemEvaluation");
  }

  paths_ = paths;
  const int N = paths_.size();
  const int VDimension = 3;
  assert(N > 0);

  // Read the first file to find dimensions
  auto points0 = particles::read_particles_as_vector(paths_[0]);
  const int D = points0.size() * VDimension;

  matrix_.resize(D, N);
  matrix_.col(0) = Eigen::Map<const Eigen::VectorXd>((double*)points0.data(), D);

  for (int i = 1; i < N; i++) {
    auto points = particles::read_particles_as_vector(paths_[i]);
    int count = points.size() * VDimension;
    if (count != D) {
      throw std::runtime_error("ParticleSystemEvaluation files must have the same number of particles");
    }
    matrix_.col(i) = Eigen::Map<const Eigen::VectorXd>((double*)points.data(), D);
  }
}

//---------------------------------------------------------------------------
ParticleSystemEvaluation::ParticleSystemEvaluation(const Eigen::MatrixXd& matrix) { matrix_ = matrix; }

//---------------------------------------------------------------------------
bool ParticleSystemEvaluation::exact_compare(const ParticleSystemEvaluation& other) const {
  if (matrix_.rows() != other.matrix_.rows() || matrix_.cols() != other.matrix_.cols()) {
    std::cerr << "Rows/Columns mismatch\n";
    return false;
  }
  bool same = true;
  for (int r = 0; r < matrix_.rows(); r++) {
    for (int c = 0; c < matrix_.cols(); c++) {
      if (!epsEqual(matrix_(r, c), other.matrix_(r, c), 0.001)) {
        std::cerr << "(" << r << "," << c << "): " << matrix_(r, c) << " vs " << other.matrix_(r, c) << "\n";
        same = false;
      }
    }
  }
  return same;
}

//---------------------------------------------------------------------------
bool ParticleSystemEvaluation::evaluation_compare(const ParticleSystemEvaluation& other) const {
  auto compactness1 = ShapeEvaluation::compute_full_compactness(*this);
  auto compactness2 = ShapeEvaluation::compute_full_compactness(other);
  if (compactness1.size() != compactness2.size()) {
    std::cout << "Shape models have a different number of modes: " << compactness1.size() << " and "
              << compactness2.size() << "\n";
    return false;
  }

  bool good = true;
  if (compactness1.size() > 0 && compactness2.size() > 0) {
    std::cout << "Comparing compactness: " << compactness1[0] << " vs " << compactness2[0] << ": ";
    if (!epsEqual(compactness1[0], compactness2[0], 0.07)) {
      std::cout << "different\n";
      good = false;
    } else {
      std::cout << "ok\n";
    }
  }

  auto gen1 = ShapeEvaluation::compute_full_generalization(*this);
  auto gen2 = ShapeEvaluation::compute_full_generalization(other);
  if (gen1.size() != gen2.size()) {
    return false;
  }
  if (gen1.size() > 0 && gen2.size() > 0) {
    std::cout << "Comparing generalization: " << gen1[0] << " vs " << gen2[0] << ": ";
    double diff = std::abs(gen1[0] - gen2[0]);
    if ((diff > 0.1 * gen1[0] || diff > 0.1 * gen2[0]) && !epsEqual(gen1[0], gen2[0], 0.1)) {
      std::cout << "different (" << diff << ")\n";
      good = false;
    } else {
      std::cout << "ok\n";
    }
  }

  auto spec1 = ShapeEvaluation::compute_full_specificity(*this);
  auto spec2 = ShapeEvaluation::compute_full_specificity(other);
  if (spec1.size() != spec2.size()) {
    return false;
  }
  if (spec1.size() > 0 && spec2.size() > 0) {
    std::cout << "Comparing specificity: " << spec1[0] << " vs " << spec2[0] << ": ";
    double diff = std::abs(spec1[0] - spec2[0]);
    if (diff > 0.25 * spec1[0] || diff > 0.25 * spec2[0]) {
      std::cout << "different (" << diff << ")\n";
      good = false;
    } else {
      std::cout << "ok\n";
    }
  }

  return good;
}

//---------------------------------------------------------------------------
bool ParticleSystemEvaluation::read_particle_file(std::string filename, Eigen::VectorXd& points) {
  points = particles::read_particles(filename);
  return true;
}

//---------------------------------------------------------------------------

}  // namespace shapeworks

#include "ParticleSystemEvaluation.h"

#include "ShapeEvaluation.h"
#include "Shapeworks.h"

namespace shapeworks {

ParticleSystemEvaluation::ParticleSystemEvaluation(const std::vector<std::string>& _paths) {
  if (_paths.empty()) {
    throw std::runtime_error("No filenames passed to readParticleSystemEvaluation");
  }

  this->paths = _paths;
  const int N = paths.size();
  const int VDimension = 3;  // TODO Don't hardcode VDimension
  assert(N > 0);

  // TODO: We're using the existing particle file reader here. This is not ideal
  //  since this particle reader loads into a std::vector, which subsequently
  //  is copied to Eigen. Refactor it to load directly to Eigen. (This is not a
  //  huge problem for now because the particle files are quite small)
  ParticlePositionReader::Pointer reader0 = ParticlePositionReader::New();

  // Read the first file to find dimensions
  reader0->SetFileName(paths[0]);
  reader0->Update();
  const int D = reader0->GetOutput().size() * VDimension;

  P.resize(D, N);
  P.col(0) = Eigen::Map<const Eigen::VectorXd>((double*)reader0->GetOutput().data(), D);

  for (int i = 1; i < N; i++) {
    ParticlePositionReader::Pointer reader = ParticlePositionReader::New();
    reader->SetFileName(paths[i]);
    reader->Update();
    int count = reader->GetOutput().size() * VDimension;
    if (count != D) {
      throw std::runtime_error("ParticleSystemEvaluation files must have the same number of particles");
    }
    P.col(i) = Eigen::Map<const Eigen::VectorXd>((double*)reader->GetOutput().data(), D);
  }
}

ParticleSystemEvaluation::ParticleSystemEvaluation(const Eigen::MatrixXd& matrix) { this->P = matrix; }

bool ParticleSystemEvaluation::ExactCompare(const ParticleSystemEvaluation& other) const {
  if (P.rows() != other.P.rows() || P.cols() != other.P.cols()) {
    std::cerr << "Rows/Columns mismatch\n";
    return false;
  }
  bool same = true;
  for (int r = 0; r < P.rows(); r++) {
    for (int c = 0; c < P.cols(); c++) {
      if (!epsEqual(P(r, c), other.P(r, c), 0.001)) {
        std::cerr << "(" << r << "," << c << "): " << P(r, c) << " vs " << other.P(r, c) << "\n";
        same = false;
      }
    }
  }
  return same;
}

bool ParticleSystemEvaluation::EvaluationCompare(const ParticleSystemEvaluation& other) const {
  auto compactness1 = ShapeEvaluation::ComputeFullCompactness(*this);
  auto compactness2 = ShapeEvaluation::ComputeFullCompactness(other);
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

  auto gen1 = ShapeEvaluation::ComputeFullGeneralization(*this);
  auto gen2 = ShapeEvaluation::ComputeFullGeneralization(other);
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

  auto spec1 = ShapeEvaluation::ComputeFullSpecificity(*this);
  auto spec2 = ShapeEvaluation::ComputeFullSpecificity(other);
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

bool ParticleSystemEvaluation::ReadParticleFile(std::string filename, Eigen::VectorXd& points) {
  std::ifstream in(filename);
  if (!in.good()) {
    return false;
  }
  vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();
  int num_points = 0;
  while (in.good()) {
    double x, y, z;
    in >> x >> y >> z;
    if (!in.good()) {
      break;
    }
    vtk_points->InsertNextPoint(x, y, z);
    num_points++;
  }
  in.close();
  points.setZero();
  points.resize(num_points * 3);

  int idx = 0;
  for (int i = 0; i < num_points; i++) {
    double* pos = vtk_points->GetPoint(i);
    points[idx++] = pos[0];
    points[idx++] = pos[1];
    points[idx++] = pos[2];
  }
  return true;
}

}  // namespace shapeworks

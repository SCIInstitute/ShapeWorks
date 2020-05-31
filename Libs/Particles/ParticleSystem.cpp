#include "ParticleSystem.h"

namespace shapeworks {

ParticleSystem::ParticleSystem(const std::vector<std::string> &_paths)
{
  if (_paths.empty()) {
    throw std::runtime_error("No filenames passed to readparticlesystem");
  }

  this->paths = _paths;
  const int N = paths.size();
  const int VDimension = 3; //TODO Don't hardcode VDimension
  assert(N > 0);

  //TODO: We're using the existing particle file reader here. This is not ideal
  // since this particle reader loads into a std::vector, which subsequently
  // is copied to Eigen. Refactor it to load directly to Eigen. (This is not a
  // huge problem for now because the particle files are quite small)
  typename itk::ParticlePositionReader<VDimension>::Pointer reader0
          = itk::ParticlePositionReader<VDimension>::New();

  // Read the first file to find dimensions
  reader0->SetFileName(paths[0]);
  reader0->Update();
  const int D = reader0->GetOutput().size() * VDimension;

  P.resize(D, N);
  P.col(0) = Eigen::Map<const Eigen::VectorXd>((double *) reader0->GetOutput().data(), D);

  for (int i = 1; i < N; i++) {
    typename itk::ParticlePositionReader<VDimension>::Pointer reader
            = itk::ParticlePositionReader<VDimension>::New();
    reader->SetFileName(paths[i]);
    reader->Update();
    P.col(i) = Eigen::Map<const Eigen::VectorXd>((double *) reader->GetOutput().data(), D);
  }
}
}

#pragma once

#include "itkZeroCrossingImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace shapeworks
{

template <class TNumericType, unsigned int VDimension>
void MeanCurvatureContainer<TNumericType, VDimension>::
ComputeCurvatureStatistics(const ParticleSystemType *system, unsigned int d)
{
  m_MeanCurvatureList[d] = system->GetDomain(d)->GetSurfaceMeanCurvature();
  m_CurvatureStandardDeviationList[d] = system->GetDomain(d)->GetSurfaceStdDevCurvature();

  if (m_verbosity > 1)
  {
      std::cout << "Mean curvature magnitude = " << m_MeanCurvatureList[d] << std::endl;
      std::cout << "Std deviation = " << m_CurvatureStandardDeviationList[d] << std::endl;
  }
}

} // end namespace shapeworks

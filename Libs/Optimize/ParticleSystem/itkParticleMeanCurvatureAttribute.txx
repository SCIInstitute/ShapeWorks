/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleMeanCurvatureAttribute.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "itkZeroCrossingImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{

template <class TNumericType, unsigned int VDimension>
void
ParticleMeanCurvatureAttribute<TNumericType, VDimension>::
ComputeCurvatureStatistics(const ParticleSystemType *system, unsigned int d)
{
  //TODO: Get rid of dynamic_cast
  const auto domain = dynamic_cast<const itk::ParticleImplicitSurfaceDomain<float, 3> *>(system->GetDomain(d));
  m_MeanCurvatureList[d] = domain->GetSurfaceMeanCurvature();
  m_CurvatureStandardDeviationList[d] = domain->GetSurfaceStdDevCurvature();

  if (m_verbosity > 1)
  {
      std::cout << "Mean curvature magnitude = " << m_MeanCurvatureList[d] << std::endl;
      std::cout << "Std deviation = " << m_CurvatureStandardDeviationList[d] << std::endl;
  }
}

} // end namespace itk

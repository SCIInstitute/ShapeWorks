#pragma once

#include "CorrespondenceMode.h"
#include "MaximumEntropySurfaceSampler.h"
#include "itkParticleDualVectorFunction.h"
#include "itkParticleEnsembleEntropyFunction.h"
#include "itkParticleShapeLinearRegressionMatrixAttribute.h"
#include "itkParticleShapeMixedEffectsMatrixAttribute.h"
#include "itkParticleMeshBasedGeneralEntropyGradientFunction.h"

namespace shapeworks {

/** \class MaximumEntropyCorrespondenceSampler
 *
 * 
 *
 */
class MaximumEntropyCorrespondenceSampler : public MaximumEntropySurfaceSampler {
public:

  using Superclass = MaximumEntropySurfaceSampler;

  /** Type of the input/output image. */
  typedef MaximumEntropySurfaceSampler::ImageType ImageType;

  /** Expose the point type */
  typedef ImageType::PointType PointType;

  //! Constructor
  MaximumEntropyCorrespondenceSampler();

private:
  MaximumEntropyCorrespondenceSampler(
    const MaximumEntropyCorrespondenceSampler&); //purposely not implemented
  void operator=(const MaximumEntropyCorrespondenceSampler&); //purposely not implemented


};

} // end namespace


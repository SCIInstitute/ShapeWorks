/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkMaximumEntropyCorrespondenceSampler.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkMaximumEntropyCorrespondenceSampler_txx
#define __itkMaximumEntropyCorrespondenceSampler_txx

namespace itk
{

template <class TImage>
MaximumEntropyCorrespondenceSampler<TImage>::MaximumEntropyCorrespondenceSampler()
{
  m_LinkingFunction = ParticleDualVectorFunction<Dimension>::New();
  m_EnsembleNormalPenaltyFunction = ParticleEnsembleNormalPenaltyFunction<Dimension>::New();
  m_EnsembleMeanFunction = ParticleEnsembleMeanFunction<Dimension>::New();
  m_GeneralEntropyGradientFunction = ParticleGeneralEntropyGradientFunction<Dimension>::New();
  m_EnsembleEntropyFunction = ParticleEnsembleEntropyFunction<Dimension>::New();
  m_EnsembleRegressionEntropyFunction = ParticleEnsembleEntropyFunction<Dimension>::New();
  m_EnsembleMixedEffectsEntropyFunction = ParticleEnsembleEntropyFunction<Dimension>::New();
  m_MeshBasedGeneralEntropyGradientFunction = ParticleMeshBasedGeneralEntropyGradientFunction<Dimension>::New();
  m_MeshBasedGeneralMeanGradientFunction = ParticleMeshBasedGeneralMeanGradientFunction<Dimension>::New();

  m_ShapeMatrix = ParticleShapeMatrixAttribute<double, Dimension>::New();
  m_GeneralShapeMatrix = ParticleGeneralShapeMatrix<double, Dimension>::New();
  m_GeneralShapeGradMatrix = ParticleGeneralShapeGradientMatrix<double, Dimension>::New();

  m_LinearRegressionShapeMatrix = ParticleShapeLinearRegressionMatrixAttribute<double, Dimension>::New();
  m_MixedEffectsShapeMatrix = ParticleShapeMixedEffectsMatrixAttribute<double, Dimension>::New();
  m_FunctionShapeData = ParticleFunctionBasedShapeSpaceData<float, Dimension>::New();
  m_EnsembleEntropyFunction->SetShapeMatrix(m_ShapeMatrix);
  m_EnsembleMeanFunction->SetShapeMatrix(m_ShapeMatrix);
  
  m_EnsembleRegressionEntropyFunction->SetShapeMatrix(m_LinearRegressionShapeMatrix);
    //  m_EnsembleRegressionEntropyFunction->SetShapeMatrix(m_ShapeMatrix);
  m_EnsembleMixedEffectsEntropyFunction->SetShapeMatrix(m_MixedEffectsShapeMatrix);
  m_GeneralEntropyGradientFunction->SetShapeData(m_FunctionShapeData);

  m_MeshBasedGeneralEntropyGradientFunction->SetShapeData(m_GeneralShapeMatrix);
  m_MeshBasedGeneralEntropyGradientFunction->SetShapeGradient(m_GeneralShapeGradMatrix);

  Superclass::m_ParticleSystem->RegisterAttribute(m_ShapeMatrix);
  Superclass::m_ParticleSystem->RegisterAttribute(m_LinearRegressionShapeMatrix);
  Superclass::m_ParticleSystem->RegisterAttribute(m_MixedEffectsShapeMatrix);
  Superclass::m_ParticleSystem->RegisterAttribute(m_FunctionShapeData);

//  Superclass::m_ParticleSystem->RegisterAttribute(m_GeneralShapeMatrix);
//  Superclass::m_ParticleSystem->RegisterAttribute(m_GeneralShapeGradMatrix);

  // PRATEEP
//  m_CorrespondenceMode = 0; // changed 09/24
  m_CorrespondenceMode = 1;
}

template<class TImage>
void
MaximumEntropyCorrespondenceSampler<TImage>::AllocateDataCaches()
{
  Superclass::AllocateDataCaches();
  //   m_CurvatureEnsembleMeanFunction->SetMeanCurvatureCache(Superclass::m_MeanCurvatureCache);
}

template <class TImage>
void
MaximumEntropyCorrespondenceSampler<TImage>::GenerateData()
{    
  this->SetInPlace(false); // this is required so that we don't release our inputs
  if (this->GetInitialized() == false)
    {
      /* Praful - not needed anymore - during fix of same bounding box in every domain - v4.3 */

          // TEMPORARY HACK.  REALLY WHAT I WANT TO DO IS GRAFT THE INPUT IMAGES TO
          // AN ARRAY OF OUTPUT IMAGES AND USE THOSE.
          // this->AllocateWorkingImages();

      /*--------------------------------------------------------------------------------------*/

    this->AllocateDataCaches();
    this->SetAdaptivityMode(Superclass::m_AdaptivityMode);
    this->SetCorrespondenceMode(m_CorrespondenceMode);
    this->GetOptimizer()->SetGradientFunction(m_LinkingFunction);
    m_LinkingFunction->SetAOn();
    m_LinkingFunction->SetBOn();
    m_LinkingFunction->SetCOff();

    this->AllocateDomainsAndNeighborhoods();

    // Point the optimizer to the particle system.
    this->GetOptimizer()->SetParticleSystem(this->GetParticleSystem());
    this->ReadTransforms();
    this->ReadPointsFiles();
    this->InitializeOptimizationFunctions();    


    this->SetInitialized(true);
    }

  if (this->GetInitializing() == true) return;

  std::cout << "MaximumEntropyCorrespondenceSampler: starting optimization ..\n" << std::flush;
  this->GetOptimizer()->StartOptimization();
}
template <class TImage>
void
MaximumEntropyCorrespondenceSampler<TImage>::InitializeOptimizationFunctions()
{
  Superclass::InitializeOptimizationFunctions();
  m_LinearRegressionShapeMatrix->Initialize();
  m_MixedEffectsShapeMatrix->Initialize();
  m_ShapeMatrix->Initialize();

  m_GeneralShapeMatrix->Initialize();
  m_GeneralShapeGradMatrix->Initialize();

}


} // end namespace

#endif

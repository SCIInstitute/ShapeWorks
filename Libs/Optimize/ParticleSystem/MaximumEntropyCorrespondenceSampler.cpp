#include "MaximumEntropyCorrespondenceSampler.h"

namespace shapeworks {

MaximumEntropyCorrespondenceSampler::MaximumEntropyCorrespondenceSampler()
{
  m_LinkingFunction = itk::ParticleDualVectorFunction<Dimension>::New();
  m_EnsembleEntropyFunction = itk::ParticleEnsembleEntropyFunction<Dimension>::New();
  m_EnsembleRegressionEntropyFunction = itk::ParticleEnsembleEntropyFunction<Dimension>::New();
  m_EnsembleMixedEffectsEntropyFunction = itk::ParticleEnsembleEntropyFunction<Dimension>::New();
  m_MeshBasedGeneralEntropyGradientFunction = itk::ParticleMeshBasedGeneralEntropyGradientFunction<Dimension>::New();

  m_ShapeMatrix = itk::ParticleShapeMatrixAttribute<double, Dimension>::New();
  m_GeneralShapeMatrix = itk::ParticleGeneralShapeMatrix<double, Dimension>::New();
  m_GeneralShapeGradMatrix = itk::ParticleGeneralShapeGradientMatrix<double, Dimension>::New();

  m_LinearRegressionShapeMatrix = itk::ParticleShapeLinearRegressionMatrixAttribute<double, Dimension>::New();
  m_MixedEffectsShapeMatrix = itk::ParticleShapeMixedEffectsMatrixAttribute<double, Dimension>::New();

  m_EnsembleEntropyFunction->SetShapeMatrix(m_ShapeMatrix);

  m_EnsembleRegressionEntropyFunction->SetShapeMatrix(m_LinearRegressionShapeMatrix);
  m_EnsembleMixedEffectsEntropyFunction->SetShapeMatrix(m_MixedEffectsShapeMatrix);

  m_MeshBasedGeneralEntropyGradientFunction->SetShapeData(m_GeneralShapeMatrix);
  m_MeshBasedGeneralEntropyGradientFunction->SetShapeGradient(m_GeneralShapeGradMatrix);

  Superclass::m_ParticleSystem->RegisterAttribute(m_ShapeMatrix);
  Superclass::m_ParticleSystem->RegisterAttribute(m_LinearRegressionShapeMatrix);
  Superclass::m_ParticleSystem->RegisterAttribute(m_MixedEffectsShapeMatrix);

  m_CorrespondenceMode = shapeworks::CorrespondenceMode::EnsembleEntropy;
}

void MaximumEntropyCorrespondenceSampler::AllocateDataCaches()
{
  Superclass::AllocateDataCaches();
}

void MaximumEntropyCorrespondenceSampler::ReInitialize()
{
  this->SetAdaptivityMode(Superclass::m_AdaptivityMode);
  this->SetCorrespondenceMode(m_CorrespondenceMode);
  this->GetOptimizer()->SetGradientFunction(m_LinkingFunction);
  this->m_LinkingFunction->SetAOn();
  this->m_LinkingFunction->SetBOn();
  this->InitializeOptimizationFunctions();
  this->m_Sigma1Cache->ZeroAllValues();
  this->m_Sigma2Cache->ZeroAllValues();
  this->m_MeanCurvatureCache->ZeroAllValues();
}

void MaximumEntropyCorrespondenceSampler::Execute()
{
  if (this->GetInitialized() == false) {
    this->AllocateDataCaches();
    this->SetAdaptivityMode(Superclass::m_AdaptivityMode);
    this->SetCorrespondenceMode(m_CorrespondenceMode);
    this->GetOptimizer()->SetGradientFunction(m_LinkingFunction);
    m_LinkingFunction->SetAOn();
    m_LinkingFunction->SetBOn();

    this->AllocateDomainsAndNeighborhoods();

    // Point the optimizer to the particle system.
    this->GetOptimizer()->SetParticleSystem(this->GetParticleSystem());
    this->ReadTransforms();
    this->ReadPointsFiles();
    this->InitializeOptimizationFunctions();

    this->SetInitialized(true);
  }

  if (this->GetInitializing() == true) return;

  //this->GetOptimizer()->SetShapeMatrix(this->m_ShapeMatrix);
  this->GetOptimizer()->StartOptimization();
}

void MaximumEntropyCorrespondenceSampler::InitializeOptimizationFunctions()
{
  Superclass::InitializeOptimizationFunctions();
  m_LinearRegressionShapeMatrix->Initialize();
  m_MixedEffectsShapeMatrix->Initialize();
  m_ShapeMatrix->Initialize();

  m_GeneralShapeMatrix->Initialize();
  m_GeneralShapeGradMatrix->Initialize();

}

} // end namespace


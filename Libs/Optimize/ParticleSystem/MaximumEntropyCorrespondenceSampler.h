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
  typedef typename MaximumEntropySurfaceSampler::ImageType ImageType;

  /** Expose the point type */
  typedef typename ImageType::PointType PointType;

  //! Constructor
  MaximumEntropyCorrespondenceSampler();

  void SetCorrespondenceOn()
  {
    m_LinkingFunction->SetBOn();
  }

  void SetCorrespondenceOff()
  {
    m_LinkingFunction->SetBOff();
  }

  void SetSamplingOn()
  {
    m_LinkingFunction->SetAOn();
  }

  void SetSamplingOff()
  {
    m_LinkingFunction->SetAOff();
  }

  bool GetCorrespondenceOn() const
  { return m_LinkingFunction->GetBOn(); }

  bool GetSamplingOn() const
  { return m_LinkingFunction->GetAOn(); }

  /** This method sets the optimization function for the sampling.
      mode 0 = isotropic adaptivity
      mode 1 = no adaptivity
  */
  virtual void SetAdaptivityMode(int mode) override
  {
    if (mode == 0) {
      if (this->m_pairwise_potential_type == 0)
        m_LinkingFunction->SetFunctionA(this->GetCurvatureGradientFunction());
      else if (this->m_pairwise_potential_type == 1)
        m_LinkingFunction->SetFunctionA(this->GetModifiedCotangentGradientFunction());
    }
    else if (mode == 1) {
      m_LinkingFunction->SetFunctionA(this->GetGradientFunction());
    }
    else if (mode == 3) {
      if (this->m_pairwise_potential_type == 0)
        m_LinkingFunction->SetFunctionA(this->GetOmegaGradientFunction());
      else if (this->m_pairwise_potential_type == 1)
        m_LinkingFunction->SetFunctionA(this->GetConstrainedModifiedCotangentGradientFunction());
    }

    this->m_AdaptivityMode = mode;
  }

  /** This method sets the optimization function for correspondences between surfaces (domains). */
  virtual void SetCorrespondenceMode(shapeworks::CorrespondenceMode mode)
  {
    if (mode == shapeworks::CorrespondenceMode::MeanEnergy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleEntropyFunction);
      m_EnsembleEntropyFunction->UseMeanEnergy();
    }
    else if (mode == shapeworks::CorrespondenceMode::EnsembleEntropy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleEntropyFunction);
      m_EnsembleEntropyFunction->UseEntropy();
    }
    else if (mode == shapeworks::CorrespondenceMode::EnsembleRegressionEntropy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleRegressionEntropyFunction);
    }
    else if (mode == shapeworks::CorrespondenceMode::EnsembleMixedEffectsEntropy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleMixedEffectsEntropyFunction);
    }
    else if (mode == shapeworks::CorrespondenceMode::MeshBasedGeneralEntropy) {
      m_LinkingFunction->SetFunctionB(m_MeshBasedGeneralEntropyGradientFunction);
      m_MeshBasedGeneralEntropyGradientFunction->UseEntropy();
    }
    else if (mode == shapeworks::CorrespondenceMode::MeshBasedGeneralMeanEnergy) {
      m_LinkingFunction->SetFunctionB(m_MeshBasedGeneralEntropyGradientFunction);
      m_MeshBasedGeneralEntropyGradientFunction->UseMeanEnergy();
    }

    m_CorrespondenceMode = mode;
  }

  void RegisterGeneralShapeMatrices()
  {
    this->m_ParticleSystem->RegisterAttribute(m_GeneralShapeMatrix);
    this->m_ParticleSystem->RegisterAttribute(m_GeneralShapeGradMatrix);
  }

  void SetAttributeScales(const std::vector<double>& s)
  {
    m_MeshBasedGeneralEntropyGradientFunction->SetAttributeScales(s);
    m_GeneralShapeMatrix->SetAttributeScales(s);
    m_GeneralShapeGradMatrix->SetAttributeScales(s);
  }

  void SetXYZ(unsigned int i, bool flag)
  {
    m_MeshBasedGeneralEntropyGradientFunction->SetXYZ(i, flag);
    m_GeneralShapeMatrix->SetXYZ(i, flag);
    m_GeneralShapeGradMatrix->SetXYZ(i, flag);
  }

  void SetNormals(int i, bool flag)
  {
    m_MeshBasedGeneralEntropyGradientFunction->SetNormals(i, flag);
    m_GeneralShapeMatrix->SetNormals(i, flag);
    m_GeneralShapeGradMatrix->SetNormals(i, flag);
  }

  void SetAttributesPerDomain(const std::vector<int> s)
  {
    std::vector<int> s1;
    if (s.size() == 0) {
      s1.resize(m_MeshBasedGeneralEntropyGradientFunction->GetDomainsPerShape());
      for (int i = 0; i < m_MeshBasedGeneralEntropyGradientFunction->GetDomainsPerShape(); i++)
        s1[i] = 0;
    }
    else
      s1 = s;
    Superclass::SetAttributesPerDomain(s1);
    m_MeshBasedGeneralEntropyGradientFunction->SetAttributesPerDomain(s1);
    m_GeneralShapeMatrix->SetAttributesPerDomain(s1);
    m_GeneralShapeGradMatrix->SetAttributesPerDomain(s1);
  }

  itk::ParticleShapeMatrixAttribute<double, Dimension>* GetShapeMatrix()
  {
    return m_ShapeMatrix.GetPointer();
  }

  itk::ParticleGeneralShapeMatrix<double, Dimension>* GetGeneralShapeMatrix()
  {
    return m_GeneralShapeMatrix.GetPointer();
  }

  itk::ParticleGeneralShapeGradientMatrix<double, Dimension>* GetGeneralShapeGradientMatrix()
  {
    return m_GeneralShapeGradMatrix.GetPointer();
  }

  itk::ParticleDualVectorFunction<Dimension>* GetLinkingFunction()
  { return m_LinkingFunction.GetPointer(); }

  itk::ParticleEnsembleEntropyFunction<Dimension>* GetEnsembleEntropyFunction()
  { return m_EnsembleEntropyFunction.GetPointer(); }

  itk::ParticleEnsembleEntropyFunction<Dimension>* GetEnsembleRegressionEntropyFunction()
  { return m_EnsembleRegressionEntropyFunction.GetPointer(); }

  itk::ParticleEnsembleEntropyFunction<Dimension>* GetEnsembleMixedEffectsEntropyFunction()
  { return m_EnsembleMixedEffectsEntropyFunction.GetPointer(); }

  itk::ParticleMeshBasedGeneralEntropyGradientFunction<Dimension>* GetMeshBasedGeneralEntropyGradientFunction()
  { return m_MeshBasedGeneralEntropyGradientFunction.GetPointer(); }

  const itk::ParticleDualVectorFunction<Dimension>* GetLinkingFunction() const
  { return m_LinkingFunction.GetPointer(); }

  const itk::ParticleEnsembleEntropyFunction<Dimension>* GetEnsembleEntropyFunction() const
  { return m_EnsembleEntropyFunction.GetPointer(); }

  const itk::ParticleEnsembleEntropyFunction<Dimension>* GetEnsembleRegressionEntropyFunction() const
  { return m_EnsembleRegressionEntropyFunction.GetPointer(); }

  const itk::ParticleEnsembleEntropyFunction<Dimension>* GetEnsembleMixedEffectsEntropyFunction() const
  { return m_EnsembleMixedEffectsEntropyFunction.GetPointer(); }

  const itk::ParticleMeshBasedGeneralEntropyGradientFunction<Dimension>*
  GetMeshBasedGeneralEntropyGradientFunction() const
  { return m_MeshBasedGeneralEntropyGradientFunction.GetPointer(); }

  virtual void AllocateDataCaches();

  void SetDomainsPerShape(int n)
  {
    Superclass::SetDomainsPerShape(n);
    m_LinearRegressionShapeMatrix->SetDomainsPerShape(n);
    m_MixedEffectsShapeMatrix->SetDomainsPerShape(n);
    m_ShapeMatrix->SetDomainsPerShape(n);
    m_MeshBasedGeneralEntropyGradientFunction->SetDomainsPerShape(n);
    m_GeneralShapeMatrix->SetDomainsPerShape(n);
    m_GeneralShapeGradMatrix->SetDomainsPerShape(n);
  }

  void SetTimeptsPerIndividual(int n)
  {
    m_MixedEffectsShapeMatrix->SetTimeptsPerIndividual(n);
  }

  shapeworks::CorrespondenceMode GetCorrespondenceMode() const
  { return m_CorrespondenceMode; }

  virtual void InitializeOptimizationFunctions();

  void ReInitialize() override;

  void Execute() override;

private:
  MaximumEntropyCorrespondenceSampler(const MaximumEntropyCorrespondenceSampler&); //purposely not implemented
  void operator=(const MaximumEntropyCorrespondenceSampler&); //purposely not implemented
  shapeworks::CorrespondenceMode m_CorrespondenceMode;

  typename itk::ParticleDualVectorFunction<Dimension>::Pointer m_LinkingFunction;

  typename itk::ParticleEnsembleEntropyFunction<Dimension>::Pointer m_EnsembleEntropyFunction;
  typename itk::ParticleEnsembleEntropyFunction<Dimension>::Pointer m_EnsembleRegressionEntropyFunction;
  typename itk::ParticleEnsembleEntropyFunction<Dimension>::Pointer m_EnsembleMixedEffectsEntropyFunction;

  typename itk::ParticleShapeMatrixAttribute<double, Dimension>::Pointer m_ShapeMatrix;

  typename itk::ParticleShapeLinearRegressionMatrixAttribute<double, Dimension>::Pointer m_LinearRegressionShapeMatrix;
  typename itk::ParticleShapeMixedEffectsMatrixAttribute<double, Dimension>::Pointer m_MixedEffectsShapeMatrix;

  typename itk::ParticleGeneralShapeMatrix<double, Dimension>::Pointer m_GeneralShapeMatrix;
  typename itk::ParticleGeneralShapeGradientMatrix<double, Dimension>::Pointer m_GeneralShapeGradMatrix;

  typename itk::ParticleMeshBasedGeneralEntropyGradientFunction<Dimension>::Pointer m_MeshBasedGeneralEntropyGradientFunction;
};

} // end namespace


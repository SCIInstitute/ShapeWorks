#pragma once

#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_matrix.h"
#include "itkParticleSystem.h"

namespace shapeworks
{
/** \class ParticleProcrustesRegistration
 *  \brief
 *
 *  
 *
 */
class ParticleProcrustesRegistration: public itk::DataObject
{
public:
  static constexpr int VDimension = 3;
  /** Standard class typedefs */
  //  typedef TDataType DataType;
  typedef ParticleProcrustesRegistration Self;
  typedef DataObject Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;
  typedef itk::WeakPointer<const Self>  ConstWeakPointer;

  /** Particle system typedefs. */
  typedef itk::ParticleSystem ParticleSystemType;
  typedef ParticleSystemType::PointType PointType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleProcrustesRegistration, DataObject);

  /** Set/Get the target particle system. */
  void SetParticleSystem(ParticleSystemType *p)
  { m_ParticleSystem = p;  }
  const ParticleSystemType *GetParticleSystem() const
  { return m_ParticleSystem; }
  ParticleSystemType *GetParticleSystem()
  { return m_ParticleSystem; }

  /** Callback suitable for adding as an observer of itk object iteration
      events. */
  void IterationCallback(itk::Object *, const itk::EventObject &)
  {
    this->RunRegistration();
  }

  /** Performs a procrustes registration and modifies the transforms of the
      particle system accordingly.  Assumes m_ParticleSystem has been set to
      point to a valid object.*/
  void RunRegistration(int i);
  void RunRegistration()
  {
    for (int i =0; i < m_DomainsPerShape; i++)
      {
      this->RunRegistration(i);
      }
  }

  /** Set/Get the number of Domains in each shape.  Procrustes will be
      performed separately for each domain. */
  void SetDomainsPerShape(int i)
  { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const
  { return m_DomainsPerShape; }

  bool GetScaling() const
  { return m_Scaling; }
  void ScalingOn()
  { m_Scaling = true; }
  void ScalingOff()
  { m_Scaling = false; }
  
  bool GetRotationTranslation() const
  { return m_RotationTranslation; }
  void RotationTranslationOn()
  { m_RotationTranslation = true; }
  void RotationTranslationOff()
  { m_RotationTranslation = false; }
  
  void SetFixedScales( const std::vector<double> v)
  { m_FixedScales = v; }

protected:
  ParticleProcrustesRegistration() : m_DomainsPerShape(1), m_Scaling(true), m_RotationTranslation(true) {  }
  virtual ~ParticleProcrustesRegistration() {};

  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {  Superclass::PrintSelf(os,indent);  }

private:
  ParticleProcrustesRegistration(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<double> m_FixedScales;
  
  int m_DomainsPerShape;
  bool m_Scaling;
  bool m_RotationTranslation;
  bool m_ComputeTransformation;
  ParticleSystemType *m_ParticleSystem;
};

} // end namespace

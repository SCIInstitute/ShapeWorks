/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkMaximumEntropyCorrespondenceSampler.h,v $

  Date:      $Date: 2011/03/24 01:17:33 $

  Version:   $Revision: 1.3 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkMaximumEntropyCorrespondenceSampler_h

#define __itkMaximumEntropyCorrespondenceSampler_h



#include "itkMaximumEntropySurfaceSampler.h"

#include "itkParticleDualVectorFunction.h"

#include "itkParticleEnsembleNormalPenaltyFunction.h"

#include "itkParticleEnsembleMeanFunction.h"

#include "itkParticleEnsembleEntropyFunction.h"

#include "itkParticleGeneralEntropyGradientFunction.h"

#include "itkParticleShapeLinearRegressionMatrixAttribute.h"



namespace itk

{

  

/** \class MaximumEntropyCorrespondenceSampler

 *

 * 

 *

 */

template <class TImage>

class ITK_EXPORT MaximumEntropyCorrespondenceSampler

  : public MaximumEntropySurfaceSampler<TImage> 

{

public:

  /** Standard class typedefs. */

  typedef MaximumEntropyCorrespondenceSampler  Self;

  typedef MaximumEntropySurfaceSampler<TImage>  Superclass;

  typedef SmartPointer<Self>   Pointer;

  typedef SmartPointer<const Self>  ConstPointer;



  /** Method for creation through the object factory. */

  itkNewMacro(Self);

  

  /** Run-time type information (and related methods). */

  itkTypeMacro(MaximumEntropyCorrespondenceSampler, MaximumEntropySurfaceSampler);



  /**Expose the image dimension. */

  itkStaticConstMacro(Dimension, unsigned int, TImage::ImageDimension);



  /** Type of the input/output image. */

  typedef typename Superclass::ImageType ImageType;



  /** Expose the point type */

  typedef typename ImageType::PointType PointType;



  

  void SetCorrespondenceOn()

  {

    m_LinkingFunction->SetBOn();

    this->Modified();

  }

  void SetCorrespondenceOff()

  {

    m_LinkingFunction->SetBOff();

    this->Modified();

  }



  void SetSamplingOn()

  {

    m_LinkingFunction->SetAOn();

    this->Modified();

  }

  void SetSamplingOff()

  {

    m_LinkingFunction->SetAOff();

    this->Modified();

  }



  bool GetCorrespondenceOn() const

  { return m_LinkingFunction->GetBOn(); }

  bool GetSamplingOn() const

  { return m_LinkingFunction->GetAOn(); }



  /** This method sets the optimization function for the sampling.

      mode 0 = isotropic adaptivity

      mode 1 = no adaptivity

  */

  virtual void SetAdaptivityMode(int mode)

  {

    if (mode == 0)

      {

      m_LinkingFunction->SetFunctionA(this->GetCurvatureGradientFunction());

      }

    else if (mode == 1)

      {

      m_LinkingFunction->SetFunctionA(this->GetGradientFunction());

      }

    else if (mode == 2)

      {

      m_LinkingFunction->SetFunctionA(this->GetQualifierGradientFunction());

      }

    else if (mode == 3)

      {

      m_LinkingFunction->SetFunctionA(this->GetOmegaGradientFunction());

      }



    Superclass::m_AdaptivityMode = mode;

    this->Modified();

  }



  /** This method sets the optimization function for correspondences between

      surfaces (domains).

      mode 0 = mean force

      mode 1 = minimum entropy

  */

  virtual void SetCorrespondenceMode(int mode)

  {

    if (mode == 1)

      {

      m_LinkingFunction->SetFunctionB(m_EnsembleEntropyFunction);

      }

    else if (mode == 2)

      {

      m_LinkingFunction->SetFunctionB(m_GeneralEntropyGradientFunction);

      }

    else if (mode == 3)

      {

      m_LinkingFunction->SetFunctionB(m_EnsembleRegressionEntropyFunction);

      }

    else

      {

      m_LinkingFunction->SetFunctionB(m_EnsembleMeanFunction);

      }

    m_LinkingFunction->SetFunctionC(m_EnsembleNormalPenaltyFunction);

    m_CorrespondenceMode = mode;

  }



  void SetAttributeScales(const std::vector<double> &s)

  {

    m_GeneralEntropyGradientFunction->SetAttributeScales(s);

  }

    

  

  void AddAttributeImage(int d,

                         typename ParticleFunctionBasedShapeSpaceData<float, Dimension>::ImageType *I)

  {

    m_FunctionShapeData->AddFunctionImage(d, I);

  }

  

  ParticleDualVectorFunction<Dimension> *GetLinkingFunction()

  { return m_LinkingFunction.GetPointer(); }

  ParticleEnsembleNormalPenaltyFunction<Dimension> *GetEnsembleNormalPenaltyFunction()

  { return m_EnsembleNormalPenaltyFunction.GetPointer(); }

  ParticleEnsembleMeanFunction<Dimension> *GetEnsembleMeanFunction()

  { return m_EnsembleMeanFunction.GetPointer(); }

  ParticleEnsembleEntropyFunction<Dimension> *GetEnsembleEntropyFunction()

  { return m_EnsembleEntropyFunction.GetPointer(); }

  ParticleEnsembleEntropyFunction<Dimension> *GetEnsembleRegressionEntropyFunction()

  { return m_EnsembleRegressionEntropyFunction.GetPointer(); }

  ParticleGeneralEntropyGradientFunction<Dimension> *GetGeneralEntropyGradientFunction()

  { return m_GeneralEntropyGradientFunction.GetPointer(); }



  

  const ParticleDualVectorFunction<Dimension> *GetLinkingFunction() const

  { return m_LinkingFunction.GetPointer(); }

  const ParticleEnsembleMeanFunction<Dimension> *GetEnsembleMeanFunction() const

  { return m_EnsembleMeanFunction.GetPointer(); }

  const ParticleEnsembleNormalPenaltyFunction<Dimension> *GetEnsembleNormalPenaltyFunction() const

  { return m_EnsembleNormalPenaltyFunction.GetPointer(); }

  const ParticleEnsembleEntropyFunction<Dimension> *GetEnsembleEntropyFunction() const

  { return m_EnsembleEntropyFunction.GetPointer(); }

  const ParticleEnsembleEntropyFunction<Dimension> *GetEnsembleRegressionEntropyFunction() const

  { return m_EnsembleRegressionEntropyFunction.GetPointer(); }

  const ParticleGeneralEntropyGradientFunction<Dimension> *GetGeneralEntropyGradientFunction() const

  { return m_GeneralEntropyGradientFunction.GetPointer(); }

  

  virtual void AllocateDataCaches();



  void SetDomainsPerShape(int n)

  {

    m_LinearRegressionShapeMatrix->SetDomainsPerShape(n);

    m_ShapeMatrix->SetDomainsPerShape(n);

    m_EnsembleMeanFunction->SetDomainsPerShape(n);

    m_EnsembleNormalPenaltyFunction->SetDomainsPerShape(n);

  }



  int GetCorrespondenceMode() const

  { return m_CorrespondenceMode; }



  virtual void InitializeOptimizationFunctions();

  

protected:

  MaximumEntropyCorrespondenceSampler();

  virtual ~MaximumEntropyCorrespondenceSampler() {};



  void PrintSelf(std::ostream& os, Indent indent) const

  {

    Superclass::PrintSelf(os, indent);

  }



  void GenerateData();

  

private:

  MaximumEntropyCorrespondenceSampler(const Self&); //purposely not implemented

  void operator=(const Self&); //purposely not implemented

  int m_CorrespondenceMode;



  typename ParticleDualVectorFunction<Dimension>::Pointer m_LinkingFunction;



  typename ParticleEnsembleNormalPenaltyFunction<Dimension>::Pointer m_EnsembleNormalPenaltyFunction;

  typename ParticleEnsembleMeanFunction<Dimension>::Pointer m_EnsembleMeanFunction;

  typename ParticleEnsembleEntropyFunction<Dimension>::Pointer m_EnsembleEntropyFunction;

  typename ParticleEnsembleEntropyFunction<Dimension>::Pointer m_EnsembleRegressionEntropyFunction;

  typename ParticleGeneralEntropyGradientFunction<Dimension>::Pointer m_GeneralEntropyGradientFunction;

  typename ParticleShapeMatrixAttribute<double, Dimension>::Pointer m_ShapeMatrix;

  typename ParticleFunctionBasedShapeSpaceData<float, Dimension>::Pointer m_FunctionShapeData;

  typename ParticleShapeLinearRegressionMatrixAttribute<double, Dimension>::Pointer m_LinearRegressionShapeMatrix;

};



} // end namespace itk



#if ITK_TEMPLATE_EXPLICIT

#include "Templates/itkMaximumEntropyCorrespondenceSampler+-.h"

#endif



#if ITK_TEMPLATE_TXX

#include "itkMaximumEntropyCorrespondenceSampler.txx"

#endif



#endif






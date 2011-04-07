/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkParticleFunctionBasedShapeSpaceData.h,v $

  Date:      $Date: 2011/03/24 01:17:33 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkParticleFunctionBasedShapeSpaceData_h

#define __itkParticleFunctionBasedShapeSpaceData_h



//#include "itkImageFileWriter.h"



#include "itkDataObject.h"

#include "itkWeakPointer.h"

#include "itkParticleAttribute.h"

#include "itkParticleContainer.h"

#include "itkVectorLinearInterpolateImageFunction.h"

#include "itkLinearInterpolateImageFunction.h"

#include "itkGradientImageFilter.h"

#include <vector>

#include "vnl/vnl_matrix.h"



namespace itk

{

/** \class ParticleFunctionBasedShapeSpaceData

 *

 * \brief

 *

 */

template <class T, unsigned int VDimension>

class ITK_EXPORT ParticleFunctionBasedShapeSpaceData

  : public ParticleAttribute<VDimension>

{

public:

  /** Standard class typedefs */

  typedef ParticleFunctionBasedShapeSpaceData Self;

  typedef ParticleAttribute<VDimension> Superclass;

  typedef SmartPointer<Self>  Pointer;

  typedef SmartPointer<const Self>  ConstPointer;

  typedef WeakPointer<const Self>  ConstWeakPointer;



  typedef T DataType;

  

 /** Type of the ITK image used by this class. */

  typedef Image<T, VDimension> ImageType;

  

  /** Point type used to store particle locations. */

  typedef Point<double, VDimension> PointType;    

  

  typedef GradientImageFilter<ImageType> GradientImageFilterType;

  typedef typename GradientImageFilterType::OutputImageType GradientImageType;

  

  typedef VectorLinearInterpolateImageFunction<GradientImageType, typename PointType::CoordRepType>

  GradientInterpolatorType;

  

  typedef LinearInterpolateImageFunction<ImageType, typename PointType::CoordRepType>

  ScalarInterpolatorType;

  

  typedef FixedArray<T, 3> VectorType;

  typedef vnl_vector_fixed<T, 3> VnlVectorType;

  

  /** Method for creation through the object factory. */

  itkNewMacro(Self);

  

  /** Run-time type information (and related methods). */

  itkTypeMacro(ParticleFunctionBasedShapeSpaceData, ParticleAttribute);



  /** Returns the number of dimensions of the shape space, which is the number

      of particles per shape times the number of position-based functions we

      are using. */

  unsigned int GetNumberOfDimensions() const

  { return m_NumberOfParticles * m_NumberOfFunctions; }



  /** Returns the number of samples in the shape space, which is the number of

      surfaces we are using. */

  unsigned int GetNumberOfSamples() const

  { return m_NumberOfSamples; }

  

  /** Adds a function image for shape d. */

  void AddFunctionImage(int d, ImageType *I)

  {

    this->Modified();



    // Make sure we have allocated a list for this shape. 

    while (d >= m_ScalarInterpolators.size() )

      {

      //      std::cout << "d = " << d << " Adding domain" << std::endl;

      this->AddDomain();

      // itkExceptionMacro("Not enough function interpolator lists have been allocated.");

      }

    

    if (d == 0) m_NumberOfFunctions++; // assumes all will have same number of functions



    typename ImageType::Pointer newimage = I;

    //    m_FunctionImages[d].push_back(I);



    //    std::cout << "adding image " << I << std::endl;

//     typename ImageFileWriter<ImageType>::Pointer writer = ImageFileWriter<ImageType>::New();

//     writer->SetInput(I);

//     writer->SetFileName("test.mha");

//     writer->Update();



    // Set up a scalar interpolator

    typename ScalarInterpolatorType::Pointer si = ScalarInterpolatorType::New();

    si->SetInputImage(I);

    m_ScalarInterpolators[d].push_back(si);

    



    // Compute gradient image and set up gradient interpolation.

    typename GradientImageFilterType::Pointer filter = GradientImageFilterType::New();

    filter->SetInput(I);

    filter->SetUseImageSpacingOn();

    filter->Update();



    typename GradientInterpolatorType::Pointer gi = GradientInterpolatorType::New();

    gi->SetInputImage(filter->GetOutput());

    m_GradientInterpolators[d].push_back(gi);

 

    

  }

  

  /** Return the number of functions referenced by an instance of this class. */

  unsigned int GetNumberOfFunctions() const

  { return m_NumberOfFunctions; }



  DataType GetScalar(unsigned int sample, unsigned int dim)

  {

    unsigned int f = dim % m_NumberOfFunctions;

    unsigned int idx = dim / m_NumberOfFunctions;



    //    std::cout << "shape=" << sample << " function=" << f << " idx=" << idx << " value=";



    

    //    std::cout << m_ScalarInterpolators[sample][f]->Evaluate(m_ParticleSystem->GetTransformedPosition(idx,sample)) << std::endl;



    //    std::cout << "Transformed pos = " << m_ParticleSystem->GetTransformedPosition(idx, sample) << std::endl;

    

    return m_ScalarInterpolators[sample][f]->Evaluate(m_ParticleSystem->GetTransformedPosition(idx,sample));

  }

  

//    inline VectorType SampleGradient(const PointType &p) const

//   {   return  m_GradientInterpolator->Evaluate(p);  }

//   inline VnlVectorType SampleGradientVnl(const PointType &p) const

//   { return VnlVectorType( this->SampleGradient(p).GetDataPointer() ); }

  typename Self::VectorType GetGradient(unsigned int sample, unsigned int dim)

  {

    unsigned int f = dim % m_NumberOfFunctions;

    unsigned int idx = dim / m_NumberOfFunctions;

    

    return m_GradientInterpolators[sample][f]->Evaluate(m_ParticleSystem->GetTransformedPosition(idx,sample));

  }

  /** Callbacks that may be defined by a subclass.  If a subclass defines one

      of these callback methods, the corresponding flag in m_DefinedCallbacks

      should be set to true so that the ParticleSystem will know to register

      the appropriate event with this method. */

  virtual void DomainAddEventCallback(Object *, const EventObject &e)

  {

  //   const itk::ParticleDomainAddEvent &event

//       = dynamic_cast<const itk::ParticleDomainAddEvent &>(e);

//     unsigned int d = event.GetDomainIndex();



    // Allocate a new list for gradient and shape interpolators

    // m_ScalarInterpolators.push_back(std::vector<typename ScalarInterpolatorType::Pointer>);

    //    m_GradientInterpolators.push_back(std::vector<typename GradientInterpolatorType::Pointer>);

        

    m_NumberOfSamples++;

    

  }

  

  virtual void PositionAddEventCallback(Object *o, const EventObject &e) 

  {

    const itk::ParticlePositionAddEvent &event

      = dynamic_cast<const itk::ParticlePositionAddEvent &>(e);

	const itk::ParticleSystem<VDimension> *ps

          = dynamic_cast<const itk::ParticleSystem<VDimension> *>(o);

    const int d = event.GetDomainIndex();

        const unsigned int idx = event.GetPositionIndex();

        m_ParticleSystem = const_cast<itk::ParticleSystem<VDimension> *>(ps);

    // cache the particle system we are working with

            const typename itk::ParticleSystem<VDimension>::PointType pos

                = ps->GetTransformedPosition(idx, d);



    // Assumes number of particles is equal in all domains

    if (d == 0) m_NumberOfParticles++;

  }

  

//   virtual void PositionSetEventCallback(Object *o, const EventObject &e) 

//   {

//     const itk::ParticlePositionSetEvent &event = dynamic_cast<const itk::ParticlePositionSetEvent &>(e);

//     const itk::ParticleSystem<VDimension> *ps= dynamic_cast<const itk::ParticleSystem<VDimension> *>(o);

//     const int d = event.GetDomainIndex();

//     const unsigned int idx = event.GetPositionIndex();

//     const typename itk::ParticleSystem<VDimension>::PointType pos = ps->GetTransformedPosition(idx, d);



//     // Find and set the values for the given particle position.



//   }

  

//   virtual void PositionRemoveEventCallback(Object *, const EventObject &) 

//   {

//     // NEED TO IMPLEMENT THIS

//   }

  



  /** This method is a hack to work around some tricky design isssues */

  void AddDomain()

  {

    m_ScalarInterpolators.push_back(std::vector<typename ScalarInterpolatorType::Pointer>());

    m_GradientInterpolators.push_back(std::vector<typename GradientInterpolatorType::Pointer>());

    //    m_FunctionImages.push_back(std::vector<typename ImageType::Pointer>());

    //    m_GradientImages.push_back(std::vector<typename GradientImageType::Pointer>()); 

  }

  



  

protected:

  ParticleFunctionBasedShapeSpaceData()

  {

    m_NumberOfParticles = 0;

    m_NumberOfSamples = 0;

    m_NumberOfFunctions = 0;

    m_ParticleSystem = 0;

    this->m_DefinedCallbacks.DomainAddEvent = true;

    this->m_DefinedCallbacks.PositionAddEvent = true;

//     this->m_DefinedCallbacks.PositionSetEvent = true;

//     this->m_DefinedCallbacks.PositionRemoveEvent = true;

  }

  virtual ~ParticleFunctionBasedShapeSpaceData() {};



  void PrintSelf(std::ostream& os, Indent indent) const

  {  Superclass::PrintSelf(os,indent);  }





private:

  ParticleFunctionBasedShapeSpaceData(const Self&); //purposely not implemented

  void operator=(const Self&); //purposely not implemented



  /** The number of samples corresponds to the number of domains in the

      particle system. */

  int m_NumberOfSamples;



  /** The number of dimensions is equal to the number of functions times the

      number of particles, so we need to cache the number of particles here. */

  int m_NumberOfParticles;



  /** */

  int m_NumberOfFunctions;



  // Used to cache the particle system that this class is registered with.  This

  // is a slight hack...

  ParticleSystem<VDimension> *m_ParticleSystem;



  //  std::vector<std::vector<typename ImageType::Pointer> > m_FunctionImages;

  //  std::vector<std::vector<typename GradientImageType::Point> > m_GradientImages;

  

  std::vector<std::vector<typename GradientInterpolatorType::Pointer> > m_GradientInterpolators;

  std::vector<std::vector<typename ScalarInterpolatorType::Pointer> > m_ScalarInterpolators;



};



} // end namespace



#endif


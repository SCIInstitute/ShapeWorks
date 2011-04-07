/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkParticleImageDomainWithGradients.h,v $

  Date:      $Date: 2011/03/24 01:17:33 $

  Version:   $Revision: 1.3 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkParticleImageDomainWithGradients_h

#define __itkParticleImageDomainWithGradients_h



#include "itkImage.h"

#include "itkImageDuplicator.h"

#include "itkParticleImageDomain.h"

#include "itkVectorLinearInterpolateImageFunction.h"

#include "itkGradientImageFilter.h"

#include "itkDerivativeImageFilter.h"

#include "itkFixedArray.h"



namespace itk

{

/** \class ParticleImageDomainWithGradients

 *

 * An image domain that extends ParticleImageDomainWithGradients with image

 * gradient information.  Gradient values are interpolated with the

 * SampleGradients(point) method.

 *

 * \sa ParticleImageDomain

 * \sa ParticleClipRegionDomain

 * \sa ParticleDomain

 */

template <class T, unsigned int VDimension=3>

class ITK_EXPORT ParticleImageDomainWithGradients : public ParticleImageDomain<T, VDimension>

{

public:

  /** Standard class typedefs */

  typedef ParticleImageDomainWithGradients Self;

  typedef ParticleImageDomain<T, VDimension> Superclass;

  typedef SmartPointer<Self>  Pointer;

  typedef SmartPointer<const Self> ConstPointer;

  typedef WeakPointer<const Self>  ConstWeakPointer;



    /** Point type of the domain (not necessarily of the image). */

  typedef typename Superclass::PointType PointType;

  

  typedef typename Superclass::ImageType ImageType;

  typedef typename Superclass::ScalarInterpolatorType ScalarInterpolatorType;

  typedef GradientImageFilter<ImageType> GradientImageFilterType;

  typedef typename GradientImageFilterType::OutputImageType GradientImageType;

  typedef VectorLinearInterpolateImageFunction<GradientImageType, typename PointType::CoordRepType>

  GradientInterpolatorType;

  typedef DerivativeImageFilter< ImageType, ImageType > DerivativeFilterType;



  typedef FixedArray<T, 3> VectorType;

  typedef vnl_vector_fixed<T, 3> VnlVectorType;

  

  /** Method for creation through the object factory. */

  itkNewMacro(Self);



  /** Run-time type information (and related methods). */

  itkTypeMacro(ParticleImageDomainWithGradients, ParticleImageDomain);



  /** Dimensionality of the domain of the particle system. */

  itkStaticConstMacro(Dimension, unsigned int, VDimension);



  /** Set/Get the itk::Image specifying the particle domain.  The set method

      modifies the parent class LowerBound and UpperBound. */

  void SetImage(ImageType *I)

  {

    Superclass::SetImage(I);

 

    // Compute gradient image and set up gradient interpolation.

    typename GradientImageFilterType::Pointer filter = GradientImageFilterType::New();

    filter->SetInput(I);

    filter->SetUseImageSpacingOn();

    filter->Update();

    m_GradientImage = filter->GetOutput();

    

    m_GradientInterpolator->SetInputImage(m_GradientImage);



    ComputeNormalMaps();

    ComputeNormalPartialDerivatives();

  }

  itkGetObjectMacro(GradientImage, GradientImageType);





  /** Sample the image at a point.  This method performs no bounds checking.

      To check bounds, use IsInsideBuffer.  SampleGradientsVnl returns a vnl

      vector of length VDimension instead of an itk::CovariantVector

      (itk::FixedArray). */

  inline VectorType SampleGradient(const PointType &p) const

  {   return  m_GradientInterpolator->Evaluate(p);  }

  inline VnlVectorType SampleGradientVnl(const PointType &p) const

  { return VnlVectorType( this->SampleGradient(p).GetDataPointer() ); }

  inline VnlVectorType SampleNormalVnl(const PointType &p, T epsilon = 1.0e-5) const

  {

    VnlVectorType grad = this->SampleGradientVnl(p);

    double q = 1.0 / (grad.magnitude() + epsilon);

    for (unsigned int i = 0; i < VDimension; i++) { grad[i] *= q; }

    return grad;

  }

  

  /** Allow public access to the scalar interpolator. */

  itkGetObjectMacro(GradientInterpolator, GradientInterpolatorType);





  /** This method is called by an optimizer after a call to Evaluate and may be

      used to apply any constraints the resulting vector, such as a projection

      to the surface tangent plane. Returns true if the gradient was modified.*/

  virtual bool ApplyVectorConstraints(vnl_vector_fixed<double, VDimension> &gradE,

                                      const PointType &pos,

                                      double maxtimestep) const

    

  {

    if (this->m_ConstraintsEnabled == true)

      {

      const double epsilon = 1.0e-10;

      

      double dotprod = 0.0;  

      VnlVectorType normal =  this->SampleNormalVnl(pos, epsilon);

      for (unsigned int i = 0; i < VDimension; i++) {   dotprod  += normal[i] * gradE[i]; }

      for (unsigned int i = 0; i < VDimension; i++) {   gradE[i] -= normal[i] * dotprod; }

     

      return true;

      }

    else return false;

  }

  

  inline void ComputeNormalMaps()

  {

    m_NormalImageX = ImageType::New();

    m_NormalImageY = ImageType::New();

    m_NormalImageZ = ImageType::New();



    typename itk::ImageDuplicator<ImageType>::Pointer imgDuplicator = itk::ImageDuplicator<ImageType>::New();

    imgDuplicator->SetInputImage(Superclass::GetImage());

    imgDuplicator->Update();



    m_NormalImageX = imgDuplicator->GetOutput();

    m_NormalImageY = imgDuplicator->GetOutput();

    m_NormalImageZ = imgDuplicator->GetOutput();



    typedef itk::ImageRegionIterator< ImageType > IteratorType;

    IteratorType itX(m_NormalImageX,m_NormalImageX->GetRequestedRegion());

    IteratorType itY(m_NormalImageY,m_NormalImageY->GetRequestedRegion());

    IteratorType itZ(m_NormalImageZ,m_NormalImageZ->GetRequestedRegion());

    PointType it_point;

    VnlVectorType vecNormal;



    for (itX = itX.Begin(); !itX.IsAtEnd(); ++itX,++itY,++itZ)

    {

      m_NormalImageX->TransformIndexToPhysicalPoint(itX.GetIndex(),it_point);

      vecNormal = SampleNormalVnl(it_point);

      itX.Set(vecNormal[0]);

      itY.Set(vecNormal[1]);

      itZ.Set(vecNormal[2]);

    }    

  }



  inline void ComputeNormalPartialDerivatives()

  {

    pdFilter_nx_x = DerivativeFilterType::New();

    pdFilter_nx_y = DerivativeFilterType::New();

    pdFilter_nx_z = DerivativeFilterType::New();

                    

    pdFilter_ny_x = DerivativeFilterType::New();

    pdFilter_ny_y = DerivativeFilterType::New();

    pdFilter_ny_z = DerivativeFilterType::New();

                    

    pdFilter_nz_x = DerivativeFilterType::New();

    pdFilter_nz_y = DerivativeFilterType::New();

    pdFilter_nz_z = DerivativeFilterType::New();



    pdFilter_nx_x->SetInput(m_NormalImageX);

    pdFilter_nx_y->SetInput(m_NormalImageX);

    pdFilter_nx_z->SetInput(m_NormalImageX);

                 

    pdFilter_ny_x->SetInput(m_NormalImageY);

    pdFilter_ny_y->SetInput(m_NormalImageY);

    pdFilter_ny_z->SetInput(m_NormalImageY);

                 

    pdFilter_nz_x->SetInput(m_NormalImageZ);

    pdFilter_nz_y->SetInput(m_NormalImageZ);

    pdFilter_nz_z->SetInput(m_NormalImageZ);



    pdFilter_nx_x->SetDirection(0);

    pdFilter_nx_y->SetDirection(1);

    pdFilter_nx_z->SetDirection(2);

                 

    pdFilter_ny_x->SetDirection(0);

    pdFilter_ny_y->SetDirection(1);

    pdFilter_ny_z->SetDirection(2);

                 

    pdFilter_nz_x->SetDirection(0);

    pdFilter_nz_y->SetDirection(1);

    pdFilter_nz_z->SetDirection(2);



    pdFilter_nx_x->Update();

    pdFilter_nx_y->Update();

    pdFilter_nx_z->Update();

                 

    pdFilter_ny_x->Update();

    pdFilter_ny_y->Update();

    pdFilter_ny_z->Update();

                 

    pdFilter_nz_x->Update();

    pdFilter_nz_y->Update();

    pdFilter_nz_z->Update();

  }



  inline float SampleNormalPartialDerivative(unsigned int nDir, unsigned int dDir, const PointType &pt) const

  {

    switch (nDir)

    {

    case 1:

      switch (dDir)

      {

      case 1:

        dirDerivativeInterpolator->SetInputImage(pdFilter_ny_y->GetOutput());

        break;

      case 2:

        dirDerivativeInterpolator->SetInputImage(pdFilter_ny_z->GetOutput());

        break;

      case 0:

      default:

        dirDerivativeInterpolator->SetInputImage(pdFilter_ny_x->GetOutput());

        break;

      }

      break;

    case 2:

      switch (dDir)

      {

      case 1:

        dirDerivativeInterpolator->SetInputImage(pdFilter_nz_y->GetOutput());

        break;

      case 2:

        dirDerivativeInterpolator->SetInputImage(pdFilter_nz_z->GetOutput());

        break;

      case 0:

      default:

        dirDerivativeInterpolator->SetInputImage(pdFilter_nz_x->GetOutput());

        break;

      }

      break;

    case 0:

    default:

      switch (dDir)

      {

      case 1:

        dirDerivativeInterpolator->SetInputImage(pdFilter_nx_y->GetOutput());

        break;

      case 2:

        dirDerivativeInterpolator->SetInputImage(pdFilter_nx_z->GetOutput());

        break;

      case 0:

      default:

        dirDerivativeInterpolator->SetInputImage(pdFilter_nx_x->GetOutput());

        break;

      }

      break;

    }



    return dirDerivativeInterpolator->Evaluate(pt);

  }



protected:

  ParticleImageDomainWithGradients()

  {

    m_GradientInterpolator = GradientInterpolatorType::New();

    dirDerivativeInterpolator = ScalarInterpolatorType::New();

  }



  void PrintSelf(std::ostream& os, Indent indent) const

  {

    Superclass::PrintSelf(os, indent);

    os << indent << "m_GradientImage = " << m_GradientImage << std::endl;

    os << indent << "m_GradientInterpolator = " << m_GradientInterpolator << std::endl;

  }

  virtual ~ParticleImageDomainWithGradients() {};

  

private:

  ParticleImageDomainWithGradients(const Self&); //purposely not implemented

  void operator=(const Self&); //purposely not implemented



  typename GradientImageType::Pointer m_GradientImage;

  typename GradientInterpolatorType::Pointer m_GradientInterpolator;



  typename ImageType::Pointer m_NormalImageX;

  typename ImageType::Pointer m_NormalImageY;

  typename ImageType::Pointer m_NormalImageZ;

  

  typename DerivativeFilterType::Pointer  pdFilter_nx_x;

  typename DerivativeFilterType::Pointer  pdFilter_nx_y;

  typename DerivativeFilterType::Pointer  pdFilter_nx_z;



  typename DerivativeFilterType::Pointer  pdFilter_ny_x;

  typename DerivativeFilterType::Pointer  pdFilter_ny_y;

  typename DerivativeFilterType::Pointer  pdFilter_ny_z;



  typename DerivativeFilterType::Pointer  pdFilter_nz_x;

  typename DerivativeFilterType::Pointer  pdFilter_nz_y;

  typename DerivativeFilterType::Pointer  pdFilter_nz_z;





  typename ScalarInterpolatorType::Pointer dirDerivativeInterpolator;

};



} // end namespace itk





#if ITK_TEMPLATE_EXPLICIT

//# include "Templates/itkParticleImageDomainWithGradients+-.h"

#endif



#if ITK_TEMPLATE_TXX

//# include "itkParticleImageDomainWithGradients.txx"

#endif



#endif


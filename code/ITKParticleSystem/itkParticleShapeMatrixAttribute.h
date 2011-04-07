/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkParticleShapeMatrixAttribute.h,v $

  Date:      $Date: 2011/03/24 01:17:34 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkParticleShapeMatrixAttribute_h

#define __itkParticleShapeMatrixAttribute_h



#include "itkDataObject.h"

#include "itkWeakPointer.h"

#include "itkParticleAttribute.h"

#include "itkParticleContainer.h"

#include "vnl/vnl_matrix.h"



namespace itk

{

/** \class ParticleShapeMatrixAttribute

 *

 * \brief Each column describes a shape.  A shape may be composed of

 * m_DomainsPerShape domains (default 1). ALL DOMAINS ARE ASSUMED TO HAVE THE

 * SAME NUMBER OF PARTICLES!  This is for simplicity and should change if this

 * method is to be applied to general classes of multi-domain shapes.

 *

 *

 * Each column represents a single shape.

 */

template <class T, unsigned int VDimension>

class ITK_EXPORT ParticleShapeMatrixAttribute

  : public vnl_matrix<T>, public ParticleAttribute<VDimension>

{

public:

  /** Standard class typedefs */

  typedef T DataType;

  typedef ParticleShapeMatrixAttribute Self;

  typedef ParticleAttribute<VDimension> Superclass;

  typedef SmartPointer<Self>  Pointer;

  typedef SmartPointer<const Self>  ConstPointer;

  typedef WeakPointer<const Self>  ConstWeakPointer;

  

  /** Method for creation through the object factory. */

  itkNewMacro(Self);

  

  /** Run-time type information (and related methods). */

  itkTypeMacro(ParticleShapeMatrixAttribute, ParticleAttribute);

  

  /** Callbacks that may be defined by a subclass.  If a subclass defines one

      of these callback methods, the corresponding flag in m_DefinedCallbacks

      should be set to true so that the ParticleSystem will know to register

      the appropriate event with this method. */

  virtual void DomainAddEventCallback(Object *, const EventObject &e)

  {

    const itk::ParticleDomainAddEvent &event = dynamic_cast<const itk::ParticleDomainAddEvent &>(e);

    unsigned int d = event.GetDomainIndex();



    if ( d % m_DomainsPerShape  == 0 )

      {

      this->ResizeMatrix(this->rows(), this->cols()+1);

      }

  }

  

  virtual void ResizeMatrix(int rs, int cs)

  {

    vnl_matrix<T> tmp(*this); // copy existing  matrix

    

    // Create new column (shape)

    this->set_size(rs, cs);

    

    // Copy old data into new matrix.

    for (unsigned int c = 0; c < tmp.cols(); c++)

      {

      for (unsigned int r = 0; r < tmp.rows(); r++)

        {

        this->operator()(r,c) = tmp(r,c);

        }

      } 

  }

  

  virtual void PositionAddEventCallback(Object *o, const EventObject &e) 

  {

    const itk::ParticlePositionAddEvent &event = dynamic_cast<const itk::ParticlePositionAddEvent &>(e);

    const itk::ParticleSystem<VDimension> *ps= dynamic_cast<const itk::ParticleSystem<VDimension> *>(o);

    const int d = event.GetDomainIndex();

    const unsigned int idx = event.GetPositionIndex();

    const typename itk::ParticleSystem<VDimension>::PointType pos = ps->GetTransformedPosition(idx, d);





  //  std::cout << ps->GetPosition(idx,d) << "\t" << ps->GetTransformedPosition(idx,d) << std::endl;

  

    const unsigned int PointsPerDomain = ps ->GetNumberOfParticles(d);



    // Make sure we have enough rows.

    if ((ps->GetNumberOfParticles(d) * VDimension * m_DomainsPerShape) > this->rows())

      {

      this->ResizeMatrix(PointsPerDomain * VDimension * m_DomainsPerShape, this->cols());

      }



    // CANNOT ADD POSITION INFO UNTIL ALL POINTS PER DOMAIN IS KNOWN

    // Add position info to the matrix

        unsigned int k = ((d % m_DomainsPerShape) * PointsPerDomain * VDimension)

                             + (idx * VDimension);

        for (unsigned int i = 0; i < VDimension; i++)

          {

          this->operator()(i+k, d / m_DomainsPerShape) = pos[i];

          }

    

    

    //   std::cout << "Row " << k << " Col " << d / m_DomainsPerShape << " = " << pos << std::endl;

  }

  

  virtual void PositionSetEventCallback(Object *o, const EventObject &e) 

  {

    const itk::ParticlePositionSetEvent &event = dynamic_cast<const itk::ParticlePositionSetEvent &>(e);

    const itk::ParticleSystem<VDimension> *ps= dynamic_cast<const itk::ParticleSystem<VDimension> *>(o);

    const int d = event.GetDomainIndex();

    const unsigned int idx = event.GetPositionIndex();

    const typename itk::ParticleSystem<VDimension>::PointType pos = ps->GetTransformedPosition(idx, d);

    const unsigned int PointsPerDomain = ps ->GetNumberOfParticles(d);



    // Modify matrix info

    //    unsigned int k = VDimension * idx;

    unsigned int k = ((d % m_DomainsPerShape) * PointsPerDomain * VDimension)

                         + (idx * VDimension);

    for (unsigned int i = 0; i < VDimension; i++)

      {

      this->operator()(i+k, d / m_DomainsPerShape) = pos[i];

      }

  }

  

  virtual void PositionRemoveEventCallback(Object *, const EventObject &) 

  {

    // NEED TO IMPLEMENT THIS

  }

  

  // For debugging purposes.

  //  void PrintMatrix()

  //  {

  //    std::cout << std::endl;

  //    for (unsigned int r = 0; r < this->rows(); r++)

  //      {

  //      for (unsigned int c = 0; c < this->cols(); c++)

  //        {

  //        std::cout << this->operator()(r, c) << "  ";

  //        }

  //    std::cout << std::endl;

  //      }

  //  }



  /** Set/Get the number of domains per shape.  This can only be safely done

      before shapes are initialized with points! */

  void SetDomainsPerShape(int i)

  { m_DomainsPerShape = i; }

  int GetDomainsPerShape() const

  { return m_DomainsPerShape; }





  virtual void BeforeIteration() {}

  virtual void AfterIteration() {}



  virtual void SetMatrix(const vnl_matrix<T> &m)

  {

    vnl_matrix<T>::operator=(m);

  }

  

protected:

  ParticleShapeMatrixAttribute() : m_DomainsPerShape(1)

  {

    this->m_DefinedCallbacks.DomainAddEvent = true;

    this->m_DefinedCallbacks.PositionAddEvent = true;

    this->m_DefinedCallbacks.PositionSetEvent = true;

    this->m_DefinedCallbacks.PositionRemoveEvent = true;

  }

  virtual ~ParticleShapeMatrixAttribute() {};



  void PrintSelf(std::ostream& os, Indent indent) const

  {  Superclass::PrintSelf(os,indent);  }



    int m_DomainsPerShape;

private:

  ParticleShapeMatrixAttribute(const Self&); //purposely not implemented

  void operator=(const Self&); //purposely not implemented





};



} // end namespace



#endif


/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEnsembleMeanFunction.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEnsembleMeanFunction_h
#define __itkParticleEnsembleMeanFunction_h

#include "itkParticleShapeMatrixAttribute.h"
#include "itkParticleVectorFunction.h"
#include <vector>

namespace itk
{

/**
 * \class ParticleEnsembleMeanFunction
 *
 */
template <unsigned int VDimension>
class ParticleEnsembleMeanFunction : public ParticleVectorFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef ParticleEnsembleMeanFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleVectorFunction<VDimension> Superclass;
  itkTypeMacro( ParticleEnsembleMeanFunction, ParticleVectorFunction);

  /** Type of particle system. */
  typedef typename Superclass::ParticleSystemType ParticleSystemType;
  typedef ParticleShapeMatrixAttribute<double, VDimension> ShapeMatrixType;
  typedef typename ShapeMatrixType::DataType DataType;

  /** Vector & Point types. */
  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystemType::PointType PointType;
  typedef vnl_vector<DataType> vnl_vector_type;
  typedef vnl_matrix<DataType> vnl_matrix_type;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  virtual VectorType Evaluate(unsigned int a, unsigned int b,
                              const ParticleSystemType *c, double &d, double &e) const;

  virtual VectorType Evaluate(unsigned int a,  unsigned int b,
                              const ParticleSystemType *c, double &d) const
  {
    double e;
    return this->Evaluate(a,b,c,d,e);
  }

  virtual double Energy(unsigned int a, unsigned int b,
                        const ParticleSystemType *c) const
  {
    double e, d;
    this->Evaluate(a,b,c,d,e);
    return e;
  }

  virtual void BeforeIteration()
  {
    m_ShapeMatrix->BeforeIteration();
    this->ComputeUpdates();

  }

  /**Access the shape matrix. */
  void SetShapeMatrix( ShapeMatrixType *s)
  {    m_ShapeMatrix = s;  }
  ShapeMatrixType *GetShapeMatrix()
  {   return  m_ShapeMatrix.GetPointer();  }
  const ShapeMatrixType *GetShapeMatrix() const
  {   return  m_ShapeMatrix.GetPointer();  }

  void PrintShapeMatrix()
  {
      m_ShapeMatrix->PrintMatrix();
  }


  /** Set/Get the number of domains per shape.  Separate means are used for
      separate shape domains. */
  void SetDomainsPerShape(int i)
  { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const
  { return m_DomainsPerShape; }

  virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
  {
    typename ParticleEnsembleMeanFunction<VDimension>::Pointer copy = ParticleEnsembleMeanFunction<VDimension>::New();

    // from itkParticleVectorFunction
    copy->m_DomainNumber    = this->m_DomainNumber;
    copy->m_ParticleSystem  = this->m_ParticleSystem;

    // local
    copy->m_DomainsPerShape = this->m_DomainsPerShape;
    copy->m_ShapeMatrix     = this->m_ShapeMatrix;
    copy->m_PointsUpdate    = this->m_PointsUpdate;

    return (typename ParticleVectorFunction<VDimension>::Pointer)copy;

  }
protected:
  ParticleEnsembleMeanFunction() : m_DomainsPerShape(1) {}
  virtual ~ParticleEnsembleMeanFunction() {}
  void operator=(const ParticleEnsembleMeanFunction &);
  ParticleEnsembleMeanFunction(const ParticleEnsembleMeanFunction &);

  virtual void ComputeUpdates();

  typename ShapeMatrixType::Pointer m_ShapeMatrix;
  vnl_matrix_type m_PointsUpdate;

  int m_DomainsPerShape;

};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleEnsembleMeanFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleEnsembleMeanFunction.txx"
#endif

#endif

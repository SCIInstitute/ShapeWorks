/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleGaussianModeWriter.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleGaussianModeWriter_h
#define __itkParticleGaussianModeWriter_h

#include "itkParticleShapeMatrixAttribute.h"
#include "itkDataObject.h"
#include <vector>
#include <string>

namespace itk
{

/**
 * \class ParticleGaussianModeWriter
 *
 */
template <unsigned int VDimension>
class ParticleGaussianModeWriter : public DataObject
{
public:
 /** Standard class typedefs. */
  typedef ParticleGaussianModeWriter Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef DataObject Superclass;
  itkTypeMacro( ParticleGaussianModeWriter, DataObject);

  /** Type of particle system and associated classes. */
  typedef ParticleSystem<VDimension> ParticleSystemType;
  typedef ParticleShapeMatrixAttribute<double, VDimension> ShapeMatrixType;
  typedef typename ShapeMatrixType::DataType DataType;
  
  /** Vector & Point types. */
  typedef typename ParticleSystemType::PointType PointType;
  typedef vnl_vector<DataType> vnl_vector_type;
  typedef vnl_matrix<DataType> vnl_matrix_type;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Write the first n modes to +- 3 std dev and the mean of the model
      described by the covariance matrix.  */
  void Update() const;
  
  int GetNumberOfModes() const
  { return m_NumberOfModes; }
  void SetNumberOfModes(int b)
  { m_NumberOfModes = b; }

  /**Access the shape matrix. */
  void SetShapeMatrix( ShapeMatrixType *s)
  {    m_ShapeMatrix = s;  }
  ShapeMatrixType *GetShapeMatrix()
  {   return  m_ShapeMatrix.GetPointer();  }
  const ShapeMatrixType *GetShapeMatrix() const
  {   return  m_ShapeMatrix.GetPointer();  }

  /** Set/Get the filename prefix. */
  void SetFileName(const std::string &s)
  { m_FileName = s; }
  void SetFileName(const char *s)
  { m_FileName = s; }
  const std::string &GetFileName() const
  { return m_FileName; }
    
protected:
  ParticleGaussianModeWriter() : m_NumberOfModes(3) {  }
  virtual ~ParticleGaussianModeWriter() {}
  void operator=(const ParticleGaussianModeWriter &);
  ParticleGaussianModeWriter(const ParticleGaussianModeWriter &);
  typename ShapeMatrixType::Pointer m_ShapeMatrix;

  int m_NumberOfModes;
  std::string m_FileName;

};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleGaussianModeWriter+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleGaussianModeWriter.txx"
#endif

#include "itkParticleGaussianModeWriter.txx"

#endif


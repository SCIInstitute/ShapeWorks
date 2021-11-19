---
title: Libs/Alignment/Transforms/itkKernelTransform2.h

---

# Libs/Alignment/Transforms/itkKernelTransform2.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::KernelTransform2](../Classes/classitk_1_1KernelTransform2.md)**  |




## Source code

```cpp
/*=========================================================================
 *
 *  Copyright UMC Utrecht and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkKernelTransform2.h,v $
Language:  C++
Date:      $Date: 2006-11-28 14:22:18 $
Version:   $Revision: 1.1 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkKernelTransform2_h
#define __itkKernelTransform2_h

#include "itkAdvancedTransform.h"
#include "itkPoint.h"
#include "itkVector.h"
#include "itkMatrix.h"
#include "itkPointSet.h"
#include <deque>
#include <math.h>
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_sample.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_qr.h"

namespace itk
{

template< class TScalarType, // probably only float and double make sense here
unsigned int NDimensions >
// Number of dimensions
class KernelTransform2 :
  public AdvancedTransform< TScalarType, NDimensions, NDimensions >
{
public:

  typedef KernelTransform2 Self;
  typedef AdvancedTransform<
    TScalarType, NDimensions, NDimensions > Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  itkTypeMacro( KernelTransform2, AdvancedTransform );

  itkNewMacro( Self );

  itkStaticConstMacro( SpaceDimension, unsigned int, NDimensions );

  typedef typename Superclass::ScalarType                ScalarType;
  typedef typename Superclass::ParametersType            ParametersType;
  typedef typename Superclass::NumberOfParametersType    NumberOfParametersType;
  typedef typename Superclass::JacobianType              JacobianType;
  typedef typename Superclass::InputPointType            InputPointType;
  typedef typename Superclass::OutputPointType           OutputPointType;
  typedef typename Superclass::InputVectorType           InputVectorType;
  typedef typename Superclass::OutputVectorType          OutputVectorType;
  typedef typename Superclass::InputCovariantVectorType  InputCovariantVectorType;
  typedef typename Superclass::OutputCovariantVectorType OutputCovariantVectorType;
  typedef typename Superclass::InputVnlVectorType        InputVnlVectorType;
  typedef typename Superclass::OutputVnlVectorType       OutputVnlVectorType;

  typedef typename Superclass
    ::NonZeroJacobianIndicesType NonZeroJacobianIndicesType;
  typedef typename Superclass::SpatialJacobianType SpatialJacobianType;
  typedef typename Superclass
    ::JacobianOfSpatialJacobianType JacobianOfSpatialJacobianType;
  typedef typename Superclass::SpatialHessianType SpatialHessianType;
  typedef typename Superclass
    ::JacobianOfSpatialHessianType JacobianOfSpatialHessianType;
  typedef typename Superclass::InternalMatrixType InternalMatrixType;

  typedef DefaultStaticMeshTraits< TScalarType,
    NDimensions, NDimensions, TScalarType, TScalarType >       PointSetTraitsType;
  typedef PointSet< InputPointType, NDimensions,
    PointSetTraitsType >                                       PointSetType;
  typedef typename PointSetType::Pointer                      PointSetPointer;
  typedef typename PointSetType::PointsContainer              PointsContainer;
  typedef typename PointSetType::PointsContainerIterator      PointsIterator;
  typedef typename PointSetType::PointsContainerConstIterator PointsConstIterator;

  typedef VectorContainer< unsigned long, InputVectorType > VectorSetType;
  typedef typename VectorSetType::Pointer                   VectorSetPointer;

  typedef vnl_matrix_fixed< TScalarType, NDimensions, NDimensions > IMatrixType;

  virtual NumberOfParametersType GetNumberOfParameters( void ) const
  {
    return ( this->m_SourceLandmarks->GetNumberOfPoints() * SpaceDimension );
  }


  itkGetObjectMacro( SourceLandmarks, PointSetType );

  virtual void SetSourceLandmarks( PointSetType * );

  itkGetObjectMacro( TargetLandmarks, PointSetType );

  virtual void SetTargetLandmarks( PointSetType * );

  itkGetObjectMacro( Displacements, VectorSetType );

  void ComputeWMatrix( void );

  void ComputeLInverse( void );

  virtual OutputPointType TransformPoint( const InputPointType & thisPoint ) const;

  virtual OutputVectorType TransformVector( const InputVectorType & ) const
  {
    itkExceptionMacro(
        << "TransformVector(const InputVectorType &) is not implemented "
        << "for KernelTransform" );
  }


  virtual OutputVnlVectorType TransformVector( const InputVnlVectorType & ) const
  {
    itkExceptionMacro(
        << "TransformVector(const InputVnlVectorType &) is not implemented "
        << "for KernelTransform" );
  }


  virtual OutputCovariantVectorType TransformCovariantVector( const InputCovariantVectorType & ) const
  {
    itkExceptionMacro(
        << "TransformCovariantVector(const InputCovariantVectorType &) is not implemented "
        << "for KernelTransform" );
  }


  virtual void GetJacobian(
    const InputPointType &,
    JacobianType &,
    NonZeroJacobianIndicesType & ) const;

  virtual void SetIdentity( void );

  virtual void SetParameters( const ParametersType & );

  virtual void SetFixedParameters( const ParametersType & );

  virtual void UpdateParameters( void );

  virtual const ParametersType & GetParameters( void ) const;

  virtual const ParametersType & GetFixedParameters( void ) const;

  virtual void SetStiffness( double stiffness )
  {
    this->m_Stiffness        = stiffness > 0 ? stiffness : 0.0;
    this->m_LMatrixComputed  = false;
    this->m_LInverseComputed = false;
    this->m_WMatrixComputed  = false;
  }


  itkGetMacro( Stiffness, double );

  virtual void SetAlpha( TScalarType itkNotUsed( Alpha ) ) {}
  virtual TScalarType GetAlpha( void ) const { return -1.0; }

  itkSetMacro( PoissonRatio, TScalarType );
  virtual const TScalarType GetPoissonRatio( void ) const
  {
    return this->m_PoissonRatio;
  }


  itkSetMacro( MatrixInversionMethod, std::string );
  itkGetConstReferenceMacro( MatrixInversionMethod, std::string );

  virtual void GetSpatialJacobian(
    const InputPointType & ipp, SpatialJacobianType & sj ) const
  {
    itkExceptionMacro( << "Not implemented for KernelTransform2" );
  }


  virtual void GetSpatialHessian(
    const InputPointType & ipp, SpatialHessianType & sh ) const
  {
    itkExceptionMacro( << "Not implemented for KernelTransform2" );
  }


  virtual void GetJacobianOfSpatialJacobian(
    const InputPointType & ipp, JacobianOfSpatialJacobianType & jsj,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const
  {
    itkExceptionMacro( << "Not implemented for KernelTransform2" );
  }


  virtual void GetJacobianOfSpatialJacobian(
    const InputPointType & ipp, SpatialJacobianType & sj,
    JacobianOfSpatialJacobianType & jsj,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const
  {
    itkExceptionMacro( << "Not implemented for KernelTransform2" );
  }


  virtual void GetJacobianOfSpatialHessian(
    const InputPointType & ipp, JacobianOfSpatialHessianType & jsh,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const
  {
    itkExceptionMacro( << "Not implemented for KernelTransform2" );
  }


  virtual void GetJacobianOfSpatialHessian(
    const InputPointType & ipp, SpatialHessianType & sh,
    JacobianOfSpatialHessianType & jsh,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const
  {
    itkExceptionMacro( << "Not implemented for KernelTransform2" );
  }


protected:

  KernelTransform2();
  virtual ~KernelTransform2();
  void PrintSelf( std::ostream & os, Indent indent ) const;

public:

  typedef vnl_matrix_fixed< TScalarType, NDimensions, NDimensions > GMatrixType;

  typedef vnl_matrix< TScalarType > LMatrixType;

  typedef vnl_matrix< TScalarType > KMatrixType;

  typedef vnl_matrix< TScalarType > PMatrixType;

  typedef vnl_matrix< TScalarType > YMatrixType;

  typedef vnl_matrix< TScalarType > WMatrixType;

  typedef vnl_matrix< TScalarType > DMatrixType;

  typedef vnl_matrix_fixed< TScalarType, NDimensions, NDimensions > AMatrixType;

  typedef vnl_vector_fixed< TScalarType, NDimensions > BMatrixType;

  typedef vnl_matrix_fixed< TScalarType, 1, NDimensions > RowMatrixType;

  typedef vnl_matrix_fixed< TScalarType, NDimensions, 1 > ColumnMatrixType;

  PointSetPointer m_SourceLandmarks;

  PointSetPointer m_TargetLandmarks;

protected:

  virtual void ComputeG( const InputVectorType & landmarkVector,
    GMatrixType & GMatrix ) const;

  virtual void ComputeReflexiveG( PointsIterator, GMatrixType & GMatrix ) const;

  virtual void ComputeDeformationContribution(
    const InputPointType & inputPoint,
    OutputPointType & result ) const;

  void ComputeK( void );

  void ComputeL( void );

  void ComputeP( void );

  void ComputeY( void );

  void ComputeD( void );

  void ReorganizeW( void );

  double m_Stiffness;

  VectorSetPointer m_Displacements;

  LMatrixType m_LMatrix;

  LMatrixType m_LMatrixInverse;

  KMatrixType m_KMatrix;

  PMatrixType m_PMatrix;

  YMatrixType m_YMatrix;

  WMatrixType m_WMatrix;

  DMatrixType m_DMatrix;

  AMatrixType m_AMatrix;

  BMatrixType m_BVector;

  //GMatrixType m_GMatrix;

  bool m_WMatrixComputed;
  bool m_LMatrixComputed;
  bool m_LInverseComputed;
  bool m_LMatrixDecompositionComputed;

  typedef vnl_svd< ScalarType > SVDDecompositionType;
  typedef vnl_qr< ScalarType >  QRDecompositionType;

  SVDDecompositionType * m_LMatrixDecompositionSVD;
  QRDecompositionType *  m_LMatrixDecompositionQR;

  IMatrixType m_I;

  NonZeroJacobianIndicesType m_NonZeroJacobianIndices;

  mutable NonZeroJacobianIndicesType m_NonZeroJacobianIndicesTemp;

  bool m_FastComputationPossible;

private:

  KernelTransform2( const Self & ); // purposely not implemented
  void operator=( const Self & );   // purposely not implemented

  TScalarType m_PoissonRatio;

  std::string m_MatrixInversionMethod;

};

} // end namespace itk


#include "itkKernelTransform2.cpp"

#endif // __itkKernelTransform2_h
```


-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000

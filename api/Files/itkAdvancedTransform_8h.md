---
title: Libs/Alignment/Transforms/itkAdvancedTransform.h

---

# Libs/Alignment/Transforms/itkAdvancedTransform.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md)** <br>Transform maps points, vectors and covariant vectors from an input space to an output space.  |




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
  Module:    $RCSfile: itkTransform.h,v $
  Language:  C++
  Date:      $Date: 2008-06-29 12:58:58 $
  Version:   $Revision: 1.64 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAdvancedTransform_h
#define __itkAdvancedTransform_h

#include "itkTransform.h"
#include "itkMatrix.h"
#include "itkFixedArray.h"

namespace itk
{

template< class TScalarType,
unsigned int NInputDimensions  = 3,
unsigned int NOutputDimensions = 3 >
class AdvancedTransform :
  public Transform< TScalarType, NInputDimensions, NOutputDimensions >
{
public:

  typedef AdvancedTransform Self;
  typedef Transform< TScalarType,
    NInputDimensions,
    NOutputDimensions >               Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  //itkNewMacro( Self );

  itkTypeMacro( AdvancedTransform, Transform );

  itkStaticConstMacro( InputSpaceDimension, unsigned int, NInputDimensions );
  itkStaticConstMacro( OutputSpaceDimension, unsigned int, NOutputDimensions );

  typedef typename Superclass::ScalarType             ScalarType;
  typedef typename Superclass::ParametersType         ParametersType;
  typedef typename Superclass::ParametersValueType    ParametersValueType;
  typedef typename Superclass::NumberOfParametersType NumberOfParametersType;
  typedef typename Superclass::DerivativeType         DerivativeType;
  typedef typename Superclass::JacobianType           JacobianType;
  typedef typename Superclass::InputVectorType        InputVectorType;
  typedef typename Superclass::OutputVectorType       OutputVectorType;
  typedef typename Superclass
    ::InputCovariantVectorType InputCovariantVectorType;
  typedef typename Superclass
    ::OutputCovariantVectorType OutputCovariantVectorType;
  typedef typename Superclass::InputVnlVectorType  InputVnlVectorType;
  typedef typename Superclass::OutputVnlVectorType OutputVnlVectorType;
  typedef typename Superclass::InputPointType      InputPointType;
  typedef typename Superclass::OutputPointType     OutputPointType;

  typedef typename Superclass::InverseTransformBaseType    InverseTransformBaseType;
  typedef typename Superclass::InverseTransformBasePointer InverseTransformBasePointer;

  typedef Transform< TScalarType,
    NInputDimensions,
    NOutputDimensions >                             TransformType;
  typedef typename TransformType::Pointer      TransformTypePointer;
  typedef typename TransformType::ConstPointer TransformTypeConstPointer;

  typedef std::vector< unsigned long > NonZeroJacobianIndicesType;
  typedef Matrix< ScalarType,
    OutputSpaceDimension, InputSpaceDimension >     SpatialJacobianType;
  typedef std::vector< SpatialJacobianType > JacobianOfSpatialJacobianType;
  // \todo: think about the SpatialHessian type, should be a 3D native type
  typedef FixedArray<
    Matrix< ScalarType,
    InputSpaceDimension, InputSpaceDimension >,
    OutputSpaceDimension >                          SpatialHessianType;
  typedef std::vector< SpatialHessianType >                JacobianOfSpatialHessianType;
  typedef typename SpatialJacobianType::InternalMatrixType InternalMatrixType;

  typedef OutputCovariantVectorType                   MovingImageGradientType;
  typedef typename MovingImageGradientType::ValueType MovingImageGradientValueType;

  virtual NumberOfParametersType GetNumberOfNonZeroJacobianIndices( void ) const;

  itkGetConstMacro( HasNonZeroSpatialHessian, bool );
  itkGetConstMacro( HasNonZeroJacobianOfSpatialHessian, bool );

  virtual void GetJacobian(
    const InputPointType & ipp,
    JacobianType & j,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const = 0;

  virtual void EvaluateJacobianWithImageGradientProduct(
    const InputPointType & ipp,
    const MovingImageGradientType & movingImageGradient,
    DerivativeType & imageJacobian,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const;

  virtual void GetSpatialJacobian(
    const InputPointType & ipp,
    SpatialJacobianType & sj ) const = 0;

  virtual void ComputeJacobianWithRespectToParameters(
    const InputPointType & itkNotUsed( p ), JacobianType & itkNotUsed( j ) ) const
  {
    itkExceptionMacro( << "This ITK4 function is currently not used in elastix." );
  }


  virtual void ComputeJacobianWithRespectToPosition(
    const InputPointType & itkNotUsed( p ), JacobianType & itkNotUsed( j ) ) const
  {
    itkExceptionMacro( << "This ITK4 function is currently not used in elastix." );
  }


  virtual void GetSpatialHessian(
    const InputPointType & ipp,
    SpatialHessianType & sh ) const = 0;

  virtual void GetJacobianOfSpatialJacobian(
    const InputPointType & ipp,
    JacobianOfSpatialJacobianType & jsj,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const = 0;

  virtual void GetJacobianOfSpatialJacobian(
    const InputPointType & ipp,
    SpatialJacobianType & sj,
    JacobianOfSpatialJacobianType & jsj,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const = 0;

  virtual void GetJacobianOfSpatialHessian(
    const InputPointType & ipp,
    JacobianOfSpatialHessianType & jsh,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const = 0;

  virtual void GetJacobianOfSpatialHessian(
    const InputPointType & ipp,
    SpatialHessianType & sh,
    JacobianOfSpatialHessianType & jsh,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const = 0;

protected:

  AdvancedTransform();
  AdvancedTransform( NumberOfParametersType numberOfParameters );
  virtual ~AdvancedTransform() {}

  bool m_HasNonZeroSpatialHessian;
  bool m_HasNonZeroJacobianOfSpatialHessian;

private:

  AdvancedTransform( const Self & ); // purposely not implemented
  void operator=( const Self & );    // purposely not implemented

};

} // end namespace itk

#include "itkAdvancedTransform.cpp"

#endif
```


-------------------------------

Updated on 2022-01-03 at 16:16:10 +0000

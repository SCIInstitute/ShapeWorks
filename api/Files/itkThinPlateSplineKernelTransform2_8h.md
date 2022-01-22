---
title: Libs/Alignment/Transforms/itkThinPlateSplineKernelTransform2.h

---

# Libs/Alignment/Transforms/itkThinPlateSplineKernelTransform2.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ThinPlateSplineKernelTransform2](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md)**  |




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
  Module:    $RCSfile: itkThinPlateSplineKernelTransform2.h,v $
  Language:  C++
  Date:      $Date: 2006-11-28 14:22:18 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkThinPlateSplineKernelTransform2_h
#define __itkThinPlateSplineKernelTransform2_h

#include "itkKernelTransform2.h"

namespace itk
{
template< class TScalarType,         // Data type for scalars (float or double)
unsigned int NDimensions = 3 >
// Number of dimensions
class ThinPlateSplineKernelTransform2 :
  public KernelTransform2< TScalarType, NDimensions >
{
public:

  typedef ThinPlateSplineKernelTransform2              Self;
  typedef KernelTransform2< TScalarType, NDimensions > Superclass;
  typedef SmartPointer< Self >                         Pointer;
  typedef SmartPointer< const Self >                   ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( ThinPlateSplineKernelTransform2, KernelTransform2 );

  typedef typename Superclass::ScalarType ScalarType;

  typedef typename Superclass::ParametersType ParametersType;

  typedef typename Superclass::JacobianType JacobianType;

  itkStaticConstMacro( SpaceDimension, unsigned int, Superclass::SpaceDimension );

  typedef typename Superclass::InputPointType            InputPointType;
  typedef typename Superclass::OutputPointType           OutputPointType;
  typedef typename Superclass::InputVectorType           InputVectorType;
  typedef typename Superclass::OutputVectorType          OutputVectorType;
  typedef typename Superclass::InputCovariantVectorType  InputCovariantVectorType;
  typedef typename Superclass::OutputCovariantVectorType OutputCovariantVectorType;
  typedef typename Superclass::PointsIterator            PointsIterator;

  void SetSigma(double sigma){}; // this is only to match the compact supported class

protected:

  ThinPlateSplineKernelTransform2()
  {
    this->m_FastComputationPossible = true;
  }


  virtual ~ThinPlateSplineKernelTransform2() {}

  typedef typename Superclass::GMatrixType GMatrixType;

  void ComputeG( const InputVectorType & x, GMatrixType & GMatrix ) const;

  virtual void ComputeDeformationContribution(
    const InputPointType & inputPoint, OutputPointType & result ) const;

private:

  ThinPlateSplineKernelTransform2( const Self & ); // purposely not implemented
  void operator=( const Self & );                  // purposely not implemented

};

} // namespace itk

#include "itkThinPlateSplineKernelTransform2.cpp"

#endif // __itkThinPlateSplineKernelTransform2_h
```


-------------------------------

Updated on 2022-01-22 at 00:21:05 +0000

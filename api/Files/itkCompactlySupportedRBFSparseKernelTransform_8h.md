---
title: Libs/Alignment/Transforms/itkCompactlySupportedRBFSparseKernelTransform.h

---

# Libs/Alignment/Transforms/itkCompactlySupportedRBFSparseKernelTransform.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::CompactlySupportedRBFSparseKernelTransform](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md)**  |




## Source code

```cpp
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCompactlySupportedRBFSparseKernelTransform.h,v $
  Language:  C++
  Date:      $Date: 2014-1-28 14:22:18 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCompactlySupportedRBFSparseKernelTransform_h
#define __itkCompactlySupportedRBFSparseKernelTransform_h

#include "itkSparseKernelTransform.h"

namespace itk
{
template <class TScalarType,         // Data type for scalars (float or double)
          unsigned int NDimensions = 3>          // Number of dimensions
class ITK_EXPORT CompactlySupportedRBFSparseKernelTransform :
        public SparseKernelTransform<   TScalarType, NDimensions>
{
public:
    typedef CompactlySupportedRBFSparseKernelTransform Self;
    typedef SparseKernelTransform<    TScalarType, NDimensions>   Superclass;
    typedef SmartPointer<Self>        Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    itkNewMacro( Self );

    itkTypeMacro( CompactlySupportedRBFSparseKernelTransform, SparseKernelTransform );

    typedef typename Superclass::ScalarType  ScalarType;

    typedef typename Superclass::ParametersType  ParametersType;

    typedef typename Superclass::JacobianType  JacobianType;

    itkStaticConstMacro(SpaceDimension, unsigned int,Superclass::SpaceDimension);

    typedef typename Superclass::InputPointType  InputPointType;
    typedef typename Superclass::OutputPointType  OutputPointType;
    typedef typename Superclass::InputVectorType InputVectorType;
    typedef typename Superclass::OutputVectorType OutputVectorType;
    typedef typename Superclass::InputCovariantVectorType InputCovariantVectorType;
    typedef typename Superclass::OutputCovariantVectorType OutputCovariantVectorType;
    typedef typename Superclass::PointsIterator PointsIterator;
    //  void SetParameters( const ParametersType & parameters );

    void SetSigma(double sigma){this->Sigma = sigma;}

    virtual void ComputeJacobianWithRespectToParameters(
        const InputPointType  &in, JacobianType &jacobian) const;


protected:
    CompactlySupportedRBFSparseKernelTransform() {this->Sigma = 1; }
    virtual ~CompactlySupportedRBFSparseKernelTransform() {}

    typedef typename Superclass::GMatrixType GMatrixType;

    const GMatrixType & ComputeG(const InputVectorType & x) const override;

    virtual void ComputeDeformationContribution( const InputPointType & inputPoint,
                                                 OutputPointType & result ) const override;

private:
    CompactlySupportedRBFSparseKernelTransform(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    // basis support
    double Sigma;

};

} // namespace itk

#include "itkCompactlySupportedRBFSparseKernelTransform.cpp"


#endif // __itkCompactlySupportedRBFSparseKernelTransform_h
```


-------------------------------

Updated on 2022-01-07 at 20:14:45 +0000

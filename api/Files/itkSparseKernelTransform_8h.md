---
title: Libs/Alignment/Transforms/itkSparseKernelTransform.h

---

# Libs/Alignment/Transforms/itkSparseKernelTransform.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md)**  |




## Source code

```cpp
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSparseKernelTransform.h,v $
  Language:  C++
  Date:      $Date: 2006-11-28 14:22:18 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSparseKernelTransform_h
#define __itkSparseKernelTransform_h

#include <itkTransform.h>
#include <itkPoint.h>
#include <itkVector.h>
#include <itkMatrix.h>
#include <itkPointSet.h>
#include <deque>
#include <math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_sample.h>

//#define EIGEN_USE_MKL_ALL
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <stdint.h>
#include <iostream>


namespace itk
{

template <class TScalarType, // probably only float and double make sense here
          unsigned int NDimensions>   // Number of dimensions
class ITK_EXPORT SparseKernelTransform :
        public Transform<TScalarType, NDimensions,NDimensions>
{
public:
    typedef SparseKernelTransform Self;
    typedef Transform<TScalarType, NDimensions, NDimensions >   Superclass;
    typedef SmartPointer<Self>        Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    itkTypeMacro( SparseKernelTransform, Transform );

    itkNewMacro( Self );

    itkStaticConstMacro(SpaceDimension, unsigned int, NDimensions);

    typedef typename Superclass::ScalarType  ScalarType;

    typedef typename Superclass::ParametersType  ParametersType;

    typedef typename Superclass::JacobianType  JacobianType;

    typedef typename Superclass::InputPointType   InputPointType;
    typedef typename Superclass::OutputPointType  OutputPointType;

    typedef typename Superclass::InputVectorType   InputVectorType;
    typedef typename Superclass::OutputVectorType  OutputVectorType;

    typedef DefaultStaticMeshTraits<TScalarType,
    NDimensions,
    NDimensions,
    TScalarType,
    TScalarType> PointSetTraitsType;
    typedef PointSet<InputPointType, NDimensions, PointSetTraitsType> PointSetType;
    typedef typename PointSetType::Pointer                        PointSetPointer;
    typedef typename PointSetType::PointsContainer                PointsContainer;
    typedef typename PointSetType::PointsContainerIterator        PointsIterator;
    typedef typename PointSetType::PointsContainerConstIterator   PointsConstIterator;
    
    typedef itk::VectorContainer<unsigned long,InputVectorType> VectorSetType;
    typedef typename VectorSetType::Pointer        VectorSetPointer;

    itkGetObjectMacro( SourceLandmarks, PointSetType);

    virtual void SetSourceLandmarks(PointSetType *);

    itkGetObjectMacro( TargetLandmarks, PointSetType);

    virtual void SetTargetLandmarks(PointSetType *);

    itkGetObjectMacro( Displacements, VectorSetType );

    void ComputeWMatrix(void) const;

    //void ComputeLInverse() const;

    virtual OutputPointType TransformPoint(const InputPointType& thisPoint) const;

    typedef Eigen::Matrix<TScalarType, NDimensions, NDimensions> IMatrixType;
    //typedef vnl_matrix_fixed<TScalarType, NDimensions, NDimensions> IMatrixType;


    //virtual const JacobianType & GetJacobian(const InputPointType  &point ) const;

    virtual void SetIdentity();

    virtual void SetParameters(const ParametersType &);

    virtual void SetFixedParameters(const ParametersType &);

    virtual void UpdateParameters(void) const;

    virtual const ParametersType& GetParameters(void) const;

    virtual const ParametersType& GetFixedParameters(void) const;
    virtual void ComputeJacobianWithRespectToParameters(
        const InputPointType  &in, JacobianType &jacobian) const;

    virtual void SetStiffness(double stiffness)
    {m_Stiffness=(stiffness>0)?stiffness:0.0;
        m_LMatrixComputed=false;
        m_LInverseComputed=false;
        m_WMatrixComputed=false;
    }
    //itkSetClampMacro(Stiffness, double, 0.0, NumericTraits<double>::max());
    // Cant use the macro because the matrices must be recomputed
    itkGetMacro(Stiffness, double);


protected:
    SparseKernelTransform();
    virtual ~SparseKernelTransform();
    void PrintSelf(std::ostream& os, Indent indent) const;

public:
    typedef Eigen::Triplet<TScalarType> TripletType;

    typedef Eigen::Matrix<TScalarType, NDimensions, NDimensions> GMatrixType;
    //typedef vnl_matrix_fixed<TScalarType, NDimensions, NDimensions> GMatrixType;

    typedef Eigen::SparseMatrix<TScalarType> LMatrixType;
    //typedef vnl_matrix<TScalarType> LMatrixType;

    typedef Eigen::SparseMatrix<TScalarType> KMatrixType;
    //typedef vnl_matrix<TScalarType> KMatrixType;

    typedef Eigen::SparseMatrix<TScalarType> PMatrixType;
    //typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, NDimensions*(NDimensions+1)> PMatrixType;
    //typedef vnl_matrix<TScalarType> PMatrixType;

    typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, Eigen::Dynamic> YMatrixType;
    //typedef vnl_matrix<TScalarType> YMatrixType;

    typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, Eigen::Dynamic> WMatrixType;
    //typedef vnl_matrix<TScalarType> WMatrixType;

    typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, Eigen::Dynamic> DMatrixType;
    //typedef vnl_matrix<TScalarType> DMatrixType;

    typedef Eigen::Matrix<TScalarType, NDimensions, NDimensions> AMatrixType;
    //typedef vnl_matrix_fixed<TScalarType,NDimensions,NDimensions> AMatrixType;

    typedef Eigen::Matrix<TScalarType,NDimensions,1> BMatrixType; // column vector
    //typedef vnl_vector_fixed<TScalarType,NDimensions> BMatrixType;

    typedef Eigen::Matrix<TScalarType,1,NDimensions> RowMatrixType;
    //typedef vnl_matrix_fixed<TScalarType, 1, NDimensions> RowMatrixType;

    typedef Eigen::Matrix<TScalarType,NDimensions,1> ColumnMatrixType;
    //typedef vnl_matrix_fixed<TScalarType, NDimensions, 1> ColumnMatrixType;

    PointSetPointer m_SourceLandmarks;

    PointSetPointer m_TargetLandmarks;

protected:
    virtual const GMatrixType & ComputeG(const InputVectorType & landmarkVector) const;

    virtual const GMatrixType & ComputeReflexiveG(PointsIterator) const;


    virtual void ComputeDeformationContribution( const InputPointType & inputPoint,
                                                 OutputPointType & result ) const;

    void ComputeK() const;

    void ComputeL() const;


    void ComputeP() const;

    void ComputeY() const;

    void ComputeD() const;

    void ReorganizeW(void) const;

    double m_Stiffness;

    VectorSetPointer m_Displacements;

    mutable LMatrixType m_LMatrix;

    mutable LMatrixType m_LMatrixInverse;

    mutable KMatrixType m_KMatrix;

    mutable PMatrixType m_PMatrix;

    mutable YMatrixType m_YMatrix;

    mutable WMatrixType m_WMatrix;

    mutable DMatrixType m_DMatrix;

    mutable AMatrixType m_AMatrix;

    mutable BMatrixType m_BVector;

    mutable GMatrixType m_GMatrix;

    mutable bool m_WMatrixComputed;
    mutable bool m_LMatrixComputed;
    mutable bool m_LInverseComputed;

    IMatrixType m_I;

private:
    SparseKernelTransform(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#include "itkSparseKernelTransform.cpp"

#endif // __itkSparseKernelTransform_h
```


-------------------------------

Updated on 2022-01-14 at 02:28:14 +0000

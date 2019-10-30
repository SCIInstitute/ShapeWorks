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
#include <itkeigen/Eigen/Dense>
#include <itkeigen/Eigen/Sparse>
#include <stdint.h>
#include <iostream>


namespace itk
{

/** \class SparseKernelTransform
 * Intended to be a base class for elastic body spline and thin plate spline.
 * This is implemented in as straightforward a manner as possible from the
 * IEEE TMI paper by Davis, Khotanzad, Flamig, and Harms, Vol. 16,
 * No. 3 June 1997. Notation closely follows their paper, so if you have it
 * in front of you, this code will make a lot more sense.
 *
 * SparseKernelTransform:
 *  Provides support for defining source and target landmarks
 *  Defines a number of data types used in the computations
 *  Defines the mathematical framework used to compute all splines,
 *    so that subclasses need only provide a kernel specific to
 *    that spline
 *
 * This formulation allows the stiffness of the spline to
 * be adjusted, allowing the spline to vary from interpolating the
 * landmarks to approximating the landmarks.  This part of the
 * formulation is based on the short paper by R. Sprengel, K. Rohr,
 * H. Stiehl. "Thin-Plate Spline Approximation for Image
 * Registration". In 18th International Conference of the IEEE
 * Engineering in Medicine and Biology Society. 1996.
 *
 * This class was modified to support its use in the ITK registration framework
 * by Rupert Brooks, McGill Centre for Intelligent Machines, Montreal, Canada
 * March 2007.  See the Insight Journal Paper  by Brooks, R., Arbel, T.
 * "Improvements to the itk::KernelTransform and its subclasses."
 *
 * \ingroup Transforms
 *
 */
template <class TScalarType, // probably only float and double make sense here
          unsigned int NDimensions>   // Number of dimensions
class ITK_EXPORT SparseKernelTransform :
        public Transform<TScalarType, NDimensions,NDimensions>
{
public:
    /** Standard class typedefs. */
    typedef SparseKernelTransform Self;
    typedef Transform<TScalarType, NDimensions, NDimensions >   Superclass;
    typedef SmartPointer<Self>        Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Run-time type information (and related methods). */
    itkTypeMacro( SparseKernelTransform, Transform );

    /** New macro for creation of through a Smart Pointer */
    itkNewMacro( Self );

    /** Dimension of the domain space. */
    itkStaticConstMacro(SpaceDimension, unsigned int, NDimensions);

    /** Scalar type. */
    typedef typename Superclass::ScalarType  ScalarType;

    /** Parameters type. */
    typedef typename Superclass::ParametersType  ParametersType;

    /** Jacobian type. */
    typedef typename Superclass::JacobianType  JacobianType;

    /** Standard coordinate point type for this class. */
    typedef typename Superclass::InputPointType   InputPointType;
    typedef typename Superclass::OutputPointType  OutputPointType;

    /** Standard vector type for this class. */
    typedef typename Superclass::InputVectorType   InputVectorType;
    typedef typename Superclass::OutputVectorType  OutputVectorType;

    /** PointList typedef. This type is used for maintaining lists of points,
   * specifically, the source and target landmark lists. */
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
    
    /** VectorSet typedef. */
    typedef itk::VectorContainer<unsigned long,InputVectorType> VectorSetType;
    typedef typename VectorSetType::Pointer        VectorSetPointer;

    /** Get the source landmarks list, which we will denote \f$ p \f$. */
    itkGetObjectMacro( SourceLandmarks, PointSetType);

    /** Set the source landmarks list. */
    virtual void SetSourceLandmarks(PointSetType *);

    /** Get the target landmarks list, which we will denote  \f$ q \f$. */
    itkGetObjectMacro( TargetLandmarks, PointSetType);

    /** Set the target landmarks list. */
    virtual void SetTargetLandmarks(PointSetType *);

    /** Get the displacements list, which we will denote \f$ d \f$,
   * where \f$ d_i = q_i - p_i \f$. */
    itkGetObjectMacro( Displacements, VectorSetType );

    /** Compute W matrix. */
    void ComputeWMatrix(void) const;

    /** Compute L matrix inverse. */
    //void ComputeLInverse() const;

    /** Compute the position of point in the new space */
    virtual OutputPointType TransformPoint(const InputPointType& thisPoint) const;

    /** 'I' (identity) matrix typedef. */
    typedef Eigen::Matrix<TScalarType, NDimensions, NDimensions> IMatrixType;
    //typedef vnl_matrix_fixed<TScalarType, NDimensions, NDimensions> IMatrixType;


    /** Compute the Jacobian Matrix of the transformation at one point */
    //virtual const JacobianType & GetJacobian(const InputPointType  &point ) const;

    /** Set the Transformation Parameters to be an identity transform */
    virtual void SetIdentity();

    /** Set the Transformation Parameters and update the internal transformation.
   * The parameters represent the source landmarks. Each landmark point is represented
   * by NDimensions doubles. All the landmarks are concatenated to form one flat
   * Array<double>. */
    virtual void SetParameters(const ParametersType &);

    /** Set Transform Fixed Parameters:
   *     To support the transform file writer this function was
   *     added to set the target landmarks similar to the
   *     SetParameters function setting the source landmarks
   */
    virtual void SetFixedParameters(const ParametersType &);

    /** Update the Parameters array from the landmarks corrdinates. */
    virtual void UpdateParameters(void) const;

    /** Get the Transformation Parameters - Gets the Source Landmarks */
    virtual const ParametersType& GetParameters(void) const;

    /** Get Transform Fixed Parameters - Gets the Target Landmarks */
    virtual const ParametersType& GetFixedParameters(void) const;
    virtual void ComputeJacobianWithRespectToParameters(
        const InputPointType  &in, JacobianType &jacobian) const;

    /** Stiffness of the spline.  A stiffness of zero results in the
   * standard interpolating spline.  A non-zero stiffness allows the
   * spline to approximate rather than interpolate the landmarks.
   * Stiffness values are usually rather small, typically in the range
   * of 0.001 to 0.1. The approximating spline formulation is based on
   * the short paper by R. Sprengel, K. Rohr, H. Stiehl. "Thin-Plate
   * Spline Approximation for Image Registration". In 18th
   * International Conference of the IEEE Engineering in Medicine and
   * Biology Society. 1996.
   */
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
    /** triplets used to fill sparse matrices. */
    typedef Eigen::Triplet<TScalarType> TripletType;

    /** 'G' matrix typedef. */
    typedef Eigen::Matrix<TScalarType, NDimensions, NDimensions> GMatrixType;
    //typedef vnl_matrix_fixed<TScalarType, NDimensions, NDimensions> GMatrixType;

    /** 'L' matrix typedef. */
    typedef Eigen::SparseMatrix<TScalarType> LMatrixType;
    //typedef vnl_matrix<TScalarType> LMatrixType;

    /** 'K' matrix typedef. */
    typedef Eigen::SparseMatrix<TScalarType> KMatrixType;
    //typedef vnl_matrix<TScalarType> KMatrixType;

    /** 'P' matrix typedef. */
    typedef Eigen::SparseMatrix<TScalarType> PMatrixType;
    //typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, NDimensions*(NDimensions+1)> PMatrixType;
    //typedef vnl_matrix<TScalarType> PMatrixType;

    /** 'Y' matrix typedef. */
    typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, Eigen::Dynamic> YMatrixType;
    //typedef vnl_matrix<TScalarType> YMatrixType;

    /** 'W' matrix typedef. */
    typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, Eigen::Dynamic> WMatrixType;
    //typedef vnl_matrix<TScalarType> WMatrixType;

    /** 'D' matrix typedef. Deformation component */
    typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, Eigen::Dynamic> DMatrixType;
    //typedef vnl_matrix<TScalarType> DMatrixType;

    /** 'A' matrix typedef. Rotational part of the Affine component */
    typedef Eigen::Matrix<TScalarType, NDimensions, NDimensions> AMatrixType;
    //typedef vnl_matrix_fixed<TScalarType,NDimensions,NDimensions> AMatrixType;

    /** 'B' matrix typedef. Translational part of the Affine component */
    typedef Eigen::Matrix<TScalarType,NDimensions,1> BMatrixType; // column vector
    //typedef vnl_vector_fixed<TScalarType,NDimensions> BMatrixType;

    /** Row matrix typedef. */
    typedef Eigen::Matrix<TScalarType,1,NDimensions> RowMatrixType;
    //typedef vnl_matrix_fixed<TScalarType, 1, NDimensions> RowMatrixType;

    /** Column matrix typedef. */
    typedef Eigen::Matrix<TScalarType,NDimensions,1> ColumnMatrixType;
    //typedef vnl_matrix_fixed<TScalarType, NDimensions, 1> ColumnMatrixType;

    /** The list of source landmarks, denoted 'p'. */
    PointSetPointer m_SourceLandmarks;

    /** The list of target landmarks, denoted 'q'. */
    PointSetPointer m_TargetLandmarks;

protected:
    /** Compute G(x)
   * This is essentially the kernel of the transform.
   * By overriding this method, we can obtain (among others):
   *    Elastic body spline
   *    Thin plate spline
   *    Volume spline */
    virtual const GMatrixType & ComputeG(const InputVectorType & landmarkVector) const;

    /** Compute a G(x) for a point to itself (i.e. for the block
   * diagonal elements of the matrix K. Parameter indicates for which
   * landmark the reflexive G is to be computed. The default
   * implementation for the reflexive contribution is a diagonal
   * matrix where the diagonal elements are the stiffness of the
   * spline. */
    virtual const GMatrixType & ComputeReflexiveG(PointsIterator) const;


    /** Compute the contribution of the landmarks weighted by the kernel funcion
      to the global deformation of the space  */
    virtual void ComputeDeformationContribution( const InputPointType & inputPoint,
                                                 OutputPointType & result ) const;

    /** Compute K matrix. */
    void ComputeK() const;

    /** Compute L matrix. */
    void ComputeL() const;


    /** Compute P matrix. */
    void ComputeP() const;

    /** Compute Y matrix. */
    void ComputeY() const;

    /** Compute displacements \f$ q_i - p_i \f$. */
    void ComputeD() const;

    /** Reorganize the components of W into
    D (deformable), A (rotation part of affine)
    and B (translational part of affine ) components.
    \warning This method release the memory of the W Matrix  */
    void ReorganizeW(void) const;

    /** Stiffness parameter */
    double m_Stiffness;

    /** The list of displacements.
   * d[i] = q[i] - p[i]; */
    VectorSetPointer m_Displacements;

    /** The L matrix. */
    mutable LMatrixType m_LMatrix;

    /** The inverse of L, which we also cache. */
    mutable LMatrixType m_LMatrixInverse;

    /** The K matrix. */
    mutable KMatrixType m_KMatrix;

    /** The P matrix. */
    mutable PMatrixType m_PMatrix;

    /** The Y matrix. */
    mutable YMatrixType m_YMatrix;

    /** The W matrix. */
    mutable WMatrixType m_WMatrix;

    /** The Deformation matrix.
      This is an auxiliary matrix that will hold the
      Deformation (non-affine) part of the transform.
      Those are the coefficients that will multiply the
      Kernel function */
    mutable DMatrixType m_DMatrix;

    /** Rotatinoal/Shearing part of the Affine component of the Transformation */
    mutable AMatrixType m_AMatrix;

    /** Translational part of the Affine component of the Transformation */
    mutable BMatrixType m_BVector;

    /** The G matrix.
    It is made mutable because m_GMatrix was made an ivar
    only to avoid copying the matrix at return time */
    mutable GMatrixType m_GMatrix;

    /** Has the W matrix been computed? */
    mutable bool m_WMatrixComputed;
    /** Has the L matrix been computed? */
    mutable bool m_LMatrixComputed;
    /** Has the L inverse matrix been computed? */
    mutable bool m_LInverseComputed;

    /** Identity matrix. */
    IMatrixType m_I;

private:
    SparseKernelTransform(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#include "itkSparseKernelTransform.cpp"

#endif // __itkSparseKernelTransform_h

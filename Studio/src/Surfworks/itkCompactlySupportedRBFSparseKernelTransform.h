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
/** \class CompactlySupportedRBFSparseKernelTransform
 * \ingroup Transforms
 */
template <class TScalarType,         // Data type for scalars (float or double)
          unsigned int NDimensions = 3>          // Number of dimensions
class ITK_EXPORT CompactlySupportedRBFSparseKernelTransform :
        public SparseKernelTransform<   TScalarType, NDimensions>
{
public:
    /** Standard class typedefs. */
    typedef CompactlySupportedRBFSparseKernelTransform Self;
    typedef SparseKernelTransform<    TScalarType, NDimensions>   Superclass;
    typedef SmartPointer<Self>        Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** New macro for creation of through a Smart Pointer */
    itkNewMacro( Self );

    /** Run-time type information (and related methods). */
    itkTypeMacro( CompactlySupportedRBFSparseKernelTransform, SparseKernelTransform );

    /** Scalar type. */
    typedef typename Superclass::ScalarType  ScalarType;

    /** Parameters type. */
    typedef typename Superclass::ParametersType  ParametersType;

    /** Jacobian Type */
    typedef typename Superclass::JacobianType  JacobianType;

    /** Dimension of the domain space. */
    itkStaticConstMacro(SpaceDimension, unsigned int,Superclass::SpaceDimension);

    /** These (rather redundant) typedefs are needed because on SGI, typedefs
   * are not inherited */
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

    /** These (rather redundant) typedefs are needed because on SGI, typedefs
   * are not inherited. */
    typedef typename Superclass::GMatrixType GMatrixType;

    const GMatrixType & ComputeG(const InputVectorType & x) const;

    /** Compute the contribution of the landmarks weighted by the kernel funcion
      to the global deformation of the space  */
    virtual void ComputeDeformationContribution( const InputPointType & inputPoint,
                                                 OutputPointType & result ) const;

private:
    CompactlySupportedRBFSparseKernelTransform(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    // basis support
    double Sigma;

};


} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCompactlySupportedRBFSparseKernelTransform.hxx"
#endif

#endif // __itkCompactlySupportedRBFSparseKernelTransform_h

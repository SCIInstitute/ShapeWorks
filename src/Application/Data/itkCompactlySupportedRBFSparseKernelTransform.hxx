/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCompactlySupportedRBFSparseKernelTransform.txx,v $
  Language:  C++
  Date:      $Date: 2014-1-28 14:22:18 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkCompactlySupportedRBFSparseKernelTransform_txx
#define _itkCompactlySupportedRBFSparseKernelTransform_txx
#include "itkCompactlySupportedRBFSparseKernelTransform.h"

namespace itk
{
  template<class TScalarType, unsigned int NDimensions>
  void CompactlySupportedRBFSparseKernelTransform<
    TScalarType, NDimensions>::ComputeJacobianWithRespectToParameters(
      const InputPointType & in, JacobianType & jacobian) const {  }

  template <class TScalarType, unsigned int NDimensions>
const typename CompactlySupportedRBFSparseKernelTransform<TScalarType, NDimensions>::GMatrixType &
CompactlySupportedRBFSparseKernelTransform<TScalarType, NDimensions>::
ComputeG(const InputVectorType & x) const
{
    double a = 3.0 * sqrt(3.14/2.0) * this->Sigma;

    const TScalarType r = (x.GetNorm())/a; // the support of the basis is only defined till 2.5*sigma
    this->m_GMatrix = GMatrixType::Zero();

    if (r <= 1)
    {
        //this->m_GMatrix.fill( NumericTraits< TScalarType >::Zero );

        TScalarType val = pow(1-r, 4.0) * (4.0*r + 1);
        for(unsigned int i=0; i<NDimensions; i++)
        {
            this->m_GMatrix(i,i) = val;

            //this->m_GMatrix[i][i] = val;
        }
    }
    return this->m_GMatrix;
}


template <class TScalarType, unsigned int NDimensions>
void
CompactlySupportedRBFSparseKernelTransform<TScalarType, NDimensions>::
ComputeDeformationContribution( const InputPointType  & thisPoint,
                                OutputPointType & result     ) const
{

    double a = 3.0 * sqrt(3.14/2.0) * this->Sigma;

    unsigned long numberOfLandmarks = this->m_SourceLandmarks->GetNumberOfPoints();

    PointsIterator sp  = this->m_SourceLandmarks->GetPoints()->Begin();

    for(unsigned int lnd=0; lnd < numberOfLandmarks; lnd++ )
    {
        InputVectorType position = thisPoint - sp->Value();
        const TScalarType r = (position.GetNorm())/a; // the support of the basis is only defined till 2.5*sigma

        TScalarType val = 0.0;
        if(r <= 1)
            val = pow(1-r, 4.0) * (4.0*r + 1);

        for(unsigned int odim=0; odim < NDimensions; odim++ )
        {
            result[ odim ] += val * this->m_DMatrix(odim,lnd);
        }
        ++sp;
    }

}


} // namespace itk
#endif

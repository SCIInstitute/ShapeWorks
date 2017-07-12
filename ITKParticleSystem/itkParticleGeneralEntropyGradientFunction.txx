/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleGeneralEntropyGradientFunction.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleGeneralEntropyGradientFunction_txx
#define __itkParticleGeneralEntropyGradientFunction_txx

#include "itkParticleImageDomainWithGradients.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkParticleGaussianModeWriter.h"
#include <string>

namespace itk
{
template <unsigned int VDimension>
void
ParticleGeneralEntropyGradientFunction<VDimension>
::ComputeUpdates()
{  
    // NOTE: This code requires that indices be contiguous, i.e. it won't work if
    // you start deleting particles.
    const unsigned int num_samples = m_ShapeData->GetNumberOfSamples();
    const unsigned int num_dims    = m_ShapeData->GetNumberOfDimensions();
    const unsigned int num_functions = m_ShapeData->GetNumberOfFunctions();
    const unsigned int num_particles = num_dims / num_functions;

    // Compute mean positions in shape space.
    // Do we need to resize the update matrix?
    if (m_PointsUpdate.rows() != VDimension * num_particles || m_PointsUpdate.cols() != num_samples)
        m_PointsUpdate.set_size(VDimension * num_particles, num_samples);

    vnl_matrix_type points_minus_mean(num_dims, num_samples);
    vnl_vector_type means(num_dims);

    // Compute the mean shape vector. Y
    for (unsigned int j = 0; j < num_dims; j++)
    {
        double total = 0.0;
        for (unsigned int i = 0; i < num_samples; i++)
        {
            points_minus_mean(j, i) = m_ShapeData->GetScalar(i, j) * m_AttributeScales[j % num_functions];
            total += points_minus_mean(j,i);
        }
        means(j) = total/(double)num_samples;
    }

    for (unsigned int j = 0; j < num_dims; j++)
    {
        for (unsigned int i = 0; i < num_samples; i++)
            points_minus_mean(j, i) -= means(j);
    }

    // Compute the covariance in the dual space (transposed shape matrix)
    vnl_matrix_type A =  points_minus_mean.transpose() * points_minus_mean * (1.0/((double)(num_samples-1)));

    // Regularize A
    for (unsigned int i = 0; i < num_samples; i++)
        A(i, i) = A(i, i) + m_MinimumVariance;

    // Find inverse of covariance matrix
    vnl_symmetric_eigensystem<float> symEigen(A);
    vnl_matrix_type Q = points_minus_mean * (symEigen.pinverse());

    // Compute the update matrix in coordinate space by multiplication with the
    // Jacobian.  Each shape gradient must be transformed by a different Jacobian
    // so we have to do this individually for each shape (sample).
    vnl_matrix_type J(num_functions, VDimension);
    vnl_vector_type v(num_functions);

    for (unsigned int i = 0; i < num_samples; i++) // go through all shapes
    {
        unsigned int k = 0;
        for (unsigned int j = 0; j < num_particles; j++)  // go through particles
        {
            // Construct the Jacobian submatrix
            for (unsigned int ii = 0; ii < num_functions; ii++, k++)
            {
                typename ShapeDataType::VectorType grad = m_ShapeData->GetGradient(i, k);
                for (unsigned int jj = 0; jj < VDimension; jj++)
                    J(ii,jj) = grad[jj] * m_AttributeScales[ii];
                v[ii] = Q(k,i);
            }

            vnl_vector_type dx = J.transpose() * v;
            // Fill in appropriate rows/col of update matrix
            for (unsigned int kk = 0; kk < VDimension; kk++)
                m_PointsUpdate[j*VDimension+kk][i] = dx[kk];
        }// done particle
    }
    std::cout << m_PointsUpdate.extract(6, num_samples,0,0) << std::endl;
    m_MinimumEigenValue = symEigen.D(0, 0);
    m_CurrentEnergy = 0.0;
    // double energy = 0.0;
    for (unsigned int i = 1; i < num_samples; i++)
    {
        if (symEigen.D(i, i) < m_MinimumEigenValue)
            m_MinimumEigenValue = symEigen.D(i, i);
        m_CurrentEnergy += log(symEigen.D(i,i));
    }
    m_CurrentEnergy /= 2;

    for (unsigned int i =0; i < num_samples; i++)
        std::cout << i << ": "<< symEigen.D(i, i) - m_MinimumVariance << std::endl;

    std::cout << "FeaVol_ENERGY = " << m_CurrentEnergy << "\t MinimumVariance = "
              << m_MinimumVariance <<  std::endl;
}

template <unsigned int VDimension>
typename ParticleGeneralEntropyGradientFunction<VDimension>::VectorType
ParticleGeneralEntropyGradientFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // NOTE: This code requires that indices be contiguous, i.e. it won't work if
    // you start deleting particles.
    energy = m_CurrentEnergy;
    maxdt = m_MinimumEigenValue;

    VectorType gradE;
    unsigned int k = idx * VDimension;
    for (unsigned int i = 0; i< VDimension; i++)
    {
        // gradE[i] = m_PointsUpdate((idx * VDimension) + i, d);
        gradE[i] = m_PointsUpdate(k + i, d );
    }

//    if (idx == 0 ) std::cout << "maxdt= " << maxdt << " idx = " << idx << "\t" << "GradE = " << gradE << std::endl;
    return system->TransformVector(gradE, system->GetInverseTransform(d));
}



} // end namespace
#endif

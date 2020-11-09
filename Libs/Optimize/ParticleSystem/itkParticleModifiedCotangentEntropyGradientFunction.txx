/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleModifiedCotangentEntropyGradientFunction.txx,v $
  Date:      $Date: 2017/06/26 $
  Version:   $Revision: 4 $
  Author:    $Author: Praful $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleModifiedCotangentEntropyGradientFunction_txx
#define __itkParticleModifiedCotangentEntropyGradientFunction_txx
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix.h"

namespace itk {

template <class TGradientNumericType, unsigned int VDimension>
typename ParticleModifiedCotangentEntropyGradientFunction<TGradientNumericType, VDimension>::VectorType
ParticleModifiedCotangentEntropyGradientFunction<TGradientNumericType, VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxmove, double &energy) const
{
    VectorType gradE;
    for (unsigned int n = 0; n < VDimension; n++)
        gradE[n] = 0.0;

    const double epsilon = 1.0e-6;
    // Get the position for which we are computing the gradient.
    PointType pos = system->GetPosition(idx, d);


    VectorType r;
    double rmag;
    energy = epsilon;
    //m_GlobalSigma - 1 per domain
    // typename ParticleSystemType::PointVectorType m_CurrentNeighborhood = system->FindNeighborhoodPoints(pos, m_GlobalSigma[d], d);
   const double m_CurrentNeighborhood = 0.0;



    if (neighbourhoods[idx].size()==0)
    {
        energy = 0;
        return gradE;
    }

    for (unsigned int k = 0; k < neighbourhoods[idx].size(); k++)
    {
        PointType pos_k = neighbourhoods[idx][k].Point;
        for (unsigned int n = 0; n < VDimension; n++)
            r[n] = pos[n] - pos_k[n];
        rmag = r.magnitude();
        energy += this->ComputeModifiedCotangent(rmag, d);

        double forc = this->ComputeModifiedCotangentDerivative(rmag, d);

        for (unsigned int n = 0; n < VDimension; n++)
          gradE[n] += (forc * r[n])/rmag;
    }

    for (unsigned int n = 0; n < VDimension; n++)
        gradE[n] /= neighbourhoods[idx].size();

    maxmove = m_GlobalSigma[d]; // deprecated - not used in gradient descent class
    energy *= -1;

    return gradE ;
}

}// end namespace
#endif

/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMTwoCostFunction_hxx
#define __itkPSMTwoCostFunction_hxx
#include "itkPSMTwoCostFunction.h"

namespace itk 
{

template <unsigned int VDimension>
typename PSMTwoCostFunction<VDimension>::VectorType
PSMTwoCostFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d,
           const ParticleSystemType *system,
           double &maxmove) const
{
    double maxA, maxB;
    maxA = 0;
    maxB = 0;
    VectorType ansA;
    ansA.fill(0.0);
    VectorType ansB;
    ansB.fill(0.0);

    const_cast<PSMTwoCostFunction *>(this)->m_Counter = m_Counter + 1.0;

    // Evaluate A if it is turned on
    if (m_AOn)
    {
        ansA = m_FunctionA->Evaluate(idx, d, system, maxA);
        const_cast<PSMTwoCostFunction *>(this)->m_AverageGradMagA = m_AverageGradMagA + ansA.magnitude();
    }

    // Evaluate B if it is turned on
    if (m_BOn)
    {
        ansB = m_FunctionB->Evaluate(idx, d, system, maxB);
        const_cast<PSMTwoCostFunction *>(this)->m_AverageGradMagB = m_AverageGradMagB + ansB.magnitude();
    }

    // Compute maximum move and return the predicted move for current configuration
    if (m_BOn)
    {
        if (m_AOn)  // both A and B are active
        {
            if (maxA > maxB) maxmove = maxB;
            else maxmove = maxA;

            maxmove = maxA; // always driven by the sampling to decrease the senstivity to covariance regularization

            return (ansA + m_RelativeGradientScaling * ansB);
        }
        else // B is active, A is not active
        {
            maxmove = maxB;
            return ansB;
        }
    }
    else  // only A is active
    {
        maxmove = maxA;
        return ansA;
    }

    // If nothing is turned on, then return a max time
    // step of 0 and a bogus vector.
    maxmove = 0.0;
    return ansA;
}

template <unsigned int VDimension>
typename PSMTwoCostFunction<VDimension>::VectorType
PSMTwoCostFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d,
           const ParticleSystemType *system,
           double &maxmove, double &energy) const
{
    double maxA = 0.0;
    double maxB = 0.0;
    double energyA = 0.0;
    double energyB = 0.0;
    VectorType ansA; ansA.fill(0.0);
    VectorType ansB; ansB.fill(0.0);

    const_cast<PSMTwoCostFunction *>(this)->m_Counter = m_Counter + 1.0;

    // evaluate individual functions: A = surface energy, B = correspondence, C = normal entropy
    if (m_AOn)
    {
        ansA = m_FunctionA->Evaluate(idx, d, system, maxA, energyA);

        const_cast<PSMTwoCostFunction *>(this)->m_AverageGradMagA = m_AverageGradMagA + ansA.magnitude();
        const_cast<PSMTwoCostFunction *>(this)->m_AverageEnergyA = m_AverageEnergyA + energyA;
    }

    if (m_BOn)
    {
        ansB = m_FunctionB->Evaluate(idx, d, system, maxB, energyB);

        const_cast<PSMTwoCostFunction *>(this)->m_AverageGradMagB = m_AverageGradMagB + ansB.magnitude();
        const_cast<PSMTwoCostFunction *>(this)->m_AverageEnergyB = m_AverageEnergyB + energyB;
    }

    // Compute the final energy, maxmove and predicted move based on
    // current configuration of A and B
    if (m_BOn)
    {
        if (m_AOn)  // both A and B are active
        {
            if (maxA > maxB) maxmove = maxB;
            else maxmove = maxA;
            energy = energyA + m_RelativeEnergyScaling * energyB;

            maxmove = maxA; // always driven by the sampling to decrease the senstivity to covariance regularization

            return (ansA + m_RelativeGradientScaling * ansB);
        }
        else // only B is active, A is not active
        {
            maxmove = maxB;
            energy = energyB;
            return ansB;
        }
    }
    else  // only A is active
    {
        maxmove = maxA;
        energy = energyA;
        return ansA;
    }

    // If nothing is turned on, then return a max time
    // step of 0 and a bogus vector.
    maxmove = 0.0;
    return ansA;
}

template <unsigned int VDimension>
double
PSMTwoCostFunction<VDimension>
::Energy(unsigned int idx, unsigned int d, 
         const ParticleSystemType *system) const
{
    double ansA = 0.0;
    double ansB = 0.0;

    if (m_AOn ) ansA = m_FunctionA->Energy(idx, d, system);
    if (m_BOn ) ansB = m_FunctionB->Energy(idx, d, system);

    // Compute the final energy for the current configuration of A and
    // B
    if (m_BOn )
    {
        if (m_AOn )  // both A and B are active
        { return ansA + m_RelativeEnergyScaling * ansB;	}
        else // B is active, A is not active
        { return ansB;}
    }

    else  // only A is active
    { return ansA; }

    return 0.0;
}

} // end namespace itk

#endif

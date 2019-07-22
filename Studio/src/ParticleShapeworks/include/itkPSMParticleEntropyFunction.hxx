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
#ifndef __itkPSMParticleEntropyFunction_hxx
#define __itkPSMParticleEntropyFunction_hxx
#include "itkPSMParticleEntropyFunction.h"

namespace itk {

template <class TGradientNumericType, unsigned int VDimension>
TGradientNumericType
PSMParticleEntropyFunction<TGradientNumericType, VDimension>
::AngleCoefficient( const GradientVectorType& p_i_normal,  const GradientVectorType& p_j_normal) const
{
    // Get the cosine of the angle between the two particles' normals
    TGradientNumericType cosine = dot_product(p_i_normal,p_j_normal) /
            (p_i_normal.magnitude()*p_j_normal.magnitude() + 1.0e-6);

    // the flat region
    if ( cosine >= m_FlatCutoff ) return 1.0;

    // the feathered region
    return ( cos( (m_FlatCutoff - cosine) / (1.0+m_FlatCutoff) * (3.14159265358979/2.0) )) ;
} 

template <class TGradientNumericType, unsigned int VDimension>
void
PSMParticleEntropyFunction<TGradientNumericType, VDimension>
::ComputeAngularWeights(const PointType &pos,
                        const typename ParticleSystemType::PointVectorType &neighborhood,
                        const PSMImageDomainWithGradients<TGradientNumericType, VDimension> *domain,
                        std::vector<double> &weights) const
{
    GradientVectorType posnormal = domain->SampleNormalVnl(pos, 1.0e-10);
    weights.resize(neighborhood.size());

    for (unsigned int i = 0; i < neighborhood.size(); i++)
    {
        weights[i] = this->AngleCoefficient(posnormal,
                                            domain->SampleNormalVnl(neighborhood[i].Point, 1.0e-10));
        if (weights[i] < 1.0e-5) weights[i] = 0.0;
    }
}

template <class TGradientNumericType, unsigned int VDimension>
double
PSMParticleEntropyFunction<TGradientNumericType, VDimension>
::EstimateSigma(unsigned int, const typename ParticleSystemType::PointVectorType &neighborhood,
                const std::vector<double> &weights,
                const PointType &pos,  double initial_sigma,  double precision,
                int &err) const
{
    const double epsilon = 1.0e-5;
    const double min_sigma = 1.0e-4;

    const double M = static_cast<double>(VDimension);
    const double MM = M * M * 2.0 + M;

    double error = 1.0e6;
    double sigma, prev_sigma;
    sigma = initial_sigma;

    while (error > precision)
    {
        VectorType r_vec;
        double A = 0.0;
        double B = 0.0;
        double C = 0.0;
        double sigma2 = sigma * sigma;
        double sigma22 = sigma2 * 2.0;

        for (unsigned int i = 0; i < neighborhood.size(); i++)
        {
            if (weights[i] < epsilon) continue;

            //    if ( neighborhood[i].Index == idx) continue;
            for (unsigned int n = 0; n < VDimension; n++)
            {
                r_vec[n] = pos[n] - neighborhood[i].Point[n];
            }

            double r = r_vec.magnitude();
            double r2 = r*r;
            double alpha = exp(-r2 / sigma22) * weights[i];
            A += alpha;
            B += r2 * alpha;
            C += r2 * r2 * alpha;
        } // end for i

        prev_sigma = sigma;

        if (A < epsilon)
        {
            err = 1;
            return sigma;
        }; // results are not meaningful

        // First order convergence update.  This is a fixed point iteration.
        //sigma = sqrt(( 1.0 / DIM ) * ( B / A));

        // Second order convergence update (Newton-Raphson).  This is the first
        // derivative of the negative of the probability density estimation function squared over the
        // second derivative.

        // old math
        //    sigma -= (sigma2 * VDimension * A * A - A  * B) / (((2.0 * sigma * VDimension) * A * A -
        //                                          (1.0/(sigma2*sigma))*(A*C-B*B)) + epsilon);

        // New math -- results are not obviously different?
        sigma -= (A * (B - A * sigma2 * M)) /
                ( (-MM * A *A * sigma) - 3.0 * A * B * (1.0 / (sigma + epsilon))
                  - (A*C + B*B) * (1.0 / (sigma2 * sigma + epsilon)) + epsilon);

        error = 1.0 - fabs((sigma/prev_sigma));

        // Constrain sigma.
        if (sigma < min_sigma)
        {
            sigma = min_sigma;
            error = precision; // we are done if sigma has vanished
        }
        else
        {
            if (sigma < 0.0) sigma = min_sigma;
        }

    } // end while (error > precision)

    err = 0;
    return sigma;

} // end estimate sigma


template <class TGradientNumericType, unsigned int VDimension>
typename PSMParticleEntropyFunction<TGradientNumericType, VDimension>::VectorType
PSMParticleEntropyFunction<TGradientNumericType, VDimension>
::Evaluate(unsigned int idx,unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // Grab a pointer to the domain.  We need a Domain that has surface normal information.
    const PSMImageDomainWithGradients<TGradientNumericType, VDimension> *
            domain = static_cast<const PSMImageDomainWithGradients<
            TGradientNumericType, VDimension> *>(system->GetDomain(d));
    const double epsilon = 1.0e-6;

    // Retrieve the previous optimal sigma value for this point.  If the value is
    // tiny (i.e. unitialized) then use a fraction of the maximum allowed
    // neighborhood radius.
    double sigma = m_SpatialSigmaCache->operator[](d)->operator[](idx);
    if (sigma < epsilon)
    { sigma = m_MinimumNeighborhoodRadius / m_NeighborhoodToSigmaRatio;}

    // Determine the extent of the neighborhood that will be used in the Parzen
    // windowing estimation.  The neighborhood extent is based on the optimal
    // sigma calculation and limited to a user supplied maximum radius (probably
    // the size of the domain).
    double neighborhood_radius = sigma * m_NeighborhoodToSigmaRatio;
    if (neighborhood_radius > m_MaximumNeighborhoodRadius)
    { neighborhood_radius = m_MaximumNeighborhoodRadius; }

    // Get the position for which we are computing the gradient.
    PointType pos = system->GetPosition(idx, d);

    // Get the neighborhood surrounding the point "pos".
    typename ParticleSystemType::PointVectorType neighborhood
            = system->FindNeighborhoodPoints(pos, neighborhood_radius, d);

    // Compute the weights based on angle between the neighbors and the center.
    std::vector<double> weights;
    this->ComputeAngularWeights(pos,neighborhood,domain,weights);

    // if cutting planes are enabled, add a virtual particle on the plane that correspond
    // to the current particle to repel it away from the plane
    const PSMImplicitSurfaceDomain<TGradientNumericType, VDimension> *
            implicit_domain = static_cast<const PSMImplicitSurfaceDomain<
            TGradientNumericType, VDimension> *>(system->GetDomain(d));

    vnl_vector_fixed<double, VDimension> x;
    double planeDist = 1e10;
    itk::Point<double, VDimension> planePoint;

    if(implicit_domain->IsCuttingPlaneDefined()){

        for (unsigned int i = 0; i < VDimension; i++)  { x[i] = pos[i]; }
        double D = fabs(dot_product( implicit_domain->GetCuttingPlaneNormal(), x - implicit_domain->GetCuttingPlanePoint()));

        for ( unsigned int i = 0; i < VDimension; i++ )
        { planePoint[i] = x[i] - (implicit_domain->GetCuttingPlaneNormal()[i] * D); }

        neighborhood.push_back( itk::PSMPointIndexPair<VDimension>( planePoint, 0 ) );
        weights.push_back( 0.5 );

        planeDist = 0.0;
        for ( unsigned int i = 0; i < VDimension; i++ )
        { planeDist += (x[i] - planePoint[i])*(x[i] - planePoint[i]); }
        planeDist = sqrt(planeDist);
    }

    // Estimate the best sigma for Parzen windowing.  In some cases, such as when
    // the neighborhood does not include enough points, the value will be bogus.
    // In these cases, an error != 0 is returned, and we try the estimation again
    // with an increased neighborhood radius.
    int err;
    sigma =  this->EstimateSigma(idx, neighborhood,weights,pos, sigma, epsilon, err);

    while (err != 0)
    {
        neighborhood_radius *= 2.0;
        // Constrain the neighborhood size.  If we have reached a maximum
        // possible neighborhood size, we'll just go with that.
        if ( neighborhood_radius > this->GetMaximumNeighborhoodRadius())
        {
            sigma = this->GetMaximumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
            neighborhood_radius =  this->GetMaximumNeighborhoodRadius();
            break;
        }
        else
        {
            sigma = neighborhood_radius / this->GetNeighborhoodToSigmaRatio();
        }

        neighborhood = system->FindNeighborhoodPoints(pos, neighborhood_radius, d);
        this->ComputeAngularWeights(pos,neighborhood,domain,weights);

        // if cutting planes are enabled, add a virtual particle on the plane that correspond
        // to the current particle to repel it away from the plane
        if(implicit_domain->IsCuttingPlaneDefined()){

            for (unsigned int i = 0; i < VDimension; i++)  { x[i] = pos[i]; }
            double D = fabs(dot_product( implicit_domain->GetCuttingPlaneNormal(), x - implicit_domain->GetCuttingPlanePoint()));

            for ( unsigned int i = 0; i < VDimension; i++ )
            { planePoint[i] = x[i] - (implicit_domain->GetCuttingPlaneNormal()[i] * D); }

            neighborhood.push_back( itk::PSMPointIndexPair<VDimension>( planePoint, 0 ) );
            weights.push_back( 0.5 );

            planeDist = 0.0;
            for ( unsigned int i = 0; i < VDimension; i++ )
            { planeDist += (x[i] - planePoint[i])*(x[i] - planePoint[i]); }
            planeDist = sqrt(planeDist);
        }

        sigma = this->EstimateSigma(idx, neighborhood, weights, pos, sigma, epsilon, err);
    } // done while err

    // Constrain sigma to a maximum reasonable size based on the user-supplied
    // limit to neighborhood size.
    if (sigma > this->GetMaximumNeighborhoodRadius())
    {
        sigma = this->GetMaximumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
        neighborhood_radius = this->GetMaximumNeighborhoodRadius();
        neighborhood = system->FindNeighborhoodPoints(pos, neighborhood_radius, d);
        this->ComputeAngularWeights(pos,neighborhood,domain,weights);

        // if cutting planes are enabled, add a virtual particle on the plane that correspond
        // to the current particle to repel it away from the plane
        if(implicit_domain->IsCuttingPlaneDefined()){

            for (unsigned int i = 0; i < VDimension; i++)  { x[i] = pos[i]; }
            double D = fabs(dot_product( implicit_domain->GetCuttingPlaneNormal(), x - implicit_domain->GetCuttingPlanePoint()));

            for ( unsigned int i = 0; i < VDimension; i++ )
            { planePoint[i] = x[i] - (implicit_domain->GetCuttingPlaneNormal()[i] * D); }

            neighborhood.push_back( itk::PSMPointIndexPair<VDimension>( planePoint, 0 ) );
            weights.push_back( 0.5 );

            planeDist = 0.0;
            for ( unsigned int i = 0; i < VDimension; i++ )
            { planeDist += (x[i] - planePoint[i])*(x[i] - planePoint[i]); }
            planeDist = sqrt(planeDist);
        }
    }

    //  std::cout << idx <<  "\t SIGMA = " << sigma << "\t NEIGHBORHOOD SIZE = " << neighborhood.size()
    //            << "\t NEIGHBORHOOD RADIUS= " << neighborhood_radius << std::endl;

    // We are done with the sigma estimation step.  Cache the sigma value for
    // next time.
    m_SpatialSigmaCache->operator[](d)->operator[](idx) = sigma;

    //----------------------------------------------

    // Compute the gradients.
    double sigma2inv = 1.0 / (2.0* sigma * sigma + epsilon);

    VectorType r;
    VectorType gradE;

    for (unsigned int n = 0; n < VDimension; n++)
    {
        gradE[n] = 0.0;
    }

    double A = 0.0;
    double maxNeighborDistance = 0.0; // limit maxmove to the distance to the farthest neighbor
    for (unsigned int i = 0; i < neighborhood.size(); i++)
    {
        //    if ( neighborhood[i].Index == idx) continue;
        if (weights[i] < epsilon) continue;

        for (unsigned int n = 0; n < VDimension; n++)
        {
            // Note that the Neighborhood object has already filtered the
            // neighborhood for points whose normals differ by > 90 degrees.
            r[n] = pos[n] - neighborhood[i].Point[n];
        }

        double curDist = r.magnitude();
        if(curDist > maxNeighborDistance)
            maxNeighborDistance = curDist;

        double q = exp( -dot_product(r, r) * sigma2inv);
        A += q;

        for (unsigned int n = 0; n < VDimension; n++)
        {
            gradE[n] += weights[i] * r[n] * q;
        }
    }

    double p = 0.0;
    if (A > epsilon)
    {
        p = -1.0 / (A * sigma * sigma);
    }

    // TEST
    //   vnl_vector_fixed<float, VDimension> tosurf = domain->SampleGradientVnl(pos);
    //   float tosurfmag = tosurf.magnitude() + 1.0e-5;

    // end test
    //   float f = domain->Sample(pos);

    for (unsigned int n = 0; n <VDimension; n++)
    {
        gradE[n] *= p;
        // TEST
        //     gradE[n] += f * (tosurf[n] / tosurfmag);
        // end test
    }
    //   maxdt = sigma * sigma
    //maxdt = 0.5;

    // limit maxmove to the distance to the farthest neighbor
    //maxdt = maxNeighborDistance;
    maxdt = sigma * 0.1;
    if(maxdt > planeDist)
        maxdt = 0.9 * planeDist;

    energy = (A * sigma2inv );

    return gradE;
}


}// end namespace
#endif

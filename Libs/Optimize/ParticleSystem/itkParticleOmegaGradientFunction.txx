/*=========================================================================
   Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
   Module:    $RCSfile: itkParticleOmegaGradientFunction.txx,v $
   Date:      $Date: 2011/03/24 01:17:33 $
   Version:   $Revision: 1.2 $
   Author:    $Author: wmartin $

   Copyright (c) 2009 Scientific Computing and Imaging Institute.
   See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
   =========================================================================*/
#ifndef __itkParticleOmegaGradientFunction_txx
#define __itkParticleOmegaGradientFunction_txx
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix.h"

namespace itk {

template <class TGradientNumericType, unsigned int VDimension>
double
ParticleOmegaGradientFunction<TGradientNumericType, VDimension>
::EstimateSigma( unsigned int idx,
                 unsigned int dom,
                 const typename ParticleSystemType::PointVectorType &neighborhood,
                 const std::vector<double> &weights,
                 const PointType &pos,
                 double initial_sigma,
                 double precision,
                 int &err,
                 double &avgKappa,
                 unsigned int numspheres, unsigned int numPlanes ) const
{
    //  avgKappa =
    //
    //  this->ComputeKappa(m_MeanCurvatureCache->operator[](this->GetDomainNumber())->operator[](idx), dom);
    avgKappa = 0.0;
    const double min_sigma = 1.0e-4;
    const double epsilon = 1.0e-5;

    const double M = static_cast<double>( VDimension );
    const double MM = M * M * 2.0 + M;

    double error = 1.0e6;
    double sigma, prev_sigma;
    sigma = initial_sigma;

    // Distance to plane is distance to last neighbor in the list
    double planeDist = 0.0;
    // AKM : Cutting Plane Disabled
    /*
    for (unsigned int i = 0; i < VDimension; i++)
    {
        planeDist += (pos[i] - neighborhood[neighborhood.size()-(numspheres+1)].Point[i]) *
                (pos[i] - neighborhood[neighborhood.size()-(numspheres+1)].Point[i]);
    }
    */

    while ( error > precision )
    {
        VectorType r_vec;
        double A = 0.0;
        double B = 0.0;
        double C = 0.0;
        double sigma2 = sigma * sigma;
        double sigma22 = sigma2 * 2.0;

        double mymc = m_MeanCurvatureCache->operator[] ( this->GetDomainNumber() )->operator[] ( idx );

        for ( unsigned int i = 0; i < neighborhood.size(); i++ )
        {
            if (weights[i] < epsilon) continue;
            double mc;
            // AKM : Cutting Plane Disabled
            if ( i >= ( neighborhood.size() - ( numspheres + numPlanes ) ) ) // special cases
            {                                     // has no valid particle index
                mc = mymc;
            }
            else
            {
                mc = m_MeanCurvatureCache->operator[] ( this->GetDomainNumber() )->operator[] ( neighborhood[i].Index );
            }

            // Curvature half-way between me and neighbor
            double Dij = ( mymc + mc ) * 0.5;
            double kappa = this->ComputeKappa(Dij, dom,sqrt(planeDist)); // Praful -- planedist not being used in the code

            avgKappa += kappa;

            for ( unsigned int n = 0; n < VDimension; n++ )
            {
                // Note that the Neighborhood object has already filtered the
                // neighborhood for points whose normals differ by > 90 degrees.
                r_vec[n] = ( pos[n] - neighborhood[i].Point[n] ) * kappa;
            }

            double r = r_vec.magnitude();
            double r2 = r * r;
            double alpha = exp( -r2 / sigma22 ) * weights[i];
            A += alpha;
            B += r2 * alpha;
            C += r2 * r2 * alpha;
        }   // end for i

        avgKappa /= static_cast<double>( neighborhood.size() );

        prev_sigma = sigma;

        if ( A < epsilon )
        {
            err = 1;
            avgKappa = 1.0;
            return sigma;
        }; // results are not meaningful

        // // First order convergence update.
        // sigma = sqrt(( 1.0 / DIM ) * ( B / A));

        // old math
        //  sigma -= (sigma2 * VDimension * A * A - A  * B) / (((2.0 * sigma * VDimension) * A * A -
        //                                          (1.0/(sigma2*sigma))*(A*C-B*B)) + epsilon);

        // New math -- results are not obviously different?
        sigma -= ( A * ( B - A * sigma2 * M ) ) /
                ( ( -MM * A * A * sigma ) - 3.0 * A * B * ( 1.0 / ( sigma + epsilon ) )
                  - ( A * C + B * B ) * ( 1.0 / ( sigma2 * sigma + epsilon ) ) + epsilon );

        error = 1.0 - fabs( ( sigma / prev_sigma ) );

        // Constrain sigma.
        if ( sigma < min_sigma )
        {
            sigma = min_sigma;
            error = precision; // we are done if sigma has vanished
        }
        else
        {
            if (sigma < 0.0) sigma = min_sigma;
        }

    }   // end while (error > precision)

    err = 0;
    return sigma;
}

template <class TGradientNumericType, unsigned int VDimension>
void
ParticleOmegaGradientFunction<TGradientNumericType, VDimension>
::BeforeEvaluate( unsigned int idx, unsigned int d, const ParticleSystemType* system )
{
    // SHIREEN
    m_MaxMoveFactor = 0.1;
    // END SHIREEN

    // Compute the neighborhood size and the optimal sigma.
    const double epsilon = 1.0e-6;

    // Grab a pointer to the domain.  We need a Domain that has surface normal
    // information and a cutting plane
    const ParticleImplicitSurfaceDomain<TGradientNumericType>* domain
            = static_cast<const ParticleImplicitSurfaceDomain<TGradientNumericType>*>( system->GetDomain( d ) );

    // Get the position for which we are computing the gradient.
    PointType pos = system->GetPosition( idx, d );

    // Retrieve the previous optimal sigma value for this point.  If the value is
    // tiny (i.e. unitialized) then use a fraction of the maximum allowed
    // neighborhood radius.
    m_CurrentSigma = this->GetSpatialSigmaCache()->operator[] ( d )->operator[] ( idx );

    // AKM : Cutting Plane Disabled
    /**/
    vnl_vector_fixed<double, VDimension> x;

    for (unsigned int i = 0; i < VDimension; i++)  { x[i] = pos[i]; }

    planePts.clear();
    spherePts.clear();
    CToP.clear();
    int numSpheres = 0;
    int numPlanes = 0;

    if (domain->IsCuttingPlaneDefined())
    {
        numPlanes = domain->GetNumberOfPlanes();
        for (unsigned int pidx = 0; pidx < numPlanes; pidx++)
        {
            double D = dot_product( domain->GetCuttingPlaneNormal(pidx), x - domain->GetCuttingPlanePoint(pidx) );
            itk::Point<double, VDimension> planept;
            for ( unsigned int i = 0; i < VDimension; i++ )
                planept[i] = x[i] - (domain->GetCuttingPlaneNormal(pidx)[i] * D);
            planePts.push_back(planept);
        }
    }

    if (domain->IsCuttingSphereDefined())
    {
        numSpheres = domain->GetNumberOfSpheres();
        for (unsigned int sidx = 0; sidx < numSpheres; sidx++)
        {
            itk::Point<double, VDimension> spherept;
            vnl_vector_fixed<double, VDimension> q;
            for ( unsigned int j = 0; j < VDimension; j++ )
                q[j] = pos[j] - domain->GetSphereCenter( sidx )[j];

            q.normalize();
            for ( unsigned int j = 0; j < VDimension; j++ )
                spherept[j] = domain->GetSphereCenter( sidx )[j] + ( q[j] * abs(domain->GetSphereRadius( sidx )) );
            spherePts.push_back(spherept);
            CToP.push_back(sqrt(dot_product(x-domain->GetSphereCenter(sidx), x-domain->GetSphereCenter(sidx))));
        }
    }

    double myKappa = this->ComputeKappa( m_MeanCurvatureCache->operator[] ( this->GetDomainNumber() )->operator[] ( idx ), d, 0 );

    if ( m_CurrentSigma < epsilon )
        m_CurrentSigma = this->GetMinimumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();

            // Determine the extent of the neighborhood that will be used in the Parzen
            // windowing estimation.  The neighborhood extent is based on the optimal
            // sigma calculation and limited to a user supplied maximum radius (probably
            // the size of the domain).
    double neighborhood_radius = (m_CurrentSigma / myKappa) * 1.3 * this->GetNeighborhoodToSigmaRatio();

    if ( neighborhood_radius > this->GetMaximumNeighborhoodRadius() )
    {
        neighborhood_radius = this->GetMaximumNeighborhoodRadius();
    }

    // Get the neighborhood surrounding the point "pos".
    m_CurrentNeighborhood = system->FindNeighborhoodPoints(pos, m_CurrentWeights, neighborhood_radius, d);

    // Add the closest point on the plane as another neighbor.
    // See http://mathworld.wolfram.com/Point-PlaneDistance.html, for example
    //  std::cout << planept << "\t" << D << std::endl;

    if (domain->IsCuttingPlaneDefined())
    {
        for (unsigned int pidx = 0; pidx < domain->GetNumberOfPlanes(); pidx++)
        {
            m_CurrentNeighborhood.push_back( itk::ParticlePointIndexPair<VDimension>( planePts[pidx], 0 ) );
            // SHIREEN
            m_CurrentWeights.push_back( 0.3 );
            // end SHIREEN
        }
    }

    // Add the closest points on any spheres that are defined in the domain.
    if (domain->IsCuttingSphereDefined())
    {
        for ( unsigned int i = 0; i < domain->GetNumberOfSpheres(); i++ )
        {
            m_CurrentNeighborhood.push_back( itk::ParticlePointIndexPair<VDimension>( spherePts[i], 0 ) );
//            if (CToP[i] > abs(domain->GetSphereRadius(i)) && domain->GetSphereRadius(i) < 0)
//            {
//                std::cerr << "Original position outside the sphere: " << d << "\t" << idx << std::endl;
//                m_CurrentWeights.push_back( -3 );
//            }
//            else if (CToP[i] < abs(domain->GetSphereRadius(i)) && domain->GetSphereRadius(i) > 0)
//            {
//                std::cerr << "Original position inside the sphere: " << d << "\t" << idx << std::endl;
//                m_CurrentWeights.push_back( -3 );
//            }
//            else
                m_CurrentWeights.push_back( 0.3 );

        }
    }

    // Compute the weights based on angle between the neighbors and the center.
    //    this->ComputeAngularWeights(pos,m_CurrentNeighborhood,domain, m_CurrentWeights);

    // Estimate the best sigma for Parzen windowing.  In some cases, such as when
    // the neighborhood does not include enough points, the value will be bogus.
    // In these cases, an error != 0 is returned, and we try the estimation again
    // with an increased neighborhood radius.
    int err;
    m_CurrentSigma = EstimateSigma( idx, d, m_CurrentNeighborhood, m_CurrentWeights, pos,
                                    m_CurrentSigma, epsilon, err, m_avgKappa, numSpheres, numPlanes );

    while ( err != 0 )
    {
        neighborhood_radius *= 2.0;

        // Constrain the neighborhood size.  If we have reached a maximum
        // possible neighborhood size, we'll just go with that.
        if ( neighborhood_radius > this->GetMaximumNeighborhoodRadius() )
        {
            m_CurrentSigma = this->GetMaximumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
            neighborhood_radius = this->GetMaximumNeighborhoodRadius();
            break;
        }
        else
        {
            m_CurrentSigma = neighborhood_radius / this->GetNeighborhoodToSigmaRatio();
        }

        m_CurrentNeighborhood = system->FindNeighborhoodPoints( pos, m_CurrentWeights, neighborhood_radius, d );

        if (domain->IsCuttingPlaneDefined())
        {
            for (unsigned int pidx = 0; pidx < domain->GetNumberOfPlanes(); pidx++)
            {
                m_CurrentNeighborhood.push_back( itk::ParticlePointIndexPair<VDimension>( planePts[pidx], 0 ) );
                // SHIREEN
                m_CurrentWeights.push_back( 0.3 );
                // end SHIREEN
            }
        }

        if (domain->IsCuttingSphereDefined())
        {
            for ( unsigned int i = 0; i < domain->GetNumberOfSpheres(); i++ )
            {
                m_CurrentNeighborhood.push_back( itk::ParticlePointIndexPair<VDimension>( spherePts[i], 0 ) );
//                if (CToP[i] > abs(domain->GetSphereRadius(i)) && domain->GetSphereRadius(i) < 0)
//                {
//                    std::cerr << "Original position outside the sphere: " << d << "\t" << idx << std::endl;
//                    m_CurrentWeights.push_back( -3 );
//                }
//                else if (CToP[i] < abs(domain->GetSphereRadius(i)) && domain->GetSphereRadius(i) > 0)
//                {
//                    std::cerr << "Original position inside the sphere: " << d << "\t" << idx << std::endl;
//                    m_CurrentWeights.push_back( -3 );
//                }
//                else
                    m_CurrentWeights.push_back( 0.3 );

            }
        }

        //  m_CurrentNeighborhood = system->FindNeighborhoodPoints(pos, neighborhood_radius, d);
        //    this->ComputeAngularWeights(pos,m_CurrentNeighborhood,domain,m_CurrentWeights);

        m_CurrentSigma = EstimateSigma( idx, d, m_CurrentNeighborhood, m_CurrentWeights, pos,
                                        m_CurrentSigma, epsilon, err, m_avgKappa, numSpheres, numPlanes );
    }   // done while err

    // Constrain sigma to a maximum reasonable size based on the user-supplied
    // limit to neighborhood size.
    if ( m_CurrentSigma > this->GetMaximumNeighborhoodRadius() )
    {
        m_CurrentSigma = this->GetMaximumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
        neighborhood_radius = this->GetMaximumNeighborhoodRadius();
        m_CurrentNeighborhood = system->FindNeighborhoodPoints( pos, m_CurrentWeights,
                                                                neighborhood_radius, d );

        if (domain->IsCuttingPlaneDefined())
        {
            for (unsigned int pidx = 0; pidx < domain->GetNumberOfPlanes(); pidx++)
            {
                m_CurrentNeighborhood.push_back( itk::ParticlePointIndexPair<VDimension>( planePts[pidx], 0 ) );
                // SHIREEN
                m_CurrentWeights.push_back( 0.3 );
                // end SHIREEN
            }
        }

        if (domain->IsCuttingSphereDefined())
        {
            for ( unsigned int i = 0; i < domain->GetNumberOfSpheres(); i++ )
            {
                m_CurrentNeighborhood.push_back( itk::ParticlePointIndexPair<VDimension>( spherePts[i], 0 ) );
//                if (CToP[i] > abs(domain->GetSphereRadius(i)) && domain->GetSphereRadius(i) < 0)
//                {
//                    std::cerr << "Original position outside the sphere: " << d << "\t" << idx << std::endl;
//                    m_CurrentWeights.push_back( -3 );
//                }
//                else if (CToP[i] < abs(domain->GetSphereRadius(i)) && domain->GetSphereRadius(i) > 0)
//                {
//                    std::cerr << "Original position inside the sphere: " << d << "\t" << idx << std::endl;
//                    m_CurrentWeights.push_back( -3 );
//                }
//                else
                    m_CurrentWeights.push_back( 0.3 );
            }
        }
        //  m_CurrentNeighborhood = system->FindNeighborhoodPoints(pos, neighborhood_radius, d);
        //      this->ComputeAngularWeights(pos,m_CurrentNeighborhood,domain,m_CurrentWeights);
    }

    // Make sure sigma doesn't change too quickly!
    m_CurrentSigma = ( this->GetSpatialSigmaCache()->operator[] ( d )->operator[] ( idx ) + m_CurrentSigma ) / 2.0;

    // We are done with the sigma estimation step.  Cache the sigma value for
    // next time.
    this->GetSpatialSigmaCache()->operator[] ( d )->operator[] ( idx ) = m_CurrentSigma;
}

template <class TGradientNumericType, unsigned int VDimension>
typename ParticleOmegaGradientFunction<TGradientNumericType, VDimension>::VectorType ParticleOmegaGradientFunction<TGradientNumericType, VDimension>
::Evaluate( unsigned int idx, unsigned int d, const ParticleSystemType* system,
            double &maxmove, double &energy ) const
{
    const double epsilon = 1.0e-6;

    // Grab a pointer to the domain.  We need a Domain that has surface normal information.
    const ParticleImplicitSurfaceDomain<TGradientNumericType>* domain
            = static_cast<const ParticleImplicitSurfaceDomain<TGradientNumericType>*>( system->GetDomain( d ) );

    unsigned int numspheres = 0;
    if (domain->IsCuttingSphereDefined())
        numspheres = domain->GetNumberOfSpheres();

    unsigned int numPlanes  = 0;
    if (domain->IsCuttingPlaneDefined())
        numPlanes = domain->GetNumberOfPlanes();

    // Get the position for which we are computing the gradient.
    PointType pos = system->GetPosition( idx, d );
    vnl_vector_fixed<double, VDimension> x;

    for (unsigned int i = 0; i < VDimension; i++)  { x[i] = pos[i]; }

    // Compute the gradients
    double sigma2inv = 1.0 / ( 2.0 * m_CurrentSigma * m_CurrentSigma + epsilon );

    VectorType r;
    VectorType gradE;

    for ( unsigned int n = 0; n < VDimension; n++ )
    {
        gradE[n] = 0.0;
    }
//    std::cout << gradE[0] << "\t" << gradE[1] << "\t" << gradE[2] << std::endl;

    double mymc = m_MeanCurvatureCache->operator[] ( d )->operator[] ( idx );
    double A = 0.0;

    // AKM : Cutting Plane Disabled
    /**/

    // Distance to plane is distance to last neighbor in the list
    double planeDist = 0.0;

    /**/

    for ( unsigned int i = 0; i < m_CurrentNeighborhood.size(); i++ )
    {
        double mc;
        // AKM : Cutting Plane Disabled
        if ( i >= ( m_CurrentNeighborhood.size() - ( numspheres + numPlanes ) ) )
            //if ( i >= ( m_CurrentNeighborhood.size() - ( numspheres ) ) )
        {
            mc = mymc;
        }
        else
        {
            mc = m_MeanCurvatureCache->operator[] ( d )->operator[] ( m_CurrentNeighborhood[i].Index );
        }

        // Curvature btwn me and my neighbor
        double Dij = ( mymc + mc ) * 0.5;
        // AKM : Cutting Plane Disabled
        double kappa = this->ComputeKappa(Dij, d,sqrt(planeDist));
        //        double kappa = this->ComputeKappa( Dij, d, sqrt( 0.0 ) );

        for ( unsigned int n = 0; n < VDimension; n++ )
        {
            // Note that the Neighborhood object has already filtered the
            // neighborhood for points whose normals differ by > 90 degrees.
            r[n] = ( pos[n] - m_CurrentNeighborhood[i].Point[n] ) * kappa;
        }

        double q = kappa * exp( -dot_product( r, r ) * sigma2inv );
        //double q = exp( -dot_product(r, r) * sigma2inv);

        A += q;

        for ( unsigned int n = 0; n < VDimension; n++ )
        {
            gradE[n] += m_CurrentWeights[i] * r[n] * q;
        }
//        std::cout << gradE[0] << "\t" << gradE[1] << "\t" << gradE[2] << std::endl;
    }

    double p = 0.0;
    if ( A > epsilon )
    {    p = -1.0 / ( A * m_CurrentSigma * m_CurrentSigma );    }

    for ( unsigned int n = 0; n < VDimension; n++ )
    {    gradE[n] *= p;    }
//    std::cout << gradE[0] << "\t" << gradE[1] << "\t" << gradE[2] << std::endl;

    // Prevents unstable moves in degenerate cases
    //  if (m_CurrentNeighborhood.size() < 4)
    //    {
    //    maxmove= domain->GetImage()->GetSpacing()[0] / 100.0;
    //    }
    //  else
    //    {
    //maxmove = ( m_CurrentSigma / m_avgKappa ) * 0.1;

    // SHIREEN
    maxmove= (m_CurrentSigma / m_avgKappa) * m_MaxMoveFactor;
    // END SHIREEN


    //Praful - July 3, 2017 -- independent to maxmove
    for ( unsigned int n = 0; n < VDimension; n++ )
    {    gradE[n] /= m_avgKappa;    }
//    std::cout << gradE[0] << "\t" << gradE[1] << "\t" << gradE[2] << std::endl;

    energy = ( A * sigma2inv ) / m_avgKappa;

    return gradE;
}
} // end namespace
#endif /* ifndef __itkParticleOmegaGradientFunction_txx */

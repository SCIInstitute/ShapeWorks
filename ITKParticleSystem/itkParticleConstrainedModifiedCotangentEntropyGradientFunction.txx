/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleConstrainedModifiedCotangentEntropyGradientFunction.txx,v $
  Date:      $Date: 2014/08/18 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: shireen $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleConstrainedModifiedCotangentEntropyGradientFunction_txx
#define __itkParticleConstrainedModifiedCotangentEntropyGradientFunction_txx
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix.h"

#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>

#include <vtkIdList.h>
#include <vtkKdTreePointLocator.h>
#include <vtkKdTree.h>

namespace itk {

template <class TGradientNumericType, unsigned int VDimension>
void
ParticleConstrainedModifiedCotangentEntropyGradientFunction<TGradientNumericType, VDimension>
::EstimateGlobalSigma(const ParticleSystemType * system)
{
    //m_GlobalSigma = 1.0;
    //m_GlobalSigma = 10.0;
    //return ;

    const double epsilon = 1.0e-6;
    const int K = 6; // hexagonal ring - one jump

    const unsigned int num_samples   = system->GetNumberOfDomains(); // num_shapes * domains_per_shape
    const unsigned int num_particles = system->GetNumberOfParticles();
    const unsigned int num_shapes    = num_samples / m_DomainsPerShape;

    /* PRATEEP */
    // use fixed sigma
    if (num_particles < 7)
    {
        m_GlobalSigma = this->GetMaximumNeighborhoodRadius(); // / this->GetNeighborhoodToSigmaRatio();
        return;
    }

    double avgDist = 0.0;
    for (unsigned int domainInShape = 0; domainInShape < m_DomainsPerShape; domainInShape++)
    {
        for (unsigned int shapeNo = 0; shapeNo < num_shapes; shapeNo++)
        {
            // linear index of the current domain in the particle system
            int dom = shapeNo * m_DomainsPerShape + domainInShape;

            // get the particle of the current shape sample
            vtkSmartPointer< vtkPoints > points = vtkSmartPointer< vtkPoints >::New();
            for (unsigned int idx = 0; idx < num_particles ; idx++)
            {
                // get the current particle
                PointType pos              = system->GetPosition(idx, dom);
                points->InsertNextPoint(pos[0], pos[1], pos[2]);

            }

            vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
            polydata->SetPoints(points);

            // Create the kdtree for the current set of particles
            vtkSmartPointer<vtkKdTreePointLocator> kDTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
            kDTree->SetDataSet(polydata);
            kDTree->BuildLocator();

            // Find the closest points to each particle
            for (unsigned int idx = 0; idx < num_particles ; idx++)
            {
                double p[3];
                points->GetPoint(idx, p);

                vtkSmartPointer<vtkIdList> result = vtkSmartPointer<vtkIdList>::New();
                kDTree->FindClosestNPoints( K+1, p, result ); // +1 to exclude myself

                double meanDist = 0;
                for(vtkIdType k = 0; k < K+1; k++)
                {
                    vtkIdType pid = result->GetId(k);

                    if (pid == idx) // close to myself
                        continue;

                    double pk[3];
                    points->GetPoint(pid, pk);

                    double curDist = sqrt(pow(p[0]-pk[0],2.0) + pow(p[1]-pk[1],2.0) + pow(p[2]-pk[2],2.0));

                    meanDist += curDist;
                }
                meanDist /= K;

                avgDist+= meanDist;
            }
        }
    }

    avgDist /= (num_particles * num_shapes * m_DomainsPerShape);

    m_GlobalSigma = avgDist / 0.57; // based on hexagonal packing, sigma is the distance to the second ring
    //m_GlobalSigma = avgDist ; // based on hexagonal packing, sigma is the distance to the second ring

    if (m_GlobalSigma < epsilon)
    {
        m_GlobalSigma = this->GetMinimumNeighborhoodRadius() / this->GetNeighborhoodToSigmaRatio();
    }
}

template <class TGradientNumericType, unsigned int VDimension>
void
ParticleConstrainedModifiedCotangentEntropyGradientFunction<TGradientNumericType, VDimension>
::BeforeEvaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system)
{
    // Praful -- SHIREEN
        m_MaxMoveFactor = 0.1;
//        std::cout<<"-------------------"<<std::endl;
//        std::cout<<"maxmovefactor = "<<m_MaxMoveFactor<<std::endl;
//        std::cout<<"-------------------"<<std::endl;
        // END Praful -- SHIREEN

    // Grab a pointer to the domain.  We need a Domain that has surface normal
    // information and a cutting plane
    const ParticleImplicitSurfaceDomain<TGradientNumericType, VDimension>* domain
            = static_cast<const ParticleImplicitSurfaceDomain<TGradientNumericType,
            VDimension>*>( system->GetDomain( d ) );

    // Get the position for which we are computing the gradient.
    PointType pos = system->GetPosition(idx, d);

    // Determine the extent of the neighborhood that will be used in the Parzen
    // windowing estimation.
    //double neighborhood_radius = m_GlobalSigma * 1.3 * this->GetNeighborhoodToSigmaRatio();
    //double neighborhood_radius = m_GlobalSigma * 10 * this->GetNeighborhoodToSigmaRatio();
    double neighborhood_radius = m_GlobalSigma * NBHD_SIGMA_FACTOR * this->GetNeighborhoodToSigmaRatio();

    if (neighborhood_radius > this->GetMaximumNeighborhoodRadius())
    {
        neighborhood_radius = this->GetMaximumNeighborhoodRadius();
    }

    // Get the neighborhood surrounding the point "pos".
    m_CurrentNeighborhood = system->FindNeighborhoodPoints(pos, m_CurrentWeights, neighborhood_radius, d);

    // PRATEEP
    vnl_vector_fixed<double, VDimension> x;
    vnl_vector_fixed<float, VDimension> grad = domain->SampleGradientVnl(pos);
    for (unsigned int i =0; i < VDimension; i++) { x[i] = pos[i]; }
    double D = dot_product( domain->GetCuttingPlaneNormal(),
                            x - domain->GetCuttingPlanePoint());
    D = -fabs(D);
    itk::Point<double, VDimension> planept;
    for (unsigned int i = 0; i < VDimension; i++)
    { planept[i] = x[i] - (domain->GetCuttingPlaneNormal()[i] * D); }
    m_CurrentNeighborhood.push_back( itk::ParticlePointIndexPair<VDimension>(planept, 0));
    m_CurrentWeights.push_back(0.3);
    // end PRATEEP

    // didn't work, for zero-mode, ensemble mean function didnt converge to zero compared to the original shapeworks weighting
    //    // adjusting the weights according to Meyer's thesis
    //    double gamma = 0.156;

    //    // get domain information with gradients
    //    const ParticleImageDomainWithGradients<float, VDimension> * domain
    //            = static_cast<const ParticleImageDomainWithGradients<float, VDimension> *>(system->GetDomain(d));

    //    // get normal for current position
    //    typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType pos_normal = domain->SampleNormalVnl(pos, 1.0e-10);

    //    for (unsigned int k = 0; k < m_CurrentNeighborhood.size(); k++)
    //    {
    //        typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType neigh_normal
    //                = domain->SampleNormalVnl(m_CurrentNeighborhood[k].Point, 1.0e-10);

    //        double dot_prod = dot_product(pos_normal,neigh_normal); // normals already normalized

    //        if (dot_prod >= gamma)
    //            m_CurrentWeights[k] = 1.0;
    //        else
    //        {
    //            if (dot_prod <= 0)
    //                m_CurrentWeights[k] = 0.0;
    //            else
    //            {
    //                m_CurrentWeights[k] = cos((gamma - dot_prod)/gamma);
    //            }
    //        }
    //    }


    //    std::cout << "idx: " << idx << ", sigma:" << m_GlobalSigma << std::endl;
    //    std::cout << "idx: " << idx << ", neighborhood_radius:" << neighborhood_radius << std::endl;
    //    std::cout << "idx: " << idx << ", number of neighbors:" << m_CurrentNeighborhood.size() << std::endl;

    // Add the closest points on any spheres that are defined in the domain.
    //std::vector<itk::Point<double, VDimension> > spherepoints;

    for ( unsigned int i = 0; i < domain->GetNumberOfSpheres(); i++ )
    {
        itk::Point<double, VDimension> spherept;
        vnl_vector_fixed<double, VDimension> q;

        for ( unsigned int j = 0; j < VDimension; j++ )
            q[j] = pos[j] - domain->GetSphereCenter( i )[j];

        q.normalize();

        for ( unsigned int j = 0; j < VDimension; j++ )
            spherept[j] = domain->GetSphereCenter( i )[j] + ( q[j] * domain->GetSphereRadius( i ) );

        //spherepoints.push_back( spherept );
        m_CurrentNeighborhood.push_back( itk::ParticlePointIndexPair<VDimension>( spherept, 0 ) );

        // give small weight to this sphere point
        // m_CurrentWeights.push_back( 0.01 );
        m_CurrentWeights.push_back( 0.3 );
    }
}


template <class TGradientNumericType, unsigned int VDimension>
typename ParticleConstrainedModifiedCotangentEntropyGradientFunction<TGradientNumericType, VDimension>::VectorType
ParticleConstrainedModifiedCotangentEntropyGradientFunction<TGradientNumericType, VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxmove, double &energy) const
{
    const double epsilon = 1.0e-6;

    // Grab a pointer to the domain.  We need a Domain that has surface normal information.
    const ParticleImplicitSurfaceDomain<TGradientNumericType, VDimension>* domain
            = static_cast<const ParticleImplicitSurfaceDomain<TGradientNumericType,
            VDimension>*>( system->GetDomain( d ) );

    unsigned int numspheres = domain->GetNumberOfSpheres();

    // Get the position for which we are computing the gradient.
    PointType pos = system->GetPosition(idx, d);

    // Compute the gradients
    VectorType r;
    VectorType gradE;
    double rmag;

    for (unsigned int n = 0; n < VDimension; n++)
    {
        gradE[n] = 0.0;
    }

    double prob_xi = epsilon;
    double M       = epsilon;
    for (unsigned int k = 0; k < m_CurrentNeighborhood.size(); k++)
    {
        for (unsigned int n = 0; n < VDimension; n++)
        {
            // Note that the Neighborhood object has already filtered the
            // neighborhood for points whose normals differ by > 90 degrees.
            r[n] = (pos[n] - m_CurrentNeighborhood[k].Point[n]) ;
        }
        rmag = r.magnitude();

        double dPhi = this->ComputeModifiedCotangentDerivative(rmag);
        for (unsigned int n = 0; n < VDimension; n++)
        {
            gradE[n] += ( ( m_CurrentWeights[k] * dPhi * r[n] )/(rmag + epsilon) );
        }

        prob_xi += m_CurrentWeights[k] * this->ComputeModifiedCotangent(rmag);
        M       += m_CurrentWeights[k];
    }

    prob_xi /= M;
    for (unsigned int n = 0; n < VDimension; n++)
    {
        gradE[n] *= ( (-1.0/ (M * prob_xi ) ) );
    }


    //maxmove = m_GlobalSigma * 0.1;

    // Praful -- SHIREEN
    maxmove = m_GlobalSigma * m_MaxMoveFactor;
    //end Praful -- SHIREEN

    energy  = prob_xi ;

    return gradE ;
}

}// end namespace
#endif

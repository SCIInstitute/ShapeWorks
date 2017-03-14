/*=========================================================================
Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
Module:    $RCSfile: itkParticleEnsembleNormalPenaltyFunction.txx,v $
Date:      $Date: 2011/03/24 01:17:33 $
Version:   $Revision: 1.2 $
Author:    $Author: wmartin $

Copyright (c) 2009 Scientific Computing and Imaging Institute.
See ShapeWorksLicense.txt for details.
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#ifndef __itkParticleEnsembleNormalPenaltyFunction_txx
#define __itkParticleEnsembleNormalPenaltyFunction_txx

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_det.h"
#include "itkParticleEnsembleNormalPenaltyFunction.h"

#include "../Utilities/utils.h"
//using namespace utils;
namespace itk
{

template <unsigned int VDimension>
typename ParticleEnsembleNormalPenaltyFunction<VDimension>::VectorType
ParticleEnsembleNormalPenaltyFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxmove, double &energy) const
{

    const double epsilon = 1.0e-8;
    const double N = (double)(system->GetNumberOfDomains() / m_DomainsPerShape);

    std::vector < typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType > curNormals;
    curNormals.resize(N);

    std::vector < typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType > curNormals_local;
    curNormals_local.resize(N);

    std::vector< double > thetas;
    thetas.resize(N);

    std::vector< double > phis;
    phis.resize(N);

    // Get the position for which we are computing the gradient
    //PointType pos = system->GetTransformedPosition(idx, d);

    // SHIREEN - normals should be sampled using local points
    PointType pos = system->GetPosition(idx, d);
    // end SHIREEN

    // get domain information with gradients
    const ParticleImageDomainWithGradients<float, VDimension> * domain
            = static_cast<const ParticleImageDomainWithGradients<float, VDimension> *>(system->GetDomain(d));

    // get normal for current position
    typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType posnormal_local = domain->SampleNormalVnl(pos);
    VectorType v_posnormal;
    v_posnormal.fill(0.0);
    for(unsigned int n = 0; n < VDimension; n++)
    {
        v_posnormal[n] = posnormal_local[n];
    }
    VectorType pn_world = system->TransformVector(v_posnormal, system->GetTransform(d)*system->GetPrefixTransform(d));
    typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType posnormal;
    posnormal[0] = pn_world[0];
    posnormal[1] = pn_world[1];
    posnormal[2] = pn_world[2];

    //------------------------------------------------- Praful Code
    int shapeNo = 0;
//    CartesianToSpherical(posnormal, mean_normal_sph);
    for (unsigned int i = d % m_DomainsPerShape; i < system->GetNumberOfDomains(); i += m_DomainsPerShape)
    {
        domain = static_cast<const ParticleImageDomainWithGradients<float,VDimension> *>(system->GetDomain(i));
        PointType neighpos = system->GetPosition(idx, i); //Praful
        typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType neighnormal = domain->SampleNormalVnl(neighpos);

        VectorType neighnormal_cart;
        neighnormal_cart[0] = neighnormal[0];
        neighnormal_cart[1] = neighnormal[1];
        neighnormal_cart[2] = neighnormal[2];

        curNormals_local[shapeNo] = neighnormal;

        VectorType neighnormal_world = system->TransformVector(neighnormal_cart, system->GetTransform(i)*system->GetPrefixTransform(i));
        curNormals[shapeNo][0] = neighnormal_world[0];
        curNormals[shapeNo][1] = neighnormal_world[1];
        curNormals[shapeNo][2] = neighnormal_world[2];

        double in[3] = {curNormals[shapeNo][0], curNormals[shapeNo][1], curNormals[shapeNo][2]};
        double out[3];
        utils::cartesian2spherical(in, out);
        phis[shapeNo] = out[1];
        thetas[shapeNo] = out[2];
        shapeNo++;
    }

    double avgNormal_sph[3];
    double avgNormal_cart[3];
    avgNormal_sph[0] = 1;
    avgNormal_sph[1] = utils::averageThetaArc(phis);
    avgNormal_sph[2] = utils::averageThetaArc(thetas);
    utils::spherical2cartesian(avgNormal_sph, avgNormal_cart);

    typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType avgnormal;
    avgnormal[0] = avgNormal_cart[0];
    avgnormal[1] = avgNormal_cart[1];
    avgnormal[2] = avgNormal_cart[2];
//------------------------------------------------- Praful Code -- end

    //compute gradient
    domain = static_cast< const ParticleImageDomainWithGradients<float,VDimension> *> (system->GetDomain(d));

    // get local copy of image
    imgDuplicator->SetInputImage(domain->GetImage());
    imgDuplicator->Update();

    // get normal components in neighborhood around current point
    typename NormalComponentImageType::SizeType nSize;
    typename NormalComponentImageType::IndexType nIndex;
    typename NormalComponentImageType::RegionType nRegion;

    // set up neighborhood iterator around current particle position in image
    nSize.Fill(1); nRegion.SetSize(nSize);
    imgDuplicator->GetOutput()->TransformPhysicalPointToIndex(pos,nIndex); nRegion.SetIndex(nIndex);
    NeighborhoodIteratorType it(radius,imgDuplicator->GetOutput(),nRegion);

    // get components of surface normals in the neighborhood
    ImageIteratorType itImg(normComp,normComp->GetRequestedRegion());
    typename NormalComponentImageType::PointType currPoint;
    typename ParticleImageDomainWithGradients<float, VDimension>::VnlVectorType currNormal;

    // create kernel - directional derivative operator
    typedef typename itk::DerivativeOperator<float, VDimension> DerivativeOperatorType;
    DerivativeOperatorType derivativeOperator;
    derivativeOperator.CreateToRadius(radius);

    vnl_matrix<double> normalPartialDerivatives(VDimension,VDimension, 0.0f);
    nSize.Fill(1); nRegion.SetSize(nSize);
    nIndex.Fill(1); nRegion.SetIndex(nIndex);

    for (unsigned int comp = 0; comp < VDimension; comp++)
    {
        itImg = itImg.Begin();
        for (unsigned int o = 0; o < it.Size(); ++o, ++itImg)
        {
            imgDuplicator->GetOutput()->TransformIndexToPhysicalPoint(it.GetIndex(o),currPoint);
            currNormal = domain->SampleNormalVnl(currPoint);
            itImg.Set(currNormal[comp]);
        }

        // compute derivative using inner product
        NeighborhoodIteratorType itComp(radius,normComp,nRegion);
        for (unsigned int dirValue = 0; dirValue < VDimension; dirValue++)
        {
            derivativeOperator.SetDirection(dirValue);
            derivativeOperator.CreateDirectional();
            normalPartialDerivatives(dirValue,comp) = inprod(itComp,derivativeOperator);
        }
    }

//    double df_dn = fprime( dot_product(v_posnormal, mean_normal) );

    // compute gradient
    vnl_vector<double> gradE_norm(VDimension, 0.0f);

    for (unsigned int n = 0; n < VDimension; n++)
    {
        gradE_norm[n] = avgnormal[n] - posnormal[n];  //mean energy based normal penalty
//                gradE_norm[n] = mean_normal[n] * df_dn; //dot product based normal penalty
    }

//        gradE_norm *= mat3x3 * normalPartialDerivatives; //dot product based normal penalty
    gradE_norm *= normalPartialDerivatives;            //mean energy based normal penalty
    //    energy = gradE_norm.magnitude();
    energy = 0.0;
    shapeNo = 0;
    for (unsigned int i = d % m_DomainsPerShape; i < system->GetNumberOfDomains(); i += m_DomainsPerShape)
    {
        typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType diff = avgnormal - curNormals[shapeNo++]; //mean energy based normal penalty
        energy += 0.5 * diff.magnitude() * diff.magnitude();  //mean energy based normal penalty
    }

    //    maxmove = domain->GetImage()->GetSpacing()[0];
//    std::cout<< "Normal_Energy: " << energy << std::endl;
    maxmove = energy * 0.5;

    //  Transform the gradient according to the transform of the given domain and return.
    return system->TransformVector(gradE_norm, system->GetInversePrefixTransform(d)
                                   * system->GetInverseTransform(d));
}

// Praful
//template <unsigned int VDimension>
//void ParticleEnsembleNormalPenaltyFunction<VDimension>::CartesianToSpherical(VectorType cart, VectorType &spher) const
//{
//    spher.fill(0.0);
//    spher[0] = cart.magnitude();
//    spher[1] = std::acos(cart[2]/spher[0]);
//    spher[2] = std::atan2(cart[1], cart[0]);
//}

//template <unsigned int VDimension>
//void ParticleEnsembleNormalPenaltyFunction<VDimension>::SphericalToCartesian(VectorType spher, VectorType &cart) const
//{
//    cart.fill(0.0);
//    cart[0] = spher[0]*std::sin(spher[1])*std::cos(spher[2]);
//    cart[1] = spher[0]*std::sin(spher[1])*std::sin(spher[2]);
//    cart[2] = spher[0]*std::cos(spher[1]);
//}

/* PRATEEP */
// f(x) = (1-x)^2 / (1+x)^2
template <unsigned int VDimension>
double ParticleEnsembleNormalPenaltyFunction<VDimension>::f(double ctheta) const
{
    const double epsilon = 1.0e-10;
    double num = (1.0f * (1-ctheta))       * (1.0f * (1-ctheta));
    double den = (1.0f + ctheta + epsilon) * (1.0f + ctheta + epsilon);
    return num / den;
}

// f'(x) = -(4 * (1-x)) / (1+x)^3
template <unsigned int VDimension>
double ParticleEnsembleNormalPenaltyFunction<VDimension>::fprime(double ctheta) const
{
    const double epsilon = 1.0e-10;
    double num = (1.0f - ctheta);
    double den = (1.0f + ctheta + epsilon);
    return (-4.0f*num) / (den * den * den);
}

} // end namespace

#endif

/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEnsembleNormalPenaltyFunction.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEnsembleNormalPenaltyFunction_h
#define __itkParticleEnsembleNormalPenaltyFunction_h

#include "itkParticleVectorFunction.h"
#include <vector>
#include "itkParticleImageDomainWithGradients.h"
#include "itkDerivativeOperator.h"
#include "itkNeighborhoodIterator.h"
#include "itkImageDuplicator.h"
#include "itkImageRegionIterator.h"

namespace itk
{

/**
 * \class ParticleEnsembleNormalPenaltyFunction
 *
 */
template <unsigned int VDimension>
class ParticleEnsembleNormalPenaltyFunction : public ParticleVectorFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef ParticleEnsembleNormalPenaltyFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleVectorFunction<VDimension> Superclass;
  itkTypeMacro( ParticleEnsembleNormalPenaltyFunction, ParticleVectorFunction);

  /** Type of particle system. */
  typedef typename Superclass::ParticleSystemType ParticleSystemType;

  /** Vector & Point types. */
  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystemType::PointType PointType;

  // Normal Penalty related classes
  typedef typename itk::Image<float,VDimension> NormalComponentImageType;
  typedef typename itk::NeighborhoodIterator<NormalComponentImageType> NeighborhoodIteratorType;
  typedef typename itk::ImageRegionIterator<NormalComponentImageType> ImageIteratorType;
  typedef typename itk::DerivativeOperator<float, VDimension> DerivativeOperatorType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  virtual VectorType Evaluate(unsigned int a, unsigned int b,
                              const ParticleSystemType *c, double &d, double &e) const;

  virtual VectorType Evaluate(unsigned int a,  unsigned int b, 
                              const ParticleSystemType *c, double &d) const
  {
    double e;
    return this->Evaluate(a,b,c,d,e);
  }

  virtual double Energy(unsigned int a, unsigned int b,
                        const ParticleSystemType *c) const
  {
    double e, d;
    this->Evaluate(a,b,c,d,e);
    return e;
  }

  /** Set/Get the number of domains per shape.  Separate means are used for
      separate shape domains. */
  void SetDomainsPerShape(int i)
  { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const
  { return m_DomainsPerShape; }

  /* PRATEEP */
  /*
  void SetDiagnosticsOutputPrefix( const std::string s)
  {
      m_diagnostics_prefix = s;
  }

  virtual void SaveDiagnosticsInfo(int iteration_number, const ParticleSystemType * system) const
  {
      const double epsilon = 1.0e-6;

      const unsigned int num_samples   = system->GetNumberOfDomains(); // num_shapes * domains_per_shape
      const unsigned int num_particles = system->GetNumberOfParticles();
      const unsigned int num_shapes    = num_samples / m_DomainsPerShape;

      // save particle normals diagnostics
      double total_energy = 0.0;
      std::string curfilename = m_diagnostics_prefix
              + "EnsembleNormalPenaltyFunction.p" + utils::num2str((int)num_particles) + ".csv";

      for (unsigned int domainInShape = 0; domainInShape < m_DomainsPerShape; domainInShape++)
      {
          for (unsigned int idx = 0; idx < num_particles ; idx++)
          {
              std::vector<float> curAngles;
              curAngles.resize(num_shapes);

              std::vector < typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType > curNormals;
              curNormals.resize(num_shapes);

              std::vector < typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType > curNormals_local;
              curNormals_local.resize(num_shapes);

              std::vector< double > thetas;
              thetas.resize(num_shapes);

              std::vector< double > phis;
              phis.resize(num_shapes);

              for (unsigned int shapeNo = 0; shapeNo < num_shapes; shapeNo++)
              {
                  // linear index of the current domain in the particle system
                  int dom = shapeNo * m_DomainsPerShape + domainInShape;

                  // get domain information with gradients
                  const ParticleImageDomainWithGradients<float, VDimension> * domain
                          = static_cast<const ParticleImageDomainWithGradients<float, VDimension> *>(system->GetDomain(dom));

                  // get the current particle
                  PointType pos              = system->GetPosition(idx, dom);

                  // get normal for current position, DT-based computation ( gradF/|gradF| ), no -ve taken in shapeworks ???
                  typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType pn
                          = domain->SampleNormalVnl(pos); // normals should be sampled using the local coordinates (lpts)

                  VectorType posnormal;
                  posnormal[0] = pn[0];
                  posnormal[1] = pn[1];
                  posnormal[2] = pn[2];

                  curNormals_local[shapeNo][0] = posnormal[0];
                  curNormals_local[shapeNo][1] = posnormal[1];
                  curNormals_local[shapeNo][2] = posnormal[2];

                  // normals to be compared needed to be transformed to the world coordinates
                  // this transformation only apply the rotational component to the normal (see GetTransformedPosition)
                  VectorType posnormal_world = system->TransformVector(posnormal,
                                                                       system->GetTransform(dom) *
                                                                       system->GetPrefixTransform(dom) );

                  curNormals[shapeNo][0] = posnormal_world[0];
                  curNormals[shapeNo][1] = posnormal_world[1];
                  curNormals[shapeNo][2] = posnormal_world[2];

                  double in[3] = {curNormals[shapeNo][0], curNormals[shapeNo][1], curNormals[shapeNo][2]};
                  double out[3];
                  utils::cartesian2spherical(in, out);

                  phis[shapeNo]   = out[1];
                  thetas[shapeNo] = out[2];
              }

              // computing the average normal
              double avgNormal_sph[3];
              double avgNormal_cart[3];

              avgNormal_sph[0] = 1; // unit vector with length = 1
              avgNormal_sph[1] = utils::averageThetaArc(phis);
              avgNormal_sph[2] = utils::averageThetaArc(thetas);

              utils::spherical2cartesian(avgNormal_sph, avgNormal_cart);

              typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType avgnormal;
              avgnormal[0] = avgNormal_cart[0];
              avgnormal[1] = avgNormal_cart[1];
              avgnormal[2] = avgNormal_cart[2];

              // compute angle of each normal to the average normal
              for (unsigned int shapeNo = 0; shapeNo < num_shapes; shapeNo++)
              {
                  typename ParticleImageDomainWithGradients<float,VDimension>::VnlVectorType posnormal = curNormals[shapeNo];

                  // get the cosine of the angle between the current normal and the average normal
                  float cosine = dot_product(posnormal,avgnormal) /
                          (posnormal.magnitude()*avgnormal.magnitude() + 1.0e-6);

//                  curAngles[shapeNo] = acos(cosine);
              }
          }

          // save the current value for the energy
          std::fstream ofs;
          ofs.open(curfilename.c_str(), std::fstream::out | std::fstream::app);
          ofs << iteration_number << ", " << total_energy << ",";
          ofs << std::endl;
          ofs.close();

      } // end domainInShape

  }
  // PRATEEP
  */

  virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
  {
    typename ParticleEnsembleNormalPenaltyFunction<VDimension>::Pointer copy = ParticleEnsembleNormalPenaltyFunction<VDimension>::New();

    // from itkParticleVectorFunction
    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;

    // local
    copy->m_DomainsPerShape = this->m_DomainsPerShape;

    return (typename ParticleVectorFunction<VDimension>::Pointer)copy;

  }

  /* PRATEEP */
  double f(double ctheta) const;

  double fprime(double ctheta) const;


protected:
  ParticleEnsembleNormalPenaltyFunction()
  {
    m_DomainsPerShape = 1;

    imgDuplicator = itk::ImageDuplicator<NormalComponentImageType>::New();

    normComp = NormalComponentImageType::New();
    typename NormalComponentImageType::SizeType tmpSize; tmpSize.Fill(VDimension);
    typename NormalComponentImageType::IndexType tmpIndex; tmpIndex.Fill(0);
    typename NormalComponentImageType::RegionType tmpRegion; tmpRegion.SetSize(tmpSize); tmpRegion.SetIndex(tmpIndex);
    normComp->SetRegions(tmpRegion); normComp->Allocate(); normComp->FillBuffer(0.0f);

    radius.Fill(1);
  }

  virtual ~ParticleEnsembleNormalPenaltyFunction() {}
  //void operator=(const ParticleEnsembleNormalPenaltyFunction &);
  ParticleEnsembleNormalPenaltyFunction(const ParticleEnsembleNormalPenaltyFunction &);

  int m_DomainsPerShape;  
  
  typename itk::ImageDuplicator<NormalComponentImageType>::Pointer imgDuplicator;

  typename NormalComponentImageType::Pointer normComp;

  typename NeighborhoodIteratorType::RadiusType radius; 

  typename itk::NeighborhoodInnerProduct<NormalComponentImageType> inprod;

  /* PRATEEP */
  std::vector< vnl_vector<double> > m_meanWorldNormalCache;
  std::string m_diagnostics_prefix;

};

} //end namespace

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleEnsembleNormalPenaltyFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleEnsembleNormalPenaltyFunction.txx"
#endif

#endif

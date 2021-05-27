/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkParticleDomain.h"
#include "MeshWrapper.h"
#include "TriMeshWrapper.h"
#include <itkObjectFactory.h>

namespace itk
{
class MeshDomain : public ParticleDomain
{
public:
  /** Standard class typedefs */
  typedef SmartPointer<MeshDomain> Pointer;
  itkSimpleNewMacro(MeshDomain);

  /** Point type used to store particle locations. */
  typedef typename ParticleDomain::PointType PointType;


  shapeworks::DomainType GetDomainType() const override {
    return shapeworks::DomainType::Mesh;
  }

  bool ApplyConstraints(PointType &p, int idx, bool dbg = false) const override;
  bool ApplyVectorConstraints(vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos) const;
  vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos, int idx) const override;
  PointType UpdateParticlePosition(const PointType &point, int idx, vnl_vector_fixed<double, DIMENSION> &update) const override;

  virtual void InvalidateParticlePosition(int idx) const override;


  double GetCurvature(const PointType &p, int idx) const override {
    //TODO Why not return the actual curvature
    return GetSurfaceMeanCurvature();
  }

  inline double GetSurfaceMeanCurvature() const override {
    // This function is used by MeanCurvatureAttribute which is used for good/bad assessment
    // These arbitrary values should eventually be replaced with actual computation
    return 0.15;
  }

  inline double GetSurfaceStdDevCurvature() const override {
    // This function is used by MeanCurvatureAttribute which is used for good/bad assessment
    // These arbitrary values should eventually be replaced with actual computation
    return 0.02;
  }

  const PointType& GetLowerBound() const override {
    return meshWrapper->GetMeshLowerBound();
  }
  const PointType& GetUpperBound() const override {
    return meshWrapper->GetMeshUpperBound();
  }


  PointType GetZeroCrossingPoint() const override {
    // TODO Hong
    // Apply constraints somehow
    if(meshWrapper == nullptr) {
      // Fixed domain. Unsure if this is the correct thing to do, but it preserves existing behaviour.
      PointType p;
      p[0] = p[1] = p[2] = 0;
      return p;
    }
    return meshWrapper->GetPointOnMesh();
  }

  PointType GetValidLocationNear(PointType p) const override {
    PointType valid;
    valid[0] = p[0]; valid[1] = p[1]; valid[2] = p[2];
    ApplyConstraints(valid, -1);
    return valid;
  }

  double GetSurfaceArea() const override {
    // TODO return actual surface area
    return 0;
  }

  double GetMaxDiameter() const override;

  inline vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint(const PointType &point, int idx) const override {
    return meshWrapper->SampleNormalAtPoint(point, idx);
  }
  inline vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType & point, int idx) const override {
    return meshWrapper->SampleNormalAtPoint(point, idx);
  }
  inline GradNType SampleGradNAtPoint(const PointType &p, int idx) const override {
    return meshWrapper->SampleGradNAtPoint(p, idx);
  }

  inline double Distance(const PointType &a, int idx_a,
                         const PointType &b, int idx_b,
                         vnl_vector_fixed<double, DIMENSION> *out_grad=nullptr) const override {
      return meshWrapper->ComputeDistance(a, idx_a, b, idx_b, out_grad);
  }
  inline double SquaredDistance(const PointType &a, int idx_a, const PointType &b, int idx_b) const override {
    double dist = meshWrapper->ComputeDistance(a, idx_a, b, idx_b);
    return dist * dist;
  }
  inline bool IsWithinDistance(const PointType &a, int idx_a,
                               const PointType &b, int idx_b, double test_dist, double& dist) const override {
    return meshWrapper->IsWithinDistance(a, idx_a, b, idx_b, test_dist, dist);
  }

  void DeleteImages() override {
    // TODO Change this to a generic delete function
  }
  void DeletePartialDerivativeImages() override {
    // TODO Change this to a generic delete function
  }

  void SetMesh(std::shared_ptr<shapeworks::MeshWrapper> mesh_)  {
    this->m_FixedDomain = false;
    meshWrapper = mesh_;
  }

  void UpdateZeroCrossingPoint() override {
  }

protected:
  MeshDomain() { }
  virtual ~MeshDomain() {}

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    DataObject::Superclass::PrintSelf(os, indent);
    os << indent << "MeshDomain\n";
  }

private:
  std::shared_ptr<shapeworks::MeshWrapper> meshWrapper;
  PointType m_ZeroCrossingPoint;

};

}

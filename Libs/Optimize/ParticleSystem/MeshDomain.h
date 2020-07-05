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

namespace itk
{
class MeshDomain : public ParticleDomain
{
public:
  /** Standard class typedefs */
  typedef SmartPointer<MeshDomain> Pointer;

  /** Point type used to store particle locations. */
  typedef typename ParticleDomain::PointType PointType;


  shapeworks::DomainType GetDomainType() const override {
    return shapeworks::DomainType::Mesh;
  }

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface.  Default behavior does nothing.  Returns true if the value of
      the point was modified and false otherwise. */
  bool ApplyConstraints(PointType &p) const override;
  bool ApplyVectorConstraints(vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos) const override;
  vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos) const override;
  PointType UpdateParticlePosition(PointType &point, vnl_vector_fixed<double, DIMENSION> &update) const override;

  double GetCurvature(const PointType &p) const override {
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

  PointType GetValidLocation() const override {
    return meshWrapper->GetPointOnMesh();
  }
  double GetSurfaceArea() const override {
    // TODO return actual surface area
    return 0;
  }

  double GetMaxDiameter() const override;

  inline vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType & point) const override {
    return meshWrapper->SampleNormalAtPoint(point);
  }

  inline double Distance(const PointType &a, const PointType &b) const override {
      return meshWrapper->ComputeDistance(a, b);
  }
  inline double SquaredDistance(const PointType &a, const PointType &b) const override {
    double dist = meshWrapper->ComputeDistance(a, b);
    return dist * dist;
  }

  void PrintCuttingPlaneConstraints(std::ofstream& out) const override {
    // TODO for Farshad: figure out constraint thing
  }
  void SetCuttingPlane(const vnl_vector<double>& a, const vnl_vector<double>& b, const vnl_vector<double>& c) {
    // TODO for Farshad: figure out constraint thing
  }
  void TransformCuttingPlane(const vnl_matrix_fixed<double, DIMENSION + 1, DIMENSION + 1> & Trans) {
    // TODO for Farshad: figure out constraint thing
  }
  void AddSphere(const vnl_vector_fixed<double, DIMENSION>& v, double r) {
    // TODO for Farshad: figure out constraint thing
  }


  void DeleteImages() override {
    // TODO Change this to a generic delete function
  }
  void DeletePartialDerivativeImages() override {
    // TODO Change this to a generic delete function
  }

  void SetMesh(shapeworks::MeshWrapper* mesh_) {
    this->m_FixedDomain = false;
    meshWrapper = mesh_;
  }

  MeshDomain() { }
  virtual ~MeshDomain() {}

protected:
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    DataObject::Superclass::PrintSelf(os, indent);
    os << indent << "MeshDomain\n";
  }
  
private:
  shapeworks::MeshWrapper* meshWrapper;

};

}

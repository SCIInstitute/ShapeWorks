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
  template<class T>
  inline std::string PrintValue(T value) const {
    return "(" + std::to_string(value[0]) + ", " + std::to_string(value[1]) + ", " + std::to_string(value[2]) + ")";
  }
  /** Standard class typedefs */
  typedef SmartPointer<MeshDomain>  Pointer;

  /** Point type used to store particle locations. */
  typedef typename ParticleDomain::PointType PointType;


  shapeworks::DomainType GetDomainType() const override {
    return shapeworks::DomainType::Mesh;
  }

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface.  Default behavior does nothing.  Returns true if the value of
      the point was modified and false otherwise. */
  inline bool ApplyConstraints(PointType& p) const override {
    // TODO snap the point to the closest position on the mesh or something
    // TODO need to refactor particle updates into a function so that mesh can do its own update method.
    p = meshWrapper->SnapToMesh(p);
    return true;
  }
  inline bool ApplyVectorConstraints(vnl_vector_fixed<double, DIMENSION> & gradE, const PointType &pos) const override {
    // TODO need to refactor into update particle method
    // TODO reduce magnitude of vector so it doesn't violate constraints
    return true;
  }
  inline vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION> & gradE, const PointType &pos) const override {

    vnl_vector_fixed<float, DIMENSION> vector;
    for (unsigned int i = 0; i < DIMENSION; i++) { vector[i] = gradE[i]; }

    vnl_vector_fixed<float, DIMENSION> projected = meshWrapper->ProjectVectorToSurfaceTangent(pos, vector);

    vnl_vector_fixed<double, DIMENSION> result;
    for (unsigned int i = 0; i < DIMENSION; i++) { result[i] = projected[i]; }
    return result;
  }

  inline PointType UpdateParticlePosition(PointType& point, vnl_vector_fixed<double, DIMENSION> & update) const override {
    PointType newpoint;

    vnl_vector_fixed<float, DIMENSION> negativeUpdate;
    for (unsigned int i = 0; i < DIMENSION; i++) { negativeUpdate[i] = -update[i]; }
    PointType walkedPoint = meshWrapper->GeodesicWalk(point, negativeUpdate);
    for (unsigned int i = 0; i < DIMENSION; i++) { newpoint[i] = walkedPoint[i]; }
    ApplyConstraints(newpoint);
    return newpoint;
  }

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

  // TODO replace the default euclidean distance computation with geodesic distance computation
  //double Distance(const PointType& a, const PointType& b) const override;

  inline std::string PrintPoint(PointType point) const {
    return "(" + std::to_string(point[0]) + ", " + std::to_string(point[1]) + ", " + std::to_string(point[2]) + ")";
  }

  const PointType& GetLowerBound() const override {
    return meshWrapper->GetMeshLowerBound();
  }
  const PointType& GetUpperBound() const override {
    return meshWrapper->GetMeshUpperBound();
  }

  PointType GetZeroCrossingPoint() const override {
    return meshWrapper->GetPointOnMesh();
  }
  double GetSurfaceArea() const override {
    // TODO return actual surface area
    return 0;
  }
  double GetMaxDimRadius() const override {
    PointType size = meshWrapper->GetMeshUpperBound() - meshWrapper->GetMeshLowerBound();
    double max = 0;
    for (int d = 0; d < 3; d++) {
      max = max > size[d] ? max : size[d];
    }
    std::cerr << "Using " << max << " as max dim radius\n";
    return max;
  }
  inline vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType & point) const override {
    return meshWrapper->SampleNormalAtPoint(point);
  }

  inline double Distance(const PointType &a, const PointType &b) const override {
      return meshWrapper->ComputeDistance(a, b);
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

} // end namespace itk

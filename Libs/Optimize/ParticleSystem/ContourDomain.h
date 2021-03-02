/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkParticleDomain.h"
#include <Eigen/Dense>
#include <vtkPolyData.h>

namespace itk
{
class ContourDomain : public ParticleDomain
{
public:
  /** Standard class typedefs */
  typedef SmartPointer<ContourDomain>  Pointer;

  /** Point type used to store particle locations. */
  typedef typename ParticleDomain::PointType PointType;

  explicit ContourDomain() {}
  virtual ~ContourDomain() {}

  void SetPolyLine(vtkSmartPointer<vtkPolyData> poly_data);

  shapeworks::DomainType GetDomainType() const override {
    return shapeworks::DomainType::Contour;
  }

  virtual bool ApplyConstraints(PointType& p, int idx, bool dbg = false) const override;

  virtual PointType UpdateParticlePosition(const PointType &point,
                                           int idx, vnl_vector_fixed<double, DIMENSION> &update) const override;

  inline vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION>& gradE,
                                                                           const PointType &pos, int idx) const override;

  inline vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType &point, int idx) const override {
    throw std::runtime_error("Contours do not have normals");
  }

  virtual vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint(const PointType &point, int idx) const override {
    throw std::runtime_error("Contours do not have gradients");
  }

  virtual GradNType SampleGradNAtPoint(const PointType &p, int idx) const override {
    throw std::runtime_error("Contours do not have gradient of normals");
  }

  virtual PointType GetValidLocationNear(PointType p) const override {
    this->ApplyConstraints(p, -1);
    return p;
  }

  virtual double GetMaxDiameter() const override {
    //todo copied from MeshDomain: should this not be the length of the bounding box diagonal?
    const PointType bb = upper_bound_ - lower_bound_;
    return std::max({bb[0], bb[1], bb[2]});
  }

  virtual void UpdateZeroCrossingPoint() override { }

  double GetCurvature(const PointType &p, int idx) const override {
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

  inline double Distance(const PointType &a, const PointType &b) const override;

  const PointType& GetLowerBound() const override {
    return lower_bound_;
  }

  const PointType& GetUpperBound() const override {
    return upper_bound_;
  }

  PointType GetZeroCrossingPoint() const override {
    const Eigen::Vector3d pt = points.row(0);
    return PointType(pt.data());
  }

  double GetSurfaceArea() const override {
    throw std::runtime_error("Contours do not have area");
  }

  void DeleteImages() override {
    // TODO what?
  }
  void DeletePartialDerivativeImages() override {
    // TODO what?
  }


protected:
  void PrintSelf(std::ostream& os, Indent indent) const override
  {
    DataObject::Superclass::PrintSelf(os, indent);
    os << indent << "ContourDomain\n";
  }

private:
  PointType lower_bound_, upper_bound_;
  //todo maybe just vtkPolyData
  Eigen::MatrixX3d points;
  bool is_closed_;

  void ComputeBounds();

  double DistanceToLine(const Eigen::Vector3d& pt, int line_idx, Eigen::Vector3d& closest_pt) const;

  int ClosestLine(const Eigen::Vector3d& pt, double& closest_distance, Eigen::Vector3d& closest_pt) const;

  int NumberOfLines() const;
};

} // end namespace itk

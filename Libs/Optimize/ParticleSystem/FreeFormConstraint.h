#pragma once

#include <Libs/Mesh/Mesh.h>

#include "Constraint.h"

namespace shapeworks {

//! FreeFormConstraint
/*!
 * Encapsulate functionality related to free form constraints
 */
class FreeFormConstraint : public Constraint {
 public:
  FreeFormConstraint() {}

  void setMesh(std::shared_ptr<shapeworks::Mesh> mesh) { mesh_ = mesh; }

  std::shared_ptr<shapeworks::Mesh> getMesh() { return mesh_; }

  bool isViolated(const Eigen::Vector3d &pt) const override;

  void print() const override { std::cout << "FF" << std::endl; }

  Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const override { return mesh_->getFFCGradient(pt); }

  double constraintEval(const Eigen::Vector3d &pt) const override { return mesh_->getFFCValue(pt); }

  //! Set polydata where per-vertex free form constraint definition exists
  void setDefinition(vtkSmartPointer<vtkPolyData> polyData);

  //! Get polydata where per-vertex free form constraint definition exists
  vtkSmartPointer<vtkPolyData> getDefinition() { return definitionPolyData_; };

  //! Apply the free form constraint to a polydata
  void applyToPolyData(vtkSmartPointer<vtkPolyData> polyData);

  //! Access the set of boundaries
  std::vector<std::vector<Eigen::Vector3d>> &boundaries();

  //! Get query (inside) point
  Eigen::Vector3d getQueryPoint() { return queryPoint_; };

  //! Set query (inside) point
  void setQueryPoint(Eigen::Vector3d queryPoint) { queryPoint_ = queryPoint; };

  //! Compute boundaries from definition polydata with ffc_paint scalars
  void computeBoundaries();

  //! Return if this FFC is set or not
  bool isSet();

  //! Set if this FFC has paint on it or not
  bool setPainted(bool painted);

  //! Reset to initial state
  void reset();

 private:

  void createFFCPaint(vtkSmartPointer<vtkPolyData> polyData);

  std::shared_ptr<shapeworks::Mesh> mesh_;

  vtkSmartPointer<vtkPolyData> definitionPolyData_;
  bool painted_ = false;

  std::vector<std::vector<Eigen::Vector3d>> boundaries_;
  Eigen::Vector3d queryPoint_;
};

}  // namespace shapeworks

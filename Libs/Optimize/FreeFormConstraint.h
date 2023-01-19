#pragma once

#include <Mesh/Mesh.h>

#include "Constraint.h"

class vtkFloatArray;

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

  bool readyForOptimize() const;

  bool isViolated(const Eigen::Vector3d& pt) const override;

  void print() const override { std::cout << "FF" << std::endl; }

  Eigen::Vector3d constraintGradient(const Eigen::Vector3d& pt) const override { return mesh_->getFFCGradient(pt); }

  double constraintEval(const Eigen::Vector3d& pt) const override { return mesh_->getFFCValue(pt); }

  //! Set polydata where per-vertex free form constraint definition exists
  void setDefinition(vtkSmartPointer<vtkPolyData> polyData);

  //! Get polydata where per-vertex free form constraint definition exists
  vtkSmartPointer<vtkPolyData> getDefinition() { return definitionPolyData_; };

  //! Apply the free form constraint to a polydata
  void applyToPolyData(vtkSmartPointer<vtkPolyData> polyData);

  //! @deprecated Access the set of boundaries
  std::vector<std::vector<Eigen::Vector3d>>& boundaries();

  //! @deprecated Get query (inside) point
  Eigen::Vector3d getQueryPoint() { return queryPoint_; };

  //! @deprecated Set query (inside) point
  void setQueryPoint(Eigen::Vector3d queryPoint) { queryPoint_ = queryPoint; };

  //! @deprecated Compute boundaries from definition polydata with ffc_paint scalars
  void computeBoundaries();

  //! Set the in/out polydata
  void setInoutPolyData(vtkSmartPointer<vtkPolyData> polyData) { inoutPolyData_ = polyData; }

  //! Get the in/out polydata
  vtkSmartPointer<vtkPolyData> getInoutPolyData() { return inoutPolyData_; };

  //! Create the in/out poly data from the definition poly data
  void createInoutPolyData();

  //! Return if this FFC is set or not
  bool isSet();

  //! Set if this FFC has paint on it or not
  void setPainted(bool painted);

  //! Reset to initial state
  void reset();

  void computeGradientFields(std::shared_ptr<Mesh> mesh);

 private:

  vtkFloatArray* getInOutScalars();
  vtkFloatArray* createFFCPaint(vtkSmartPointer<vtkPolyData> polyData);

  std::shared_ptr<shapeworks::Mesh> mesh_;

  vtkSmartPointer<vtkPolyData> definitionPolyData_;
  bool painted_ = false;

  std::vector<std::vector<Eigen::Vector3d>> boundaries_;
  Eigen::Vector3d queryPoint_;
  vtkSmartPointer<vtkPolyData> inoutPolyData_;
};

}  // namespace shapeworks

#pragma once

#include "Constraint.h"
#include "Libs/Mesh/Mesh.h"

class vtkFloatArray;

namespace shapeworks {

/**
 * \class FreeFormConstraint
 * \ingroup Group-Constraints
 *
 *  Encapsulate functionality related to free-form constraints (FFC). Inherits from Constraint. See Constraint.h for more information
 *
 */
class FreeFormConstraint : public Constraint {
 public:
  FreeFormConstraint() {}

  /// Sets the mesh that defines the FFC
  void setMesh(std::shared_ptr<shapeworks::Mesh> mesh) { mesh_ = mesh; }

  /// Returns the mesh that defines the FFC
  std::shared_ptr<shapeworks::Mesh> getMesh() { return mesh_; }

  /// Prepares for optimize
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

  //! Compute gradient fields for a mesh
  void computeGradientFields(std::shared_ptr<Mesh> mesh);

  //! Convert legacy FFC to new format
  void convertLegacyFFC(vtkSmartPointer<vtkPolyData> polyData);


 private:

  //! Computes whether point is inside or outside the boundary
  vtkSmartPointer<vtkFloatArray> computeInOutForFFCs(vtkSmartPointer<vtkPolyData> polyData, Eigen::Vector3d query,
                                                     vtkSmartPointer<vtkPolyData> halfmesh);

  //! Computes the gradient vector field for FFCs w.r.t the boundary
  std::vector<Eigen::Matrix3d> setGradientFieldForFFCs(std::shared_ptr<Mesh> mesh,
                                                       vtkSmartPointer<vtkDoubleArray> absvalues, Eigen::MatrixXd V,
                                                       Eigen::MatrixXi F);

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

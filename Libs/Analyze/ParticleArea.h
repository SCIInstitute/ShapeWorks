#pragma once

#include <itkPoint.h>

#include <Eigen/Core>
#include <QColor>

#include "vtkPolyData.h"

namespace shapeworks {

class ParticleArea {
 public:
  //! assign particle ids for each vertex based on closest geodesic distance
  static void assign_vertex_particles(vtkSmartPointer<vtkPolyData> poly_data,
                                      std::vector<itk::Point<double>> particles);

  //! assign vertex colors based on particle ids
  static void assign_vertex_colors(vtkSmartPointer<vtkPolyData> poly_data, std::vector<QColor> colors);

  //! assign vertex areas based on particle ids
  static void assign_vertex_areas(vtkSmartPointer<vtkPolyData> poly_data, Eigen::VectorXd areas);

  //! convert lut to array of colors
  static std::vector<QColor> colors_from_lut(vtkSmartPointer<vtkLookupTable> lut);

  //! compute the area assigned to each particle
  static Eigen::VectorXd compute_particle_triangle_areas(vtkSmartPointer<vtkPolyData> poly_data,
                                                         std::vector<itk::Point<double>> particles);

  static void create_ffc_from_particles(
      const std::string& mesh_path, const Eigen::MatrixXd& particles,
      const std::string& ffc_filename, bool constraints_flag = false, double alpha = 3.0);
};

}  // namespace shapeworks

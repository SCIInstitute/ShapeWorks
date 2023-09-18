#pragma once

#include <QColor>

#include "vtkPolyData.h"
#include <itkPoint.h>

#include <Eigen/Core>

namespace shapeworks {

class ParticleArea {
 public:
  //! assign particle ids for each vertex based on closest geodesic distance
  static void assign_vertex_particles(vtkSmartPointer<vtkPolyData> poly_data,
                                      std::vector<itk::Point<double>> particles);

  //! assign vertex colors based on particle ids
  static void assign_vertex_colors(vtkSmartPointer<vtkPolyData> poly_data, std::vector<QColor> colors);

  //! convert lut to array of colors
  static std::vector<QColor> colors_from_lut(vtkSmartPointer<vtkLookupTable> lut);

  //! compute the area assigned to each particle
  static Eigen::VectorXd compute_particle_areas(vtkSmartPointer<vtkPolyData> poly_data, std::vector<itk::Point<double>> particles);

  //! compute the area assigned to each particle
  static Eigen::VectorXd compute_particle_triangle_areas(vtkSmartPointer<vtkPolyData> poly_data, std::vector<itk::Point<double>> particles);

};

}  // namespace shapeworks

#pragma once

#include <Project/Project.h>
class QWidget;

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <QImage>
#include <QStringList>

class vtkImageData;
class vtkRenderer;
class QComboBox;

namespace shapeworks {

class Session;

//! Utilities for Studio
class StudioUtils {
 public:
  static bool ask_multiple_domains_as_single(QWidget* parent, std::shared_ptr<Project> project);

  //! convert a vector of std::string to QStringList
  static QStringList to_string_list(std::vector<std::string> vector);

  //! convert vtkImageData to QImage
  static QImage vtk_image_to_qimage(vtkSmartPointer<vtkImageData> image_data);

  //! reverse a poly data
  static vtkSmartPointer<vtkPolyData> reverse_poly_data(vtkSmartPointer<vtkPolyData> poly_data);

  //! return platform string
  static QString get_platform_string();

  //! add a color border to a viewport
  static void add_viewport_border(vtkRenderer* renderer, double* color, double line_width = 6.0);

  //! write a mesh to file
  static bool write_mesh(vtkSmartPointer<vtkPolyData> poly_data, QString filename);

  //! convert brightness and contrast to window width and level
  static void brightness_contrast_to_window_width_level(double brightness, double contrast, double min_intensity,
                                                        double max_intensity, double& window_width,
                                                        double& window_level);

  //! convert window width and level to brightness and contrast
  static void window_width_level_to_brightness_contrast(double window_width, double window_level, double min_intensity,
                                                        double max_intensity, double& brightness, double& contrast);

  //! update a combobox with domain names
  static void update_domain_combobox(QComboBox* combobox, QSharedPointer<Session> session);
};

}  // namespace shapeworks

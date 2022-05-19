#pragma once

#include <Libs/Project/Project.h>
class QWidget;

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <QImage>
#include <QStringList>

class vtkImageData;

namespace shapeworks {

//! Utilities for Studio
class StudioUtils {
 public:
  static bool ask_multiple_domains_as_single(QWidget *parent, std::shared_ptr<Project> project);

  //! convert a vector of std::string to QStringList
  static QStringList to_string_list(std::vector<std::string> vector);

  //! convert vtkImageData to QImage
  static QImage vtk_image_to_qimage(vtkSmartPointer<vtkImageData> image_data);

  //! reverse a poly data
  static vtkSmartPointer<vtkPolyData> reverse_poly_data(vtkSmartPointer<vtkPolyData> poly_data);
};

}  // namespace shapeworks

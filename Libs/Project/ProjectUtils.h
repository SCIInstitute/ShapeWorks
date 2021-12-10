#pragma once

#include <vtkTransform.h>
#include <vtkSmartPointer.h>

#include <vector>

namespace shapeworks {

class ProjectUtils {
public:

  // convert a list of doubles from a spreadsheet to a vtkTransform
  static vtkSmartPointer<vtkTransform> convert_transform(std::vector<double> list);

  // convert a vtkTransform to a list of doubles (e.g. for project spreadsheet)
  static std::vector<double> convert_transform(vtkSmartPointer<vtkTransform> transform);
};
}

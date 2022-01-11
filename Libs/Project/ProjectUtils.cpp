#include "ProjectUtils.h"

namespace shapeworks {

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> shapeworks::ProjectUtils::convert_transform(std::vector<double> list)
{
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Identity();
  if (list.size() == 12) {
    double tx = list[9];
    double ty = list[10];
    double tz = list[11];
    transform->Translate(tx, ty, tz);
  }
  else if (list.size() == 16) {
    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    for (int i = 0; i < 16; i++) {
      matrix->GetData()[i] = list[i];
    }
    transform->SetMatrix(matrix);
  }
  return transform;
}

//---------------------------------------------------------------------------
std::vector<double> ProjectUtils::convert_transform(vtkSmartPointer<vtkTransform> transform)
{
  std::vector<double> list;
  for (int i = 0; i < 16; i++) {
    list.push_back(transform->GetMatrix()->GetData()[i]);
  }
  return list;
}
}

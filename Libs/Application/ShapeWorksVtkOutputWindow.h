#pragma once

#include <vtkOutputWindow.h>

#include <QObject>

namespace shapeworks {

//! Implementation of vtkOutputWindow to capture and display VTK error messages
class ShapeWorksVtkOutputWindow : public QObject, public vtkOutputWindow {
  Q_OBJECT;

 public:
  static ShapeWorksVtkOutputWindow* New();

  vtkTypeMacro(ShapeWorksVtkOutputWindow, vtkOutputWindow);

  ShapeWorksVtkOutputWindow();

  void DisplayErrorText(const char* text) override;
  void DisplayWarningText(const char* text) override;
  void DisplayGenericWarningText(const char* text) override;
  void DisplayDebugText(const char* text) override;
 private:
};
}  // namespace shapeworks

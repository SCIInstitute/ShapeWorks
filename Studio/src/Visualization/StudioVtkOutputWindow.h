#pragma once

#include <vtkOutputWindow.h>

#include <QObject>

namespace shapeworks {

//! Implementation of vtkOutputWindow to capture and display VTK error messages
class StudioVtkOutputWindow : public QObject, public vtkOutputWindow {
  Q_OBJECT;

 public:
  static StudioVtkOutputWindow* New();

  vtkTypeMacro(StudioVtkOutputWindow, vtkOutputWindow);

  StudioVtkOutputWindow();

  void DisplayErrorText(const char* text) override;
  void DisplayWarningText(const char* text) override;

 private:
};
}  // namespace shapeworks

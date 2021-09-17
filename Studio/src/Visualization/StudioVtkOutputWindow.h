#pragma once

#include <QObject>
#include <vtkOutputWindow.h>

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

Q_SIGNALS:
  void warning(QString message);
  void error(QString message);

private:

};
}
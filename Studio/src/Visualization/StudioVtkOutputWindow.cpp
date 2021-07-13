#include "StudioVtkOutputWindow.h"

#include <iostream>
#include <vtkObjectFactory.h>

namespace shapeworks {

vtkStandardNewMacro(StudioVtkOutputWindow);

//---------------------------------------------------------------------------
StudioVtkOutputWindow::StudioVtkOutputWindow()
{
}

//---------------------------------------------------------------------------
void StudioVtkOutputWindow::DisplayErrorText(const char* text)
{
  std::cerr << text;
  emit error(text);
}

//---------------------------------------------------------------------------
void StudioVtkOutputWindow::DisplayWarningText(const char* text)
{
  std::cerr << text;
  emit warning(text);
}

}




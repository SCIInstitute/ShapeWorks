#include "StudioVtkOutputWindow.h"

#include <Logging.h>
#include <vtkObjectFactory.h>

namespace shapeworks {

vtkStandardNewMacro(StudioVtkOutputWindow);

//---------------------------------------------------------------------------
StudioVtkOutputWindow::StudioVtkOutputWindow() {}

//---------------------------------------------------------------------------
void StudioVtkOutputWindow::DisplayErrorText(const char* text) { SW_LOG_ERROR(text); }

//---------------------------------------------------------------------------
void StudioVtkOutputWindow::DisplayWarningText(const char* text) { SW_LOG_WARNING(text); }

}  // namespace shapeworks

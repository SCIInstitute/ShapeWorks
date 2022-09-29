#include "StudioVtkOutputWindow.h"

#include <Logging.h>
#include <vtkObjectFactory.h>

namespace shapeworks {

vtkStandardNewMacro(StudioVtkOutputWindow);

//---------------------------------------------------------------------------
StudioVtkOutputWindow::StudioVtkOutputWindow() {}

//---------------------------------------------------------------------------
void StudioVtkOutputWindow::DisplayErrorText(const char* text) { SW_ERROR(text); }

//---------------------------------------------------------------------------
void StudioVtkOutputWindow::DisplayWarningText(const char* text) { SW_WARN(text); }

//---------------------------------------------------------------------------
void StudioVtkOutputWindow::DisplayGenericWarningText(const char* text) { SW_WARN(text); }

//---------------------------------------------------------------------------
void StudioVtkOutputWindow::DisplayDebugText(const char* text) { SW_DEBUG(text); }

//---------------------------------------------------------------------------

}  // namespace shapeworks

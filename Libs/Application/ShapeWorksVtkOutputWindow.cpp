#include "ShapeWorksVtkOutputWindow.h"

#include <Logging.h>
#include <vtkObjectFactory.h>

namespace shapeworks {

vtkStandardNewMacro(ShapeWorksVtkOutputWindow);

//---------------------------------------------------------------------------
ShapeWorksVtkOutputWindow::ShapeWorksVtkOutputWindow() {}

//---------------------------------------------------------------------------
void ShapeWorksVtkOutputWindow::DisplayErrorText(const char* text) { SW_ERROR(text); }

//---------------------------------------------------------------------------
void ShapeWorksVtkOutputWindow::DisplayWarningText(const char* text) { SW_WARN(text); }

//---------------------------------------------------------------------------
void ShapeWorksVtkOutputWindow::DisplayGenericWarningText(const char* text) { SW_WARN(text); }

//---------------------------------------------------------------------------
void ShapeWorksVtkOutputWindow::DisplayDebugText(const char* text) { SW_DEBUG(text); }

//---------------------------------------------------------------------------

}  // namespace shapeworks

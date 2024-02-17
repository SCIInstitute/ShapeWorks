#include <Utils/StudioUtils.h>
#include <Visualization/Lightbox.h>
#include <Visualization/StudioSliceInteractorStyle.h>
#include <vtkAssemblyPath.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkImageProperty.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
vtkStandardNewMacro(StudioSliceInteractorStyle);

//-----------------------------------------------------------------------------
StudioSliceInteractorStyle::StudioSliceInteractorStyle() {}

//-----------------------------------------------------------------------------
StudioSliceInteractorStyle::~StudioSliceInteractorStyle() {}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnLeftButtonDown() {
  // forward events
  vtkInteractorStyleImage::OnLeftButtonDown();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnRightButtonDown() {
  // forward events
  vtkInteractorStyleImage::OnRightButtonDown();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnKeyDown() {
  int* click_pos = this->GetInteractor()->GetEventPosition();

  // char keycode = this->GetInteractor()->GetKeyCode();
  std::string keysym = GetInteractor()->GetKeySym();
  if (keysym == "Up" | keysym == "Down") {
    lightbox_->handle_key(click_pos, keysym);
  }

  this->GetInteractor()->Render();

  // forward events
  vtkInteractorStyleTrackballCamera::OnKeyDown();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::WindowLevel() {
  if (!CurrentImageProperty) return;
  if (!current_image_slice_) return;

  // Extract window width and level from the current image property
  vtkSmartPointer<vtkImageProperty> prop = CurrentImageProperty;
  double windowWidth = prop->GetColorWindow();
  double windowCenter = prop->GetColorLevel();

  double contrast;
  double brightness;

  // get intensity range of the image, using current_image_actor_ which is a vtkImageSlice
  double range[2];
  vtkImageData* imageData = current_image_slice_->GetMapper()->GetInput();
  imageData->GetScalarRange(range);

  StudioUtils::window_width_level_to_brightness_contrast(windowWidth, windowCenter, range[0], range[1], brightness, contrast);

  // Get the interactor
  vtkRenderWindowInteractor* rwi = this->Interactor;
  if (!rwi) return;

  // Get the renderer size
  int* rendererSize = this->CurrentRenderer->GetSize();

  // Get the event position
  int eventPos[2];
  rwi->GetEventPosition(eventPos);

  // Get the last event position
  int lastEventPos[2];
  rwi->GetLastEventPosition(lastEventPos);

  // Calculate change in mouse position
  double dx = static_cast<double>(eventPos[0] - lastEventPos[0]) / static_cast<double>(rendererSize[0]) * 100.0;
  double dy = static_cast<double>(eventPos[1] - lastEventPos[1]) / static_cast<double>(rendererSize[1]) * 100.0;

  // Adjust brightness and contrast based on mouse movement
  brightness += dx;
  contrast += dy;

  // Ensure brightness and contrast stay within valid range
  brightness = std::max<double>(3, std::min<double>(100, brightness));
  contrast = std::max<double>(3, std::min<double>(97, contrast));

  StudioUtils::brightness_contrast_to_window_width_level(brightness, contrast, range[0], range[1], windowWidth, windowCenter);

  lightbox_->set_shared_brightness_and_contrast(brightness, contrast);

  // Apply window width and level to the image actor's property
  prop->SetColorWindow(windowWidth);
  prop->SetColorLevel(windowCenter);
  this->Interactor->Render();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::StartWindowLevel() {
  // forward events
  vtkInteractorStyleImage::StartWindowLevel();

  if (!this->CurrentRenderer) {
    return;
  }

  vtkPropCollection* props = this->CurrentRenderer->GetViewProps();
  vtkProp* prop = nullptr;
  vtkAssemblyPath* path;
  vtkImageSlice* imageProp = nullptr;
  vtkCollectionSimpleIterator pit;

  int i = -1;

  for (int k = 0; k < 2; k++) {
    int j = 0;
    for (props->InitTraversal(pit); (prop = props->GetNextProp(pit));) {
      bool foundImageProp = false;
      for (prop->InitPathTraversal(); (path = prop->GetNextPath());) {
        vtkProp* tryProp = path->GetLastNode()->GetViewProp();
        imageProp = vtkImageSlice::SafeDownCast(tryProp);
        if (imageProp) {
          if (j == i && imageProp->GetPickable()) {
            foundImageProp = true;
            break;
          }
          imageProp = nullptr;
          j++;
        }
      }
      if (foundImageProp) {
        break;
      }
    }
    if (i < 0) {
      i += j;
    }
  }

  if (imageProp) {
    current_image_slice_ = imageProp;
  }
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::set_lightbox(Lightbox* lightbox) { this->lightbox_ = lightbox; }

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnMouseWheelForward() {
  // do nothing so that it will be passed on to Qt
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnMouseWheelBackward() {
  // do nothing so that it will be passed on to Qt
}

}  // namespace shapeworks

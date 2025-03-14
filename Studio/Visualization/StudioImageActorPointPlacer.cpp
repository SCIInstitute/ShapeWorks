#include "StudioImageActorPointPlacer.h"

#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageMapper3D.h"
#include "vtkMath.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"

vtkStandardNewMacro(StudioImageActorPointPlacer);

//----------------------------------------------------------------------------
StudioImageActorPointPlacer::StudioImageActorPointPlacer() {}

//----------------------------------------------------------------------------
StudioImageActorPointPlacer::~StudioImageActorPointPlacer() {}

//----------------------------------------------------------------------------
void StudioImageActorPointPlacer::PrintSelf(ostream& os, vtkIndent indent) { this->Superclass::PrintSelf(os, indent); }

//----------------------------------------------------------------------------
int StudioImageActorPointPlacer::ComputeWorldPosition(vtkRenderer* renderer, double displayPos[2], double worldPos[3],
                                                      double worldOrient[9]) {
  vtkImageActor* actor = this->GetImageActor();
  if (!actor) {
    return 0;
  }

  // Use a cell picker for accurate picking on the transformed image
  vtkNew<vtkCellPicker> picker;
  picker->SetTolerance(0.005);

  // Pick only on our specific actor
  picker->InitializePickList();
  picker->AddPickList(actor);
  picker->PickFromListOn();

  // Perform the pick
  if (!picker->Pick(displayPos[0], displayPos[1], 0.0, renderer)) {
    return 0;
  }

  // Check if we picked the correct actor
  vtkImageActor* pickedActor = vtkImageActor::SafeDownCast(picker->GetProp3D());
  if (pickedActor != actor) {
    return 0;
  }

  // Get the picked position - this accounts for the transformation
  picker->GetPickPosition(worldPos);

  // If orientation is requested, compute it using the transformed plane
  if (worldOrient) {
    // Get the normal from the picker - this will be in transformed space
    double* normal = picker->GetPickNormal();

    // Z axis is the normal to the image plane
    worldOrient[6] = normal[0];
    worldOrient[7] = normal[1];
    worldOrient[8] = normal[2];

    // Compute a suitable X axis that's perpendicular to Z
    double xAxis[3] = {1.0, 0.0, 0.0};
    if (fabs(normal[0]) > 0.99) {
      xAxis[0] = 0.0;
      xAxis[1] = 1.0;
    }

    // Make X perpendicular to Z
    double dot = vtkMath::Dot(xAxis, normal);
    xAxis[0] -= dot * normal[0];
    xAxis[1] -= dot * normal[1];
    xAxis[2] -= dot * normal[2];
    vtkMath::Normalize(xAxis);

    worldOrient[0] = xAxis[0];
    worldOrient[1] = xAxis[1];
    worldOrient[2] = xAxis[2];

    // Y axis is Z cross X
    double yAxis[3];
    vtkMath::Cross(normal, xAxis, yAxis);
    worldOrient[3] = yAxis[0];
    worldOrient[4] = yAxis[1];
    worldOrient[5] = yAxis[2];
  }

  return 1;
}

//----------------------------------------------------------------------------
int StudioImageActorPointPlacer::ValidateWorldPosition(double worldPos[3]) {
  vtkImageActor* actor = this->GetImageActor();
  if (!actor) {
    return 0;
  }

  // Create a cell locator for the actor's mapper input
  vtkImageMapper3D* mapper = actor->GetMapper();
  if (!mapper) {
    return 0;
  }

  vtkImageData* image = vtkImageData::SafeDownCast(mapper->GetInput());
  if (!image) {
    return 0;
  }

  // Transform the world position back to image space
  double imagePos[3];
  if (actor->GetUserTransform()) {
    vtkNew<vtkTransform> inverseTransform;
    inverseTransform->DeepCopy(actor->GetUserTransform());
    inverseTransform->Inverse();
    inverseTransform->TransformPoint(worldPos, imagePos);
  } else {
    imagePos[0] = worldPos[0];
    imagePos[1] = worldPos[1];
    imagePos[2] = worldPos[2];
  }

  // Check if the point is within the image bounds
  double bounds[6];
  image->GetBounds(bounds);

  if (imagePos[0] < bounds[0] || imagePos[0] > bounds[1] || imagePos[1] < bounds[2] || imagePos[1] > bounds[3] ||
      imagePos[2] < bounds[4] || imagePos[2] > bounds[5]) {
    return 0;
  }

  return 1;
}

//----------------------------------------------------------------------------
int StudioImageActorPointPlacer::UpdateWorldPosition(vtkRenderer* renderer, double worldPos[3], double worldOrient[9]) {
  // For a point already on the plane, we need to ensure it stays within bounds
  if (!this->ValidateWorldPosition(worldPos)) {
    return 0;
  }

  // Orientation handling
  if (worldOrient) {
    vtkImageActor* actor = this->GetImageActor();
    if (actor && actor->GetUserTransform()) {
      // Use the normal from the transformed image plane for the Z axis
      vtkNew<vtkCellPicker> picker;
      picker->SetTolerance(0.005);

      // Pick at the world position
      double displayPos[3];
      renderer->SetWorldPoint(worldPos[0], worldPos[1], worldPos[2], 1.0);
      renderer->WorldToDisplay();
      renderer->GetDisplayPoint(displayPos);

      // Try to pick on the actor
      picker->InitializePickList();
      picker->AddPickList(actor);
      picker->PickFromListOn();

      if (picker->Pick(displayPos[0], displayPos[1], 0.0, renderer)) {
        vtkImageActor* pickedActor = vtkImageActor::SafeDownCast(picker->GetProp3D());
        if (pickedActor == actor) {
          // Use the picked normal
          double* normal = picker->GetPickNormal();

          worldOrient[6] = normal[0];
          worldOrient[7] = normal[1];
          worldOrient[8] = normal[2];

          // Compute suitable X and Y axes as in ComputeWorldPosition
          double xAxis[3] = {1.0, 0.0, 0.0};
          if (fabs(normal[0]) > 0.99) {
            xAxis[0] = 0.0;
            xAxis[1] = 1.0;
          }

          double dot = vtkMath::Dot(xAxis, normal);
          xAxis[0] -= dot * normal[0];
          xAxis[1] -= dot * normal[1];
          xAxis[2] -= dot * normal[2];
          vtkMath::Normalize(xAxis);

          worldOrient[0] = xAxis[0];
          worldOrient[1] = xAxis[1];
          worldOrient[2] = xAxis[2];

          double yAxis[3];
          vtkMath::Cross(normal, xAxis, yAxis);
          worldOrient[3] = yAxis[0];
          worldOrient[4] = yAxis[1];
          worldOrient[5] = yAxis[2];
        }
      }
    }
  }

  return 1;
}

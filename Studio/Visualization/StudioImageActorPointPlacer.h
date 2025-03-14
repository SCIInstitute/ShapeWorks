#ifndef StudioImageActorPointPlacer_h
#define StudioImageActorPointPlacer_h

#include "vtkImageActorPointPlacer.h"

class StudioImageActorPointPlacer : public vtkImageActorPointPlacer
{
public:
  static StudioImageActorPointPlacer* New();
  vtkTypeMacro(StudioImageActorPointPlacer, vtkImageActorPointPlacer);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // Override ComputeWorldPosition to take transform into account
  int ComputeWorldPosition(vtkRenderer *renderer,
                           double displayPos[2],
                           double worldPos[3],
                           double worldOrient[9]) override;

  // Override ValidateWorldPosition
  int ValidateWorldPosition(double worldPos[3]) override;
  
  // Override UpdateWorldPosition
  int UpdateWorldPosition(vtkRenderer *renderer,
                         double worldPos[3],
                         double worldOrient[9]) override;

/*
  // Override UpdateDisplayPosition
  int UpdateDisplayPosition(vtkRenderer *renderer,
                           double displayPos[2],
                           double worldPos[3]) override;
*/
protected:
  StudioImageActorPointPlacer();
  ~StudioImageActorPointPlacer() override;

private:
  StudioImageActorPointPlacer(const StudioImageActorPointPlacer&) = delete;
  void operator=(const StudioImageActorPointPlacer&) = delete;
};

#endif

#pragma once

#include "vtkAbstractWidget.h"

class vtkContourRepresentation;
class vtkPolyData;
class vtkPointPlacer;
class vtkRenderer;

namespace shapeworks {

class Viewer;
class StudioSphereRepresentation;

class PaintWidget : public vtkAbstractWidget {
 public:
  // Description:
  // Instantiate this class.
  static PaintWidget* New();

  // Description:
  // Standard methods for a VTK class.
  vtkTypeMacro(PaintWidget, vtkAbstractWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set / get the Point Placer. The point placer is
  // responsible for converting display coordinates into
  // world coordinates according to some constraints, and
  // for validating world positions.
  // void SetPointPlacer( vtkPointPlacer * );
  vtkSetMacro(PointPlacer, vtkPointPlacer*);
  vtkGetMacro(PointPlacer, vtkPointPlacer*);

  vtkSetMacro(Renderer, vtkRenderer*);
  vtkGetMacro(Renderer, vtkRenderer*);

  void set_viewer(Viewer* viewer);

  // Description:
  // The method for activiating and deactiviating this widget. This method
  // must be overridden because it is a composite widget and does more than
  // its superclasses' vtkAbstractWidget::SetEnabled() method.
  virtual void SetEnabled(int);

  // Description:
  // Specify an instance of vtkWidgetRepresentation used to represent this
  // widget in the scene. Note that the representation is a subclass of vtkProp
  // so it can be added to the renderer independent of the widget.
  void SetRepresentation(vtkContourRepresentation* r) {
    this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));
  }

  // Description:
  // Create the default widget representation if one is not set.
  void CreateDefaultRepresentation();

  void update_position();

  // Description:
  // Initialize the contour widget from a user supplied set of points. The
  // state of the widget decides if you are still defining the widget, or
  // if you've finished defining (added the last point) are manipulating
  // it. Note that if the polydata supplied is closed, the state will be
  // set to manipulate.
  //  State: Define = 0, Manipulate = 1.
  virtual void Initialize(vtkPolyData* poly, int state = 1);
  virtual void Initialize() { this->Initialize(NULL); }

  void set_brush_size(float size);
  void set_brush_color(float r, float g, float b);

 protected:
  PaintWidget();
  ~PaintWidget();

  // The state of the widget
  // BTX
  enum { Start, Paint, Erase };
  // ETX

  int WidgetState;
  vtkPointPlacer* PointPlacer;
  vtkRenderer* Renderer;

  // Callback interface to capture events when
  // placing the widget.
  static void StartPaintAction(vtkAbstractWidget* w);
  static void EndPaintAction(vtkAbstractWidget* w);
  static void StartEraseAction(vtkAbstractWidget* w);
  static void EndEraseAction(vtkAbstractWidget* w);
  static void MoveAction(vtkAbstractWidget* w);
  static void LeaveAction(vtkAbstractWidget* w);
  static void KeyPressAction(vtkAbstractWidget* w);

  bool use_point_placer(double displayPos[2], int newState);
  void set_cursor(int requestedShape);

  StudioSphereRepresentation* sphere_cursor_;

  bool mouse_in_window_;

  Viewer* viewer_;

 private:
  PaintWidget(const PaintWidget&);     // Not implemented
  void operator=(const PaintWidget&);  // Not implemented
};
}  // namespace shapeworks

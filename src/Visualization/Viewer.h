#ifndef STUDIO_VISUALIZATION_VIEWER_H
#define STUDIO_VISUALIZATION_VIEWER_H

#include <QSharedPointer>

class vtkRenderer;
class vtkLookupTable;
class CustomImagePlaneWidget;
class vtkRenderWindowInteractor;
class vtkImageData;
class vtkCamera;
class vtkGlyph3D;

class Shape;

class Viewer
{

public:

  Viewer();
  ~Viewer();

  void set_renderer( vtkSmartPointer<vtkRenderer> renderer );
  vtkSmartPointer<vtkRenderer> get_renderer();

  void display_shape( QSharedPointer<Shape> shape, QString mode, bool auto_center );

  void reset_camera();

private:

  vtkSmartPointer<vtkRenderer>             renderer_;

  vtkSmartPointer<vtkPoints>               glyph_points_;
  vtkSmartPointer<vtkPolyData>             glyph_point_set_;
  vtkSmartPointer<vtkGlyph3D>              glyphs_;
  vtkSmartPointer<vtkPolyDataMapper>       glyph_mapper_;
  vtkSmartPointer<vtkActor>                glyph_actor;

  vtkSmartPointer<vtkPolyDataMapper>       surface_mapper_;
  vtkSmartPointer<vtkActor>                surface_actor_;

  vtkSmartPointer<vtkLookupTable> lut_;
};

#endif /* STUDIO_VISUALIZATION_VIEWER_H */

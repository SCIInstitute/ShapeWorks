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
class vtkSphereSource;

class DisplayObject;

class Viewer;

class StudioInteractorStyle;

typedef QSharedPointer< Viewer > ViewerHandle;
typedef QVector< ViewerHandle > ViewerList;

//! 3D Viewer
/*!
 * The Viewer class encapsulates all the functionality for visualizing a single DisplayObject
 *
 */
class Viewer
{

public:

  Viewer();
  ~Viewer();

  void set_renderer( vtkSmartPointer<vtkRenderer> renderer );
  vtkSmartPointer<vtkRenderer> get_renderer();

  void display_object( QSharedPointer<DisplayObject> object );

  void clear_viewer();

  void reset_camera();

  void set_glyph_size_and_quality( double size, double quality );
  void set_show_glyphs( bool show );
  void set_show_surface( bool show );

  void update_glyph_properties();

  void handle_pick( int* click_pos );

private:

  bool visible_;

  void update_actors();

  bool show_glyphs_;
  bool show_surface_;

  double glyph_size_;
  double glyph_quality_;

  vtkSmartPointer<vtkRenderer>             renderer_;

  vtkSmartPointer<vtkSphereSource>         sphere_source;
  vtkSmartPointer<vtkPoints>               glyph_points_;
  vtkSmartPointer<vtkPolyData>             glyph_point_set_;
  vtkSmartPointer<vtkGlyph3D>              glyphs_;
  vtkSmartPointer<vtkPolyDataMapper>       glyph_mapper_;
  vtkSmartPointer<vtkActor>                glyph_actor_;

  vtkSmartPointer<vtkPolyDataMapper>       surface_mapper_;
  vtkSmartPointer<vtkActor>                surface_actor_;

  vtkSmartPointer<vtkLookupTable>          lut_;

  vtkSmartPointer<StudioInteractorStyle>   style_;
};

#endif /* STUDIO_VISUALIZATION_VIEWER_H */


#include <vtkSmartPointer.h>

#include <vector>

class vtkHandleWidget;
class vtkSphereSource;
class vtkPolyDataMapper;
class vtkPoints;
class vtkPolyData;
class vtkGlyph3D;
class vtkActor;
class vtkLookupTable;

namespace shapeworks {

class Viewer;
class LandmarkCallback;

//! LandmarkWidget
/*!
 * Widget to display and manipulate landmarks
 *
 */
class LandmarkWidget {
 public:
  LandmarkWidget(Viewer* viewer);
  ~LandmarkWidget();

  void update_landmarks();

  void store_positions();

  void update_glyph_properties();

  void clear_landmark_handles();

 private:
  vtkSmartPointer<vtkHandleWidget> create_handle();

  bool is_drag_mode();

  bool is_glyph_mode();

  void update_glyphs();

  void assign_handle_to_domain(vtkSmartPointer<vtkHandleWidget> handle, int domain_id);

  Viewer* viewer_ = nullptr;

  std::vector<vtkSmartPointer<vtkHandleWidget>> handles_;

  vtkSmartPointer<vtkSphereSource> sphere_;
  vtkSmartPointer<LandmarkCallback> callback_;

  vtkSmartPointer<vtkPoints> glyph_points_;
  vtkSmartPointer<vtkPolyData> glyph_point_set_;
  vtkSmartPointer<vtkGlyph3D> glyphs_;
  vtkSmartPointer<vtkPolyDataMapper> glyph_mapper_;
  vtkSmartPointer<vtkActor> glyph_actor_;
  vtkSmartPointer<vtkLookupTable> glyph_lut_;
};

}  // namespace shapeworks

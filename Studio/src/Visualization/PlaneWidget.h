
#include <vtkSmartPointer.h>

#include <vector>

class vtkHandleWidget;
class vtkSphereSource;
class vtkPlaneSource;
class vtkPolyDataMapper;
class vtkActor;

namespace shapeworks {

class Viewer;
class PlaneCallback;
class StudioHandleWidget;

//! PlaneWidget
/*!
 * Widget to display and manipulate landmarks
 *
 */
class PlaneWidget {
 public:
  PlaneWidget(Viewer* viewer);
  ~PlaneWidget();

  void update_plane_points();
  void update_planes();
  void update();

  void store_positions();

  void update_glyph_properties();

  void clear_planes();

  void handle_right_click(int domain, int plane, int point);

  void delete_plane(int domain, int plane);

 private:
  bool block_update_ = false;

  vtkSmartPointer<StudioHandleWidget> create_handle();

  void assign_handle_to_domain(vtkSmartPointer<StudioHandleWidget> handle, int domain_id);

  int count_plane_points();
  int count_complete_planes();

  Viewer* viewer_ = nullptr;

  // control points
  vtkSmartPointer<vtkSphereSource> sphere_;
  std::vector<vtkSmartPointer<StudioHandleWidget>> handles_;

  // planes
  std::vector<vtkSmartPointer<vtkPlaneSource>> plane_sources_;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> plane_mappers_;
  std::vector<vtkSmartPointer<vtkActor>> plane_actors_;

  vtkSmartPointer<PlaneCallback> callback_;
};

}  // namespace shapeworks

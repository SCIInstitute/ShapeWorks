
#include <Libs/Optimize/ParticleSystem/PlaneConstraint.h>
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
 * Widget to display and manipulate constraint planes
 *
 */
class PlaneWidget {
 public:
  PlaneWidget(Viewer* viewer);
  ~PlaneWidget();

  //! Update the widget from shape data
  void update();

  //! Store positions back to shape data (this is called by callback)
  void store_positions();

  //! Update the sizes and detail of control spheres
  void update_glyph_properties();

  //! Clear point handles and planes
  void clear_planes();

  //! Handle a right click on a particular point
  void handle_right_click(int domain, int plane, int point);

  //! Delete a particular plane
  void delete_plane(int domain, int plane_id);

  //! Flip the normal for a given plane
  void flip_plane(int domain, int plane_id);

  //! Set an offset for a given plane
  void set_plane_offset(int domain, int plane_id, int offset);

  //! Finalize the offset for a given plane
  void finalize_plane_offset(int domain, int plane_id);

 private:
  //! update the point handles
  void update_plane_points();
  //! update the plane display actors
  void update_planes();

  vtkSmartPointer<StudioHandleWidget> create_handle();

  void assign_handle_to_domain(vtkSmartPointer<StudioHandleWidget> handle, int domain_id);

  int count_plane_points();
  int count_complete_planes();

  PlaneConstraint& get_plane_reference(int domain, int plane);

  double get_offset_scale(int domain_id);

  bool block_update_ = false;

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

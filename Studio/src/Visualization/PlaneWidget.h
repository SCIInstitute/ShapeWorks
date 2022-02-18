
#include <vtkSmartPointer.h>

#include <vector>

class vtkHandleWidget;
class vtkSphereSource;

namespace shapeworks {

class Viewer;
class PlaneCallback;

//! PlaneWidget
/*!
 * Widget to display and manipulate landmarks
 *
 */
class PlaneWidget {
 public:
  PlaneWidget(Viewer* viewer);
  ~PlaneWidget();

  void update_planes();

  void store_positions();

  void update_glyph_properties();

  void clear_planes();

 private:
  vtkSmartPointer<vtkHandleWidget> create_handle();

  void assign_handle_to_domain(vtkSmartPointer<vtkHandleWidget> handle, int domain_id);

  Viewer* viewer_ = nullptr;

  std::vector<vtkSmartPointer<vtkHandleWidget>> handles_;

  vtkSmartPointer<vtkSphereSource> sphere_;
  vtkSmartPointer<PlaneCallback> callback_;
};

}  // namespace shapeworks

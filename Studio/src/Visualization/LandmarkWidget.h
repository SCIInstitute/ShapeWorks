
#include <vtkSmartPointer.h>

#include <vector>

class vtkHandleWidget;
class vtkSphereSource;

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

  void clear_landmarks();

 private:
  vtkSmartPointer<vtkHandleWidget> create_handle();

  void assign_handle_to_domain(vtkSmartPointer<vtkHandleWidget> handle, int domain_id);

  Viewer* viewer_ = nullptr;

  std::vector<vtkSmartPointer<vtkHandleWidget>> handles_;

  vtkSmartPointer<vtkSphereSource> sphere_;
  vtkSmartPointer<LandmarkCallback> callback_;
};

}  // namespace shapeworks

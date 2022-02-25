#include <vtkHandleWidget.h>

namespace shapeworks {
class PlaneWidget;

class StudioHandleWidget : public vtkHandleWidget {
 public:
  static StudioHandleWidget *New();

  StudioHandleWidget();

  void set_domain(int domain) { domain_ = domain; };
  void set_plane(int plane) { plane_ = plane; };
  void set_point(int point) { point_ = point; };

  void set_plane_widget(PlaneWidget *plane_widget) { plane_widget_ = plane_widget; }

 protected:
  static void RightClickSelectAction(vtkAbstractWidget *);

  int domain_ = -1;
  int plane_ = -1;
  int point_ = -1;
  PlaneWidget *plane_widget_;
};
}  // namespace shapeworks

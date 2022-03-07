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
  static void RightClickSelectAction(vtkAbstractWidget *w);

  static void CustomMoveAction(vtkAbstractWidget *w);
  static void CustomSelectAction(vtkAbstractWidget *w);
  static void CustomEndSelectAction(vtkAbstractWidget *w);

  int domain_ = -1;
  int plane_ = -1;
  int point_ = -1;
  PlaneWidget *plane_widget_;

  bool shift_active_ = false;
  int start_y_ = 0;
};
}  // namespace shapeworks

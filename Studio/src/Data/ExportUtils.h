
#include <vtkSmartPointer.h>

#include <QSharedPointer>
#include <QWidget>

class vtkPolyData;
namespace shapeworks {
class Session;
class ShapeWorksStudioApp;
class Shape;

//! Export utilities for Studio
class ExportUtils {
 public:
  static QString get_save_filename(ShapeWorksStudioApp* parent, QString title, QString filetypes, QString default_ext);

  static void export_all_subjects_particle_scalars(ShapeWorksStudioApp* parent, QSharedPointer<Session> session);

  static bool write_scalars(ShapeWorksStudioApp* app, vtkSmartPointer<vtkPolyData> poly_data, QString filename);

  static bool write_particle_scalars(ShapeWorksStudioApp* app, std::shared_ptr<Shape> shape, QString filename);
};

}  // namespace shapeworks

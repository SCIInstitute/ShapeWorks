#include "ExportUtils.h"

#include <StringUtils.h>
#include <Data/Session.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Utils/StudioUtils.h>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QProgressDialog>

#include <vtkPolyData.h>
#include <vtkPointData.h>

namespace shapeworks {

//---------------------------------------------------------------------------
QString ExportUtils::get_save_filename(ShapeWorksStudioApp* parent, QString title, QString filetypes,
                                       QString default_ext) {
  QString last_directory = parent->prefs().get_last_directory();
  QString filename = QFileDialog::getSaveFileName(parent, title, last_directory, filetypes);
  if (filename.isEmpty()) {
    return "";
  }

  if (QFileInfo(filename).suffix() == "") {
    filename = filename + default_ext;
  }
  parent->prefs().set_last_directory(QFileInfo(filename).absolutePath());
  return filename;
}

//---------------------------------------------------------------------------
void ExportUtils::export_all_subjects_particle_scalars(ShapeWorksStudioApp* parent, QSharedPointer<Session> session) {
  bool single = StudioUtils::ask_multiple_domains_as_single(parent, session->get_project());

  QString filename = ExportUtils::get_save_filename(parent, QWidget::tr("Export All Shapes Particle Scalars"),
                                                    QWidget::tr("CSV files (*.csv)"), ".csv");
  if (filename.isEmpty()) {
    return;
  }

  QProgressDialog progress("Exporting Scalars...", "Abort", 0, session->get_num_shapes(), parent);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(2000);
  // progress.show();

  QFileInfo fi(filename);
  QString base = fi.path() + QDir::separator() + fi.completeBaseName();
  for (const auto& shape : session->get_shapes()) {
    QString shape_filename = base + "_" + shape->get_display_name() + "." + fi.completeSuffix();

    auto display_mode = session->get_display_mode();
    auto mesh_group = shape->get_meshes(display_mode, true);

    auto feature_maps = session->get_project()->get_feature_names();
    for (const std::string& feature : feature_maps) {
      shape->load_feature(display_mode, feature);
    }

    if (single) {
      auto poly_data = mesh_group.get_combined_poly_data();
      ExportUtils::write_scalars(parent, poly_data, shape_filename);
      parent->handle_message("Wrote: " + shape_filename);
    } else {
      auto domain_names = session->get_project()->get_domain_names();
      QFileInfo fi(shape_filename);
      QString domain_base = fi.path() + QDir::separator() + fi.completeBaseName();
      for (int domain = 0; domain < domain_names.size(); domain++) {
        QString name = domain_base + "_" + QString::fromStdString(domain_names[domain]) + "." + fi.completeSuffix();

        auto poly_data = mesh_group.meshes()[domain]->get_poly_data();
        if (!ExportUtils::write_scalars(parent, poly_data, name)) {
          return;
        }
        parent->handle_message("Wrote: " + name);
      }
    }

    if (progress.wasCanceled()) {
      break;
    }
    progress.setValue(progress.value() + 1);
  }
}

//---------------------------------------------------------------------------
bool ExportUtils::write_scalars(ShapeWorksStudioApp *app, vtkSmartPointer<vtkPolyData> poly_data, QString filename)
{
  if (!poly_data || !poly_data->GetPointData()->GetScalars()) {
    app->handle_error("Error, no scalars to export");
    return false;
  }

  std::ofstream output;
  output.open(filename.toStdString().c_str());
  if (output.bad()) {
    app->handle_error("Error writing to file: " + filename);
    return false;
  }
  output << "point,x,y,z";

  int num_arrays = poly_data->GetPointData()->GetNumberOfArrays();

  for (int i = 0; i < num_arrays; i++) {
    std::string name = StringUtils::safeString(poly_data->GetPointData()->GetArrayName(i));
    if (name == "") {
      name = "scalars";
    }
    auto array = poly_data->GetPointData()->GetArray(i);
    int num_components = array->GetNumberOfComponents();
    if (num_components == 1) {
      output << "," << name;
    } else {
      for (int j = 0; j < num_components; j++) {
        output << "," << name << "_" << j;
      }
    }
  }

  output << "\n";

  // iterate over vertices
  vtkPoints* points = poly_data->GetPoints();
  int num_points = points->GetNumberOfPoints();

  for (int i = 0; i < num_points; i++) {
    output << i;
    output << "," << poly_data->GetPoint(i)[0];
    output << "," << poly_data->GetPoint(i)[1];
    output << "," << poly_data->GetPoint(i)[2];

    for (int j = 0; j < num_arrays; j++) {
      auto array = poly_data->GetPointData()->GetArray(j);
      int num_components = array->GetNumberOfComponents();
      for (int k = 0; k < num_components; k++) {
        output << "," << array->GetTuple(i)[k];
      }
    }
    output << "\n";
  }

  output.close();
  return true;
}
}  // namespace shapeworks

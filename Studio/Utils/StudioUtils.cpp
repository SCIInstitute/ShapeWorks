#include <Data/Session.h>
#include <Logging.h>
#include <Mesh/Mesh.h>
#include <Utils/StudioUtils.h>
#include <vtkActor2D.h>
#include <vtkCoordinate.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyLine.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkReverseSense.h>

#include <QComboBox>
#include <QMessageBox>

namespace shapeworks {

//---------------------------------------------------------------------------
bool StudioUtils::ask_multiple_domains_as_single(QWidget* parent, std::shared_ptr<Project> project) {
  bool single = true;
  if (project->get_number_of_domains_per_subject() > 1) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parent, "Multiple Domains",
                                  "This export contains multiple domains.\n\n"
                                  "Would you like each domain exported separately?\n\n"
                                  "Each will be suffixed with the domain name.",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      single = false;
    }
  }
  return single;
}

//---------------------------------------------------------------------------
QStringList StudioUtils::to_string_list(std::vector<std::string> vector) {
  QStringList list;
  for (const auto& v : vector) {
    list << QString::fromStdString(v);
  }
  return list;
}

//---------------------------------------------------------------------------
// adapted from: https://kitware.github.io/vtk-examples/site/Cxx/Qt/ImageDataToQImage/
QImage StudioUtils::vtk_image_to_qimage(vtkSmartPointer<vtkImageData> image_data) {
  if (!image_data) {
    return QImage();
  }

  /// \todo retrieve just the UpdateExtent
  int width = image_data->GetDimensions()[0];
  int height = image_data->GetDimensions()[1];
  int num_components = image_data->GetNumberOfScalarComponents();

  QImage image(width, height, QImage::Format_ARGB32);
  QRgb* rgb_ptr = reinterpret_cast<QRgb*>(image.bits()) + width * (height - 1);
  unsigned char* colors_ptr = reinterpret_cast<unsigned char*>(image_data->GetScalarPointer());

  // Loop over the vtkImageData contents.
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      // Swap the vtkImageData RGB values with an equivalent QColor
      if (num_components == 4) {
        *(rgb_ptr++) = QColor(colors_ptr[0], colors_ptr[1], colors_ptr[2], colors_ptr[3]).rgba();
      } else {
        *(rgb_ptr++) = QColor(colors_ptr[0], colors_ptr[1], colors_ptr[2]).rgb();
      }
      colors_ptr += num_components;
    }

    rgb_ptr -= width * 2;
  }

  return image;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> StudioUtils::reverse_poly_data(vtkSmartPointer<vtkPolyData> poly_data) {
  auto reverse_filter = vtkSmartPointer<vtkReverseSense>::New();
  reverse_filter->SetInputData(poly_data);
  reverse_filter->ReverseNormalsOff();
  reverse_filter->ReverseCellsOn();
  reverse_filter->Update();
  poly_data = reverse_filter->GetOutput();
  return poly_data;
}

//---------------------------------------------------------------------------
QString StudioUtils::get_platform_string() {
  QString platform = "windows";
#ifdef Q_OS_MACOS
  platform = "macos";
#elif defined(Q_OS_LINUX)
  platform = "linux";
#endif
  return platform;
}

//---------------------------------------------------------------------------
void StudioUtils::add_viewport_border(vtkRenderer* renderer, double* color, double line_width) {
  // points start at upper right and proceed anti-clockwise
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(4);
  points->InsertPoint(0, 1, 1, 0);
  points->InsertPoint(1, 0, 1, 0);
  points->InsertPoint(2, 0, 0, 0);
  points->InsertPoint(3, 1, 0, 0);

  vtkNew<vtkCellArray> cells;
  cells->Initialize();
  vtkNew<vtkPolyLine> lines;

  lines->GetPointIds()->SetNumberOfIds(5);
  for (unsigned int i = 0; i < 4; ++i) {
    lines->GetPointIds()->SetId(i, i);
  }
  lines->GetPointIds()->SetId(4, 0);
  cells->InsertNextCell(lines);

  vtkNew<vtkPolyData> poly;
  poly->Initialize();
  poly->SetPoints(points);
  poly->SetLines(cells);

  // use normalized viewport coordinates since they are independent of window size
  vtkNew<vtkCoordinate> coordinate;
  coordinate->SetCoordinateSystemToNormalizedViewport();

  vtkNew<vtkPolyDataMapper2D> mapper;
  mapper->SetInputData(poly);
  mapper->SetTransformCoordinate(coordinate);

  vtkNew<vtkActor2D> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(color);
  actor->GetProperty()->SetLineWidth(line_width);

  renderer->AddViewProp(actor);
}

//---------------------------------------------------------------------------
bool StudioUtils::write_mesh(vtkSmartPointer<vtkPolyData> poly_data, QString filename) {
  if (!poly_data) {
    SW_ERROR("Error exporting mesh: not ready yet");
  }
  try {
    Mesh mesh(poly_data);
    mesh.write(filename.toStdString());
  } catch (std::exception& e) {
    SW_ERROR("{}", e.what());
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------
void StudioUtils::brightness_contrast_to_window_width_level(double brightness, double contrast, double min_intensity,
                                                            double max_intensity, double& window_width,
                                                            double& window_level) {
  // Calculate window width
  window_width = (contrast / 100.0) * (max_intensity - min_intensity);

  // Calculate window level
  window_level = min_intensity + (brightness / 100.0) * (max_intensity - min_intensity);
}

//---------------------------------------------------------------------------
void StudioUtils::window_width_level_to_brightness_contrast(double window_width, double window_level,
                                                            double min_intensity, double max_intensity,
                                                            double& brightness, double& contrast) {
  // Calculate brightness
  brightness = ((window_level - min_intensity) / (max_intensity - min_intensity)) * 100.0;

  // Calculate contrast
  contrast = (window_width / (max_intensity - min_intensity)) * 100.0;
}

//---------------------------------------------------------------------------
void StudioUtils::update_domain_combobox(QComboBox* combobox, QSharedPointer<Session> session) {
  auto domain_names = session->get_project()->get_domain_names();

  int currentIndex = combobox->currentIndex();
  if (domain_names.size() != combobox->count()) {
    combobox->clear();
    for (auto&& item : domain_names) {
      combobox->addItem(QString::fromStdString(item));
    }
  }
  if (currentIndex < 0) {
    currentIndex = 0;
  }
  if (currentIndex < combobox->count()) {
    combobox->setCurrentIndex(currentIndex);
  }
}
}  // namespace shapeworks

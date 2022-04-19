#include <Utils/StudioUtils.h>
#include <vtkImageData.h>

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
  QImage image(width, height, QImage::Format_RGB32);
  QRgb* rgb_ptr = reinterpret_cast<QRgb*>(image.bits()) + width * (height - 1);
  unsigned char* colors_ptr = reinterpret_cast<unsigned char*>(image_data->GetScalarPointer());

  // Loop over the vtkImageData contents.
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      // Swap the vtkImageData RGB values with an equivalent QColor
      *(rgb_ptr++) = QColor(colors_ptr[0], colors_ptr[1], colors_ptr[2]).rgb();
      colors_ptr += image_data->GetNumberOfScalarComponents();
    }

    rgb_ptr -= width * 2;
  }

  return image;
}
}  // namespace shapeworks

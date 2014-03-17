#include <QMainWindow>
#include <QTimer>

#include <vtkSmartPointer.h>

#include <itkImage.h>
#include <itkImageFileReader.h>

#include <Visualization/Viewer.h>

// Forward Qt class declarations
class Ui_ShapeWorksStudioApp;

class ViewerLayout;

class ShapeWorksStudioApp : public QMainWindow
{
  Q_OBJECT
public:

  ShapeWorksStudioApp( int argc, char** argv );
  ~ShapeWorksStudioApp();

public Q_SLOTS:

  void on_actionQuit_triggered();
  void on_actionImport_triggered();

private:

  void createMenu();

  // designer form
  Ui_ShapeWorksStudioApp* ui;

  typedef float PixelType;
  typedef itk::Image< PixelType, 3 > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;

  std::vector<ImageType::Pointer> images;

  Viewer *viewer;
};

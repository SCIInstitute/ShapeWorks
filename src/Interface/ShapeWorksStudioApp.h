#include <QMainWindow>
#include <QTimer>
#include <QActionGroup>

#include <vtkSmartPointer.h>

#include <itkImage.h>
#include <itkImageFileReader.h>

#include <Data/DataManager.h>
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
  void on_thumbnail_size_slider_valueChanged();
  void on_vertical_scroll_bar_valueChanged();
  void on_addButton_clicked();

private:

  void createMenu();

  void update_scrollbar();

  // designer form
  Ui_ShapeWorksStudioApp* ui;

  QActionGroup *action_group_;

  typedef float PixelType;
  typedef itk::Image< PixelType, 3 > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;

  std::vector<ImageType::Pointer> images;

  Viewer *viewer;

  DataManager *dataManager;

};

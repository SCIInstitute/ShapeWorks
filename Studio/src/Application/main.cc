#include <QApplication>
#include <QResource>
#include <Visualization/ShapeWorksStudioApp.h>
#include <iostream>

#include <vtkObject.h>
#include <itkMacro.h>

int main(int argc, char** argv)
{
  try {

    std::cerr << "ShapeWorksStudio initializing...\n";
    vtkObject::GlobalWarningDisplayOff();

    QApplication app(argc, argv);

    QSharedPointer<ShapeWorksStudioApp> studio_app =
      QSharedPointer<ShapeWorksStudioApp>(new ShapeWorksStudioApp(argc, argv));
    QResource::registerResource(RSCS_FILE);
    studio_app->setWindowIcon(QIcon(ICON_FILE));
    studio_app->show();

    // do this after "show" for mac initialization
    studio_app->initialize_vtk();

    if (argc == 2) {
      studio_app->open_project(QString(argv[1]));
    }
    return app.exec();
  } catch (itk::ExceptionObject & excep) {
    std::cerr << excep << std::endl;
  } catch (std::exception e) {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << e.what() << "\n";
  }
}

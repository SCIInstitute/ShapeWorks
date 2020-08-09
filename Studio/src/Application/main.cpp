#include <QApplication>
#include <QResource>
#include <Visualization/ShapeWorksStudioApp.h>
#include <iostream>

#include <vtkObject.h>
#include <itkMacro.h>

#include <Data/StudioLog.h>

#include <Utils/WindowsCrashHandler.h>

int main(int argc, char** argv)
{
  try {

#ifdef WIN32
    init_crash_handler();
    ::SetErrorMode( 0 );
#endif


    STUDIO_LOG_MESSAGE("ShapeWorksStudio initializing...");
    vtkObject::GlobalWarningDisplayOff();

    QApplication app(argc, argv);

    QSharedPointer<ShapeWorksStudioApp> studio_app =
      QSharedPointer<ShapeWorksStudioApp>(new ShapeWorksStudioApp());
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

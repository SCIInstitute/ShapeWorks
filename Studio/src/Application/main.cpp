#include <QApplication>
#include <QResource>
#include <QMessageBox>

#include <Visualization/ShapeWorksStudioApp.h>
#include <iostream>

#include <vtkObject.h>
#include <itkMacro.h>

#include <Data/StudioLog.h>

#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>

#ifdef _WIN32
#include <windows.h>
#include <Utils/WindowsCrashHandler.h>
#endif

int main(int argc, char** argv)
{
  try {
    STUDIO_LOG_MESSAGE("ShapeWorksStudio initializing...");


    // needed to ensure appropriate OpenGL context is created for VTK rendering.
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

#ifdef _WIN32
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    STUDIO_LOG_MESSAGE("ShapeWorksStudio win32 initializing...");
    init_crash_handler();
    ::SetErrorMode( 0 );
#endif

    vtkObject::GlobalWarningDisplayOff();

    QApplication app(argc, argv);

    QSharedPointer<ShapeWorksStudioApp> studio_app =
      QSharedPointer<ShapeWorksStudioApp>(new ShapeWorksStudioApp());
    QResource::registerResource(RSCS_FILE);
    studio_app->setWindowIcon(QIcon(ICON_FILE));
    studio_app->show();

    if (!shapeworks::StudioLog::Instance().check_log_open()) {
      QMessageBox::warning(NULL, "ShapeWorksStudio", "Unable to open log file: " +
                                                     shapeworks::StudioLog::Instance().get_log_filename());
    }

    // do this after "show" for mac initialization
    studio_app->initialize_vtk();

    if (argc == 2) {
      studio_app->open_project(QString(argv[1]));
    }
    else {
      studio_app->show_splash_screen();
    }
    return app.exec();
  } catch (itk::ExceptionObject & excep) {
    std::cerr << excep << std::endl;
  } catch (std::exception e) {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << e.what() << "\n";
  }
}

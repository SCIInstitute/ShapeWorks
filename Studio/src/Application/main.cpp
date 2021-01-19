#include <QApplication>
#include <QResource>
#include <QMessageBox>
#include <QDir>

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

    QSharedPointer<shapeworks::ShapeWorksStudioApp> studio_app =
      QSharedPointer<shapeworks::ShapeWorksStudioApp>(new shapeworks::ShapeWorksStudioApp());
    QResource::registerResource(RSCS_FILE);
    studio_app->setWindowIcon(QIcon(ICON_FILE));
    studio_app->show();

    if (!shapeworks::StudioLog::Instance().check_log_open()) {
      QMessageBox::warning(NULL, "ShapeWorksStudio", "Unable to open log file: " +
                                                     shapeworks::StudioLog::Instance().get_log_filename());
    }

    // do this after "show" for mac initialization
    studio_app->initialize_vtk();

    if (argc > 1) {
      QString filename = QString(argv[1]);
      if (filename.toLower().endsWith(".xlsx") || filename.toLower().endsWith(".xml")) {
        studio_app->open_project(filename);
      }
      else {
        QStringList files;
        QDir dir(".");
        for (int i = 1; i < argc; i++) {
          // need to rewrite as the project path will be set to the first file
          files << dir.absoluteFilePath(argv[i]);
        }
        QTimer::singleShot(0, [=]() {
          studio_app->import_files(files);
        });
      }
    }
    else {
      studio_app->show_splash_screen();
    }
    return app.exec();
  } catch (itk::ExceptionObject& excep) {
    std::cerr << excep << std::endl;
  } catch (std::exception e) {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << e.what() << "\n";
  }
}

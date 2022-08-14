#include <tbb/tbb.h>

#include <QApplication>
#include <QResource>
#include <QMessageBox>
#include <QDir>

#include <Interface/ShapeWorksStudioApp.h>
#include <Applications/Configuration.h>
#include <Data/StudioLog.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <QVTKOpenGLNativeWidget.h>


#ifdef _WIN32
#include <Utils/WindowsCrashHandler.h>
#include <windows.h>
#endif

int main(int argc, char** argv) {
  // tbb::task_scheduler_init init(1);

  try {
    STUDIO_LOG_MESSAGE("ShapeWorks Studio " SHAPEWORKS_VERSION " initializing...");

    // needed to ensure appropriate OpenGL context is created for VTK rendering.
    QSurfaceFormat format = QVTKOpenGLNativeWidget::defaultFormat();
#ifdef _WIN32
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
#endif
    format.setSamples(0);
    QSurfaceFormat::setDefaultFormat(format);

#ifdef _WIN32
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    STUDIO_LOG_MESSAGE("ShapeWorks Studio win32 initializing...");
    init_crash_handler();
    ::SetErrorMode(0);
#endif

    QApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QSharedPointer<shapeworks::ShapeWorksStudioApp> studio_app =
        QSharedPointer<shapeworks::ShapeWorksStudioApp>(new shapeworks::ShapeWorksStudioApp());
    QResource::registerResource(RSCS_FILE);
    studio_app->setWindowIcon(QIcon(ICON_FILE));
    studio_app->initialize_vtk();
    studio_app->show();

    if (!shapeworks::StudioLog::Instance().check_log_open()) {
      QMessageBox::warning(NULL, "ShapeWorks Studio",
                           "Unable to open log file: " + shapeworks::StudioLog::Instance().get_log_filename());
    }

    if (argc > 1) {
      QString filename = QString(argv[1]);
      if (filename.endsWith(".xlsx", Qt::CaseInsensitive) || filename.endsWith(".xml", Qt::CaseInsensitive) ||
          filename.endsWith(".swproj", Qt::CaseInsensitive)) {
        QTimer::singleShot(0, [=]() { studio_app->open_project(filename); });
      } else {
        QStringList files;
        QDir dir(".");
        for (int i = 1; i < argc; i++) {
          // need to rewrite as the project path will be set to the first file
          files << dir.absoluteFilePath(argv[i]);
        }
        QTimer::singleShot(0, [=]() { studio_app->import_files(files); });
      }
    } else {
      studio_app->show_splash_screen();
    }
    return app.exec();
  } catch (itk::ExceptionObject& excep) {
    std::cerr << excep << std::endl;
  } catch (std::exception& e) {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << e.what() << "\n";
  }
}

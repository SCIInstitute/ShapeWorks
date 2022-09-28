#include <tbb/tbb.h>

#include <QApplication>
#include <QResource>
#include <QMessageBox>
#include <QDir>

#include <itkMacro.h>

#include <Interface/ShapeWorksStudioApp.h>
#include <Applications/Configuration.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Logging.h>
#include <QVTKOpenGLNativeWidget.h>

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QResource>
#include <QStandardPaths>
#include <QSurfaceFormat>
#include <iostream>

#ifdef _WIN32
#include <Utils/WindowsCrashHandler.h>
#include <windows.h>
#endif

using namespace shapeworks;

static void new_log() {
  QDateTime date_time = QDateTime::currentDateTime();
  QString session_name = date_time.toString("yyyy-MM-dd_HH_mm_ss");

  auto app_data_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QString path = app_data_path + QDir::separator() + "shapeworks" + QDir::separator() + "logs";

  QDir dir(path);
  if (!dir.exists()) {
    dir.mkpath(".");
  }

  // clean out old logs
  QStringList logs = dir.entryList(QDir::Files, QDir::Time);

  for (int i = 100; i < logs.size(); i++) {
    dir.remove(logs[i]);
  }

  QString logfile = path + QDir::separator() + "studio-" + session_name + ".txt";

  Logging::Instance().open_file_log(logfile.toStdString());
}

int main(int argc, char** argv) {
  // tbb::task_scheduler_init init(1);

  try {
    new_log();
    SW_LOG("ShapeWorks Studio " SHAPEWORKS_VERSION " initializing...");

    // needed to ensure appropriate OpenGL context is created for VTK rendering.
    QSurfaceFormat format = QVTKOpenGLNativeWidget::defaultFormat();
#ifdef _WIN32
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
#endif
    format.setSamples(0);
    QSurfaceFormat::setDefaultFormat(format);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#ifdef _WIN32
    SW_LOG("ShapeWorks Studio win32 initializing...");
    init_crash_handler();
    ::SetErrorMode(0);
#endif

    QApplication app(argc, argv);

    auto studio_app = QSharedPointer<ShapeWorksStudioApp>::create();
    QResource::registerResource(RSCS_FILE);
    studio_app->setWindowIcon(QIcon(ICON_FILE));
    studio_app->initialize_vtk();
    studio_app->show();

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

#include <QMainWindow>
#include <QTimer>

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


private:

  void createMenu();

  // designer form
  Ui_ShapeWorksStudioApp* ui;

  ViewerLayout* viewerLayout;

  QTimer eventTimer;

};

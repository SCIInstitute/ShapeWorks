#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

// Qt
#include <QSharedPointer>
#include <QWidget>

// ShapeWorks
#include <ParticleShapeStatistics.h>

// Studio
#include <Data/Shape.h>
#include <Data/Preferences.h>
#include <Visualization/Visualizer.h>
#include <Visualization/BarGraph.h>

class Ui_DeepSSMTool;

namespace shapeworks {

class Session;
class Lightbox;
class ShapeWorksStudioApp;

class DeepSSMTool : public QWidget {
Q_OBJECT;
public:

  DeepSSMTool(Preferences& prefs);
  ~DeepSSMTool();

  /// set the pointer to the session
  void set_session(QSharedPointer<Session> session);

  /// set the pointer to the application
  void set_app(ShapeWorksStudioApp* app);

  //! Set if this tool is active
  void set_active(bool active);

  //! Return if this tool is active
  bool get_active();

  void load_params();
  void store_params();

  void shutdown();


public Q_SLOTS:


signals:

  void update_view();
  void progress(int);
  void message(QString);
  void error(QString);
  void warning(QString);

private:


  Preferences& preferences_;

  Ui_DeepSSMTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;

};

}

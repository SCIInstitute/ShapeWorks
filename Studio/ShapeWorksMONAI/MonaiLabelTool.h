#pragma once

#include <Data/Preferences.h>
#include <Data/Session.h>
#include <Project.h>
#include <Shape.h>
#include <ShapeWorksMONAI/MonaiLabelJob.h>
#include <Interface/ShapeWorksStudioApp.h>

#include <QElapsedTimer>
#include <QObject>
#include <QSharedPointer>
#include <QWidget>

class Ui_MonaiLabelTool;
class QLabel;
class QTableWidget;
class QLineEdit;
class Preferences;

namespace shapeworks {
class ShapeWorksStudioApp;
class Session;
}

namespace monailabel {

class MonaiLabelJob;
using shapeworks::ShapeList;
using shapeworks::ShapeWorksStudioApp;
using shapeworks::Session;



class MonaiLabelTool : public QWidget {
  Q_OBJECT;

 public:
  const static std::string MONAI_MODE_SEGMENTATION;
  const static std::string MONAI_MODE_DEEPGROW;
  const static std::string MONAI_MODE_DEEPEDIT;

  MonaiLabelTool(Preferences& prefs);
  ~MonaiLabelTool();

  void set_session(QSharedPointer<Session> session);
  void set_app(ShapeWorksStudioApp* app);
  bool is_active();
  void load_params();
  void loadParamsFromUi();
  void shutdown();
  void runSegmentationTool();
  void resizeEvent(QResizeEvent* event) override;
  int getCurrentSampleNumber();
  void enable_actions();

 public Q_SLOTS:
  void handle_error(QString msg);
  void onConnectServer();
  void updateDisplayPanels();
  // void updateUiElements();
  void onServerAddressChanged();
  void onModelTypeChanged(int index);
  void triggerUpdateView();
  void update_panels();
  void handle_thread_complete();
  void handle_progress(int val, QString message);
  void handleSampleView();

 Q_SIGNALS:
  void update_view();
  void progress(int);
  void sampleChanged();


 private:
  Preferences& preferences_;
  Ui_MonaiLabelTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;
  bool tool_is_running_ = false;
  QSharedPointer<MonaiLabelJob> monai_label_logic_;
  QElapsedTimer timer_;
  ShapeList shapes_;

  std::string server_address_;
  std::string model_type_;
  std::string strategy_;
  std::string client_id_;
};

}  // namespace monailabel

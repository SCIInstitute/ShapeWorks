#pragma once

#include <QElapsedTimer>
#include <QSharedPointer>
#include <QWidget>

#include <Data/Preferences.h>
#include <Project.h>
#include <Shape.h>

class Ui_MonaiLabelTool;
class QLabel;
class QTableWidget;
class QLineEdit;

namespace shapeworks {

class Session;
class Lightbox;
class ShapeWorksStudioApp;

class MonaiLabelTool : public QWidget {
  
  using PointList = std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, 3>>;
  Q_OBJECT;

 public:
  MonaiLabelTool(Preferences& prefs);
  ~MonaiLabelTool() {
    // clean temp dirs
  };
  void set_session(QSharedPointer<Session> session);
  void set_app(ShapeWorksStudioApp* app);
  bool is_active();
  void load_params();
  void store_params();
  void shutdown();
  ShapeList get_shapes();
  void resizeEvent(QResizeEvent* event) override;

  std::string get_display_feature();

 public Q_SLOTS:
  void loadServer();
  void restoreDefaults();
  void monitorTraining();
  void onEditControlPoints(std::vector<std::vector<double>>& pointListNode, std::string tagName);
  void onTraining(); 
  void onStopTraining();
  bool isTrainingRunning(bool check_only);
  void onNextSampleButton(); // handle later
  void onUploadImage();
  void onImportLabel();
  void onSaveLabel();
  void onClickSegmentation();
  void onUpdateDeepGrow();
  void onClickDeepGrow(); // not a slot
  void updateParameterNodeFromGUI();
 Q_SIGNALS:
  void update_view();
  void progress(int);

 private:
  std::string getSessionId();
  Preferences& preferences_;
  Ui_MonaiTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;
  bool tool_is_running_ = false;
  MonaiTool::ToolMode current_tool_ = MonaiTool::ToolMode::DeepSSM_AugmentationType;
  QSharedPointer<MONAILabelJob> monai_label_logic_;
  QElapsedTimer timer_; // link with monitorTrraining
  ShapeList shapes_;
  PointList positiveList; // write make_shared in constructor /?TODO:
  PointList negativeList;

};

}  // namespace shapeworks

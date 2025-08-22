---
title: Studio/ShapeWorksMONAI/MonaiLabelTool.h

---

# Studio/ShapeWorksMONAI/MonaiLabelTool.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |
| **[monailabel](../Namespaces/namespacemonailabel.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[monailabel::MonaiLabelTool](../Classes/classmonailabel_1_1MonaiLabelTool.md)**  |




## Source code

```cpp
#pragma once

#include <Data/Preferences.h>
#include <Data/Session.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Project.h>
#include <Shape.h>
#include <ShapeWorksMONAI/MonaiLabelJob.h>

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
}  // namespace shapeworks
namespace monailabel {

class MonaiLabelJob;
using shapeworks::Session;
using shapeworks::ShapeWorksStudioApp;
class MonaiLabelTool : public QWidget {
  Q_OBJECT;

 public:
  const static std::string MONAI_MODE_SEGMENTATION;
  const static std::string MONAI_MODE_DEEPGROW;
  const static std::string MONAI_MODE_DEEPEDIT;
  const static std::string MONAI_SAMPLE_STRATEGY_RANDOM;

  MonaiLabelTool(Preferences& prefs);
  ~MonaiLabelTool();

  void set_session(QSharedPointer<Session> session);
  void set_app(ShapeWorksStudioApp* app);
  bool is_active();
  void loadParamsFromUi();
  void shutdown();
  void runSegmentationTool();
  void resizeEvent(QResizeEvent* event) override;
  int getCurrentSampleNumber();
  void enable_actions();
  void activate();

 public Q_SLOTS:
  void handle_error(QString msg);
  void onConnectServer();
  void onServerAddressChanged();
  void onModelTypeChanged(int index);
  void triggerUpdateView();
  void handle_progress(int val, QString message);
  void handleSampleNumberChanged();
  void handleClientInitialized(bool success);
  void handleUploadSampleCompleted();
  void handleSegmentationCompleted();
  void handleSubmitLabelCompleted();

 Q_SIGNALS:
  void update_view();
  void progress(int);
  void sampleChanged();

 private:

  void set_connect_button();

  Preferences& preferences_;
  Ui_MonaiLabelTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;
  bool tool_is_running_ = false;
  QSharedPointer<MonaiLabelJob> monai_label_job_;
  QElapsedTimer timer_;

  std::string server_address_;
  std::string model_type_;
  std::string strategy_;
  std::string client_id_;
  int samples_processed_ = 0;
};

}  // namespace monailabel
```


-------------------------------

Updated on 2025-08-22 at 08:23:44 +0000

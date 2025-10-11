---
title: Studio/ShapeWorksMONAI/MonaiLabelJob.h

---

# Studio/ShapeWorksMONAI/MonaiLabelJob.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |
| **[monailabel](../Namespaces/namespacemonailabel.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[monailabel::MonaiLabelJob](../Classes/classmonailabel_1_1MonaiLabelJob.md)**  |




## Source code

```cpp
#pragma once

#include <Data/Session.h>
#include <Job/Job.h>
#include <Project/Project.h>
#include <Shape.h>
#include <ShapeWorksMONAI/MonaiLabelTool.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <QObject>
#include <QSharedPointer>
#include <string>

namespace py = pybind11;

namespace shapeworks {
class ShapeWorksStudioApp;
class Session;
class Job;
}  // namespace shapeworks

namespace monailabel {

using shapeworks::Job;
using shapeworks::ProjectHandle;
using shapeworks::Session;
using shapeworks::ShapeWorksStudioApp;

class MonaiLabelJob : public Job {
  Q_OBJECT;

 public:
  const static std::string MONAI_RESULT_EXTENSION;
  const static std::string MONAI_RESULT_DTYPE;

  MonaiLabelJob(QSharedPointer<Session> session, const std::string& server_url, const std::string& client_id,
                const std::string& strategy, const std::string& model_type);
  ~MonaiLabelJob();
  void setServer(const std::string& server_url);
  void setModelType(const std::string& model_type);
  inline const std::string& getServer() { return server_url_; }
  void setClientId(const std::string& client_id = "");
  inline const std::string& getClientId() { return client_id_; };
  void initializeClient();
  inline std::shared_ptr<py::object> getClient() const { return monai_client_; };

  py::dict getInfo();
  std::string getModelName(std::string modelType);
  std::vector<std::string> getModelNames(const std::string& model_type);
  std::string getSessionId();
  py::dict getParamsFromConfig(std::string section, std::string name);

  // MONAI Client callers
  py::dict nextSample(std::string strategy, py::dict params);
  py::tuple infer(std::string model, std::string image_in, py::dict params, std::string label_in, std::string file,
                  std::string session_id);
  py::dict saveLabel(std::string image_in, std::string label_in, py::dict params);
  py::dict uploadImage(std::string image_in, std::string image_id);

  void updateShapes();

  void runSegmentationModel();
  void run() override;
  QString name() override;
  void python_message(std::string str);
  void setCurrentSampleNumber(int n);

 public Q_SLOTS:
  void onUploadSampleClicked();     // Triggered when  upload Sample button is clicked
  void onRunSegmentationClicked();  // Triggered when Run Segmentation is clicked
  void onSubmitLabelClicked();      // Triggered when Submit Label is clicked

 Q_SIGNALS:
  void triggerUpdateView();
  void triggerClientInitialized(bool success);
  void triggerUploadSampleCompleted();
  void triggerSegmentationCompleted();
  void triggerSubmitLabelCompleted();

 private:
  std::string tmp_dir_;
  std::string server_url_;
  std::string client_id_;
  // TODO: implement remote server session
  //   std::string username_;
  //   std::string password_;
  //   std::string auth_token_;
  std::string strategy_;
  std::string model_type_ = "";
  std::string model_name_ = "";
  std::unordered_map<std::string, std::vector<std::string>>
      models_available_;  // TODO: add functionality to interchange between models from UI
  std::shared_ptr<py::object> monai_client_;

  // QT states
  bool waitingForUpload;
  bool waitingForSegmentation;
  bool waitingForLabelSubmission;

  // Current subjects
  int sample_number_;
  std::string currentSampleId_;
  std::string currentSegmentationPath_;
  std::vector<std::string> currentSegmentationPaths_;
  std::vector<std::string> organNames_;

  QSharedPointer<Session> session_;
  ProjectHandle project_;

  bool error_occurred_ = false;
};
}  // namespace monailabel
```


-------------------------------

Updated on 2025-10-11 at 04:45:09 +0000

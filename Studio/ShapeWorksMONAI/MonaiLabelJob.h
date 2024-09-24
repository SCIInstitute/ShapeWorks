#pragma once

#include <Data/Session.h>
#include <Job/Job.h>
#include <Project/Project.h>
#include <Project/Subject.h>
#include <Shape.h>
#include <ShapeWorksMONAI/MonaiLabelTool.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <QObject>
#include <QSharedPointer>
#include <string>

namespace py = pybind11;
using namespace pybind11::literals;

namespace shapeworks {
    class ShapeWorksStudioApp;
    class Subject;
    class Session;
    class Job;
}

namespace monailabel {

using shapeworks::ProjectHandle;
using shapeworks::Session;
using shapeworks::ShapeHandle;
using shapeworks::ShapeList;
using shapeworks::Subject;
using shapeworks::ShapeWorksStudioApp;
using shapeworks::Job;

class MonaiLabelJob : public Job {
  Q_OBJECT;

 public:
  MonaiLabelJob(QSharedPointer<Session> session, const std::string& server_url,
                const std::string& client_id, const std::string& strategy,
                const std::string& model_type);
  ~MonaiLabelJob();

  void setServer(const std::string& server_url);
  void setModelType(const std::string& model_type);
  inline const std::string& getServer() { return server_url_; }
  void setClientId(const std::string& client_id = "");
  inline const std::string& getClientId() { return client_id_; };

  void initializeClient();
  inline std::shared_ptr<py::object> getClient() const {
    return monai_client_;
  };

  py::dict getInfo();
  std::string getModelName(std::string modelType);
  std::string getSessionId();
  py::dict getParamsFromConfig(std::string section, std::string name);

  // MONAI Client callers
  py::dict nextSample(std::string strategy, py::dict params);
  py::tuple infer(std::string model, std::string image_in, py::dict params,
                  std::string label_in, std::string file,
                  std::string session_id);
  py::dict saveLabel(std::string image_in, std::string label_in,
                     py::dict params);
  py::dict uploadImage(std::string image_in, std::string image_id);

  void set_subject_image_filename(const std::shared_ptr<Subject>& subject,
                                  const std::string& filename);
  std::string get_feature_name();

  void updateShapes();

  void runSegmentationModel();
  void run() override;
  QString name() override;
  void python_message(std::string str);
  void setCurrentSampleNumber(int n);

 public Q_SLOTS:
//   void onNextSampleClicked();  // Triggered when Next Sample button is clicked
  void onUploadSampleClicked();  // Triggered when  upload Sample button is clicked
  void onRunSegmentationClicked();   // Triggered when Run Segmentation is clicked
  void onSubmitLabelClicked();  // Triggered when Submit Label is clicked

 Q_SIGNALS:
  void triggerUpdateView();

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
  std::shared_ptr<py::object> monai_client_;

  // QT states
  bool waitingForUpload;
  bool waitingForSegmentation;
  bool waitingForLabelSubmission;


  // Current subjects
  int sample_number_;
  py::dict currentSample_;
  std::string currentSampleId_;
  std::string currentSegmentationPath_;

  QSharedPointer<Session> session_;
  ProjectHandle project_;
  ShapeList shapes_;
};
}  // namespace monailabel

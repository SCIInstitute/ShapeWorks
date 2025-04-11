#include <Data/Session.h>
#include <Logging.h>
#include <Project/ProjectUtils.h>
#include <Shape.h>
#include <ShapeWorksMONAI/MonaiLabelJob.h>
#include <ShapeWorksMONAI/MonaiLabelUtils.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <QFileInfo>
#include <QSharedPointer>
#include <QString>
#include <string>

#include "qdir.h"

namespace py = pybind11;

using namespace shapeworks;

namespace monailabel {

const std::string MonaiLabelJob::MONAI_RESULT_EXTENSION(".nrrd");
const std::string MonaiLabelJob::MONAI_RESULT_DTYPE("uint8");

//---------------------------------------------------------------------------
MonaiLabelJob::MonaiLabelJob(QSharedPointer<Session> session, const std::string &server_url,
                             const std::string &client_id, const std::string &strategy, const std::string &model_type)
    : session_(session),
      server_url_(server_url),
      client_id_(client_id),
      waitingForUpload(false),
      waitingForSegmentation(false),
      waitingForLabelSubmission(false),
      strategy_(strategy),
      model_type_(model_type),
      monai_client_(nullptr) {
  project_ = session_->get_project();
  QDir projectDir(QString::fromStdString(session->get_project()->get_project_path()));
  QString labels_dir = projectDir.filePath("labels-prediction");

  if (MonaiLabelUtils::createDir(labels_dir)) {
    tmp_dir_ = labels_dir.toStdString();
  } else {
    SW_ERROR("Unable to create directory not label predictions!");
  }
  sample_number_ = 0;
}

//---------------------------------------------------------------------------
MonaiLabelJob::~MonaiLabelJob() {}

//---------------------------------------------------------------------------
void MonaiLabelJob::setServer(const std::string &server_url) {
  server_url_ = !server_url.empty() ? server_url : "http://127.0.0.1:8000";
}

//---------------------------------------------------------------------------
void MonaiLabelJob::setModelType(const std::string &model_type) {
  model_type_ = !model_type.empty() ? model_type : "segmentation";
}

//---------------------------------------------------------------------------
void MonaiLabelJob::setClientId(const std::string &client_id) {
  client_id_ = !client_id.empty() ? client_id : "user-xyz";
}

//---------------------------------------------------------------------------
void MonaiLabelJob::setCurrentSampleNumber(int n) { sample_number_ = n; }

//---------------------------------------------------------------------------
void MonaiLabelJob::initializeClient() {
  SW_DEBUG("Initializing MONAI Client with server: {} tmp dir: {} client_id: {}", server_url_, tmp_dir_, client_id_);
  try {
    error_occurred_ = false;

    py::module monai_label = py::module::import("MONAILabel");
    py::object monai_client_class = monai_label.attr("MONAILabelClient");
    py::str py_server_url(server_url_);
    py::str py_tmp_dir(tmp_dir_);
    py::str py_client_id(client_id_);
    SW_DEBUG("Instantiating monai_client_class");
    monai_client_ = std::make_shared<py::object>(monai_client_class(py_server_url, py_tmp_dir, py_client_id));
    SW_DEBUG("Instantiating monai_client_class completed");
    if (!monai_client_) {
      SW_ERROR("Error in instantiating MONAI client");
      return;
    } else {
      SW_DEBUG("MONAI Client object created.");
    }
    model_name_ = getModelName(model_type_);
    models_available_[model_type_] = {model_name_};
    triggerClientInitialized(!error_occurred_);
  } catch (std::exception &e) {
    SW_ERROR("Error importing MONAILabel or initializing MONAILabelClient: {}", e.what());
    return;
  }
}

//---------------------------------------------------------------------------
py::dict MonaiLabelJob::getInfo() {
  py::dict response;
  try {
    if (!monai_client_) {
      SW_ERROR("MONAI client not initialized yet");
      return response;
    }
    response = (*monai_client_).attr("info")();
  } catch (const py::error_already_set &e) {
    SW_ERROR("Python error: {}", e.what());
    error_occurred_ = true;
  } catch (const std::exception &e) {
    SW_ERROR("Exception occurred: {}", e.what());
    error_occurred_ = true;
  } catch (...) {
    SW_ERROR("An unknown error occurred!");
    error_occurred_ = true;
  }
  return response;
}

//---------------------------------------------------------------------------
std::vector<std::string> MonaiLabelJob::getModelNames(const std::string &model_type) {
  auto it = models_available_.find(model_type);
  if (it != models_available_.end()) {
    return it->second;
  } else {
    return {};
  }
}

//---------------------------------------------------------------------------
std::string MonaiLabelJob::getModelName(std::string modelType) {
  py::str mappedSection = py::str("models");
  py::dict info = getInfo();
  std::string modelName = "";
  if (info.contains(mappedSection)) {
    py::dict sectionConfig = info[mappedSection].cast<py::dict>();  // models dict
    for (const auto &item : sectionConfig) {
      std::string nameFound = py::str(item.first);
      py::dict modelConfig = item.second.cast<py::dict>();
      if (modelConfig.contains(py::str("type"))) {
        std::string type = py::str(modelConfig[py::str("type")]);
        if (type.find(modelType) != std::string::npos) {
          modelName = nameFound;
        }
      }
    }
  }

  SW_DEBUG("Model Name {} fetched", modelName);

  return modelName;
}

//---------------------------------------------------------------------------
std::string MonaiLabelJob::getSessionId() {
  std::string session_id = "";
  if (!currentSampleId_.empty()) {
    // if (currentSample.contains("session") &&
    //     currentSample["session"].cast<bool>()) {
    //   // TODO: add logic later for Remote server sessions
    // }
  }
  return session_id;
}

//---------------------------------------------------------------------------
py::dict MonaiLabelJob::getParamsFromConfig(std::string section, std::string name) {
  py::dict info = getInfo();
  std::unordered_map<std::string, std::string> mapping = {
      {"infer", "models"}, {"train", "trainers"}, {"activelearning", "strategies"}, {"scoring", "scoring"}};
  auto it = mapping.find(section);
  std::string mappedSection = (it != mapping.end()) ? it->second : section;
  py::dict result;
  if (info.contains(py::str(mappedSection))) {
    py::dict sectionConfig = info[py::str(mappedSection)].cast<py::dict>();
    if (sectionConfig.contains(py::str(name))) {
      py::dict modelConfig = sectionConfig[py::str(name)].cast<py::dict>();
      if (modelConfig.contains(py::str("config"))) {
        py::dict configDict = modelConfig[py::str("config")].cast<py::dict>();
        for (auto item : configDict) {
          py::str key = py::str(item.first);
          py::object value = item.second.cast<py::object>();
          if (py::isinstance<py::list>(value)) {
            py::list valueList = value.cast<py::list>();
            if (!valueList.empty()) {
              // result[key] = py::str(valueList[0]);
              result[key] = valueList[0];
            }
          } else {
            // result[key] = py::str(value);
            result[key] = value;
          }
        }
      }
    }
  }
  // SW_DEBUG("Getting params from config {} ",
  //          py::repr(result).cast<std::string>());
  return result;
}

//---------------------------------------------------------------------------
py::dict MonaiLabelJob::nextSample(std::string strategy, py::dict params) {
  py::dict response;
  if (!monai_client_) {
    SW_ERROR("MONAI client not initialized yet");
    return response;
  }
  response = (*monai_client_).attr("next_sample")(strategy, params);
  return response;
}

//---------------------------------------------------------------------------
py::dict MonaiLabelJob::uploadImage(std::string image_in, std::string image_id) {
  py::dict response;

  try {
    if (!monai_client_) {
      SW_ERROR("MONAI client not initialized yet");
      return response;
    }
    SW_DEBUG("Uploading sample number {} to MONAI Label server", sample_number_);
    response = (*monai_client_).attr("upload_image")(image_in, image_id.empty() ? py::none() : py::cast(image_id));
    // SW_DEBUG("Upload sample response: " +
    //  py::repr(response).cast<std::string>());

  } catch (const py::error_already_set &e) {
    SW_ERROR("Python error: {}", e.what());
  } catch (const std::exception &e) {
    SW_ERROR("Exception occurred: {}", e.what());
  } catch (...) {
    SW_ERROR("An unknown error occurred!");
  }

  return response;
}

//---------------------------------------------------------------------------
py::tuple MonaiLabelJob::infer(std::string model, std::string image_in, py::dict params, std::string label_in,
                               std::string file, std::string session_id) {
  py::tuple result = py::make_tuple();

  try {
    if (!monai_client_) {
      SW_ERROR("MONAI client not initialized yet");
      return result;
    }
    params[py::str("result_extension")] = MonaiLabelJob::MONAI_RESULT_EXTENSION;
    params[py::str("result_dtype")] = MonaiLabelJob::MONAI_RESULT_DTYPE;
    result = (*monai_client_)
                 .attr("infer")(model, image_in, params, label_in.empty() ? py::none() : py::cast(label_in),
                                file.empty() ? py::none() : py::cast(file),
                                session_id.empty() ? py::none() : py::cast(session_id));
    // std::cout << "DEBUG | infer call successfully made " << py::repr(result).cast<std::string>() << std::endl;
    // SW_DEBUG("Infer response: " + py::repr(result).cast<std::string>());
  }

  catch (const py::error_already_set &e) {
    SW_ERROR("Python error: {}", e.what());
  } catch (const std::exception &e) {
    SW_ERROR("Exception occurred: {}", e.what());
  } catch (...) {
    SW_ERROR("An unknown error occurred!");
  }

  return result;
}

//---------------------------------------------------------------------------
py::dict MonaiLabelJob::saveLabel(std::string image_in, std::string label_in, py::dict params) {
  py::dict response;
  try {
    if (!monai_client_) {
      SW_ERROR("MONAI client not initialized yet");
      return response;
    }
    response = (*monai_client_).attr("save_label")(image_in, label_in, "", params);
    // SW_DEBUG("Save Label response: " +
    // py::repr(response).cast<std::string>());
  }

  catch (const py::error_already_set &e) {
    SW_ERROR("Python error: {}", e.what());
  } catch (const std::exception &e) {
    SW_ERROR("Exception occurred: {}", e.what());
  } catch (...) {
    SW_ERROR("An unknown error occurred!");
  }
  return response;
}

//---------------------------------------------------------------------------
void MonaiLabelJob::runSegmentationModel() {
  if (!monai_client_) {
    SW_DEBUG("MONAI client not initialized, initializing now...");
    initializeClient();
  } else {
    SW_DEBUG("MONAI client already initialized");
  }

  SW_DEBUG("Waiting for Upload Sample Action...");
  waitingForUpload = true;
  waitingForSegmentation = false;
  waitingForLabelSubmission = false;
}

//---------------------------------------------------------------------------
void MonaiLabelJob::run() { runSegmentationModel(); }

//---------------------------------------------------------------------------
QString MonaiLabelJob::name() { return "MONAI Label"; }

//---------------------------------------------------------------------------
void MonaiLabelJob::python_message(std::string str) { SW_LOG(str); }

/*-----Q_SLOTS----*/

//---------------------------------------------------------------------------
void MonaiLabelJob::onUploadSampleClicked() {
  if (waitingForUpload) {
    progress(-1);
    py::dict params = getParamsFromConfig("activelearning", strategy_);
    auto subjects = session_->get_project()->get_subjects();
    auto shapes = session_->get_shapes();

    if (sample_number_ < shapes.size()) {
      auto cur_shape = shapes[sample_number_];
      auto cur_subject = cur_shape->get_subject();

      project::types::StringMap map = cur_subject->get_feature_filenames();
      std::string fp = map[MonaiLabelUtils::getFeatureName(session_)];

      QFileInfo imageFileInfo(QString::fromStdString(fp));
      QString absoluteImagePath = imageFileInfo.absoluteFilePath();
      QString subjectName = imageFileInfo.completeBaseName();
      QString extension = imageFileInfo.completeSuffix();

      py::dict response = uploadImage(absoluteImagePath.toStdString(), subjectName.toStdString());

      if (response[py::str("image")].cast<std::string>() == subjectName.toStdString()) {
        currentSampleId_ = subjectName.toStdString();
      } else {
        SW_ERROR("Upload source volume failed!");
      }

      waitingForSegmentation = true;
    } else {
      SW_ERROR(
          "Current sample's index {} exceeds total number of samples in "
          "project {}",
          sample_number_, shapes.size());
      return;
    }

    Q_EMIT triggerUploadSampleCompleted();
  }
}

//---------------------------------------------------------------------------
void MonaiLabelJob::onRunSegmentationClicked() {
  if (waitingForSegmentation) {
    progress(-1);
    if (currentSampleId_.empty()) {
      SW_ERROR("Sample not uploaded yet!");
      return;
    }
    SW_LOG("⚙️ Processing inference on the current subject");
    py::dict params = getParamsFromConfig("infer", model_name_);

    py::tuple result = infer(model_name_, currentSampleId_, params, "", "", getSessionId());

    currentSegmentationPath_ = result[0].cast<std::string>();
    py::dict result_params = result[1].cast<py::dict>();

    // Extract label names from result_params
    py::dict label_dict = result_params["label_names"].cast<py::dict>();
    std::map<int, std::string> organLabels;
    organNames_.resize(0);

    for (auto &item : label_dict) {
      std::string organName = item.first.cast<std::string>();
      int label = item.second.cast<int>();
      if (label > 0) {  // Exclude background (0)
        organLabels[label] = organName;
        organNames_.push_back(organName);
      }
    }

    MonaiLabelUtils::processSegmentation(currentSegmentationPath_, organLabels, tmp_dir_, currentSampleId_,
                                         currentSegmentationPaths_);

    QDir projDir(QString::fromStdString(tmp_dir_));
    QString destPath = projDir.filePath(QString::fromStdString(currentSampleId_ + ".nrrd"));
    MonaiLabelUtils::copySegmentation(QString::fromStdString(currentSegmentationPath_), destPath);
    MonaiLabelUtils::deleteTempFile(QString::fromStdString(currentSegmentationPath_));
    currentSegmentationPath_ = destPath.toStdString();
    SW_DEBUG("Prediction label saved at {}", currentSegmentationPath_);

    waitingForSegmentation = false;
    waitingForLabelSubmission = true;

    updateShapes();
    Q_EMIT triggerSegmentationCompleted();
    Q_EMIT triggerUpdateView();
  }
}

//---------------------------------------------------------------------------
void MonaiLabelJob::onSubmitLabelClicked() {
  if (waitingForLabelSubmission) {
    if (currentSampleId_.empty() || currentSegmentationPath_.empty()) {
      SW_ERROR("Upload sample and run segmentation before label submission!");
      return;
    }

    py::list label_info;
    py::dict entry;

    std::string image_in = currentSampleId_;
    std::string label_in = currentSegmentationPath_;

    entry[py::str("name")] = image_in;
    entry[py::str("idx")] = 1;
    label_info.append(entry);

    py::dict params;
    params[py::str("label_info")] = label_info;
    auto response = saveLabel(image_in, label_in, params);
    Q_EMIT triggerSubmitLabelCompleted();
    runSegmentationModel();
  }
}

//---------------------------------------------------------------------------
void MonaiLabelJob::updateShapes() {
  if (!currentSampleId_.empty() && !currentSegmentationPaths_.empty()) {
    auto shapes = session_->get_shapes();
    session_->get_project()->set_domain_names(organNames_);
    if (sample_number_ < shapes.size()) {
      auto cur_shape = shapes[sample_number_];
      auto cur_subject = cur_shape->get_subject();
      cur_subject->set_number_of_domains(currentSegmentationPaths_.size());
      cur_subject->set_original_filenames(currentSegmentationPaths_);
    }

  } else {
    SW_ERROR("Segmentation not done successfully!");
    return;
  }
}

}  // namespace monailabel

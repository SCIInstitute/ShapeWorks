#include <ShapeWorksMONAI/MonaiLabelJob.h>
#include <ShapeWorksMONAI/MonaiLabelUtils.h>

#include <string>
#include <Data/Session.h>
#include <Shape.h>
#include <Logging.h>
#include <Project/ProjectUtils.h>

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <QSharedPointer>
#include <QFileInfo>
#include "qdir.h"
#include <QString>

namespace py = pybind11;
using namespace pybind11::literals; //TODO:remove redundant includes

using namespace shapeworks;

namespace monailabel {

//---------------------------------------------------------------------------
MonaiLabelJob::MonaiLabelJob(QSharedPointer<Session> session,
                             const std::string &server_url,
                             const std::string &client_id,
                             const std::string &strategy,
                             const std::string &model_type)
    : session_(session),
      server_url_(server_url),
      client_id_(client_id),
      waitingForUpload(false),
      waitingForSegmentation(false),
      waitingForLabelSubmission(false),
      strategy_(strategy),
      model_type_(model_type),
      monai_client_(nullptr) {
  std::cout << "Initializing monai label job object with server url " << server_url << std::endl;
  project_ = session_->get_project();

  QDir projectDir(QString::fromStdString(session->get_project()->get_project_path()));
  QString labels_dir = projectDir.filePath("labels-prediction");
  if (MonaiLabelUtils::createDir(labels_dir)) {
    std::cout << "Tmp dir for MONAI Client set to " << tmp_dir_ << std::endl;
    tmp_dir_ = labels_dir.toStdString();
  }
  else std::cout << "tmp dir not created" << std::endl;
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
void MonaiLabelJob::setCurrentSampleNumber(int n) {
  std::cout << "Setting Current Sample number in MONAI Job " << n << std::endl;
  sample_number_ = n;
}

//---------------------------------------------------------------------------
void MonaiLabelJob::initializeClient() {
    std:: cout << "Initializing MONAI Client with server: {} tmp dir: {} client_id: {}" <<  server_url_ << tmp_dir_ << client_id_ ;


    try {
        py::module monai_label = py::module::import("MONAILabel");
        py::object monai_client_class = monai_label.attr("MONAILabelClient");

        // Convert C++ std::string to py::str before passing to Python
        py::str py_server_url(server_url_);
        py::str py_tmp_dir(tmp_dir_);
        py::str py_client_id(client_id_);

        monai_client_ = std::make_shared<py::object>(monai_client_class(py_server_url, py_tmp_dir, py_client_id));

        std::cout << "Monai client initialization success!" << std::endl;

        // Ensure the client was created successfully
        if (!monai_client_) {
            std::cerr << "Error in initializing MONAI client" << std::endl;
            return;
        }
        std::cout << "fetching model name" << std::endl;
        model_name_ = getModelName(model_type_);
        std::cout << "model name = " << model_name_ << std::endl;

    } catch (std::exception &e) {
        std::cerr << "Error importing MONAILabel or initializing MONAILabelClient: " << e.what() << std::endl;
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
  } catch (const py::error_already_set& e) {
    SW_ERROR("Python error: {}", e.what());
  } catch (const std::exception& e) {
    SW_ERROR("Exception occurred: {}", e.what());
  } catch (...) {
    SW_ERROR("An unknown error occurred!");
  }
  return response;
}

//---------------------------------------------------------------------------
std::string MonaiLabelJob::getModelName(std::string modelType) {
  py::str mappedSection = py::str("models");
  py::dict info = getInfo();
  std::string modelName = "";
  if (info.contains(mappedSection)) {
    py::dict sectionConfig =
        info[mappedSection].cast<py::dict>();  // models dict
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
py::dict MonaiLabelJob::getParamsFromConfig(std::string section,
                                                std::string name) {
  std::cout << "Fetching params for section = " << section << std::endl;
  py::dict info = getInfo();
  std::unordered_map<std::string, std::string> mapping = {
      {"infer", "models"},
      {"train", "trainers"},
      {"activelearning", "strategies"},
      {"scoring", "scoring"}};
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
              result[key] = py::str(valueList[0]);
            }
          } else {
            result[key] = py::str(value);
          }
        }
      }
    }
  }
  std::cout << "Result = \n " << py::repr(result).cast<std::string>() << std::endl;
  SW_DEBUG("Result from getting params from config \n {} ", py::repr(result).cast<std::string>());
  return result;
}

//---------------------------------------------------------------------------
py::dict MonaiLabelJob::nextSample(std::string strategy, py::dict params) {
  py::dict response;
  if (!monai_client_) {
    SW_ERROR("MONAI client not initialized yet");
    return response;
  }
  std::cout << "Fetching next sample from server " << std::endl;
  response = (*monai_client_).attr("next_sample")(strategy, params);
  std::cout << "Next sample response \n" << py::repr(response).cast<std::string>() << std::endl;
  return response;
}

//---------------------------------------------------------------------------
py::dict MonaiLabelJob::uploadImage(std::string image_in,
                                    std::string image_id) {
  py::dict response;

  try {
    if (!monai_client_) {
      SW_ERROR("MONAI client not initialized yet");
      return response;
    }
    SW_DEBUG("Uploading sample number {} to MONAI Label server",
             sample_number_);
    std::cout << "Uploading current sample to server " << std::endl;

    response =
        (*monai_client_)
            .attr("upload_image")(
                image_in, image_id.empty() ? py::none() : py::cast(image_id));
    std::cout << "Upload sample response \n"
              << py::repr(response).cast<std::string>() << std::endl;

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
py::tuple MonaiLabelJob::infer(std::string model, std::string image_in,
                               py::dict params, std::string label_in,
                               std::string file, std::string session_id) {
  py::tuple result = py::make_tuple();

  try {
    if (!monai_client_) {
      SW_ERROR("MONAI client not initialized yet");
      return result;
    }
    params[py::str("result_extension")] = ".nrrd";
    params[py::str("result_dtype")] = "uint8";
    result = (*monai_client_)
                 .attr("infer")(
                     model, image_in, params,
                     label_in.empty() ? py::none() : py::cast(label_in),
                     file.empty() ? py::none() : py::cast(file),
                     session_id.empty() ? py::none() : py::cast(session_id));
    std::cout << "Infer sample response \n"
              << py::repr(result).cast<std::string>() << std::endl;
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
py::dict MonaiLabelJob::saveLabel(std::string image_in, std::string label_in,
                                  py::dict params) {
  py::dict response;
  try {
    if (!monai_client_) {
      SW_ERROR("MONAI client not initialized yet");
      return response;
    }
    response =
        (*monai_client_).attr("save_label")(image_in, label_in, "", params);
    std::cout << "Save Label response \n"
              << py::repr(response).cast<std::string>() << std::endl;
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
void MonaiLabelJob::set_subject_image_filename(
    const std::shared_ptr<Subject> &subject, const std::string &filename) {
  if (!QFileInfo(QString::fromStdString(filename)).exists()) {
    SW_WARN("File doesn't exist: {}", filename);
  } else {
    shapeworks::project::types::StringMap map;
    map[get_feature_name()] = filename;
    subject->set_feature_filenames(map);
  }
}

//---------------------------------------------------------------------------
void MonaiLabelJob::runSegmentationModel() {
  if (!monai_client_) {
    initializeClient();
  }
  SW_DEBUG("Waiting for Upload Sample Action...");
  std::cout << "Waiting to upload sample..."<< std::endl;
  waitingForUpload = true;
  waitingForSegmentation = false;
  waitingForLabelSubmission = false;
}

void MonaiLabelJob::run() {
  runSegmentationModel();
}

//---------------------------------------------------------------------------
QString MonaiLabelJob::name() { return "MONAI Label"; }

//---------------------------------------------------------------------------
void MonaiLabelJob::python_message(std::string str) { 
  SW_LOG(str); 
}

//---------------------------------------------------------------------------
std::string MonaiLabelJob::get_feature_name() {
  std::string feature_name = session_->get_image_name();
  auto image_names = session_->get_project()->get_image_names();
  if (image_names.size() > 0) {
    feature_name = image_names[0];
  }
  std::cout << "Feature Name = " << feature_name << std::endl;
  return feature_name;
}

/*-----Q_SLOTS----*/

//---------------------------------------------------------------------------
void MonaiLabelJob::onUploadSampleClicked() {
  std::cout << "Upload Sample Clicked with sample num = " << sample_number_
            << std::endl;
  if (waitingForUpload) {
    std::cout << "getting params" << std::endl;
    py::dict params = getParamsFromConfig("activelearning", strategy_);
    auto subjects = session_->get_project()->get_subjects();
    auto shapes = session_->get_shapes();
    std::cout << "Getting current shape info " << std::endl;

    if (sample_number_ < shapes.size()) {
      auto cur_shape = shapes[sample_number_];
      auto cur_subject = cur_shape->get_subject();

      project::types::StringMap map = cur_subject->get_feature_filenames();
      std::string fp = map[get_feature_name()];
      QFileInfo imageFileInfo(QString::fromStdString(fp));
      QString absoluteImagePath = imageFileInfo.absoluteFilePath();
      QString subjectName = imageFileInfo.completeBaseName();
      QString extension = imageFileInfo.completeSuffix();

      std::cout << "Abs FP: " << absoluteImagePath.toStdString() << std::endl;
      std::cout << "Subject Name: " << subjectName.toStdString() << std::endl;
      std::cout << "Extension: " << extension.toStdString() << std::endl;

      py::dict response = uploadImage(absoluteImagePath.toStdString(), subjectName.toStdString());
      
      if (response[py::str("image")].cast<std::string>() == subjectName.toStdString()) {
        std::cout << "Sample uploaded successfully" << std::endl;
        currentSampleId_ = subjectName.toStdString();
      }
      else {
        std::cout << "upload failed" << std::endl;
      }
      waitingForSegmentation = true;
    }
    else {
      SW_ERROR("Current sample's index {} exceeds total number of samples in project {}", sample_number_, shapes.size());
      return;
    }

    Q_EMIT triggerUpdateView();
  }
}

//---------------------------------------------------------------------------
void MonaiLabelJob::onRunSegmentationClicked() {
  if (waitingForSegmentation) {
    if (currentSampleId_.empty()) {
      std::cout << "Current sample not fetched yet! Check logic!"
                     << std::endl;
      return;
    }
    std::cout << "Fetching params for infer..." << std::endl;

    py::dict params = getParamsFromConfig("infer", model_name_);

    py::tuple result =
        infer(model_name_, currentSampleId_, params, "", "", getSessionId());

    currentSegmentationPath_ =
        result[0].cast<std::string>();  // result for segmentation


    QDir projDir(QString::fromStdString(tmp_dir_));
    QString destPath = projDir.filePath(QString::fromStdString(currentSampleId_ + ".nrrd"));
    std::cout << "destPath = " << destPath.toStdString() << std::endl;
    bool res = MonaiLabelUtils::copySegmentation(QString::fromStdString(currentSegmentationPath_), destPath);


    currentSegmentationPath_ = destPath.toStdString();
    std::cout << "Label saved at " << currentSegmentationPath_ << std::endl;

    waitingForSegmentation = false;
    waitingForLabelSubmission = true;
    updateShapes();
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
    entry[py::str("idx")] = 1; // TODO: handle multi-organ label submission
    label_info.append(entry);
    py::dict params;
    params[py::str("label_info")] = label_info;
    auto response = saveLabel(image_in, label_in, params);
    runSegmentationModel();
  }
}

//---------------------------------------------------------------------------
void MonaiLabelJob::updateShapes() {
  if (!currentSampleId_.empty() && !currentSegmentationPath_.empty()) {
    auto shapes = session_->get_shapes();
    if (sample_number_ < shapes.size()) {
      auto cur_shape = shapes[sample_number_];
      auto cur_subject = cur_shape->get_subject();
      std::cout << "Setting Segmentation for subject " << sample_number_<< std::endl;  
      cur_subject->set_original_filenames({currentSegmentationPath_});
    }
  } else {
    SW_ERROR("Current Sample does not exist yet!");
    return;
  }
}

}  // namespace monailabel

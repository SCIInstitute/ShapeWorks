// pybind
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include "qdir.h"
namespace py = pybind11;
using namespace pybind11::literals;  // to bring in the `_a` literal

// std
#include <fstream>
#include <functional>
#include <iostream>
#include <random>

// qt
#include <QFile>
#include <QTextStream>
#include <QThread>

// shapeworks
#include <Data/Session.h>
#include <DeepSSM/DeepSSMJob.h>
#include <DeepSSM/DeepSSMParameters.h>
#include <Groom.h>
#include <Logging.h>
#include <Mesh/MeshUtils.h>
#include <Optimize.h>
#include <Optimize/OptimizeParameters.h>

namespace shapeworks {

//---------------------------------------------------------------------------
DeepSSMJob::DeepSSMJob(QSharedPointer<Session> session, DeepSSMTool::ToolMode tool_mode)
    : session_(session), tool_mode_(tool_mode) {
  project_ = session_->get_project();
}

//---------------------------------------------------------------------------
DeepSSMJob::~DeepSSMJob() {}

//---------------------------------------------------------------------------
void DeepSSMJob::run() {
  switch (tool_mode_) {
    case DeepSSMTool::ToolMode::DeepSSM_PrepType:
      run_prep();
      break;
    case DeepSSMTool::ToolMode::DeepSSM_AugmentationType:
      run_augmentation();
      break;
    case DeepSSMTool::ToolMode::DeepSSM_TrainingType:
      run_training();
      break;
    case DeepSSMTool::ToolMode::DeepSSM_TestingType:
      run_testing();
      break;
  }
}

//---------------------------------------------------------------------------
QString DeepSSMJob::name() {
  switch (tool_mode_) {
    case DeepSSMTool::ToolMode::DeepSSM_PrepType:
      return "DeepSSM: Prep";
      break;
    case DeepSSMTool::ToolMode::DeepSSM_AugmentationType:
      return "DeepSSM: Augmentation";
      break;
    case DeepSSMTool::ToolMode::DeepSSM_TrainingType:
      return "DeepSSM: Training";
      break;
    case DeepSSMTool::ToolMode::DeepSSM_TestingType:
      return "DeepSSM: Testing";
      break;
  }

  return "DeepSSM";
}

//---------------------------------------------------------------------------
void DeepSSMJob::run_prep() {
  // groom training
  auto subjects = project_->get_subjects();
  auto shapes = session_->get_shapes();
  SW_LOG("DeepSSM: Grooming Training Data");
  update_prep_message(PrepStep::GROOM_TRAINING);
  Q_EMIT progress(0);

  auto train_id_list = get_list(FileType::ID, SplitType::TRAIN);
  auto val_id_list = get_list(FileType::ID, SplitType::VAL);
  auto test_id_list = get_list(FileType::ID, SplitType::TEST);

  SW_LOG("Total subjects: {}", subjects.size());
  SW_LOG("Train subjects: {}", train_id_list.size());
  SW_LOG("Val subjects: {}", val_id_list.size());
  SW_LOG("Test subjects: {}", test_id_list.size());

  /////////////////////////////////////////////////////////
  // Step 1. Groom Training
  /////////////////////////////////////////////////////////

  // first, set all excluded
  for (auto &subject : subjects) {
    subject->set_excluded(true);
  }

  // include only training data
  std::vector<Mesh> meshes;
  for (int i = 0; i < train_id_list.size(); i++) {
    auto id = train_id_list[i];
    auto shape = shapes[std::stoi(id)];
    shape->get_subject()->set_excluded(false);
    meshes.push_back(shape->get_original_meshes(true).meshes()[0]->get_poly_data());
  }
  project_->update_subjects();
  session_->trigger_reinsert_shapes();

  // choose reference
  int reference = MeshUtils::findReferenceMesh(meshes, 20);
  SW_LOG("Reference shape: {}", reference);

  GroomParameters groom_params{project_};
  groom_params.set_alignment_reference(reference);
  groom_params.save_to_project();

  // run grooming
  Groom groom{project_};
  groom.run();

  update_prep_message(PrepStep::OPTIMIZE_TRAINING);
  Q_EMIT progress(25);

  /////////////////////////////////////////////////////////
  // Step 2. Optimize Training Shapes
  /////////////////////////////////////////////////////////

  OptimizeParameters optimize_params{project_};
  Optimize optimize;
  optimize_params.set_up_optimize(&optimize);
  optimize.Run();

  update_prep_message(PrepStep::GROOM_IMAGES);
  Q_EMIT progress(75);

  /////////////////////////////////////////////////////////
  // Step 3. Groom Images
  /////////////////////////////////////////////////////////

  // load reference image
  auto image_filenames = subjects[reference]->get_feature_filenames();
  std::string image_filename;
  if (!image_filenames.empty()) {
    image_filename = image_filenames.begin()->second;
  }
  if (image_filename.empty()) {
    SW_ERROR("No image found for reference shape");
    return;
  }

  // apply alignment transform
  auto transform = shapes[reference]->get_alignment();
  Image reference_image{image_filename};

  // convert to ITK transform
  // auto itk_transform = MeshUtils::convertToITKTransform(transform);

  // image.applyTransform(transform);
  //  save as reference_image.nrrd
  reference_image.write("deepssm/reference_image.nrrd");

  // for each training image
  for (int i = 0; i < train_id_list.size(); i++) {
    auto id = train_id_list[i];
    auto shape = shapes[std::stoi(id)];
    auto subject = shape->get_subject();

    // load image
    auto image_filenames = subject->get_feature_filenames();
    std::string image_filename;
    if (!image_filenames.empty()) {
      image_filename = image_filenames.begin()->second;
    }
    if (image_filename.empty()) {
      SW_ERROR("No image found for subject {}", id);
      return;
    }

    // apply alignment transform
    auto transform = shape->get_alignment();
    auto procrustes = shape->get_procrustes_transform();

    transform->Concatenate(procrustes);

    Image image{image_filename};

    // invert to use for image resampling
    transform->Inverse();

    auto itk_transform = shapeworks::convert_to_image_transform(transform);

    // debug
    // image.write("deepssm/train_images/before_" + id + ".nrrd");

    image.applyTransform(itk_transform, reference_image);

    // create output directory
    QDir dir("deepssm/train_images");
    if (!dir.exists()) {
      dir.mkpath(".");
    }

    // save as image.nrrd
    image.write("deepssm/train_images/" + id + ".nrrd");
  }

  update_prep_message(PrepStep::DONE);
  Q_EMIT progress(100);
}

//---------------------------------------------------------------------------
void DeepSSMJob::run_augmentation() {
  auto train_img_list = get_list(FileType::GROOMED_IMAGE, SplitType::TRAIN);
  auto train_pts = get_list(FileType::PARTICLES, SplitType::TRAIN);

  py::list train_img_list_py = py::cast(train_img_list);
  py::list train_pts_py = py::cast(train_pts);

  DeepSSMParameters params(project_);

  QString sampler_type = QString::fromStdString(params.get_aug_sampler_type()).toLower();
  py::module py_data_aug = py::module::import("DataAugmentationUtils");

  py::object run_aug = py_data_aug.attr("runDataAugmentation");
  std::string aug_dir = "deepssm/Augmentation/";
  int aug_dims =
      run_aug(aug_dir, train_img_list_py, train_pts_py, params.get_aug_num_samples(), params.get_aug_num_dims(),
              params.get_aug_percent_variability(), sampler_type.toStdString(), 0, /* mixture_num */
              QThread::idealThreadCount()                                          /* processes */
              // 1   /* processes */
              )
          .cast<int>();

  params.set_training_num_dims(aug_dims);
  params.save_to_project();

  py::object vis_aug = py_data_aug.attr("visualizeAugmentation");
  vis_aug(aug_dir + "TotalData.csv", "violin", false);
}

//---------------------------------------------------------------------------
void DeepSSMJob::run_training() {
  auto subjects = project_->get_subjects();

  auto test_img_list = get_list(FileType::IMAGE, SplitType::TEST);

  DeepSSMParameters params(project_);

  py::module py_deep_ssm_utils = py::module::import("DeepSSMUtils");

  std::string out_dir = "deepssm/";
  std::string aug_dir = out_dir + "Augmentation/";
  std::string aug_data_csv = aug_dir + "TotalData.csv";

  double down_factor = 0.75;
  std::string down_dir = out_dir + "DownsampledImages/";
  int batch_size = params.get_training_batch_size();
  std::string loader_dir = out_dir + "TorchDataLoaders/";

  int epochs = params.get_training_epochs();
  double learning_rate = params.get_training_learning_rate();
  bool decay_lr = params.get_training_decay_learning_rate();
  bool fine_tune = params.get_training_fine_tuning();
  int fine_tune_epochs = params.get_training_fine_tuning_epochs();
  double fine_tune_learning_rate = params.get_training_fine_tuning_learning_rate();
  int num_dims = params.get_training_num_dims();

  double train_split = (100.0 - params.get_validation_split()) / 100.0;

  SW_LOG("DeepSSM: Loading Train/Validation Loaders");
  py::object get_train_val_loaders = py_deep_ssm_utils.attr("getTrainValLoaders");
  get_train_val_loaders(loader_dir, aug_data_csv, batch_size, down_factor, down_dir, train_split);

  SW_LOG("DeepSSM: Loading Test Loader");
  py::object get_test_loader = py_deep_ssm_utils.attr("getTestLoader");
  get_test_loader(loader_dir, test_img_list, down_factor, down_dir);

  py::object prepare_config_file = py_deep_ssm_utils.attr("prepareConfigFile");
  SW_LOG("DeepSSM: Preparing Config File");
  std::string config_file = "deepssm/configuration.json";
  prepare_config_file(config_file, "model", num_dims, out_dir, loader_dir, aug_dir, epochs, learning_rate, decay_lr,
                      fine_tune, fine_tune_epochs, fine_tune_learning_rate);

  SW_LOG("DeepSSM: Training");
  py::object train_deep_ssm = py_deep_ssm_utils.attr("trainDeepSSM");
  train_deep_ssm(config_file);
}

//---------------------------------------------------------------------------
void DeepSSMJob::run_testing() {
  DeepSSMParameters params(project_);

  auto id_list = get_list(FileType::ID, SplitType::TEST);
  QFile file("deepssm/TorchDataLoaders/test_names.txt");
  if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
    QStringList list;
    for (auto &id : id_list) {
      QString item = "'" + QString::fromStdString(id) + "'";
      list << item;
    }

    QTextStream stream(&file);
    stream << "[";
    stream << list.join(", ");
    stream << "]";
  }

  py::module py_deep_ssm_utils = py::module::import("DeepSSMUtils");

  std::string config_file = "deepssm/configuration.json";
  SW_LOG("DeepSSM: Testing");
  py::object test_deep_ssm = py_deep_ssm_utils.attr("testDeepSSM");
  test_deep_ssm(config_file);
}

//---------------------------------------------------------------------------
void DeepSSMJob::python_message(std::string str) { SW_LOG(str); }

//---------------------------------------------------------------------------
std::vector<std::string> DeepSSMJob::get_list(FileType file_type, SplitType split_type) {
  auto subjects = project_->get_subjects();

  std::mt19937 rng{42};

  std::vector<int> ids;
  for (size_t i = 0; i < subjects.size(); i++) {
    ids.push_back(i);
  }

  std::shuffle(ids.begin(), ids.end(), rng);

  std::vector<std::string> list;

  DeepSSMParameters params(project_);

  double val_split = params.get_validation_split();
  double test_split = params.get_testing_split();
  double train_split = 100.0 - val_split - test_split;

  int start = 0;
  int end = subjects.size();
  if (split_type == SplitType::TRAIN) {
    end = subjects.size() * (train_split / 100.0);
  } else if (split_type == SplitType::VAL) {
    start = subjects.size() * (train_split / 100.0);
    end = subjects.size() * ((train_split + val_split) / 100.0);
  } else {
    start = subjects.size() * ((train_split + val_split) / 100.0);
  }

  for (int i = start; i < end; i++) {
    auto id = ids[i];
    if (file_type == FileType::ID) {
      list.push_back(std::to_string(id));
    } else if (file_type == FileType::GROOMED_IMAGE) {
      list.push_back("deepssm/train_images/" + std::to_string(id) + ".nrrd";
    } else if (file_type == FileType::IMAGE) {
      auto image_filenames = subjects[id]->get_feature_filenames();
      if (!image_filenames.empty()) {
        list.push_back(image_filenames.begin()->second);
      }
    } else {
      auto particle_filenames = subjects[id]->get_world_particle_filenames();
      if (!particle_filenames.empty()) {
        list.push_back(particle_filenames[0]);
      }
    }
  }
  return list;
}

//---------------------------------------------------------------------------
QString DeepSSMJob::get_prep_message() {
  std::lock_guard<std::mutex> lock(mutex_);
  return prep_message_;
}

//---------------------------------------------------------------------------
void DeepSSMJob::update_prep_message(PrepStep step) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto message = [=](PrepStep current_step, PrepStep step) -> QString {
    if (step == current_step) {
      return "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>Running</td>";
    } else if (step < current_step) {
      return "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>Done</td>";
    } else {
      return "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>Waiting</td>";
    }
  };

  prep_message_ =
      "<html><table border=\"0\">"
      "<tr><td>Groom Training Data</td>" +
      message(step, PrepStep::GROOM_TRAINING) +
      "</tr>"
      "<tr><td>Optimize Training Data</td>" +
      message(step, PrepStep::OPTIMIZE_TRAINING) + "</tr>" + "<tr><td>Groom Images</td>" +
      message(step, PrepStep::GROOM_IMAGES) + "</tr></table></html>";
}
}  // namespace shapeworks

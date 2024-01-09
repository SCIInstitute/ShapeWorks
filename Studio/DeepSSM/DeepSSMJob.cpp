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

  py::module py_deep_ssm_utils = py::module::import("DeepSSMUtils");

  DeepSSMParameters params(project_);

  /////////////////////////////////////////////////////////
  /// Step 1. Create Split
  /////////////////////////////////////////////////////////
  double val_split = params.get_validation_split();
  double test_split = params.get_testing_split();
  double train_split = 100.0 - val_split - test_split;
  py::object create_split = py_deep_ssm_utils.attr("create_split");
  create_split(project_, train_split, val_split, test_split);

  if (is_aborted()) {
    return;
  }

  /////////////////////////////////////////////////////////
  /// Step 2. Groom Training Shapes
  /////////////////////////////////////////////////////////
  update_prep_message(PrepStep::GROOM_TRAINING);
  py::object groom_training_shapes = py_deep_ssm_utils.attr("groom_training_shapes");

  // force ICP on to get reference
  GroomParameters groom_params{project_};
  groom_params.set_alignment_method("Iterative Closest Point");
  groom_params.set_alignment_enabled(true);
  groom_params.save_to_project();

  groom_training_shapes(project_);
  project_->save();

  if (is_aborted()) {
    return;
  }

  /////////////////////////////////////////////////////////
  /// Step 3. Optimize Training Particles
  /////////////////////////////////////////////////////////
  update_prep_message(PrepStep::OPTIMIZE_TRAINING);
  py::object optimize_training_particles = py_deep_ssm_utils.attr("optimize_training_particles");
  optimize_training_particles(project_);
  project_->save();

  if (is_aborted()) {
    return;
  }
  /////////////////////////////////////////////////////////
  /// Step 4. Groom Training Images
  /////////////////////////////////////////////////////////

  update_prep_message(PrepStep::GROOM_TRAINING_IMAGES);
  py::object groom_training_images = py_deep_ssm_utils.attr("groom_training_images");
  groom_training_images(project_);
  project_->save();

  if (is_aborted()) {
    return;
  }
  /////////////////////////////////////////////////////////
  /// Step 5. Groom Validation and Test Images
  /////////////////////////////////////////////////////////
  update_prep_message(PrepStep::GROOM_VAL_AND_TEST_IMAGES);
  py::object groom_val_test_images = py_deep_ssm_utils.attr("groom_val_test_images");
  groom_val_test_images(project_);
  project_->save();

  if (is_aborted()) {
    return;
  }
  /////////////////////////////////////////////////////////
  /// Step 6. Optimize Validation Particles with Fixed Domains
  /////////////////////////////////////////////////////////
  update_prep_message(PrepStep::OPTIMIZE_VALIDATION);

  // DeepSSMUtils.prep_project_for_val_particles(project)
  py::object prep_project_for_val_particles = py_deep_ssm_utils.attr("prep_project_for_val_particles");
  prep_project_for_val_particles(project_);

  // DeepSSMUtils.groom_validation_shapes(project)
  py::object groom_validation_shapes = py_deep_ssm_utils.attr("groom_validation_shapes");
  groom_validation_shapes(project_);

  // run optimize
  Optimize optimize;
  optimize.SetUpOptimize(project_);
  optimize.Run();

  project_->update_subjects();
  project_->save();

  /////////////////////////////////////////////////////////
  update_prep_message(PrepStep::DONE);
  Q_EMIT progress(100);
}

//---------------------------------------------------------------------------
void DeepSSMJob::run_augmentation() {
  DeepSSMParameters params(project_);

  QString sampler_type = QString::fromStdString(params.get_aug_sampler_type()).toLower();

  py::module py_deep_ssm_utils = py::module::import("DeepSSMUtils");
  py::object run_data_aug = py_deep_ssm_utils.attr("run_data_augmentation");

  int aug_dims = run_data_aug(project_, params.get_aug_num_samples(), params.get_aug_num_dims(),
                              params.get_aug_percent_variability(), sampler_type.toStdString(), 0, /* mixture_num */
                              QThread::idealThreadCount()                                          /* processes */
                              // 1   /* processes */
                              )
                     .cast<int>();

  params.set_training_num_dims(aug_dims);
  params.save_to_project();

  py::module py_data_aug = py::module::import("DataAugmentationUtils");
  py::object vis_aug = py_data_aug.attr("visualizeAugmentation");

  std::string aug_dir = "deepssm/augmentation/";
  vis_aug(aug_dir + "TotalData.csv", "violin", false);
}

//---------------------------------------------------------------------------
void DeepSSMJob::run_training() {
  auto subjects = project_->get_subjects();

  DeepSSMParameters params(project_);
  int batch_size = params.get_training_batch_size();

  py::module py_deep_ssm_utils = py::module::import("DeepSSMUtils");

  py::object prepare_data_loaders = py_deep_ssm_utils.attr("prepare_data_loaders");
  prepare_data_loaders(project_, batch_size);

  std::string out_dir = "deepssm/";
  std::string aug_dir = out_dir + "augmentation/";
  std::string aug_data_csv = aug_dir + "TotalData.csv";

  std::string loader_dir = out_dir + "torch_loaders/";

  int epochs = params.get_training_epochs();
  double learning_rate = params.get_training_learning_rate();
  bool decay_lr = params.get_training_decay_learning_rate();
  bool fine_tune = params.get_training_fine_tuning();
  int fine_tune_epochs = params.get_training_fine_tuning_epochs();
  double fine_tune_learning_rate = params.get_training_fine_tuning_learning_rate();
  int num_dims = params.get_training_num_dims();

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

  std::vector<std::string> test_indices = get_list(FileType::ID, SplitType::TEST);

  QFile file("deepssm/torch_loaders/test_names.txt");
  if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
    QStringList list;
    for (auto &id : test_indices) {
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

  std::vector<std::string> list;

  for (int id = 0; id < subjects.size(); id++) {
    auto extra_values = subjects[id]->get_extra_values();

    std::string split = extra_values["split"];

    if (split_type == SplitType::TRAIN) {
      if (split != "train") {
        continue;
      }
    } else if (split_type == SplitType::VAL) {
      if (split != "val") {
        continue;
      }
    } else if (split_type == SplitType::TEST) {
      if (split != "test") {
        continue;
      }
    }

    if (file_type == FileType::ID) {
      list.push_back(std::to_string(id));
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

  QString m = "<html><table border=\"0\">";

  m = m + "<tr><td>Groom Training Shapes</td>" + message(step, PrepStep::GROOM_TRAINING) + "</tr>";
  m = m + "<tr><td>Optimize Training Particles</td>" + message(step, PrepStep::OPTIMIZE_TRAINING) + "</tr>";
  m = m + "<tr><td>Groom Training Images</td>" + message(step, PrepStep::GROOM_TRAINING_IMAGES) + "</tr>";
  m = m + "<tr><td>Groom Validation and Test Images</td>" + message(step, PrepStep::GROOM_VAL_AND_TEST_IMAGES) +
      "</tr>";
  m = m + "<tr><td>Optimize Validation Particles</td>" + message(step, PrepStep::OPTIMIZE_VALIDATION) + "</tr>";
  m = m + "</tr></table></html>";

  prep_message_ = m;
  Q_EMIT progress(-1);
}
}  // namespace shapeworks

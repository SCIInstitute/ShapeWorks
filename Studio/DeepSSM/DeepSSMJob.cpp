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
  try {
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
  } catch (const std::exception& e) {
    SW_ERROR("Exception Caught: {}", e.what());
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
  params.set_prep_step_complete(false);
  params.set_aug_step_complete(false);
  params.set_training_step_complete(false);
  params.set_prep_message("");
  params.save_to_project();

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

  QElapsedTimer timer;
  timer.start();

  // force ICP on to get reference
  GroomParameters groom_params{project_};
  groom_params.set_alignment_method("Iterative Closest Point");
  groom_params.set_alignment_enabled(true);
  groom_params.save_to_project();

  groom_training_shapes(project_);
  project_->save();

  QString duration = QString::number(timer.elapsed() / 1000.0, 'f', 1);
  SW_LOG("DeepSSM: Grooming Training Shapes complete.  Duration: {} seconds", duration.toStdString());

  if (is_aborted()) {
    return;
  }

  /////////////////////////////////////////////////////////
  /// Step 3. Optimize Training Particles
  /////////////////////////////////////////////////////////
  update_prep_message(PrepStep::OPTIMIZE_TRAINING);
  timer.start();
  py::object optimize_training_particles = py_deep_ssm_utils.attr("optimize_training_particles");
  optimize_training_particles(project_);
  project_->save();
  duration = QString::number(timer.elapsed() / 1000.0, 'f', 1);
  SW_LOG("DeepSSM: Optimize Training Particles complete.  Duration: {} seconds", duration.toStdString());

  if (is_aborted()) {
    return;
  }
  /////////////////////////////////////////////////////////
  /// Step 4. Groom Training Images
  /////////////////////////////////////////////////////////

  update_prep_message(PrepStep::GROOM_TRAINING_IMAGES);
  timer.start();
  py::object groom_training_images = py_deep_ssm_utils.attr("groom_training_images");
  groom_training_images(project_);
  project_->save();
  duration = QString::number(timer.elapsed() / 1000.0, 'f', 1);
  SW_LOG("DeepSSM: Groom Training Images complete.  Duration: {} seconds", duration.toStdString());

  if (is_aborted()) {
    return;
  }
  /////////////////////////////////////////////////////////
  /// Step 5. Groom Validation Images
  /////////////////////////////////////////////////////////
  update_prep_message(PrepStep::GROOM_VAL_IMAGES);
  timer.start();
  py::object groom_val_test_images = py_deep_ssm_utils.attr("groom_val_test_images");
  groom_val_test_images(project_, DeepSSMTool::get_split(project_, DeepSSMTool::SplitType::VAL));
  project_->save();
  duration = QString::number(timer.elapsed() / 1000.0, 'f', 1);
  SW_LOG("DeepSSM: Groom Validation Images complete.  Duration: {} seconds", duration.toStdString());

  if (is_aborted()) {
    return;
  }
  /////////////////////////////////////////////////////////
  /// Step 6. Optimize Validation Particles with Fixed Domains
  /////////////////////////////////////////////////////////
  update_prep_message(PrepStep::OPTIMIZE_VALIDATION);
  py::object prep_project_for_val_particles = py_deep_ssm_utils.attr("prep_project_for_val_particles");
  prep_project_for_val_particles(project_);

  timer.start();
  py::object groom_validation_shapes = py_deep_ssm_utils.attr("groom_validation_shapes");
  groom_validation_shapes(project_);
  duration = QString::number(timer.elapsed() / 1000.0, 'f', 1);
  SW_LOG("DeepSSM: Groom Validation Shapes complete.  Duration: {} seconds", duration.toStdString());

  timer.start();

  // run optimize
  Optimize optimize;
  optimize.SetUpOptimize(project_);
  optimize.Run();

  project_->update_subjects();
  project_->save();
  duration = QString::number(timer.elapsed() / 1000.0, 'f', 1);
  SW_LOG("DeepSSM: Optimize Validation Particles complete.  Duration: {} seconds", duration.toStdString());

  /////////////////////////////////////////////////////////
  update_prep_message(PrepStep::DONE);
  params.set_prep_step_complete(true);
  params.set_aug_step_complete(false);
  params.set_training_step_complete(false);

  params.set_prep_message(prep_message_.toStdString());
  params.save_to_project();

  Q_EMIT progress(100);
}

//---------------------------------------------------------------------------
void DeepSSMJob::run_augmentation() {
  DeepSSMParameters params(project_);
  params.set_training_step_complete(false);
  params.save_to_project();

  QString sampler_type = QString::fromStdString(params.get_aug_sampler_type()).toLower();

  py::module py_deep_ssm_utils = py::module::import("DeepSSMUtils");
  py::object run_data_aug = py_deep_ssm_utils.attr("run_data_augmentation");

  int aug_dims = run_data_aug(project_, params.get_aug_num_samples(),
                              0 /* num dims, set to zero to allow percent variability to be used */,
                              params.get_aug_percent_variability(), sampler_type.toStdString(), 0 /* mixture_num */,
                              QThread::idealThreadCount() /* processes */
                              )
                     .cast<int>();

  params.set_training_num_dims(aug_dims);
  params.save_to_project();

  py::module py_data_aug = py::module::import("DataAugmentationUtils");
  py::object vis_aug = py_data_aug.attr("visualizeAugmentation");

  std::string aug_dir = "deepssm/augmentation/";
  vis_aug(aug_dir + "TotalData.csv", "violin", false);
  if (!is_aborted()) {
    params.set_aug_step_complete(true);
  }
  params.save_to_project();
}

//---------------------------------------------------------------------------
void DeepSSMJob::run_training() {
  auto subjects = project_->get_subjects();

  DeepSSMParameters params(project_);
  params.set_training_step_complete(false);
  params.save_to_project();

  int batch_size = params.get_training_batch_size();

  py::module py_deep_ssm_utils = py::module::import("DeepSSMUtils");

  py::object prepare_data_loaders = py_deep_ssm_utils.attr("prepare_data_loaders");
  prepare_data_loaders(project_, batch_size, "train");
  prepare_data_loaders(project_, batch_size, "val");

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

  py::object prepare_config_file = py_deep_ssm_utils.attr("prepare_config_file");
  SW_LOG("DeepSSM: Preparing Config File");
  std::string config_file = "deepssm/configuration.json";
  prepare_config_file(config_file, "model", num_dims, out_dir, loader_dir, aug_dir, epochs, learning_rate, decay_lr,
                      fine_tune, fine_tune_epochs, fine_tune_learning_rate, params.get_loss_function(),
                      params.get_tl_net_enabled(), params.get_tl_net_ae_epochs(), params.get_tl_net_tf_epochs(),
                      params.get_tl_net_joint_epochs(), params.get_tl_net_alpha(), params.get_tl_net_a_ae(),
                      params.get_tl_net_c_ae(), params.get_tl_net_a_lat(), params.get_tl_net_c_lat());

  SW_LOG("DeepSSM: Training");
  py::object train_deep_ssm = py_deep_ssm_utils.attr("trainDeepSSM");
  train_deep_ssm(config_file);

  if (!is_aborted()) {
    params.set_training_step_complete(true);
    params.save_to_project();
  }
}

//---------------------------------------------------------------------------
void DeepSSMJob::run_testing() {
  DeepSSMParameters params(project_);

  py::module py_deep_ssm_utils = py::module::import("DeepSSMUtils");

  std::vector<int> test_indices = DeepSSMTool::get_split(project_, DeepSSMTool::SplitType::TEST);

  // Groom Test Images
  SW_MESSAGE("Grooming Test Images");
  py::object groom_val_test_images = py_deep_ssm_utils.attr("groom_val_test_images");
  groom_val_test_images(project_, test_indices);
  project_->save();

  // Prepare Test Data Loaders
  SW_MESSAGE("Preparing Test Data Loaders");
  py::object prepare_data_loaders = py_deep_ssm_utils.attr("prepare_data_loaders");
  int batch_size = params.get_training_batch_size();
  prepare_data_loaders(project_, batch_size, "test");

  QFile file("deepssm/torch_loaders/test_names.txt");
  if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
    QStringList list;
    for (auto& id : test_indices) {
      QString item = "'" + QString::number(id) + "'";
      list << item;
    }

    QTextStream stream(&file);
    stream << "[";
    stream << list.join(", ");
    stream << "]";
  }

  std::string config_file = "deepssm/configuration.json";
  SW_LOG("DeepSSM: Testing");
  py::object test_deep_ssm = py_deep_ssm_utils.attr("testDeepSSM");
  test_deep_ssm(config_file);
}

//---------------------------------------------------------------------------
void DeepSSMJob::python_message(std::string str) { SW_LOG(str); }

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
  m = m + "<tr><td>Groom Validation Images</td>" + message(step, PrepStep::GROOM_VAL_IMAGES) + "</tr>";
  m = m + "<tr><td>Optimize Validation Particles</td>" + message(step, PrepStep::OPTIMIZE_VALIDATION) + "</tr>";
  m = m + "</tr></table></html>";

  prep_message_ = m;
  Q_EMIT progress(-1);
}
}  // namespace shapeworks

// pybind
#include <pybind11/embed.h>
#include <pybind11/stl.h>
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
#include <DeepSSM/DeepSSMJob.h>
#include <DeepSSM/DeepSSMParameters.h>
#include <Logging.h>

namespace shapeworks {

//---------------------------------------------------------------------------
DeepSSMJob::DeepSSMJob(ProjectHandle project, DeepSSMTool::ToolMode tool_mode)
    : project_(project), tool_mode_(tool_mode) {}

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
  SW_LOG("DeepSSM: Grooming Training Data");
  update_prep_message(PrepStep::GROOM_TRAINING);
  Q_EMIT progress(25);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  update_prep_message(PrepStep::OPTIMIZE_TRAINING);
  Q_EMIT progress(50);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  update_prep_message(PrepStep::NEXT);
  Q_EMIT progress(75);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  update_prep_message(PrepStep::DONE);
  Q_EMIT progress(100);
}

//---------------------------------------------------------------------------
void DeepSSMJob::run_augmentation() {
  auto train_img_list = get_list(FileType::IMAGE, SplitType::TRAIN);
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

  // auto train_img_list = get_list(FileType::IMAGE, SplitType::TRAIN);
  // auto train_pts = get_list(FileType::PARTICLES, SplitType::TRAIN);
  auto test_img_list = get_list(FileType::IMAGE, SplitType::TEST);

  // py::list train_img_list_py = py::cast(train_img_list);
  // py::list train_pts_py = py::cast(train_pts);

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

  int start = 0;
  int end = subjects.size() * (100.0 - params.get_testing_split()) / 100.0;
  if (split_type == SplitType::TEST) {
    start = end;
    end = subjects.size();
  }

  for (int i = start; i < end; i++) {
    auto id = ids[i];
    if (file_type == FileType::ID) {
      list.push_back(std::to_string(id));
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
      "<tr><td>Grooming Training Data</td>" +
      message(step, PrepStep::GROOM_TRAINING) +
      "</tr>"
      "<tr><td>Optimizing Training Data</td>" +
      message(step, PrepStep::OPTIMIZE_TRAINING) + "</tr>" + "<tr><td>Next</td>" +
      message(step, PrepStep::NEXT) + "</tr></table></html>";
}
}  // namespace shapeworks

// pybind
#include <pybind11/embed.h>
#include <pybind11/stl.h>
namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal

// std
#include <functional>
#include <iostream>
#include <fstream>
#include <random>

// qt
#include <QThread>

// shapeworks
#include <DeepSSM/QDeepSSM.h>
#include <DeepSSM/DeepSSMParameters.h>

namespace shapeworks {

//---------------------------------------------------------------------------
void QDeepSSM::update_progress()
{
  //emit progress(static_cast<int>(this->progress_));
}

//---------------------------------------------------------------------------
QDeepSSM::QDeepSSM(ProjectHandle project)
{
  this->project_ = project;
}

//---------------------------------------------------------------------------
QDeepSSM::~QDeepSSM()
{
}

//---------------------------------------------------------------------------
void QDeepSSM::run_augmentation()
{

  try {

    auto train_img_list = this->get_list(FileType::IMAGE, SplitType::TRAIN);
    auto train_pts = this->get_list(FileType::PARTICLES, SplitType::TRAIN);

    py::list train_img_list_py = py::cast(train_img_list);
    py::list train_pts_py = py::cast(train_pts);

    DeepSSMParameters params(this->project_);

    QString sampler_type = QString::fromStdString(params.get_aug_sampler_type()).toLower();
    py::module py_data_aug = py::module::import("DataAugmentationUtils");

    py::object run_aug = py_data_aug.attr("runDataAugmentation");
    std::string aug_dir = "deepssm/Augmentation/";
    run_aug(aug_dir, train_img_list_py, train_pts_py, params.get_aug_num_samples(),
            params.get_aug_num_dims(), params.get_aug_percent_variability(),
            sampler_type.toStdString(),
            0, /* mixture_num */
      //QThread::idealThreadCount(), /* processes */
            1, /* processes */
            nullptr /* world point list? */);

    py::object vis_aug = py_data_aug.attr("visualizeAugmentation");
    vis_aug(aug_dir + "TotalData.csv", "violin", false);

  } catch (py::error_already_set& e) {
    emit error(e.what());
  }

}

//---------------------------------------------------------------------------
void QDeepSSM::run_training()
{

  try {

    auto subjects = this->project_->get_subjects();

    auto train_img_list = this->get_list(FileType::IMAGE, SplitType::TRAIN);
    auto train_pts = this->get_list(FileType::PARTICLES, SplitType::TRAIN);
    auto test_img_list = this->get_list(FileType::IMAGE, SplitType::VALIDATION);

    py::list train_img_list_py = py::cast(train_img_list);
    py::list train_pts_py = py::cast(train_pts);

    DeepSSMParameters params(this->project_);

    py::module py_deep_ssm_utils = py::module::import("DeepSSMUtils");

    std::string out_dir = "deepssm/";
    std::string aug_dir = out_dir + "Augmentation/";
    std::string aug_data_csv = aug_dir + "TotalData.csv";

    double down_factor = 0.75;
    std::string down_dir = out_dir + "DownsampledImages/";
    int batch_size = 8;
    std::string loader_dir = out_dir + "TorchDataLoaders/";

    int epochs = params.get_training_epochs();
    double learning_rate = params.get_training_learning_rate();
    bool decay_lr = params.get_training_decay_learning_rate();
    bool fine_tune = params.get_training_fine_tuning();

    emit message("DeepSSM: Loading Train/Validation Loaders");
    py::object get_train_val_loaders = py_deep_ssm_utils.attr("getTrainValLoaders");
    get_train_val_loaders(loader_dir, aug_data_csv, batch_size, down_factor, down_dir);

    emit message("DeepSSM: Loading Test Loader");
    py::object get_test_loader = py_deep_ssm_utils.attr("getTestLoader");
    get_test_loader(loader_dir, test_img_list, down_factor, down_dir);

    py::object prepare_config_file = py_deep_ssm_utils.attr("prepareConfigFile");

    emit message("DeepSSM: Preparing Config File");
    std::string config_file = "deepssm/configuration.json";
    prepare_config_file(config_file, "model",
                        3 /* TODO: Replace this with output from augmentation*/,
                        out_dir, loader_dir, aug_dir, epochs, learning_rate, decay_lr, fine_tune);

    emit message("DeepSSM: Training");
    py::object train_deep_ssm = py_deep_ssm_utils.attr("trainDeepSSM");
    train_deep_ssm(config_file);

  } catch (py::error_already_set& e) {
    emit error(e.what());
  }
}

//---------------------------------------------------------------------------
void QDeepSSM::run_inference()
{

}

//---------------------------------------------------------------------------
void QDeepSSM::python_message(std::string str)
{
  emit message(QString::fromStdString(str));
}

//---------------------------------------------------------------------------
std::vector<std::string> QDeepSSM::get_list(FileType file_type, SplitType split_type)
{
  auto subjects = this->project_->get_subjects();

  std::mt19937 rng{42};


  std::vector<int> ids;
  for (size_t i = 0; i < subjects.size(); i++) {
    ids.push_back(i);
  }

  std::shuffle(ids.begin(), ids.end(), rng);

  std::vector<std::string> list;

  DeepSSMParameters params(this->project_);

  int start = 0;
  int end = subjects.size() * params.get_training_split();
  if (split_type == SplitType::VALIDATION) {
    start = end;
    end = subjects.size() * (params.get_training_split() + params.get_validation_split());
  } else if (split_type == SplitType::TEST) {
    start = subjects.size() * (params.get_training_split() + params.get_validation_split());
    end = subjects.size();
  }

  for (int i = start; i < end; i++) {
    auto id = ids[i];
    if (file_type == FileType::IMAGE) {
      auto image_filenames = subjects[id]->get_image_filenames();
      if (!image_filenames.empty()) {
        list.push_back(image_filenames[0]);
      }
    }
    else {
      auto particle_filenames = subjects[id]->get_local_particle_filenames();
      if (!particle_filenames.empty()) {
        list.push_back(particle_filenames[0]);
      }
    }
  }
  return list;
}

}

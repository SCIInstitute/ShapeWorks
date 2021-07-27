// pybind
#include <pybind11/embed.h>
#include <pybind11/stl.h>
namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal

#include <functional>
#include <iostream>
#include <fstream>

#include <QThread>

#include <DeepSSM/QDeepSSM.h>
#include <DeepSSM/DeepSSMParameters.h>


namespace shapeworks {

class Logger {
public:

  void set_callback(const std::function<void(std::string)>& callback)
  {
    this->callback_ = callback;
  }

  void cpp_log(std::string msg)
  {
    this->callback_(msg);
  }

private:
  std::function<void(std::string)> callback_;
};

PYBIND11_EMBEDDED_MODULE(logger, m)
{
  py::class_<Logger, std::shared_ptr<Logger>>(m, "Logger")
    .def(py::init<>())
    .def("log", &Logger::cpp_log);
};

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
    this->initialize_python();

    auto train_img_list = this->get_list(FileType::IMAGE, SplitType::TRAIN);
    auto train_pts = this->get_list(FileType::PARTICLES, SplitType::TRAIN);

    Logger logger_object = Logger();
    logger_object.set_callback(std::bind(&QDeepSSM::python_message, this, std::placeholders::_1));
    py::module logger = py::module::import("logger");

    py::list train_img_list_py = py::cast(train_img_list);
    py::list train_pts_py = py::cast(train_pts);

    DeepSSMParameters params(this->project_);

    QString sampler_type = QString::fromStdString(params.get_aug_sampler_type()).toLower();
    py::module py_data_aug = py::module::import("DataAugmentationUtils");

    py::object set_logger = py_data_aug.attr("set_logger");
    set_logger(logger_object);

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
    this->initialize_python();

    auto subjects = this->project_->get_subjects();

    auto train_img_list = this->get_list(FileType::IMAGE, SplitType::TRAIN);
    auto train_pts = this->get_list(FileType::PARTICLES, SplitType::TRAIN);
    auto test_img_list = this->get_list(FileType::IMAGE, SplitType::TEST);

    Logger logger_object = Logger();
    logger_object.set_callback(std::bind(&QDeepSSM::python_message, this, std::placeholders::_1));
    py::module logger = py::module::import("logger");

    py::list train_img_list_py = py::cast(train_img_list);
    py::list train_pts_py = py::cast(train_pts);

    DeepSSMParameters params(this->project_);

    py::module py_deep_ssm_utils = py::module::import("DeepSSMUtils");

    //py::object set_logger = py_data_aug.attr("set_logger");
    //set_logger(logger_object);


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
    // prepareConfigFile(config_file, model_name, embedded_dim, out_dir, loader_dir, aug_dir, epochs, learning_rate,
    //    decay_lr, fine_tune):
    std::string config_file = "deepssm/configuration.json";
    prepare_config_file(config_file, "deepssm",
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
void QDeepSSM::initialize_python()
{
  static bool python_initialized = false;
  if (!python_initialized) {
    py::initialize_interpreter();

    py::module sys = py::module::import("sys");

    // read list generated by something like this:
    // python -c "import sys; print('\n'.join(sys.path))" > $HOME/.shapeworks/python_path.txt

    std::vector<std::string> python_path;
    std::string home = getenv("HOME");
#ifdef _WIN32
    home = getenv("USERPROFILE");
#endif

#ifdef __APPLE__
    setenv("OMP_NUM_THREADS", "1", 1);
#endif

    std::fstream file;
    file.open(home + "/.shapeworks/python_path.txt", std::ios::in);
    if (file.is_open()) {
      std::string tp;
      while (getline(file, tp)) {
        python_path.push_back(tp);
      }
      file.close();
    }

    sys.attr("path") = python_path;

    // this is necessary or the plots will crash the process
    py::module py_matplot_lib = py::module::import("matplotlib");
    py_matplot_lib.attr("use")("agg");
    py::gil_scoped_release release;
  }
  python_initialized = true;
}

//---------------------------------------------------------------------------
void QDeepSSM::finalize_python()
{
  if (this->python_initialized_) {
    py::finalize_interpreter();
  }
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

  std::vector<std::string> list;

  int partition_number = subjects.size() * partition;

  int start = 0;
  int end = partition_number;
  if (split_type == SplitType::TEST) {
    start = partition_number;
    end = subjects.size();
  }

  for (int i = start; i < end; i++) {
    if (file_type == FileType::IMAGE) {
      auto image_filenames = subjects[i]->get_image_filenames();
      if (!image_filenames.empty()) {
        list.push_back(image_filenames[0]);
      }
    }
    else {
      auto particle_filenames = subjects[i]->get_local_particle_filenames();
      if (!particle_filenames.empty()) {
        list.push_back(particle_filenames[0]);
      }

    }
  }
  return list;
}

}
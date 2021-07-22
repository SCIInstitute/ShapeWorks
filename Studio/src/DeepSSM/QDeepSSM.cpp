

// pybind
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <DeepSSM/DeepSSMParameters.h>

namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal

#include <QThread>

#include <DeepSSM/QDeepSSM.h>

#include <iostream>

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
void QDeepSSM::run()
{

  this->initialize_python();

  auto subjects = this->project_->get_subjects();

  std::vector<std::string> train_img_list;
  std::vector<std::string> train_pts;

  for (auto subject : subjects) {
    auto image_filenames = subject->get_image_filenames();
    if (!image_filenames.empty()) {
      train_img_list.push_back(image_filenames[0]);

    }
    auto particle_filenames = subject->get_local_particle_filenames();
    if (!particle_filenames.empty()) {
      train_pts.push_back(particle_filenames[0]);
    }
  }

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
  run_aug("deepssm/", train_img_list_py, train_pts_py, params.get_aug_num_samples(),
          params.get_aug_num_dims(), params.get_aug_percent_variability(),
          sampler_type.toStdString(),
          0, /* mixture_num */
          //QThread::idealThreadCount(), /* processes */
          1, /* processes */
          nullptr /* world point list? */);

  py::object vis_aug = py_data_aug.attr("visualizeAugmentation");
  vis_aug("deepssm/TotalData.csv", "violin", false);

}

//---------------------------------------------------------------------------
void QDeepSSM::initialize_python()
{
  static bool python_initialized = false;
  if (!python_initialized) {
    py::initialize_interpreter();
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

}
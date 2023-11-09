#include <Common/Logging.h>
#include <Libs/Project/Project.h>
#include <Python/PythonWorker.h>
#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

// eigen format
#include <Eigen/Dense>

#include "qt/QtGui/qimage.h"

namespace py = pybind11;
using namespace pybind11::literals;  // to bring in the `_a` literal

#include <Job/ShapeScalarJob.h>

namespace shapeworks {

//---------------------------------------------------------------------------
ShapeScalarJob::ShapeScalarJob(QSharedPointer<Session> session, QString target_feature,
                               Eigen::MatrixXd target_particles, JobType job_type)
    : session_(session), target_feature_(target_feature), target_particles_(target_particles), job_type_(job_type) {}

//---------------------------------------------------------------------------
void ShapeScalarJob::run() {
  SW_DEBUG("Running shape scalar job");

  try {
    prep_data();

    py::module np = py::module::import("numpy");
    py::object A = np.attr("array")(all_particles_);
    py::object B = np.attr("array")(all_scalars_);

    py::module sw = py::module::import("shapeworks");

    if (job_type_ == JobType::MSE_Plot) {
      // returns a tuple of (png_raw_bytes, y_pred, mse)
      using ResultType = std::tuple<py::array, Eigen::MatrixXd, double>;

      py::object run_mbpls = sw.attr("shape_scalars").attr("run_mbpls");
      ResultType result = run_mbpls(A, B, num_components_, num_folds_).cast<ResultType>();

      py::array png_raw_bytes = std::get<0>(result);
      Eigen::MatrixXd y_pred = std::get<1>(result);
      double mse = std::get<2>(result);

      // interpret png_raw_bytes as a QImage
      QImage image;
      image.loadFromData((const uchar*)png_raw_bytes.data(), png_raw_bytes.size(), "PNG");
      plot_ = QPixmap::fromImage(image);

      SW_LOG("mse = {}", mse);

    } else if (job_type_ == JobType::Predict) {
      py::object new_x = np.attr("array")(target_particles_.transpose());
      py::object run_prediction = sw.attr("shape_scalars").attr("pred_from_mbpls");

      using ResultType = Eigen::VectorXd;
      ResultType result = run_prediction(A, B, new_x).cast<ResultType>();

      auto y_pred = result;

      prediction_ = y_pred;
    } else if (job_type_ == JobType::Find_Components) {
      // returns a tuple of (png_raw_bytes, y_pred, mse)
      using ResultType = py::array;

      py::object run = sw.attr("shape_scalars").attr("run_find_num_components");
      ResultType result = run(A, B, max_components_, num_folds_).cast<ResultType>();

      py::array png_raw_bytes = result;

      // interpret png_raw_bytes as a QImage
      QImage image;
      image.loadFromData((const uchar*)png_raw_bytes.data(), png_raw_bytes.size(), "PNG");
      plot_ = QPixmap::fromImage(image);
    }
    SW_DEBUG("End shape scalar job");

  } catch (const std::exception& e) {
    SW_ERROR("Exception in shape scalar job: {}", e.what());
  }
}

//---------------------------------------------------------------------------
QString ShapeScalarJob::name() { return "Shape / Scalar Correlation"; }

//---------------------------------------------------------------------------
QPixmap ShapeScalarJob::get_plot() { return plot_; }

//---------------------------------------------------------------------------
Eigen::VectorXd ShapeScalarJob::predict_scalars(QSharedPointer<Session> session, QString target_feature,
                                                Eigen::MatrixXd target_particles) {
  // blocking call to predict scalars for given target particles

  auto job = QSharedPointer<ShapeScalarJob>::create(session, target_feature, target_particles, JobType::Predict);

  Eigen::VectorXd prediction;

  std::atomic_bool finished(false);
  connect(job.data(), &ShapeScalarJob::finished, [&] {
    SW_DEBUG("shape scalar predict job finished");
    prediction = job->get_prediction();
    finished = true;
  });

  session->get_py_worker()->run_job(job);

  while (!finished) {
    QThread::msleep(100);
  }

  return prediction;
}

//---------------------------------------------------------------------------
void ShapeScalarJob::prep_data() {
  int num_particles = session_->get_num_particles();
  auto shapes = session_->get_shapes();
  int num_shapes = shapes.size();

  Eigen::MatrixXd all_particles(num_shapes, 3 * num_particles);
  Eigen::MatrixXd all_scalars(num_shapes, 1 * num_particles);

  for (int i = 0; i < shapes.size(); i++) {
    Eigen::VectorXd particles = shapes[i]->get_global_correspondence_points();

    // write as row into all_particles
    all_particles.row(i) = particles.transpose();

    shapes[i]->get_reconstructed_meshes(true);
    shapes[i]->load_feature(DisplayMode::Reconstructed, target_feature_.toStdString());

    Eigen::VectorXf scalars = shapes[i]->get_point_features(target_feature_.toStdString());

    // convert to Eigen::VectorXd
    Eigen::VectorXd scalars_d = scalars.cast<double>();

    // write into all_scalars
    all_scalars.row(i) = scalars_d.transpose();
  }

  all_particles_ = all_particles;
  all_scalars_ = all_scalars;
}

//---------------------------------------------------------------------------
void ShapeScalarJob::run_mse() {}

//---------------------------------------------------------------------------
void ShapeScalarJob::run_prediction() {}

//---------------------------------------------------------------------------
}  // namespace shapeworks

#include <Common/Logging.h>
#include <Libs/Project/Project.h>
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
ShapeScalarJob::ShapeScalarJob(QSharedPointer<Session> session, QString target_feature)
    : session_(session), target_feature_(target_feature) {}

//---------------------------------------------------------------------------
void ShapeScalarJob::run() {
  SW_DEBUG("Running shape scalar job");

  try {
    // get all particles as a single dimensional Eigen Matrix

    int num_particles = session_->get_num_particles();

    auto shapes = session_->get_shapes();
    int num_shapes = shapes.size();

    Eigen::MatrixXd all_particles(num_particles * num_shapes, 3);

    Eigen::MatrixXd all_scalars(num_particles * num_shapes, 1);

    for (int i = 0; i < shapes.size(); i++) {
      SW_LOG("Processing shape: {}", i);

      Eigen::VectorXd particles = shapes[i]->get_global_correspondence_points();

      // reinterpret as 3 columns as an Eigen Matrix
      Eigen::MatrixXd particles_reshaped = particles.reshaped(particles.size() / 3, 3);

      SW_LOG("Particles: {} x {}", particles_reshaped.rows(), particles_reshaped.cols());
      SW_LOG("write into all_particles");
      // write into all_particles
      all_particles.block(i, 0, particles_reshaped.rows(), particles_reshaped.cols()) = particles_reshaped;

      Eigen::VectorXf scalars = shapes[i]->get_point_features(target_feature_.toStdString());

      // convert to Eigen::VectorXd
      Eigen::VectorXd scalars_d = scalars.cast<double>();

      SW_LOG("write into all_scalars");
      // write into all_scalars
      all_scalars.block(i, 0, scalars.rows(), scalars.cols()) = scalars_d;
    }

    SW_LOG("all_particles: {} x {}", all_particles.rows(), all_particles.cols());
    SW_LOG("all_scalars: {} x {}", all_scalars.rows(), all_scalars.cols());

    /// write all_particles to a csv file in /tmp
    // std::ofstream out("/tmp/all_particles.csv");
    // const static Eigen::IOFormat CSVFormat(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", "\n");
    // out << all_particles.format(CSVFormat);
    // out.close();

    /// write all_scalars to a csv file in /tmp
    // std::ofstream out2("/tmp/all_scalars.csv");
    // out2 << all_scalars.format(CSVFormat);
    // out2.close();

    // A = np.array(all_particles)
    SW_LOG("import numpy");
    py::module np = py::module::import("numpy");
    SW_LOG("create A");
    py::object A = np.attr("array")(all_particles);

    // B = np.array(all_scalars)
    SW_LOG("create B");
    py::object B = np.attr("array")(all_scalars);

    SW_LOG("load from disk");
    A = np.attr("loadtxt")("/tmp/A.csv", "delimiter"_a = ",");
    B = np.attr("loadtxt")("/tmp/B.csv", "delimiter"_a = ",");

    py::module sw = py::module::import("shapeworks");
    py::object run_mbpls = sw.attr("shape_scalars").attr("run_mbpls");

    // returns a tuple of (png_raw_bytes, y_pred, mse)
    using ResultType = std::tuple<py::array, Eigen::MatrixXd, double>;

    ResultType result = run_mbpls(A, B).cast<ResultType>();

    py::array png_raw_bytes = std::get<0>(result);
    Eigen::MatrixXd y_pred = std::get<1>(result);
    double mse = std::get<2>(result);

    // interpret png_raw_bytes as a QImage
    QImage image;
    image.loadFromData((const uchar *)png_raw_bytes.data(), png_raw_bytes.size(), "PNG");
    //image.loadFromData(png_raw_bytes.data(), png_raw_bytes.size(), "PNG");

    plot_ = QPixmap::fromImage(image);

    plot_.save("/tmp/plot.png");

    SW_LOG("mse = {}", mse);
    /*

    SW_LOG("about to create mbpls");

    py::module mbpls = py::module::import("mbpls.mbpls");
    py::object mbpls_class = mbpls.attr("MBPLS");
    // mbpls_model = MBPLS(n_components=3)
    py::object mbpls_object = mbpls_class("n_components"_a = 3);

    SW_LOG("about to run fit");

    // mbpls_model.fit(A,B)
    mbpls_object.attr("fit")(A, B);

    SW_LOG("about to run predict");

    // y_pred = mbpls_model.predict(A)
    py::object y_pred = mbpls_object.attr("predict")(A);

    // print the type of mbpls_object.attr("P_")
    std::cout << "type of mbpls_object.attr(\"P_\") is " << py::str(mbpls_object.attr("P_").get_type()) << std::endl;

    // P = mbpls_model.P_
    py::object P = mbpls_object.attr("P_");

    // P = P[0]
    P = P.attr("__getitem__")(0);

    // get P_np as an Eigen Matrix
    Eigen::MatrixXd P_eigen = py::cast<Eigen::MatrixXd>(P);

    // p1_hat = mbpls_model.P_[0][:,0]
    // py::object p1_hat =
    //    mbpls_object.attr("P_")[0].attr("__getitem__")(0).attr("__getitem__")(py::slice(0, py::none(), 1));

    // store p1_hat as an Eigen Vector
    // p1_hat_ = p1_hat.cast<Eigen::VectorXd>();

    // output dims to SW_LOG
    SW_LOG("P_eigen: {} x {}", P_eigen.rows(), P_eigen.cols());
*/

    SW_DEBUG("End shape scalar job");

  } catch (const std::exception& e) {
    SW_ERROR("Exception in shape scalar job: {}", e.what());
  }
}

//---------------------------------------------------------------------------
QString ShapeScalarJob::name() { return "Shape / Scalar Correlation"; }

//---------------------------------------------------------------------------
QPixmap ShapeScalarJob::get_plot()
{
  return plot_;
}

//---------------------------------------------------------------------------
}  // namespace shapeworks

#include <Common/Logging.h>
#include <Libs/Project/Project.h>
#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

// eigen format
#include <Eigen/Dense>

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

    // write all_particles to a csv file in /tmp
    std::ofstream out("/tmp/all_particles.csv");
    const static Eigen::IOFormat CSVFormat(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", "\n");
    out << all_particles.format(CSVFormat);
    out.close();

    // write all_scalars to a csv file in /tmp
    std::ofstream out2("/tmp/all_scalars.csv");
    out2 << all_scalars.format(CSVFormat);
    out2.close();

    // A = np.array(all_particles)
    py::module np = py::module::import("numpy");
    py::object A = np.attr("array")(all_particles);

    // B = np.array(all_scalars)
    py::object B = np.attr("array")(all_scalars);

    // A = np.attr("loadtxt")("/tmp/A.csv", "delimiter"_a = ",");

    // B = np.attr("loadtxt")("/tmp/B.csv", "delimiter"_a = ",");

    py::module mbpls = py::module::import("mbpls.mbpls");
    py::object mbpls_class = mbpls.attr("MBPLS");
    // mbpls_model = MBPLS(n_components=3)
    py::object mbpls_object = mbpls_class("n_components"_a = 3);

    // mbpls_model.fit(A,B)
    mbpls_object.attr("fit")(A, B);

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

    SW_DEBUG("End shape scalar job");

  } catch (const std::exception& e) {
    SW_ERROR("Exception in shape scalar job: {}", e.what());
  }
}

//---------------------------------------------------------------------------
QString ShapeScalarJob::name() { return "Shape / Scalar Correlation"; }

//---------------------------------------------------------------------------
}  // namespace shapeworks

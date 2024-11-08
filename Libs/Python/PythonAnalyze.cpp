#include "PythonAnalyze.h"

#include <Analyze/Analyze.h>
#include <Analyze/Shape.h>
#include <pybind11/eigen.h>

namespace py = pybind11;
using namespace pybind11::literals;
using namespace shapeworks;

void define_python_analyze(py::module_ m) {
  py::class_<Analyze>(m, "Analyze")
      .def(py::init<ProjectHandle>())
      .def("get_mean_shape",
           [](Analyze& analyze) {
             auto mesh_group = analyze.get_mean_shape()->get_reconstructed_meshes(true);
             if (!mesh_group.valid()) {
               throw std::runtime_error("Invalid mesh group");
             }
             return Mesh(mesh_group.meshes()[0]->get_poly_data());
           })
      .def("run_offline_analysis", &Analyze::run_offline_analysis)
      .def("get_shapes", &Analyze::get_shapes)
      .def("get_num_modes", &Analyze::get_num_modes)
      .def("get_mean_shape_points",
           [](Analyze& analyze) -> decltype(auto) {
             auto particles = analyze.get_mean_shape_points();
             return particles.get_combined_global_particles();
           })
      .def("get_shape_points", &Analyze::get_shape_points)
      .def("get_mode_shape", &Analyze::get_mode_shape)
      .def("groups_active", &Analyze::groups_active)
      .def("get_subject_features", &Analyze::get_subject_features)
      .def("get_num_subjects", &Analyze::get_num_subjects)
      .def("get_num_particles", &Analyze::get_num_particles)
      .def("create_shape_from_points", &Analyze::create_shape_from_points);
}

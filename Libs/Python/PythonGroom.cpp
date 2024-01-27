#include "PythonGroom.h"

#include <Groom/Groom.h>
#include <pybind11/eigen.h>

namespace py = pybind11;
using namespace pybind11::literals;
using namespace shapeworks;

void define_python_groom(py::module_ m) {
  py::class_<Groom>(m, "Groom").def(py::init<std::shared_ptr<Project>>()).def("run", &Groom::run);
}

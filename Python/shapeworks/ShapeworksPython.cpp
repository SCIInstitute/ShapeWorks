#include "PythonAPI.h"

using namespace pybind11::literals;

// #include "Shapeworks.h"
#include "Image.h"
// #include "Mesh.h"
//#include "Transform.h"

//example
int add(int i, int j)
{
  return i + j;
}


PYBIND11_MODULE(shapeworkspy, m)
{
  m.doc() = "ShapeWorks Python API";

  m.def("add", &add, "i"_a=1, "j"_a=2, "A function which adds two numbers");

  py::class_<shapeworks::Image>(m, "Image")
  .def(py::init<>())                    // necessary just for a default ctor?
  .def(py::init<const std::string &>()) // can the argument for init be named (it's filename in this case)
  .def("read", &shapeworks::Image::read, "filename"_a)
  .def("write", &shapeworks::Image::write, "filename"_a, "compressed"_a=true)
  .def("recenter", &shapeworks::Image::recenter);
}

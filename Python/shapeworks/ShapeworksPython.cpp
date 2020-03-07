#include "PythonAPI.h"

using namespace pybind11::literals;

// #include "Shapeworks.h"
#include "Image.h"
#include "Mesh.h"
#include "Transform.h"

// TODO
// - add operator= to the pybind11 (for Image and Mesh so far...)

// used for demonstration code below
int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(shapeworkspy, m)
{
  m.doc() = "ShapeWorks Python API";

  py::class_<shapeworks::Image>(m, "Image")
  .def(py::init<const std::string &>()) // can the argument for init be named (it's filename in this case)
  .def("write", &shapeworks::Image::write, "filename"_a, "compressed"_a=true)
  //.def_readonly_static("dims", &shapeworks::Image::dims, "number of dimensions, usually (always) 3")
  .def("antialias", &shapeworks::Image::antialias, "smooth the image", "numIterations"_a=50, "maxRMSErr"_a=0.01f, "numLayers"_a=3) //todo: want to use shapeworks::Image::dims, but it's not getting exported to python, even with def_readonly_static above
  .def("recenter", &shapeworks::Image::recenter)
  .def("isoresample", &shapeworks::Image::isoresample)
  .def("pad", &shapeworks::Image::pad, "padding"_a, "value"_a)
  .def("applyTransform", &shapeworks::Image::applyTransform, "transform"_a)
  .def("extractLabel", &shapeworks::Image::extractLabel, "label"_a=1)
  .def("closeHoles", &shapeworks::Image::closeHoles)
  .def("threshold", &shapeworks::Image::threshold, "min"_a = std::numeric_limits<shapeworks::Image::PixelType>::epsilon(), "max"_a = std::numeric_limits<shapeworks::Image::PixelType>::max())
  .def("centerOfMass", &shapeworks::Image::centerOfMass)
  //.def("size", &shapeworks::Image::size) // todo: doesn't work unless defined, and currently only a declaration exists
  //.def("center", &shapeworks::Image::center) // same as size above
  //.def("__repr__", &shapeworks::Image::print)  // todo should be lambda __repr__ that calls Image::operator<<, see file:///Users/cam/tools/pybind11/docs/classes.rst; maybe even easier than that? just  .def("__repr__", &shapeworks::Image::operator<<)  (also not defined as with center and size above)
  // .def("__set__", &shapeworks::Image::operator=) //todo
  ;

  py::class_<shapeworks::Mesh>(m, "Mesh")
  .def(py::init<>())
  .def("read", &shapeworks::Mesh::read, "filename"_a)
  .def("write", &shapeworks::Mesh::write, "filename"_a)
  .def("coverage", &shapeworks::Mesh::coverage, "Not sure what this computes. Maybe spatial overlap?", "other_mesh"_a)
  .def("smooth", &shapeworks::Mesh::smooth)
  .def("compare_points_equal", &shapeworks::Mesh::compare_points_equal, "other_mesh"_a)
  .def("compare_scalars_equal", &shapeworks::Mesh::compare_scalars_equal, "other_mesh"_a)
  ;
  
  // this is simply a demonstration of creating a submodule, which may not be necessary
  py::module sub_optimize = m.def_submodule("Optimize", "ShapeWorks optimization classes and functions");
  sub_optimize.def("add", &add, "adds two numbers", "i"_a=1, "j"_a=2);
}


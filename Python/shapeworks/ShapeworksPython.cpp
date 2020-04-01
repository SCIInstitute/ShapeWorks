#include "PythonAPI.h"

using namespace pybind11::literals;

#include "Shapeworks.h"
#include "Image.h"
#include "Mesh.h"
#include "Transform.h"
#include "Optimize.h"
#include "ParticleSystem.h"

// TODO
// - add operator= to the pybind11 (for Image, Mesh, etc)
// - split these out into separate files (e.g., one for each directory under Libs)

// used for demonstration code below
int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(shapeworks, m)
{
  m.doc() = "ShapeWorks Python API";

  // Shapeworks globals
  py::class_<shapeworks::Dims>(m, "Dims")
  .def(py::init<>())
  .def(py::init<unsigned, unsigned, unsigned>())
  //.def("__repr__",              &shapeworks::Dims::print)   //todo, this should use insertion operator<<
  //.def("__setitem__",           &shapeworks::Dims::operator[], "idx"_a, "val"_a) //todo
  //.def("__getitem__",           &shapeworks::Dims::operator[], "idx"_a)
  ;

  py::class_<shapeworks::Point3>(m, "Point3")
  .def(py::init<>())
//  .def(py::init<unsigned, unsigned, unsigned>(), [](double x, double y, double z){ self[0] = x; self[1] = y; self[2] = z; }) // todo: use a lambda kind of like this to initialize Point3 from three values
  ;
//TODO: enable subscripting of Point3 in Python
// >>> sz = img.size()
// >>> sz[0]
// Traceback (most recent call last):
//   File "<stdin>", line 1, in <module>
// TypeError: 'shapeworks.Point3' object is not subscriptable

  // TODO: add Vector3 and Matrix (Matrix44) 

  py::class_<shapeworks::Transform>(m, "Transform")
  .def(py::init<>())
  .def("reset",                 &shapeworks::Transform::reset)
  .def("translate",             &shapeworks::Transform::translate, "v"_a)
  .def("rotate",                &shapeworks::Transform::rotate, "axis"_a, "angle"_a)
  .def("scale",                 &shapeworks::Transform::scale, "s"_a)
  .def("__repr__",              &shapeworks::Transform::print)
  ;

  py::class_<shapeworks::Image>(m, "Image")
  .def(py::init<const std::string &>()) // can the argument for init be named (it's filename in this case)
  .def("write",                 &shapeworks::Image::write, "filename"_a, "compressed"_a=true)
  .def("antialias",             &shapeworks::Image::antialias, "smooth the image", "numIterations"_a=50, "maxRMSErr"_a=0.01f, "numLayers"_a=3)
  .def("recenter",              &shapeworks::Image::recenter)
  .def("isoresample",           &shapeworks::Image::isoresample, "isoSpacing"_a=1.0, "outputSize"_a=shapeworks::Dims())
  .def("pad",                   &shapeworks::Image::pad, "padding"_a, "value"_a)
  .def("applyTransform",        &shapeworks::Image::applyTransform, "transform"_a)
  .def("extractLabel",          &shapeworks::Image::extractLabel, "label"_a=1)
  .def("closeHoles",            &shapeworks::Image::closeHoles)
  .def("threshold",             &shapeworks::Image::threshold, "min"_a = std::numeric_limits<shapeworks::Image::PixelType>::epsilon(), "max"_a = std::numeric_limits<shapeworks::Image::PixelType>::max())
  .def("centerOfMass",          &shapeworks::Image::centerOfMass)
  .def("dims",                  &shapeworks::Image::dims)
  .def("size",                  &shapeworks::Image::size)
  .def("center",                &shapeworks::Image::center)
  .def("__repr__",              &shapeworks::Image::print)
  //.def("__set__",             &shapeworks::Image::operator=) //todo
  ;
  
  py::class_<shapeworks::Mesh>(m, "Mesh")
  .def(py::init<>())
  .def("read",                  &shapeworks::Mesh::read, "filename"_a)
  .def("write",                 &shapeworks::Mesh::write, "filename"_a)
  .def("coverage",              &shapeworks::Mesh::coverage, "Not sure what this computes. Maybe spatial overlap", "other_mesh"_a)
  .def("smooth",                &shapeworks::Mesh::smooth)
  .def("compare_points_equal",  &shapeworks::Mesh::compare_points_equal, "other_mesh"_a)
  .def("compare_scalars_equal", &shapeworks::Mesh::compare_scalars_equal, "other_mesh"_a)
  ;

  // ParticleSystem
  py::class_<ParticleSystem>(m, "ParticleSystem")
  .def(py::init<>())
  .def("LoadParticles",         &ParticleSystem::LoadParticles, "paths"_a)
  .def("Particles",             &ParticleSystem::Particles)    // note: must import Eigenpy (github stack-of-tasks/eigenpy)
  .def("Paths",                 &ParticleSystem::Paths)
  .def("N",                     &ParticleSystem::N)
  .def("D",                     &ParticleSystem::D)
  ;

  // Optimize (TODO)
  py::class_<Optimize>(m, "Optimize")
  ;
  
  // this is simply a demonstration of creating a submodule, which may not be necessary
  py::module sub_module = m.def_submodule("submodule", "ShapeWorks submodule classes and functions");
  sub_module.def("add", &add, "adds two numbers", "i"_a=1, "j"_a=2);
}


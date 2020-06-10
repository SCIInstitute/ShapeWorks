#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include "Shapeworks.h"
#include "Image.h"
#include "ImageUtils.h"
#include "Mesh.h"
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

  py::class_<shapeworks::Coord>(m, "Coord")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

  // Shapeworks globals
  py::class_<shapeworks::Dims>(m, "Dims")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>())
  //.def("__repr__",              &shapeworks::Dims::print)   //todo, this should use insertion operator<<
  //.def("__setitem__",           &shapeworks::Dims::operator[], "idx"_a, "val"_a) //todo
  //.def("__getitem__",           &shapeworks::Dims::operator[], "idx"_a)
  ;

  py::class_<shapeworks::Point3>(m, "Point3")
  .def(py::init<>())
//  .def(py::init<unsigned, unsigned, unsigned>(), [](double x, double y, double z){ self[0] = x; self[1] = y; self[2] = z; }) // todo: use a lambda kind of like this to initialize Point3 from three values
  ;

  py::class_<shapeworks::Vector3>(m, "Vector3")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

  py::class_<shapeworks::Matrix44>(m, "Matrix44")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

  py::class_<shapeworks::Matrix33>(m, "Matrix33")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

  py::class_<shapeworks::IPoint3>(m, "IPoint3")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

  py::class_<shapeworks::FPoint3>(m, "FPoint3")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

  py::class_<shapeworks::Vector>(m, "Vector")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

  py::class_<shapeworks::Point>(m, "Point")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

  py::class_<shapeworks::Matrix>(m, "Matrix")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

//TODO: enable subscripting of Point3 in Python
// >>> sz = img.size()
// >>> sz[0]
// Traceback (most recent call last):
//   File "<stdin>", line 1, in <module>
// TypeError: 'shapeworks.Point3' object is not subscriptable

  py::class_<shapeworks::Image>(m, "Image")
  .def(py::init<const std::string &>()) // can the argument for init be named (it's filename in this case)
  .def(py::init<shapeworks::Image::ImageType::Pointer>())
  // .def(py::init<const shapeworks::Image &&>())
  .def(py::init<const shapeworks::Image &>())
  // .def(py::self = py::self)
  .def("write",                 &shapeworks::Image::write, "filename"_a, "compressed"_a=true)
  .def("antialias",             &shapeworks::Image::antialias, "smooth the image", "iterations"_a=50, "maxRMSErr"_a=0.01f, "layers"_a=3)
  .def("recenter",              &shapeworks::Image::recenter)
  .def("resample",              &shapeworks::Image::resample, "physicalSpacing"_a, "logicalDims"_a)
  .def("pad",                   &shapeworks::Image::pad, "padding"_a=0, "value"_a=0.0)
  .def("translate",             &shapeworks::Image::translate, "v"_a)
  .def("scale",                 &shapeworks::Image::scale, "v"_a)
  .def("rotate",                &shapeworks::Image::rotate, "angle"_a, "v"_a)
  // .def("applyTransform",        &shapeworks::Image::applyTransform, "transform"_a)
  .def("extractLabel",          &shapeworks::Image::extractLabel, "label"_a=1.0)
  .def("closeHoles",            &shapeworks::Image::closeHoles)
  .def("threshold",             &shapeworks::Image::threshold, "min"_a = std::numeric_limits<shapeworks::Image::PixelType>::epsilon(), "max"_a = std::numeric_limits<shapeworks::Image::PixelType>::max())
  .def("computeDT",             &shapeworks::Image::computeDT, "isovalue"_a=0.0)
  .def("applyCurvatureFilter",  &shapeworks::Image::applyCurvatureFilter, "iterations"_a=10)
  .def("applyGradientFilter",   &shapeworks::Image::applyGradientFilter)
  .def("applySigmoidFilter",    &shapeworks::Image::applySigmoidFilter, "alpha"_a=10.0, "beta"_a=10.0)
  .def("applyTPLevelSetFilter", &shapeworks::Image::applyTPLevelSetFilter, "featureimage"_a, "scaling"_a=20.0)
  .def("gaussianBlur",          &shapeworks::Image::gaussianBlur, "sigma"_a)
  .def("crop",                  &shapeworks::Image::crop, "region"_a)
  // .def("clip",                  py::overload_cast<shapeworks::Point, shapeworks::Point, shapeworks::Point, shapeworks::Image::PixelType>(&shapeworks::Image::clip))
  // .def("clip",                  py::overload_cast<shapeworks::Vector, shapeworks::Point, shapeworks::Image::PixelType>(&shapeworks::Image::clip))
  .def("setOrigin",             &shapeworks::Image::setOrigin, "origin"_a=shapeworks::Point3({0,0,0}))
  .def("reflect",                &shapeworks::Image::reflect, "normal"_a=shapeworks::makeVector({-1,1,1}))
  .def("centerOfMass",          &shapeworks::Image::centerOfMass, "minval"_a=0.0, "maxval"_a=1.0)
  .def("dims",                  &shapeworks::Image::dims)
  .def("size",                  &shapeworks::Image::size)
  .def("spacing",               &shapeworks::Image::spacing)
  .def("origin",                &shapeworks::Image::origin)
  .def("center",                &shapeworks::Image::center)
  .def("coordsys",              &shapeworks::Image::coordsys)
  // .def("__repr__",              &shapeworks::Image::print)
  // .def("__set__",               &shapeworks::Image::operator=, "img"_a)
  ;

  py::class_<shapeworks::Image::Region>(m, "Region")
  .def("origin",                &shapeworks::Image::Region::origin)
  .def("size",                  &shapeworks::Image::Region::size)
  .def("pad",                   &shapeworks::Image::Region::pad, "other"_a)
  .def("shrink",                &shapeworks::Image::Region::shrink, "other"_a)
  .def("grow",                  &shapeworks::Image::Region::grow, "other"_a)
  .def("expand",                &shapeworks::Image::Region::expand, "other"_a)
  ;

  py::class_<shapeworks::ImageUtils>(m, "ImageUtils")
  .def_static("boundingBox",    &shapeworks::ImageUtils::boundingBox, "filenames"_a)
  .def_static("createCenterOfMassTransform",
                                &shapeworks::ImageUtils::createCenterOfMassTransform, "image"_a)
  .def_static("rigidRegistration",
                                &shapeworks::ImageUtils::rigidRegistration, "img"_a, "target"_a, "source"_a, "isoValue"_a=0.0, "iterations"_a=20)
  .def_static("computeWarp",    &shapeworks::ImageUtils::computeWarp, "source_file"_a, "target_file"_a, "pointFactor"_a=1)
  .def_static("topologyPreservingSmooth", 
                                &shapeworks::ImageUtils::topologyPreservingSmooth<shapeworks::Image>, "img"_a, "scaling"_a = 20.0, "sigmoidAlpha"_a = 10.5, "sigmoidBeta"_a = 10.0, "curvatureIterations"_a = 10, "applycurvaturefilter"_a = true)
  .def_static("isoresample",    &shapeworks::ImageUtils::isoresample<shapeworks::Image>, "img"_a, "isoSpacing"_a = 1.0, "outputSize"_a = shapeworks::Dims())
  ;

  py::class_<shapeworks::Mesh>(m, "Mesh")
  .def(py::init<>())
  .def("read",                  &shapeworks::Mesh::read, "filename"_a)
  .def("write",                 &shapeworks::Mesh::write, "filename"_a)
  .def("coverage",              &shapeworks::Mesh::coverage, "Not sure what this computes. Maybe spatial overlap", "other_mesh"_a)
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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include "Shapeworks.h"
#include "ShapeworksUtils.h"
#include "Image.h"
#include "ImageUtils.h"
#include "Mesh.h"
#include "MeshUtils.h"
#include "Optimize.h"
#include "ParticleSystem.h"
#include "ShapeEvaluation.h"

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
  py::object Coord = py::cast(itk::Index<3>());
  m.attr("Coord") = Coord;
  py::object Dims = py::cast(itk::Size<3>());
  m.attr("Dims") = Dims;
  py::object Point3 = py::cast(itk::Point<double, 3>());
  m.attr("Point3") = Point3;
  py::object Vector3 = py::cast(itk::Vector<double, 3>());
  m.attr("Vector3") = Vector3;
  py::object Matrix44 = py::cast(itk::Matrix<double, 4, 4>());
  m.attr("Matrix44") = Matrix44;
  py::object Matrix33 = py::cast(itk::Matrix<double, 3, 3>());
  m.attr("Matrix33") = Matrix33;
  py::object IPoint3 = py::cast(itk::Point<int, 3>());
  m.attr("IPoint3") = IPoint3;
  py::object FPoint3 = py::cast(itk::Point<float, 3>());
  m.attr("FPoint3") = FPoint3;
  m.attr("Vector") = Vector3;
  m.attr("Point") = Point3;
  m.attr("Matrix") = Matrix33;

  // Shapeworks globals
  // py::class_<shapeworks::Coord>(m, "Coord")
  // .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>())
  // ;

  // Shapeworks globals
  // py::class_<shapeworks::Dims>(m, "Dims")
  // .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>())
  //.def("__repr__",              &shapeworks::Dims::print)   //todo, this should use insertion operator<<
  //.def("__setitem__",           &shapeworks::Dims::operator[], "idx"_a, "val"_a) //todo
  //.def("__getitem__",           &shapeworks::Dims::operator[], "idx"_a)
  // ;

  // m.def("toPoint",               py::overload_cast<shapeworks::Dims>(&shapeworks::Point::toPoint));
  // m.def("toPoint",               py::overload_cast<shapeworks::Coord>(&shapeworks::Point::toPoint));
  // m.def("toPoint",               py::overload_cast<shapeworks::Vector>(&shapeworks::Point::toPoint));

  // Shapeworks globals
  // py::class_<shapeworks::Point3>(m, "Point3")
  // .def(py::init<>())
//  .def(py::init<unsigned, unsigned, unsigned>(), [](double x, double y, double z){ self[0] = x; self[1] = y; self[2] = z; }) // todo: use a lambda kind of like this to initialize Point3 from three values
  // .def("toPoint",               py::overload_cast<shapeworks::Dims>(&shapeworks::Point3::toPoint))
  // .def("toPoint",               py::overload_cast<shapeworks::Coord>(&shapeworks::Point3::toPoint))
  // .def("toPoint",               py::overload_cast<shapeworks::Vector>(&shapeworks::Point3::toPoint))
  // ;

  // Shapeworks globals
  // py::class_<shapeworks::Vector3>(m, "Vector3")
  // .def(py::init<>())
  // .def("toVector",              py::overload_cast<shapeworks::Dims>(&shapeworks::Vector3::toVector))
  // .def("toVector",              py::overload_cast<shapeworks::Point>(&shapeworks::Vector3::toVector))
  // ;

  // Shapeworks globals
  // py::class_<shapeworks::Matrix44>(m, "Matrix44")
  // .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  // ;

  // Shapeworks globals
  // py::class_<shapeworks::Matrix33>(m, "Matrix33")
  // .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  // ;

  // Shapeworks globals
  // py::class_<shapeworks::IPoint3>(m, "IPoint3")
  // .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  // ;

  // Shapeworks globals
  // py::class_<shapeworks::FPoint3>(m, "FPoint3")
  // .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  // ;

  //TODO: enable subscripting of Point3 in Python
  // >>> sz = img.size()
  // >>> sz[0]
  // Traceback (most recent call last):
  //   File "<stdin>", line 1, in <module>
  // TypeError: 'shapeworks.Point3' object is not subscriptable

  // ShapeworksUtils
  py::class_<shapeworks::ShapeworksUtils>(m, "ShapeworksUtils")
  .def_static("is_directory",   &shapeworks::ShapeworksUtils::is_directory, "pathname"_a)
  .def_static("getMatrix",      &shapeworks::ShapeworksUtils::getMatrix, "mat"_a)
  .def_static("getOffset",      &shapeworks::ShapeworksUtils::getOffset, "mat"_a)
  // .def_static("connectPipelines",
  //                               &shapeworks::ShapeworksUtils::connectPipelines<itk::VTKImageExport<shapeworks::Image::ImageType>, vtkImageImport::New()>, "exporter"_a, "importer"_a)
  ;

  // Image
  py::class_<shapeworks::Image>(m, "Image")
  .def(py::init<const std::string &>()) // can the argument for init be named (it's filename in this case)
  .def(py::init<shapeworks::Image::ImageType::Pointer>())
  // .def(py::init<const shapeworks::Image &&>())
  .def(py::init<const shapeworks::Image &>())
  // .def(py::self = shapeworks::Image&())
  // .def(py::self = const shapeworks::Image &&)
  // .def(operator ImageType::Pointer)
  // .def(-py::self)
  .def(py::self + py::self)
  .def(py::self += py::self)
  .def(py::self - py::self)
  .def(py::self -= py::self)
  .def(py::self * shapeworks::Image::PixelType())
  .def(py::self *= shapeworks::Image::PixelType())
  .def(py::self / shapeworks::Image::PixelType())
  .def(py::self /= shapeworks::Image::PixelType())
  .def(py::self + shapeworks::Image::PixelType())
  .def(py::self += shapeworks::Image::PixelType())
  .def(py::self - shapeworks::Image::PixelType())
  .def(py::self -= shapeworks::Image::PixelType())
  .def(py::self == py::self)
  .def("write",                 &shapeworks::Image::write, "filename"_a, "compressed"_a=true)
  .def("antialias",             &shapeworks::Image::antialias, "smooth the image", "iterations"_a=50, "maxRMSErr"_a=0.01f, "layers"_a=0)
  .def("recenter",              &shapeworks::Image::recenter)
  .def("resample",              &shapeworks::Image::resample, "physicalSpacing"_a, "logicalDims"_a)
  .def("pad",                   py::overload_cast<int, shapeworks::Image::PixelType>(&shapeworks::Image::pad))
  .def("pad",                   py::overload_cast<int, int, int,shapeworks::Image::PixelType>(&shapeworks::Image::pad))
  .def("translate",             &shapeworks::Image::translate, "v"_a)
  .def("scale",                 &shapeworks::Image::scale, "v"_a)
  .def("rotate",                &shapeworks::Image::rotate, "angle"_a, "axis"_a)
  .def("applyTransform",        py::overload_cast<shapeworks::TransformPtr, shapeworks::Dims, shapeworks::Point3, shapeworks::Vector, shapeworks::Image::ImageType::DirectionType>(&shapeworks::Image::applyTransform))
  .def("applyTransform",        py::overload_cast<shapeworks::TransformPtr>(&shapeworks::Image::applyTransform))
  .def("extractLabel",          &shapeworks::Image::extractLabel, "label"_a=1.0)
  .def("closeHoles",            &shapeworks::Image::closeHoles, "foreground"_a=0.0)
  .def("binarize",              &shapeworks::Image::binarize, "minval"_a=0.0, "maxval"_a=std::numeric_limits<shapeworks::Image::PixelType>::max(), "inner_value"_a=1.0, "outer_value"_a=0.0)
  .def("computeDT",             &shapeworks::Image::computeDT, "isovalue"_a=0.0)
  .def("applyCurvatureFilter",  &shapeworks::Image::applyCurvatureFilter, "iterations"_a=10)
  .def("applyGradientFilter",   &shapeworks::Image::applyGradientFilter)
  .def("applySigmoidFilter",    &shapeworks::Image::applySigmoidFilter, "alpha"_a=10.0, "beta"_a=10.0)
  .def("applyTPLevelSetFilter", &shapeworks::Image::applyTPLevelSetFilter, "featureimage"_a, "scaling"_a=20.0)
  .def("gaussianBlur",          &shapeworks::Image::gaussianBlur, "sigma"_a=0.0)
  .def("crop",                  &shapeworks::Image::crop, "region"_a)
  .def("clip",                  py::overload_cast<const shapeworks::Point&, const shapeworks::Point&, const shapeworks::Point&, const shapeworks::Image::PixelType>(&shapeworks::Image::clip))
  .def("clip",                  py::overload_cast<const shapeworks::Vector&, const shapeworks::Point&, const shapeworks::Image::PixelType>(&shapeworks::Image::clip))
  .def("setOrigin",             &shapeworks::Image::setOrigin, "origin"_a=shapeworks::Point3({0,0,0}))
  .def("reflect",                &shapeworks::Image::reflect, "axis"_a)
  .def("centerOfMass",          &shapeworks::Image::centerOfMass, "minval"_a=0.0, "maxval"_a=1.0)
  .def("dims",                  &shapeworks::Image::dims)
  .def("size",                  &shapeworks::Image::size)
  .def("spacing",               &shapeworks::Image::spacing)
  .def("origin",                &shapeworks::Image::origin)
  .def("center",                &shapeworks::Image::center)
  .def("coordsys",              &shapeworks::Image::coordsys)
  .def("centerOfMass",          &shapeworks::Image::centerOfMass, "minval"_a=0.0, "maxval"_a=1.0)
  .def("boundingBox",           &shapeworks::Image::boundingBox, "isoValue"_a=1.0)
  .def("logicalToPhysical",     &shapeworks::Image::logicalToPhysical, "v"_a)
  .def("physicalToLogical",     &shapeworks::Image::physicalToLogical, "p"_a)
  .def("compare",               &shapeworks::Image::compare, "other"_a, "verifyall"_a=true, "tolerance"_a=0.0, "precision"_a=1e-12)
  .def_static("getPolyData",    &shapeworks::Image::getPolyData, "image"_a, "isoValue"_a=0.0)
  // .def("__repr__",              &shapeworks::Image::print)
  // .def("__set__",               &shapeworks::Image::operator=, "img"_a)
  ;

  // Image::Region
  py::class_<shapeworks::Image::Region>(m, "Region")
  .def(py::init<shapeworks::Dims>())
  .def(py::init<shapeworks::Coord, shapeworks::Coord>())
  .def(py::init<>())
  .def(py::self == py::self)
  .def("valid",                 &shapeworks::Image::Region::valid)
  .def("origin",                &shapeworks::Image::Region::origin)
  .def("size",                  &shapeworks::Image::Region::size)
  .def("clip",                  &shapeworks::Image::Region::clip, "image"_a)
  .def("pad",                   &shapeworks::Image::Region::pad, "padding"_a)
  .def("shrink",                &shapeworks::Image::Region::shrink, "other"_a)
  .def("grow",                  &shapeworks::Image::Region::grow, "other"_a)
  .def("expand",                &shapeworks::Image::Region::expand, "other"_a)
  // .def("ImageType::RegionType")
  ;

  // ImageUtils
  py::class_<shapeworks::ImageUtils>(m, "ImageUtils")
  .def_static("boundingBox",    &shapeworks::ImageUtils::boundingBox, "filenames"_a, "isoValue"_a=1.0)
  .def_static("createCenterOfMassTransform",
                                &shapeworks::ImageUtils::createCenterOfMassTransform, "image"_a)
  .def_static("createRigidRegistrationTransform",
                                &shapeworks::ImageUtils::createRigidRegistrationTransform, "source_dt"_a, "target_dt"_a, "isoValue"_a=0.0, "iterations"_a=20)
  .def_static("createWarpTransform",    
                                &shapeworks::ImageUtils::createWarpTransform, "source_landmarks"_a, "target_landmarks"_a, "stride"_a=1)
  .def_static("topologyPreservingSmooth", 
                                &shapeworks::ImageUtils::topologyPreservingSmooth, "img"_a, "scaling"_a = 20.0, "sigmoidAlpha"_a = 10.5, "sigmoidBeta"_a = 10.0)
  .def_static("isoresample",    &shapeworks::ImageUtils::isoresample, "image"_a, "isoSpacing"_a = 1.0, "outputSize"_a = shapeworks::Dims())
  ;

  // Mesh
  py::class_<shapeworks::Mesh>(m, "Mesh")
  .def(py::init<const std::string &>())
  .def("write",                 &shapeworks::Mesh::write, "pathname"_a)
  .def("coverage",              &shapeworks::Mesh::coverage, "other_mesh"_a)
  .def("compare_points_equal",  &shapeworks::Mesh::compare_points_equal, "other_mesh"_a)
  .def("compare_scalars_equal", &shapeworks::Mesh::compare_scalars_equal, "other_mesh"_a)
  ;

  // MeshUtils
  py::class_<shapeworks::MeshUtils>(m, "MeshUtils")
  .def_static("createIcpTransform",  
                                &shapeworks::MeshUtils::createIcpTransform, "source"_a, "target"_a, "iterations"_a=20)
  ;

  // ParticleSystem
  py::class_<shapeworks::ParticleSystem>(m, "ParticleSystem")
  .def(py::init<const std::vector<std::string> &>())
  .def("Particles",             &shapeworks::ParticleSystem::Particles) // note: must import Eigenpy (github stack-of-tasks/eigenpy)
  .def("Paths",                 &shapeworks::ParticleSystem::Paths)
  .def("N",                     &shapeworks::ParticleSystem::N)
  .def("D",                     &shapeworks::ParticleSystem::D)
  ;

  // ShapeEvaluation
  py::class_<shapeworks::ShapeEvaluation<3>>(m, "ShapeEvaluation")
  .def(py::init<>())
  .def_static("ComputeCompactness",
                                &shapeworks::ShapeEvaluation<3>::ComputeCompactness, "particleSystem"_a, "nModes"_a, "saveTo"_a="")
  .def_static("ComputeGeneralization",
                                &shapeworks::ShapeEvaluation<3>::ComputeGeneralization, "particleSystem"_a, "nModes"_a, "saveTo"_a="")
  .def_static("ComputeSpecificity",
                                &shapeworks::ShapeEvaluation<3>::ComputeSpecificity, "particleSystem"_a, "nModes"_a, "saveTo"_a="")

  ; // <todo> make template arguments global?

  // Optimize (TODO)
  py::class_<Optimize>(m, "Optimize")
  ;
  
  // this is simply a demonstration of creating a submodule, which may not be necessary
  py::module sub_module = m.def_submodule("submodule", "ShapeWorks submodule classes and functions");
  sub_module.def("add", &add, "adds two numbers", "i"_a=1, "j"_a=2);
}

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
//#include <pybind11/stl_bind.h>  // look at Binding STL containers portion of manual; not sure we even use any in ShapeWorks
#include <pybind11/operators.h>
#include <pybind11/eigen.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include <sstream>

#include "Shapeworks.h"
#include "ShapeworksUtils.h"
#include "Image.h"
#include "ImageUtils.h"
#include "Mesh.h"
#include "MeshUtils.h"
#include "Optimize.h"
#include "ParticleSystem.h"
#include "ShapeEvaluation.h"

using namespace shapeworks;

// TODO
// - add operator= to the pybind11 (for Image, Mesh, etc) // won't this just work? There could be memory issues with refs...
// - split these out into separate files (e.g., one for each directory under Libs)

// used for demonstration code below
int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(shapeworks, m)
{
  m.doc() = "ShapeWorks Python API";

  // Shapeworks globals
  // py::object Coord = py::cast(itk::Index<3>());
  // m.attr("Coord") = Coord;
  // py::object Vector3 = py::cast(itk::Vector<double, 3>());
  // m.attr("Vector3") = Vector3;
  // py::object Matrix44 = py::cast(itk::Matrix<double, 4, 4>());
  // m.attr("Matrix44") = Matrix44;
  // py::object Matrix33 = py::cast(itk::Matrix<double, 3, 3>());
  // m.attr("Matrix33") = Matrix33;
  // py::object IPoint3 = py::cast(itk::Point<int, 3>());
  // m.attr("IPoint3") = IPoint3;
  // py::object FPoint3 = py::cast(itk::Point<float, 3>());
  // m.attr("FPoint3") = FPoint3;
  // m.attr("Vector") = Vector3;
  // m.attr("Point") = Point3;
  // m.attr("Matrix") = Matrix33;

  // Shapeworks globals
  // py::class_<Coord>(m, "Coord")
  // .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>())
  // ;

  // Shapeworks globals
  py::class_<Dims>(m, "Dims")
  .def(py::init<>())
  .def(py::init<unsigned, unsigned, unsigned>())
  .def("__repr__",
       [](const Dims& d) {
         return (std::stringstream()<<d).str();
       })
  .def("__getitem__",
       [](const Dims& d, size_t idx) { return d[idx]; })
  .def("__setitem__",
       [](Dims& d, size_t idx, unsigned val) { d[idx] = val; })
  ;

  py::class_<Point>(m, "Point")
  .def(py::init<>())
  // .def(py::init<double, double, double>()) // no constructor available
  // .def(py::init<double, double, double>(), // this is one way to do it: constructs default then assigns values
  //      [](double x, double y, double z) {
  //        self[0] = x; self[1] = y; self[2] = z;
  //      })
  .def(py::init([](double x, double y, double z) { return Point({x,y,z}); }))  // directly call initializer list ctor  (***the winner!***)
  .def("__repr__", [](const Point& p) { return (std::stringstream()<<p).str(); })
  .def("__getitem__", [](const Point& d, size_t idx) { return d[idx]; })
  .def("__setitem__", [](Point& d, size_t idx, double val) { d[idx] = val; })
  ;

  m.def("toPoint", py::overload_cast<const Dims&>(&toPoint), "converts Dims to Point");
  // m.def("toPoint",               py::overload_cast<Coord>(&Point::toPoint));
  // m.def("toPoint",               py::overload_cast<Vector>(&Point::toPoint));


  // py::class_<Vector3>(m, "Vector3")
  // .def(py::init<>())
  // .def("toVector",              py::overload_cast<Dims>(&Vector3::toVector))
  // .def("toVector",              py::overload_cast<Point>(&Vector3::toVector))
  // ;

  // Shapeworks globals
  // py::class_<Matrix44>(m, "Matrix44")
  // .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  // ;

  // Shapeworks globals
  // py::class_<Matrix33>(m, "Matrix33")
  // .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  // ;

  // Shapeworks globals
  // py::class_<IPoint3>(m, "IPoint3")
  // .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  // ;

  // Shapeworks globals
  // py::class_<FPoint3>(m, "FPoint3")
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
  py::class_<ShapeworksUtils>(m, "ShapeworksUtils")
  .def_static("is_directory",   &ShapeworksUtils::is_directory, "pathname"_a)
  .def_static("getMatrix",      &ShapeworksUtils::getMatrix, "mat"_a)
  .def_static("getOffset",      &ShapeworksUtils::getOffset, "mat"_a)
  // .def_static("connectPipelines",
  //                               &ShapeworksUtils::connectPipelines<itk::VTKImageExport<Image::ImageType>, vtkImageImport::New()>, "exporter"_a, "importer"_a)
  ;

  // Image
  py::class_<Image>(m, "Image")
  .def(py::init<const std::string &>()) // can the argument for init be named (it's filename in this case)
  .def(py::init<Image::ImageType::Pointer>())
  // .def(py::init<const Image &&>())
  .def(py::init<const Image &>())
  // .def(py::self = Image&())
  // .def(py::self = const Image &&)
  // .def(operator ImageType::Pointer)
  // .def(-py::self)
  .def(py::self + py::self)
  .def(py::self += py::self)
  .def(py::self - py::self)
  .def(py::self -= py::self)
  .def(py::self * Image::PixelType())
  .def(py::self *= Image::PixelType())
  .def(py::self / Image::PixelType())
  .def(py::self /= Image::PixelType())
  .def(py::self + Image::PixelType())
  .def(py::self += Image::PixelType())
  .def(py::self - Image::PixelType())
  .def(py::self -= Image::PixelType())
  .def(py::self == py::self)
  .def("write",                 &Image::write, "filename"_a, "compressed"_a=true)
  .def("antialias",             &Image::antialias, "smooth the image", "iterations"_a=50, "maxRMSErr"_a=0.01f, "layers"_a=0)
  .def("recenter",              &Image::recenter)
  .def("resample",              &Image::resample, "physicalSpacing"_a, "logicalDims"_a)
  .def("pad",                   py::overload_cast<int, Image::PixelType>(&Image::pad))
  .def("pad",                   py::overload_cast<int, int, int,Image::PixelType>(&Image::pad))
  .def("translate",             &Image::translate, "v"_a)
  .def("scale",                 &Image::scale, "v"_a)
  .def("rotate",                &Image::rotate, "angle"_a, "axis"_a)
  .def("applyTransform",        py::overload_cast<TransformPtr, Dims, Point3, Vector, Image::ImageType::DirectionType>(&Image::applyTransform))
  .def("applyTransform",        py::overload_cast<TransformPtr>(&Image::applyTransform))
  .def("extractLabel",          &Image::extractLabel, "label"_a=1.0)
  .def("closeHoles",            &Image::closeHoles, "foreground"_a=0.0)
  .def("binarize",              &Image::binarize, "minval"_a=0.0, "maxval"_a=std::numeric_limits<Image::PixelType>::max(), "inner_value"_a=1.0, "outer_value"_a=0.0)
  .def("computeDT",             &Image::computeDT, "isovalue"_a=0.0)
  .def("applyCurvatureFilter",  &Image::applyCurvatureFilter, "iterations"_a=10)
  .def("applyGradientFilter",   &Image::applyGradientFilter)
  .def("applySigmoidFilter",    &Image::applySigmoidFilter, "alpha"_a=10.0, "beta"_a=10.0)
  .def("applyTPLevelSetFilter", &Image::applyTPLevelSetFilter, "featureimage"_a, "scaling"_a=20.0)
  .def("gaussianBlur",          &Image::gaussianBlur, "sigma"_a=0.0)
  .def("crop",                  &Image::crop, "region"_a)
  .def("clip",                  py::overload_cast<const Point&, const Point&, const Point&, const Image::PixelType>(&Image::clip))
  .def("clip",                  py::overload_cast<const Vector&, const Point&, const Image::PixelType>(&Image::clip))
  .def("setOrigin",             &Image::setOrigin, "origin"_a=Point3({0,0,0}))
  .def("reflect",                &Image::reflect, "axis"_a)
  .def("centerOfMass",          &Image::centerOfMass, "minval"_a=0.0, "maxval"_a=1.0)
  .def("dims",                  &Image::dims)
  .def("size",                  &Image::size)
  .def("spacing",               &Image::spacing)
  .def("origin",                &Image::origin)
  .def("center",                &Image::center)
  .def("coordsys",              &Image::coordsys)
  .def("centerOfMass",          &Image::centerOfMass, "minval"_a=0.0, "maxval"_a=1.0)
  .def("boundingBox",           &Image::boundingBox, "isoValue"_a=1.0)
  .def("logicalToPhysical",     &Image::logicalToPhysical, "v"_a)
  .def("physicalToLogical",     &Image::physicalToLogical, "p"_a)
  .def("compare",               &Image::compare, "other"_a, "verifyall"_a=true, "tolerance"_a=0.0, "precision"_a=1e-12)
  .def_static("getPolyData",    &Image::getPolyData, "image"_a, "isoValue"_a=0.0)
  // .def("__repr__",              &Image::print)
  // .def("__set__",               &Image::operator=, "img"_a)
  ;

  // Image::Region
  py::class_<Image::Region>(m, "Region")
  .def(py::init<Dims>())
  .def(py::init<Coord, Coord>())
  .def(py::init<>())
  .def(py::self == py::self)
  .def("valid",                 &Image::Region::valid)
  .def("origin",                &Image::Region::origin)
  .def("size",                  &Image::Region::size)
  .def("clip",                  &Image::Region::clip, "image"_a)
  .def("pad",                   &Image::Region::pad, "padding"_a)
  .def("shrink",                &Image::Region::shrink, "other"_a)
  .def("grow",                  &Image::Region::grow, "other"_a)
  .def("expand",                &Image::Region::expand, "other"_a)
  // .def("ImageType::RegionType")
  ;

  // ImageUtils
  py::class_<ImageUtils>(m, "ImageUtils")
  .def_static("boundingBox",    &ImageUtils::boundingBox, "filenames"_a, "isoValue"_a=1.0)
  .def_static("createCenterOfMassTransform",
                                &ImageUtils::createCenterOfMassTransform, "image"_a)
  .def_static("createRigidRegistrationTransform",
                                &ImageUtils::createRigidRegistrationTransform, "source_dt"_a, "target_dt"_a, "isoValue"_a=0.0, "iterations"_a=20)
  .def_static("createWarpTransform",    
                                &ImageUtils::createWarpTransform, "source_landmarks"_a, "target_landmarks"_a, "stride"_a=1)
  .def_static("topologyPreservingSmooth", 
                                &ImageUtils::topologyPreservingSmooth, "img"_a, "scaling"_a = 20.0, "sigmoidAlpha"_a = 10.5, "sigmoidBeta"_a = 10.0)
  .def_static("isoresample",    &ImageUtils::isoresample, "image"_a, "isoSpacing"_a = 1.0, "outputSize"_a = Dims())
  ;

  // Mesh
  py::class_<Mesh>(m, "Mesh")
  .def(py::init<const std::string &>())
  .def("write",                 &Mesh::write, "pathname"_a)
  .def("coverage",              &Mesh::coverage, "other_mesh"_a)
  .def("compare_points_equal",  &Mesh::compare_points_equal, "other_mesh"_a)
  .def("compare_scalars_equal", &Mesh::compare_scalars_equal, "other_mesh"_a)
  ;

  // MeshUtils
  py::class_<MeshUtils>(m, "MeshUtils")
  .def_static("createIcpTransform",  
                                &MeshUtils::createIcpTransform, "source"_a, "target"_a, "iterations"_a=20)
  ;

  // ParticleSystem
  py::class_<ParticleSystem>(m, "ParticleSystem")
  .def(py::init<const std::vector<std::string> &>())
  .def("Particles",             &ParticleSystem::Particles) // note: must import Eigenpy (github stack-of-tasks/eigenpy)
  .def("Paths",                 &ParticleSystem::Paths)
  .def("N",                     &ParticleSystem::N)
  .def("D",                     &ParticleSystem::D)
  ;

  // ShapeEvaluation  
  // py::class_<ShapeEvaluation<3>>(m, "ShapeEvaluation")
  // .def(py::init<>())
  // .def_static("ComputeCompactness",
  //                               &ShapeEvaluation<3>::ComputeCompactness, "particleSystem"_a, "nModes"_a, "saveTo"_a="")
  // .def_static("ComputeGeneralization",
  //                               &ShapeEvaluation<3>::ComputeGeneralization, "particleSystem"_a, "nModes"_a, "saveTo"_a="")
  // .def_static("ComputeSpecificity",
  //                               &ShapeEvaluation<3>::ComputeSpecificity, "particleSystem"_a, "nModes"_a, "saveTo"_a="")

  // ; // <todo> make template arguments global?

  // Optimize (TODO)
  py::class_<Optimize>(m, "Optimize")
  ;
  
  // this is simply a demonstration of creating a submodule, which may not be necessary (could add Groom, Optimize, etc)
  py::module sub_module = m.def_submodule("submodule", "ShapeWorks submodule classes and functions");
  sub_module.def("add", &add, "adds two numbers", "i"_a=1, "j"_a=2);
}

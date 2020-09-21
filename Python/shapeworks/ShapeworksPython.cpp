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

  m.attr("Pi") = std::atan(1.0) * 4.0;

  // Shapeworks Globals
  py::class_<Coord>(m, "Coord")
  .def(py::init<>())
  .def(py::init<unsigned, unsigned, unsigned>())
  .def("__repr__", [](const Coord& c) { return (std::stringstream()<<c).str(); })
  .def("__getitem__", [](const Coord& c, size_t idx) { return c[idx]; })
  .def("__setitem__", [](Coord& c, size_t idx, unsigned val) { c[idx] = val; })
  .def("__add__", [](const Coord& c1, const Coord& c2) { return c1 + c2; })
  .def("__sub__", [](const Coord& c1, const Coord& c2) { return c1 - c2; })
  .def("__mul__", [](const Coord& c1, const Coord& c2) { return c1 * c2; })
  .def("__iadd__", [](Coord& c1, const Coord& c2) { return c1 += c2; })
  .def("__isub__", [](Coord& c1, const Coord& c2) { return c1 -= c2; })
  .def("__mul__", [](const Coord& c, const double x) { return c * x; })
  .def("__truediv__", [](const Coord& c, const double x) { return c / x; })
  .def("__imul__", [](Coord& c, const double x) { return c *= x; })
  .def("__itruediv__", [](Coord& c, const double x) { return c /= x; })
  ;

  // Shapeworks Globals
  py::class_<Dims>(m, "Dims")
  .def(py::init<>())
  .def(py::init<unsigned, unsigned, unsigned>())
  .def("__repr__", [](const Dims &d) { return (std::stringstream() << d).str(); })
  .def("__getitem__", [](const Dims &d, size_t idx) { return d[idx]; })
  .def("__setitem__", [](Dims &d, size_t idx, unsigned val) { d[idx] = val; })
  .def("__add__", [](const Dims &d1, const Dims &d2) { return d1 + d2; })
  .def("__sub__", [](const Dims &d1, const Dims &d2) { return d1 + d2; })
  .def("__mul__", [](const Dims &d1, const Dims &d2) { return d1 * d2; })
  .def("__iadd__", [](Dims &d1, const Dims &d2) { return d1 += d2; })
  .def("__isub__", [](Dims &d1, const Dims &d2) { return d1 -= d2; })
  .def("__mul__", [](const Dims& d, const double x) { return d * x; })
  .def("__truediv__", [](const Dims& d, const double x) { return d / x; })
  .def("__imul__", [](Dims &d, const double x) { return d *= x; })
  .def("__itruediv__", [](Dims &d, const double x) { return d /= x; })
  ;

  // Shapeworks Globals
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
  .def("__add__", [](const Point& p1, const Point& p2) { return p1 + p2; })
  .def("__sub__", [](const Point& p1, const Point& p2) { return p1 - p2; })
  .def("__mul__", [](const Point& p1, const Point& p2) { return p1 * p2; })
  .def("__iadd__", [](Point& p1, const Point& p2) { return p1 += p2; })
  .def("__isub__", [](Point& p1, const Point& p2) { return p1 -= p2; })
  .def("__mul__", [](const Point& p, const double x) { return p * x; })
  .def("__truediv__", [](const Point& p, const double x) { return p / x; })
  .def("__imul__", [](Point& p, const double x) { return p *= x; })
  .def("__itruediv__", [](Point& p, const double x) { return p /= x; })
  ;

  py::class_<Vector>(m, "Vector")
  .def(py::init<>())
  .def(py::init([](double x, double y, double z) { return makeVector({x,y,z}); }))
  .def("__repr__", [](const Vector& v) { return (std::stringstream()<<v).str(); })
  .def("__getitem__", [](const Vector& v, size_t idx) { return v[idx]; })
  .def("__setitem__", [](Vector& v, size_t idx, double val) { v[idx] = val; })
  .def("__add__", [](const Vector& v1, const Vector& v2) { return v1 + v2; })
  .def("__sub__", [](const Vector& v1, const Vector& v2) { return v1 - v2; })
  .def("__mul__", [](const Vector& v1, const Vector& v2) { return v1 * v2; })
  .def("__iadd__", [](Vector& v1, const Vector& v2) { return v1 += v2; })
  .def("__isub__", [](Vector& v1, const Vector& v2) { return v1 -= v2; })
  .def("__mul__", [](const Vector& v, const double x) { return v * x; })
  ;

  // Shapeworks Globals
  py::class_<Matrix44>(m, "Matrix44")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

  // Shapeworks Globals
  py::class_<Matrix>(m, "Matrix")
  .def(py::init<>())
  // .def(py::init<unsigned, unsigned, unsigned>)
  ;

  // Shapeworks Globals
  py::class_<IPoint3>(m, "IPoint3")
  .def(py::init<>())
  .def(py::init([](int x, int y, int z) { return IPoint3({x,y,z}); }))
  .def("__repr__", [](const IPoint3& p) { return (std::stringstream()<<p).str(); })
  .def("__getitem__", [](const IPoint3& p, size_t idx) { return p[idx]; })
  .def("__setitem__", [](IPoint3& p, size_t idx, int val) { p[idx] = val; })
  .def("__add__", [](const IPoint3& p1, const IPoint3& p2) { return p1 + p2; })
  .def("__sub__", [](const IPoint3& p1, const IPoint3& p2) { return p1 - p2; })
  .def("__mul__", [](const IPoint3& p1, const IPoint3& p2) { return p1 * p2; })
  .def("__iadd__", [](IPoint3& p1, const IPoint3& p2) { return p1 += p2; })
  .def("__isub__", [](IPoint3& p1, const IPoint3& p2) { return p1 -= p2; })
  .def("__mul__", [](const IPoint3& p, const double x) { return p * x; })
  .def("__truediv__", [](const IPoint3& p, const double x) { return p / x; })
  .def("__imul__", [](IPoint3& p, const double x) { return p *= x; })
  .def("__itruediv__", [](IPoint3& p, const double x) { return p /= x; })
  ;

  // Shapeworks Globals
  py::class_<FPoint3>(m, "FPoint3")
  .def(py::init<>())
  .def(py::init([](float x, float y, float z) { return Point({x,y,z}); }))
  .def("__repr__", [](const FPoint3& p) { return (std::stringstream()<<p).str(); })
  .def("__getitem__", [](const FPoint3& p, size_t idx) { return p[idx]; })
  .def("__setitem__", [](FPoint3& p, size_t idx, float val) { p[idx] = val; })
  .def("__add__", [](const FPoint3& p1, const FPoint3& p2) { return p1 + p2; })
  .def("__sub__", [](const FPoint3& p1, const FPoint3& p2) { return p1 - p2; })
  .def("__mul__", [](const FPoint3& p1, const FPoint3& p2) { return p1 * p2; })
  .def("__iadd__", [](FPoint3& p1, const FPoint3& p2) { return p1 += p2; })
  .def("__isub__", [](FPoint3& p1, const FPoint3& p2) { return p1 -= p2; })
  .def("__mul__", [](const FPoint3& p, const double x) { return p * x; })
  .def("__truediv__", [](const FPoint3& p, const double x) { return p / x; })
  .def("__imul__", [](FPoint3& p, const double x) { return p *= x; })
  .def("__itruediv__", [](FPoint3& p, const double x) { return p /= x; })
  ;

  // Shapeworks Globals
  // TODO: bind transforms
  // typedef itk::Transform<double, 3> GenericTransform;
  // using GenericTransform = itk::Transform<double, 3>;
  // using TransformPtr = GenericTransform::Pointer;
  // using IdentityTransform = itk::IdentityTransform<double, 3>;
  // using IdentityTransformPtr = IdentityTransform::Pointer;
  // py::object IdentityTransformPtr = py::cast(itk::Transform<double, 3>::New());

  // m.def("TransformPtr", [] { return itk::Transform<double, 3>::New(); });
  // m.def("IdentityTransformPtr", [] { return itk::IdentityTransform<double, 3>::New(); });
  // m.def("AffineTransformPtr", [] { return itk::AffineTransform<double, 3>::New(); });

  // Shapeworks Globals
  m.def("toPoint", py::overload_cast<const Dims &>(&toPoint), "converts Dims to Point");
  m.def("toPoint", py::overload_cast<const Coord &>(&toPoint), "converts Coord to Point");
  m.def("toPoint", py::overload_cast<const Vector &>(&toPoint), "converts Vector to Point");
  m.def("toVector", py::overload_cast<const Dims &>(&toVector), "converts Dims to Vector");
  m.def("toVector", py::overload_cast<const Point &>(&toVector), "converts Point to Vector");
  // m.def("negate", &negate<Point>, "p"_a);
  // m.def("invert", invert, "v"_a);
  // m.def("dot", dot, "a"_a, "b"_a);
  // m.def("cross", cross, "a"_a, "b"_a);

  py::enum_<Axis>(m, "Axis")
  .value("invalid", Axis::invalid)
  .value("X", Axis::X)
  .value("Y", Axis::Y)
  .value("Z", Axis::Z)
  .export_values();
  ;

  m.def("axis_is_valid", py::overload_cast<const Vector &>(&axis_is_valid));
  m.def("axis_is_valid", py::overload_cast<const Axis &>(&axis_is_valid));
  m.def("degToRad", degToRad, "deg"_a);
  
  // ShapeworksUtils
  py::class_<ShapeworksUtils>(m, "ShapeworksUtils")
  .def_static("is_directory",   &ShapeworksUtils::is_directory, "pathname"_a)
  .def_static("getMatrix",      &ShapeworksUtils::getMatrix, "mat"_a)
  .def_static("getOffset",      &ShapeworksUtils::getOffset, "mat"_a)
  // .def_static("connectPipelines",
  //                               &ShapeworksUtils::connectPipelines<itk::VTKImageExport<Image::ImageType::Pointer>, vtkImageImport::New()>, "exporter"_a, "importer"_a)
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
  .def("resample",              py::overload_cast<TransformPtr, Point3, Dims, Vector3, Image::ImageType::DirectionType, Image::InterpolationType>(&Image::resample))
  .def("resample",              py::overload_cast<const Vector&, Image::InterpolationType>(&Image::resample))
  .def("resize",                &Image::resize, "logicalDims"_a, "value"_a=0.0)
  .def("pad",                   py::overload_cast<int, Image::PixelType>(&Image::pad))
  .def("pad",                   py::overload_cast<int, int, int, Image::PixelType>(&Image::pad))
  .def("translate",             &Image::translate, "v"_a)
  .def("scale",                 &Image::scale, "v"_a)
  .def("rotate",                &Image::rotate, "angle"_a, "axis"_a)
  .def("applyTransform",        py::overload_cast<TransformPtr, Image::InterpolationType>(&Image::applyTransform))
  .def("applyTransform",        py::overload_cast<TransformPtr, Point3, Dims, Vector3, Image::ImageType::DirectionType, Image::InterpolationType>(&Image::applyTransform))
  .def("extractLabel",          &Image::extractLabel, "label"_a=1.0)
  .def("closeHoles",            &Image::closeHoles, "foreground"_a=0.0)
  .def("binarize",              &Image::binarize, "minVal"_a=0.0, "maxVal"_a=std::numeric_limits<Image::PixelType>::max(), "innerVal"_a=1.0, "outerVal"_a=0.0)
  .def("computeDT",             &Image::computeDT, "isovalue"_a=0.0)
  .def("applyCurvatureFilter",  &Image::applyCurvatureFilter, "iterations"_a=10)
  .def("applyGradientFilter",   &Image::applyGradientFilter)
  .def("applySigmoidFilter",    &Image::applySigmoidFilter, "alpha"_a=10.0, "beta"_a=10.0)
  .def("applyTPLevelSetFilter", &Image::applyTPLevelSetFilter, "featureImage"_a, "scaling"_a=20.0)
  .def("gaussianBlur",          &Image::gaussianBlur, "sigma"_a=0.0)
  .def("crop",                  &Image::crop, "region"_a)
  .def("clip",                  py::overload_cast<const Point&, const Point&, const Point&, const Image::PixelType>(&Image::clip))
  .def("clip",                  py::overload_cast<const Vector&, const Point&, const Image::PixelType>(&Image::clip))
  .def("setOrigin",             &Image::setOrigin, "origin"_a=Point3({0,0,0}))
  .def("reflect",                &Image::reflect, "axis"_a)
  .def("dims",                  &Image::dims)
  .def("size",                  &Image::size)
  .def("spacing",               &Image::spacing)
  .def("origin",                &Image::origin)
  .def("center",                &Image::center)
  .def("coordsys",              &Image::coordsys)
  .def("centerOfMass",          &Image::centerOfMass, "minVal"_a=0.0, "maxVal"_a=1.0)
  .def("boundingBox",           &Image::boundingBox, "isovalue"_a=1.0)
  .def("logicalToPhysical",     &Image::logicalToPhysical, "v"_a)
  .def("physicalToLogical",     &Image::physicalToLogical, "p"_a)
  .def("compare",               &Image::compare, "other"_a, "verifyall"_a=true, "tolerance"_a=0.0, "precision"_a=1e-12)
  .def_static("getPolyData",    &Image::getPolyData, "image"_a, "isoValue"_a=0.0)
  .def("toMesh",                &Image::toMesh, "isovalue"_a=1.0)
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

  py::enum_<Image::InterpolationType>(m, "InterpolationType")
  .value("Linear", Image::InterpolationType::Linear)
  .value("NearestNeighbor", Image::InterpolationType::NearestNeighbor)
  .export_values();
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
                                &ImageUtils::topologyPreservingSmooth, "image"_a, "scaling"_a=20.0, "sigmoidAlpha"_a=10.5, "sigmoidBeta"_a=10.0)
  .def_static("isoresample",    &ImageUtils::isoresample, "image"_a, "isoSpacing"_a = 1.0, "interp"_a = Image::InterpolationType::Linear)
  ;

  // Mesh
  py::class_<Mesh>(m, "Mesh")
  .def(py::init<const std::string &>())
  .def(py::init<vtkSmartPointer<vtkPolyData>>())
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
  py::class_<ShapeEvaluation>(m, "ShapeEvaluation")
  .def_static("ComputeCompactness",
                                &ShapeEvaluation::ComputeCompactness, "particleSystem"_a, "nModes"_a, "saveTo"_a="")
  .def_static("ComputeGeneralization",
                                &ShapeEvaluation::ComputeGeneralization, "particleSystem"_a, "nModes"_a, "saveTo"_a="")
  .def_static("ComputeSpecificity",
                                &ShapeEvaluation::ComputeSpecificity, "particleSystem"_a, "nModes"_a, "saveTo"_a="")

  ;

  // Optimize (TODO)
  py::class_<Optimize>(m, "Optimize")
  ;
  
  // this is simply a demonstration of creating a submodule, which may not be necessary (could add Groom, Optimize, etc)
  py::module sub_module = m.def_submodule("submodule", "ShapeWorks submodule classes and functions");
  sub_module.def("add", &add, "adds two numbers", "i"_a=1, "j"_a=2);
}

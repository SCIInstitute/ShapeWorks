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

PYBIND11_MODULE(shapeworks, m)
{
  m.doc() = "ShapeWorks Python API";

  m.attr("Pi") = std::atan(1.0) * 4.0;

  // Shapeworks Globals
  py::class_<Coord>(m, "Coord")
  .def(py::init<>())
  .def(py::init<unsigned, unsigned, unsigned>())
  .def("__repr__", [](const Coord& c) {
      std::ostringstream ss;
      ss << c;
      return ss.str();
    })
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
  .def("__repr__", [](const Dims &d) {
      std::ostringstream ss;
      ss << d;
      return ss.str();
    })
  .def("__getitem__", [](const Dims& d, size_t idx) { return d[idx]; })
  .def("__setitem__", [](Dims& d, size_t idx, unsigned val) { d[idx] = val; })
  .def("__add__", [](const Dims& d1, const Dims& d2) { return d1 + d2; })
  .def("__sub__", [](const Dims& d1, const Dims& d2) { return d1 - d2; })
  .def("__mul__", [](const Dims& d1, const Dims& d2) { return d1 * d2; })
  .def("__iadd__", [](Dims& d1, const Dims& d2) { return d1 += d2; })
  .def("__isub__", [](Dims& d1, const Dims& d2) { return d1 -= d2; })
  .def("__mul__", [](const Dims& d, const double x) { return d * x; })
  .def("__truediv__", [](const Dims& d, const double x) { return d / x; })
  .def("__imul__", [](Dims &d, const double x) { return d *= x; })
  .def("__itruediv__", [](Dims &d, const double x) { return d /= x; })
  ;

  // Shapeworks Globals
  py::class_<Point>(m, "Point")
  .def(py::init<>())
  .def(py::init([](double x, double y, double z) { return Point({x,y,z}); }))  // directly call initializer list ctor  (***the winner!***)
  .def("__repr__", [](const Point& p) {
      std::ostringstream ss;
      ss << p;
      return ss.str();
    })
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
  .def("__repr__", [](const Vector& v) {
      std::ostringstream ss;
      ss << v;
      return ss.str();
    })
  .def("__getitem__", [](const Vector& v, size_t idx) { return v[idx]; })
  .def("__setitem__", [](Vector& v, size_t idx, double val) { v[idx] = val; })
  .def("__add__", [](const Vector& v1, const Vector& v2) { return v1 + v2; })
  .def("__sub__", [](const Vector& v1, const Vector& v2) { return v1 - v2; })
  .def("__mul__", [](const Vector& v1, const Vector& v2) { return v1 * v2; })
  .def("__iadd__", [](Vector& v1, const Vector& v2) { return v1 += v2; })
  .def("__isub__", [](Vector& v1, const Vector& v2) { return v1 -= v2; })
  .def("__mul__", [](const Vector& v, const double x) { return v * x; })
  // .def("__truediv__", [](const Vector& v, const double x) { return v / x; })
  // .def("__imul__", [](Vector& v, const double x) { return v *= x; })
  // .def("__itruediv__", [](Vector& v, const double x) { return v /= x; })
  ;

  // Shapeworks Globals
  py::class_<Matrix>(m, "Matrix")
  .def(py::init([] { Matrix m;
    m.SetIdentity();
    return m; 
    }))
  .def("__repr__", [](const Matrix& m) {
      std::ostringstream ss;
      ss << m;
      return ss.str();
    })
  .def("__getitem__", [](const Matrix& m, size_t r, size_t c) { return m[r][c]; })
  .def("__setitem__", [](Matrix& m, size_t r, size_t c, int val) { m[r][c] = val; })
  .def("__add__", [](const Matrix& m1, const Matrix& m2) { return m1 + m2; })
  .def("__sub__", [](const Matrix& m1, const Matrix& m2) { return m1 - m2; })
  .def("__mul__", [](const Matrix& m1, const Matrix& m2) { return m1 * m2; })
  .def("__iadd__", [](Matrix& m1, const Matrix& m2) { return m1 += m2; })
  .def("__isub__", [](Matrix& m1, const Matrix& m2) { return m1 -= m2; })
  .def("__mul__", [](const Matrix& m, const double x) { return m * x; })
  .def("__truediv__", [](const Matrix& m, const double x) { return m / x; })
  .def("__imul__", [](Matrix& m, const double x) { return m *= x; })
  .def("__itruediv__", [](Matrix& m, const double x) { return m /= x; })
  ;

  // Shapeworks Globals
  py::class_<Matrix44>(m, "Matrix44")
  .def(py::init([] { Matrix44 m;
    m.SetIdentity();
    return m; 
    }))
  .def("__repr__", [](const Matrix44& m) {
      std::ostringstream ss;
      ss << m;
      return ss.str();
    })
  .def("__getitem__", [](const Matrix44& m, size_t r, size_t c) { return m[r, c]; })
  .def("__setitem__", [](Matrix44& m, size_t r, size_t c, int val) { m[r][c] = val; })
  .def("__add__", [](const Matrix44& m1, const Matrix44& m2) { return m1 + m2; })
  .def("__sub__", [](const Matrix44& m1, const Matrix44& m2) { return m1 - m2; })
  .def("__mul__", [](const Matrix44& m1, const Matrix44& m2) { return m1 * m2; })
  .def("__iadd__", [](Matrix44& m1, const Matrix44& m2) { return m1 += m2; })
  .def("__isub__", [](Matrix44& m1, const Matrix44& m2) { return m1 -= m2; })
  .def("__mul__", [](const Matrix44& m, const double x) { return m * x; })
  .def("__truediv__", [](const Matrix44& m, const double x) { return m / x; })
  .def("__imul__", [](Matrix44& m, const double x) { return m *= x; })
  .def("__itruediv__", [](Matrix44& m, const double x) { return m /= x; })
  ;

  // Shapeworks Globals
  py::class_<IPoint3>(m, "IPoint3")
  .def(py::init<>())
  .def(py::init([](int x, int y, int z) { return IPoint3({x,y,z}); }))
  .def("__repr__", [](const IPoint3& p) {
      std::ostringstream ss;
      ss << p;
      return ss.str();
    })
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
  .def("__repr__", [](const FPoint3& p) {
      std::ostringstream ss;
      ss << p;
      return ss.str();
    })
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
  py::class_<itk::SmartPointer<itk::Transform<double, 3u, 3u> >>(m, "TransformPtr");

  // Shapeworks Globals
  m.def("createTransform", createTransform, "creates transform from matrix", "mat"_a, "translate"_a=makeVector({0,0,0}));
  m.def("toPoint", py::overload_cast<const Dims &>(toPoint), "converts Dims to Point");
  m.def("toPoint", py::overload_cast<const Coord &>(toPoint), "converts Coord to Point");
  m.def("toPoint", py::overload_cast<const Vector &>(toPoint), "converts Vector to Point");
  m.def("toVector", py::overload_cast<const Dims &>(toVector), "converts Dims to Vector");
  m.def("toVector", py::overload_cast<const Point &>(toVector), "converts Point to Vector");
  m.def("negate", negate<Coord>, "negate function for Coord");
  m.def("negate", negate<Dims>, "negate function for Dims");
  m.def("negate", negate<Point>, "negate function for Point");
  m.def("negate", negate<Vector>, "negate function for Vector");
  m.def("negate", negate<IPoint3>, "negate function for IPoint3");
  m.def("negate", negate<FPoint3>, "negate function for FPoint3");
  m.def("invertValue", invertValue<Point>, "inversion function for Point");
  m.def("invertValue", invertValue<Vector>, "inversion function for Vector");
  m.def("dotProduct", dotProduct, "vector dot product", "a"_a, "b"_a);
  m.def("crossProduct", crossProduct, "vector cross product", "a"_a, "b"_a);

  py::enum_<Axis>(m, "Axis")
  .value("invalid", Axis::invalid)
  .value("X", Axis::X)
  .value("Y", Axis::Y)
  .value("Z", Axis::Z)
  .export_values();
  ;

  m.def("axis_is_valid", py::overload_cast<const Vector &>(&axis_is_valid), "ensure an axis is valid");
  m.def("axis_is_valid", py::overload_cast<const Axis &>(&axis_is_valid), "ensure an axis is valid");
  m.def("degToRad", degToRad, "convert degrees to radians", "deg"_a);
  m.def("toAxis", toAxis, "convert to axis", "str"_a);
  
  // ShapeworksUtils
  py::class_<ShapeworksUtils>(m, "ShapeworksUtils")
  .def_static("is_directory",   &ShapeworksUtils::is_directory, "checks if pathname is a directory", "pathname"_a)
  .def_static("getMatrix",      &ShapeworksUtils::getMatrix, "converts a vtkMatrix4x4 to a Matrix33", "mat"_a)
  .def_static("getOffset",      &ShapeworksUtils::getOffset, "converts a vtkMatrix4x4 to a corresponding translationVector", "mat"_a)
  ;

  // Image::InterpolationType
  py::enum_<Image::InterpolationType>(m, "InterpolationType")
  .value("Linear", Image::InterpolationType::Linear)
  .value("NearestNeighbor", Image::InterpolationType::NearestNeighbor)
  .export_values();
  ;

  // Image
  py::class_<Image>(m, "Image")
  .def(py::init<const std::string &>()) // can the argument for init be named (it's filename in this case)
  .def(py::init<Image::ImageType::Pointer>())
  // .def(py::init<Image &&>())
  .def(py::init<const Image &>())
  .def("__set__",               py::overload_cast<const Image&>(&Image::operator=))
  // .def("__set__",               py::overload_cast<Image&&>(&Image::operator=))
  .def("__neg__", [](Image& img) { return -img; })
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
  .def("write",                 &Image::write, "writes the current image (determines type by its extension)", "filename"_a, "compressed"_a=true)
  .def("antialias",             &Image::antialias, "antialiases binary volumes", "iterations"_a=50, "maxRMSErr"_a=0.01f, "layers"_a=0)
  .def("resample",              py::overload_cast<TransformPtr, Point3, Dims, Vector3, Image::ImageType::DirectionType, Image::InterpolationType>(&Image::resample), "resamples by applying transform then sampling from given origin along direction axes at spacing physical units per pixel for dims pixels using specified interpolator")
  .def("resample",              py::overload_cast<const Vector&, Image::InterpolationType>(&Image::resample), "resamples image using new physical spacing, updating logical dims to keep all image data for this spacing")
  .def("resize",                &Image::resize, "resizes an image (computes new physical spacing)", "logicalDims"_a, "interp"_a=Image::InterpolationType::Linear)
  .def("recenter",              &Image::recenter, "recenters an image by changing its origin in the image header to the physical coordinates of the center of the image")
  .def("pad",                   py::overload_cast<int, Image::PixelType>(&Image::pad), "pads an image with specified value by specified number of voxels in the x-, y-, and/or z- directions; origin remains at the same location (note: negative padding to shrink an image is permitted)")
  .def("pad",                   py::overload_cast<int, int, int, Image::PixelType>(&Image::pad), "pads an image with specified value by specified number of voxels in the x-, y-, and/or z- directions; origin remains at the same location (note: negative padding to shrink an image is permitted)")
  .def("translate",             &Image::translate, "translates image", "v"_a)
  .def("scale",                 &Image::scale, "scale image around center (not origin)", "v"_a)
  .def("rotate",                &Image::rotate, "rotate around center (not origin) using axis (default z-axis) by angle (in radians)", "angle"_a, "axis"_a)
  .def("applyTransform",        py::overload_cast<TransformPtr, Image::InterpolationType>(&Image::applyTransform), "applies the given transformation to the image by using resampling filter")
  .def("applyTransform",        py::overload_cast<TransformPtr, Point3, Dims, Vector3, Image::ImageType::DirectionType, Image::InterpolationType>(&Image::applyTransform), "applies the given transformation to the image by using resampling filter")
  .def("extractLabel",          &Image::extractLabel, "extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image", "label"_a=1.0)
  .def("closeHoles",            &Image::closeHoles, "closes holes in a volume defined by values larger than specified value", "foreground"_a=0.0)
  .def("binarize",              &Image::binarize, "sets portion of image greater than min and less than or equal to max to the specified value", "minVal"_a=0.0, "maxVal"_a=std::numeric_limits<Image::PixelType>::max(), "innerVal"_a=1.0, "outerVal"_a=0.0)
  .def("computeDT",             &Image::computeDT, "computes signed distance transform volume from an image at the specified isovalue", "isovalue"_a=0.0)
  .def("applyCurvatureFilter",  &Image::applyCurvatureFilter, "denoises an image using curvature driven flow using curvature flow image filter", "iterations"_a=10)
  .def("applyGradientFilter",   &Image::applyGradientFilter, "computes gradient magnitude of an image region at each pixel using gradient magnitude filter")
  .def("applySigmoidFilter",    &Image::applySigmoidFilter, "computes sigmoid function pixel-wise using sigmoid image filter", "alpha"_a=10.0, "beta"_a=10.0)
  .def("applyTPLevelSetFilter", &Image::applyTPLevelSetFilter, "segments structures in image using topology preserving geodesic active contour level set filter", "featureImage"_a, "scaling"_a=20.0)
  .def("gaussianBlur",          &Image::gaussianBlur, "applies gaussian blur", "sigma"_a=0.0)
  .def("crop",                  &Image::crop, "crop image down to the current region (e.g., from bounding-box), or the specified min/max in each direction", "region"_a)
  .def("clip",                  py::overload_cast<const Point&, const Point&, const Point&, const Image::PixelType>(&Image::clip), "sets values on the back side of cutting plane (containing three non-colinear points) to val (default 0.0)")
  .def("clip",                  py::overload_cast<const Vector&, const Point&, const Image::PixelType>(&Image::clip), "sets values on the back side of cutting plane (normal n containing point p) to val (default 0.0)")
  .def("setOrigin",             &Image::setOrigin, "sets the image origin in physical space to the given value", "origin"_a=Point3({0,0,0}))
  .def("reflect",               &Image::reflect, "reflect image with respect to logical image center and the specified axis", "axis"_a)
  .def("dims",                  &Image::dims, "logical dimensions of the image")
  .def("size",                  &Image::size, "physical dimensions of the image (dims * spacing)")
  .def("spacing",               &Image::spacing, "physical spacing of the image")
  .def("origin",                &Image::origin, "physical coordinates of image origin")
  .def("center",                &Image::center, "physical coordinates of center of this image")
  .def("coordsys",              &Image::coordsys, "return coordinate system in which this image lives in physical space")
  .def("centerOfMass",          &Image::centerOfMass, "returns average physical coordinate of pixels in range (minval, maxval]", "minVal"_a=0.0, "maxVal"_a=1.0)
  .def("boundingBox",           &Image::boundingBox, "computes the logical coordinates of the largest region of data <= the given isoValue", "isovalue"_a=1.0)
  .def("logicalToPhysical",     &Image::logicalToPhysical, "converts from pixel coordinates to physical space", "v"_a)
  .def("physicalToLogical",     &Image::physicalToLogical, "converts from a physical coordinate to a logical coordinate", "p"_a)
  .def("compare",               &Image::compare, "compares two images", "other"_a, "verifyall"_a=true, "tolerance"_a=0.0, "precision"_a=1e-12)
  .def_static("getPolyData",    &Image::getPolyData, "creates a vtkPolyData for the given image", "image"_a, "isoValue"_a=0.0)
  .def("toMesh",                &Image::toMesh, "converts to Mesh", "isovalue"_a=1.0)
  // .def("__repr__",              operator<<)

  // Try to give the Python direct access to the underlying ITK image; see issue #780
  // .def("toITKImage",            &Image::operator Image::ImageType::Pointer) // cannot convert to itk::SmartPointer<itk::Image...
  // .def("toITKImage",            &Image::getITKImage) // can't convert return value to a Python type! itk::Image<float, 3u>
  // .def("toITKImage",            [](Image& I) { return *(static_cast<Image::ImageType::Pointer>(I); }) // ugly template errors
  // .def("toITKImage",            [](Image& I) { return *(I.operator Image::ImageType::Pointer()); })   // same errors
  // .def("toITKImage",            []() { return Image::ImageType::Pointer(); })
  ;

  // Image::Region
  py::class_<Image::Region>(m, "Region")
  .def(py::init<Dims>())
  .def(py::init<Coord, Coord>())
  .def(py::init<>())
  .def(py::self == py::self)
  .def_readwrite("min",         &Image::Region::min)
  .def_readwrite("max",         &Image::Region::max)
  .def("valid",                 &Image::Region::valid, "ensure if region is valid")
  .def("origin",                &Image::Region::origin, "return origin of region")
  .def("size",                  &Image::Region::size, "return size of region")
  .def("clip",                  &Image::Region::clip, "clip region to fit inside image", "image"_a)
  .def("pad",                   &Image::Region::pad, "grows or shrinks the region by the specified amount", "padding"_a)
  .def("shrink",                &Image::Region::shrink, "shrink this region down to the smallest portions of both", "other"_a)
  .def("grow",                  &Image::Region::grow, "grow this region up to the largest portions of both", "other"_a)
  .def("expand",                &Image::Region::expand, "expand this region to include this point", "other"_a)
  ;

  // ImageUtils
  py::class_<ImageUtils>(m, "ImageUtils")
  .def_static("boundingBox",    &ImageUtils::boundingBox, "compute largest bounding box surrounding the specified isovalue of the specified set of images", "filenames"_a, "isoValue"_a=1.0)
  .def_static("createCenterOfMassTransform", [](const Image& img){
    auto xform_ptr = shapeworks::ImageUtils::createCenterOfMassTransform(img);
    return xform_ptr;
  }, "generates the Transform necessary to move the contents of this binary image to the center")
  .def_static("createRigidRegistrationTransform", [](const Image& source_dt, const Image& target_dt, float isoValue=0.0, unsigned iterations=20){
    auto xform_ptr = shapeworks::ImageUtils::createRigidRegistrationTransform(source_dt, target_dt, isoValue, iterations);
    return xform_ptr;
  }, "creates transform from source distance map to target using ICP registration (isovalue is used to create meshes from dts passed to ICP)")
  .def_static("createWarpTransform", [](const std::string &source_landmarks, const std::string &target_landmarks, const int stride=1){
    auto xform_ptr = shapeworks::ImageUtils::createWarpTransform(source_landmarks, target_landmarks, stride);
    return xform_ptr;
  }, "computes a warp transform from the source to the target landmarks")
  .def_static("topologyPreservingSmooth", 
                                &ImageUtils::topologyPreservingSmooth, "creates a feature image (by applying gradient then sigmoid filters), then passes it to the TPLevelSet filter [curvature flow filter is often applied to the image before this filter]",
                                "image"_a, "scaling"_a=20.0, "sigmoidAlpha"_a=10.5, "sigmoidBeta"_a=10.0)
  .def_static("isoresample",    &ImageUtils::isoresample, "create an isotropic resampling of the given image volume", "image"_a, "isoSpacing"_a=1.0, "interp"_a=Image::InterpolationType::Linear)
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
  // sub_module.def("add", &add, "adds two numbers", "i"_a=1, "j"_a=2);
}

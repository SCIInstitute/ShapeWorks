#include <Eigen/Eigen>

#include <Libs/Optimize/Optimize.h>

Eigen::MatrixXd optimize_get_particle_system(shapeworks::Optimize *opt)
{
  shapeworks::MatrixContainer container = opt->GetParticleSystem();
  return container.matrix_;
}

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
//#include <pybind11/stl_bind.h>  // look at Binding STL containers portion of manual; not sure we even use any in ShapeWorks
#include <pybind11/operators.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include <sstream>
#include <itkImportImageFilter.h>
#include <vtkDoubleArray.h>

#include "Shapeworks.h"
#include "ShapeworksUtils.h"
#include "Image.h"
#include "VectorImage.h"
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
  .def(py::init<long, long, long>())
  .def("__repr__", [](const Coord& c) {
    std::ostringstream ss;
    ss << c;
    return ss.str();
  })
  .def("__getitem__", [](const Coord& c, size_t idx) { return c[idx]; })
  .def("__setitem__", [](Coord& c, size_t idx, unsigned val) { c[idx] = val; })
  .def("__add__", [](const Coord& c1, const Coord& c2) { return c1 + c2; })
  .def("__sub__", [](const Coord& c1, const Coord& c2) {
    Coord c;
    c[0] = c1[0] - c2[0];
    c[1] = c1[1] - c2[1];
    c[2] = c1[2] - c2[2];
    return c;
  })
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
  .def("__getitem__", [](const Point& p, size_t idx) { return p[idx]; })
  .def("__setitem__", [](Point& p, size_t idx, double val) { p[idx] = val; })
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

  // Shapeworks Globals
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
  .def("__truediv__", [](const Vector& v, const double x) {
    Vector v_;
    v_[0] = v[0]/x;
    v_[1] = v[1]/x;
    v_[2] = v[2]/x;
    return v_;
  })
  .def("__imul__", [](Vector& v, const double x) {
    v[0] = v[0]*x;
    v[1] = v[1]*x;
    v[2] = v[2]*x;
    return v;
   })
  .def("__itruediv__", [](Vector& v, const double x) {
    v[0] = v[0]/x;
    v[1] = v[1]/x;
    v[2] = v[2]/x;
    return v;
   })
  ;

  // Shapeworks Globals
  py::class_<Matrix>(m, "Matrix")
  .def(py::init([] {
    Matrix mat;
    mat.SetIdentity();
    return mat;
  }))
  .def("__repr__", [](const Matrix& mat) {
    std::ostringstream ss;
    ss << mat;
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
  .def(py::init([] {
    Matrix44 mat;
    mat.SetIdentity();
    return mat;
  }))
  .def("__repr__", [](const Matrix44& mat) {
    std::ostringstream ss;
    ss << mat;
    return ss.str();
  })
  .def("__getitem__", [](const Matrix44& m, size_t r, size_t c) { return m[r][c]; })
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
  .def("__sub__", [](const IPoint3& p1, const IPoint3& p2) { return IPoint3(p1 - p2); })
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
  .def("__sub__", [](const FPoint3& p1, const FPoint3& p2) { return FPoint3(p1 - p2); })
  .def("__mul__", [](const FPoint3& p1, const FPoint3& p2) { return p1 * p2; })
  .def("__iadd__", [](FPoint3& p1, const FPoint3& p2) { return p1 += p2; })
  .def("__isub__", [](FPoint3& p1, const FPoint3& p2) { return p1 -= p2; })
  .def("__mul__", [](const FPoint3& p, const double x) { return p * x; })
  .def("__truediv__", [](const FPoint3& p, const double x) { return p / x; })
  .def("__imul__", [](FPoint3& p, const double x) { return p *= x; })
  .def("__itruediv__", [](FPoint3& p, const double x) { return p /= x; })
  ;

  // Shapeworks Globals
  py::class_<itk::SmartPointer<itk::Transform<double, 3u, 3u> >>(m, "TransformPtr")
  .def("__repr__", [](const TransformPtr &transform) {
    std::stringstream stream;
    itk::Transform<double, 3, 3>::ParametersType p = transform->GetParameters();
    int r = 0;
    for (int i=0; i<4; i++)
    {
      stream << "\n";
      for (int j=0; j<3; j++)
      {
        stream << p[r] << " ";
        r++;
      }
    }
    return stream.str();
  })
  ;

  py::class_<Plane>(m, "Plane")
  .def(py::init([](std::vector<double>& n, std::vector<double>& o) {
    return makePlane(makeVector({n[0], n[1], n[2]}), Point({o[0], o[1], o[2]}));
  }))
  ;

  // Shapeworks Globals
  m.def("createTransform", createTransform, "creates transform from matrix", "mat"_a, "translate"_a=makeVector({0,0,0}));
  m.def("toPoint", py::overload_cast<const Dims &>(toPoint), "converts Dims to Point", "d"_a);
  m.def("toPoint", py::overload_cast<const Coord &>(toPoint), "converts Coord to Point", "c"_a);
  m.def("toPoint", py::overload_cast<const Vector &>(toPoint), "converts Vector to Point", "d"_a);
  m.def("toVector", py::overload_cast<const Dims &>(toVector), "converts Dims to Vector", "p"_a);
  m.def("toVector", py::overload_cast<const Point &>(toVector), "converts Point to Vector", "v"_a);
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

  m.def("axis_is_valid", py::overload_cast<const Vector &>(&axis_is_valid), "ensure an axis is valid", "axis"_a);
  m.def("axis_is_valid", py::overload_cast<const Axis &>(&axis_is_valid), "ensure an axis is valid", "axis"_a);
  m.def("degToRad", degToRad, "convert degrees to radians", "deg"_a);
  m.def("toAxis", toAxis, "convert to axis", "str"_a);

  // ShapeworksUtils
  py::class_<ShapeworksUtils>(m, "ShapeworksUtils")
  .def_static("is_directory",   &ShapeworksUtils::is_directory, "checks if pathname is a directory", "pathname"_a)
  .def_static("getMatrix",      &ShapeworksUtils::getMatrix, "converts a vtkMatrix4x4 to a Matrix33", "mat"_a)
  .def_static("getOffset",      &ShapeworksUtils::getOffset, "converts a vtkMatrix4x4 to a corresponding translationVector", "mat"_a)
  ;

  // TransformType
  py::enum_<XFormType>(m, "TransformType")
  .value("CenterOfMass", XFormType::CenterOfMass)
  .value("IterativeClosestPoint", XFormType::IterativeClosestPoint)
  .export_values();
  ;

  // Image
  py::class_<Image> image(m, "Image");

  // Image::InterpolationType (even though this is part of Image, it feels cleaner to keep it global in the module)
  py::enum_<Image::InterpolationType>(m, "InterpolationType")
  .value("Linear", Image::InterpolationType::Linear)
  .value("NearestNeighbor", Image::InterpolationType::NearestNeighbor)
  .export_values();
  ;

  // Image bindings
  image.def(py::init<const std::string &>()) // can the argument for init be named (it's filename in this case)
  .def(py::init<Image::ImageType::Pointer>())
  .def(py::init([](py::array_t<long> np_array) {  // FIXME: this is broken (or not even called)
    using ImportType = itk::ImportImageFilter<Image::PixelType, 3>;
    auto importer = ImportType::New();
    auto info = np_array.request();
    importer->SetImportPointer(static_cast<float *>(info.ptr), np_array.size(), false);

    ImportType::SizeType size;
    size[0] = np_array.shape()[2];
    size[1] = np_array.shape()[1];
    size[2] = np_array.shape()[0];

    ImportType::IndexType start;
    start.assign(*np_array.data());

    ImportType::RegionType region;
    region.SetIndex(start);
    region.SetSize(size);

    importer->SetRegion(region);
    importer->Update();
    return Image(importer->GetOutput());
  }))
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
  .def("__repr__", [](const Image &img) {
    std::stringstream stream;
    stream << img;
    return stream.str();
  })
  .def("copy",                  [](Image& image) { return Image(image); })
  .def("write",                 &Image::write, "writes the current image (determines type by its extension)", "filename"_a, "compressed"_a=true)
  .def("antialias",             &Image::antialias, "antialiases binary volumes (layers is set to 3 when not specified)", "iterations"_a=50, "maxRMSErr"_a=0.01f, "layers"_a=3)
  .def("resample",              py::overload_cast<TransformPtr, Point3, Dims, Vector3, Image::ImageType::DirectionType, Image::InterpolationType>(&Image::resample), "resamples by applying transform then sampling from given origin along direction axes at spacing physical units per pixel for dims pixels using specified interpolator", "transform"_a, "origin"_a, "dims"_a, "spacing"_a, "direction"_a, "interp"_a=Image::InterpolationType::NearestNeighbor)
  .def("resample",              py::overload_cast<const Vector&, Image::InterpolationType>(&Image::resample), "resamples image using new physical spacing, updating logical dims to keep all image data for this spacing", "physicalSpacing"_a, "interp"_a=Image::InterpolationType::Linear)
  .def("resample", [](Image& image, const TransformPtr transform, const std::vector<double>& p, const std::vector<unsigned>& d, const std::vector<double>& v, const Image::ImageType::DirectionType direction, Image::InterpolationType interp) {
    return image.resample(transform, Point({p[0], p[1], p[2]}), Dims({d[0], d[1], d[2]}), makeVector({v[0], v[1], v[2]}), direction, interp);
  }, "resamples by applying transform then sampling from given origin along direction axes at spacing physical units per pixel for dims pixels using specified interpolator", "transform"_a, "origin"_a, "dims"_a, "spacing"_a, "direction"_a, "interp"_a=Image::InterpolationType::NearestNeighbor)
  .def("resample", [](Image& image, const std::vector<double>& v, Image::InterpolationType interp) {
    return image.resample(makeVector({v[0], v[1], v[2]}), interp);
  }, "resamples image using new physical spacing, updating logical dims to keep all image data for this spacing", "physicalSpacing"_a, "interp"_a=Image::InterpolationType::Linear)
  .def("resample",              py::overload_cast<double, Image::InterpolationType>(&Image::resample), "isotropically resamples image using giving isospacing", "isoSpacing"_a=1.0, "interp"_a=Image::InterpolationType::Linear)
  .def("resize",                &Image::resize, "change logical dims (computes new physical spacing)", "logicalDims"_a, "interp"_a=Image::InterpolationType::Linear)
  .def("resize", [](Image& image, std::vector<unsigned>& d, Image::InterpolationType interp) {
    return image.resize(Dims({d[0], d[1], d[2]}), interp);
  }, "change logical dims (computes new physical spacing)", "logicalDims"_a, "interp"_a=Image::InterpolationType::Linear)
  .def("recenter",              &Image::recenter, "recenters an image by changing its origin in the image header to the physical coordinates of the center of the image")
  .def("pad",                   py::overload_cast<int, Image::PixelType>(&Image::pad), "pads an image in all directions with constant value", "pad"_a, "value"_a=0.0)
  .def("pad",                   py::overload_cast<int, int, int, Image::PixelType>(&Image::pad), "pads an image by desired number of voxels in each direction with constant value", "padx"_a, "pady"_a, "padz"_a, "value"_a=0.0)
  .def("translate",             &Image::translate, "translates image", "v"_a)
  .def("translate", [](Image& image, const std::vector<double>& v) {
    return image.translate(makeVector({v[0], v[1], v[2]}));
  }, "translates image", "v"_a)
  .def("scale",                 &Image::scale, "scale image around center (not origin)", "v"_a)
  .def("scale", [](Image& image, const std::vector<double>& v) {
    return image.scale(makeVector({v[0], v[1], v[2]}));
  }, "scale image around center (not origin)", "v"_a)
  .def("rotate",                py::overload_cast<const double, const Vector3&>(&Image::rotate), "rotate around center (not origin) using axis (default z-axis) by angle (in radians)", "angle"_a, "axis"_a)
  .def("rotate",                py::overload_cast<const double, Axis>(&Image::rotate), "rotate around center (not origin) using axis (default z-axis) by angle (in radians)", "angle"_a, "axis"_a)
  .def("rotate", [](Image& image, const double angle, const std::vector<double>& v) {
    return image.rotate(angle, makeVector({v[0], v[1], v[2]}));
  }, "rotate around center (not origin) using axis (default z-axis) by angle (in radians)", "angle"_a, "axis"_a)
  .def("applyTransform",        py::overload_cast<TransformPtr, Image::InterpolationType>(&Image::applyTransform), "applies the given transformation to the image by using resampling filter", "transform"_a, "interp"_a=Image::InterpolationType::Linear)
  .def("applyTransform",        py::overload_cast<TransformPtr, Point3, Dims, Vector3, Image::ImageType::DirectionType, Image::InterpolationType>(&Image::applyTransform), "applies the given transformation to the image by using resampling filter with new origin, dims, spacing and direction values", "transform"_a, "origin"_a, "dims"_a, "spacing"_a, "direction"_a, "interp"_a=Image::InterpolationType::NearestNeighbor)
  .def("applyTransform", [](Image& image, const TransformPtr transform, const std::vector<double>& p, const std::vector<unsigned>& d, const std::vector<double>& v, const Image::ImageType::DirectionType direction, Image::InterpolationType interp) {
    return image.applyTransform(transform, Point({p[0], p[1], p[2]}), Dims({d[0], d[1], d[2]}), makeVector({v[0], v[1], v[2]}), direction, interp);
  }, "applies the given transformation to the image by using resampling filter with new origin, dims, spacing and direction values", "transform"_a, "origin"_a, "dims"_a, "spacing"_a, "direction"_a, "interp"_a=Image::InterpolationType::NearestNeighbor)
  .def("extractLabel",          &Image::extractLabel, "extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image", "label"_a=1.0)
  .def("closeHoles",            &Image::closeHoles, "closes holes in a volume defined by values larger than specified value", "foreground"_a=0.0)
  .def("binarize",              &Image::binarize, "sets portion of image greater than min and less than or equal to max to the specified value", "minVal"_a=0.0, "maxVal"_a=std::numeric_limits<Image::PixelType>::max(), "innerVal"_a=1.0, "outerVal"_a=0.0)
  .def("computeDT",             &Image::computeDT, "computes signed distance transform volume from an image at the specified isovalue", "isovalue"_a=0.0)
  .def("applyCurvatureFilter",  &Image::applyCurvatureFilter, "denoises an image using curvature driven flow using curvature flow image filter", "iterations"_a=10)
  .def("applyGradientFilter",   &Image::applyGradientFilter, "computes gradient magnitude of an image region at each pixel using gradient magnitude filter")
  .def("applySigmoidFilter",    &Image::applySigmoidFilter, "computes sigmoid function pixel-wise using sigmoid image filter", "alpha"_a=10.0, "beta"_a=10.0)
  .def("applyTPLevelSetFilter", &Image::applyTPLevelSetFilter, "segments structures in image using topology preserving geodesic active contour level set filter", "featureImage"_a, "scaling"_a=20.0)
  .def("applyIntensityFilter",  &Image::applyIntensityFilter, "applies intensity windowing image filter", "min"_a=0.0, "max"_a=0.0)
  .def("gaussianBlur",          &Image::gaussianBlur, "applies gaussian blur", "sigma"_a=0.0)
  .def("crop",                  &Image::crop, "crop image down to the current region (e.g., from bounding-box), or the specified min/max in each direction", "region"_a)
  .def("clip", [](Image& image, const std::vector<double>& o, std::vector<double>& p1, std::vector<double>& p2, const Image::PixelType val) {
    return image.clip(Point({o[0], o[1], o[2]}), Point({p1[0], p1[1], p1[2]}), Point({p2[0], p2[1], p2[2]}), val);
  }, "sets values on the back side of cutting plane (containing three non-colinear points) to val (default 0.0)", "o"_a, "p1"_a, "p2"_a, "val"_a=0.0)
  .def("clip", [](Image& image, const std::vector<double>& n, std::vector<double>& q, const Image::PixelType val) {
    return image.clip(makeVector({n[0], n[1], n[2]}), Point({q[0], q[1], q[2]}), val);
  }, "sets values on the back side of cutting plane (normal n containing point p) to val (default 0.0)", "n"_a, "q"_a, "val"_a=0.0)
  .def("setOrigin",             &Image::setOrigin, "sets the image origin in physical space to the given value", "origin"_a=Point({0,0,0}))
  .def("setOrigin",
       [](Image& image, std::vector<double>& p) {
         return image.setOrigin(Point({p[0], p[1], p[2]}));
       },
       "sets the image origin in physical space to the given value",
       "origin"_a=std::vector<double>({0,0,0}))
  .def("setSpacing",            &Image::setSpacing, "sets the image spacing to the given value", "spacing"_a=makeVector({1.0, 1.0, 1.0}))
  .def("setSpacing",
       [](Image& image, std::vector<double>& v) {
         return image.setSpacing(makeVector({v[0], v[1], v[2]}));
       },
       "sets the image spacing to the given value",
       "spacing"_a=std::vector<double>({1.0, 1.0, 1.0}))
  .def("reflect",               &Image::reflect, "reflect image with respect to logical image center and the specified axis", "axis"_a)
  .def("dims",                  &Image::dims, "logical dimensions of the image")
  .def("size",                  &Image::size, "physical dimensions of the image (dims * spacing)")
  .def("spacing",               &Image::spacing, "physical spacing of the image")
  .def("origin",                &Image::origin, "physical coordinates of image origin")
  .def("center",                &Image::center, "physical coordinates of center of this image")
  .def("coordsys",              &Image::coordsys, "return coordinate system in which this image lives in physical space")
  .def("centerOfMass",          &Image::centerOfMass, "returns average physical coordinate of pixels in range (minval, maxval]", "minVal"_a=0.0, "maxVal"_a=1.0)
  .def("min",                   &Image::min, "minimum of image")
  .def("max",                   &Image::max, "maximum of image")
  .def("mean",                  &Image::mean, "mean of image")
  .def("std",                   &Image::std, "standard deviation of image")
  .def("boundingBox",           &Image::boundingBox, "computes the logical coordinates of the largest region of data <= the given isoValue", "isovalue"_a=1.0)
  .def("logicalToPhysical", [](Image& image, std::vector<long>& c) {
    return image.logicalToPhysical(Coord({c[0], c[1], c[2]}));
  }, "converts from pixel coordinates to physical space", "c"_a)
  .def("physicalToLogical", [](Image& image, std::vector<double>& p) {
    return image.physicalToLogical(Point({p[0], p[1], p[2]}));
  }, "converts from a physical coordinate to a logical coordinate", "p"_a)
  .def("createTransform",       py::overload_cast<XFormType>(&Image::createTransform), "creates a transform based on transform type", "type"_a=XFormType::CenterOfMass)
  .def("createTransform",       py::overload_cast<const Image&, XFormType, float, unsigned>(&Image::createTransform), "creates a transform based on transform type", "target"_a, "type"_a=XFormType::IterativeClosestPoint, "isoValue"_a=0.0, "iterations"_a=20)
  .def("topologyPreservingSmooth",
       &Image::topologyPreservingSmooth,
       "creates a feature image (by applying gradient then sigmoid filters), then passes it to the TPLevelSet filter [curvature flow filter is often applied to the image before this filter]",
       "scaling"_a=20.0, "sigmoidAlpha"_a=10.5, "sigmoidBeta"_a=10.0)
  .def("compare",               &Image::compare, "compares two images", "other"_a, "verifyall"_a=true, "tolerance"_a=0.0, "precision"_a=1e-12)
  .def("toMesh", [](Image &image, Image::PixelType isovalue) {
    return image.toMesh(isovalue);
  }, "converts image to mesh", "isovalue"_a)
  .def("toArray", [](const Image &image) {
    Image::ImageType::Pointer img = image.getITKImage();
    const auto size = img->GetLargestPossibleRegion().GetSize();
    const auto shape = std::vector<size_t>{size[2], size[1], size[0]};
    return py::array(py::dtype::of<typename Image::ImageType::Pointer::ObjectType::PixelType>(), shape, img->GetBufferPointer());
  })
  ;

  // Region
  py::class_<Region>(m, "Region")
  .def(py::init<Dims>())
  .def(py::init<Coord, Coord>())
  .def(py::init<>())
  .def(py::self == py::self)
  .def("__repr__", [](const Region &region) {
    std::stringstream stream;
    stream << region;
    return stream.str();
  })
  .def_readwrite("min",         &Region::min)
  .def_readwrite("max",         &Region::max)
  .def("valid",                 &Region::valid, "ensure if region is valid")
  .def("origin",                &Region::origin, "return origin of region")
  .def("size",                  &Region::size, "return size of region")
  .def("pad",                   &Region::pad, "grows or shrinks the region by the specified amount", "padding"_a)
  .def("shrink",                &Region::shrink, "shrink this region down to the smallest portions of both", "other"_a)
  .def("grow",                  &Region::grow, "grow this region up to the largest portions of both", "other"_a)
  .def("expand",                &Region::expand, "expand this region to include this point", "other"_a)
  ;

  // VectorImage
  py::class_<VectorImage>(m, "VectorImage")
  .def(py::init([](const Image &dt) {
                  return VectorImage(dt);
                }),
      "create a vector image from an image (usually a distance transform) that can be sampled at any point in space",
      "image"_a)
  .def("evaluate",
       [](VectorImage &image, std::vector<double> &pt) {
         auto v = image.evaluate(Point({pt[0], pt[1], pt[2]}));
         return std::vector<double>({v[0], v[1], v[2]});
       },
       "evaluate the vector image at any given point in space",
       "pt"_a)
  ;

  // ImageUtils
  py::class_<ImageUtils>(m, "ImageUtils")
  .def_static("boundingBox", [](std::vector<std::string> filenames, Image::PixelType val) {
    return shapeworks::ImageUtils::boundingBox(filenames, val);
  }, "compute largest bounding box surrounding the specified isovalue of the specified set of filenames", "filenames"_a, "isoValue"_a=1.0)
  .def_static("boundingBox", [](std::vector<Image> images, Image::PixelType val) {
    return shapeworks::ImageUtils::boundingBox(images, val);
  }, "compute largest bounding box surrounding the specified isovalue of the specified set of images", "images"_a, "isoValue"_a=1.0)
  .def_static("createWarpTransform", [](const std::string &source_landmarks, const std::string &target_landmarks, const int stride) {
    auto xform_ptr = shapeworks::ImageUtils::createWarpTransform(source_landmarks, target_landmarks, stride);
    return xform_ptr;
  }, "computes a warp transform from the source to the target landmarks", "source_landmarks"_a, "target_landmarks"_a, "stride"_a=1)
  ;

  // Mesh
  py::class_<Mesh> mesh(m, "Mesh");

  // Mesh::AlignmentType
  py::enum_<Mesh::AlignmentType>(mesh, "AlignmentType")
  .value("Rigid", Mesh::AlignmentType::Rigid)
  .value("Similarity", Mesh::AlignmentType::Similarity)
  .value("Affine", Mesh::AlignmentType::Affine)
  .export_values();
  ;

  // Mesh::DistanceMethod
  py::enum_<Mesh::DistanceMethod>(mesh, "DistanceMethod")
  .value("POINT_TO_POINT", Mesh::DistanceMethod::POINT_TO_POINT)
  .value("POINT_TO_CELL", Mesh::DistanceMethod::POINT_TO_CELL)
  .export_values();
  ;

  // Mesh bindings
  mesh.def(py::init<const std::string &>())
  .def(py::init<vtkSmartPointer<vtkPolyData>>())
  .def(py::self == py::self)

  .def("__repr__",
       [](const Mesh &mesh) -> decltype(auto) {
         std::stringstream stream;
         stream << mesh;
         return stream.str();
       })

  .def("copy",
       [](Mesh& mesh) -> decltype(auto) { return Mesh(mesh); })

  .def("write",
       &Mesh::write,
       "writes mesh, format specified by filename extension",
       "pathname"_a)

  .def("coverage",
       &Mesh::coverage,
       "determines coverage between current mesh and another mesh (e.g. acetabular cup / femoral head)",
       "otherMesh"_a, "allowBackIntersections"_a=true, "angleThreshold"_a=0, "backSearchRadius"_a=0)

  .def("smooth",
       &Mesh::smooth,
       "applies laplacian smoothing",
       "iterations"_a=0, "relaxation"_a=0.0)

  .def("decimate",
       &Mesh::decimate,
       "applies filter to reduce number of triangles in mesh",
       "reduction"_a=0.5, "angle"_a=15.0, "preserveTopology"_a=true)

  .def("invertNormals",
       &Mesh::invertNormals,
       "handle flipping normals")

  .def("reflect",
       [](Mesh& mesh, const Axis &axis, std::vector<double>& v) -> decltype(auto) {
         return mesh.reflect(axis, makeVector({v[0], v[1], v[2]}));
       },
       "reflect meshes with respect to a specified center and specific axis"
       , "axis"_a, "origin"_a=std::vector<double>({0.0, 0.0, 0.0}))
  .def("createTransform",       &Mesh::createTransform, "creates a transform based on transform type", "target"_a, "type"_a=XFormType::IterativeClosestPoint, "align"_a=Mesh::AlignmentType::Similarity, "iterations"_a=10)
  .def("applyTransform",        &Mesh::applyTransform, "applies the given transformation to the mesh", "transform"_a)
  .def("fillHoles",             &Mesh::fillHoles, "finds holes in a mesh and closes them")
  .def("probeVolume",           &Mesh::probeVolume, "samples data values at specified point locations", "image"_a)
  .def("clip",                  &Mesh::clip, "clips a mesh using a cutting plane", "plane"_a)
  .def("translate", [](Mesh& mesh, const std::vector<double>& v) -> decltype(auto) {
    return mesh.translate(makeVector({v[0], v[1], v[2]}));
  }, "translates mesh", "v"_a)
  .def("scale", [](Mesh& mesh, const std::vector<double>& v) -> decltype(auto) {
    return mesh.scale(makeVector({v[0], v[1], v[2]}));
  }, "scale mesh", "v"_a)
  .def("boundingBox",           &Mesh::boundingBox, "computes bounding box of current mesh", "center"_a=false)
  .def("fix",                   &Mesh::fix, "quality control mesh", "smoothBefore"_a=true, "smoothAfter"_a=true, "lambda"_a=0.5, "iterations"_a=1, "decimate"_a=true, "percentage"_a=0.5)
  .def("clipClosedSurface",     &Mesh::clipClosedSurface, "clips a mesh using a cutting plane resulting in a closed surface", "plane"_a)
  .def("generateNormals",       &Mesh::generateNormals, "computes cell normals and orients them such that they point in the same direction")
  .def("toImage",
       [](Mesh& mesh, std::vector<double>& v, std::vector<unsigned>& d, std::vector<double>& p) -> decltype(auto) {
         return mesh.toImage(makeVector({v[0], v[1], v[2]}), Dims({d[0], d[1], d[2]}), Point({p[0], p[1], p[2]}));
       },
       "rasterizes mesh to create binary images, automatically computing size and origin if necessary",
       "spacing"_a=std::vector<double>({1.0, 1.0, 1.0}), "size"_a=std::vector<unsigned>({0, 0, 0}), "origin"_a=std::vector<double>({-1.0, -1.0, -1.0}))
  .def("distance",              &Mesh::distance, "computes surface to surface distance", "target"_a, "method"_a=Mesh::DistanceMethod::POINT_TO_POINT)
  .def("toDistanceTransform",
       [](Mesh& mesh, std::vector<double>& v, std::vector<unsigned>& d, std::vector<double>& p) -> decltype(auto) {
         return mesh.toDistanceTransform(makeVector({v[0], v[1], v[2]}), Dims({d[0], d[1], d[2]}), Point({p[0], p[1], p[2]}));
       },
       "converts mesh to distance transform, automatically computing size and origin if necessary",
       "spacing"_a=std::vector<double>({1.0, 1.0, 1.0}), "size"_a=std::vector<unsigned>({0, 0, 0}), "origin"_a=std::vector<double>({-1.0, -1.0, -1.0}))
  .def("center",                &Mesh::center, "center of mesh")
  .def("centerOfMass",          &Mesh::centerOfMass, "center of mass of mesh")
  .def("numPoints",             &Mesh::numPoints, "number of points")
  .def("numFaces",              &Mesh::numFaces, "number of faces")
  .def("getPoint",              &Mesh::getPoint, "return (x,y,z) coordinates of vertex at given index", "p"_a)
  .def("getFieldNames",         &Mesh::getFieldNames, "print all field names in mesh")
  .def("setField", [](Mesh &mesh, std::vector<double>& v, std::string name) {
    vtkSmartPointer<vtkDoubleArray> arr = vtkSmartPointer<vtkDoubleArray>::New();
    arr->SetNumberOfValues(v.size());
    for (int i=0; i<v.size(); i++) {
      arr->SetTuple1(i, v[i]);
    }
    return mesh.setField(name, arr);
  }, "sets the given field for points with array", "array"_a, "name"_a)
  .def("getField", [](const Mesh &mesh, std::string name) {
    auto array = mesh.getField<vtkDataArray>(name);
    const auto shape = std::vector<size_t>{static_cast<unsigned long>(array->GetNumberOfTuples()), static_cast<unsigned long>(array->GetNumberOfComponents()), 1};
    auto vtkarr = vtkSmartPointer<vtkDoubleArray>(vtkDoubleArray::New());
    vtkarr->SetNumberOfValues(array->GetNumberOfValues());
    // LOTS of copying going on here, see github #903
    array->GetData(0, array->GetNumberOfTuples()-1, 0, array->GetNumberOfComponents()-1, vtkarr); // copy1
    return py::array(py::dtype::of<double>(), shape, vtkarr->GetVoidPointer(0)); // copy2 (not positive if py::array ctor copies or references)
  }, "gets the field", "name"_a)
  .def("setFieldValue",         &Mesh::setFieldValue, "sets the given index of field to value", "idx"_a, "value"_a, "name"_a="")
  .def("getFieldValue",         &Mesh::getFieldValue, "gets the value at the given index of field", "idx"_a, "name"_a)
  .def("getFieldRange",         &Mesh::getFieldRange, "returns the range of the given field", "name"_a)
  .def("getFieldMean",          &Mesh::getFieldMean, "returns the mean the given field", "name"_a)
  .def("getFieldStd",           &Mesh::getFieldStd, "returns the standard deviation of the given field", "name"_a)
  ;

  // MeshUtils
  py::class_<MeshUtils>(m, "MeshUtils")

  // TODO: fails to compile on Windows due to missing Eigen symbols
  // https://github.com/SCIInstitute/ShapeWorks/issues/954
  // .def_static("distilVertexInfo",
  //             &MeshUtils::distilVertexInfo,
  //             "distils vertex information from VTK poly data to Eigen matrices",
  //             "mesh"_a)
  // .def_static("distilFaceInfo",
  //             &MeshUtils::distilFaceInfo,
  //             "distils face information from VTK poly data to Eigen matrices",
  //             "mesh"_a)

  // TODO: Bind generateWarpMatrix, warpMesh, warpMeshes later if required
  // .def_static("generateWarpMatrix", &MeshUtils::generateWarpMatrix, "compute the warp matrix using the mesh and reference points", "TV"_a, "TF"_a, "Vref"_a)
  // .def_static("warpMesh",       &MeshUtils::warpMesh, "compute individual warp", "movPts"_a, "W"_a, "Fref"_a)
  // .def_static("warpMeshes",     &MeshUtils::warpMeshes, "compute transformation from set of points files using template mesh warp & face matrices", "movingPointPaths"_a, "outputMeshPaths"_a, "W"_a, "Fref"_a, "numP"_a)
  .def_static("boundingBox", [](std::vector<std::string> filenames, bool center) {
    return shapeworks::MeshUtils::boundingBox(filenames, center);
  }, "calculate bounding box incrementally for meshes", "filenames"_a, "center"_a=false)
  .def_static("boundingBox", [](std::vector<Mesh> meshes, bool center) {
    return shapeworks::MeshUtils::boundingBox(meshes, center);
  }, "calculate bounding box incrementally for shapework meshes", "meshes"_a, "center"_a=false)
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
  .def(py::init<>())
  .def("LoadParameterFile",     &Optimize::LoadParameterFile)
  .def("Run",                   &Optimize::Run)
  .def("SetIterationCallbackFunction",
                                &Optimize::SetIterationCallbackFunction)
  .def("GetParticleSystem",     &optimize_get_particle_system)
  ;
}

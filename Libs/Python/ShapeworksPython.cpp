

#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <Optimize/Optimize.h>

#include <Eigen/Eigen>

Eigen::MatrixXd optimize_get_particle_system(shapeworks::Optimize* opt) {
  shapeworks::MatrixContainer container = opt->GetParticleSystem();
  return container.matrix_;
}

#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include <itkImportImageFilter.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>

#include <boost/filesystem.hpp>
#include <sstream>

#include "EigenUtils.h"
#include "Image.h"
#include "ImageUtils.h"
#include "Mesh.h"
#include "MeshUtils.h"
#include "MeshWarper.h"
#include "Optimize.h"
#include "Parameters.h"
#include "ParticleShapeStatistics.h"
#include "ParticleSystemEvaluation.h"
#include "Project.h"
#include "PythonAnalyze.h"
#include "PythonGroom.h"
#include "ReconstructSurface.h"
#include "ShapeEvaluation.h"
#include "Shapeworks.h"
#include "ShapeworksUtils.h"
#include "Subject.h"
#include "Variant.h"
#include "VectorImage.h"
#include "pybind_utils.h"
#include <Optimize/Function/EarlyStop/MorphologicalDeviationScore.h>
namespace fs = boost::filesystem;

using namespace shapeworks;

PYBIND11_MODULE(shapeworks_py, m) {
  m.doc() = "ShapeWorks Python API";

  m.attr("Pi") = std::atan(1.0) * 4.0;

  m.def("seed", &ShapeWorksUtils::set_rng_seed, "sets the seed for random number generation (internal use)",
        "seed"_a = std::chrono::system_clock::now().time_since_epoch().count());

  m.def("setup_console_logging", &ShapeWorksUtils::setup_console_logging, "sets up console logging options",
        "show_progress"_a = true, "xml_status"_a = false);

  m.def("set_progress_callback", &ShapeWorksUtils::set_progress_callback, "sets up a progress callback", "callback"_a);

  // Axis
  py::enum_<Axis>(m, "Axis")
      .value("invalid", Axis::invalid)
      .value("X", Axis::X)
      .value("Y", Axis::Y)
      .value("Z", Axis::Z)
      .export_values();

  m.def(
      "axis_is_valid",
      [](const std::vector<double>& axis) -> decltype(auto) {
        return axis_is_valid(makeVector({axis[0], axis[1], axis[2]}));
      },
      "ensure an axis is valid", "axis"_a);

  m.def("degToRad", degToRad, "convert degrees to radians", "deg"_a);

  m.def("toAxis", toAxis, "convert to axis", "str"_a);

  // Transform
  py::class_<ImageUtils::TPSTransform::Pointer>(m, "WarpTransform")
      .def("__repr__",
           [](const ImageUtils::TPSTransform::Pointer& transform) { return "itkThinPlateSplineKernelTransform"; });

  // Image::InterpolationType (even though this is part of Image, it could be
  // used elsewhere so we keep it global in the module)
  py::enum_<Image::InterpolationType>(m, "InterpolationType")
      .value("Linear", Image::InterpolationType::Linear)
      .value("NearestNeighbor", Image::InterpolationType::NearestNeighbor)
      .export_values();

  m.def(
      "mean", [](py::array& field) { return mean(pyToArr(field, false /*take_ownership*/)); },
      "incrementally compute (single-component) mean of field");

  m.def(
      "stddev", [](py::array& field) { return stddev(pyToArr(field, false /*take_ownership*/)); },
      "compute (single-component) standard deviation of field");

  m.def(
      "range", [](py::array& field) { return range(pyToArr(field, false /*take_ownership*/)); },
      "compute (single-component) range of field");

  // Image
  py::class_<Image>(m, "Image")
      .def(py::init<const std::string&>())
      .def(py::init<const Image&>())

      // constructor that wraps numpy float32 array
      .def(py::init
           // The reasons we don't simply specify py::array_t<float>
           // as a parameter are:
           // - to take ownership of the array
           // - to ensure dtype is same as Image::PixelType, and
           // - to ensure the array isn't silently cast by copying (the default pybind11 behavior)
           ([](py::array& np_array) { return Image(wrapNumpyArr(np_array)); }),
           "Initialize an image from a numpy array (must be dtype float32).\nTransfers ownership of the array without "
           "copying.\nIf a copy is desired, construct using Image(np.array(arr)).")

      .def(
          "assign",
          [](Image& image, py::array& np_array) -> decltype(auto) {
            // verify dims are the same as existing image; warn if not
            auto curr_dims = image.dims();
            Dims new_dims = {{static_cast<Dims::SizeValueType>(np_array.shape()[2]),
                              static_cast<Dims::SizeValueType>(np_array.shape()[1]),
                              static_cast<Dims::SizeValueType>(np_array.shape()[0])}};
            if (curr_dims[0] != new_dims[0] || curr_dims[1] != new_dims[1] || curr_dims[2] != new_dims[2]) {
              std::cerr << "curr_dims: " << curr_dims[0] << " " << curr_dims[1] << " " << curr_dims[2] << std::endl;
              std::cerr << "new_dims: " << new_dims[0] << " " << new_dims[1] << " " << new_dims[2] << std::endl;
              throw std::invalid_argument("dims of new image are not the same as old image.");
            }

            // get existing origin, shape, and coordsys
            auto origin = image.origin();
            auto spacing = image.spacing();
            auto coordsys = image.coordsys();

            // wrap the numpy array
            image = Image(wrapNumpyArr(np_array));

            // set new image to have the same origin, shape, and coordsys
            image.setOrigin(origin);
            image.setSpacing(spacing);
            image.setCoordsys(coordsys);

            return image;
          },
          "Initialize an image from a numpy array (must be dtype float32).\nTransfers ownership of the array without "
          "copying.\nIf a copy is desired, construct using Image(np.array(arr)).")

      .def("__neg__", [](Image& img) -> decltype(auto) { return -img; })
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

      .def("__repr__",
           [](const Image& img) -> decltype(auto) {
             std::stringstream stream;
             stream << img;
             return stream.str();
           })

      .def("copy", [](Image& image) -> decltype(auto) { return Image(image); })

      .def("write", &Image::write, "writes the current image (determines type by its extension)", "filename"_a,
           "compressed"_a = true)

      .def("antialias", &Image::antialias, "antialiases binary volumes (layers is set to 3 when not specified)",
           "iterations"_a = 50, "maxRMSErr"_a = 0.01f, "layers"_a = 3)

      .def(
          "resample",
          [](Image& image, Eigen::Matrix<double, 4, 4>& eigen_mat, const std::vector<double>& p,
             const std::vector<unsigned>& d, const std::vector<double>& v,
             const Eigen::Matrix<double, 3, 3, Eigen::RowMajor>& direction,
             Image::InterpolationType interp) -> decltype(auto) {
            auto itk_xform = eigen44ToItkTransform(eigen_mat);
            return image.resample(itk_xform, Point({p[0], p[1], p[2]}), Dims({d[0], d[1], d[2]}),
                                  makeVector({v[0], v[1], v[2]}), eigenToItk(direction), interp);
          },
          "resamples by applying transform then sampling from given origin along direction axes at spacing physical "
          "units per pixel for dims pixels using specified interpolator",
          "transform"_a, "origin"_a, "dims"_a, "spacing"_a, "direction"_a,
          "interp"_a = Image::InterpolationType::NearestNeighbor)

      .def(
          "resample",
          [](Image& image, const std::vector<double>& v, Image::InterpolationType interp) -> decltype(auto) {
            return image.resample(makeVector({v[0], v[1], v[2]}), interp);
          },
          "resamples image using new physical spacing, updating logical dims to keep all image data for this spacing",
          "physicalSpacing"_a, "interp"_a = Image::InterpolationType::Linear)

      .def("resample", py::overload_cast<double, Image::InterpolationType>(&Image::resample),
           "isotropically resamples image using giving isospacing", "isoSpacing"_a = 1.0,
           "interp"_a = Image::InterpolationType::Linear)

      .def(
          "resize",
          [](Image& image, std::vector<unsigned>& d, Image::InterpolationType interp) -> decltype(auto) {
            return image.resize(Dims({d[0], d[1], d[2]}), interp);
          },
          "change logical dims (computes new physical spacing)", "logicalDims"_a,
          "interp"_a = Image::InterpolationType::Linear)

      .def("recenter", &Image::recenter,
           "recenters an image by changing its origin in the image header to the physical coordinates of the center of "
           "the image")

      .def("pad", py::overload_cast<int, Image::PixelType>(&Image::pad),
           "pads an image by same number of pixels in all directions with constant value", "pad"_a, "value"_a = 0.0)

      .def("pad", py::overload_cast<int, int, int, Image::PixelType>(&Image::pad),
           "pads an image by desired number of pixels in each direction with constant value", "padx"_a, "pady"_a,
           "padz"_a, "value"_a = 0.0)

      .def("pad", py::overload_cast<IndexRegion&, Image::PixelType>(&Image::pad),
           "pads an image to include the given region with constant value", "region"_a, "value"_a = 0.0)

      .def(
          "translate",
          [](Image& image, const std::vector<double>& v) -> decltype(auto) {
            return image.translate(makeVector({v[0], v[1], v[2]}));
          },
          "translates image", "v"_a)

      .def(
          "scale",
          [](Image& image, const std::vector<double>& scale_vec) -> decltype(auto) {
            return image.scale(makeVector({scale_vec[0], scale_vec[1], scale_vec[2]}));
          },
          "scale image by scale_vec around center (not origin)", "scale_vec"_a)

      .def("rotate", py::overload_cast<const double, const Vector3&>(&Image::rotate),
           "rotate around center (not origin) using axis (default z-axis) by angle (in radians)", "angle"_a, "axis"_a)

      .def("rotate", py::overload_cast<const double, Axis>(&Image::rotate),
           "rotate around center (not origin) using axis (default z-axis) by angle (in radians)", "angle"_a, "axis"_a)

      .def(
          "rotate",
          [](Image& image, const double angle, const std::vector<double>& v) -> decltype(auto) {
            return image.rotate(angle, makeVector({v[0], v[1], v[2]}));
          },
          "rotate around center (not origin) using axis (default z-axis) by angle (in radians)", "angle"_a, "axis"_a)

      .def(
          "applyTransform",
          [](Image& image, Eigen::Matrix<double, 4, 4>& eigen_mat, Image::InterpolationType interp,
             bool meshTransform) -> decltype(auto) {
            if (meshTransform) {
              eigen_mat = eigen_mat.inverse().eval();
              Eigen::VectorXd lastColumn = eigen_mat.col(eigen_mat.cols() - 1);
              Eigen::VectorXd lastRow = eigen_mat.row(eigen_mat.rows() - 1);
              eigen_mat.col(eigen_mat.cols() - 1) = lastRow;
              eigen_mat.row(eigen_mat.rows() - 1) = lastColumn;
            }
            auto itk_xform = eigen44ToItkTransform(eigen_mat);
            return image.applyTransform(itk_xform, interp);
          },
          "applies the given transformation to the image by using the specified resampling filter (Linear or "
          "NearestNeighbor)",
          "transform"_a, "interp"_a = Image::InterpolationType::Linear, "meshTransform"_a = false)

      .def(
          "applyTransform",
          [](Image& image, ImageUtils::TPSTransform::Pointer transform,
             Image::InterpolationType interp) -> decltype(auto) { return image.applyTransform(transform, interp); },
          "applies the given warp transformation to the image using the specified resampling filter (Linear or "
          "NearestNeighbor)",
          "transform"_a, "interp"_a = Image::InterpolationType::Linear)

      .def(
          "applyTransform",
          [](Image& image, Eigen::Matrix<double, 4, 4>& eigen_mat, const std::vector<double>& p,
             const std::vector<unsigned>& d, const std::vector<double>& v,
             const Eigen::Matrix<double, 3, 3, Eigen::RowMajor>& direction, Image::InterpolationType interp,
             bool meshTransform) {
            if (meshTransform) {
              eigen_mat = eigen_mat.inverse().eval();
              Eigen::VectorXd lastColumn = eigen_mat.col(eigen_mat.cols() - 1);
              Eigen::VectorXd lastRow = eigen_mat.row(eigen_mat.rows() - 1);
              eigen_mat.col(eigen_mat.cols() - 1) = lastRow;
              eigen_mat.row(eigen_mat.rows() - 1) = lastColumn;
            }
            auto itk_xform = eigen44ToItkTransform(eigen_mat);
            return image.applyTransform(itk_xform, Point({p[0], p[1], p[2]}), Dims({d[0], d[1], d[2]}),
                                        makeVector({v[0], v[1], v[2]}), eigenToItk(direction), interp);
          },
          "applies the given transformation to the image by using resampling filter with new origin, dims, spacing, "
          "and sampling along given direction axes (a 3x3 row-major matrix) using the specified interpolation method "
          "(Linear or NearestNeighbor)",
          "transform"_a, "origin"_a, "dims"_a, "spacing"_a, "direction"_a,
          "interp"_a = Image::InterpolationType::NearestNeighbor, "meshTransform"_a = false)

      .def("extractLabel", &Image::extractLabel,
           "extracts/isolates a specific pixel label from a given multi-label volume and outputs the corresponding "
           "binary image",
           "label"_a = 1.0)

      .def("closeHoles", &Image::closeHoles, "closes holes in a volume defined by values larger than specified value",
           "foreground"_a = 0.0)

      .def("binarize", &Image::binarize,
           "sets portion of image greater than min and less than or equal to max to the specified value",
           "minVal"_a = 0.0, "maxVal"_a = std::numeric_limits<Image::PixelType>::max(), "innerVal"_a = 1.0,
           "outerVal"_a = 0.0)

      .def("computeDT", &Image::computeDT,
           "computes signed distance transform volume from an image at the specified isovalue", "isovalue"_a = 0.0)

      .def("applyCurvatureFilter", &Image::applyCurvatureFilter,
           "denoises an image using curvature driven flow using curvature flow image filter", "iterations"_a = 10)

      .def("applyGradientFilter", &Image::applyGradientFilter,
           "computes gradient magnitude at each pixel using gradient magnitude filter")

      .def("applySigmoidFilter", &Image::applySigmoidFilter,
           "computes sigmoid function pixel-wise using sigmoid image filter", "alpha"_a = 10.0, "beta"_a = 10.0)

      .def("applyTPLevelSetFilter", &Image::applyTPLevelSetFilter,
           "segments structures in image using topology preserving geodesic active contour level set filter",
           "featureImage"_a, "scaling"_a = 20.0)

      .def("applyIntensityFilter", &Image::applyIntensityFilter, "applies intensity windowing image filter",
           "min"_a = 0.0, "max"_a = 0.0)

      .def("gaussianBlur", &Image::gaussianBlur, "applies gaussian blur", "sigma"_a = 0.0)

      .def("crop", &Image::crop, "crops the image down to the given (physical) region, with optional padding",
           "region"_a, "padding"_a = 0)

      .def("fitRegion", &Image::fitRegion, "crops or pads the image to fit a region with optional padding value",
           "region"_a, "value"_a = 0)

      .def(
          "clip",
          [](Image& image, const std::vector<double>& o, std::vector<double>& p1, std::vector<double>& p2,
             const Image::PixelType val) {
            return image.clip(
                makePlane(Point({o[0], o[1], o[2]}), Point({p1[0], p1[1], p1[2]}), Point({p2[0], p2[1], p2[2]})), val);
          },
          "sets values on the back side of cutting plane (containing three non-colinear points) to val (default 0.0)",
          "o"_a, "p1"_a, "p2"_a, "val"_a = 0.0)

      .def(
          "clip",
          [](Image& image, const std::vector<double>& n, std::vector<double>& q, const Image::PixelType val) {
            return image.clip(makePlane(Point({q[0], q[1], q[2]}), makeVector({n[0], n[1], n[2]})), val);
          },
          "sets values on the back side of cutting plane (normal n containing point p) to val (default 0.0)", "n"_a,
          "q"_a, "val"_a = 0.0)

      .def(
          "setSpacing",
          [](Image& self, std::vector<double>& v) -> decltype(auto) {
            return self.setSpacing(makeVector({v[0], v[1], v[2]}));
          },
          "set image spacing, the size of each pixel", "spacing"_a = std::vector<double>({1.0, 1.0, 1.0}))

      .def(
          "setOrigin",
          [](Image& image, std::vector<double>& p) {
            return image.setOrigin(Point({p[0], p[1], p[2]}));
          },
          "sets the image origin in physical space to the given value", "origin"_a = std::vector<double>({0, 0, 0}))

      .def(
          "setCoordsys",
          [](Image& image, const Eigen::Matrix<double, 3, 3, Eigen::RowMajor>& coordsys) {
            if (coordsys.rows() != 3 || coordsys.cols() != 3) {
              throw std::invalid_argument("coordsys must be a 3x3 row-major numpy array");
            }
            return image.setCoordsys(eigenToItk(coordsys));
          },
          "sets the orientation of this image", "coordsys"_a = std::vector<double>({1, 0, 0, 0, 1, 0, 0, 0, 1}))

      .def("reflect", &Image::reflect, "reflect image with respect to logical image center and the specified axis",
           "axis"_a)

      .def(
          "dims", [](const Image& self) -> decltype(auto) { return py::array(3, self.dims().data()); },
          "logical dimensions of the image")

      .def(
          "size", [](const Image& self) -> decltype(auto) { return py::array(3, self.size().GetDataPointer()); },
          "physical dimensions of the image (dims * spacing)")

      .def(
          "spacing", [](const Image& self) -> decltype(auto) { return py::array(3, self.spacing().GetDataPointer()); },
          "physical spacing of the image")

      .def(
          "origin", [](const Image& self) -> decltype(auto) { return py::array(3, self.origin().GetDataPointer()); },
          "physical coordinates of image origin")

      .def(
          "center", [](const Image& self) -> decltype(auto) { return py::array(3, self.center().GetDataPointer()); },
          "physical coordinates of center of this image")

      .def(
          "coordsys", [](const Image& self) -> decltype(auto) { return itkToEigen(self.coordsys()); },
          "return 3x3 coordinate system in which this image lives in physical space")

      .def(
          "centerOfMass",
          [](const Image& self, double minVal, double maxVal) -> decltype(auto) {
            return py::array(3, self.centerOfMass().GetDataPointer());
          },
          "returns average physical coordinate of pixels in range (minval, maxval]", "minVal"_a = 0.0, "maxVal"_a = 1.0)

      .def("min", &Image::min, "minimum of image")

      .def("max", &Image::max, "maximum of image")

      .def("mean", &Image::mean, "mean of image")

      .def("std", &Image::std, "standard deviation of image")

      .def("logicalBoundingBox", &Image::logicalBoundingBox, "returns the index coordinates of this image's region")

      // py::overload_cast doesn't work with const functions, so we directly static_cast for these two functions
      .def("physicalboundingBox", static_cast<PhysicalRegion (Image::*)() const>(&Image::physicalBoundingBox),
           "returns region of physical space occupied by this image")

      .def("physicalBoundingBox",
           static_cast<PhysicalRegion (Image::*)(Image::PixelType) const>(&Image::physicalBoundingBox),
           "returns region of physical space occupied by the region of data <= the given isoValue", "isovalue"_a = 1.0)

      .def(
          "logicalToPhysical",
          [](Image& self, IndexRegion region) -> decltype(auto) { return self.logicalToPhysical(region); },
          "converts from a logical region (index coordinates) to a physical region", "region"_a)

      .def(
          "logicalToPhysical",
          [](Image& self, std::vector<long>& c) -> decltype(auto) {
            return py::array(3, self.logicalToPhysical(Coord({c[0], c[1], c[2]})).GetDataPointer());
          },
          "converts a logical (index) coordinate to physical space", "c"_a)

      .def(
          "physicalToLogical",
          [](Image& self, PhysicalRegion region) -> decltype(auto) { return self.physicalToLogical(region); },
          "converts from a physical region to a logical region (index coordinates)", "region"_a)

      .def(
          "physicalToLogical",
          [](Image& self, std::vector<double>& p) -> decltype(auto) {
            return py::array(3, self.physicalToLogical(Point({p[0], p[1], p[2]})).data());
          },
          "converts a physical coordinate to a logical (index) space", "p"_a)

      .def("compare", &Image::compare, "compares two images", "other"_a, "verifyall"_a = true, "tolerance"_a = 0.0,
           "precision"_a = 1e-6)

      .def(
          "toArray",
          [](const Image& image, bool copy, bool for_viewing) -> decltype(auto) {
            // We pass the array in column-major ('F') order when it will be used
            // for viewing (updating both shape and strides).
            const auto dims = image.dims();
            auto shape = std::vector<size_t>{dims[2], dims[1], dims[0]};
            if (for_viewing) {
              shape = std::vector<size_t>{dims[0], dims[1], dims[2]};
            }

            auto strides = std::vector<size_t>{dims[0] * dims[1] * sizeof(Image::PixelType),
                                               dims[0] * sizeof(Image::PixelType), sizeof(Image::PixelType)};
            if (for_viewing)
              strides = std::vector<size_t>{sizeof(Image::PixelType), dims[0] * sizeof(Image::PixelType),
                                            dims[0] * dims[1] * sizeof(Image::PixelType)};

            const auto py_dtype = py::dtype::of<Image::PixelType>();

#if 0
         std::cout << "Image info: " << std::endl
                   << "\tshape: " << shape[0] << " x " << shape[1] << " x " << shape[2] << std::endl
                   << "\tstrides: " << strides[0] << ", " << strides[1] << ", " << strides[2]
                   << "\tdtype: " << typeid(Image::PixelType).name()
                                  << " (" << sizeof(Image::PixelType) << " bytes)" << std::endl
                   << "\tpy_dtype: " << py_dtype << std::endl
                   << "\tcopy_requested: " << copy << std::endl
                   << "\tfor_viewing: " << for_viewing << std::endl;
#endif

            // When a valid object is passed as 'base', it tells pybind not to take
            // ownership of the data because 'base' will (allegedly) own it. Note
            // that ANY valid object is good for this purpose. This means that
            // image will continue to own its own array, which will be deleted
            // along with image (or when most itk operations are performed since
            // they tend to allocate fresh data).
            py::str dummyDataOwner;
            py::array img{py_dtype, shape, strides, image.getITKImage()->GetBufferPointer(),
                          (copy ? pybind11::handle() : dummyDataOwner)};
            assert(copy == img.owndata());

            // prevent pyvista.wrap from copying (transpose will still work fine)
            if (for_viewing)
              pybind11::detail::array_proxy(img.ptr())->flags |= pybind11::detail::npy_api::NPY_ARRAY_F_CONTIGUOUS_;
            else
              pybind11::detail::array_proxy(img.ptr())->flags |= pybind11::detail::npy_api::NPY_ARRAY_C_CONTIGUOUS_;

            return img;
          },
          "returns raw array of image data, directly sharing data by default, copying if specified.\nNOTE: many Image "
          "operations reallocate image array, so while the array returned from this function is writable, it is best "
          "used immediately for Python operations; use for_viewing argument to get array in column-major ('F') order "
          "('sw2vtkImage' already does this).",
          "copy"_a = false, "for_viewing"_a = false)

      .def(
          "createCenterOfMassTransform",
          [](Image& image) -> decltype(auto) { return itkTransformToEigen(image.createCenterOfMassTransform()); },
          "creates a transform that translates center of mass to center of image")

      .def(
          "createRigidRegistrationTransform",
          [](Image& image, const Image& other, float isovalue, unsigned iterations) -> decltype(auto) {
            return itkTransformToEigen(image.createRigidRegistrationTransform(other, isovalue, iterations));
          },
          "creates transform to target image using iterative closest point (ICP) registration; images MUST be distance "
          "transforms; isovalue is used to create meshes from these distance transform images, which are then passed "
          "to ICP for the given number of iterations",
          "target"_a, "isoValue"_a = 0.0, "iterations"_a = 20)

      .def("topologyPreservingSmooth", &Image::topologyPreservingSmooth,
           "creates a feature image (by applying gradient then sigmoid filters), then passes it to the TPLevelSet "
           "filter [curvature flow filter is often applied to the image before this filter]",
           "scaling"_a = 20.0, "sigmoidAlpha"_a = 10.5, "sigmoidBeta"_a = 10.0)

      .def("compare", &Image::compare, "compares two images", "other"_a, "verifyall"_a = true, "tolerance"_a = 0.0,
           "precision"_a = 1e-12)

      .def(
          "toMesh", [](Image& image, Image::PixelType isovalue) { return image.toMesh(isovalue); },
          "converts image to mesh at specified isovalue", "isovalue"_a)

      .def("isolate", &Image::isolate, "isolate largest object")

      .def(
          "evaluate",
          [](Image& image, std::vector<double>& pt) -> decltype(auto) {
            return image.evaluate(Point({pt[0], pt[1], pt[2]}));
          },
          "evaluate the image at any given point in space", "pt"_a);

  // PhysicalRegion
  py::class_<PhysicalRegion>(m, "PhysicalRegion")

      .def(py::init<>())

      .def(py::init([](std::vector<double> min, std::vector<double> max) {
        return PhysicalRegion(Point({min[0], min[1], min[2]}), Point({max[0], max[1], max[2]}));
      }))

      .def(py::init([](std::string str) { return PhysicalRegion(str); }))

      .def(py::self == py::self)

      .def("__repr__",
           [](const PhysicalRegion& region) {
             std::stringstream stream;
             stream << region;
             return stream.str();
           })

      .def_property(
          "min",
          // read (return special array so members of returned point can be modified, e.g., min[0] = 1.0)
          py::cpp_function(
              [](PhysicalRegion& region) -> py::array_t<double> {
                py::str dummyDataOwner;  // pretend the data has an owner and it won't be copied (pybind trick)
                py::array arr(py::dtype::of<Point::ValueType>(),  // dtype
                              std::vector<ssize_t>({3}),          // shape
                              std::vector<ssize_t>(),             // spacing
                              region.min.GetDataPointer(),        // data ptr
                              dummyDataOwner);                    // "inspire" py::array not to copy data
                return arr;
              },
              py::return_value_policy::move),

          // assign
          py::cpp_function(
              [](PhysicalRegion& region, std::vector<double> min) -> decltype(auto) {
                region.min = Point({min[0], min[1], min[2]});
                return min;
              },
              py::return_value_policy::reference),
          "min point of region")

      .def_property(
          "max",
          // read (return special array so members of returned point can be modified, e.g., max[0] = 1.0)
          py::cpp_function(
              [](PhysicalRegion& region) -> py::array_t<double> {
                py::str dummyDataOwner;  // pretend the data has an owner and it won't be copied (pybind trick)
                py::array arr(py::dtype::of<Point::ValueType>(),  // dtype
                              std::vector<ssize_t>({3}),          // shape
                              std::vector<ssize_t>(),             // spacing
                              region.max.GetDataPointer(),        // data ptr
                              dummyDataOwner);                    // "inspire" py::array not to copy data
                return arr;
              },
              py::return_value_policy::move),

          // assign
          py::cpp_function(
              [](PhysicalRegion& region, std::vector<double> max) -> decltype(auto) {
                region.max = Point({max[0], max[1], max[2]});
                return max;
              },
              py::return_value_policy::reference),
          "max point of region")

      .def("valid", &PhysicalRegion::valid, "ensure if region is valid")

      .def(
          "origin",
          [](const PhysicalRegion& region) -> decltype(auto) { return py::array(3, region.origin().GetDataPointer()); },
          "return origin of region")

      .def(
          "size",
          [](const PhysicalRegion& region) -> decltype(auto) { return py::array(3, region.size().GetDataPointer()); },
          "return size of region")

      .def("shrink", &PhysicalRegion::shrink, "shrink this region down to the smallest portions of both", "other"_a)

      .def("expand", py::overload_cast<const PhysicalRegion&>(&PhysicalRegion::expand),
           "expand this region up to the largest portions of both", "other"_a)

      .def("expand", py::overload_cast<const Point&>(&PhysicalRegion::expand),
           "expand this region to include this point", "point"_a)

      .def("pad", &PhysicalRegion::pad, "grows or shrinks the region by the specified amount", "padding"_a)

      .def("to_string", &PhysicalRegion::to_string, "returns a string representation of this region");

  // IndexRegion
  py::class_<IndexRegion>(m, "IndexRegion")

      .def(py::init<>())

      .def(py::init([](std::vector<double> min, std::vector<double> max) {
        return IndexRegion(Coord({static_cast<long>(min[0]), static_cast<long>(min[1]), static_cast<long>(min[2])}),
                           Coord({static_cast<long>(max[0]), static_cast<long>(max[1]), static_cast<long>(max[2])}));
      }))

      .def(py::init([](std::vector<double> dims) {
        if (dims[0] < 0.0 || dims[1] < 0.0 || dims[2] < 0.0)
          throw std::invalid_argument("cannot convert negative values to IndexRegion");
        return IndexRegion(Dims({static_cast<unsigned long>(dims[0]), static_cast<unsigned long>(dims[1]),
                                 static_cast<unsigned long>(dims[2])}));
      }))

      .def(py::self == py::self)

      .def("__repr__",
           [](const IndexRegion& region) {
             std::stringstream stream;
             stream << region;
             return stream.str();
           })

      .def_property(
          "min",
          // read (return special array so members of returned point can be modified, e.g., min[0] = 1.0)
          py::cpp_function(
              [](IndexRegion& region) -> py::array_t<Coord::IndexValueType> {
                py::str dummyDataOwner;  // pretend the data has an owner and it won't be copied (pybind trick)
                py::array arr(py::dtype::of<Coord::IndexValueType>(),  // dtype
                              std::vector<ssize_t>({3}),               // shape
                              std::vector<ssize_t>(),                  // spacing
                              region.min.data(),                       // data ptr
                              dummyDataOwner);                         // "inspire" py::array not to copy data
                return arr;
              },
              py::return_value_policy::move),

          // assign
          py::cpp_function(
              [](IndexRegion& region, std::vector<double> min) -> decltype(auto) {
                region.min =
                    Coord({static_cast<Coord::IndexValueType>(min[0]), static_cast<Coord::IndexValueType>(min[1]),
                           static_cast<Coord::IndexValueType>(min[2])});
                return min;
              },
              py::return_value_policy::reference),
          "min point of region")

      .def_property(
          "max",
          // read (return special array so members of returned point can be modified, e.g., max[0] = 1.0)
          py::cpp_function(
              [](IndexRegion& region) -> py::array_t<Coord::IndexValueType> {
                py::str dummyDataOwner;  // pretend the data has an owner and it won't be copied (pybind trick)
                py::array arr(py::dtype::of<Coord::IndexValueType>(),  // dtype
                              std::vector<ssize_t>({3}),               // shape
                              std::vector<ssize_t>(),                  // spacing
                              region.max.data(),                       // data ptr
                              dummyDataOwner);                         // "inspire" py::array not to copy data
                return arr;
              },
              py::return_value_policy::move),

          // assign
          py::cpp_function(
              [](IndexRegion& region, std::vector<double> max) -> decltype(auto) {
                region.max =
                    Coord({static_cast<Coord::IndexValueType>(max[0]), static_cast<Coord::IndexValueType>(max[1]),
                           static_cast<Coord::IndexValueType>(max[2])});
                return max;
              },
              py::return_value_policy::reference),
          "max point of region")

      .def("valid", &IndexRegion::valid, "ensure if region is valid")

      .def(
          "origin", [](const IndexRegion& region) -> decltype(auto) { return py::array(3, region.origin().data()); },
          "return origin of region")

      .def(
          "size", [](const IndexRegion& region) -> decltype(auto) { return py::array(3, region.size().data()); },
          "return size of region")

      .def("pad", &IndexRegion::pad, "grows or shrinks the region by the specified amount", "padding"_a);

  // VectorImage
  py::class_<VectorImage>(m, "VectorImage")

      .def(py::init([](const Image& dt) { return VectorImage(dt); }),
           "create a vector image from an image (usually a distance transform) that can be sampled at any point in "
           "space",
           "image"_a)

      .def(
          "evaluate",
          [](VectorImage& image, std::vector<double>& pt) -> decltype(auto) {
            auto v = image.evaluate(Point({pt[0], pt[1], pt[2]}));
            return std::vector<double>({v[0], v[1], v[2]});
          },
          "evaluate the vector image at any given point in space", "pt"_a);

  // ImageUtils
  py::class_<ImageUtils>(m, "ImageUtils")

      .def_static("boundingBox",
                  py::overload_cast<const std::vector<std::string>&, Image::PixelType>(&ImageUtils::boundingBox),
                  "compute largest bounding box surrounding the specified isovalue of the specified set of filenames",
                  "filenames"_a, "isoValue"_a = 1.0)

      .def_static("boundingBox",
                  py::overload_cast<const std::vector<std::reference_wrapper<const Image>>&, Image::PixelType>(
                      &ImageUtils::boundingBox),
                  "compute largest bounding box surrounding the specified isovalue of the specified set of images",
                  "images"_a, "isoValue"_a = 1.0)

      .def_static("createWarpTransform", &ImageUtils::createWarpTransform,
                  "computes a warp transform from the source to the target landmarks (in the given files) using every "
                  "stride points",
                  "source_landmarks"_a, "target_landmarks"_a, "stride"_a = 1);

  // Mesh
  py::class_<Mesh> mesh(m, "Mesh");

  // Mesh::FieldType
  py::enum_<Mesh::FieldType>(mesh, "FieldType")
      .value("Point", Mesh::FieldType::Point)
      .value("Face", Mesh::FieldType::Face)
      .export_values();
  ;

  // Mesh::AlignmentType
  py::enum_<Mesh::AlignmentType>(mesh, "AlignmentType")
      .value("Rigid", Mesh::AlignmentType::Rigid)
      .value("Similarity", Mesh::AlignmentType::Similarity)
      .value("Affine", Mesh::AlignmentType::Affine)
      .export_values();
  ;

  // Mesh::DistanceMethod
  py::enum_<Mesh::DistanceMethod>(mesh, "DistanceMethod")
  .value("PointToPoint", Mesh::DistanceMethod::PointToPoint)
  .value("PointToCell", Mesh::DistanceMethod::PointToCell)
  .value("SymmetricPointToCell", Mesh::DistanceMethod::SymmetricPointToCell)
  .export_values();
  ;

  // Mesh::CurvatureType
  py::enum_<Mesh::CurvatureType>(mesh, "CurvatureType")
      .value("Principal", Mesh::CurvatureType::Principal)
      .value("Gaussian", Mesh::CurvatureType::Gaussian)
      .value("Mean", Mesh::CurvatureType::Mean)
      .export_values();
  ;

  // Mesh::SubdivisionType
  py::enum_<Mesh::SubdivisionType>(mesh, "SubdivisionType")
      .value("Butterfly", Mesh::SubdivisionType::Butterfly)
      .value("Loop", Mesh::SubdivisionType::Loop)
      .export_values();
  ;

  // Mesh bindings
  mesh.def(py::init<const std::string&>())

      .def(py::init<vtkSmartPointer<vtkPolyData>>())
      .def(py::self == py::self)
      .def(py::self += py::self)

      .def(py::init<const Eigen::MatrixXd&, const Eigen::MatrixXi&>())  // Mesh constructor from matrices

      .def("__repr__",
           [](const Mesh& mesh) -> decltype(auto) {
             std::stringstream stream;
             stream << mesh;
             return stream.str();
           })

      .def("copy", [](Mesh& mesh) -> decltype(auto) { return Mesh(mesh); })

      .def("write", &Mesh::write, "writes mesh, format specified by filename extension", "pathname"_a,
           "binaryFile"_a = false)

      .def("coverage", &Mesh::coverage,
           "determines coverage between current mesh and another mesh (e.g. acetabular cup / femoral head)",
           "otherMesh"_a, "allowBackIntersections"_a = true, "angleThreshold"_a = 0, "backSearchRadius"_a = 0)

      .def("smooth", &Mesh::smooth, "applies laplacian smoothing", "iterations"_a = 0, "relaxation"_a = 0.0)

      .def("smoothSinc", &Mesh::smoothSinc, "applies windowed sinc smoothing", "iterations"_a = 0, "passband"_a = 0.0)

      .def("remesh", &Mesh::remesh,
           "applies remeshing using approximated centroidal voronoi diagrams for a given number of vertices and "
           "adaptivity",
           "numVertices"_a, "adaptivity"_a)

      .def("remeshPercent", &Mesh::remeshPercent,
           "applies remeshing using approximated centroidal voronoi diagrams for a given percentage of vertices and "
           "adaptivity",
           "percentage"_a, "adaptivity"_a)

      .def("invertNormals", &Mesh::invertNormals, "handle flipping normals")

      .def(
          "reflect",
          [](Mesh& mesh, const Axis& axis, std::vector<double>& v) -> decltype(auto) {
            return mesh.reflect(axis, makeVector({v[0], v[1], v[2]}));
          },
          "reflect meshes with respect to a specified center and specific axis", "axis"_a,
          "origin"_a = std::vector<double>({0.0, 0.0, 0.0}))

      .def(
          "createTransform",
          [](Mesh& mesh, const Mesh& other, Mesh::AlignmentType type, unsigned iterations) -> decltype(auto) {
            return vtkTransformToEigen(mesh.createTransform(other, type, iterations));
          },
          "creates a transform using specified AlignmentType (Mesh.Rigid, Mesh.Similarity, Mesh.Affine) for specified "
          "number of iterations (default alignment: Similarity, default iterations: 10)",
          "target"_a, "align"_a = Mesh::AlignmentType::Similarity, "iterations"_a = 10)

      .def(
          "applyTransform",
          [](Mesh& mesh, Eigen::Matrix<double, 4, 4>& eigen_mat, bool imageTransform) -> decltype(auto) {
            if (imageTransform) {
              Eigen::VectorXd lastColumn = eigen_mat.col(eigen_mat.cols() - 1);
              Eigen::VectorXd lastRow = eigen_mat.row(eigen_mat.rows() - 1);
              eigen_mat.col(eigen_mat.cols() - 1) = lastRow;
              eigen_mat.row(eigen_mat.rows() - 1) = lastColumn;
              eigen_mat = eigen_mat.inverse().eval();
            }
            auto vtk_xform = eigen44ToVtkTransform(eigen_mat);
            return mesh.applyTransform(vtk_xform);
          },
          "applies the given transformation to the mesh", "transform"_a, "imageTransform"_a = false)

      .def(
          "rotate",
          [](Mesh& mesh, const double angle, const Axis axis) -> decltype(auto) { return mesh.rotate(angle, axis); },
          "rotate using axis by angle (in degrees)", "angle"_a, "axis"_a)

      .def("fillHoles", &Mesh::fillHoles, "finds holes in a mesh and closes them", "hole_size"_a = 1000)

      .def("probeVolume", &Mesh::probeVolume, "samples image data values at point locations specified by image",
           "image"_a)

      .def(
          "clip",
          [](Mesh& mesh, const std::vector<double>& p, const std::vector<double>& n) -> decltype(auto) {
            return mesh.clip(makePlane(Point({p[0], p[1], p[2]}), makeVector({n[0], n[1], n[2]})));
          },
          "clips a mesh using a cutting plane", "point"_a, "normal"_a)

      .def(
          "clip",
          [](Mesh& mesh, const std::vector<double>& o, const std::vector<double>& p1,
             const std::vector<double>& p2) -> decltype(auto) {
            return mesh.clip(
                makePlane(Point({o[0], o[1], o[2]}), Point({p1[0], p1[1], p1[2]}), Point({p2[0], p2[1], p2[2]})));
          },
          "clips a mesh using a cutting plane", "o"_a, "p1"_a, "p2"_a)

      .def(
          "translate",
          [](Mesh& mesh, const std::vector<double>& v) -> decltype(auto) {
            return mesh.translate(makeVector({v[0], v[1], v[2]}));
          },
          "translates mesh", "v"_a)

      .def(
          "scale",
          [](Mesh& mesh, const std::vector<double>& v) -> decltype(auto) {
            return mesh.scale(makeVector({v[0], v[1], v[2]}));
          },
          "scale mesh", "v"_a)

      .def("boundingBox", &Mesh::boundingBox, "computes bounding box of current mesh")

      .def("fixElement", &Mesh::fixElement, "fix element winding of mesh")

      .def("extractLargestComponent", &Mesh::extractLargestComponent, "extract the largest connected component from the mesh")

      .def(
          "distance",
          [](Mesh& mesh, const Mesh& target, const Mesh::DistanceMethod method) -> decltype(auto) {
            auto distances_and_ids = mesh.distance(target, method);
            return py::make_tuple(arrToPy(distances_and_ids[0], MOVE_ARRAY), arrToPy(distances_and_ids[1], MOVE_ARRAY));
          },
          "computes closest distance from vertices of this mesh to target mesh, returning indices of faces or vertices "
          "in target mesh that contain closest points",
          "target"_a, "method"_a = Mesh::DistanceMethod::PointToCell)

      .def(
          "clipClosedSurface",
          [](Mesh& mesh, const std::vector<double>& p, const std::vector<double>& n) -> decltype(auto) {
            return mesh.clipClosedSurface(makePlane(Point({p[0], p[1], p[2]}), makeVector({n[0], n[1], n[2]})));
          },
          "clips a mesh using a cutting plane resulting in a closed surface", "point"_a, "normal"_a)

      .def("computeNormals", &Mesh::computeNormals, "computes and adds oriented point and cell normals")

      .def(
          "closestPoint",
          [](Mesh& mesh, std::vector<double> p) -> decltype(auto) {
            double distance;
            vtkIdType face_id = -1;
            auto pt = mesh.closestPoint(Point({p[0], p[1], p[2]}), distance, face_id);
            return py::make_tuple(py::array(3, pt.GetDataPointer()), face_id);
          },
          "returns closest point to given point on mesh", "point"_a)

      .def(
          "closestPointId",
          [](Mesh& mesh, std::vector<double> p) -> decltype(auto) {
            return mesh.closestPointId(Point({p[0], p[1], p[2]}));
          },
          "returns closest point id in this mesh to the given point in space", "point"_a)

      .def(
          "interpolateFieldAtPoint",
          [](Mesh& mesh, const std::string& name, std::vector<double> p) -> decltype(auto) {
            return mesh.interpolateFieldAtPoint(name, Point({p[0], p[1], p[2]}));
          },
          "Interpolate the feature at the location using barycentric coordinate", "field"_a, "point"_a)

      .def("geodesicDistance", static_cast<double (Mesh::*)(int, int) const>(&Mesh::geodesicDistance),
           // py::overload_cast_const<int, int>(&Mesh::geodesicDistance),
           "computes geodesic distance between two vertices (specified by their indices) on mesh", "source"_a,
           "target"_a)

      .def(
          "geodesicDistance",
          [](Mesh& mesh, const std::vector<double> p) -> decltype(auto) {
            auto array = mesh.geodesicDistance(Point({p[0], p[1], p[2]}));
            return arrToPy(array, MOVE_ARRAY);
          },
          "computes geodesic distance between a point (landmark) and each vertex on mesh", "landmark"_a)

      .def(
          "geodesicDistance",
          [](Mesh& mesh, const std::vector<std::vector<double>> p) -> decltype(auto) {
            std::vector<Point> points;
            for (int i = 0; i < p.size(); i++) {
              points.push_back(Point({p[i][0], p[i][1], p[i][2]}));
            }
            auto array = mesh.geodesicDistance(points);
            return arrToPy(array, MOVE_ARRAY);
          },
          "computes geodesic distance between a set of points (curve) and all vertices on mesh", "curve"_a)

      .def(
          "curvature",
          [](Mesh& mesh, const Mesh::CurvatureType type) -> decltype(auto) {
            auto array = mesh.curvature(type);
            return arrToPy(array, MOVE_ARRAY);
          },
          "computes and adds curvature (principal (default) or gaussian or mean)",
          "type"_a = Mesh::CurvatureType::Principal)

      .def("applySubdivisionFilter", &Mesh::applySubdivisionFilter,
           "applies subdivision filter (butterfly (default) or loop)", "type"_a = Mesh::SubdivisionType::Butterfly,
           "subdivision"_a = 1)

      .def(
          "toImage",
          [](Mesh& mesh, PhysicalRegion& region, std::vector<double>& spacing) -> decltype(auto) {
            return mesh.toImage(region, Point({spacing[0], spacing[1], spacing[2]}));
          },
          "rasterize specified region to create binary image of desired dims (default: unit spacing)",
          "region"_a = PhysicalRegion(), "spacing"_a = std::vector<double>({1.0, 1.0, 1.0}))

      .def(
          "toDistanceTransform",
          [](Mesh& mesh, PhysicalRegion& region, std::vector<double>& spacing,
             std::vector<unsigned long>& padding) -> decltype(auto) {
            return mesh.toDistanceTransform(region, Point({spacing[0], spacing[1], spacing[2]}),
                                            Dims({padding[0], padding[1], padding[2]}));
          },
          "converts specified region to distance transform image with specified spacing and padding (default: unit "
          "spacing and 1 pixel of padding)",
          "region"_a = PhysicalRegion(), "spacing"_a = std::vector<double>({1.0, 1.0, 1.0}),
          "padding"_a = std::vector<unsigned long>({1, 1, 1}))

      .def(
          "center", [](Mesh& mesh) -> decltype(auto) { return py::array(3, mesh.center().GetDataPointer()); },
          "center of mesh")

      .def(
          "centerOfMass",
          [](Mesh& mesh) -> decltype(auto) { return py::array(3, mesh.centerOfMass().GetDataPointer()); },
          "center of mass of mesh")

      .def("numPoints", &Mesh::numPoints, "number of points")

      .def("numFaces", &Mesh::numFaces, "number of faces")

      .def("points", &Mesh::points, "matrix with number of points with (x,y,z) coordinates of each point")

      .def("faces", &Mesh::faces,
           "matrix with number of faces with indices of the three points from which each face is composed")

      .def(
          "getPoint",
          [](Mesh& mesh, int id) -> decltype(auto) { return py::array(3, mesh.getPoint(id).GetDataPointer()); },
          "(x,y,z) coordinates of vertex at given index", "id"_a)

      .def(
          "getFace",
          [](Mesh& mesh, int id) -> decltype(auto) { return py::array(3, mesh.getFace(id).GetDataPointer()); },
          "return indices of the three points with which the face at the given index is composed", "id"_a)

      .def("getFieldNames", &Mesh::getFieldNames, "print all field names in mesh")

      .def(
          "setField",
          [](Mesh& mesh, const std::string& name, py::array& array, const Mesh::FieldType type) -> decltype(auto) {
            auto vtkarr = pyToArr(array);
            return mesh.setField(name, vtkarr, type);
          },
          "sets the given field for points with array", "name"_a, "array"_a, "type"_a)

      .def(
          "getField",
          [](const Mesh& mesh, const std::string name, const Mesh::FieldType type) -> decltype(auto) {
            auto array = mesh.getField(name, type);
            if (!array) {
              throw std::invalid_argument("field '" + name + "' does not exist");
            }
            return arrToPy(array, SHARE_ARRAY);
          },
          "gets the field", "name"_a, "type"_a)

      .def("setFieldValue", &Mesh::setFieldValue, "sets the given index of field to value", "idx"_a, "value"_a,
           "name"_a = "")

      .def("getFieldValue", &Mesh::getFieldValue, "gets the value at the given index of field", "idx"_a, "name"_a)

      .def("getMultiFieldValue", &Mesh::getMultiFieldValue, "gets the vector value at the given index of field",
           "idx"_a, "name"_a)

      .def("compareField", &Mesh::compareField, "compares two meshes based on fields", "other_mesh"_a, "name1"_a,
           "name2"_a = "", "eps"_a = -1.0)

      .def("computeThickness", &Mesh::computeThickness, "Computes cortical thickness", "ct"_a, "dt"_a = nullptr,
           "maxDist"_a = 10000, "medianRadius"_a = 5.0, "distanceMesh"_a = "")

      .def("interpolate_scalars_to_mesh", &Mesh::interpolate_scalars_to_mesh, "Interpolate scalars to mesh", "name"_a,
           "positions"_a, "scalar_values"_a)

      ;

  // MeshWarping
  py::class_<MeshWarper>(m, "MeshWarper")

      .def(py::init<>())

      .def_static(
          "prepareMesh",
          [](const Mesh& mesh) -> decltype(auto) { return Mesh(MeshWarper::prep_mesh(mesh.getVTKMesh())); },
          "Return the prepared mesh used for warping (before vertices were inserted).", "mesh"_a)

      .def(
          "generateWarp",
          [](MeshWarper& w, const Mesh& mesh_ref, const Eigen::MatrixXd& particles_ref) -> decltype(auto) {
            w.set_reference_mesh(mesh_ref.getVTKMesh(), particles_ref);
            return w.generate_warp();
          },
          "Assign the reference mesh/particles (matrix [Nx3]) and pre-compute the warping", "reference_mesh"_a,
          "reference_particles"_a)

      .def(
          "generateWarp",
          [](MeshWarper& w, const Mesh& mesh_ref, const Eigen::MatrixXd& particles_ref,
             const Eigen::MatrixXd& landmarks) -> decltype(auto) {
            w.set_reference_mesh(mesh_ref.getVTKMesh(), particles_ref, landmarks);
            return w.generate_warp();
          },
          "Assign the reference mesh/particles (matrix [Nx3]) and landmarks (matrix [Nx3]) and pre-compute the warping",
          "reference_mesh"_a, "reference_particles"_a, "landmarks"_a)

      .def(
          "getReferenceMesh", [](MeshWarper& w) -> decltype(auto) { return Mesh(w.get_reference_mesh()); },
          "Return the mesh used for warping.")

      .def(
          "getReferenceParticles", [](MeshWarper& w) -> decltype(auto) { return w.get_reference_particles(); },
          "Return the particles used for warping.")

      .def(
          "hasBadParticles", [](MeshWarper& w) -> decltype(auto) { return w.has_bad_particles(); },
          "Return true if warping has removed any bad particle(s).")

      .def(
          "getWarpMatrix", [](MeshWarper& w) -> decltype(auto) { return w.get_warp_matrix(); },
          "Return the warping matrix (Vertices = Warp * Control).")

      .def(
          "getLandmarksMap", [](MeshWarper& w) -> decltype(auto) { return w.get_landmarks_map(); },
          "Return the map of landmarks to vertices.")

      .def(
          "getGoodParticlesIndices", [](MeshWarper& w) -> decltype(auto) { return w.get_good_particle_indices(); },
          "Return the indexes of good particles.")

      .def(
          "buildMesh",
          [](MeshWarper& w, const Eigen::MatrixXd& particles) -> decltype(auto) {
            return Mesh(w.build_mesh(particles));
          },
          "Build the mesh from particle positions (matrix [Nx3])", "particles"_a)

      .def(
          "extractLandmarks",
          [](MeshWarper& w, const Mesh& warped_mesh) -> decltype(auto) {
            return w.extract_landmarks(warped_mesh.getVTKMesh());
          },
          "Extract the landmarks from the warped mesh and return the landmarks (matrix [Nx3])", "warped_mesh"_a);

  // MeshUtils
  py::class_<MeshUtils>(m, "MeshUtils")

      .def_static("boundingBox", py::overload_cast<const std::vector<std::string>&, bool>(&MeshUtils::boundingBox),
                  "calculate bounding box incrementally for meshes", "filenames"_a, "center"_a = false)

      .def_static(
          "boundingBox",
          py::overload_cast<const std::vector<std::reference_wrapper<const Mesh>>&, bool>(&MeshUtils::boundingBox),
          "calculate bounding box incrementally for meshes", "meshes"_a, "center"_a = false)

      .def_static("findReferenceMesh", &MeshUtils::findReferenceMesh, "find reference mesh from a set of meshes",
                  "meshes"_a, "random_subset"_a = -1)

      .def_static("boundaryLoopExtractor", &MeshUtils::extract_boundary_loop,
                  "for a mesh extracts the boundary loop and export the boundary loop as a contour .vtp file", "mesh"_a)

      .def_static(
          "sharedBoundaryExtractor",
          [](const Mesh& mesh_l, const Mesh& mesh_r, float tol) -> decltype(auto) {
            std::array<Mesh, 3> output = MeshUtils::shared_boundary_extractor(mesh_l, mesh_r, tol);

            // std::move passes ownership to Python
            return py::make_tuple(std::move(output[0]), std::move(output[1]), std::move(output[2]));
          },
          "extract the shared boundary for the given left and right meshes and save the individual meshes", "mesh_l"_a,
          "mesh_r"_a, "tol"_a = 1e-3)

      .def_static("generateNormals", &MeshUtils::generateNormals,
                  "generates and adds normals for points and faces for each mesh in given set of meshes", "meshes"_a,
                  "forceRegen"_a = false)

      .def_static(
          "computeMeanNormals",
          [](const std::vector<std::string>& filenames, bool autoGenerateNormals) -> decltype(auto) {
            auto array = MeshUtils::computeMeanNormals(filenames, autoGenerateNormals);
            return arrToPy(array, MOVE_ARRAY);
          },
          "computes average normals for each point in given set of meshes", "filenames"_a,
          "autoGenerateNormals"_a = true)

      .def_static(
          "computeMeanNormals",
          [](const std::vector<std::reference_wrapper<const Mesh>>& meshes) -> decltype(auto) {
            auto array = MeshUtils::computeMeanNormals(meshes);
            return arrToPy(array, MOVE_ARRAY);
          },
          "computes average normals for each point in given set of meshes", "meshes"_a)

      ;

  // ParticleSystem
  py::class_<ParticleSystemEvaluation>(m, "ParticleSystem")

      .def(py::init<const std::vector<std::string>&>())

      .def(
          "ShapeAsPointSet",
          [](ParticleSystemEvaluation& p, int id_shape) -> decltype(auto) {
            Eigen::MatrixXd points = p.get_matrix().col(id_shape);
            points.resize(3, points.size() / 3);
            points.transposeInPlace();
            return points;
          },
          "Return the particle pointset [Nx3] of the specified shape", "id_shape"_a)

      .def("Particles", &ParticleSystemEvaluation::get_matrix)

      .def("Paths", &ParticleSystemEvaluation::get_paths)

      .def("N", &ParticleSystemEvaluation::num_samples)

      .def("D", &ParticleSystemEvaluation::num_dims)

      .def("ExactCompare", &ParticleSystemEvaluation::exact_compare)

      .def("EvaluationCompare", &ParticleSystemEvaluation::evaluation_compare);

  // ShapeEvaluation
  py::class_<ShapeEvaluation>(m, "ShapeEvaluation")

      .def_static("ComputeCompactness", &ShapeEvaluation::compute_compactness,
                  "Computes the compactness measure for a particle system", "particleSystem"_a, "nModes"_a,
                  "saveTo"_a = "")

      .def_static("ComputeGeneralization", &ShapeEvaluation::compute_generalization,
                  "Computes the generalization measure for a particle system", "particleSystem"_a, "nModes"_a,
                  "saveTo"_a = "", "surface_distance_mode"_a = false)

      .def_static("ComputeSpecificity", &ShapeEvaluation::compute_specificity,
                  "Computes the specificity measure for a particle system", "particleSystem"_a, "nModes"_a,
                  "saveTo"_a = "", "surface_distance_mode"_a = false)

      .def_static("ComputeFullCompactness", &ShapeEvaluation::compute_full_compactness,
                  "Computes the compactness measure for a particle system, all modes", "particleSystem"_a,
                  "progress_callback"_a = nullptr)

      .def_static("ComputeFullGeneralization", &ShapeEvaluation::compute_full_generalization,
                  "Computes the generalization measure for a particle system, all modes", "particleSystem"_a,
                  "progress_callback"_a = nullptr, "check_abort"_a = nullptr, "surface_distance_mode"_a = false)

      .def_static("ComputeFullSpecificity", &ShapeEvaluation::compute_full_specificity,
                  "Computes the specificity measure for a particle system, all modes", "particleSystem"_a,
                  "progress_callback"_a = nullptr, "check_abort"_a = nullptr, "surface_distance_mode"_a = false);

  py::class_<ParticleShapeStatistics>(m, "ParticleShapeStatistics")

      .def(py::init<>())

      // int do_pca(ParticleSystemEvaluation particleSystem, int domainsPerShape = 1);
      .def("PCA", py::overload_cast<ParticleSystemEvaluation, int>(&ParticleShapeStatistics::do_pca),
           "calculates the eigen values and eigen vectors of the data", "particleSystem"_a, "domainsPerShape"_a = 1)

      // int do_pca(std::shared_ptr<Project> project);
      .def("PCA", py::overload_cast<std::shared_ptr<Project>>(&ParticleShapeStatistics::do_pca),
           "calculates the eigen values and eigen vectors of the data from a project", "project"_a)

      .def("principalComponentProjections", &ParticleShapeStatistics::principal_component_projections,
           "projects the original data on the calculated principal components")

      .def("sampleSize", &ParticleShapeStatistics::get_num_samples, "returns the sample size of the particle system")

      .def("numDims", &ParticleShapeStatistics::get_num_dimensions,
           "returns the number of features of the particle system")

      .def("eigenVectors", &ParticleShapeStatistics::get_eigen_vectors, "returns the eigenvectors")

      .def("eigenValues", &ParticleShapeStatistics::get_eigen_values, "return the eigen values")

      .def("pcaLoadings", &ParticleShapeStatistics::get_pca_loadings,
           "returns the  coefficients of the linear combination of \
       the original variables from which the principal \
       components are constructed")

      .def("percentVarByMode", &ParticleShapeStatistics::get_percent_variance_by_mode,
           "return the variance accounted for by the principal components")

      .def("projectNewSample", &ParticleShapeStatistics::project_new_sample, "project a new sample into the PCA space",
           "newSample"_a)

      .def("getMean", &ParticleShapeStatistics::get_mean, "returns the mean shape particles");

  define_python_analyze(m);
  define_python_groom(m);

  py::class_<ReconstructSurface<ThinPlateSplineTransform>>(m, "ReconstructSurface_ThinPlateSplineTransform")

      .def(py::init<>())

      .def(py::init<const std::string&, const std::string&, const std::string&>())

      .def("setOutPrefix", &ReconstructSurface<ThinPlateSplineTransform>::setOutPrefix, "prefix"_a)

      .def("setOutPath", &ReconstructSurface<ThinPlateSplineTransform>::setOutPath, "path"_a)

      .def("setDoProcrustes", &ReconstructSurface<ThinPlateSplineTransform>::setDoProcrustes, "doProcrustes"_a)

      .def("setDoProcrustesScaling", &ReconstructSurface<ThinPlateSplineTransform>::setDoProcrustesScaling,
           "doProcrustesScaling"_a)

      .def("setMeanBeforeWarp", &ReconstructSurface<ThinPlateSplineTransform>::setMeanBeforeWarp, "meanBeforeWarp"_a)

      .def("setEnableOutput", &ReconstructSurface<ThinPlateSplineTransform>::setEnableOutput, "enableOutput"_a)

      .def("setModeIndex", &ReconstructSurface<ThinPlateSplineTransform>::setModeIndex, "modeIndex"_a)

      .def("setNumOfModes", &ReconstructSurface<ThinPlateSplineTransform>::setNumOfModes, "numOfModes"_a)

      .def("setNumOfSamplesPerMode", &ReconstructSurface<ThinPlateSplineTransform>::setNumOfSamplesPerMode,
           "numOfSamplesPerMode"_a)

      .def("setNumOfParticles", &ReconstructSurface<ThinPlateSplineTransform>::setNumOfParticles, "numOfParticles"_a)

      .def("setNumOfClusters", &ReconstructSurface<ThinPlateSplineTransform>::setNumOfClusters, "numOfClusters"_a)

      .def("setMaxStdDev", &ReconstructSurface<ThinPlateSplineTransform>::setMaxStdDev, "maxStdDev"_a)

      .def("setMaxVarianceCaptured", &ReconstructSurface<ThinPlateSplineTransform>::setMaxVarianceCaptured,
           "maxVarianceCaptured"_a)

      .def("setMaxAngleDegrees", &ReconstructSurface<ThinPlateSplineTransform>::setMaxAngleDegrees, "maxAngleDegrees"_a)

      .def("surface", &ReconstructSurface<ThinPlateSplineTransform>::surface, "localPointsFiles"_a)

      .def("samplesAlongPCAModes", &ReconstructSurface<ThinPlateSplineTransform>::samplesAlongPCAModes,
           "worldPointsFiles"_a)

      .def("meanSurface", &ReconstructSurface<ThinPlateSplineTransform>::meanSurface, "distanceTransformFiles"_a,
           "localPointsFiles"_a, "worldPointsFiles"_a);

  py::class_<ReconstructSurface<RBFSSparseTransform>>(m, "ReconstructSurface_RBFSSparseTransform")

      .def(py::init<>())

      .def(py::init<const std::string&, const std::string&, const std::string&>())

      .def("setOutPrefix", &ReconstructSurface<RBFSSparseTransform>::setOutPrefix, "prefix"_a)

      .def("setOutPath", &ReconstructSurface<RBFSSparseTransform>::setOutPath, "path"_a)

      .def("setDoProcrustes", &ReconstructSurface<RBFSSparseTransform>::setDoProcrustes, "doProcrustes"_a)

      .def("setDoProcrustesScaling", &ReconstructSurface<RBFSSparseTransform>::setDoProcrustesScaling,
           "doProcrustesScaling"_a)

      .def("setMeanBeforeWarp", &ReconstructSurface<RBFSSparseTransform>::setMeanBeforeWarp, "meanBeforeWarp"_a)

      .def("setEnableOutput", &ReconstructSurface<RBFSSparseTransform>::setEnableOutput, "enableOutput"_a)

      .def("setModeIndex", &ReconstructSurface<RBFSSparseTransform>::setModeIndex, "modeIndex"_a)

      .def("setNumOfModes", &ReconstructSurface<RBFSSparseTransform>::setNumOfModes, "numOfModes"_a)

      .def("setNumOfSamplesPerMode", &ReconstructSurface<RBFSSparseTransform>::setNumOfSamplesPerMode,
           "numOfSamplesPerMode"_a)

      .def("setNumOfParticles", &ReconstructSurface<RBFSSparseTransform>::setNumOfParticles, "numOfParticles"_a)

      .def("setNumOfClusters", &ReconstructSurface<RBFSSparseTransform>::setNumOfClusters, "numOfClusters"_a)

      .def("setMaxStdDev", &ReconstructSurface<RBFSSparseTransform>::setMaxStdDev, "maxStdDev"_a)

      .def("setMaxVarianceCaptured", &ReconstructSurface<RBFSSparseTransform>::setMaxVarianceCaptured,
           "maxVarianceCaptured"_a)

      .def("setMaxAngleDegrees", &ReconstructSurface<RBFSSparseTransform>::setMaxAngleDegrees, "maxAngleDegrees"_a)

      .def("surface", &ReconstructSurface<RBFSSparseTransform>::surface, "localPointsFiles"_a)

      .def("samplesAlongPCAModes", &ReconstructSurface<RBFSSparseTransform>::samplesAlongPCAModes, "worldPointsFiles"_a)

      .def("meanSurface", &ReconstructSurface<RBFSSparseTransform>::meanSurface, "distanceTransformFiles"_a,
           "localPointsFiles"_a, "worldPointsFiles"_a);

  // Optimize (TODO)
  py::class_<Optimize>(m, "Optimize")

      .def(py::init<>())

      .def("LoadParameterFile", &Optimize::LoadParameterFile)

      .def("SetUpOptimize", &Optimize::SetUpOptimize, "projectFile"_a)

      .def("Run", &Optimize::Run)

      .def("SetIterationCallbackFunction", &Optimize::SetIterationCallbackFunction)

      .def("GetParticleSystem", &optimize_get_particle_system);

  // DomainType
  py::enum_<DomainType>(m, "DomainType")
      .value("MeshDomain", DomainType::Mesh)
      .value("ImageDomain", DomainType::Image)
      .value("ContourDomain", DomainType::Contour)
      .export_values();
  ;

  // Project
  py::class_<Project, std::shared_ptr<Project> /* <- holder type */> proj(m, "Project");
  proj.def(py::init<>())

      .def("load", &Project::load, "Load from XLSX file", "filename"_a)
      .def(
          "save",
          [](Project& project, std::string filename) -> decltype(auto) {
            // store cwd
            std::string cwd = fs::current_path().string();
            project.save(filename);
            // restore cwd
            fs::current_path(cwd);
          },
          "Save to XLSX file", "filename"_a = "")
      .def("get_filename", &Project::get_filename, "Return the filename")
      .def("get_project_path", &Project::get_project_path, "Return the project path")
      .def("set_filename", &Project::set_filename, "Set project filename", "filename"_a)
      .def("get_headers", &Project::get_headers, "Return the headers of the subject sheet")
      .def("get_string_column", &Project::get_string_column, "Return a column by name", "name"_a)
      .def("get_number_of_subjects", &Project::get_number_of_subjects, "Return the number of subjects in the project")
      .def("get_number_of_domains_per_subject", &Project::get_number_of_domains_per_subject,
           "Return the number of domains")
      .def("get_original_domain_types", &Project::get_original_domain_types, "Return the original domain types")
      .def("set_original_domain_types", &Project::set_original_domain_types, "Set the original domain types", "types"_a)
      .def("get_groomed_domain_types", &Project::get_groomed_domain_types, "Return the groomed domain types")
      .def("set_groomed_domain_types", &Project::set_groomed_domain_types, "Set the groomed domain types", "types"_a)
      .def("get_domain_names", &Project::get_domain_names, "Return the domain names (e.g. femur, pelvis, etc)")
      .def(
          "get_subjects",
          [](Project& project) -> decltype(auto) {
            std::vector<Subject> py_subjects;
            for (auto s : project.get_subjects()) {
              py_subjects.push_back(*s);
            }
            return py_subjects;
          },
          "Return the list of Subjects")

      .def("get_originals_present", &Project::get_originals_present, "Return if original files are present")
      .def("get_groomed_present", &Project::get_groomed_present, "Return if groomed files are present")
      .def("get_particles_present", &Project::get_particles_present, "Return if particle files are present")
      .def("get_images_present", &Project::get_images_present)
      .def("get_feature_names", &Project::get_feature_names)
      .def("get_group_names", &Project::get_group_names)
      .def("get_group_values", &Project::get_group_values, "group_names"_a)
      .def("get_parameters", &Project::get_parameters, "name"_a, "domain_name"_a = "")
      .def("set_parameters", &Project::set_parameters, "name"_a, "params"_a, "domain_name"_a = "")
      .def("clear_parameters", &Project::clear_parameters, "name"_a)
      .def("store_subjects", &Project::update_subjects)
      .def("get_supported_version", &Project::get_supported_version)
      .def("get_version", &Project::get_version)
      .def(
          "set_subjects",
          [](Project& project, std::vector<Subject> subjects) -> decltype(auto) {
            std::vector<std::shared_ptr<Subject>> sharedSubjects;
            for (auto sub : subjects) {
              std::shared_ptr<Subject> s = std::make_shared<Subject>(sub);
              sharedSubjects.push_back(s);
            }
            return project.set_subjects(sharedSubjects);
          },
          "subjects"_a);  // Project

  // Subject
  py::class_<Subject>(m, "Subject")

      .def(py::init<>())

      .def("set_original_filenames", &Subject::set_original_filenames, "Set original filenames (one per domain)",
           "filenames"_a)

      .def("get_original_filenames", &Subject::get_original_filenames, "Get original filenames")

      .def("set_groomed_filenames", &Subject::set_groomed_filenames, "Set groomed filenames", "filenames"_a)

      .def("get_groomed_filenames", &Subject::get_groomed_filenames, "Get groomed filenames")

      .def("set_local_particle_filenames", &Subject::set_local_particle_filenames,
           "Set local particle filenames (one per domain)", "filenames"_a)

      .def("get_local_particle_filenames", &Subject::get_local_particle_filenames, "Get local particle filenames")

      .def("set_world_particle_filenames", &Subject::set_world_particle_filenames, "Set the world particle filenames",
           "filenames"_a)

      .def("get_world_particle_filenames", &Subject::get_world_particle_filenames, "Get the world particle filenames")

      .def("set_landmarks_filenames", &Subject::set_landmarks_filenames, "Set the landmarks filenames (one per domain)",
           "filenames"_a)

      .def("get_landmarks_filenames", &Subject::get_landmarks_filenames, "Get the landmarks filenames (one per domain)")

      .def("set_constraints_filenames", &Subject::set_constraints_filenames,
           "Set the constraint filenames (one per domain)", "filenames"_a)

      .def("get_constraints_filenames", &Subject::get_constraints_filenames,
           "Get the constraints filenames (one per domain)")

      .def("set_number_of_domains", &Subject::set_number_of_domains, "Set the number of domains", "number_of_domains"_a)

      .def("get_number_of_domains", &Subject::get_number_of_domains, "Get the number of domains")

      .def(
          "get_feature_filenames",
          [](Subject& subject) -> decltype(auto) {
            project::types::StringMap m = subject.get_feature_filenames();
            std::map<std::string, std::string> map;

            for (auto& [k, v] : m) {
              map[k] = v;
            }

            return map;
          },
          "Get the feature map filenames")

      .def(
          "set_feature_filenames",
          [](Subject& subject, std::map<std::string, std::string> map) -> decltype(auto) {
            project::types::StringMap m;
            for (auto& [k, v] : map) {
              m[k] = v;
            }
            subject.set_feature_filenames(m);
          },
          "Set the feature map filenames", "filenames"_a)

      .def("get_groomed_transforms", &Subject::get_groomed_transforms,
           "Get the groomed transforms (one vector per domain)")

      .def("set_groomed_transforms", &Subject::set_groomed_transforms,
           "Set the groomed transforms (one vector per domain)", "transforms"_a)

      .def("get_procrustes_transforms", &Subject::get_procrustes_transforms,
           "Get the procrustes transforms (one vector per domain)")

      .def("set_procrustes_transforms", &Subject::set_procrustes_transforms,
           "Set the procrustes transforms (one vector per domain)", "transforms"_a)

      .def("get_group_values", &Subject::get_group_values, "Get the group values map")

      .def("get_group_value", &Subject::get_group_value, "Get a specific group value", "group_name"_a)

      .def(
          "set_group_values",
          [](Subject& subject, std::map<std::string, std::string> map) -> decltype(auto) {
            project::types::StringMap m;

            for (auto& [k, v] : map) {
              m[k] = v;
            }
            subject.set_group_values(m);
          },
          "Set group values map"
          "group_values"_a)

      .def(
          "get_extra_values",
          [](Subject& subject) -> decltype(auto) {
            project::types::StringMap m = subject.get_extra_values();
            std::map<std::string, std::string> map;

            for (auto& [k, v] : m) {
              map[k] = v;
            }

            return map;
          },
          "Get extra values (extra columns we don't interpret)")

      .def(
          "set_extra_values",
          [](Subject& subject, std::map<std::string, std::string> map) -> decltype(auto) {
            project::types::StringMap m;

            for (auto& [k, v] : map) {
              m[k] = v;
            }
            subject.set_extra_values(m);
          },
          "Set extra values", "extra_values"_a)

      .def("get_display_name", &Subject::get_display_name, "Get the display name")

      .def("set_display_name", &Subject::set_display_name, "Set the display name", "display_name"_a)

      .def("set_excluded", &Subject::set_excluded, "Set excluded", "excluded"_a)

      .def("set_fixed", &Subject::set_fixed, "Set fixed", "fixed"_a)

      .def(
          "get_groomed_clipped_mesh",
          [](Subject& subject, int domain_id) -> decltype(auto) {
            if (domain_id >= subject.get_groomed_filenames().size()) {
              throw std::invalid_argument("domain_id out of range");
            }
            auto groomed_filename = subject.get_groomed_filenames()[domain_id];
            Mesh mesh = MeshUtils::create_mesh_from_file(groomed_filename);
            // if there are constraints, apply them
            if (domain_id < subject.get_constraints_filenames().size()) {
              auto constraints_filename = subject.get_constraints_filenames()[domain_id];
              Constraints constraint;
              constraint.read(constraints_filename);
              constraint.clipMesh(mesh);
            }
            return mesh;
          },
          "Get the mesh clipped by constraints", "domain_id"_a = 0)

      ;  // Subject

  // Parameters
  py::class_<Parameters>(m, "Parameters")

      .def(py::init<>())

      .def(
          "get",
          [](Parameters& params, const std::string& key, const std::string& default_value) -> decltype(auto) {
            return static_cast<std::string>(params.get(key, Variant(default_value)));
          },
          "get a parameter based on a key, return default if it doesn't exist", "key"_a, "default"_a = "")

      .def("key_exists", &Parameters::key_exists, "return if a key exists or not", "key"_a)

      .def("set", &Parameters::set, "set a parameter based on a key", "key"_a, "Variant"_a)

      .def(
          "set",
          [](Parameters& params, const std::string& key, const std::string& value) -> decltype(auto) {
            params.set(key, Variant(value));
          },
          "set a parameter based on a key", "key"_a, "value"_a)

      .def("remove_entry", &Parameters::remove_entry, "remove an entry", "key"_a)

      .def(
          "as_map",
          [](const Parameters& params) -> decltype(auto) {
            project::types::StringMap m = params.get_map();
            std::map<std::string, std::string> map;

            for (auto& [k, v] : m) {
              map[k] = v;
            }

            return map;
          },
          "get underlying map")

      .def("reset_parameters", &Parameters::reset_parameters,
           "reset parameters to blank");  // Parameters

  py::class_<Variant>(m, "Variant")

      .def(py::init<>())
      .def(py::init<std::vector<int>>())
      .def(py::init<std::vector<double>>())
      .def(py::init<std::vector<bool>>())
      .def(py::init<const std::string&>())
      .def(py::init<int>())
      .def(py::init<double>())
      .def(py::init<const char*>())
      .def(py::init<bool>())

      .def(
          "as_str", [](const Variant& v) -> decltype(auto) { return static_cast<std::string>(v); },
          "Return the variant string content")

      ;  // Variant

  py::class_<MorphologicalDeviationScore>(m, "MorphologicalDeviationScore")
      .def(py::init<>(), "Create an instance for MorphologicalDeviationScore")
      .def("SetControlShapes", &MorphologicalDeviationScore::SetControlShapes,
           py::arg("X"),
           R"pbdoc(
                 Fit PPCA model on control shapes.

                 Parameters
                 ----------
                 X : numpy.ndarray
                     Matrix of control shapes (n_samples x n_features)

                 Returns
                 -------
                 bool
                     True if fitting was successful, False otherwise.
             )pbdoc")

      .def("GetMorphoDevScore",
           &MorphologicalDeviationScore::GetMorphoDevScore, py::arg("X"),
           R"pbdoc(
                 Compute Mahalanobis-based deviation score for test samples.

                 Parameters
                 ----------
                 X : numpy.ndarray
                     Matrix of test samples (n_samples x n_features)

                 Returns
                 -------
                 numpy.ndarray
                     Vector of Mahalanobis distances for each sample.
             )pbdoc");
}  // PYBIND11_MODULE(shapeworks_py)

#include "ImageUtils.h"

#include <Eigen/Dense>
#include <itkPointSet.h>
#include <pthread.h>

// ITK image factories
#include <itkMetaImageIOFactory.h>
#include <itkNiftiImageIOFactory.h>
#include <itkNrrdImageIOFactory.h>
#include <itkResampleImageFilter.h>

#include "Logging.h"

namespace shapeworks {

PhysicalRegion ImageUtils::boundingBox(const std::vector<std::string>& filenames, Image::PixelType isoValue) {
  if (filenames.empty()) throw std::invalid_argument("No filenames provided to compute a bounding box");

  Image img(filenames[0]);
  PhysicalRegion bbox;
  Dims dims(img.dims());  // images must all be the same size

  for (auto filename : filenames) {
    Image img(filename);
    if (img.dims() != dims) {
      throw std::invalid_argument("Image sizes do not match (" + filename + ")");
    }

    bbox.expand(img.physicalBoundingBox(isoValue));
  }

  return bbox;
}

PhysicalRegion ImageUtils::boundingBox(const std::vector<std::reference_wrapper<const Image>>& images,
                                       Image::PixelType isoValue) {
  if (images.empty()) throw std::invalid_argument("No images provided to compute a bounding box");

  PhysicalRegion bbox;
  Dims dims(images[0].get().dims());  // images must all be the same size

  for (auto img : images) {
    if (img.get().dims() != dims) throw std::invalid_argument("Image sizes do not match");

    bbox.expand(img.get().physicalBoundingBox(isoValue));
  }

  return bbox;
}

void ImageUtils::register_itk_factories() {
  static bool registered = false;
  if (!registered) {
    // register all the factories
    itk::NrrdImageIOFactory::RegisterOneFactory();
    itk::NiftiImageIOFactory::RegisterOneFactory();
    itk::MetaImageIOFactory::RegisterOneFactory();
    registered = true;
  }
}

//------------------------------------------------------------------------------
ImageType::Pointer ImageUtils::make_axis_aligned(ImageType::Pointer input) {
  // Check if image is already axis-aligned
  auto direction = input->GetDirection();
  bool is_oblique = false;
  for (unsigned int i = 0; i < 3; i++) {
    for (unsigned int j = 0; j < 3; j++) {
      double expected = (i == j) ? 1.0 : 0.0;
      if (std::abs(direction(i, j) - expected) > 1e-6) {
        is_oblique = true;
        break;
      }
    }
    if (is_oblique) {
      break;
    }
  }

  // If already axis-aligned, return as-is
  if (!is_oblique) {
    return input;
  }

  SW_DEBUG("Image is oblique, resampling to axis-aligned");

  using ResampleFilterType = itk::ResampleImageFilter<ImageType, ImageType>;
  auto resampler = ResampleFilterType::New();

  // Set identity direction
  ImageType::DirectionType identity_direction;
  identity_direction.SetIdentity();

  // Calculate bounding box and PROPER SPACING
  auto region = input->GetLargestPossibleRegion();
  auto size = region.GetSize();
  auto spacing = input->GetSpacing();

  // Find bounding box corners
  Point3 min_pt, max_pt;
  for (int i = 0; i < 3; i++) {
    min_pt[i] = std::numeric_limits<double>::max();
    max_pt[i] = std::numeric_limits<double>::lowest();
  }

  for (int corner = 0; corner < 8; corner++) {
    ImageType::IndexType index;
    index[0] = (corner & 1) ? size[0] - 1 : 0;
    index[1] = (corner & 2) ? size[1] - 1 : 0;
    index[2] = (corner & 4) ? size[2] - 1 : 0;

    Point3 point;
    input->TransformIndexToPhysicalPoint(index, point);

    for (int dim = 0; dim < 3; dim++) {
      min_pt[dim] = std::min(min_pt[dim], point[dim]);
      max_pt[dim] = std::max(max_pt[dim], point[dim]);
    }
  }

  // Calculate spacing for axis-aligned coordinate system
  ImageType::SpacingType output_spacing;
  for (int i = 0; i < 3; i++) {
    // Find the minimum effective spacing along this axis
    double min_spacing = std::numeric_limits<double>::max();
    for (int j = 0; j < 3; j++) {
      if (std::abs(direction(i, j)) > 1e-6) {
        min_spacing = std::min(min_spacing, spacing[j] / std::abs(direction(i, j)));
      }
    }
    output_spacing[i] = min_spacing;
  }

  // Calculate output size with corrected spacing
  ImageType::SizeType output_size;
  for (int i = 0; i < 3; i++) {
    output_size[i] = static_cast<size_t>(std::ceil((max_pt[i] - min_pt[i]) / output_spacing[i])) + 1;
  }

  resampler->SetInput(input);
  resampler->SetOutputDirection(identity_direction);
  resampler->SetOutputOrigin(min_pt);
  resampler->SetOutputSpacing(output_spacing);
  resampler->SetSize(output_size);

  resampler->Update();
  return resampler->GetOutput();
}

//------------------------------------------------------------------------------
// libgomp can't start threads in a forked child once the parent has used OpenMP, so Eigen's parallel products hang
// there (e.g. the DataAugmentation multiprocessing pool). Keep Eigen single threaded in forked children.
[[maybe_unused]] static const int eigen_fork_handler =
    pthread_atfork(nullptr, nullptr, [] { Eigen::setNbThreads(1); });

//------------------------------------------------------------------------------
void EigenThinPlateSplineKernelTransform::ComputeWMatrix() {
  const Eigen::Index num_landmarks = m_SourceLandmarks->GetNumberOfPoints();
  const Eigen::Index n = num_landmarks + 4;

  ComputeD();

  std::vector<InputPointType> points;
  points.reserve(num_landmarks);
  for (auto it = m_SourceLandmarks->GetPoints()->Begin(); it != m_SourceLandmarks->GetPoints()->End(); ++it) {
    points.push_back(it.Value());
  }

  // L = [K P; P^T 0] with K(i,j) = |p_i - p_j| and P rows [p_i 1]. Y holds one column of displacements per axis.
  Eigen::MatrixXd l_matrix = Eigen::MatrixXd::Zero(n, n);
  Eigen::MatrixXd y_matrix = Eigen::MatrixXd::Zero(n, 3);
  auto displacement = m_Displacements->Begin();
  for (Eigen::Index i = 0; i < num_landmarks; ++i, ++displacement) {
    l_matrix(i, i) = m_Stiffness;
    for (Eigen::Index j = i + 1; j < num_landmarks; ++j) {
      l_matrix(i, j) = l_matrix(j, i) = (points[i] - points[j]).GetNorm();
    }
    for (int d = 0; d < 3; ++d) {
      l_matrix(i, num_landmarks + d) = l_matrix(num_landmarks + d, i) = points[i][d];
      y_matrix(i, d) = displacement.Value()[d];
    }
    l_matrix(i, num_landmarks + 3) = l_matrix(num_landmarks + 3, i) = 1.0;
  }

  Eigen::MatrixXd weights = l_matrix.partialPivLu().solve(y_matrix);

  // LU needs L to be invertible, which duplicate or coplanar landmarks break. Fall back to the minimum norm solution,
  // matching the SVD pseudo-inverse that KernelTransform::ComputeWMatrix uses.
  if (!weights.allFinite() || (l_matrix * weights - y_matrix).norm() > 1e-6 * y_matrix.norm()) {
    weights = l_matrix.completeOrthogonalDecomposition().solve(y_matrix);
  }

  // Same layout as KernelTransform::ReorganizeW
  m_DMatrix.set_size(3, num_landmarks);
  for (int d = 0; d < 3; ++d) {
    for (Eigen::Index i = 0; i < num_landmarks; ++i) {
      m_DMatrix(d, i) = weights(i, d);
    }
    for (int j = 0; j < 3; ++j) {
      m_AMatrix(d, j) = weights(num_landmarks + j, d);
    }
    m_BVector(d) = weights(num_landmarks + 3, d);
  }
}

//------------------------------------------------------------------------------
ImageUtils::TPSTransform::Pointer ImageUtils::createWarpTransform(const std::string& source_landmarks_file,
                                                                  const std::string& target_landmarks_file,
                                                                  const int stride) {
  typedef TPSTransform::PointSetType PointSet;

  // Read the source and target sets of landmark points
  PointSet::Pointer sourceLandMarks = PointSet::New();
  PointSet::Pointer targetLandMarks = PointSet::New();
  PointSet::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
  PointSet::PointsContainer::Pointer targetLandMarkContainer = targetLandMarks->GetPoints();

  std::ifstream insourcefile;
  std::ifstream intargetfile;
  insourcefile.open(source_landmarks_file);
  intargetfile.open(target_landmarks_file);
  if (!insourcefile.is_open() || !intargetfile.is_open()) {
    throw std::invalid_argument("Error: unable to open landmark files. Check paths.");
  }

  PointSet::PointIdentifier id{itk::NumericTraits<PointSet::PointIdentifier>::Zero};
  Point3 src, tgt;
  int count{0};

  while (!insourcefile.eof() && !intargetfile.eof()) {
    insourcefile >> src[0] >> src[1] >> src[2];
    intargetfile >> tgt[0] >> tgt[1] >> tgt[2];

    if (count % stride == 0) {
      // swap src and tgt b/c ITK transforms must be inverted on creation since some do not provide an invert function
      sourceLandMarkContainer->InsertElement(id, tgt);
      targetLandMarkContainer->InsertElement(id, src);
      id++;
    }
    count++;
  }
  insourcefile.close();
  intargetfile.close();

  // Create and return warp transform
  TPSTransform::Pointer tps(TPSTransform::New());
  tps->SetSourceLandmarks(sourceLandMarks);
  tps->SetTargetLandmarks(targetLandMarks);
  tps->ComputeWMatrix();

  return tps;
}

}  // namespace shapeworks

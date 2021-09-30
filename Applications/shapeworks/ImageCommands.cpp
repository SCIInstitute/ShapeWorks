#include "Commands.h"
#include "Image.h"
#include "ImageUtils.h"

namespace shapeworks {

///////////////////////////////////////////////////////////////////////////////
// ReadImage
///////////////////////////////////////////////////////////////////////////////
void ReadImage::buildParser()
{
  const std::string prog = "read-image";
  const std::string desc = "reads an image";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of file to read.");

  Command::buildParser();
}

bool ReadImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];
  if (filename.length() == 0) {
    std::cerr << "readimage error: no filename specified, must pass `--name <filename>`\n";
    return false;
  }

  try {
    sharedData.image = Image(filename);
    return true;
  } catch(std::exception &e) {
    std::cerr << "exception while reading " << filename << ": " << e.what() << std::endl;
    return false;
  } catch(...) {
    std::cerr << "unknown exception while reading " << filename << std::endl;
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// WriteImage
///////////////////////////////////////////////////////////////////////////////
void WriteImage::buildParser()
{
  const std::string prog = "write-image";
  const std::string desc = "writes the current image (determines type by its extension)";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of file to write.");
  parser.add_option("--compressed").action("store").type("bool").set_default(true).help("Whether to compress file [default: true].");

  Command::buildParser();
}

bool WriteImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  std::string filename = options["name"];
  if (filename.length() == 0) {
    std::cerr << "writeimage error: no filename specified, must pass `--name <filename>`\n";
    return false;
  }

  bool compressed = static_cast<bool>(options.get("compressed"));

  sharedData.image.write(filename, compressed);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ImageInfo
///////////////////////////////////////////////////////////////////////////////
void ImageInfo::buildParser()
{
  const std::string prog = "image-info";
  const std::string desc = "prints requested image dimensions, spacing, size, origin, direction (coordinate system), center, center of mass and bounding box [default: prints everything]";
  parser.prog(prog).description(desc);

  parser.add_option("--dims").action("store_true").set_default(false).help("Whether to display image dimensions [default: true].");
  parser.add_option("--spacing").action("store_true").set_default(false).help("Whether to display physical spacing [default: true].");
  parser.add_option("--size").action("store_true").set_default(false).help("Whether to display size [default: true].");
  parser.add_option("--origin").action("store_true").set_default(false).help("Whether to display physical origin [default: true].");
  parser.add_option("--direction").action("store_true").set_default(false).help("Whether to display direction [default: true].");
  parser.add_option("--center").action("store_true").set_default(false).help("Whether to display center. [default: true]");
  parser.add_option("--centerofmass").action("store_true").set_default(false).help("Whether to display center of mass. [default: true]");
  parser.add_option("--boundingbox").action("store_true").set_default(false).help("Whether to display bounding box. [default: true]");

  Command::buildParser();
}

bool ImageInfo::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  bool dims = static_cast<bool>(options.get("dims"));
  bool spacing = static_cast<bool>(options.get("spacing"));
  bool size = static_cast<bool>(options.get("size"));
  bool origin = static_cast<bool>(options.get("origin"));
  bool direction = static_cast<bool>(options.get("direction"));
  bool center = static_cast<bool>(options.get("center"));
  bool centerOfMass = static_cast<bool>(options.get("centerofmass"));
  bool boundingBox = static_cast<bool>(options.get("boundingbox"));

  // by default: print everything
  if (options.num_set() == 0)
    dims = spacing = size = origin = direction = center = centerOfMass = boundingBox = true;

  if (dims)
    std::cout << "image dimensions:      " << sharedData.image.dims() << std::endl;
  if (spacing)
    std::cout << "physical spacing:      " << sharedData.image.spacing() << std::endl;
  if (size)
    std::cout << "size (spacing * dims): " << sharedData.image.size() << std::endl;
  if (origin)
    std::cout << "physical origin:       " << sharedData.image.origin() << std::endl;
  if (center)
    std::cout << "center:                " << sharedData.image.center() << std::endl;
  if (centerOfMass)
    std::cout << "center of mass (0,1]:  " << sharedData.image.centerOfMass() << std::endl;
  if (boundingBox)
    std::cout << "physical bounding box: " << sharedData.image.physicalBoundingBox() << std::endl;
  if (boundingBox)
    std::cout << "logical bounding box:  " << sharedData.image.logicalBoundingBox() << std::endl;
  if (direction)
    std::cout << "direction (coordsys):  " << std::endl
              << sharedData.image.coordsys();

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Antialias
///////////////////////////////////////////////////////////////////////////////
void Antialias::buildParser()
{
  const std::string prog = "antialias";
  const std::string desc = "antialiases binary volumes";
  parser.prog(prog).description(desc);

  parser.add_option("--iterations").action("store").type("int").set_default(50).help("Maximum number of iterations [default: %default].");
  parser.add_option("--maxrmserror").action("store").type("double").set_default(0.01).help("Maximum RMS error determines how fast the solver converges. Range [0.0, 1.0], larger is faster [default: %default].");
  parser.add_option("--layers").action("store").type("int").set_default(3).help("Number of layers around a 3d pixel to use for this computation [default: %default].");

  Command::buildParser();
}

bool Antialias::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  int iterations = static_cast<int>(options.get("iterations"));
  double maxRMSErr = static_cast<double>(options.get("maxrmserror"));
  int layers = static_cast<int>(options.get("layers"));

  sharedData.image.antialias(iterations, maxRMSErr, layers);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ResampleImage
///////////////////////////////////////////////////////////////////////////////
void ResampleImage::buildParser()
{
  const std::string prog = "resample";
  const std::string desc = "resamples an image using new physical spacing (computes new dims)";
  parser.prog(prog).description(desc);

  parser.add_option("--isospacing").action("store").type("double").set_default(0.0).help("Use this spacing in all dimensions.");
  parser.add_option("--spacex").action("store").type("double").set_default(1.0).help("Pixel spacing in x-direction [default: %default].");
  parser.add_option("--spacey").action("store").type("double").set_default(1.0).help("Pixel spacing in y-direction [default: %default].");
  parser.add_option("--spacez").action("store").type("double").set_default(1.0).help("Pixel spacing in z-direction [default: %default].");
  parser.add_option("--sizex").action("store").type("unsigned").set_default(0).help("Output size in x-direction [default: current size].");
  parser.add_option("--sizey").action("store").type("unsigned").set_default(0).help("Output size in y-direction [default: current size].");
  parser.add_option("--sizez").action("store").type("unsigned").set_default(0).help("Output size in z-direction [default: current size].");
  parser.add_option("--originx").action("store").type("double").set_default(std::numeric_limits<float>::max()).help("Output origin in x-direction [default: current origin].");
  parser.add_option("--originy").action("store").type("double").set_default(std::numeric_limits<float>::max()).help("Output origin in y-direction [default: current origin].");
  parser.add_option("--originz").action("store").type("double").set_default(std::numeric_limits<float>::max()).help("Output origin in z-direction [default: current origin].");
  std::list<std::string> interps{"linear", "nearest"};
  parser.add_option("--interp").action("store").type("choice").choices(interps.begin(), interps.end()).set_default("linear").help("Interpolation method to use [default: %default].");

  Command::buildParser();
}

bool ResampleImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double isoSpacing = static_cast<double>(options.get("isospacing"));
  double spaceX = static_cast<double>(options.get("spacex"));
  double spaceY = static_cast<double>(options.get("spacey"));
  double spaceZ = static_cast<double>(options.get("spacez"));
  unsigned sizeX = static_cast<unsigned>(options.get("sizex"));
  unsigned sizeY = static_cast<unsigned>(options.get("sizey"));
  unsigned sizeZ = static_cast<unsigned>(options.get("sizez"));
  double originX = static_cast<double>(options.get("originx"));
  double originY = static_cast<double>(options.get("originy"));
  double originZ = static_cast<double>(options.get("originz"));
  std::string interpopt(options.get("interp"));
  Image::InterpolationType interp = interpopt == "nearest" ? Image::NearestNeighbor : Image::Linear;

  if (isoSpacing > 0.0)
    sharedData.image.resample(isoSpacing, interp);
  else if (sizeX == 0 || sizeY == 0 || sizeX == 0)
    sharedData.image.resample(makeVector({spaceX, spaceY, spaceZ}), interp);
  else 
  {
    Point3 origin({originX, originY, originZ});
    if (originX >= 1e9 || originY >= 1e9 || originZ >= 1e9)
      origin = sharedData.image.origin();
    sharedData.image.resample(IdentityTransform::New(), origin,
                              Dims({sizeX, sizeY, sizeZ}), makeVector({spaceX, spaceY, spaceZ}),
                              sharedData.image.coordsys(), interp);
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ResizeImage
///////////////////////////////////////////////////////////////////////////////
void ResizeImage::buildParser()
{
  const std::string prog = "resize";
  const std::string desc = "resizes an image (computes new physical spacing)";
  parser.prog(prog).description(desc);

  parser.add_option("--sizex", "-x").action("store").type("unsigned").set_default(0).help("Output size in x-direction [default: current size].");
  parser.add_option("--sizey", "-y").action("store").type("unsigned").set_default(0).help("Output size in y-direction [default: current size].");
  parser.add_option("--sizez", "-z").action("store").type("unsigned").set_default(0).help("Output size in z-direction [default: current size].");

  Command::buildParser();
}

bool ResizeImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  unsigned sizeX = static_cast<unsigned>(options.get("sizex"));
  unsigned sizeY = static_cast<unsigned>(options.get("sizey"));
  unsigned sizeZ = static_cast<unsigned>(options.get("sizez"));

  sharedData.image.resize(Dims({sizeX, sizeY, sizeZ}));
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// RecenterImage
///////////////////////////////////////////////////////////////////////////////
void RecenterImage::buildParser()
{
  const std::string prog = "recenter";
  const std::string desc = "recenters an image by changing its origin in the image header to the physical coordinates of the center of the image";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool RecenterImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  sharedData.image.recenter();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// PadImage
///////////////////////////////////////////////////////////////////////////////
void PadImage::buildParser()
{
  const std::string prog = "pad";
  const std::string desc = "pads an image with specified value by specified number of voxels in the x-, y-, and/or z- directions; origin remains at the same location (note: negative padding to shrink an image is permitted)";
  parser.prog(prog).description(desc);

  parser.add_option("--padding").action("store").type("int").set_default(0).help("Pad this many voxels in ALL directions (used if set) [default: %default].");
  parser.add_option("--padx","-x").action("store").type("int").set_default(0).help("Pad this many voxels in the x-direction [default: %default].");
  parser.add_option("--pady","-y").action("store").type("int").set_default(0).help("Pad this many voxels in the y-direction [default: %default].");
  parser.add_option("--padz","-z").action("store").type("int").set_default(0).help("Pad this many voxels in the z-direction [default: %default].");
  parser.add_option("--value").action("store").type("double").set_default(0.0).help("Value used to fill padded voxels [default: %default].");

  Command::buildParser();
}

bool PadImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  int padding = static_cast<int>(options.get("padding"));
  int padx = static_cast<int>(options.get("padx"));
  int pady = static_cast<int>(options.get("pady"));
  int padz = static_cast<int>(options.get("padz"));
  double value = static_cast<double>(options.get("value"));

  if (padding < 0 || padding > 0)
    sharedData.image.pad(padding, value);
  else
    sharedData.image.pad(padx, pady, padz, value);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// TranslateImage
///////////////////////////////////////////////////////////////////////////////
void TranslateImage::buildParser()
{
  const std::string prog = "translate-image";
  const std::string desc = "translates image by specified physical (image space) distance";
  parser.prog(prog).description(desc);

  parser.add_option("--centerofmass").action("store_true").set_default(false).help("Use center of mass [default: false].");
  parser.add_option("--tx", "-x").action("store").type("double").set_default(0).help("X distance.");
  parser.add_option("--ty", "-y").action("store").type("double").set_default(0).help("Y distance.");
  parser.add_option("--tz", "-z").action("store").type("double").set_default(0).help("Z distance.");

  Command::buildParser();
}

bool TranslateImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  bool centerOfMass = static_cast<bool>(options.get("centerofmass"));

  if (centerOfMass)
  {
    sharedData.image.applyTransform(sharedData.image.createTransform(XFormType::CenterOfMass));
    return true;
  }
  else
  {
    double tx = static_cast<double>(options.get("tx"));
    double ty = static_cast<double>(options.get("ty"));
    double tz = static_cast<double>(options.get("tz"));

    sharedData.image.translate(makeVector({tx, ty, tz}));
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// ScaleImage
///////////////////////////////////////////////////////////////////////////////
void ScaleImage::buildParser()
{
  const std::string prog = "scale-image";
  const std::string desc = "scales image by specified value";
  parser.prog(prog).description(desc);

  parser.add_option("--sx", "-x").action("store").type("double").set_default(1.0).help("X scale.");
  parser.add_option("--sy", "-y").action("store").type("double").set_default(1.0).help("Y scale.");
  parser.add_option("--sz", "-z").action("store").type("double").set_default(1.0).help("Z scale.");

  Command::buildParser();
}

bool ScaleImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double sx = static_cast<double>(options.get("sx"));
  double sy = static_cast<double>(options.get("sy"));
  double sz = static_cast<double>(options.get("sz"));

  sharedData.image.scale(makeVector({sx, sy, sz}));
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Rotate
///////////////////////////////////////////////////////////////////////////////
void Rotate::buildParser()
{
  const std::string prog = "rotate";
  const std::string desc = "rotates image by specified value";
  parser.prog(prog).description(desc);

  parser.add_option("--rx", "-x").action("store").type("double").help("Physical axis around which to rotate [default: z-axis].");
  parser.add_option("--ry", "-y").action("store").type("double").help("Physical axis around which to rotate [default: z-axis].");
  parser.add_option("--rz", "-z").action("store").type("double").set_default(1.0).help("Physical axis around which to rotate [default: z-axis].");
  parser.add_option("--radians").action("store").type("double").help("Angle in radians.");
  parser.add_option("--degrees").action("store").type("double").help("Angle in degrees.");

  Command::buildParser();
}

bool Rotate::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double rx = static_cast<double>(options.get("rx"));
  double ry = static_cast<double>(options.get("ry"));
  double rz = static_cast<double>(options.get("rz"));
  double radians = static_cast<double>(options.get("radians"));
  double degrees = static_cast<double>(options.get("degrees"));

  Vector3 axis(makeVector({rx, ry, rz}));

  if (radians == 0.0 && degrees == 0.0)
  {
    std::cerr << "Must specify a rotation angle\n";
    return false;
  }
  // if degrees is specified, use it
  if (degrees != 0.0)
    radians = degToRad(degrees);

  sharedData.image.rotate(radians, makeVector({rx, ry, rz}));
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ExtractLabel
///////////////////////////////////////////////////////////////////////////////
void ExtractLabel::buildParser()
{
  const std::string prog = "extract-label";
  const std::string desc = "extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image";
  parser.prog(prog).description(desc);

  parser.add_option("--label").action("store").type("double").set_default(1.0).help("Label value to be extracted [default: %default].");

  Command::buildParser();
}

bool ExtractLabel::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double label = static_cast<double>(options.get("label"));

  sharedData.image.extractLabel(label);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// CloseHoles
///////////////////////////////////////////////////////////////////////////////
void CloseHoles::buildParser()
{
  const std::string prog = "close-holes";
  const std::string desc = "closes holes in a volume defined by values larger than specified value";
  parser.prog(prog).description(desc);

  parser.add_option("--value").action("store").type("double").set_default(0.0).help("Largest value not in volume [default: %default].");

  Command::buildParser();
}

bool CloseHoles::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double value = static_cast<double>(options.get("value"));

  sharedData.image.closeHoles(value);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Binarize
///////////////////////////////////////////////////////////////////////////////
void Binarize::buildParser()
{
  const std::string prog = "binarize";
  const std::string desc = "sets portion of image greater than min and less than or equal to max to the specified value";
  parser.prog(prog).description(desc);

  parser.add_option("--min").action("store").type("double").set_default(std::numeric_limits<double>::epsilon()).help("Lower threshold level [default: 0.0].");
  parser.add_option("--max").action("store").type("double").set_default(std::numeric_limits<double>::max()).help("Upper threshold level [default: inf ].");
  parser.add_option("--value").action("store").type("double").set_default(1.0).help("Value to set region [default: %default].");

  Command::buildParser();
}

bool Binarize::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double min = static_cast<double>(options.get("min"));
  double max = static_cast<double>(options.get("max"));
  double value = static_cast<double>(options.get("value"));

  sharedData.image.binarize(min, max, value);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ComputeDT
///////////////////////////////////////////////////////////////////////////////
void ComputeDT::buildParser()
{
  const std::string prog = "compute-dt";
  const std::string desc = "computes signed distance transform volume from an image at the specified isovalue";
  parser.prog(prog).description(desc);

  parser.add_option("--isovalue").action("store").type("double").set_default(0.0).help("Level set value that defines the interface between foreground and background [default: %default].");

  Command::buildParser();
}

bool ComputeDT::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double isoValue = static_cast<double>(options.get("isovalue"));

  sharedData.image.computeDT(isoValue);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// CurvatureFilter
///////////////////////////////////////////////////////////////////////////////
void CurvatureFilter::buildParser()
{
  const std::string prog = "curvature";
  const std::string desc = "denoises an image using curvature driven flow using curvature flow image filter";
  parser.prog(prog).description(desc);

  parser.add_option("--iterations").action("store").type("int").set_default(10).help("Number of iterations [default: %default].");

  Command::buildParser();
}

bool CurvatureFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  int iterations = static_cast<int>(options.get("iterations"));

  if (iterations < 0)
  {
    std::cerr << "iterations must be >= 0\n";
    return false;
  }
  else
  {
    sharedData.image.applyCurvatureFilter(iterations);
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// GradientFilter
///////////////////////////////////////////////////////////////////////////////
void GradientFilter::buildParser()
{
  const std::string prog = "gradient";
  const std::string desc = "computes gradient magnitude of an image region at each pixel using gradient magnitude filter";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool GradientFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  sharedData.image.applyGradientFilter();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// SigmoidFilter
///////////////////////////////////////////////////////////////////////////////
void SigmoidFilter::buildParser()
{
  const std::string prog = "sigmoid";
  const std::string desc = "computes sigmoid function pixel-wise using sigmoid image filter";
  parser.prog(prog).description(desc);

  parser.add_option("--alpha").action("store").type("double").set_default(10.0).help("Value of alpha [default: %default].");
  parser.add_option("--beta").action("store").type("double").set_default(10.0).help("Value of beta [default: %default].");

  Command::buildParser();
}

bool SigmoidFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double alpha = static_cast<double>(options.get("alpha"));
  double beta = static_cast<double>(options.get("beta"));

  sharedData.image.applySigmoidFilter(alpha, beta);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// TPLevelSetFilter
///////////////////////////////////////////////////////////////////////////////
void TPLevelSetFilter::buildParser()
{
  const std::string prog = "tp-levelset";
  const std::string desc = "segments structures in image using topology preserving geodesic active contour level set filter";
  parser.prog(prog).description(desc);

  parser.add_option("--featureimage").action("store").type("string").set_default("").help("Path of feature image for filter");
  parser.add_option("--scaling").action("store").type("double").set_default(20.0).help("Value of scale [default: %default].");

  Command::buildParser();
}

bool TPLevelSetFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  std::string featureImagePath = static_cast<std::string>(options.get("featureimage"));

  if (featureImagePath == "")
  {
    std::cerr << "Must specify a feature image\n";
    return false;
  }
  else
  {
    Image featureImage(featureImagePath);
    double scaling = static_cast<double>(options.get("scaling"));

    sharedData.image.applyTPLevelSetFilter(featureImage, scaling);
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// IntensityFilter
///////////////////////////////////////////////////////////////////////////////
void IntensityFilter::buildParser()
{
  const std::string prog = "intensity";
  const std::string desc = "applies intensity windowing image filter";
  parser.prog(prog).description(desc);

  parser.add_option("--min").action("store").type("double").set_default(0.0).help("Minimum value of window [default: %default].");
  parser.add_option("--max").action("store").type("double").set_default(0.0).help("Maximum value of window [default: %default].");

  Command::buildParser();
}

bool IntensityFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double min = static_cast<double>(options.get("min"));
  double max = static_cast<double>(options.get("max"));

  sharedData.image.applyIntensityFilter(min, max);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// TopologyPreservingFilter
///////////////////////////////////////////////////////////////////////////////
void TopologyPreservingFilter::buildParser()
{
  const std::string prog = "topo-preserving-smooth";
  const std::string desc = "helper command that applies gradient and sigmoid filters to create a feature image for the TPLevelSet filter; note that a curvature flow filter is sometimes applied to the image before this";
  parser.prog(prog).description(desc);

  parser.add_option("--scaling").action("store").type("double").set_default(20.0).help("Scale for TPLevelSet level set filter [default: %default].");
  parser.add_option("--alpha").action("store").type("double").set_default(10.0).help("Value of alpha for sigmoid fitler [default: %default].");
  parser.add_option("--beta").action("store").type("double").set_default(10.0).help("Value of beta for sigmoid fitler [default: %default].");

  Command::buildParser();
}

bool TopologyPreservingFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double scaling = static_cast<double>(options.get("scaling"));
  double alpha = static_cast<double>(options.get("alpha"));
  double beta = static_cast<double>(options.get("beta"));

  sharedData.image.topologyPreservingSmooth(scaling, alpha, beta);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Blur
///////////////////////////////////////////////////////////////////////////////
void Blur::buildParser()
{
  const std::string prog = "blur";
  const std::string desc = "applies gaussian blur";
  parser.prog(prog).description(desc);

  parser.add_option("--sigma").action("store").type("double").set_default(0.0).help("Value of sigma [default: %default].");

  Command::buildParser();
}

bool Blur::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double sigma = static_cast<double>(options.get("sigma"));

  sharedData.image.gaussianBlur(sigma);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ICPRigid
///////////////////////////////////////////////////////////////////////////////
void ICPRigid::buildParser()
{
  const std::string prog = "icp-image";
  const std::string desc = "transform current image using iterative closest point (ICP) 3D rigid registration computed from current distance map to target distance map";
  parser.prog(prog).description(desc);

  parser.add_option("--target").action("store").type("string").set_default("").help("Distance map of target image.");
  parser.add_option("--isovalue").action("store").type("double").set_default(0.0).help("Isovalue of distance maps used to create ICPtransform [default: %default].");
  parser.add_option("--iterations").action("store").type("unsigned").set_default(20).help("Number of iterations run ICP registration [default: %default].");
  parser.add_option("--centerofmass").action("store_true").set_default(false).help("Use center of mass rather than ICP [default: false].");

  Command::buildParser();
}

bool ICPRigid::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  std::string targetDT = static_cast<std::string>(options.get("target"));
  double isoValue = static_cast<double>(options.get("isovalue"));
  unsigned iterations = static_cast<unsigned>(options.get("iterations"));

  if (targetDT == "")
  {
    std::cerr << "Must specify a target distance map\n";
    return false;
  }
  else
  {
    auto type = static_cast<bool>(options.get("centerofmass")) ?
                                  XFormType::CenterOfMass : XFormType::IterativeClosestPoint;

    Image target(targetDT);
    auto xform = sharedData.image.createTransform(target, type, isoValue, iterations);
    TransformPtr transform(xform);
    sharedData.image.applyTransform(transform, target.origin(), target.dims(), target.spacing(), target.coordsys());
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// BoundingBoxImage
///////////////////////////////////////////////////////////////////////////////
void BoundingBoxImage::buildParser()
{
  const std::string prog = "bounding-box-image";
  const std::string desc = "compute largest physical bounding box surrounding the specified isovalue of the specified set of images";
  parser.prog(prog).description(desc);

  parser.add_option("--names").action("store").type("multistring").set_default("").help("Paths to images (must be followed by `--`), ex: \"bounding-box-image --names *.nrrd -- --isovalue 1.5\")");
  parser.add_option("--isovalue").action("store").type("double").set_default(1.0).help("Threshold value [default: %default].");

  Command::buildParser();
}

bool BoundingBoxImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::vector<std::string> filenames = options.get("names");
  double isoValue = static_cast<double>(options.get("isovalue"));

  sharedData.region = ImageUtils::boundingBox(filenames, isoValue);
  std::cout << "Bounding box:\n" << sharedData.region << std::endl;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ImageBounds
///////////////////////////////////////////////////////////////////////////////
void ImageBounds::buildParser()
{
  const std::string prog = "image-bounds";
  const std::string desc = "return bounds of image, optionally with an isovalue to restrict region";
  parser.prog(prog).description(desc);

  parser.add_option("--isovalue").action("store").type("double").help("Isovalue [default: entire image].");

  Command::buildParser();
}

bool ImageBounds::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  auto isovalue = options.get("isovalue");

  if (isovalue.isValid())
  {
    sharedData.region = sharedData.image.physicalBoundingBox(static_cast<double>(isovalue));
  }
  else
  {
    sharedData.region = sharedData.image.physicalBoundingBox();
  }

  std::cout << "Bounding box:\n" << sharedData.region << std::endl;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// SetRegion
///////////////////////////////////////////////////////////////////////////////
void SetRegion::buildParser()
{
  const std::string prog = "set-region";
  const std::string desc = "set the current (physical) region to the specified min/max in each direction, for use with downstreams commands such as crop (note: could instead use the image-bounds command with an isovalue)";
  parser.prog(prog).description(desc);

  parser.add_option("--xmin").action("store").type("double").help("Minimum X.");
  parser.add_option("--xmax").action("store").type("double").help("Maximum X.");
  parser.add_option("--ymin").action("store").type("double").help("Minimum Y.");
  parser.add_option("--ymax").action("store").type("double").help("Maximum Y.");
  parser.add_option("--zmin").action("store").type("double").help("Minimum Z.");
  parser.add_option("--zmax").action("store").type("double").help("Maximum Z.");

  Command::buildParser();
}

bool SetRegion::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  double xmin = static_cast<double>(options.get("xmin"));
  double ymin = static_cast<double>(options.get("ymin"));
  double zmin = static_cast<double>(options.get("zmin"));
  double xmax = static_cast<double>(options.get("xmax"));
  double ymax = static_cast<double>(options.get("ymax"));
  double zmax = static_cast<double>(options.get("zmax"));

  sharedData.region = PhysicalRegion(Point({xmin, ymin, zmin}),
                                     Point({xmax, ymax, zmax}));
  std::cout << "region: " << sharedData.region << std::endl;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// CropImage
///////////////////////////////////////////////////////////////////////////////
void CropImage::buildParser()
{
  const std::string prog = "crop";
  const std::string desc = "crop image down to the current region of physical space (from bounding-box or set-region commands)";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool CropImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  sharedData.image.crop(sharedData.region);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ClipImage
///////////////////////////////////////////////////////////////////////////////
void ClipImage::buildParser()
{
  const std::string prog = "clip-image";
  const std::string desc = "clips volume with the specified cutting planes defined by three 3D points";
  parser.prog(prog).description(desc);

  parser.add_option("--x1").action("store").type("double").set_default(0.0).help("Value of x1 for cutting plane [default: %default].");
  parser.add_option("--y1").action("store").type("double").set_default(0.0).help("Value of y1 for cutting plane [default: %default].");
  parser.add_option("--z1").action("store").type("double").set_default(0.0).help("Value of z1 for cutting plane [default: %default].");
  parser.add_option("--x2").action("store").type("double").set_default(0.0).help("Value of x2 for cutting plane [default: %default].");
  parser.add_option("--y2").action("store").type("double").set_default(0.0).help("Value of y2 for cutting plane [default: %default].");
  parser.add_option("--z2").action("store").type("double").set_default(0.0).help("Value of z2 for cutting plane [default: %default].");
  parser.add_option("--x3").action("store").type("double").set_default(0.0).help("Value of x3 for cutting plane [default: %default].");
  parser.add_option("--y3").action("store").type("double").set_default(0.0).help("Value of y3 for cutting plane [default: %default].");
  parser.add_option("--z3").action("store").type("double").set_default(0.0).help("Value of z3 for cutting plane [default: %default].");
  parser.add_option("--value").action("store").type("double").set_default(0.0).help("Value of clipped pixels [default: %default].");

  Command::buildParser();
}

bool ClipImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  Point p1({static_cast<double>(options.get("x1")), static_cast<double>(options.get("x2")), static_cast<double>(options.get("x3"))});
  Point p2({static_cast<double>(options.get("y1")), static_cast<double>(options.get("y2")), static_cast<double>(options.get("y3"))});
  Point p3({static_cast<double>(options.get("z1")), static_cast<double>(options.get("z2")), static_cast<double>(options.get("z3"))});

  sharedData.image.clip(makePlane(p1, p2, p3), static_cast<double>(options.get("value")));
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ReflectImage
///////////////////////////////////////////////////////////////////////////////
void ReflectImage::buildParser()
{
  const std::string prog = "reflect-image";
  const std::string desc = "reflect image with respect to logical image center and the specified axis";
  parser.prog(prog).description(desc);

  parser.add_option("--axis").action("store").type("string").set_default("").help("Axis along which to reflect (X, Y, or Z).");

  Command::buildParser();
}

bool ReflectImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  std::string axis_str(static_cast<std::string>(options.get("axis")));
  Axis axis(toAxis(axis_str));

  sharedData.image.reflect(axis);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// SetOrigin
///////////////////////////////////////////////////////////////////////////////
void SetOrigin::buildParser()
{
  const std::string prog = "set-origin";
  const std::string desc = "set origin";
  parser.prog(prog).description(desc);

  parser.add_option("--x", "-x").action("store").type("double").set_default(0).help("X value of origin [default: %default].");
  parser.add_option("--y", "-y").action("store").type("double").set_default(0).help("Y value of origin [default: %default].");
  parser.add_option("--z", "-z").action("store").type("double").set_default(0).help("Z value of origin [default: %default].");

  Command::buildParser();
}

bool SetOrigin::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double x = static_cast<double>(options.get("x"));
  double y = static_cast<double>(options.get("y"));
  double z = static_cast<double>(options.get("z"));

  sharedData.image.setOrigin(Point3({x, y, z}));
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// SetSpacing
///////////////////////////////////////////////////////////////////////////////
void SetSpacing::buildParser()
{
  const std::string prog = "set-spacing";
  const std::string desc = "set spacing";
  parser.prog(prog).description(desc);

  parser.add_option("--x", "-x").action("store").type("double").set_default(1.0).help("x value of spacing [default: %default].");
  parser.add_option("--y", "-y").action("store").type("double").set_default(1.0).help("y value of spacing [default: %default].");
  parser.add_option("--z", "-z").action("store").type("double").set_default(1.0).help("z value of spacing [default: %default].");

  Command::buildParser();
}

bool SetSpacing::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double x = static_cast<double>(options.get("x"));
  double y = static_cast<double>(options.get("y"));
  double z = static_cast<double>(options.get("z"));

  sharedData.image.setSpacing(makeVector({x, y, z}));
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Warp Image
///////////////////////////////////////////////////////////////////////////////
void WarpImage::buildParser()
{
  const std::string prog = "warp-image";
  const std::string desc = "finds the warp between the source and target landmarks and transforms image by this warp";
  parser.prog(prog).description(desc);

  parser.add_option("--source").action("store").type("string").set_default("").help("Path to source landmarks.");
  parser.add_option("--target").action("store").type("string").set_default("").help("Path to target landmarks.");
  parser.add_option("--stride").action("store").type("int").set_default(1).help("Every _stride_ points will be used for warping [default: %default].");
  Command::buildParser();
}

bool WarpImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  std::string source = static_cast<std::string>(options.get("source"));
  std::string target = static_cast<std::string>(options.get("target"));
  int stride = static_cast<int>(options.get("stride"));

  if (source == "" || target == "")
  {
    std::cerr << "Must specify both source and target landmarks\n";
    return false;
  }

  TransformPtr transform(ImageUtils::createWarpTransform(source, target, stride));
  sharedData.image.applyTransform(transform);

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// CompareImage
///////////////////////////////////////////////////////////////////////////////
void CompareImage::buildParser()
{
  const std::string prog = "compare-image";
  const std::string desc = "compare two images";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Compare this image with another.");
  parser.add_option("--verifyall").action("store").type("bool").set_default(true).help("Also verify origin, spacing, and direction matches [default: true].");
  parser.add_option("--tolerance").action("store").type("double").set_default(0.0).help("Allowed percentage of pixel differences [default: %default].");
  parser.add_option("--precision").action("store").type("double").set_default(1e-12).help("Allowed difference between two pixels for them to still be considered equal [default: 1e-12].");

  Command::buildParser();
}

bool CompareImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  std::string filename = options["name"];
  if (filename.length() == 0) {
    std::cerr << "compareimage error: no filename specified with which to compare, must pass `--name <filename>`\n";
    return false;
  }

  bool verifyAll = static_cast<bool>(options.get("verifyall"));
  double tolerance = static_cast<double>(options.get("tolerance"));
  double precision = static_cast<double>(options.get("precision"));

  if (tolerance > 100 || tolerance < 0)
  {
    std::cerr << "tolerance value must be between 0 and 100 (inclusive)";
    return false;
  }

  if (sharedData.image.compare(Image(filename), verifyAll, tolerance/100, precision))
  {
    std::cout << "compare success\n";
    return true;
  }
  else
  {
    std::cout << "compare failure\n";
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// NegateImage
///////////////////////////////////////////////////////////////////////////////
void NegateImage::buildParser()
{
  const std::string prog = "negate";
  const std::string desc = "negate the values in the given image";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool NegateImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  sharedData.image = -sharedData.image;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// AddImage
///////////////////////////////////////////////////////////////////////////////
void AddImage::buildParser()
{
  const std::string prog = "add";
  const std::string desc = "add a value to each pixel in the given image and/or add another image in a pixelwise manner";
  parser.prog(prog).description(desc);

  parser.add_option("--value", "-x").action("store").type("double").set_default("0.0").help("Value to add to each pixel [default: %default].");
  parser.add_option("--name").action("store").type("string").set_default("").help("Name of image to add pixelwise.");

  Command::buildParser();
}

bool AddImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double value = static_cast<double>(options.get("value"));
  std::string filename = options["name"];

  if (filename.length() == 0 && value == 0.0) {
    std::cerr << "add error: no filename or value specified. Must pass value or name.\n";
    return false;
  }

  if (value != 0.0)
    sharedData.image += value;

  if (filename.length() != 0)
    sharedData.image += Image(filename);

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// SubtractImage
///////////////////////////////////////////////////////////////////////////////
void SubtractImage::buildParser()
{
  const std::string prog = "subtract";
  const std::string desc = "subtract a value from each pixel in this image and/or subtract another image in a pixelwise manner";
  parser.prog(prog).description(desc);

  parser.add_option("--value", "-x").action("store").type("double").set_default("0.0").help("Value to subtract from each pixel [default: %default].");
  parser.add_option("--name").action("store").type("string").set_default("").help("Name of image to subtract pixelwise.");

  Command::buildParser();
}

bool SubtractImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double value = static_cast<double>(options.get("value"));
  std::string filename = options["name"];

  if (filename.length() == 0 && value == 0.0) {
    std::cerr << "sub error: no filename or value specified. Must pass value or name.\n";
    return false;
  }

  if (value != 0.0)
    sharedData.image -= value;

  if (filename.length() != 0)
    sharedData.image -= Image(filename);

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// MultiplyImage
///////////////////////////////////////////////////////////////////////////////
void MultiplyImage::buildParser()
{
  const std::string prog = "multiply";
  const std::string desc = "multiply an image by a constant";
  parser.prog(prog).description(desc);

  parser.add_option("--value", "-x").action("store").type("double").set_default("1.0").help("Value with which to multiply [default: %default]");

  Command::buildParser();
}

bool MultiplyImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double value = static_cast<double>(options.get("value"));
  if (value != 0.0)
    sharedData.image *= value;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// DivideImage
///////////////////////////////////////////////////////////////////////////////
void DivideImage::buildParser()
{
  const std::string prog = "divide";
  const std::string desc = "divide an image by a constant";
  parser.prog(prog).description(desc);

  parser.add_option("--value", "-x").action("store").type("double").set_default("0.0").help("Value with which to divide [default: %default].");

  Command::buildParser();
}

bool DivideImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double value = static_cast<double>(options.get("value"));
  if (value != 0.0)
    sharedData.image /= value;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ImageToMesh
///////////////////////////////////////////////////////////////////////////////
void ImageToMesh::buildParser()
{
  const std::string prog = "image-to-mesh";
  const std::string desc = "converts the current image to a mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--isovalue", "-v").action("store").type("double").set_default(1.0).help("Isovalue to determine mesh boundary [default: %default].");

  Command::buildParser();
}

bool ImageToMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double isoValue = static_cast<double>(options.get("isovalue"));

  sharedData.mesh = std::make_unique<Mesh>(sharedData.image.toMesh(isoValue));
  return sharedData.validMesh();
}

} // shapeworks

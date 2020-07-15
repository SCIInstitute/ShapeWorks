#include "Commands.h"
#include "Image.h"
#include "ImageUtils.h"
#include "ShapeEvaluation.h"
#include <limits>

namespace shapeworks {

// boilerplate for a command. Copy this to start a new command
// Next, be sure to add the command to shapeworks.cpp!
#if 0
///////////////////////////////////////////////////////////////////////////////
// Example
///////////////////////////////////////////////////////////////////////////////
void Example::buildParser()
{
  const std::string prog = "example";
  const std::string desc = "brief description of command";
  parser.prog(prog).description(desc);

  parser.add_option("--optionName").action("store").type("double").set_default(0.01).help("Description of optionName.");
  //additional options... 
  
  Command::buildParser();
}

bool Example::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  double optionName = static_cast<double>(options.get("optionName"));
  //read additional options... 

  return sharedData.image.example(optionName, ...);
}
#endif

///////////////////////////////////////////////////////////////////////////////
// ReadImage
///////////////////////////////////////////////////////////////////////////////
void ReadImage::buildParser()
{
  const std::string prog = "read-image";
  const std::string desc = "reads an image";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of file to read");

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

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of file to write");
  parser.add_option("--compressed").action("store").type("bool").set_default(true).help("Whether to compress file [default: true]");

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
  const std::string prog = "info";
  const std::string desc = "prints requested image dimensions, spacing, size, origin, direction (coordinate system), center, center of mass and bounding box [default: prints everything]";
  parser.prog(prog).description(desc);

  parser.add_option("--dims").action("store").type("bool").set_default(false).help("Whether to display image dimensions");
  parser.add_option("--spacing").action("store").type("bool").set_default(false).help("Whether to display physical spacing");
  parser.add_option("--size").action("store").type("bool").set_default(false).help("Whether to display size");
  parser.add_option("--origin").action("store").type("bool").set_default(false).help("Whether to display physical origin");
  parser.add_option("--direction").action("store").type("bool").set_default(false).help("Whether to display direction");
  parser.add_option("--center").action("store").type("bool").set_default(false).help("Whether to display center");
  parser.add_option("--centerofmass").action("store").type("bool").set_default(false).help("Whether to display center of mass");
  parser.add_option("--boundingbox").action("store").type("bool").set_default(false).help("Whether to display bounding box");

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
  bool centerofmass = static_cast<bool>(options.get("centerofmass"));
  bool boundingbox = static_cast<bool>(options.get("boundingbox"));

  // by default: print everything
  if (options.num_set() == 0)
    dims = spacing = size = origin = direction = center = centerofmass = boundingbox = true;

  if (dims)
    std::cout << "image dimensions:      " << sharedData.image.dims() << std::endl;
  if (spacing)
    std::cout << "physical spacing:      " << sharedData.image.spacing() << std::endl;
  if (size)
    std::cout << "size (spacing * dims): " << sharedData.image.size() << std::endl;
  if (origin)
    std::cout << "physical origin:       " << sharedData.image.origin() << std::endl;
  if (direction)
    std::cout << "direction (coordsys):  " << std::endl
              << sharedData.image.coordsys() << std::endl;
  if (center)
    std::cout << "center:                " << sharedData.image.center() << std::endl;
  if (centerofmass)
    std::cout << "center of mass (0,1]:  " << sharedData.image.centerOfMass() << std::endl;
  if (boundingbox)
    std::cout << "bounding box:          " << sharedData.image.boundingBox() << std::endl;
  
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

  parser.add_option("--maxrmserror").action("store").type("double").set_default(0.01).help("Maximum RMS error determines how fast the solver converges. Range [0.0, 1.0], larger is faster [default: 0.01].");
  parser.add_option("--iterations").action("store").type("int").set_default(50).help("Maximum number of iterations [default: 50].");
  parser.add_option("--layers").action("store").type("int").set_default(0).help("Number of layers around a 3d pixel to use for this computation [default: 3].");

  Command::buildParser();
}

bool Antialias::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double maxRMSErr = static_cast<double>(options.get("maxrmserror"));
  int iterations = static_cast<int>(options.get("iterations"));
  int layers = static_cast<int>(options.get("layers"));

  if (layers < 0)
  {
    std::cerr << "layers must be >= 0\n";
    return false;
  }
  else
  {
    sharedData.image.antialias(iterations, maxRMSErr, layers);
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// ResampleImage
///////////////////////////////////////////////////////////////////////////////
void ResampleImage::buildParser()
{
  const std::string prog = "resample";
  const std::string desc = "resamples an image";
  parser.prog(prog).description(desc);

  parser.add_option("--isospacing").action("store").type("double").set_default(0.0f).help("Use this spacing in all dimensions.");
  parser.add_option("--spacex").action("store").type("double").set_default(1.0f).help("Pixel spacing in x-direction [default: 1.0].");
  parser.add_option("--spacey").action("store").type("double").set_default(1.0f).help("Pixel spacing in y-direction [default: 1.0].");
  parser.add_option("--spacez").action("store").type("double").set_default(1.0f).help("Pixel spacing in z-direction [default: 1.0].");
  parser.add_option("--sizex").action("store").type("unsigned").set_default(0).help("Output size in x-direction [default: calculated using current size and desired spacing].");
  parser.add_option("--sizey").action("store").type("unsigned").set_default(0).help("Output size in y-direction [default: calculated using current size and desired spacing].");
  parser.add_option("--sizez").action("store").type("unsigned").set_default(0).help("Output size in z-direction [default: calculated using current size and desired spacing].");

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

  if (isoSpacing > 0.0)
    ImageUtils::isoresample(sharedData.image, isoSpacing, Dims({sizeX, sizeY, sizeZ}));
  else
    sharedData.image.resample(Point3({spaceX, spaceY, spaceZ}), Dims({sizeX, sizeY, sizeZ}));
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

  parser.add_option("--padding").action("store").type("int").set_default(0).help("Pad this many voxels in ALL directions (used if set) [default: 0].");
  parser.add_option("--padx","-x").action("store").type("int").set_default(0).help("Pad this many voxels in the x-direction [default: 0].");
  parser.add_option("--pady","-y").action("store").type("int").set_default(0).help("Pad this many voxels in the y-direction [default: 0].");
  parser.add_option("--padz","-z").action("store").type("int").set_default(0).help("Pad this many voxels in the z-direction [default: 0].");
  parser.add_option("--value").action("store").type("double").set_default(0.0).help("Value used to fill padded voxels [default: 0.0].");

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
// Translate
///////////////////////////////////////////////////////////////////////////////
void Translate::buildParser()
{
  const std::string prog = "translate";
  const std::string desc = "translates image by specified physical (image space) distance";
  parser.prog(prog).description(desc);

  parser.add_option("--centerofmass").action("store").type("bool").set_default(false).help("Use center of mass [default: false].");
  parser.add_option("--tx", "-x").action("store").type("double").set_default(0).help("X distance");
  parser.add_option("--ty", "-y").action("store").type("double").set_default(0).help("Y distance");
  parser.add_option("--tz", "-z").action("store").type("double").set_default(0).help("Z distance");

  Command::buildParser();
}

bool Translate::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  bool centerofmass = static_cast<bool>(options.get("centerofmass"));

  if (centerofmass)
  {
    sharedData.image.applyTransform(ImageUtils::createCenterOfMassTransform(sharedData.image));
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
// Scale
///////////////////////////////////////////////////////////////////////////////
void Scale::buildParser()
{
  const std::string prog = "scale";
  const std::string desc = "scales image by specified value";
  parser.prog(prog).description(desc);

  parser.add_option("--sx", "-x").action("store").type("double").set_default(1.0).help("X scale");
  parser.add_option("--sy", "-y").action("store").type("double").set_default(1.0).help("Y scale");
  parser.add_option("--sz", "-z").action("store").type("double").set_default(1.0).help("Z scale");

  Command::buildParser();
}

bool Scale::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double sx = static_cast<double>(options.get("sx"));
  double sy = static_cast<double>(options.get("sy"));
  double sz = static_cast<double>(options.get("sz"));

  if (sx == 0 || sy == 0 || sz == 0)
  {
    std::cerr << "Error: cannot scale by 0 in any dimension\n";
    return false;
  }
  else
  {
    sharedData.image.scale(makeVector({sx, sy, sz}));
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Rotate
///////////////////////////////////////////////////////////////////////////////
void Rotate::buildParser()
{
  const std::string prog = "rotate";
  const std::string desc = "rotates image by specified value";
  parser.prog(prog).description(desc);

  parser.add_option("--rx", "-x").action("store").type("double").help("Physical axis around which to rotate [default: z-axis]");
  parser.add_option("--ry", "-y").action("store").type("double").help("Physical axis around which to rotate [default: z-axis]");
  parser.add_option("--rz", "-z").action("store").type("double").set_default(1.0).help("Physical axis around which to rotate [default: z-axis]");
  parser.add_option("--radians").action("store").type("double").help("Angle in radians");
  parser.add_option("--degrees").action("store").type("double").help("Angle in degrees");

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
  if (!axis_is_valid(axis))
  {
    std::cerr << "Must specify a valid axis\n";
    return false;
  }
  else if (radians == 0.0 && degrees == 0.0)
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

  parser.add_option("--label").action("store").type("double").set_default(1.0).help("Label value to be extracted [default: 1.0].");

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

  parser.add_option("--value").action("store").type("double").set_default(0.0).help("Largest value not in volume [default: 0.0].");

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
  parser.add_option("--value").action("store").type("double").set_default(1.0).help("Value to set region [default: 1.0].");
  
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

  parser.add_option("--isovalue").action("store").type("double").set_default(0.0).help("Level set value that defines the interface between foreground and background [default: 0.0].");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool ComputeDT::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  double isovalue = static_cast<double>(options.get("isovalue"));

  sharedData.image.computeDT(isovalue);
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

  parser.add_option("--iterations").action("store").type("int").set_default(10).help("Number of iterations [default: 10].");

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

  parser.add_option("--alpha").action("store").type("double").set_default(10.0).help("Value of alpha [default: 10.0].");
  parser.add_option("--beta").action("store").type("double").set_default(10.0).help("Value of beta [default: 10.0].");

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
  parser.add_option("--scaling").action("store").type("double").set_default(20.0).help("Value of scale [default: 20.0].");
  
  Command::buildParser();
}

bool TPLevelSetFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  std::string featureimage = static_cast<std::string>(options.get("featureimage"));

  if (featureimage == "")
  {
    std::cerr << "Must specify a feature image\n";
    return false;
  }
  else
  {
    Image featureImage(featureimage);
    double scaling = static_cast<double>(options.get("scaling"));

    sharedData.image.applyTPLevelSetFilter(featureImage, scaling);
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// TopologyPreservingFilter
///////////////////////////////////////////////////////////////////////////////
void TopologyPreservingFilter::buildParser()
{
  const std::string prog = "topo-preserving-smooth";
  const std::string desc = "Helper command that applies gradient and sigmoid filters to create a feature image for the TPLevelSet filter; note that a curvature flow filter is sometimes applied to the image before this.";
  parser.prog(prog).description(desc);

  parser.add_option("--scaling").action("store").type("double").set_default(20.0).help("Scale for TPLevelSet level set filter [default: 20.0].");
  parser.add_option("--alpha").action("store").type("double").set_default(10.0).help("Value of alpha for sigmoid fitler [default: 10.0].");
  parser.add_option("--beta").action("store").type("double").set_default(10.0).help("Value of beta for sigmoid fitler [default: 10.0.0].");

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

  ImageUtils::topologyPreservingSmooth(sharedData.image, scaling, alpha, beta);
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

  parser.add_option("--sigma").action("store").type("double").set_default(0.0).help("Value of sigma [default: 0.0].");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
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
  const std::string prog = "icp";
  const std::string desc = "transform current image using iterative closest point (ICP) 3D rigid registration computed from source to target distance maps";
  parser.prog(prog).description(desc);

  parser.add_option("--source").action("store").type("string").set_default("").help("Distance map of source image.");
  parser.add_option("--target").action("store").type("string").set_default("").help("Distance map of target image.");
  parser.add_option("--isovalue").action("store").type("double").set_default(0.0).help("isovalue of distance maps used to create ICPtransform [default: 0.0].");
  parser.add_option("--iterations").action("store").type("unsigned").set_default(20).help("Number of iterations run ICP registration [default: 20].");

  Command::buildParser();
}

bool ICPRigid::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  std::string sourceDT = static_cast<std::string>(options.get("source"));
  std::string targetDT = static_cast<std::string>(options.get("target"));
  double isovalue = static_cast<double>(options.get("isovalue"));
  unsigned iterations = static_cast<unsigned>(options.get("iterations"));

  if (sourceDT == "")
  {
    std::cerr << "Must specify a source distance map\n";
    return false;
  }
  else if (targetDT == "")
  {
    std::cerr << "Must specify a target distance map\n";
    return false;
  }
  else
  {
    Image source_dt(sourceDT);
    Image target_dt(targetDT);
    TransformPtr transform(ImageUtils::createRigidRegistrationTransform(source_dt, target_dt, isovalue, iterations));
    sharedData.image.applyTransform(transform, target_dt.dims(), target_dt.origin(), target_dt.spacing(), target_dt.coordsys());
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// BoundingBox
///////////////////////////////////////////////////////////////////////////////
void BoundingBox::buildParser()
{
  const std::string prog = "bounding-box";
  const std::string desc = "compute largest bounding box surrounding the specified isovalue of the specified set of binary images";
  parser.prog(prog).description(desc);

  parser.add_option("--names").action("store").type("multistring").set_default("").help("Paths to images (must be followed by `--`), ex: \"bounding-box --names *.nrrd -- --isovalue 1.5\")");
  parser.add_option("--padding").action("store").type("int").set_default(0).help("Number of extra voxels in each direction to pad the largest bounding box [default: 0].");
  parser.add_option("--isovalue").action("store").type("double").set_default(1.0).help("Threshold value [default: 1.0].");

  Command::buildParser();
}

bool BoundingBox::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::vector<std::string> filenames = options.get("names");
  int padding = static_cast<int>(options.get("padding"));
  double isovalue = static_cast<double>(options.get("isovalue"));

  sharedData.region = ImageUtils::boundingBox(filenames, isovalue);
  sharedData.region.pad(padding);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// CropImage
///////////////////////////////////////////////////////////////////////////////
void CropImage::buildParser()
{
  const std::string prog = "crop";
  const std::string desc = "crop image down to the current region (e.g., from bounding-box command), or the specified min/max in each direction [default: image dimensions]";
  parser.prog(prog).description(desc);

  parser.add_option("--xmin").action("store").type("unsigned").set_default(0).help("Minimum X.");
  parser.add_option("--xmax").action("store").type("unsigned").set_default(0).help("Maximum X.");
  parser.add_option("--ymin").action("store").type("unsigned").set_default(0).help("Minimum Y.");
  parser.add_option("--ymax").action("store").type("unsigned").set_default(0).help("Maximum Y.");
  parser.add_option("--zmin").action("store").type("unsigned").set_default(0).help("Minimum Z.");
  parser.add_option("--zmax").action("store").type("unsigned").set_default(0).help("Maximum Z.");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool CropImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  unsigned xmin = static_cast<unsigned>(options.get("xmin"));
  unsigned ymin = static_cast<unsigned>(options.get("ymin"));
  unsigned zmin = static_cast<unsigned>(options.get("zmin"));
  unsigned xmax = static_cast<unsigned>(options.get("xmax"));
  unsigned ymax = static_cast<unsigned>(options.get("ymax"));
  unsigned zmax = static_cast<unsigned>(options.get("zmax"));

  if (xmin == 0 && ymin == 0 && zmin == 0 &&
      xmax == 0 && ymax == 0 && zmax == 0)
    sharedData.image.crop(sharedData.region);
  else
  {
    Image::Region region(sharedData.image.dims());
    if (xmin < xmax) { region.min[0] = xmin; region.max[0] = xmax; }
    if (ymin < ymax) { region.min[0] = ymin; region.max[0] = ymax; }
    if (zmin < zmax) { region.min[0] = zmin; region.max[0] = zmax; }
    sharedData.image.crop(region);
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ClipVolume
///////////////////////////////////////////////////////////////////////////////
void ClipVolume::buildParser()
{
  const std::string prog = "clip";
  const std::string desc = "clips volume with the specified cutting planes defined by three 3D points";
  parser.prog(prog).description(desc);

  parser.add_option("--x1").action("store").type("double").set_default(0.0).help("Value of x1 for cutting plane [default: 0.0].");
  parser.add_option("--y1").action("store").type("double").set_default(0.0).help("Value of y1 for cutting plane [default: 0.0].");
  parser.add_option("--z1").action("store").type("double").set_default(0.0).help("Value of z1 for cutting plane [default: 0.0].");
  parser.add_option("--x2").action("store").type("double").set_default(0.0).help("Value of x2 for cutting plane [default: 0.0].");
  parser.add_option("--y2").action("store").type("double").set_default(0.0).help("Value of y2 for cutting plane [default: 0.0].");
  parser.add_option("--z2").action("store").type("double").set_default(0.0).help("Value of z2 for cutting plane [default: 0.0].");
  parser.add_option("--x3").action("store").type("double").set_default(0.0).help("Value of x3 for cutting plane [default: 0.0].");
  parser.add_option("--y3").action("store").type("double").set_default(0.0).help("Value of y3 for cutting plane [default: 0.0].");
  parser.add_option("--z3").action("store").type("double").set_default(0.0).help("Value of z3 for cutting plane [default: 0.0].");
  parser.add_option("--value").action("store").type("double").set_default(0.0).help("Value of clipped pixels [default: 0.0].");

  Command::buildParser();
}

bool ClipVolume::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  Point p1({static_cast<double>(options.get("x1")), static_cast<double>(options.get("x2")), static_cast<double>(options.get("x3"))});
  Point p2({static_cast<double>(options.get("y1")), static_cast<double>(options.get("y2")), static_cast<double>(options.get("y3"))});
  Point p3({static_cast<double>(options.get("z1")), static_cast<double>(options.get("z2")), static_cast<double>(options.get("z3"))});

  sharedData.image.clip(p1, p2, p3, static_cast<double>(options.get("value")));
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ReflectVolume
///////////////////////////////////////////////////////////////////////////////
void ReflectVolume::buildParser()
{
  const std::string prog = "reflect";
  const std::string desc = "reflect image with respect to logical image center and the specified axis";
  parser.prog(prog).description(desc);

  parser.add_option("--axis").action("store").type("string").set_default("").help("Axis along which to reflect (X, Y, or Z).");

  Command::buildParser();
}

bool ReflectVolume::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  std::string axis_str(static_cast<std::string>(options.get("axis")));
  Axis axis(toAxis(axis_str));
  if (!axis_is_valid(axis))
  {
    std::cerr << "Must specify a valid axis (X, Y, or Z)\n";
    return false;
  }
  else
  {
    sharedData.image.reflect(axis);
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// SetOrigin
///////////////////////////////////////////////////////////////////////////////
void SetOrigin::buildParser()
{
  const std::string prog = "set-origin";
  const std::string desc = "set origin";
  parser.prog(prog).description(desc);

  parser.add_option("--x", "-x").action("store").type("double").set_default(0).help("x value of origin [default: 0.0].");
  parser.add_option("--y", "-y").action("store").type("double").set_default(0).help("y value of origin [default: 0.0].");
  parser.add_option("--z", "-z").action("store").type("double").set_default(0).help("z value of origin [default: 0.0].");

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
// Warp Image
///////////////////////////////////////////////////////////////////////////////
void WarpImage::buildParser()
{
  const std::string prog = "warp-image";
  const std::string desc = "Finds the warp between the source and target landmarks and transforms image by this warp.";
  parser.prog(prog).description(desc);

  parser.add_option("--source").action("store").type("string").set_default("").help("Path to source landmarks.");
  parser.add_option("--target").action("store").type("string").set_default("").help("Path to target landmarks.");
  parser.add_option("--stride").action("store").type("int").set_default(1).help("Every _stride_ points will be used for warping [default: 1].");
  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
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
// Compare
///////////////////////////////////////////////////////////////////////////////
void Compare::buildParser()
{
  const std::string prog = "compare";
  const std::string desc = "compare two images";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Compare this image with another");
  parser.add_option("--verifyall").action("store").type("bool").set_default(true).help("Verify origin, spacing, and direction of both images match [default: true]");
  parser.add_option("--tolerance").action("store").type("double").set_default(0.0).help("Allowed percentage of pixel differences [default: 0.0]");
  parser.add_option("--precision").action("store").type("double").set_default(1e-12).help("Allowed difference between two pixels for them to still be considered equal [default: 0.0]");

  Command::buildParser();
}

bool Compare::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validImage())
  {
    std::cerr << "No image to operate on\n";
    return false;
  }

  std::string filename = options["name"];
  if (filename.length() == 0) {
    std::cerr << "compare error: no filename specified with which to compare, must pass `--name <filename>`\n";
    return false;
  }

  bool verifyall = static_cast<bool>(options.get("verifyall"));
  double tolerance = static_cast<double>(options.get("tolerance"));
  double precision = static_cast<double>(options.get("precision"));

  if (tolerance > 100 || tolerance < 0)
  {
    std::cerr << "tolerance value must be between 0 and 100 (inclusive)";
    return false;
  }

  if (sharedData.image.compare(Image(filename), verifyall, tolerance/100, precision))
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
  const std::string desc = "negate the values in this image";
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
  const std::string desc = "add a value to each pixel in this image and/or add another image in a pixelwise manner";
  parser.prog(prog).description(desc);

  parser.add_option("--value", "-x").action("store").type("double").set_default("0.0").help("Value to add to each pixel");
  parser.add_option("--name").action("store").type("string").set_default("").help("Name of image to add pixelwise");

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

  parser.add_option("--value", "-x").action("store").type("double").set_default("0.0").help("Value to subtract from each pixel");
  parser.add_option("--name").action("store").type("string").set_default("").help("Name of image to subtract pixelwise");

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

  parser.add_option("--value", "-x").action("store").type("double").set_default("0.0").help("Value with which to multiply");

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

  parser.add_option("--value", "-x").action("store").type("double").set_default("0.0").help("Value with which to divide");

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
// ReadParticleSystem
///////////////////////////////////////////////////////////////////////////////
void ReadParticleSystem::buildParser()
{
  const std::string prog = "read-particle-system";
  const std::string desc = "reads a particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--names").action("store").type("multistring").set_default("").help("paths to .particle files (must be followed by `--`), ex: \"--names *.particle -- next-command...\")");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool ReadParticleSystem::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::vector<std::string> filenames = options.get("names");
  try {
    sharedData.particleSystem = ParticleSystem(filenames);
    return true;
  } catch(...) {
    std::cerr << "exception while reading particle system" << std::endl;
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Compactness
///////////////////////////////////////////////////////////////////////////////
void Compactness::buildParser()
{
  const std::string prog = "compactness";
  const std::string desc = "Compute compactness of a loaded particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--nmodes").action("store").type("int").set_default("1").help("Number of modes to use");
  parser.add_option("--saveto").action("store").type("string").set_default("").help("Save the scree plots for all modes to a file");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool Compactness::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validParticleSystem())
  {
    std::cerr << "No valid particle system to operate on\n";
    return false;
  }

  const int nModes = static_cast<int>(options.get("nmodes"));
  const std::string saveTo = static_cast<std::string>(options.get("saveto"));
  const double r = ShapeEvaluation<3>::ComputeCompactness(sharedData.particleSystem, nModes, saveTo);
  std::cout << "Particle system compactness: " << r << std::endl;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Generalization
///////////////////////////////////////////////////////////////////////////////
void Generalization::buildParser()
{
  const std::string prog = "generalization";
  const std::string desc = "compute generalization of a loaded particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--nmodes").action("store").type("int").set_default("1").help("Number of modes to use");
  parser.add_option("--saveto").action("store").type("string").set_default("").help("Save the reconstructions sorted by generalization along with the mapping to the original shape");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool Generalization::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validParticleSystem())
  {
    std::cerr << "No valid particle system to operate on\n";
    return false;
  }

  const int nModes = static_cast<int>(options.get("nmodes"));
  const std::string saveTo = static_cast<std::string>(options.get("saveto"));
  const double r = ShapeEvaluation<3>::ComputeGeneralization(sharedData.particleSystem, nModes, saveTo);
  std::cout << "Particle system generalization: " << r << std::endl;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Specificity
///////////////////////////////////////////////////////////////////////////////
void Specificity::buildParser()
{
  const std::string prog = "specificity";
  const std::string desc = "compute specificity of a loaded particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--nmodes").action("store").type("int").set_default("1").help("Number of modes to use");
  parser.add_option("--saveto").action("store").type("string").set_default("").help("Save the reconstructions sorted by specificity along with the mapping to the original shape");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool Specificity::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validParticleSystem())
  {
    std::cerr << "No valid particle system to operate on\n";
    return false;
  }

  const int nModes = static_cast<int>(options.get("nmodes"));
  const std::string saveTo = static_cast<std::string>(options.get("saveto"));
  const double r = ShapeEvaluation<3>::ComputeSpecificity(sharedData.particleSystem, nModes, saveTo);
  std::cout << "Particle system specificity: " << r << std::endl;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ReadMesh
///////////////////////////////////////////////////////////////////////////////
void ReadMesh::buildParser()
{
  const std::string prog = "read-mesh";
  const std::string desc = "reads a mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to read");

  Command::buildParser();
}

bool ReadMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];

  return sharedData.mesh.read(filename);
}

///////////////////////////////////////////////////////////////////////////////
// WriteMesh
///////////////////////////////////////////////////////////////////////////////
void WriteMesh::buildParser()
{
  const std::string prog = "write-mesh";
  const std::string desc = "writes the current mesh (determines type by its extension)";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to write");

  Command::buildParser();
}

bool WriteMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string filename = options["name"];

  return sharedData.mesh.write(filename);
}

///////////////////////////////////////////////////////////////////////////////
// Coverage
///////////////////////////////////////////////////////////////////////////////
void Coverage::buildParser()
{
  const std::string prog = "coverage";
  const std::string desc = "coverage between two meshes";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Second mesh to apply coverage");

  Command::buildParser();
}

bool Coverage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string name = options["name"];

  if (name == "")
  {
    std::cerr << "Must specify second mesh\n";
    return false;
  }

  return sharedData.mesh.coverage(Mesh(name));
}

} // shapeworks

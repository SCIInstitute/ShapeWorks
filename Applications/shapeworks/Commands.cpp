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

  parser.add_option("--optionName").action("store").type("float").set_default(0.01).help("Description of optionName.");
  //additional options... 
  
  Command::buildParser();
}

bool Example::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float optionName = static_cast<float>(options.get("optionName"));
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

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to read");

  Command::buildParser();
}

bool ReadImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];

  try {
    sharedData.image = Image(filename);
    return true;
  } catch(...) {
    std::cerr << "exception while reading " << filename;
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

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to write");
  parser.add_option("--compressed").action("store").type("bool").set_default(true).help("whether to compress file [default is true]");

  Command::buildParser();
}

bool WriteImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];
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
  const std::string desc = "prints logical dims and physical origin, spacing, and coordinate system (direction)";
  parser.prog(prog).description(desc);

  //<ctc> todo: add options for --size, --origin, --spacing, --csys to print only these various bits (will help with the WriteImageInfoToFiles exe)
  // ...make it possible to specify bools directly, cuz ugh!

  Command::buildParser();
}

bool ImageInfo::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];
  bool compressed = static_cast<bool>(options.get("compressed"));
  
  std::cout << "logical dims:          " << sharedData.image.dims() << std::endl;
  std::cout << "physical spacing:      " << sharedData.image.spacing() << std::endl;
  std::cout << "size (spacing * dims): " << sharedData.image.size() << std::endl;
  std::cout << "physical origin:       " << sharedData.image.origin() << std::endl;
  std::cout << "direction (coordsys):  " << std::endl << sharedData.image.coordsys();
  std::cout << "center:                " << sharedData.image.center() << std::endl;
  std::cout << "center of mass (0,1]:  " << sharedData.image.centerOfMass() << std::endl;
  std::cout << "bounding box:          " << sharedData.image.boundingBox();
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

  parser.add_option("--maxrmserror").action("store").type("float").set_default(0.01).help("The maximum RMS error determines how fast the solver converges. Range [0.0, 1.0], larger is faster [default 0.01].");
  parser.add_option("--iterations").action("store").type("int").set_default(50).help("Number of iterations [default 50].");
  parser.add_option("--layers").action("store").type("int").set_default(0).help("Number of layers around a 3d pixel to use for this computation [default image dims].");

  Command::buildParser();
}

bool Antialias::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float maxRMSErr = static_cast<float>(options.get("maxrmserror"));
  int iterations = static_cast<int>(options.get("iterations"));
  int layers = static_cast<int>(options.get("layers"));

  if (layers < 0)
  {
    std::cerr << "Must specify a valid layers argument\n";
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
  parser.add_option("--spacex").action("store").type("double").set_default(1.0f).help("Pixel spacing in x-direction [default 1.0].");
  parser.add_option("--spacey").action("store").type("double").set_default(1.0f).help("Pixel spacing in y-direction [default 1.0].");
  parser.add_option("--spacez").action("store").type("double").set_default(1.0f).help("Pixel spacing in z-direction [default 1.0].");
  parser.add_option("--sizex").action("store").type("unsigned").set_default(0).help("Image size in x-direction [default estimated from the image].");
  parser.add_option("--sizey").action("store").type("unsigned").set_default(0).help("Image size in y-direction [default estimated from the image].");
  parser.add_option("--sizez").action("store").type("unsigned").set_default(0).help("Image size in z-direction [default estimated from the image].");

  Command::buildParser();
}

bool ResampleImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
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
  sharedData.image.recenter();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// PadImage
///////////////////////////////////////////////////////////////////////////////
void PadImage::buildParser()
{
  const std::string prog = "pad";
  const std::string desc = "pads an image with a contant value in the x-, y-, and z- directions";
  parser.prog(prog).description(desc);

  parser.add_option("--padding").action("store").type("int").set_default(0).help("Number of voxels to be padded in each direction.");
  parser.add_option("--value").action("store").type("float").set_default(0.0).help("Value to be used to fill padded voxels.");
  
  Command::buildParser();
}

bool PadImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  int padding = static_cast<int>(options.get("padding"));
  float value = static_cast<float>(options.get("value"));

  sharedData.image.pad(padding, value);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Translate
///////////////////////////////////////////////////////////////////////////////
void Translate::buildParser()
{
  const std::string prog = "translate";
  const std::string desc = "translates images";
  parser.prog(prog).description(desc);

  parser.add_option("--centerofmass").action("store").type("bool").set_default(false).help("Use center of mass [default set to false].");
  parser.add_option("--tx", "-x").action("store").type("double").help("explicit tx in image space (physical coordinates)");
  parser.add_option("--ty", "-y").action("store").type("double").help("explicit ty in image space (e.g., 3.14)");
  parser.add_option("--tz", "-z").action("store").type("double").help("explicit tz in image space");

  Command::buildParser();
}

bool Translate::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
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

    if (tx == 0 || ty == 0 || tz == 0)
    {
      std::cerr << "Must specify a valid translate arguemnt\n";
      return false;
    }
    else
    {
      sharedData.image.translate(makeVector({tx, ty, tz}));
      return true;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Scale
///////////////////////////////////////////////////////////////////////////////
void Scale::buildParser()
{
  const std::string prog = "scale";
  const std::string desc = "scales images";
  parser.prog(prog).description(desc);

  parser.add_option("--sx", "-x").action("store").type("double").set_default(1.0).help("x scale");
  parser.add_option("--sy", "-y").action("store").type("double").set_default(1.0).help("y scale");
  parser.add_option("--sz", "-z").action("store").type("double").set_default(1.0).help("z scale");

  Command::buildParser();
}

bool Scale::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  double sx = static_cast<double>(options.get("sx"));
  double sy = static_cast<double>(options.get("sy"));
  double sz = static_cast<double>(options.get("sz"));

  if (sx == 0 || sy == 0 || sz == 0)
  {
    std::cerr << "Must specify a valid scale arguemnt\n";
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
  const std::string desc = "rotates images";
  parser.prog(prog).description(desc);

  parser.add_option("--rx", "-x").action("store").type("double").help("physical axis around which to rotate (z-axis if unspecified)");
  parser.add_option("--ry", "-y").action("store").type("double").help("physical axis around which to rotate (z-axis if unspecified)");
  parser.add_option("--rz", "-z").action("store").type("double").set_default(1.0).help("physical axis around which to rotate (z-axis if unspecified)");
  parser.add_option("--radians").action("store").type("double").help("angle in radians");
  parser.add_option("--degrees").action("store").type("double").help("angle in degrees");

  Command::buildParser();
}

bool Rotate::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
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
  {
    // if degrees is specified, use it
    if (degrees != 0.0)
      radians = degToRad(degrees);

    sharedData.image.rotate(radians, makeVector({rx, ry, rz}));
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// ExtractLabel
///////////////////////////////////////////////////////////////////////////////
void ExtractLabel::buildParser()
{
  const std::string prog = "extract-label";
  const std::string desc = "extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image";
  parser.prog(prog).description(desc);

  parser.add_option("--label").action("store").type("float").set_default(1.0).help("The label value which has to be extracted. [default 1.0].");

  Command::buildParser();
}

bool ExtractLabel::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float label = static_cast<float>(options.get("label"));

  sharedData.image.extractLabel(label);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// CloseHoles
///////////////////////////////////////////////////////////////////////////////
void CloseHoles::buildParser()
{
  const std::string prog = "close-holes";
  const std::string desc = "closes holes in a given binary volume";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool CloseHoles::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  sharedData.image.closeHoles();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Threshold
///////////////////////////////////////////////////////////////////////////////
void Threshold::buildParser()
{
  const std::string prog = "threshold";
  const std::string desc = "threholds image into binary label based on upper and lower intensity bounds given by user";
  parser.prog(prog).description(desc);

  parser.add_option("--min").action("store").type("float").set_default(std::numeric_limits<float>::epsilon()).help("The lower threshold level (optional, default = epsilon)");
  parser.add_option("--max").action("store").type("float").set_default(std::numeric_limits<float>::max()).help("The upper threshold level (optional, default = FLT_MAX)");
  
  Command::buildParser();
}

bool Threshold::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float min = static_cast<float>(options.get("min"));
  float max = static_cast<float>(options.get("max"));

  if (min > max)
  {
    std::cout << "Min value should be lesser than max value";
    return false;
  }
  else
  {
    sharedData.image.threshold(min, max);
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// ComputeDT
///////////////////////////////////////////////////////////////////////////////
void ComputeDT::buildParser()
{
  const std::string prog = "compute-dt";
  const std::string desc = "computes distance transform volume from a binary (antialiased) image";
  parser.prog(prog).description(desc);

  parser.add_option("--isovalue").action("store").type("float").set_default(0.0).help("The level set value that defines the interface between foreground and background.");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool ComputeDT::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float isovalue = static_cast<float>(options.get("isovalue"));

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

  parser.add_option("--iterations").action("store").type("int").set_default(10).help("number of iterations");
  
  Command::buildParser();
}

bool CurvatureFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  int iterations = static_cast<int>(options.get("iterations"));

  if (iterations < 0)
  {
    std::cerr << "Must specify a valid iterations argument\n";
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

  parser.add_option("--alpha").action("store").type("double").set_default(10.0).help("value of alpha");
  parser.add_option("--beta").action("store").type("double").set_default(10.0).help("value of beta");
  
  Command::buildParser();
}

bool SigmoidFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
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
  const std::string desc = "segemnts structures in images using topology preserving geodesic active contour level set filter";
  parser.prog(prog).description(desc);

  parser.add_option("--featureimage").action("store").type("string").set_default("").help("path of feature image for filter");
  parser.add_option("--scaling").action("store").type("double").set_default(20.0).help("value of scale [default: 20]");
  
  Command::buildParser();
}

bool TPLevelSetFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string featureimage = static_cast<std::string>(options.get("featureimage"));
  if (featureimage == "")
  {
    std::cerr << "Must specify a valid feature image value\n";
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
  const std::string desc = "helper command that applies curvature, gradient, sigmoid, and uses them for the TPLevelSet filter";
  parser.prog(prog).description(desc);

  parser.add_option("--scaling").action("store").type("double").set_default(20.0).help("scale for TPLevelSet level set filter");
  parser.add_option("--alpha").action("store").type("double").set_default(10.0).help("value of alpha for sigmoid fitler");
  parser.add_option("--beta").action("store").type("double").set_default(10.0).help("value of beta for sigmoid fitler");  
  parser.add_option("--iterations").action("store").type("int").set_default(10).help("number of iterations for curvature filter");
  parser.add_option("--applycurvature").action("store").type("bool").set_default(true).help("default it true, but in some cases it has already been applied");
  
  Command::buildParser();
}

bool TopologyPreservingFilter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  double scaling = static_cast<double>(options.get("scaling"));
  double alpha = static_cast<double>(options.get("alpha"));
  double beta = static_cast<double>(options.get("beta"));
  int iterations = static_cast<int>(options.get("iterations"));
  bool applycurvature = static_cast<bool>(options.get("applycurvature"));

  if (iterations < 0)
  {
    std::cout << "Must specify a valid iterations argument";
    return false;
  }
  else
  {
    ImageUtils::topologyPreservingSmooth<Image>(sharedData.image, scaling, alpha, beta, iterations, applycurvature);
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Blur
///////////////////////////////////////////////////////////////////////////////
void Blur::buildParser()
{
  const std::string prog = "blur";
  const std::string desc = "applies gaussian blur";
  parser.prog(prog).description(desc);

  parser.add_option("--sigma").action("store").type("double").set_default(0.0).help("value of sigma");
  
  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool Blur::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  double sigma = static_cast<double>(options.get("sigma"));
  
  sharedData.image.gaussianBlur(sigma);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// BoundingBox
///////////////////////////////////////////////////////////////////////////////
void BoundingBox::buildParser()
{
  const std::string prog = "binary-bounding-box";
  const std::string desc = "compute largest bounding box size of the given set of binary images";
  parser.prog(prog).description(desc);

  parser.add_option("--names").action("store").type("multistring").set_default("").help("paths to images");
  parser.add_option("--padding").action("store").type("int").set_default(0).help("Number of extra voxels in each direction to pad the largest bounding box");

  Command::buildParser();
}

bool BoundingBox::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::vector<std::string> filenames = options.get("names");
  int padding = static_cast<int>(options.get("padding"));

  sharedData.region = ImageUtils::boundingBox(filenames);
  sharedData.region.pad(padding);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// CropImage
///////////////////////////////////////////////////////////////////////////////
void CropImage::buildParser()
{
  const std::string prog = "crop";
  const std::string desc = "performs translational alignment of shape image based on its center of mass or given 3D point";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool CropImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  sharedData.image.crop(sharedData.region);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ICPRigid
///////////////////////////////////////////////////////////////////////////////
void ICPRigid::buildParser()
{
  const std::string prog = "icp";
  const std::string desc = "performs iterative closed point (ICP) 3D rigid registration on pair of images";
  parser.prog(prog).description(desc);

  parser.add_option("--target").action("store").type("string").set_default("").help("distance map of target image.");
  parser.add_option("--source").action("store").type("string").set_default("").help("distance map of source image.");
  parser.add_option("--isovalue").action("store").type("float").set_default(0.0).help("");
  parser.add_option("--iterations").action("store").type("unsigned").set_default(20).help("number of iterations run ICP registration.");

  Command::buildParser();
}

bool ICPRigid::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string target = static_cast<std::string>(options.get("target"));
  std::string source = static_cast<std::string>(options.get("source"));
  float isovalue = static_cast<float>(options.get("isovalue"));
  unsigned iterations = static_cast<unsigned>(options.get("iterations"));

  if (target == "")
  {
    std::cerr << "Must specify a valid target value\n";
    return false;
  }
  else if (source == "")
  {
    std::cerr << "Must specify a valid source value\n";
    return false;
  }
  else
  {
    Image target(target);
    Image source(source);
    ImageUtils::rigidRegistration(target, source, isovalue, iterations);
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// ClipVolume
///////////////////////////////////////////////////////////////////////////////
void ClipVolume::buildParser()
{
  const std::string prog = "clip";
  const std::string desc = "chops volume with corresponding cutting planes";
  parser.prog(prog).description(desc);

  parser.add_option("--x1").action("store").type("double").set_default(0.0).help("value of cuttingplane [0][0]");
  parser.add_option("--x2").action("store").type("double").set_default(0.0).help("value of cuttingplane [0][1]");
  parser.add_option("--x3").action("store").type("double").set_default(0.0).help("value of cuttingplane [0][2]");
  parser.add_option("--y1").action("store").type("double").set_default(0.0).help("value of cuttingplane [1][0]");
  parser.add_option("--y2").action("store").type("double").set_default(0.0).help("value of cuttingplane [1][1]");
  parser.add_option("--y3").action("store").type("double").set_default(0.0).help("value of cuttingplane [1][2]");
  parser.add_option("--z1").action("store").type("double").set_default(0.0).help("value of cuttingplane [2][0]");
  parser.add_option("--z2").action("store").type("double").set_default(0.0).help("value of cuttingplane [2][1]");
  parser.add_option("--z3").action("store").type("double").set_default(0.0).help("value of cuttingplane [2][2]");
  parser.add_option("--val").action("store").type("double").set_default(0.0).help("value of clipped pixels (default: 0.0)");

  Command::buildParser();
}

bool ClipVolume::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  Point p1({static_cast<double>(options.get("x1")), static_cast<double>(options.get("x2")), static_cast<double>(options.get("x3"))});
  Point p2({static_cast<double>(options.get("y1")), static_cast<double>(options.get("y2")), static_cast<double>(options.get("y3"))});
  Point p3({static_cast<double>(options.get("z1")), static_cast<double>(options.get("z2")), static_cast<double>(options.get("z3"))});

  sharedData.image.clip(p1, p2, p3, static_cast<double>(options.get("val")));
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ReflectVolume
///////////////////////////////////////////////////////////////////////////////
void ReflectVolume::buildParser()
{
  const std::string prog = "reflect-volume";
  const std::string desc = "reflect images with respect to image center and specific axis";
  parser.prog(prog).description(desc);

  parser.add_option("--axis").action("store").type("double").set_default(0).help("axis along which it needs to be reflected");

  Command::buildParser();
}

bool ReflectVolume::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  double axis = static_cast<double>(options.get("axis"));

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

  parser.add_option("--x", "-x").action("store").type("double").set_default(0).help("x value of origin");
  parser.add_option("--y", "-y").action("store").type("double").set_default(0).help("y value of origin");
  parser.add_option("--z", "-z").action("store").type("double").set_default(0).help("z value of origin");

  Command::buildParser();
}

bool SetOrigin::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  double x = static_cast<double>(options.get("x"));
  double y = static_cast<double>(options.get("y"));
  double z = static_cast<double>(options.get("z"));

  sharedData.image.setOrigin(Point3({x, y, z}));
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

  parser.add_option("--name").action("store").type("string").set_default("").help("name of image with which to compare");
  parser.add_option("--x", "-x").action("store").type("double").set_default(0.0).help("explicit x in image space (physical coordinates)");
  parser.add_option("--y", "-y").action("store").type("double").set_default(0.0).help("explicit y in image space (e.g., 3.14)");
  parser.add_option("--z", "-z").action("store").type("double").set_default(0.0).help("explicit z in image space");

  Command::buildParser();
}

bool Compare::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];

  return sharedData.image == Image(filename);
}

///////////////////////////////////////////////////////////////////////////////
// Filter
///////////////////////////////////////////////////////////////////////////////
void Filter::buildParser()
{
  const std::string prog = "filter";
  const std::string desc = "perform given filter on image";
  parser.prog(prog).description(desc);

  parser.add_option("--type").action("store").type("string").set_default("").help("type of filter");
  parser.add_option("--iterations").action("store").type("unsigned").set_default(10).help("number of iterations");
  parser.add_option("--alpha").action("store").type("double").set_default(10.0).help("value of alpha");
  parser.add_option("--beta").action("store").type("double").set_default(10.0).help("value of beta");
  parser.add_option("--featureimage").action("store").type("string").set_default("").help("path of feature image for filter");
  parser.add_option("--scaling").action("store").type("double").set_default(20.0).help("value of scale [default: 20]");
  parser.add_option("--sigma").action("store").type("double").set_default(0.0).help("value of sigma");
  parser.add_option("--maxrmserror").action("store").type("float").set_default(0.01).help("The maximum RMS error determines how fast the solver converges. Range [0.0, 1.0], larger is faster [default 0.01].");
  parser.add_option("--numlayers").action("store").type("int").set_default(0).help("Number of layers around a 3d pixel to use for this computation [default image dims].");

  Command::buildParser();
}

bool Filter::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string type = static_cast<std::string>(options.get("type"));

  if (type == "")
  {
    std::cerr << "Must specify valid filter type value\n";
    return false;
  }

  if (!type.compare("curvature"))
  {
    unsigned iterations = static_cast<unsigned>(options.get("iterations"));

    sharedData.image.applyCurvatureFilter(iterations);
    return true;
  }

  if (!type.compare("gradient"))
  {
    sharedData.image.applyGradientFilter();
    return true;
  }

  if (!type.compare("sigmoid"))
  {
    double alpha = static_cast<double>(options.get("alpha"));
    double beta = static_cast<double>(options.get("beta"));

    sharedData.image.applySigmoidFilter(alpha, beta);
    return true;
  }

  if (!type.compare("tplevelset") || !type.compare("tp-levelset") || !type.compare("tplevel-set"))
  {
    std::string featureimage = static_cast<std::string>(options.get("featureimage"));
    if (featureimage == "")
    {
      std::cerr << "Must specify a valid feature image value\n";
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

  if (!type.compare("gaussian"))
  {
    double sigma = static_cast<double>(options.get("sigma"));

    sharedData.image.gaussianBlur(sigma);
    return true;
  }

  if (!type.compare("antialias"))
  {
    unsigned iterations = static_cast<unsigned>(options.get("iterations"));
    float maxRMSErr = static_cast<float>(options.get("maxrmserror"));
    int numLayers = static_cast<int>(options.get("numlayers"));

    sharedData.image.antialias(iterations, maxRMSErr, numLayers);
    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Warp Image
///////////////////////////////////////////////////////////////////////////////
void WarpImage::buildParser()
{
  const std::string prog = "warp-image";
  const std::string desc = "warps an image from source to target based on landmark guidence";
  parser.prog(prog).description(desc);
  parser.add_option("--source_landmarks").action("store").type("string").set_default("").help("Path to source landmarks.");
  parser.add_option("--target_landmarks").action("store").type("string").set_default("").help("Path to target landmarks.");
  parser.add_option("--factor").action("store").type("int").set_default(1).help("Every Nth(factor) point used for warping.");
  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool WarpImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string source_landmarks = static_cast<std::string>(options.get("source_landmarks"));
  std::string target_landmarks = static_cast<std::string>(options.get("target_landmarks"));
  int factor = static_cast<int>(options.get("factor"));

  if (source_landmarks == "" || target_landmarks == "")
  {
    std::cerr << "Must specify both source and target landmarks\n";
    return false;
  }

  TransformPtr transform(ImageUtils::computeWarp(source_landmarks, target_landmarks, factor));
  sharedData.image.applyTransform(transform);

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

  parser.add_option("--names").action("store").type("multistring").set_default("").help("paths to .particle files");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool ReadParticleSystem::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::vector<std::string> filenames = options.get("names");
  try {
    sharedData.particleSystem.LoadParticles(filenames);
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
  const int nModes = static_cast<int>(options.get("nmodes"));
  const std::string saveTo = static_cast<std::string>(options.get("saveto"));
  const double r = ShapeEvaluation<3>::ComputeCompactness(sharedData.particleSystem, nModes, saveTo);
  std::cout << r << std::endl;

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
  const int nModes = static_cast<int>(options.get("nmodes"));
  const std::string saveTo = static_cast<std::string>(options.get("saveto"));
  const double r = ShapeEvaluation<3>::ComputeGeneralization(sharedData.particleSystem, nModes, saveTo);
  std::cout << r << std::endl;

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
  parser.add_option("--saveto").action("store").type("string").set_default("").help("Save the reconstructions sorted by generalization along with the mapping to the original shape");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
bool Specificity::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  const int nModes = static_cast<int>(options.get("nmodes"));
  const std::string saveTo = static_cast<std::string>(options.get("saveto"));
  const double r = ShapeEvaluation<3>::ComputeSpecificity(sharedData.particleSystem, nModes, saveTo);
  std::cout << r << std::endl;

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

  parser.add_option("--second_mesh").action("store").type("string").set_default("").help("Second mesh to apply coverage.");

  Command::buildParser();
}

bool Coverage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string second_mesh_string = static_cast<std::string>(options.get("second_mesh"));

  if (second_mesh_string == "")
  {
    std::cerr << "Must specify second mesh\n";
    return false;
  }

  Mesh second_mesh;
  second_mesh.read(second_mesh_string);

  return sharedData.mesh.coverage(second_mesh);
}

} // shapeworks

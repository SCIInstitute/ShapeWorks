#include "Commands.h"
#include "Image.h"

namespace Shapeworks {



// boilerplate for a command. Copy this to start a new command
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

///////////////////////////////////////////////////////////////////////////////
int Example::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float optionName = static_cast<float>(options.get("optionName"));
  //read additional options... 

  return sharedData.image.example(optionName, ...);
}
#endif




///////////////////////////////////////////////////////////////////////////////
// Antialias
///////////////////////////////////////////////////////////////////////////////
void Antialias::buildParser()
{
  const std::string prog = "antialias";
  const std::string desc = "antialiases binary volumes";
  parser.prog(prog).description(desc);

  parser.add_option("--maxRMSError").action("store").type("float").set_default(0.01).help("The maximum RMS error determines how fast the solver converges. Range [0.0, 1.0], larger is faster [default 0.01].");
  parser.add_option("--numIterations").action("store").type("int").set_default(50).help("Number of iterations [default 50].");
  parser.add_option("--numLayers").action("store").type("int").set_default(0).help("Number of layers around a 3d pixel to use for this computation [default image dims].");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int Antialias::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float maxRMSErr = static_cast<float>(options.get("maxRMSError"));
  int numIterations = static_cast<int>(options.get("numIterations"));
  int numLayers = static_cast<int>(options.get("numLayers"));

  return sharedData.image.antialias(numIterations, maxRMSErr, numLayers);
}


///////////////////////////////////////////////////////////////////////////////
// ResampleImage
///////////////////////////////////////////////////////////////////////////////
void ResampleImage::buildParser()
{
  const std::string prog = "resampleimage";
  const std::string desc = "resamples images to be isotropic";
  parser.prog(prog).description(desc);

  parser.add_option("--isBinary").action("store").type("bool").set_default(false).help("A flag to treat the input image as a binary image (specialized resampling pipeline) [default disabled].");
  parser.add_option("--recenter").action("store").type("bool").set_default(false).help("A flag to recenter the image, i.e. change the origin in the image header to the physcial coordinates of the first voxel (lower left corner) [default disabled].");
  parser.add_option("--isoSpacing").action("store").type("float").set_default(1.0f).help("The isotropic spacing in all dimensions [default 1.0].");
  parser.add_option("--sizeX").action("store").type("unsigned").set_default(0).help("Image size in x-direction [default autmatically estimated from the input image].");
  parser.add_option("--sizeY").action("store").type("unsigned").set_default(0).help("Image size in y-direction [default autmatically estimated from the input image].");
  parser.add_option("--sizeZ").action("store").type("unsigned").set_default(0).help("Image size in z-direction [default autmatically estimated from the input image].");

  Command::buildParser();
}

int ResampleImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  bool isBinary = static_cast<bool>(options.get("isBinary"));
  bool recenter = static_cast<bool>(options.get("recenter"));
  float isoSpacing = static_cast<float>(options.get("isoSpacing"));
  unsigned sizeX = static_cast<unsigned>(options.get("sizeX"));
  unsigned sizeY = static_cast<unsigned>(options.get("sizeY"));
  unsigned sizeZ = static_cast<unsigned>(options.get("sizeZ"));

  bool success = sharedData.image.resample(isBinary, isoSpacing, Dims({sizeX, sizeY, sizeZ}));
  if (success && recenter)
    success = sharedData.image.recenter();
  return success;
}


///////////////////////////////////////////////////////////////////////////////
// RecenterImage
///////////////////////////////////////////////////////////////////////////////
void RecenterImage::buildParser()
{
  const std::string prog = "recenterimage";
  const std::string desc = "recenters an image by changing its origin";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

int RecenterImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  return sharedData.image.recenter();
}


///////////////////////////////////////////////////////////////////////////////
// Binarize
///////////////////////////////////////////////////////////////////////////////
void Binarize::buildParser()
{
  const std::string prog = "binarize";
  const std::string desc = "binarizes an image at some given threshold";
  parser.prog(prog).description(desc);

  parser.add_option("--threshold").action("store").type("float").set_default(0.0f).help("Resulting image has two values for pixels: > threshold set to inside value, <= threshold set to outside value [default 0.0].");
  parser.add_option("--inside").action("store").type("float").set_default(1.0f).help("Value of pixels > threshold [default 1.0].");
  parser.add_option("--outside").action("store").type("float").set_default(0.0f).help("Value of pixels <= threshold [default 0.0].");

  Command::buildParser();
}

int Binarize::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float threshold = static_cast<float>(options.get("threshold"));
  float inside = static_cast<float>(options.get("inside"));
  float outside = static_cast<float>(options.get("outside"));

  // For the command line version, we want binarize of an already binarized image to produce the same image, so we add
  // the tiniest possible epsilon to the specified threshold if that threshold is set to zero. 
  float eps = (threshold == 0.0f) ? std::numeric_limits<float>::epsilon() : 0.0f;

  return sharedData.image.binarize(threshold + eps, inside, outside);
}


///////////////////////////////////////////////////////////////////////////////
// Clip
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Smoothmesh
///////////////////////////////////////////////////////////////////////////////
void Smoothmesh::buildParser()
{
  const std::string prog = "smoothmesh";
  const std::string desc = "smooths meshes";
  parser.prog(prog).description(desc);
  
  // TODO
  // parser.add_option("--maxRMSError").action("store").type("float").set_default(0.01).help("The maximum RMS error allowed.");
  // parser.add_option("--numIterations").action("store").type("int").set_default(50).help("Number of iterations.");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int Smoothmesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  // float maxRMSErr = static_cast<float>(options.get("maxRMSError"));
  // int numIter = static_cast<int>(options.get("numIterations"));

  return sharedData.mesh.smooth(/*maxRMSErr, numIter*/);
}



} // Shapeworks

#include "Commands.h"
#include "Image.h"

namespace shapeworks {



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
// ReadImage
///////////////////////////////////////////////////////////////////////////////
void ReadImage::buildParser()
{
  const std::string prog = "readimage";
  const std::string desc = "reads an image";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to read");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int ReadImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];

  return sharedData.image.read(filename);
}


///////////////////////////////////////////////////////////////////////////////
// WriteImage
///////////////////////////////////////////////////////////////////////////////
void WriteImage::buildParser()
{
  const std::string prog = "writeimage";
  const std::string desc = "writes the current image (determines type by its extension)";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to write");
  parser.add_option("--compressed").action("store").type("bool").set_default(true).help("whether to compress file [default is true]");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int WriteImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];
  bool compressed = static_cast<bool>(options.get("compressed"));
  
  return sharedData.image.write(filename, compressed);
}


///////////////////////////////////////////////////////////////////////////////
// ReadMesh
///////////////////////////////////////////////////////////////////////////////
void ReadMesh::buildParser()
{
  const std::string prog = "readmesh";
  const std::string desc = "reads a mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to read");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int ReadMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];

  return sharedData.mesh.read(filename);
}


///////////////////////////////////////////////////////////////////////////////
// WriteMesh
///////////////////////////////////////////////////////////////////////////////
void WriteMesh::buildParser()
{
  const std::string prog = "writemesh";
  const std::string desc = "writes the current mesh (determines type by its extension)";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to write");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int WriteMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];
  
  return sharedData.mesh.write(filename);
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
  parser.add_option("--numiterations").action("store").type("int").set_default(50).help("Number of iterations [default 50].");
  parser.add_option("--numlayers").action("store").type("int").set_default(0).help("Number of layers around a 3d pixel to use for this computation [default image dims].");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int Antialias::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float maxRMSErr = static_cast<float>(options.get("maxrmserror"));
  int numIterations = static_cast<int>(options.get("numiterations"));
  int numLayers = static_cast<int>(options.get("numlayers"));

  return sharedData.image.antialias(numIterations, maxRMSErr, numLayers);
}


///////////////////////////////////////////////////////////////////////////////
// ResampleImage
///////////////////////////////////////////////////////////////////////////////
void ResampleImage::buildParser()
{
  const std::string prog = "resample";
  const std::string desc = "resamples images to be isotropic";
  parser.prog(prog).description(desc);

  parser.add_option("--isbinary").action("store").type("bool").set_default(false).help("For binary input image (this will antialias the sample using bspline interpolation) [default false].");
  parser.add_option("--isospacing").action("store").type("float").set_default(1.0f).help("The isotropic spacing in all dimensions [default 1.0].");
  parser.add_option("--sizex").action("store").type("unsigned").set_default(0).help("Image size in x-direction [default autmatically estimated from the input image].");
  parser.add_option("--sizey").action("store").type("unsigned").set_default(0).help("Image size in y-direction [default autmatically estimated from the input image].");
  parser.add_option("--sizez").action("store").type("unsigned").set_default(0).help("Image size in z-direction [default autmatically estimated from the input image].");

  Command::buildParser();
}

int ResampleImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  bool isbinary = static_cast<bool>(options.get("isbinary"));
  float isoSpacing = static_cast<float>(options.get("isospacing"));
  unsigned sizeX = static_cast<unsigned>(options.get("sizex"));
  unsigned sizeY = static_cast<unsigned>(options.get("sizey"));
  unsigned sizeZ = static_cast<unsigned>(options.get("sizez"));

  return sharedData.image.resample(isoSpacing, isbinary, Dims({sizeX, sizeY, sizeZ}));
}


///////////////////////////////////////////////////////////////////////////////
// RecenterImage
///////////////////////////////////////////////////////////////////////////////
void RecenterImage::buildParser()
{
  const std::string prog = "recenterimage";
  const std::string desc = "recenters an image by changing its origin in the image header to the physcial coordinates of the center of the image";
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
// Padvolume
///////////////////////////////////////////////////////////////////////////////
void Padvolume::buildParser()
{
  const std::string prog = "padvolume";
  const std::string desc = "A command line tool that pad a contant value in the x-, y-, and z- directions of a given volume";
  parser.prog(prog).description(desc);

  parser.add_option("--paddingSize").action("store").type("int").set_default(0).help("Number of voxels to be padded in each direction.");
  parser.add_option("--paddingValue").action("store").type("float").set_default(0).help("Value to be used to fill padded voxels.");
  
  Command::buildParser();
}

int Padvolume::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  int paddingSize = static_cast<int>(options.get("paddingSize"));
  float paddingValue = static_cast<float>(options.get("paddingValue"));

  return sharedData.image.padvolume(paddingSize, paddingValue);
}


///////////////////////////////////////////////////////////////////////////////
// Clip
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// SmoothMesh
///////////////////////////////////////////////////////////////////////////////
void SmoothMesh::buildParser()
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
int SmoothMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  // float maxRMSErr = static_cast<float>(options.get("maxRMSError"));
  // int numIter = static_cast<int>(options.get("numIterations"));

  return sharedData.mesh.smooth(/*maxRMSErr, numIter*/);
}



} // shapeworks

#include "Commands.h"
#include "Image.h"
#include <limits>

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
  const std::string prog = "read-image";
  const std::string desc = "reads an image";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to read");
  parser.add_option("--dicom_dir").action("store").type("string").set_default("").help("name of dicom dir to read");

  Command::buildParser();
}

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
  const std::string prog = "write-image";
  const std::string desc = "writes the current image (determines type by its extension)";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to write");
  parser.add_option("--compressed").action("store").type("bool").set_default(true).help("whether to compress file [default is true]");

  Command::buildParser();
}

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
  const std::string prog = "read-mesh";
  const std::string desc = "reads a mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to read");

  Command::buildParser();
}

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
  const std::string prog = "write-mesh";
  const std::string desc = "writes the current mesh (determines type by its extension)";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("name of file to write");

  Command::buildParser();
}

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
  const std::string prog = "isoresample";
  const std::string desc = "resamples images to be isotropic";
  parser.prog(prog).description(desc);

  parser.add_option("--isospacing").action("store").type("double").set_default(1.0f).help("The isotropic spacing in all dimensions [default 1.0].");
  parser.add_option("--sizex").action("store").type("unsigned").set_default(0).help("Image size in x-direction [default autmatically estimated from the input image].");
  parser.add_option("--sizey").action("store").type("unsigned").set_default(0).help("Image size in y-direction [default autmatically estimated from the input image].");
  parser.add_option("--sizez").action("store").type("unsigned").set_default(0).help("Image size in z-direction [default autmatically estimated from the input image].");

  Command::buildParser();
}

int ResampleImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  double isoSpacing = static_cast<double>(options.get("isospacing"));
  unsigned sizeX = static_cast<unsigned>(options.get("sizex"));
  unsigned sizeY = static_cast<unsigned>(options.get("sizey"));
  unsigned sizeZ = static_cast<unsigned>(options.get("sizez"));

  return sharedData.image.isoresample(isoSpacing, Dims({sizeX, sizeY, sizeZ}));
}

///////////////////////////////////////////////////////////////////////////////
// RecenterImage
///////////////////////////////////////////////////////////////////////////////
void RecenterImage::buildParser()
{
  const std::string prog = "recenter-image";
  const std::string desc = "recenters an image by changing its origin in the image\n\t\t\theader to the physical coordinates of the center of the image";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

int RecenterImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  return sharedData.image.recenter();
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
  parser.add_option("--value").action("store").type("float").set_default(0).help("Value to be used to fill padded voxels.");
  
  Command::buildParser();
}

int PadImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  int padding = static_cast<int>(options.get("padding"));
  float value = static_cast<float>(options.get("value"));

  return sharedData.image.pad(padding, value);
}

///////////////////////////////////////////////////////////////////////////////
// SmoothMesh
///////////////////////////////////////////////////////////////////////////////
void SmoothMesh::buildParser()
{
  const std::string prog = "smooth-mesh";
  const std::string desc = "smooths meshes";
  parser.prog(prog).description(desc);
  
  // TODO
  // parser.add_option("--maxRMSError").action("store").type("float").set_default(0.01).help("The maximum RMS error allowed.");
  // parser.add_option("--numIterations").action("store").type("int").set_default(50).help("Number of iterations.");

  Command::buildParser();
}

int SmoothMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  // float maxRMSErr = static_cast<float>(options.get("maxRMSError"));
  // int numIter = static_cast<int>(options.get("numIterations"));

  return sharedData.mesh.smooth(/*maxRMSErr, numIter*/);
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

int Coverage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string second_mesh_string = static_cast<std::string>(options.get("second_mesh"));

  if (second_mesh_string == "")
  {
    std::cerr << "Must specify second mesh\n";
    return -1;
  }
  Mesh second_mesh;
  second_mesh.read(second_mesh_string);

  return sharedData.mesh.coverage(second_mesh);
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
  parser.add_option("--tx").action("store").type("double").set_default(0.0).help("Description of optionName.");
  parser.add_option("--ty").action("store").type("double").set_default(0.0).help("Description of optionName.");
  parser.add_option("--tz").action("store").type("double").set_default(0.0).help("Description of optionName.");

  Command::buildParser();
}

int Translate::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  bool centerofmass = static_cast<bool>(options.get("centerofmass"));
  
  if (centerofmass)
  {
    Point3 com = sharedData.image.centerOfMass();
    sharedData.transform.translate(com.GetVectorFromOrigin());
  }
  else
  {
    double tx = static_cast<double>(options.get("tx"));
    double ty = static_cast<double>(options.get("ty"));
    double tz = static_cast<double>(options.get("tz"));

    double v[3] = {tx, ty, tz};
    sharedData.transform.translate(Vector3(v));
  }

  return 1;
}

///////////////////////////////////////////////////////////////////////////////
// ApplyTransform
///////////////////////////////////////////////////////////////////////////////
void ApplyTransform::buildParser()
{
  const std::string prog = "applytransform";
  const std::string desc = "apply current transformation";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

int ApplyTransform::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  return sharedData.image.applyTransform(sharedData.transform);
}

///////////////////////////////////////////////////////////////////////////////
// ExtractLabel
///////////////////////////////////////////////////////////////////////////////
void ExtractLabel::buildParser()
{
  const std::string prog = "extractlabel";
  const std::string desc = "extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image";
  parser.prog(prog).description(desc);

  parser.add_option("--label").action("store").type("float").set_default(1.0).help("The label value which has to be extracted. [default 1.0].");

  Command::buildParser();
}

int ExtractLabel::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float label = static_cast<float>(options.get("label"));

  return sharedData.image.extractLabel(label);
}

///////////////////////////////////////////////////////////////////////////////
// CloseHoles
///////////////////////////////////////////////////////////////////////////////
void CloseHoles::buildParser()
{
  const std::string prog = "closeholes";
  const std::string desc = "closes holes in a given binary volume";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

int CloseHoles::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  return sharedData.image.closeHoles();
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

int Threshold::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float min = static_cast<float>(options.get("min"));
  float max = static_cast<float>(options.get("max"));

  return sharedData.image.threshold(min, max);
}

///////////////////////////////////////////////////////////////////////////////
// FastMarch
///////////////////////////////////////////////////////////////////////////////
void FastMarch::buildParser()
{
  const std::string prog = "fastmarch";
  const std::string desc = "computes distance transform volume from a binary (antialiased) image";
  parser.prog(prog).description(desc);

  parser.add_option("--isovalue").action("store").type("float").set_default(0.0).help("The level set value that defines the interface between foreground and background.");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int FastMarch::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float isovalue = static_cast<float>(options.get("isovalue"));

  return sharedData.image.fastMarch(isovalue);
}

} // shapeworks

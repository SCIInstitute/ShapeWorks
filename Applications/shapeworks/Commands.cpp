#include "Commands.h"
#include "Image.h"

namespace Shapeworks {

///////////////////////////////////////////////////////////////////////////////
// Antialias
///////////////////////////////////////////////////////////////////////////////
void Antialias::buildParser()
{
  const std::string prog = "antialias";
  const std::string usage = "%prog [OPTION]";
  const std::string desc = "antialiases binary volumes";
  const std::string epilog = "";
  parser.prog(prog).usage(usage).description(desc).epilog(epilog);
  
  parser.add_option("--maxRMSError").action("store").type("float").set_default(0.01).help("The maximum RMS error allowed.");
  parser.add_option("--numIterations").action("store").type("int").set_default(50).help("Number of iterations.");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int Antialias::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float maxRMSErr = static_cast<float>(options.get("maxRMSError"));
  int numIter = static_cast<int>(options.get("numIterations"));

  return sharedData.image.antialias(maxRMSErr, numIter);
}

///////////////////////////////////////////////////////////////////////////////
// Clip
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// etc (todo)

///////////////////////////////////////////////////////////////////////////////
// Smoothmesh
///////////////////////////////////////////////////////////////////////////////
void Smoothmesh::buildParser()
{
  const std::string prog = "smoothmesh";
  const std::string usage = "%prog [OPTION]";
  const std::string desc = "smooths meshes";
  const std::string epilog = "";
  parser.prog(prog).usage(usage).description(desc).epilog(epilog);
  
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

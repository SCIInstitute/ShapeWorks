#include "Commands.h"
#include "Image.h"

namespace Shapeworks {

///////////////////////////////////////////////////////////////////////////////
// Antialias

void Antialias::buildParser()
{
  const std::string prog = "antialias";
  const std::string usage = "%prog [OPTION]";
  const std::string version = "%prog 0.1";
  const std::string desc = "antialiases binary volumes";
  const std::string epilog = "";

  parser.prog(prog).usage(usage).version(version).description(desc).epilog(epilog);

  parser.add_option("--inFilename").action("store").type("string").set_default("").help("The filename of the input image to be resampled.");
  parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output resampled image.");
  parser.add_option("--maxRMSError").action("store").type("float").set_default(0.01).help("The maximum RMS error allowed.");
  parser.add_option("--numIterations").action("store").type("int").set_default(50).help("Number of iterations.");
}

int Antialias::execute(const optparse::Values options)
{
  std::string inFilename  = options["inFilename"];
  std::string outFilename = options["outFilename"];
  float maxRMSErr         = static_cast<float>(options.get("maxRMSError"));
  int numIter             = static_cast<int>(options.get("numIterations"));

  Image img(inFilename);
  img.antialias(maxRMSErr, numIter);
  return img.write(outFilename);
}

///////////////////////////////////////////////////////////////////////////////
// Clip

// etc, todo

} // Shapeworks

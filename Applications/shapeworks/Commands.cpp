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
  //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
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
  std::string outFilename = options["outFilename"];  // <ctc> should throw an exception (C++11) if it's not set; [x] check this. I had to take another route on the option parser, so we need to check this ourselves, but it does give us the option of making it even easier to stack commands... just need to figure out where to keep the Image associated with a given run. Maybe Executable?.

  float maxRMSErr         = static_cast<float>(options.get("maxRMSError"));
  int numIter             = static_cast<int>(options.get("numIterations"));

  // <ctc> to enable postfix commands (a cmd uses results from the previous cmd), we would load infile or use prev results here, then call image function,
  //       or... create a static image shared by image commands in order for the results to be shared
  //       or... optionally share results through commands or executable class
  //       ...all of which complicate matters to some degree, so we really want to make sure this is an important feature before creating it
  //
  //       ...however, we *do* want a particular instance of (say) Image class to be able to use data without every function needing to read/write files,
  //          which will make a command here look (something) like the following (TODO sooner than later):
  
  Image img(inFilename);
  img.antialias(maxRMSErr, numIter);
  return img.write(outFilename);
}

///////////////////////////////////////////////////////////////////////////////
// Clip

// etc, todo

} // Shapeworks

#include "Command.h"
#include <sstream>


namespace Shapeworks {

///////////////////////////////////////////////////////////////////////////////
void Command::buildParser()
{
  // ensure only arguments for this command get passed and remaining are returned by parse_args
  parser.disable_interspersed_args(); 

  parser.add_option("--inFilename").action("store").type("string").set_default("").help("Specify input filename. Otherwise uses currently loaded file.");
  parser.add_option("--outFilename").action("store").type("string").set_default("").help("Specify output filename. Otherwise doesn't save output.");
}

///////////////////////////////////////////////////////////////////////////////
std::vector<std::string> Command::parse_args(const std::vector<std::string> &arguments)
{
  parser.parse_args(arguments);
  auto remaining_args = parser.args();
  return std::vector<std::string>(remaining_args.begin(), remaining_args.end());
}

///////////////////////////////////////////////////////////////////////////////
int Command::run(SharedCommandData &sharedData)
{
  const optparse::Values &options = parser.get_parsed_options();
  std::string inFilename  = options["inFilename"];
  std::string outFilename = options["outFilename"];

  read(inFilename, sharedData);
  if (this->execute(options, sharedData))
    if (write(outFilename, sharedData))
      return EXIT_SUCCESS;

  return EXIT_FAILURE;
}

///////////////////////////////////////////////////////////////////////////////
bool ImageCommand::read(const std::string inFilename, SharedCommandData &sharedData)
{
  if (!inFilename.empty())
    return sharedData.image.read(inFilename);

  std::cout << "Not reading anything (image)." << std::endl;
  return true; // treat nothing to read as a noop (a soft success)
}

///////////////////////////////////////////////////////////////////////////////
bool ImageCommand::write(const std::string outFilename, SharedCommandData &sharedData)
{
  if (!outFilename.empty())
    return sharedData.image.write(outFilename);

  std::cout << "Not writing anything (image)." << std::endl;
  return true; // treat nothing to write as a noop (a soft success)
}

///////////////////////////////////////////////////////////////////////////////
bool MeshCommand::read(const std::string inFilename, SharedCommandData &sharedData)
{
  if (!inFilename.empty())
    return sharedData.mesh.read(inFilename);

  std::cout << "Not reading anything (mesh)." << std::endl;
  return true; // treat nothing to read as a noop (a soft success)
}

///////////////////////////////////////////////////////////////////////////////
bool MeshCommand::write(const std::string outFilename, SharedCommandData &sharedData)
{
  if (!outFilename.empty())
    return sharedData.mesh.write(outFilename);

  std::cout << "Not writing anything (mesh)." << std::endl;
  return true; // treat nothing to write as a noop (a soft success)
}


} // Shapeworks


std::ostream& operator<<(std::ostream& os, const Shapeworks::Command &cmd)
{
  os << cmd.desc();
  return os;
}

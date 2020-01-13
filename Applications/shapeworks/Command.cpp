#include "Command.h"
#include <sstream>


namespace shapeworks {

///////////////////////////////////////////////////////////////////////////////
void Command::buildParser()
{
  // ensure only arguments for this command get passed and remaining are returned by parse_args
  parser.disable_interspersed_args();
  parser.usage("%prog [args]...").epilog("");

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

  return true; // treat nothing to read as a noop (a soft success)
}

///////////////////////////////////////////////////////////////////////////////
bool ImageCommand::write(const std::string outFilename, SharedCommandData &sharedData)
{
  if (!outFilename.empty())
    return sharedData.image.write(outFilename);

  return true; // treat nothing to write as a noop (a soft success)
}

///////////////////////////////////////////////////////////////////////////////
bool MeshCommand::read(const std::string inFilename, SharedCommandData &sharedData)
{
  if (!inFilename.empty())
    return sharedData.mesh.read(inFilename);

  return true; // treat nothing to read as a noop (a soft success)
}

///////////////////////////////////////////////////////////////////////////////
bool MeshCommand::write(const std::string outFilename, SharedCommandData &sharedData)
{
  if (!outFilename.empty())
    return sharedData.mesh.write(outFilename);

  return true; // treat nothing to write as a noop (a soft success)
}


} // shapeworks


std::ostream& operator<<(std::ostream& os, const shapeworks::Command &cmd)
{
  os << cmd.desc();
  return os;
}

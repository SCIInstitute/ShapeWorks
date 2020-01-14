#include "Command.h"
#include <sstream>


namespace shapeworks {

///////////////////////////////////////////////////////////////////////////////
void Command::buildParser()
{
  // ensure only arguments for this command get passed and remaining are returned by parse_args
  parser.disable_interspersed_args();
  parser.usage("%prog [args]...").epilog("");
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

  return this->execute(options, sharedData) ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // shapeworks


std::ostream& operator<<(std::ostream& os, const shapeworks::Command &cmd)
{
  os << cmd.desc();
  return os;
}

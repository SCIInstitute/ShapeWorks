#include "Command.h"
#include <sstream>


namespace Shapeworks {

std::vector<std::string> Command::parse_args(const std::vector<std::string> &arguments)
{
  options = parser.parse_args(arguments);
  auto remaining_args = parser.args();
  return std::vector<std::string>(remaining_args.begin(), remaining_args.end());
}

int Command::run()
{
  std::cout << "Command::run " << this->name() << "...\n";
  if (this->execute(options))
    return EXIT_SUCCESS;
  else
    return EXIT_FAILURE;
}

} // Shapeworks


std::ostream& operator<<(std::ostream& os, const Shapeworks::Command &cmd)
{
  os << cmd.desc();  // <ctc> changed this from calling cmd.parser.description to simpliy cmd.desc() to remove this from being a friend of Command
  // <ctc> ...but figure out why the friend part doesn't work!!!
  return os;
}

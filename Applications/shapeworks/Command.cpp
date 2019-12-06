#include "Command.h"
#include <sstream>


namespace Shapeworks {

int Command::run(const std::vector<std::string> &arguments)
{
  std::cout << "Command::run " << this->name() << "...\n";
  const optparse::Values options = parser.parse_args(arguments);

  if (!parser.args().empty())  // <ctc> [] test wrong number arguments here as well as in the command's execute function (can it be called if this fails?)
  {
    // note: throws an exception or returns EXIT_FAILURE if there is an error
    std::stringstream ss;
    ss << "\tunknown arguments: " << std::endl;
    for (auto arg : parser.args())
      ss << arg << ", ";
    throw std::runtime_error("Command '" + this->name() + "' failed:\n" + ss.str());
  }

  return this->execute(options);
}

} // Shapeworks


std::ostream& operator<<(std::ostream& os, const Shapeworks::Command &cmd)
{
  os << cmd.desc();  // <ctc> changed this from calling cmd.parser.description to simpliy cmd.desc() to remove this from being a friend of Command
  // <ctc> ...but figure out why the friend part doesn't work!!!
  return os;
}

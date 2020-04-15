#include "Executable.h"

namespace shapeworks {

///////////////////////////////////////////////////////////////////////////////
void Executable::buildParser()
{
  parser.description("Unified Shapeworks executable for all commands.");

  parser.usage("Usage: %prog <command> [args]...");
  parser.version("%prog 1.0\nMIT license (todo: verify)");
  parser.description("Command line utilities for understanding groups of related shapes.");
  parser.epilog("Available commands:");
  parser.disable_interspersed_args(); // so everything after a command's name will be passed to that command (ex: its --help argumemnt)
  
  // global options
  parser.add_option("-q", "--quiet").action("store_false").dest("verbose").set_default("1").help("don't print status messages");
}

///////////////////////////////////////////////////////////////////////////////
Executable::Executable()
{
  buildParser();
}

///////////////////////////////////////////////////////////////////////////////
void Executable::addCommand(Command &command)
{
  auto cmdkey = commands.find(command.name());
  if (cmdkey != commands.end()) {
    throw std::runtime_error(cmdkey->first + " already exists!");
  }
#if DEBUG_CONSOLIDATION
  std::cout << "Adding " << command.name() << "...\n";
#endif
  commands.insert(std::pair<std::string, Command&>(command.name(),command));

  std::map<std::string, std::string> &command_type_descriptions = parser_epilog[command.type()];
  command_type_descriptions[command.name()] = command.desc();

  unsigned opt_width = 24;
  unsigned indent = 2;
  std::stringstream ss;
  ss << "Available commands:\n---------------------\n";
  for (auto cmdtype: parser_epilog)
  {
    ss << "\n";
    ss << cmdtype.first << "\n";
    for (auto cmd: cmdtype.second)
      ss << std::string(indent, ' ')
         << cmd.first << std::string(opt_width - indent - cmd.first.length(), ' ')
         << cmd.second << "\n";
  }
  parser.epilog(ss.str());
}

///////////////////////////////////////////////////////////////////////////////
int Executable::run(std::vector<std::string> arguments, SharedCommandData &sharedData)
{
  bool retval = 0;
  while (!retval && !arguments.empty())
  {
    auto cmd = commands.find(arguments[0]);
    if (cmd != commands.end()) {
#if DEBUG_CONSOLIDATION
      std::cout << "Executing " << cmd->first << "...\n";
#endif
      auto args = std::vector<std::string>(arguments.begin() + 1, arguments.end());
      arguments = cmd->second.parse_args(args);
      retval = cmd->second.run(sharedData);
    }
    else {
      std::stringstream ss;
      ss << "\tunknown arguments: " << std::endl;
      for (auto arg : parser.args())
        ss << arg << ", ";
      throw std::runtime_error("Unknown arguments or command '" + arguments[0] + "' not found.\n");
    }
  }

  return retval;
}

///////////////////////////////////////////////////////////////////////////////
int Executable::run(int argc, char const *const *argv)
{
  const optparse::Values options = parser.parse_args(argc, argv);
  
  // shapeworks global options
  // todo: handle --quiet, --verbose, whatever else is global

  if (parser.args().empty())
  {
    std::cerr << "no command specified \n";
    parser.print_help(); // prints available commands
    return 1;
  }

  // items used for successive operations by commands
  SharedCommandData sharedData;
  return run(parser.args(), sharedData);
}


} // shapeworks


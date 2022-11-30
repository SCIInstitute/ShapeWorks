#include "Executable.h"
#include <sstream>
#include <Applications/Configuration.h>

namespace shapeworks {

///////////////////////////////////////////////////////////////////////////////
void Executable::buildParser()
{  
  parser.description("Unified ShapeWorks executable that includes command line utilities for automated construction of compact statistical landmark-based shape models of ensembles of shapes");
  parser.usage("Usage: %prog <command> [args]...");
  parser.version(std::string("ShapeWorks Version ") + SHAPEWORKS_VERSION);
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

/// formatDesc
///
/// formats the description of this command, automatically padding the lines for clarity
///
/// \param desc command description
/// \param indent the amount to indent if line breaks are needed
/// \param desc_line_length line_len = indent + desc_line_length
std::string formatDesc(std::string desc, const int indent = 24, const int desc_line_length = 67)
{
  std::string padded_desc;
  while (desc.length() > desc_line_length)
  {
    int last_space_pos = desc.rfind(" ", desc_line_length);
    if (last_space_pos == std::string::npos) last_space_pos = desc_line_length;
    std::string line(desc.substr(0, last_space_pos));
    desc.erase(0, last_space_pos + 1); // also erase the trailing space
    padded_desc.append(line + "\n");
    padded_desc += std::string(indent, ' ');
  }
  padded_desc.append(desc);
  
  return padded_desc;
}

///////////////////////////////////////////////////////////////////////////////
void Executable::addCommand(Command &command)
{
  auto cmdkey = commands.find(command.name());
  if (cmdkey != commands.end()) {
    throw std::runtime_error(cmdkey->first + " already exists!");
  }

  commands.insert(std::pair<std::string, Command&>(command.name(), command));
  auto nodashname = command.name();
  nodashname.erase(std::remove(nodashname.begin(), nodashname.end(), '-'), nodashname.end());
  commands.insert(std::pair<std::string, Command&>(nodashname, command));

  std::map<std::string, std::string> &command_type_descriptions = parser_epilog[command.type()];
  command_type_descriptions[command.name()] = command.desc();

  unsigned opt_width = 32;
  unsigned desc_width = 100 - opt_width;
  unsigned indent = 2;
  std::stringstream ss;
  ss << "Available commands (dashes optional):\n---------------------\n";
  for (auto cmdtype: parser_epilog)
  {
    ss << "\n";
    ss << cmdtype.first << "\n";
    for (auto cmd: cmdtype.second)
      ss << std::string(indent, ' ')
         << cmd.first << std::string(opt_width - indent - cmd.first.length(), ' ')
         << formatDesc(cmd.second, opt_width, desc_width) << "\n";
  }
  parser.epilog(ss.str());
}

///////////////////////////////////////////////////////////////////////////////
int Executable::run(std::vector<std::string> arguments, SharedCommandData &sharedData)
{
  bool retval = EXIT_SUCCESS;
  while (retval == EXIT_SUCCESS && !arguments.empty())
  {
    auto cmd = commands.find(arguments[0]);
    if (cmd != commands.end()) {
      auto args = std::vector<std::string>(arguments.begin() + 1, arguments.end());
      arguments = cmd->second.parse_args(args);
      try {
        retval = cmd->second.run(sharedData);
      } catch(std::exception &e) {
        throw std::runtime_error("'" + cmd->first + "' Error: " + e.what());
      }
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
  // todo: handle --quiet, --verbose, --version, etc.

  if (parser.args().empty())
  {
    std::cerr << "no command specified \n";
    parser.print_help(); // prints available commands
    return EXIT_FAILURE;
  }

  // items used for successive operations by commands
  SharedCommandData sharedData;
  return run(parser.args(), sharedData);
}

} // shapeworks

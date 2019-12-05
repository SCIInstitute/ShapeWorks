#include "Executable.h"

namespace Shapeworks {

void Executable::buildParser()
{
  parser.description("Unified Shapeworks executable for all commands.");

  parser.usage("Usage: %prog <command> [args]...");
  parser.version("%prog 1.0\nMIT license (todo: verify)");
  parser.description("Command line utilities for understanding groups of related shapes.");
  parser.epilog("Available commands:\n");
  parser.disable_interspersed_args(); // so everything after a command's name will be passed to that command (ex: its --help argumemnt)
  
  // global options
  parser.add_option("-q", "--quiet").action("store_false").dest("verbose").set_default("1").help("don't print status messages");
}

// constructor
Executable::Executable()
{
  std::cout << "Shapeworks executable ctor...\n";
  buildParser();
}

void Executable::addCommand(/*const*/ Command &command, bool replace) //<ctc> command should probably be const (todo: try it)
{
  auto cmdkey = commands.find(command.name());
  if (cmdkey != commands.end()) {
    std::cout << "Found " << cmdkey->first << " " << cmdkey->second << '\n';
    std::cout << "Pass true for second argument to replace command.\n";
    if (!replace) return;
  }
  std::cout << "Adding " << command.name() << "...\n";
  const std::string foo("foo");
  commands.insert(std::pair<std::string, Command&>(command.name(),command));
  parser.usage(parser.usage() + "\n" + command.desc());
}

int Executable::run(int argc, char const *const *argv)
{
  const optparse::Values options = parser.parse_args(argc, argv);
  
  // shapeworks global options
  // todo: handle --quiet, --verbose, whatever else is global
  
  if (!parser.args().empty())
  {
    auto cmd = commands.find(parser.args()[0]);
    if (cmd != commands.end()) {
      std::cout << "Executing " << cmd->first << "...\n";
      return cmd->second.run(std::vector<std::string>(parser.args().begin() + 1, parser.args().end()));
    }
    else {
      throw std::runtime_error("Command '" + parser.args()[0] + " not found.\n");
    }
  }

  std::cout << "no command specified, usage: \n";
  parser.print_help(); //<ctc> should print all commands as epilog ([] check this)
  return 0;
}


} // Shapeworks


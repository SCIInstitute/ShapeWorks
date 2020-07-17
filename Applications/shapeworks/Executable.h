#pragma once

/*
 * Unified shapeworks executable.
 */

#include "Command.h"
#include <optparse.h>

namespace shapeworks {

class Executable {
public:
  Executable();

  void addCommand(Command &command);
  int run(int argc, char const *const *argv);

private:
  void buildParser();
  optparse::OptionParser parser;
  std::map<std::string, Command&> commands;
  std::map<std::string, std::map<std::string, std::string> > parser_epilog; // <command_type, <command_name, desc> >

  int run(std::vector<std::string> arguments, SharedCommandData &sharedData);
};

}; // shapeworks

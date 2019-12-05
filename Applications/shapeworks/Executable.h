#pragma once

/*
 * Unified shapeworks executable.
 */

#include "Command.h"
#include <optparse.h>

namespace Shapeworks {


class Executable {
public:
  Executable();

  void addCommand(/*const*/ Command &command, bool replace = false); //<ctc> maybe (probably) the commands themselves should be const
  int run(int argc, char const *const *argv);

private:
  optparse::OptionParser parser;
  void buildParser();
  std::map<std::string, Command&> commands;
  //std::map<std::string, std::reference_wrapper<Command>> commands;
  //std::map<std::string, Command> commands;

};


}; // Shapeworks

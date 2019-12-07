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

  void addCommand(Command &command);
  int run(int argc, char const *const *argv);

private:
  void buildParser();
  optparse::OptionParser parser;
  std::map<std::string, Command&> commands;

  int run(std::vector<std::string> arguments);
};


}; // Shapeworks

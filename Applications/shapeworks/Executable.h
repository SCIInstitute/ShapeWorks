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
  optparse::OptionParser parser;
  void buildParser();
  std::map<std::string, Command&> commands;

};


}; // Shapeworks

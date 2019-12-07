#pragma once

/*
 * Command provided by unified shapeworks executable.
 */

#include "OptionParser.h"

#include <iostream>
#include <stdexcept>


namespace Shapeworks {

class Command {
public:
  const std::string name() const { return parser.prog(); }
  const std::string usage() const { return parser.get_usage(); }  // <ctc> are usage and desc necessary functions ([] double-check)
  const std::string desc() const { return parser.description(); }

  std::vector<std::string> parse_args(const std::vector<std::string> &arguments);
  int run();

  //friend std::ostream& operator<<(std::ostream& os, const Command &cmd); //<ctc> why doesn't this work (a scope issue?)

private:
  virtual void buildParser()
  {
    parser.prog("noop").description("Default (noop) command.");
  }

  virtual int execute(const optparse::Values options) = 0;  // <ctc> make this pure virtual so there is no default command
  // {
  //   // note: throws an exception or returns EXIT_FAILURE if there is an error
  //   std::cout << "shapeworks default command (noop)." << std::endl;
  //   return EXIT_SUCCESS;
  // }
    
protected:
  optparse::OptionParser parser;
  optparse::Values options;
};


}; // Shapeworks


std::ostream& operator<<(std::ostream& os, const Shapeworks::Command &cmd);


#pragma once

#include "Command.h"

namespace Shapeworks {

class Antialias : public Command
{
public:
  static Antialias& getCommand()
  {
    static Antialias instance;

    /*debug stuff*/
    std::cout << "created Antialias command: \n\tname: " << instance.name() << "\n\tdesc: " << instance.desc()
              << "\n\tusage: " << instance.usage() << std::endl;
    

    /*end debug stuff*/

    return instance;
  }

private:
  Antialias() { buildParser(); } // purposely private ctor so only the single instance can be retrieved
  virtual void buildParser();
  virtual int execute(const optparse::Values options);

};


} // Shapeworks

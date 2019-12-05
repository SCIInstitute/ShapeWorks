#pragma once

#include "Command.h"

namespace Shapeworks {

class Antialias : public Command
{
public:
  static Antialias& getCommand() { static Antialias instance; return instance; }

private:
  Antialias() {} // purposely private ctor so only the single instance can be retrieved
  virtual void buildParser();
  virtual int execute(const optparse::Values options);

};


} // Shapeworks

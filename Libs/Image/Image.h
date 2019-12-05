#pragma once

#include "ImageUtils.h"

namespace Shapeworks {

class Image
{
public:

  // todo: break commands up into local/static portions so file load/save isn't always required
  static int Antialias(const std::string &inFilename, const std::string &outFilename, float maxRMSErr, int numIter);


private:
  // todo: save internal data for use between commands

};

} // Shapeworks

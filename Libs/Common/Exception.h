#pragma once

#include <stdexcept>

namespace shapeworks {

class shapeworks_exception : public std::runtime_error
{
public:
  shapeworks_exception(char const* const message) throw();
  shapeworks_exception(std::string const& message) throw();

};

} // shapeworks

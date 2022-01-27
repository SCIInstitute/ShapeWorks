#include "Exception.h"

namespace shapeworks {

shapeworks_exception::shapeworks_exception(char const* const message) throw() : std::runtime_error(message)
{
}

shapeworks_exception::shapeworks_exception(std::string const& message) throw() : std::runtime_error(message)
{
}

} // shapeworks

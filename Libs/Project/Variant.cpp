#include "Variant.h"

#include <sstream>
#include <algorithm>

using namespace shapeworks;

//---------------------------------------------------------------------------
Variant::operator std::string() {
  return str_;
}

//---------------------------------------------------------------------------
Variant::operator bool()
{
  // first try to read as the word 'true' or 'false', otherwise 0 or 1
  std::string str_lower(str_);
  std::transform(str_.begin(), str_.end(), str_lower.begin(),
                 [](unsigned char c) {
    return std::tolower(c);
  });
  bool t = (valid_ && (std::istringstream(str_lower) >> std::boolalpha >> t)) ? t : false;
  if (!t) {
    t = (valid_ && (std::istringstream(str_) >> t)) ? t : false;
  }
  return t;
}

//---------------------------------------------------------------------------
Variant::operator int()
{
  int t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator unsigned int()
{
  unsigned int t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator long()
{
  long t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator unsigned long()
{
  unsigned long t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator float()
{
  float t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator double()
{
  double t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

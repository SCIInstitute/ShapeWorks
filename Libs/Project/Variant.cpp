#include "Variant.h"

#include <algorithm>
#include <iterator>
#include <sstream>

using namespace shapeworks;

//---------------------------------------------------------------------------
Variant::operator std::string() const { return str_; }

//---------------------------------------------------------------------------
Variant::operator bool() const {
  // first try to read as the word 'true' or 'false', otherwise 0 or 1
  std::string str_lower(str_);
  std::transform(str_.begin(), str_.end(), str_lower.begin(), [](unsigned char c) { return std::tolower(c); });
  bool t = (valid_ && (std::istringstream(str_lower) >> std::boolalpha >> t)) ? t : false;
  if (!t) {
    t = (valid_ && (std::istringstream(str_) >> t)) ? t : false;
  }
  return t;
}

//---------------------------------------------------------------------------
Variant::operator int() const {
  int t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator unsigned int() const {
  unsigned int t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator long() const {
  long t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator unsigned long() const {
  unsigned long t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator float() const {
  float t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator double() const {
  double t;
  return (valid_ && (std::istringstream(str_) >> t)) ? t : 0;
}

//---------------------------------------------------------------------------
Variant::operator std::vector<double>() const {
  std::istringstream iss(str_);
  std::vector<double> v((std::istream_iterator<double>(iss)), std::istream_iterator<double>());
  return v;
}

//---------------------------------------------------------------------------
Variant::operator std::vector<int>() const {
  std::istringstream iss(str_);
  std::vector<int> v((std::istream_iterator<int>(iss)), std::istream_iterator<int>());
  return v;
}

//---------------------------------------------------------------------------
Variant::operator std::vector<bool>() const {
  std::istringstream iss(str_);
  std::vector<bool> v((std::istream_iterator<bool>(iss)), std::istream_iterator<bool>());
  return v;
}
